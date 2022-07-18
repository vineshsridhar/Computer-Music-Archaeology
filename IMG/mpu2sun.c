/* @(#)mpu2sun.c 1.2 89/10/10 */
/*
**	MPU2Sun -- Turn MPU files into Sun MIDI files 
**	ehf/dss 4/89 from psl 1/88.
*/

#include <stdio.h>
#include <midi.h>
#include <math.h>

#define	MAXCHAN	16
#define	MAXV	8
#define  RT_TCWME 0xF8		/* guess (sau) */

char	*title	= 0;			/* title of piece */
int	meter1, meter2;			/* time signature of piece */
int	perc	= 0;			/* input is percussion */
long	start[MAXCHAN];			/* starts of notes */
int	curkey[MAXCHAN];		/* keys of notes */
long	last[MAXCHAN];			/* starts of rests */
int	qpb	= MPU_CLOCK_PERIOD;	/* quanta per bar */
int	quant;				/* clocks per quantum */
int	numv	= 1;			/* number of voices to display */
int	voices[MAXCHAN]	= { 1, };	/* associate voices & channels */

syntax(prog)
char	*prog;
{
	fprintf(stderr, "Usage: %s [-ttempo] mpu_file ... >midi-file\n", prog);
	exit(2);
}

static	float scale;
main(argc, argv)
char *argv[];
{
	int i, n, voice, chan;
	FILE *f;
	double bpm = 100.;
        extern double atof();

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
			case 't':
				bpm = atof(&(argv[i][2]));
				break;
			default:
			    syntax(argv[0]);
		}
	    }
	}
	scale =1000./MPU_TPS(bpm,120.);
	n = 0;
	if (isatty(fileno(stdout))) {
	    (void) freopen("/dev/midi", "w", stdout);
	}
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if ((f = sopen(argv[i], "r")) != NULL) {
		    cvt(f, stdout);
		    sclose(f);
		    n++;
		} else
		    perror(argv[i]);
	    }
	}
	if (n == 0)
	    cvt(stdin, stdout);
	exit(0);
}

cvt(ifp, ofp)
FILE	*ifp, *ofp;
{
	int i;
	long now;
	unsigned short delay=0;
	MCMD *mp;

	now = 0L;
	while (mp = getmcmd(ifp, now)) {
/*	        fprintf(stderr,"now = %f\n ",(double)now /120.);*/
		    delay += (mp->when - now)*scale +.5;
		    now = mp->when;
		if (mp->cmd[0] != RT_TCWME) {
			fputc(delay>>8,ofp);
			fputc(delay,ofp);
			fputc(mp->len,ofp);
			for (i = 0; i < mp->len; i++)
			    fputc(mp->cmd[i], ofp);
			delay = 0;
		}
	}
}
