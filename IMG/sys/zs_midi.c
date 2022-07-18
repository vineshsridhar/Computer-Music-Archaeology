#ifndef lint
static	char sccsid[] = "@(#)zs_midi.c 1.2 90/06/05 Copyr 1990 Sun Micro";
#endif
/* Copyright (c) 1990 by Sun Microsystems, Inc. */

/*
 *	Asynchronous protocol handler for MIDI
 */
#include "zsm.h"
#define	NZSMLINE	(2*NZSM)
#include <sys/types.h>
#include <sys/param.h>
#include <sys/systm.h>
#include <sys/stream.h>
#include <sys/user.h>
#include <sys/proc.h>
#include <sys/file.h>
#include <sys/uio.h>
#include <sys/buf.h>
#include <sys/kernel.h>
#include <sys/reboot.h>
#include <sys/conf.h>

#include <machine/clock.h>
#include <machine/cpu.h>
#ifdef sun4c
#include <machine/psl.h>
#include <machine/intreg.h>
#endif
#include <sun/consdev.h>
#include <sundev/mbvar.h>
#include <sys/ttycom.h>
#include <sys/tty.h>
#include <sundev/zsreg.h>
#include <sundev/zscom.h>
#include <debug/debug.h>
#include <sundev/midi_ioctl.h>

int midi_tick_delay = 1;

int zsm_debug = 0;
#define	dprintf	if(zsm_debug)printf

/* circular buffer */
struct circbuf {
	u_char	*data;
	int	in;
	int	out;
	int	size;
};
#define	COUNT(b)	(b.in >= b.out ? b.in-b.out : b.size+b.in-b.out)
#define	HIWAT(b)	(3 * b.size / 4)
#define	LOWAT(b)	(b.size / 4)
#define	INC(b, io)	(io = ++io >= b.size ? 0 : io)
#define	DEC(b, io)	(io = --io < 0 ? b.size-1 : io)

struct zsmidi {
	struct zscom *zm_common;
	int	zm_flags;
#define	ISOPEN	0x1
#define	R_NBIO	0x2
#define	W_NBIO	0x4
#define	TXBUSY	0x8
#define	CLOSING	0x10
	int	zm_pause;
	u_char	zm_rr0;
	u_char	zm_needsoft;
	struct	midi_ioctl zm_ioc;
	/* output stuff */
	struct	circbuf zm_wb;
	int	zm_ocnt;
	int	zm_odelay;
	int	zm_oerror;
	int	zm_osleep;
	struct proc *zm_wsel;
	/* input stuff */
	struct	circbuf zm_rb;
	int	zm_laststamp;
	int	zm_iactive;
	int	zm_istart;
	int	zm_icnt;
	int	zm_iptr;
	int	zm_ilost;
	int	zm_isleep;
	struct proc *zm_rsel;
} zsmidi[NZSMLINE];

int zsm_delaydone();
static	u_int timestamp();

#define	PCLK	(19660800/4)	/* basic clock rate for UARTs */
#define	ZSPEED(n)	ZSTimeConst(PCLK, n)

/*
 * The midi zs protocol
 */
static int	zsm_attach(/*struct zscom *zs*/);
static int	zsm_txint(/*struct zscom *zs*/);
       int	zsm_xsint(/*struct zscom *zs*/);
static int	zsm_rxint(/*struct zscom *zs*/);
static int	zsm_srint(/*struct zscom *zs*/);
static int	zsm_softint(/*struct zscom *zs*/);

struct zsops zsops_midi = {
	zsm_attach,
	zsm_txint,
	zsm_xsint,
	zsm_rxint,
	zsm_srint,
	zsm_softint,
};

static int
zsm_attach(zs)
	register struct zscom *zs;
{
	struct zsmidi *zm = &zsmidi[zs->zs_unit];
#ifdef sun4c
	extern int (*int_vector[])(), spurious(), hi_timerintr();
	long dummy;
	int s;

	if (int_vector[14] != spurious && int_vector[14] != hi_timerintr) {
		printf("can't use midi: level 14 used by something else\n");
		return;
	}
	s = splx(pritospl(7));
	int_vector[14] = hi_timerintr;
	dummy = COUNTER->limit14;	/* clear any pending intr */
	COUNTER->limit14 = 0;		/* free run */
	splx(s);
#endif
	zm->zm_common = zs;
}

/*ARGSUSED*/
int
zsm_open(dev, flags)
	dev_t dev;
{
	register int unit;
	register struct zsmidi *zm;
	struct zscom *zs;
	int s;

	unit = minor(dev);
	if (unit >= NZSMLINE)
		return (ENXIO);	/* device not configured in */
	zm = &zsmidi[unit];
	if ((zs = zm->zm_common) == NULL)
		return (ENXIO);	/* device not found by autoconfig */

	s = splzs();
	zs->zs_priv = (caddr_t)zm;
	zsopinit(zs, &zsops_midi);
	(void) splx(s);

	if (!(zm->zm_flags & ISOPEN)) {
		/* set defaults */
#ifndef sun4c
		zm->zm_ioc.tick =  2 * (1000000/hz);
		zm->zm_ioc.speed = 1200;
		zm->zm_ioc.rdsize = 4096;
		zm->zm_ioc.wrsize = 4096;
#else
		zm->zm_ioc.tick = 1000;
		zm->zm_ioc.speed = 31250;
		zm->zm_ioc.rdsize = 8192;
		zm->zm_ioc.wrsize = 8192;
#endif
		zsm_init(zm);
	}
	if ((flags & FREAD) && (flags & (FNBIO|FNDELAY)))
		zm->zm_flags |= R_NBIO;
	if ((flags & FWRITE) && (flags & (FNBIO|FNDELAY)))
		zm->zm_flags |= W_NBIO;
	zm->zm_flags |= ISOPEN;
	return (0);
}

/*ARGSUSED*/
int
zsm_close(dev)
	dev_t dev;
{
	register struct zsmidi *zm = &zsmidi[minor(dev)];
	register struct zscom *zs = zm->zm_common;
	u_char *rd, *wd;
	int rs, ws;

	zs = zm->zm_common;
dprintf("zsm_close: busy %d exit %d\n", zm->zm_flags & TXBUSY,
					u.u_procp->p_flag & SWEXIT);
	while ((zm->zm_flags & TXBUSY) && (u.u_procp->p_flag & SWEXIT) == 0)
		if (sleep((caddr_t)&lbolt, PCATCH|(PZERO+1)))
			break;		/* interrupted */
	zsm_notes_off(zm);
#ifndef sun4c
	untimeout(zsm_delaydone, (caddr_t)zm);
#endif
	(void) splzs();
	ZBIC(1, ZSWR1_RIE);
#ifdef sun4c
	hi_untimeout(zsm_delaydone, (caddr_t)zm);
#endif
	rd = zm->zm_rb.data;
	rs = zm->zm_rb.size;
	wd = zm->zm_wb.data;
	ws = zm->zm_wb.size;
	bzero((caddr_t)zm, sizeof *zm);
	zm->zm_common = zs;
	(void) spl0();
dprintf("zsm_close done ocnt %d\n", zm->zm_ocnt);
	if (rd)
		kmem_free(rd, rs);
	if (wd)
		kmem_free(wd, ws);
	
}

zsm_notes_off(zm)
	register struct zsmidi *zm;
{
	static struct uio uio;
	static struct iovec iov;
#define	MIDIOFF_LEN	3
#define	NCHAN		6
	static char buf[MIDIOFF_LEN*NCHAN+MIDI_HDRSIZE];
	register char *cp;
	int chan;
	int err;
	int note;

	zm->zm_flags |= CLOSING;
	while (zm->zm_flags & TXBUSY)
		sleep((caddr_t)&lbolt, PZERO-1);
	/* now we are quiesced - clear output buffer */
dprintf("flushing %d\n", COUNT(zm->zm_wb));
	zm->zm_pause &= ~MIDI_WRITE_PAUSE;
	zm->zm_wb.in = zm->zm_wb.out = 0;
	for (note = 24; note < 100; note++) {
	    uio.uio_iov = &iov;
	    uio.uio_iovcnt = 1;
	    uio.uio_offset = 0;
	    uio.uio_segflg = UIO_SYSSPACE;
	    uio.uio_fmode = FWRITE;
	    uio.uio_resid = sizeof buf;
	    iov.iov_len = sizeof buf;
	    iov.iov_base = buf;
	    cp = buf;	
	    *cp++ = 0;	/* delay 1*/
	    *cp++ = 0;	/* delay 2*/
	    *cp++ = MIDIOFF_LEN*NCHAN;	/* len */
	    for (chan = 0; chan < NCHAN; chan++) {
		    *cp++ = 0x90 | chan;
		    *cp++ = note;
		    *cp++ = 0;
	    }
	    zm->zm_flags &= ~W_NBIO;	/* block for this stuff */
		zm->zm_pause = 0;
	    err = zsm_write(zm - zsmidi, &uio);
	}
dprintf("close write err %d, flags %x\n", err, zm->zm_flags);
	while (zm->zm_flags & TXBUSY)
		sleep((caddr_t)&lbolt, PZERO-1);
}

zsm_read(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct zsmidi *zm = &zsmidi[minor(dev)];
	int s;
	int in, out, size;
	int first = 1;
	register int delay;

	zm->zm_pause |= MIDI_READING;
	while (uio->uio_resid > 0) {
		s = splzs();
		/* finish off previous record */
		delay = (timestamp() - zm->zm_laststamp) / zm->zm_ioc.tick;
		if (delay >= midi_tick_delay && zm->zm_iactive) {
			zm->zm_rb.data[zm->zm_istart] = zm->zm_icnt;
			in = zm->zm_istart + zm->zm_icnt + 1;
			if (in >= zm->zm_rb.size)
				in -= zm->zm_rb.size;
			zm->zm_rb.in = in;
			zm->zm_iactive = 0;
			zm->zm_icnt = 0;
		}
		splx(s);

		if (zm->zm_rb.in == zm->zm_rb.out) {
			if (!first)
				return (0);
			if (zm->zm_flags & R_NBIO)
				return (EWOULDBLOCK);
			s = spl3();
			zm->zm_isleep = 1;
			while (zm->zm_isleep && zm->zm_rb.in == zm->zm_rb.out)
				sleep((caddr_t)&zm->zm_isleep, PZERO+1);
			zm->zm_isleep = 0;
			splx(s);
			continue;
		}

		s = splzs();
		if (zm->zm_iactive) {
			splx(s);
			continue;
		}
		first = 0;
		in = zm->zm_rb.in;
		out = zm->zm_rb.out;
		if (in >= out)
			size = in - out; 
		else
			size = zm->zm_rb.size - out;
		splx(s);
		size = MIN(size, uio->uio_resid);
dprintf("zsmrd: %d bytes\n", size);
		uiomove(zm->zm_rb.data + out, size, UIO_READ, uio);
		out += size;
		if (out >= zm->zm_rb.size)
			out = 0;
		zm->zm_rb.out = out;
	}
	return (0);
}

zsm_write(dev, uio)
	dev_t dev;
	struct uio *uio;
{
	register struct zsmidi *zm = &zsmidi[minor(dev)];
	int in, out, size, allowed;
	int s;

	zm->zm_pause |= MIDI_WRITING;
	s = spl3();		/* zs softint level */
	if (zm->zm_pause & MIDI_WRITE_PAUSE) {
	    do {
		    zm->zm_osleep = 1;
		    if (zm->zm_flags & TXBUSY) {
			if (zm->zm_flags & W_NBIO) {
				splx(s);
				return (EWOULDBLOCK);
			}
			sleep((caddr_t)&zm->zm_osleep, PZERO+1);
		    }
	    } while (zm->zm_flags & TXBUSY);
	}
	zm->zm_osleep = 0;
	(void) splx(s);

	while (uio->uio_resid > 0) {
		allowed = zm->zm_wb.size - COUNT(zm->zm_wb) - 1;
		if (allowed <= 0) {
			if (zm->zm_flags & W_NBIO)
				return (EWOULDBLOCK);
			s = spl3();
			zm->zm_osleep = 1;
			while (zm->zm_osleep &&
		  	    COUNT(zm->zm_wb) > HIWAT(zm->zm_wb))
				sleep((caddr_t)&zm->zm_osleep, PZERO+1);
			zm->zm_osleep = 0;
			splx(s);
			continue;
		}
		in = zm->zm_wb.in;
		out = zm->zm_wb.out;
		if (in >= out)
			size = zm->zm_wb.size - in;
		else
			size = out - in;
		size = MIN(allowed, size);
		size = MIN(uio->uio_resid, size);
dprintf("zsmwr: %d bytes\n", size);
		uiomove(zm->zm_wb.data + in, size, UIO_WRITE, uio);
		zm->zm_wb.in += size;
		if (zm->zm_wb.in >= zm->zm_wb.size)
			zm->zm_wb.in = 0;
		s = splzs();
		if ((zm->zm_flags & TXBUSY) == 0 &&
		    (zm->zm_pause & MIDI_WRITE_PAUSE) == 0)
			zsm_start(zm);
		splx(s);
	}
	return (0);
}

zsm_select(dev, rw)
	dev_t dev;
	int rw;
{
	register struct zsmidi *zm = &zsmidi[minor(dev)];
	int ok = 0;
	int s;

	s = spl3();

	switch (rw) {
	case FREAD:
		zm->zm_isleep = 1;
		if (zm->zm_rb.in != zm->zm_rb.out) {
			zm->zm_isleep = 0;
			ok = 1;
		} else
			zm->zm_rsel = u.u_procp;
		break;

	case FWRITE:
		zm->zm_osleep = 1;
		if (COUNT(zm->zm_wb) <= HIWAT(zm->zm_wb)) {
			zm->zm_osleep = 0;
			ok = 1;
		} else
			zm->zm_wsel = u.u_procp;
		break;

	default:
		ok = 1;
		break;
	}
	splx(s);
	return (ok);
}


zsm_ioctl(dev, cmd, arg, mode)
	int cmd;
	dev_t dev;
	caddr_t arg;
{
	register struct zsmidi *zm = &zsmidi[minor(dev)];
	struct midi_ioctl *iocp = (struct midi_ioctl *)arg;
	int s;

	switch (cmd) {

	case MIDI_SET:
		if (iocp->tick < 1 || iocp->speed < 50 ||
		    iocp->rdsize < 16 || iocp->wrsize < 16 ||
		    iocp->rdsize > 65536 || iocp->wrsize > 65536)
			return (EINVAL);
		zm->zm_ioc = *iocp;
		zsm_init(zm);
		break;

	case MIDI_GET:
		*iocp = zm->zm_ioc;
		break;

	case MIDI_FLUSH:
		s = splzs();
		zm->zm_wb.in = zm->zm_wb.out = 0;
		zm->zm_ocnt = 0;
		zm->zm_odelay = 0;
		zm->zm_osleep = 0;
		zm->zm_rb.in = zm->zm_rb.out = 0;
		zm->zm_iactive = 0;
		zm->zm_istart = 0;
		zm->zm_icnt = 0;
		zm->zm_iptr = 0;
		zm->zm_isleep = 0;
		zm->zm_laststamp = timestamp();
		(void) splx(s);
#ifdef sun4c
		hi_untimeout(zsm_delaydone, (caddr_t)zm);
#else
		untimeout(zsm_delaydone, (caddr_t)zm);
#endif
		wakeup((caddr_t)&zm->zm_osleep);
		if (zm->zm_wsel) {
			selwakeup(zm->zm_wsel, 0);
			zm->zm_wsel = 0;
		}
		wakeup((caddr_t)&zm->zm_isleep);
		if (zm->zm_rsel) {
			selwakeup(zm->zm_rsel, 0);
			zm->zm_rsel = 0;
		}
		break;

	case MIDI_DRAIN:
		s = spl3();		/* zs softint level */
		do {
			zm->zm_osleep = 1;
			if (zm->zm_flags & TXBUSY)
				sleep((caddr_t)&zm->zm_osleep, PZERO+1);
		} while (zm->zm_flags & TXBUSY);
		zm->zm_osleep = 0;
		(void) splx(s);
		break;

	case MIDI_PAUSE:
		s = splzs();
		if ((zm->zm_pause & MIDI_READ_PAUSE) &&
		    !(*(int *)arg & MIDI_READ_PAUSE)) {
			/* get rid of any leftover from before */
			zm->zm_rb.in = zm->zm_rb.out = 0;
			zm->zm_iactive = 0;
			zm->zm_icnt = 0;
			zm->zm_laststamp = timestamp();
		}
		zm->zm_pause = *(int *)arg;
		if ((zm->zm_pause & MIDI_WRITING) &&
		    !(zm->zm_pause & MIDI_WRITE_PAUSE) &&
		    !(zm->zm_flags & TXBUSY))
			zsm_start(zm);
		splx(s);
		break;

	default:
		return (EINVAL);
	}
	return (0);
}

/*
 */
zsm_init(zm)
	register struct zsmidi *zm;
{
	register struct zscom *zs = zm->zm_common;
	register int wr1, wr3, wr4, wr5, speed;
	int s;
	char c;

	/* allocate circular buffers */
	if (zm->zm_rb.data)
		kmem_free((char *)zm->zm_rb.data, zm->zm_rb.size);
	if (zm->zm_wb.data)
		kmem_free((char *)zm->zm_wb.data, zm->zm_wb.size);
	zm->zm_rb.data = (u_char *)kmem_alloc(zm->zm_ioc.rdsize);
	zm->zm_wb.data = (u_char *)kmem_alloc(zm->zm_ioc.wrsize);
	if (zm->zm_rb.data == NULL || zm->zm_wb.data == NULL)
		panic("zsm_init");
	zm->zm_rb.size = zm->zm_ioc.rdsize;
	zm->zm_wb.size = zm->zm_ioc.wrsize;
	zm->zm_rb.in = zm->zm_rb.out = 0;
	zm->zm_wb.in = zm->zm_wb.out = 0;
dprintf("zsm_init\n");

	s = splzs();
	wr1 = ZSWR1_TIE | ZSWR1_RIE;
	wr3 = ZSWR3_RX_ENABLE | ZSWR3_RX_8;
	wr4 = ZSWR4_X16_CLK | ZSWR4_1_STOP;
	wr5 = /*ZSWR5_RTS |*/ ZSWR5_DTR | ZSWR5_TX_ENABLE | ZSWR5_TX_8;

	ZWRITE(3, 0);	/* disable receiver while setting parameters */
	zs->zs_addr->zscc_control = ZSWR0_RESET_STATUS;
	DELAY(2);
	zs->zs_addr->zscc_control = ZSWR0_RESET_ERRORS;
	DELAY(2);
	c = zs->zs_addr->zscc_data; /* swallow junk */
	DELAY(2);
	c = zs->zs_addr->zscc_data; /* swallow junk */
	DELAY(2);
	c = zs->zs_addr->zscc_data; /* swallow junk */
	DELAY(2);
	ZWRITE(1, wr1); DELAY(2);
	ZWRITE(4, wr4); DELAY(2);
	ZWRITE(3, wr3); DELAY(2);
	ZWRITE(5, wr5); DELAY(2);
	speed = ZSPEED(zm->zm_ioc.speed);
	ZWRITE(11, ZSWR11_TXCLK_BAUD + ZSWR11_RXCLK_BAUD); DELAY(2);
	ZWRITE(14, ZSWR14_BAUD_FROM_PCLK); DELAY(2);
	ZWRITE(12, speed); DELAY(2);
	ZWRITE(13, speed >> 8); DELAY(2);
	ZWRITE(14, ZSWR14_BAUD_ENA + ZSWR14_BAUD_FROM_PCLK); DELAY(2);

	zm->zm_laststamp = timestamp();
	(void) splx(s);
}

/*
 * Start output 
 */
zsm_start(zm)
	register struct zsmidi *zm;
{
	register struct zscom *zs = zm->zm_common;
	register int delay;
	register int cntbyte, ocnt;

top:
	if (COUNT(zm->zm_wb) < MIDI_HDRSIZE)
		goto quiet;
	cntbyte = zm->zm_wb.out + 2;
	if (cntbyte > zm->zm_wb.size)
		cntbyte -= zm->zm_wb.size;
	ocnt = zm->zm_wb.data[cntbyte];
	if (COUNT(zm->zm_wb) < MIDI_HDRSIZE + ocnt) 
		goto quiet;

	zm->zm_flags |= TXBUSY;
	delay = zm->zm_wb.data[zm->zm_wb.out];
	INC(zm->zm_wb, zm->zm_wb.out);
	delay = (delay << 8) + zm->zm_wb.data[zm->zm_wb.out];
	INC(zm->zm_wb, zm->zm_wb.out);
	zm->zm_ocnt = zm->zm_wb.data[zm->zm_wb.out];
	INC(zm->zm_wb, zm->zm_wb.out);
	if (delay) {
		if ((zm->zm_flags & CLOSING) ||
		    (zm->zm_pause & MIDI_WRITE_PAUSE)) {
			zm->zm_ocnt = 0;
			DEC(zm->zm_wb, zm->zm_wb.out);
			DEC(zm->zm_wb, zm->zm_wb.out);
			DEC(zm->zm_wb, zm->zm_wb.out);
			goto quiet;
		}
		zsm_txdelay(zm, delay);
	} else if (zm->zm_ocnt) {
		ZWRITEDATA(zm->zm_wb.data[zm->zm_wb.out]);
		INC(zm->zm_wb, zm->zm_wb.out);
		zm->zm_ocnt--;
	} else
		goto top;
	return;
quiet:
	zm->zm_flags &= ~TXBUSY;
	ZSSETSOFT(zs);	/* triggers wakeup */
}

int delay_toooften = 0;

zsm_txdelay(zm, delay)
	register struct zsmidi *zm;
{
	register struct zscom *zs = zm->zm_common;
	unsigned int usec;
	struct timeval tv;

	usec = delay * zm->zm_ioc.tick;
	if (usec > 10000000)
		usec = 1000000;	/* sanity */
#ifdef sun4c
	if (usec >= 1000000) {
		tv.tv_sec = usec / 1000000;
		tv.tv_usec = usec - tv.tv_sec * 1000000;
	} else {
		tv.tv_sec = 0;
		tv.tv_usec = usec;
	}
	if (!zm->zm_odelay) {
		zm->zm_odelay = 1;
		hi_timeout(zsm_delaydone, zm, tv);
	} else
		delay_toooften++;
#else  !sun4c
	/* can't call timeout directly since we're at splzs, do it in softint */
	zm->zm_odelay =  usec / (1000000/hz);
	ZSSETSOFT(zs);
#endif sun4c
}

/*
 * Since this is called at hardware level 14, and normal zs
 * driver level is 12, we can only touch the data register and not
 * the control register of the SCC chip.   Otherwise, we might interrupt
 * a ZWRITE sequence and cause it to misbehave. Fortunately, the data 
 * register is enough.
 */
int delaydone_count = 0;

zsm_delaydone(zm)
	register struct zsmidi *zm;
{
	register struct zscom *zs = zm->zm_common;
#ifndef sun4c
	int s;
#endif

#ifndef sun4c
dprintf("delaydone\n");
	s = splzs();
#endif

#ifdef sun4c
	zm->zm_odelay = 0;
#endif
	delaydone_count++;
	if (zm->zm_ocnt == 0) 
		zsm_start(zm);
	else {
		ZWRITEDATA(zm->zm_wb.data[zm->zm_wb.out]);
		INC(zm->zm_wb, zm->zm_wb.out);
		zm->zm_ocnt--;
	}

#ifndef sun4c
	splx(s);
#endif
}

/*
 * Transmitter interrupt service routine.
 */
static int
zsm_txint(zs)
	register struct zscom *zs;
{
	register struct zsmidi *zm = (struct zsmidi *)zs->zs_priv;
	register struct zscc_device *zsaddr = zs->zs_addr;

	if (zsaddr->zscc_control & ZSRR0_TX_READY) {
		if (COUNT(zm->zm_wb) < zm->zm_ocnt) {
			zm->zm_oerror++;
			zm->zm_ocnt = 0;
			zm->zm_wb.in = zm->zm_wb.out = 0;
		}
		if (zm->zm_ocnt) {
			ZWRITEDATA(zm->zm_wb.data[zm->zm_wb.out]);
			INC(zm->zm_wb, zm->zm_wb.out);
			zm->zm_ocnt--;
		} else 
			zsm_start(zm);
		if ((zm->zm_flags & TXBUSY) == 0)
			ZSSETSOFT(zs);
		if (zm->zm_osleep && COUNT(zm->zm_wb) <= HIWAT(zm->zm_wb))
			ZSSETSOFT(zs);
	}
	zsaddr->zscc_control = ZSWR0_RESET_TXINT;
	DELAY(2);
}

/*
 * External/Status interrupt.
 */
int
zsm_xsint(zs)
	register struct zscom *zs;
{
	register struct zsmidi *zm = (struct zsmidi *)zs->zs_priv;
	register struct zscc_device *zsaddr = zs->zs_addr;
	register u_char s0, x0, c;

	s0 = zsaddr->zscc_control;
	DELAY(2);
	x0 = s0 ^ zm->zm_rr0;
	zm->zm_rr0 = s0;
	zsaddr->zscc_control = ZSWR0_RESET_STATUS;
	ZSSETSOFT(zs);
}

/*
 * Receiver interrupt.  Try to put the character into the circular buffer
 */
static int
zsm_rxint(zs)
	struct zscom *zs;
{
	register struct zsmidi *zm = (struct zsmidi *)zs->zs_priv;
	register u_char c;
	register u_int delay;
	register int in;
	u_int now;

	c = zs->zs_addr->zscc_data;
	now = timestamp();
	delay = now - zm->zm_laststamp;
	zm->zm_laststamp = now;
	delay /= zm->zm_ioc.tick;
	if (zm->zm_rb.size - (COUNT(zm->zm_rb)+zm->zm_icnt) < MIDI_HDRSIZE+1) {
		zm->zm_ilost++;
		ZSSETSOFT(zs);
		return;
	}
	if (delay >= midi_tick_delay || !zm->zm_iactive ||
		zm->zm_icnt == MIDI_MAXREC) {
		/* finish off previous record */
		if (zm->zm_iactive) {
			zm->zm_rb.data[zm->zm_istart] = zm->zm_icnt;
			in = zm->zm_istart + zm->zm_icnt + 1;
			if (in >= zm->zm_rb.size)
				in -= zm->zm_rb.size;
			zm->zm_rb.in = in;
			zm->zm_iactive = 0;
			zm->zm_icnt = 0;
		}
		if (!(zm->zm_pause & MIDI_READ_PAUSE)) {
			/* start new record */
			zm->zm_rb.data[zm->zm_rb.in] = delay >> 8;
			INC(zm->zm_rb, zm->zm_rb.in);
			zm->zm_rb.data[zm->zm_rb.in] = delay;
			INC(zm->zm_rb, zm->zm_rb.in);
			zm->zm_istart = zm->zm_rb.in;
			zm->zm_iptr = zm->zm_istart;
			INC(zm->zm_rb, zm->zm_iptr);
			zm->zm_icnt = 0;
			zm->zm_iactive = 1;
		}
	}
	if (zm->zm_iactive) {
		/* now stuff the character in the buffer */
		zm->zm_rb.data[zm->zm_iptr] = c;
		INC(zm->zm_rb, zm->zm_iptr);
		zm->zm_icnt++;
	}
	if (zm->zm_isleep)
		ZSSETSOFT(zs);

}

/*
 * Special receive condition interrupt handler.
 */
static int
zsm_srint(zs)
	register struct zscom *zs;
{
	register struct zsmidi *zm = (struct zsmidi *)zs->zs_priv;
	register struct zscc_device *zsaddr = zs->zs_addr;
	register short s1;
	register u_char c;

	s1 = ZREAD(1);
	DELAY(2);
	c = zsaddr->zscc_data;	/* swallow bad char */
	DELAY(2);
	zsaddr->zscc_control = ZSWR0_RESET_ERRORS;
	DELAY(2);
	if (s1 & ZSRR1_DO) {
		ZSSETSOFT(zs);
	}
}

/*
 * Handle a software interrupt 
 */
static int
zsm_softint(zs)
	register struct zscom *zs;
{
	register struct zsmidi *zm = (struct zsmidi *)zs->zs_priv;

#ifndef sun4c
	if (zm->zm_odelay) {
dprintf("delay %d\n", zm->zm_odelay);
		if (zm->zm_odelay > 10*hz)
			zm->zm_odelay = hz;
		timeout(zsm_delaydone, (caddr_t)zm, zm->zm_odelay);
		zm->zm_odelay = 0;
	}
#endif
	if (zm->zm_isleep && zm->zm_rb.in != zm->zm_rb.out) {
		if (zm->zm_rsel) {
			selwakeup(zm->zm_rsel, 0);
			zm->zm_rsel = 0;
		}
		zm->zm_isleep = 0;
		wakeup((caddr_t)&zm->zm_isleep);
	}
	if (zm->zm_osleep && (COUNT(zm->zm_wb) <= HIWAT(zm->zm_wb) ||
	    (zm->zm_flags & TXBUSY) == 0)) {
		if (zm->zm_wsel) {
			selwakeup(zm->zm_wsel, 0);
			zm->zm_wsel = 0;
		}
		zm->zm_osleep = 0;
		wakeup((caddr_t)&zm->zm_osleep);
	}
	if (zm->zm_ilost) {
		static int sec = -1;

		if ((zm->zm_pause & MIDI_READING) &&
		    !(zm->zm_pause & MIDI_READ_PAUSE)) {
			if (sec != time.tv_sec) {
				sec = time.tv_sec;
				printf("midi%d: lost %d chars\n", zs->zs_unit, zm->zm_ilost);
				zm->zm_ilost = 0;
			}
		} else
			zm->zm_ilost = 0;
	}
	if (zm->zm_oerror) {
		printf("midi%d: output data error\n", zs->zs_unit);
		zm->zm_oerror = 0;
	}
	return (0);
}

/*
 * return current time stamp in microseconds
 */
int funnystamp = 0;

static u_int
timestamp()
{
	register u_int stamp;
	static u_int last;

	stamp = time.tv_sec * 1000000 + time.tv_usec;
#ifdef sun4c
	stamp += (COUNTER->counter10 & CTR_USEC_MASK) >> CTR_USEC_SHIFT;
#endif
	if ((int)(stamp - last) <= 0) {
		stamp += 1000000/hz;
		funnystamp++;
	}
	last = stamp;
	return (stamp);
}

#ifdef sun4c
struct	hicall {
	struct timeval	c_time;	/* absolute time */
	caddr_t	c_arg;		/* argument to routine */
	int	(*c_func)();	/* routine */
	struct	hicall *c_next;
};

#define	NHICALL	20
static struct hicall hicall[NHICALL];
struct hicall *hifree, hitodo;

int timeout_already = 0;

hi_timeout(func, arg, tv)
	int (*func)();
	caddr_t arg;
	struct timeval tv;
{
	struct timeval now;
	int s;
	register struct hicall *p1, *p2, *pnew;
	static int first = 1;

	s = splx(pritospl(7));
	tv.tv_sec += time.tv_sec;
	tv.tv_usec += time.tv_usec;
	tv.tv_usec += (COUNTER->counter10 & CTR_USEC_MASK) >> CTR_USEC_SHIFT;
	while (tv.tv_usec >= 1000000) {
		tv.tv_usec -= 1000000;
		tv.tv_sec++;
	}
	if (first) {
		int i;

		first = 0;
		for (i = 1; i < NHICALL; i++)
			hicall[i-1].c_next = &hicall[i];
		hifree = &hicall[0];
	}
	pnew = hifree;
	if (pnew == NULL)
		panic("hi_timeout");
	hifree = pnew->c_next;
	pnew->c_arg = arg;
	pnew->c_func = func;
	pnew->c_time = tv;
	p1 = &hitodo;
	while (p2 = p1->c_next) {
		if (p2->c_time.tv_sec > tv.tv_sec)
			break;
		if (p2->c_time.tv_sec == tv.tv_sec &&
		    p2->c_time.tv_usec >= tv.tv_usec)
			break;
		p1 = p2;
	}
	p1->c_next = pnew;
	pnew->c_next = p2;
	if (p1 == &hitodo)
		hi_timerintr();
	else
		timeout_already++;
	splx(s);
}

int hi_intrcount = 0;

hi_timerintr()
{
	register long now_sec, now_usec, usec;
	register struct hicall *p1;
	register caddr_t arg;
	register int (*func)();

	hi_intrcount++;

	/* read register to clear interrupt */
	now_usec = COUNTER->limit14;

	for (;;) {
		now_sec = time.tv_sec;
		now_usec = time.tv_usec;
		now_usec +=
			(COUNTER->counter10 & CTR_USEC_MASK) >> CTR_USEC_SHIFT;
		if (now_usec > 1000000) {
			now_sec++;
			now_usec -= 1000000;
		}

		p1 = hitodo.c_next;
		if (p1 == 0 || p1->c_time.tv_sec > now_sec ||
		    (p1->c_time.tv_sec == now_sec &&
		     p1->c_time.tv_usec > now_usec))
			break;

		arg = p1->c_arg;
		func = p1->c_func;
		hitodo.c_next = p1->c_next;
		p1->c_next = hifree;
		hifree = p1;
		(*func)(arg);
	}
	if (p1 == 0) {
		COUNTER->limit14 = 0;
		return;
	}
	/* set or reset the limit register based on the first entry */
	usec = 1000000 * (p1->c_time.tv_sec - now_sec);
	usec += p1->c_time.tv_usec - now_usec;
	*INTREG |= IR_ENA_CLK14;
	if (usec >= 1000000)
		COUNTER->limit14 = 1000000 << CTR_USEC_SHIFT;
	else
		COUNTER->limit14 = usec << CTR_USEC_SHIFT;
}

hi_untimeout(func, arg)
	int (*func)();
	caddr_t arg;
{
	register struct hicall *p1, *p2;
	register int s;

	s = splx(pritospl(7));
	for (p1 = &hitodo; p2 = p1->c_next; p1 = p2) {
		if (p2->c_func == func && p2->c_arg == arg) {
			p1->c_next = p2->c_next;
			p2->c_next = hifree;
			hifree = p2;
			break;
		}
	}
	splx(s);
}
#endif sun4c
