/*
**	JUST -- Justify (clean-up) rhythm
**	psl 12/85, threshold added 12/88
*/

#include <stdlib.h>
#include <stdio.h>
#include <midi.h>
#include <math.h>
#include "sopen.h"

#define	MAXCHAN	MIDI_MAX_CHANS
#define	MAXKEY	128

void just(FILE *ifp, FILE *ofp);

int	Dflg	= 1;			/* what to do with durations */
int	Dt[MAXCHAN][MAXKEY];		/* offsets used for starts of notes */
long	Quant	= (MPU_CLOCK_PERIOD / 24);/* clocks in justification quantum */
int	Thresh	= 1;			/* ignore time-tags <= this */

syntax(prog)
char	*prog;
{
	fprintf(stderr, "Usage: %s [-d] [-q#] [files or stdin]\n", prog);
	fprintf(stderr, "-d\tPreserve original durations exactly.\n");
	fprintf(stderr, "-i\tJustify key-offs independently of key-ons.\n");
	fprintf(stderr, "-q#\tQuantify to # beats per bar.\n");
	fprintf(stderr, "-t#\tIgnore time differences < # (threshold).\n");
	exit(2);
}

main(argc, argv)
char *argv[];
{
	int i, n;
	FILE *f;

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'd':
		    Dflg = 2;
		    break;
		case 'i':
		    Dflg = 0;
		    break;
		case 'q':
		    Quant = 1;
		    if (Quant = atoi(&argv[i][2]))
			Quant = (2 * MPU_CLOCK_PERIOD) / Quant;
		    break;
		case 't':
		    Thresh = atoi(&argv[i][2]);
		    break;
		default:
		    syntax(argv[0]);
		}
	    }
	}
	n = 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if ((f = sopen(argv[i], "r")) != NULL) {
		    just(f, stdout);
		    sclose(f);
		    n++;
		} else
		    perror(argv[i]);
	    }
	}
	if (n == 0)
	    just(stdin, stdout);
	exit(0);
}

void just(ifp, ofp)
FILE	*ifp, *ofp;
{
	int status, type, chan, key, vel;
	long inow, onow, anow, last;
	MCMD *mp;

	
	for (inow = last = 0; mp = getmcmd(ifp, inow); putmcmd(ofp, mp)) {
	    anow = (mp->when - inow) <= Thresh? last : mp->when;
	    inow = mp->when;
	    status = mp->cmd[0];
	    type = (status & M_CMD_MASK);
	    onow = -1;
	    if (type == CH_KEY_OFF) {
		type = CH_KEY_ON;
		mp->cmd[2] = 0;
	    }
	    if (type == CH_KEY_ON) {		/* calc chan, key, & vel */
		chan = (status & M_CHAN_MASK);
		key = mp->cmd[1];
		vel = mp->cmd[2];
		if (!vel && Dflg == 2)		/* preserve exact duration */
		    onow = inow + Dt[chan][key];
		if (!vel && Dflg == 1)		/* preserve approx. duration */
		    anow += Dt[chan][key];
	    }
	    if (onow == -1) {			/* justify */
		onow = anow + Quant / 2;
		onow -= (onow % Quant);
	    }
	    onow = onow < last? last : onow;	/* just in case */
	    if (Dflg && type == CH_KEY_ON && vel)
		Dt[chan][key] = onow - anow;	/* remember change to input */
	    last = mp->when = onow;
	}
	Rt_tcwme.when = anow > last? anow : last;
	putmcmd(ofp, &Rt_tcwme);
}
