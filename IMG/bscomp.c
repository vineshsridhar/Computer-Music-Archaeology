/*	BSCOMP -- Composition routines for IMG
**	gloabls
**	bsinit()
**	bscomp(sp, frames, lseed, lock, debug, opts, mchn, mkey, kmap, tmpfil)
**	bsidmp(sp, file, cntin, mchn, mkey)
**	bssys(buf)
**	psl 2/90
*/
#define	BS	/* to include BS stuff in libamc.h */
#include <stdlib.h>
#include <stdio.h>
#include	<midi.h>
#include	<stdio.h>
#include "key2name.h"

#define	FALSE	0
#define	TRUE	1

void bssys(char *buf);
void bsidmp(STYLE *sp, char *file, int cntin, int mchn, int mkey);

char	Sysbuf[512];		/* for sys() calls */
int	Debug	= 0;		/* for trace output */
int	Accaok	= 0;		/* is Tmpfile.acc up to date? */
int	Acclok	= 0;		/* is Tmpfile.lead up to date? */
int	Mkccok	= 0;		/* is Tmpfile.cc up to date? */

bsinit()	 		/* Do any initialization ... [?] */
{
}

/* Compose the requested piece of music.
** Return 1 if sp->tempo changed, -1 on error, 0 otherwise.
*/
bscomp(sp, frames, lseed, lock, debug, opts, mchn, mkey, kmap, tmpfil)
STYLE	*sp;			/* current style */
int	frames;			/* length in 30ths of a second */
int	lseed;			/* random seed for lead generation */
int	lock;			/* lock bits */
int	debug;			/* debug bits */
int	opts;			/* optimize bits */
int	mchn, mkey;		/* channel & key for metronome */
char	*kmap[NUMC];		/* drum key maps */
char	*tmpfil;		/* root of file names */
{
	char *cp, buf[256], obuf[128];
	int tempo, clox, unit, i, retval;
	double bars;
	extern char *strcopy();
	extern double atof();

	retval = 0;
	tempo = sp->tempo;
	clox = (2 * tempo * frames) / 30;
DBG(stderr, "tempo=%d frames=%d clox=%d\n", tempo, frames, clox);
	if (!(lock & M_MKCC) && (!Mkccok || !(opts & M_MKCC))) {
	    unit = sp->units;
	    i = clox % unit;
	    if (i) {			/* adjust to a multiple of unit */
		clox += ((i + i) >= unit? unit : 0) - i;
		for (;;) {		/* by changing tempo */
		    tempo = (15 * clox + frames - 1) / frames;
		    if (tempo > 240)
			clox -= unit;
		    else if (tempo < 10)
			clox += unit;
		    else
			break;
		}
		if (tempo != sp->tempo) {
DBG(stderr, "change tempo to %d; ", tempo);
		    sp->tempo = tempo;
		    retval = 1;
		}
	    }
	    bars = clox / (float) sp->barlen;
DBG(stderr,"tempo=%d, %d clox, %d frames, %g bars\n",tempo,clox,frames,bars);
	    sprintf(Sysbuf, "%s -b%g -k%s -s%s >%s.cc",
	     MKCC, bars, key2pc(sp->key), sp->name, tmpfil);
	    bssys(Sysbuf);
	    Mkccok = TRUE;
	    Accaok = FALSE;
	    if (debug & M_MKCC) {
		sprintf(Sysbuf, "cat %s.cc", tmpfil);
		system(Sysbuf);
	    }
	}
	if (!(lock & M_ACCA)
	 && (!Accaok || !(opts & M_ACCA))
	 && (sp->bchan || sp->cchan || sp->dchan)) {
	    sprintf(buf, "%s -c%d=%d -v%d=%d -c%d=%d -v%d=%d -v%d=%d -t%d",
	     ACCA,
	     sp->bchan, BCHAN, sp->bchan, sp->bvelo,
	     sp->cchan, CCHAN, sp->cchan, sp->cvelo,
	     sp->dchan, sp->dvelo, sp->dchan);
	    cp = strcopy(Sysbuf, buf);
	    for (i = 0; i < MAXNDP; i++) {
		if (sp->dpchn[i]) {
		    sprintf(buf, " -c%d=%d",
		     sp->dpart == i? sp->dchan : 0, sp->dpchn[i]);
		    cp = strcopy(cp, buf);
		}
	    }
	    if (debug & M_ACCA)
		sprintf(buf, " -d %s.cc 2>/tmp/acca.2", tmpfil);
	    else
		sprintf(buf, " %s.cc", tmpfil);
	    cp = strcopy(cp, buf);
	    if (sp->dchan && kmap[sp->dchan-1] && kmap[sp->dchan-1][0] > ' ') {
		sprintf(buf, " | %s -c%d %s",
		 KMAP, sp->dchan, kmap[sp->dchan - 1]);
		cp = strcopy(cp, buf);
	    }
	    if ((sp->bchan && sp->btrns != 2)
	     || (sp->cchan && sp->ctrns != 2)) {
		sprintf(buf, " | %s", TRANS);
		cp = strcopy(cp, buf);
		if (sp->bchan && sp->btrns != 2) {
		    sprintf(buf, " %d=%do", sp->bchan, sp->btrns - 2);
		    cp = strcopy(cp, buf);
		}
		if (sp->cchan && sp->ctrns != 2) {
		    sprintf(buf, " %d=%do", sp->cchan, sp->ctrns - 2);
		    cp = strcopy(cp, buf);
		}
	    }
	    sprintf(buf, " >%s.acc", tmpfil);
	    cp = strcopy(cp, buf);
	    bssys(Sysbuf);
	    Accaok = TRUE;
	}
	if (!(lock & M_ACCL)
	 && (!Acclok || !(opts & M_ACCL))
	 && (sp->lchan[0] || sp->lchan[1] || sp->lchan[2])) {
	    bars = clox / (float) sp->barlen;
	    if (sp->lsfix)
		Acclok = TRUE;
	    sprintf(buf,
	     "%s -b%g, -c%d=%d -c%d=%d -c%d=%d -e%d -k%s -p%d -s%d",
	     ACCL, bars,
	     sp->lchan[0], sp->lvelo[0],
	     sp->lchan[1], sp->lvelo[1],
	     sp->lchan[2], sp->lvelo[2],
	     sp->lener, key2pc(sp->key), sp->lpred, lseed);
	    cp = strcopy(Sysbuf, buf);
	    if (debug & M_ACCL)
		cp = strcopy(cp, " -d 2>/tmp/accl.2");
	    sprintf(buf, " %s.cc", tmpfil);
	    cp = strcopy(cp, buf);
	    if ((sp->lchan[0] && sp->ltrns[0] != 2)
	     || (sp->lchan[1] && sp->ltrns[1] != 2)
	     || (sp->lchan[2] && sp->ltrns[2] != 2)) {
		sprintf(buf, " | %s", TRANS);
		cp = strcopy(cp, buf);
		if (sp->lchan[0] && sp->ltrns[0] != 2) {
		    sprintf(buf, " %d=%do", sp->lchan[0], sp->ltrns[0] - 2);
		    cp = strcopy(cp, buf);
		}
		if (sp->lchan[1] && sp->ltrns[1] != 2) {
		    sprintf(buf, " %d=%do", sp->lchan[1], sp->ltrns[1] - 2);
		    cp = strcopy(cp, buf);
		}
		if (sp->lchan[2] && sp->ltrns[2] != 2) {
		    sprintf(buf, " %d=%do", sp->lchan[2], sp->ltrns[2] - 2);
		    cp = strcopy(cp, buf);
		}
	    }
	    sprintf(buf, " >%s.lead", tmpfil);
	    cp = strcopy(cp, buf);
	    bssys(Sysbuf);
	}
	sprintf(obuf, "%s.mpu", tmpfil);
	bsidmp(sp, obuf, sp->cnti * (sp->barlen / 120), mchn, mkey);
	if (!sp->lchan[0] && !sp->lchan[1] && !sp->lchan[2]
	 && !sp->bchan && !sp->cchan && !sp->dchan) {
	    fprintf(stderr, "Nothing to play - all channels OFF\n");
	    return(-1);
	}
	if (!sp->lchan[0] && !sp->lchan[1] && !sp->lchan[2])
	    sprintf(Sysbuf, "%s <%s.acc >>%s", MCLEAN, tmpfil, obuf);
	else if (!sp->bchan && !sp->cchan && !sp->dchan)
	    sprintf(Sysbuf, "%s <%s.lead >>%s", MCLEAN, tmpfil, obuf);
	else
	    sprintf(Sysbuf, "%s %s.lead %s.acc | %s >>%s",
	     MERGE, tmpfil, tmpfil, MCLEAN, obuf);
	bssys(Sysbuf);
	return(retval);
}

/* Write out program changes, vol control, & (optionally) count-in measure(s).
** Program changes & vol control take the first 120 clocks (0.25 bars).
** "cntin" is length of count-in (quarter notes)
*/
void bsidmp(sp, file, cntin, mchn, mkey)
STYLE	*sp;			/* current style */
char	*file;			/* where to write it */
int	cntin;			/* how much count in */
int	mchn, mkey;		/* channel & key for metronome */
{
	char instbuf[256], cntlbuf[256], buf[64], *ip, *cp;
	int j;

	sprintf(buf, "%s -d60 >%s", INST, file);
	ip = strcopy(instbuf, buf);
	sprintf(buf, "%s -d60 >>%s", CNTL, file);
	cp = strcopy(cntlbuf, buf);
	for (j = 0; j < 3; j++) {
	    if (sp->lchan[j] && sp->linst[j]) {
		sprintf(buf, " %d=%d", sp->lchan[j], sp->linst[j]);
		ip = strcopy(ip, buf);
		sprintf(buf, " %d,1=%d %d,2=%d %d,4=%d %d,7=%d",
		 sp->lchan[j], sp->lmctl[j], sp->lchan[j], sp->lbctl[j],
		 sp->lchan[j], sp->lfctl[j], sp->lchan[j], sp->lvctl[j]);
		cp = strcopy(cp, buf);
	    }
	}
	if (sp->bchan && sp->binst) {
	    sprintf(buf, " %d=%d", sp->bchan, sp->binst);
	    ip = strcopy(ip, buf);
	    sprintf(buf, " %d,1=%d %d,2=%d %d,4=%d %d,7=%d",
	     sp->bchan, sp->bmctl, sp->bchan, sp->bbctl,
	     sp->bchan, sp->bfctl, sp->bchan, sp->bvctl);
	    cp = strcopy(cp, buf);
	}
	if (sp->cchan && sp->cinst) {
	    sprintf(buf, " %d=%d", sp->cchan, sp->cinst);
	    ip = strcopy(ip, buf);
	    sprintf(buf, " %d,1=%d %d,2=%d %d,4=%d %d,7=%d",
	     sp->cchan, sp->cmctl, sp->cchan, sp->cbctl,
	     sp->cchan, sp->cfctl, sp->cchan, sp->cvctl);
	    cp = strcopy(cp, buf);
	}
	if (sp->dchan && sp->dinst) {
	    sprintf(buf, " %d=%d", sp->dchan, sp->dinst);
	    ip = strcopy(ip, buf);
	    sprintf(buf, " %d,1=%d %d,2=%d %d,4=%d %d,7=%d",
	     sp->dchan, sp->dmctl, sp->dchan, sp->dbctl,
	     sp->dchan, sp->dfctl, sp->dchan, sp->dvctl);
	    cp = strcopy(cp, buf);
	}
	bssys(instbuf);
	bssys(cntlbuf);
	if (cntin && mchn) {
	    sprintf(instbuf, "echo '0 %x %x 70 78 %x 0' | axtobb | rpt %d >>%s",
	     CH_KEY_ON | (mchn - 1), mkey, mkey, cntin, file);
	    bssys(instbuf);
	}
}

/* Call system() with argument; dump debugging info if necessary.
*/
void bssys(buf)
char	*buf;
{
DBG(stderr, "%s\n", buf);
	system(buf);
}
