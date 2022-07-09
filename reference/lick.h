/*
** LICK.H -- Generate "random" banjo parts
** psl 7/87
*/
#include <stdio.h>
#include <stdlib.h> // I added this for atoi
#include <time.h> // for time()
#include <string.h> // for strcmp, strcpy

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
#define DEFADAGIO "./tmp/lick.gio"
#define DEFALLEGRO "./tmp/lick.gro"
#define DEFSMF "./tmp/lick.mid"

struct Chordstr {
	char name[NAMELEN]; /* "C", "C#m", "C7", etc. (* => diminished) */
	int ctones[NTONES]; /* chord tones (0=C, 1=C#, 11=B, -1=end) */
	int ptones[NTONES]; /* passing tones (0=C, 1=C#, 11=B, -1=end) */
};
struct Fingerstr { /* finger info */
	int mask; /* rhpat mask */
	int lostr; /* lowest (numbered) string it can reach */
	int histr; /* highest (numbered) string it can reach */
	int bstr; /* best (favorite) string */
};

extern int chord_pat[]; /* chord indices, one per sixteenth note */
extern int piece_len; /* how many sixteenths in the piece */
//extern int Cpnum; /* which chord pattern to use (getchords.c) */
extern int ilhloc; /* initial left hand location */
//extern int rhpat[][PATLEN];/* right-hand picking patterns */
extern int nrhpat; /* number of patterns we can use */
extern struct Chordstr chord_info[MAXCHRD];
extern struct Fingerstr Fi[DIGITS];
extern int sharp_key;
extern FILE *trace_outfile;
extern int chord_parsing_bug;

// Function prototypes shared by lick0.c, lick1d.c, lick2.c:
char *pitchname(int p);
void compose();
int getchords(char *file);
int pitchof(int s, int f);
int ontlist(int p, int *lp);
int overlap(int s1[DIGITS], int s2[DIGITS]);
void output(int t, int str[DIGITS], int frt[STRINGS]);
int randr(int lo, int hi);
