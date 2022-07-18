/*
**	UNJUST		psl 12/85
** Copy MIDI data, perturbing the time values a little.
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <midi.h>
#include "sopen.h"
#include "putamcmd.h"

#define	MAXCHAN	16
#define	MAXKEY	128
#define	DEFQ	MPU_CLOCK_PERIOD / 32	/* equivalent to -q32.0 */

void unjust(FILE *in, FILE *out);


int	Flat	= 0;			/* pseudo-gaussian by default */
long	Quant	= DEFQ;			/* clocks in justification quantum */
long	Q2;				/* half of Quant */
long	Offset	= 0L;			/* global advance/delay amount */

// extern	double	atof();

main(argc,argv)
char *argv[];
{
	int i = 1, n;
	long now;
	FILE *f;

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'a':		/* advance by arg MPU clocks */
		    Offset = -atoi(&argv[i][2]);
		    break;
		case 'd':		/* delay by arg MPU clocks */
		    Offset = atoi(&argv[i][2]);
		    break;
		case 'f':		/* flat distribution */
		    Flat++;
		    break;
		case 'q':		/* set range as note values */
		    Quant = MPU_CLOCK_PERIOD / atof(&argv[i][2]);
		    if (Quant < 1)
			goto syntax;
		    break;
		case 'r':		/* set random range (in MPU clocks) */
		    Quant = atoi(&argv[i][2]);
		    if (Quant < 0)
			goto syntax;
		    break;
		default:
syntax:
		    fprintf(stderr, "Usage: %s [-a#] [-d#] ", argv[0]);
		    fprintf(stderr, "[-flat] [-q#.#] [-r#] [files or stdin]\n");
		    fprintf(stderr, "-a#\tadvances timing by # MPU clocks\n");
		    fprintf(stderr, "-d#\tdelays timing by # MPU clocks\n");
		    fprintf(stderr, "-flat\tsets flat (normal) distribution\n");
		    fprintf(stderr, "-q#\tsets the range to +|- 1/#th note\n");
		    fprintf(stderr, "-r#\tsets the range to +|-# MPU clocks\n");
		    fprintf(stderr, "Range is centered on 0, i.e. +/- #/2\n");
		    fprintf(stderr, "default is -q32 = -r15 (+/- 7 clocks)\n");
		    exit(2);
		}
	    }
	}
	Quant = ((Quant - 1) | 1);		/* guarantee odd */
	Q2 = (Quant + (Flat? 0 : 1)) / 2;
	time(&now);
	srand((int) now);
	n = 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if ((f = sopen(argv[i], "r")) == NULL)
		    perror(argv[i]);
		else {
		    unjust(f, stdout);
		    sclose(f);
		}
		n++;
	    }
	}
	if (n == 0)
	    unjust(stdin, stdout);
	exit(0);
}

void unjust(in, out)
FILE	*in, *out;
{
	int status, type, chan, key, vel;
	long onow, nnow, limit;
	long dt[MAXCHAN][MAXKEY];	/* offsets used for starts of notes */
	MCMD *mp;

	onow = nnow = limit = 0L;
	while (mp = getmcmd(in, onow)) {
	    onow = mp->when;
	    status = mp->cmd[0];
	    type = (status & M_CMD_MASK);
	    if (type == CH_KEY_OFF) {
		type = CH_KEY_ON;
		mp->cmd[2] = 0;
	    }
	    if (type == CH_KEY_ON) {
		chan = (status & M_CHAN_MASK);
		key = mp->cmd[1];
		vel = mp->cmd[2];
		if (vel) {			/* note on */
		    if (Flat)
			nnow = onow + (rand() % Quant) - Q2;
		    else
			nnow = onow + (rand() % Q2) - (rand() % Q2);
		    if (nnow < limit)
			nnow = limit;
		    dt[chan][key] = nnow - onow;
		} else {			/* note off */
		    nnow = onow + dt[chan][key];
		}
	    }
	    nnow += Offset;
	    mp->when = nnow < limit? limit : nnow;
	    limit = putamcmd(out, mp);
	}
	flushamcmd(out);
}
