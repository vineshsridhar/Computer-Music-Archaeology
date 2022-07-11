/*
** LICK.H -- Generate "random" banjo parts
** psl 7/87
** define NOMIDI to compile & run with ascii output
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#ifndef NOMIDI
#include <midi.h>
#endif NOMIDI

#define MAXCHRD 16 /* max number of unique chords */
#define MAXLEN 1024 /* max piece length, in sixteenths */
#define PATLEN 8 /* # of sixteenths / pattern */

#define MAXREACH 4 /* how big your hands are */

#define CPS 30 /* MPU clocks per step (sixteenth notes) */
#define KVEL 64 /* default key velocity */
#define CHAN 0 /* default channel */

#define STRINGS 5
#define FIRST 0 /* index of first string */
#define SECOND 1
#define THIRD 2
#define FOURTH 3
#define FIFTH 4 /* index of fifth string */

#define DIGITS 3
#define THUMB 0
#define INDEX 1
#define MIDDLE 2

#define OPEN 0 /* fret of open string */

#define NAMELEN 8
#define NTONES 13

struct chrdstr {
	char name[NAMELEN]; /* "C", "C#m", "C7", etc. (* => diminished) */
	int ctones[NTONES]; /* chord tones (0=C, 1=C#, 11=B, -1=end) */
	int ptones[NTONES]; /* passing tones (0=C, 1=C#, 11=B, -1=end) */
};
struct fistr { /* finger info */
	int mask; /* rhpat mask */
	int lostr; /* lowest (numbered) string it can reach */
	int histr; /* highest (numbered) string it can reach */
	int bstr; /* best (favorite) string */
};

extern char *Knams[]; /* key names */
extern int Nchrds; /* how many chords have been defined */
extern int Mcpn; /* how many chord patterns have been defined */
extern int Cpat[]; /* chord indices, one per sixteenth note */
extern int Plen; /* how many sixteenths in the piece */
extern int Cpnum; /* which chord pattern to use (getchords.c) */
extern int Ilhloc; /* initial left hand location */
extern int Tuning[STRINGS];/* How the banjo is tuned */
extern int rhpat[][PATLEN];/* right-hand picking patterns */
extern int Nrhpat; /* number of patterns we can use */
extern struct chrdstr Cstr[MAXCHRD];
extern struct fistr Fi[DIGITS];
extern FILE *Trace;
extern FILE *Tabfp;

char *pitchname();
getchords(char *file);
compose();
pitchof(int s,int i);
ontlist(int p, int *lp);
pickstring(int fpat, int strngs[DIGITS]);
meval(int t, int ap, int ns);
eval(int t0, int strings[MAXLEN][DIGITS], int frets[MAXLEN][STRINGS]);
overlap(int s1[DIGITS], int s2[DIGITS]);
output(int t, int str[DIGITS], int frt[STRINGS]);
randr(int lo, int hi);
cnvlist(char *str, int *list, int len);
