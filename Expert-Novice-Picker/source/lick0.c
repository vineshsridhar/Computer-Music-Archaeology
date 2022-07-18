/*
** LICK -- Generate "random" banjo parts
** lick0 module: main(), output(), misc functions
** psl 7/87
*/
#include "lick.h"

char *Cpfile = 0; /* file with chord pattern for getchords() */
char *Knams[] = {
	"C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
};
int Tuning[STRINGS] = { /* How the banjo is tuned */
	62, 59, 55, 50, 67, /* open G-Major */
};
int Chan[STRINGS] = {
	0, 1, 2, 3, 4, /* default channel numbers */
};
int Ilhloc = OPEN; /* initial left hand position */

#define DEFTAB "/tmp/lick.tab"
#define DEFTRC "/tmp/lick.trace"
FILE *Tabfp = 0;
FILE *Trace = 0;
main(argc, argv)
char *argv[];
{
	register int i, f, s;
	char *cp;
	long now;

	for (i = 1; i < argc; i++) {
		if (argv[i][0] == '-') {
			switch (argv[i][1]) {
			case 'c': /* set midi channels */
				f = atoi(&argv[i][2]) - 1;
				for (s = STRINGS; --s >= FIRST; Chan[s] = f);
				break;
			case 'l': /* specify initial left hand pos */
				Ilhloc = atoi(&argv[i][2]);
				break;
			case 'r': /* limit rhpat choices */
				s = atoi(&argv[i][2]);
				if (s <= 0 || s > Nrhpat) {
					fprintf(stderr, "rhpat range is 1..%d0, Nrhpat);
					goto syntax;
				}
				Nrhpat = s;
				break;
			case 's':
				s = atoi(&argv[i][2]);
				f = atoi(&argv[i][4]);
				if (argv[i][3] != '='
				  || s < 1 || s > 5
				  || f < 0 || f > 127)
					goto syntax;
				Tuning[s - 1] = f;
				break;
			case 't':
				if (argv[i][2] <= ' '
				  || !(Tabfp = fopen(&argv[i][2], "w"))) {
					perror(&argv[i][2]);
					goto syntax;
				}
				break;
			case 'T':
				if (!(Trace = fopen(DEFTRC, "w")))
					perror(DEFTRC);
				break;
			default:
				goto syntax;
			}
		} else if (Cpfile) {
			goto syntax;
		} else {
			Cpfile = argv[i];
		}
	}
	if (!Cpfile) {
syntax:
		fprintf(stderr, "Usage: %s CFILE [options]0, argv[0]);
		fprintf(stderr, " CFILE contains chord specs in 'gc' format0);
		fprintf(stderr, " -c# specifies MIDI chan for all strings0);
		fprintf(stderr, " -l# specifies (approx) left hand location0);
		fprintf(stderr, " -r# limits right hand patterns0);
		fprintf(stderr, " -s#=# tunes a string0);
		fprintf(stderr, " -tFILE puts tablature in FILE0);
		fprintf(stderr, " -T outputs trace info in '%s'0, DEFTRC);
		fprintf(stderr, "Defaults: -l%d -r%d", OPEN, Nrhpat);
		fprintf(stderr, " -s1=%d -s2=%d -s3=%d -s4=%d -s5=%d -t%s0,
		  Tuning[0], Tuning[1], Tuning[2], Tuning[3], Tuning[4], DEFTAB);
		fprintf(stderr, "Each string defaults to its own channel, 1-50);
		exit(2);
	}
	time(&now);
	srand((int) now);
	if (!Tabfp && !(Tabfp = fopen(DEFTAB, "w")))
		perror(DEFTAB);
	if (!getchords(Cpfile))
		exit(1);
	compose();
}

pitchof(s, f) /* return MIDI-style pitch for string s on fret f */
{
	return(Tuning[s] + f);
}

ontlist(p, lp) /* return 1 iff p is on list *lp */
int *lp;
{

	p %= 12;
	while (*lp != -1)
		if (p == *lp++)
			return(1);
	return(0);
}

rlimit(s, f, mr, bfp, tfp) /* update reach limits *bfp & *tfp */
int *bfp, *tfp;
{
	if (f > 0) {
		f += (s == FIFTH? 5 : 0);
		if (f + mr < *tfp)
			*tfp = f + mr;
		if (f - mr > *bfp)
			*bfp = f - mr;
	}
}

overlap(s1, s2) /* check for string crossing */
int s1[DIGITS], s2[DIGITS];
{
	return ((s1[THUMB] >= 0 && s2[INDEX] >= 0 && s1[THUMB] <= s2[INDEX])
	  || (s1[THUMB] >= 0 && s2[MIDDLE] >= 0 && s1[THUMB] <= s2[MIDDLE])
	  || (s1[INDEX] >= 0 && s2[THUMB] >= 0 && s1[INDEX] >= s2[THUMB])
	  || (s1[INDEX] >= 0 && s2[MIDDLE] >= 0 && s1[INDEX] <= s2[MIDDLE])
	  || (s1[MIDDLE] >= 0 && s2[THUMB] >= 0 && s1[MIDDLE] >= s2[THUMB])
	  || (s1[MIDDLE] >= 0 && s2[INDEX] >= 0 && s1[MIDDLE] >= s2[INDEX]));
}

reach(f1, f2) /* calculate fret reach */
int f1[STRINGS], f2[STRINGS];
{
	register int s, f, min, max;

	min = 99;
	max = 0;
	for (s = STRINGS; --s >= 0; ) {
		if ((f = f1[s]) > 0) {
			if (s == 4)
				f += 5;
			if (f < min)
				min = f;
			if (f > max)
				max = f;
		}
		if ((f = f2[s]) > 0) {
			if (s == 4)
				f += 5;
			if (f < min)
				min = f;
			if (f > max)
				max = f;	
		}
	}
	f = max - min;
	if (f < 0)
		f = 0;
	return(f);
}

randr(lo, hi) /* return normal dist. rand # in range lo - hi (inclusive) */
{
	register int i;

	i = rand() % 0x1777;
	return(lo + i % (hi + 1 - lo));
}

abs(i)
register int i;
{
	return(i < 0 ? -i : i);
}

char *
pitchname(p)
{
	static char buf[64];

	sprintf(buf, "%s%d", Knams[p % 12], p / 12 - 1);
	return(buf);
}

output(t, str, frt)
int str[DIGITS], frt[STRINGS];
{
	register int d, s, dt, f;
	int sf[STRINGS];
	static int init = 0;

	if (Tabfp) {
		if (init == 0) {
			fprintf(Tabfp, "#TUNING ");
			for (s = STRINGS; --s >= 0; )
				fprintf(Tabfp, "%2d ", Tuning[s]);
			fprintf(Tabfp, "0NUT 5 0 0 0 00SPEED 160);
			init++;
		}
		for (s = STRINGS; --s >= 0; sf[s] = -1);
		for (d = 0; d < DIGITS; d++) {
			if ((s = str[d]) >= 0) {
				fprintf(Tabfp, "%c", "TIM"[d]);
				sf[s] = frt[s];
			}
		}
		fprintf(Tabfp, " ");
		for (s = STRINGS; --s >= 0; ) {
			if (sf[s] >= 0)
				fprintf(Tabfp, "%2d ", sf[s]);
			else
				fprintf(Tabfp, " | ");
		}
		if (t <= 0 || Cpat[t] != Cpat[t - 1])
			fprintf(Tabfp, " %s", Cstr[Cpat[t]].name);
		fprintf(Tabfp, "0);
	}
	for (d = 0; d < DIGITS; d++) {
		if ((s = str[d]) >= 0) {
			f = frt[s];
			if (Trace)
				fprintf(Trace, "d=%d, s=%d, f=%d, p=%d (%s)0,
				  d, s, f, pitchof(s, f), pitchname(pitchof(s, f)));
			
#ifndef NOMIDI
			putc(0, stdout); /* timing byte */
			putc(CH_KEY_ON | Chan[s], stdout);
			putc(pitchof(s, frt[s]), stdout);

			putc(KVEL, stdout);
#endif NOMIDI
		}
	}
#ifndef NOMIDI
	dt = CPS;
	for (d = DIGITS; --d >= 0; ) {
		if ((s = str[d]) >= 0) {
			putc(dt, stdout); /* timing byte */
			dt = 0;
			putc(CH_KEY_ON | Chan[s], stdout);
			putc(pitchof(s, frt[s]), stdout);
			putc(0, stdout);
		}
	}
	if (dt) {
		putc(dt, stdout);
		putc(MPU_NO_OP, stdout);
	}
#endif NOMIDI
}