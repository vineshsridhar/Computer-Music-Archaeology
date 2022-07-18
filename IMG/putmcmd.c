/*
**	PUTMCMD -- routines to write an MPU command from absolute-time MIDI
**		
**	long putmcmd(ofp, mp)	FILE *ofp; MCMD *mp;
**		Write command *mp to ofp (single stream)
**		Return mp_>when
**	iputmcmds(sid, ofp, when)	FILE *ofp; long when;
**		Set file pointer and initial time for stream sid
**	long putmcmds(sid, mp)		FILE *ofp; MCMD *mp;
**		Write command *mp to stream sid
**		Return mp_>when
**	
**	Common usage:
**		now = 0L;
**		putmcmd(0, 0);			** sets output "now" to 0 **
**		while (mp = getmcmd(stdin, now)) {
**			now = mp->when;
**			...
**			putmcmd(stdout, mp);
**		}
**		Rt_tcwme.when = now;		** defined here **
**		putmcmd(stdout, &Rt_tcwme);
**	psl 2/87
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>


u_char	Rt_tcwmebuf[1] = {		/* MPU_TCWME command buffer */
	MPU_TCWME,
};
MCMD	Rt_tcwme	= {		/* MPU_TCWME command */
	0, 1, Rt_tcwmebuf,
};
u_char	Mpu_nopbuf[1] = {		/* MPU_NO_OP command buffer */
	MPU_NO_OP,
};
MCMD	Mpu_nop	= {			/* MPU_NO_OP command */
	0, 1, Mpu_nopbuf,
};
long	Onow;				/* output clock (single stream) */
long	Sonow[MAXSID];			/* output clocks (mult stream) */
FILE	*Sofp[MAXSID];			/* output FILE *s (mult streams) */

long
putmcmd(ofp, mp)
FILE	*ofp;
MCMD	*mp;
{
	register int dt, i;
	static int flush=2;		/* sau */

	if (flush==2)
		flush = (getenv("FLUSH") != 0);

	if (ofp == (FILE *) 0)
	    return(Onow = (mp == (MCMD *) 0)? 0L : mp->when);
	dt = mp->when - Onow;
	if (dt < 0)
	    return(mp->when);
	while (dt >= MPU_CLOCK_PERIOD) {
	    putc(MPU_TCIP, ofp);
	    dt -= MPU_CLOCK_PERIOD;
	}
	putc(dt, ofp);
	for (i = 0; i < mp->len; i++)
	    putc(mp->cmd[i], ofp);
	if (flush)
		 fflush(ofp);
	return(Onow = mp->when);
}

iputmcmds(sid, ofp, when)
FILE	*ofp;
long	when;
{
	if (0 <= sid && sid < MAXSID) {
	    Sofp[sid] = ofp;
	    Sonow[sid] = when;
	}
}

long
putmcmds(sid, mp)
MCMD	*mp;
{
	long onow, ret;

	if (0 <= sid && sid < MAXSID) {
	    onow = Onow;		/* save single stream value */
	    Onow = Sonow[sid];
	    ret = putmcmd(Sofp[sid], mp);
	    Sonow[sid] = Onow;
	    Onow = onow;
	    return(ret);
	}
	return(0L);
}