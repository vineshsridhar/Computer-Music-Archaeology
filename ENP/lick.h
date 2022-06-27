/*
** LICK.H -- Generate "random" banjo parts
** psl 7/87
** define NOMIDI to compile & run with ascii output
*/
#include <stdio.h>
#include <stdlib.h> // I added this for atoi
#include <time.h> // for time()
#include <string.h> // for strcmp, strcpy

#ifndef NOMIDI
#include <midi.h>
#endif

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

#define DEFLILY "./tmp/lick.ly"
#define DEFALLEGRO "./tmp/lick_a.gro"

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
//extern int Mcpn; /* how many chord patterns have been defined */
extern int Cpat[]; /* chord indices, one per sixteenth note */
extern int Plen; /* how many sixteenths in the piece */
//extern int Cpnum; /* which chord pattern to use (getchords.c) */
extern int Ilhloc; /* initial left hand location */
extern int Tuning[STRINGS];/* How the banjo is tuned */
//extern int rhpat[][PATLEN];/* right-hand picking patterns */
extern int Nrhpat; /* number of patterns we can use */
extern struct chrdstr Cstr[MAXCHRD];
extern struct fistr Fi[DIGITS];
extern int sharp_key;
extern FILE *Trace;
extern FILE *Tabfp;
extern FILE *Lily;
extern FILE *Allegro;


// Added Function Prototypes
char *pitchname(int p);
void compose();
int getchords(char *file);
int pitchof(int s, int f);
int ontlist(int p, int *lp);
void pickstring(int fpat, int strngs[DIGITS]);
int overlap(int s1[DIGITS], int s2[DIGITS]);
int meval(int t, int ap, int ns);
int eval(int t0, int strings[MAXLEN][DIGITS], int frets[MAXLEN][STRINGS]);
void output(int t, int str[DIGITS], int frt[STRINGS]);
void cnvlist(char *str, int *list, int len);
char *peel(char **spp);
int randr(int lo, int hi);
char *lilychord(int pat, int spc);
char *lilypitchname(int p);
char *allegropitchname(int p);