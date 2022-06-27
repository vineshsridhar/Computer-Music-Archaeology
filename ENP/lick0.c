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

char *LilyKnams[] = {
	"c", "cis", "d", "dis", "e", "f", "fis", "g", "gis", "a", "ais", "b",
};

char *LilyKnamsFlats[] = {
	"c", "des", "d", "ees", "e", "f", "ges", "g", "aes", "a", "bes", "b",
};

char *AllegroKnams[] = {
	"C", "CS", "D", "DS", "E", "F", "FS", "G", "GS", "A", "AS", "B",
};

int Tuning[STRINGS] = { /* How the banjo is tuned */
	62, 59, 55, 50, 67, /* open G-Major */
};
int Chan[STRINGS] = {
	0, 1, 2, 3, 4, /* default channel numbers */
};
int Ilhloc = OPEN; /* initial left hand position */

#define DEFTAB "./tmp/lick.tab"
#define DEFTRC "./tmp/lick.trace"

FILE *Tabfp = 0;
FILE *Trace = 0;
FILE *Lily = 0;
FILE *Allegro = 0;
int main(int argc, char *argv[])
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
					fprintf(stderr, "rhpat range is 1..%d", Nrhpat);
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
		fprintf(stderr, "Usage: %s CFILE [options]\n", argv[0]);
		fprintf(stderr, " CFILE contains chord specs in 'gc' format\n");
		fprintf(stderr, " -c# specifies MIDI chan for all strings\n");
		fprintf(stderr, " -l# specifies (approx) left hand location\n");
		fprintf(stderr, " -r# limits right hand patterns\n");
		fprintf(stderr, " -s#=# tunes a string\n");
		fprintf(stderr, " -tFILE puts tablature in FILE\n");
		fprintf(stderr, " -T outputs trace info in '%s'\n", DEFTRC);
		fprintf(stderr, "Defaults: -l%d -r%d", OPEN, Nrhpat);
		fprintf(stderr, " -s1=%d -s2=%d -s3=%d -s4=%d -s5=%d -t%s\n",
		  Tuning[0], Tuning[1], Tuning[2], Tuning[3], Tuning[4], DEFTAB);
		fprintf(stderr, "Each string defaults to its own channel, 1-50\n");
		exit(2);
	}
	time(&now);
	srand((int) now);
	if (!Tabfp && !(Tabfp = fopen(DEFTAB, "w")))
		perror(DEFTAB);
	if (!getchords(Cpfile))
		exit(1);

	if(!(Allegro = fopen(DEFALLEGRO, "w"))) {
		perror(DEFALLEGRO);
	}

	compose();

	fprintf(Lily, "\\bar \"|.\" \n}\n>>");
}

int pitchof(int s, int f) /* return MIDI-style pitch for string s on fret f */
{
	return(Tuning[s] + f);
}

int ontlist(int p, int *lp) /* return 1 iff p is on list *lp */
{

	p %= 12;
	while (*lp != -1)
		if (p == *lp++) {
			return(1);

		}
	return(0);
}

int overlap(int s1[DIGITS], int s2[DIGITS]) /* check for string crossing */
{
	return ((s1[THUMB] >= 0 && s2[INDEX] >= 0 && s1[THUMB] <= s2[INDEX])
	  || (s1[THUMB] >= 0 && s2[MIDDLE] >= 0 && s1[THUMB] <= s2[MIDDLE])
	  || (s1[INDEX] >= 0 && s2[THUMB] >= 0 && s1[INDEX] >= s2[THUMB])
	  || (s1[INDEX] >= 0 && s2[MIDDLE] >= 0 && s1[INDEX] <= s2[MIDDLE])
	  || (s1[MIDDLE] >= 0 && s2[THUMB] >= 0 && s1[MIDDLE] >= s2[THUMB])
	  || (s1[MIDDLE] >= 0 && s2[INDEX] >= 0 && s1[MIDDLE] >= s2[INDEX]));
}

int randr(int lo, int hi) /* return normal dist. rand # in range lo - hi (inclusive) */
{
	register int i;

	i = rand() % 0x1777;
	return(lo + i % (hi + 1 - lo));
}

int abs(register int i)
{
	return(i < 0 ? -i : i);
}

char *pitchname(int p)
{
	static char buf[64];

	sprintf(buf, "%s%d", Knams[p % 12], p / 12 - 1);
	return(buf);
}

char *allegropitchname(int p)
{
	static char buf[64];

	sprintf(buf, "%s%d", AllegroKnams[p % 12], p / 12 - 1);
	return(buf);
}

char *lilypitchname(int p)
{
	static char buf[64];
	char** keynotes = sharp_key ? LilyKnams : LilyKnamsFlats;

	switch(p / 12 - 1) {

		case 0:
			sprintf(buf, "%s,,,16", keynotes[p % 12]);
			break;
		case 1:
			sprintf(buf, "%s,,16", keynotes[p % 12]);
			break;
		case 2:
			sprintf(buf, "%s,16", keynotes[p % 12]);
			break;
		case 3:
			sprintf(buf, "%s16", keynotes[p % 12]);
			break;
		case 4:
			sprintf(buf, "%s'16", keynotes[p % 12]);
			break;
		case 5:
			sprintf(buf, "%s''16", keynotes[p % 12]);
			break;
		case 6:
			sprintf(buf, "%s'''16", keynotes[p % 12]);
			break;
		case 7:
			sprintf(buf, "%s''''16", keynotes[p % 12]);
			break;
		case 8:
			sprintf(buf, "%s'''''16", keynotes[p % 12]);
			break;
	}

	return(buf);
}

void output(int t, int str[DIGITS], int frt[STRINGS])
{
	register int d, s, dt, f, n;
	int sf[STRINGS];
	static int init = 0;
	char *name[64]; 

	if (Tabfp) {
		if (init == 0) {
			fprintf(Tabfp, "#TUNING ");
			for (s = STRINGS; --s >= 0; )
				fprintf(Tabfp, "%2d ", Tuning[s]);
			fprintf(Tabfp, "\n#NUT 5 0 0 0 0\n#SPEED 16\n");

			fprintf(Lily, "\\new Staff {\n");
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
		if (t <= 0 || Cpat[t] != Cpat[t - 1]) {// Print chord change
			fprintf(Tabfp, " %s", Cstr[Cpat[t]].name);

			// Allegro chords.
			for (int *lp=(&Cstr[Cpat[t]])->ctones; *lp > -1; fprintf(Allegro, "S %s3 N0 V1 Z105\n", AllegroKnams[*lp++]));

		} else if (t % 16 == 0) {

			// Allegro chords.
			for (int *lp=(&Cstr[Cpat[t]])->ctones; *lp > -1; fprintf(Allegro, "S %s3 N0 V1 Z105\n", AllegroKnams[*lp++]));
		}
		fprintf(Tabfp, "\n");
	}



	n = 0;
	for (d = 0; d < DIGITS; d++) {
		if ((s = str[d]) >= 0) {
			n++;
			f = frt[s];
			if (Trace)
				fprintf(Trace, "d=%d, s=%d, f=%d, p=%d (%s)\n",
				  d, s, f, pitchof(s, f), pitchname(pitchof(s, f)));

			// Lilypond
			fprintf(Lily, "%s ", lilypitchname(pitchof(s, f)));
			// Allegro
			fprintf(Allegro, "S %s V2 Z105\n", allegropitchname(pitchof(s, f)));

#ifndef NOMIDI
			putc(0, stdout); /* timing byte */
			putc(CH_KEY_ON | Chan[s], stdout);
			putc(pitchof(s, frt[s]), stdout);

			putc(KVEL, stdout);
#endif
		}
	}

	// Lilypond.
	if (n == 0)
		fprintf(Lily, "r16");

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
#endif
}