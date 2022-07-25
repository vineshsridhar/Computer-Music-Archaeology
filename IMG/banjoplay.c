/*
**	BANJOPLAY -- Compose banjo part to fit chords
**	banjotune(key, stones, chans);			"tune" the banjo
**		int key;		0-11, 0=>C, 11=>B
**		int stones[12];		scale of piece, booleans
**		int chans[5];		0-15, MIDI channels for each string
**	banjoplay(start, stop, otones, ntones, vel, ofp); "play" the banjo
**		long start;		beginning time, MPU clocks
**		long stop;		ending time, MPU clocks
**		int otones[12];		notes in current chord, booleans
**		int ntones[12];		notes in next chord, booleans
**					ntones[0] < 0 => end
**		int vel;		MIDI key velocity for notes
**		FILE *ofp;		output file handle
**	psl 2/89
*/
#include <stdlib.h>
#include	<stdio.h>
#define	BANJO			/* for libamc.h */
#include	"midi.h"
#include "banjotune.h"
#include "key2name.h"

#define	WHOLE		(MPU_CLOCK_PERIOD<<1)
#define	SIXTEENTH	(MPU_CLOCK_PERIOD>>3)
#define	DBG		if(Debug)fprintf
#define	PITCHOF(S,F)	(Tuning[S]+(F))
#define	RANDR(L,H)	((L)+(rand()/3)%((H)-(L)+1))
#define	ABS(A)		((A)<0?-(A):(A))
#define	MIN(A,B)	((A)<(B)?(A):(B))
#define	MAX(A,B)	((A)>(B)?(A):(B))
#define	PR50		(rand()&010)		/* 50-50 fcn */
/* calc smallest offset (transposition) to put note N in key K on the banjo */
#define	LOWPOS(N,K)	(12*((61-(K)-(N))/12)+(K))


void ending(long start, long stop, int vel, int lastkey, FILE *ofp);
int seqout(RIFF *rp, long start, int vel, FILE *ofp);
void pickstring(STEP *cstep, STEP *pstep);
void pickfret(int lhloc, STEP *cstep, int tones[12], int beatval);
int eval(PSEQ *ctp, PSEQ *ptp, int tones[][12]);
void output(int t0, PSEQ *tp, int vel, FILE *ofp);

// Right hand fingers
struct	banfstr	Fi[DIGITS] = {	/* finger info */
/*	code	lowest	highest	favorite	*/
	{ T,	THIRD,	FIFTH,	FIFTH, },	/* thumb (numbers are -1) */
	{ I,	SECOND,	FOURTH,	THIRD, },	/* index */
	{ M,	FIRST,	THIRD,	FIRST, },	/* middle */
};

// Right hand patterns.
int	Rhpat[][PATLEN] = {
	T,   I,   M,   T,   I,   M,   T,   M,		/* forward */
	T,   M,   I,   T,   M,   I,   T,   I,		/* backward */
	T,   I,   M,   T,   M,   I,   T,   M,		/* backup */
	T,   I,   M,   T,   M,   T,   M,   I,		/* split roll */
	T,   I,   T,   I,   T,   I,   T,   I,		/* flat-pick */
	T,   I,   T,   M,   T,   I,   T,   M,		/* double thumb */
	I,   M,   I,   M,   T,   I,   M,   T,		/* foggymtn */
	T,   I,   M,   T,   T,   I,   M,   T,		/* 4-note roll */
	T,   I,   M,   I,   T,   I,   M,   I,		/* guitar roll */
	T,   I,   T,   M,   T,   0,   T|M, 0,		/* John Hickman */
	I,   0,   T|M, 0,   I,   0,   T|M, 0,		/* bum-chang */
};

// BSEQ = "Banjo Sequence"
// Each struct stores MPU start time
// and a list of notes to play (relative to lowest root of key)
// there are 5 indices, representing the five strings. Some sequences have multiple notes that play at the same time.
static	BSEQ	Beg0[]	= {
	{ 60,	-1,-1,-1, 7,-1, },
	{ 120,	-1,-1,-1, 7,-1, },
	{ 160,	-1,-1,-1, 9,-1, },
	{ 180,	-1,-1,12, 7,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg1[]	= {
	{ 0,	24,19,16,12,-1, },
	{ 60,	19,-1,-1,-1,-1, },
	{ 120,	-1,-1,-1, 7,-1, },
	{ 160,	-1,-1,-1, 9,-1, },
	{ 180,	-1,-1,12,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg2[]	= {
	{ 30,	-1,17,-1,-1,-1, },
	{ 45,	-1,-1,14,-1,-1, },
	{ 60,	-1,-1,-1,11,-1, },
	{ 120,	-1,-1,-1, 7,-1, },
	{ 160,	-1,-1,-1, 9,-1, },
	{ 180,	-1,-1,12,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg3[]	= {
	{ 60,	-1,-1,-1, 7,-1, },
	{ 90,	-1,-1,-1, 9,-1, },
	{ 120,	-1,-1,-1,11,-1, },
	{ 150,	-1,-1,-1,12,-1, },
	{ 180,	-1,-1,14,-1,-1, },
	{ 210,	-1,-1,15,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg4[]	= {
	{ 60,	24,19,-1,-1,-1, },
	{ 120,	-1,18,-1,-1,-1, },
	{ 180,	21,17,-1,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg5[]	= {
	{ 60,	19,19,-1,-1,-1, },
	{ 120,	19,18,-1,-1,-1, },
	{ 180,	19,17,-1,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};
static	BSEQ	Beg6[]	= {
	{ 40,	19,-1,-1,-1,-1, },
	{ 60,	-1,-1,-1, 7,-1, },
	{ 120,	-1,-1,-1, 9,-1, },
	{ 180,	-1,-1,12,-1,-1, },
	{ 240,	-1,-1,-1,-1,-1, },
};

// Beginning riffs
// RIFF structs store
// - length of riff
// - range of key values
// - key to use for distance calc (in BSEQ)
// - BSEQ string with notes
static	RIFF	Bbegs[]	= {
	{ 240,    7, 12,  7, Beg0, },
	{ 240,    7, 19, 12, Beg1, },
	{ 240,    7, 17, 17, Beg2, },
	{ 240,    7, 15,  7, Beg3, },
	{ 240,   17, 24, 19, Beg4, },
	{ 240,   17, 19, 19, Beg5, },
	{ 240,    7, 19, 12, Beg6, },
	{ 0, },
};
int	Nbegs	= 7;

// Ending sequences/riffs
static	BSEQ	End0[]	= {
	{ 0,	-1,-1,-1, 0,-1, },
	{ 120,	-1,-1, 4,-1,-1, },
	{ 160,	 7,-1,-1,-1,-1, },
	{ 180,	-1, 9,-1,-1,-1, },
	{ 240,	-1,-1,-1,-1,12, },
	{ 360,	12, 7, 4,-1,-1, },
	{ 390,	-1,-1,-1,-1,-1, },
	{ 480,	-1,-1,-1,-1,-1, },
};
static	BSEQ	End1[]	= {
	{ 0,	12,-1,-1,-1,-1, },
	{ 120,	-1, 9,-1,-1,-1, },
	{ 160,	 7,-1,-1,-1,-1, },
	{ 180,	-1, 4,-1,-1,-1, },
	{ 240,	-1,-1, 0,-1,-1, },
	{ 360,	12, 7, 4,-1,-1, },
	{ 390,	-1,-1,-1,-1,-1, },
	{ 480,	-1,-1,-1,-1,-1, },
};
static	BSEQ	End2[]	= {
	{ 0,	-1,-1, 3,-1,-1, },
	{ 35,	-1, 4,-1,-1,-1, },	/* half-swung */
	{ 60,	 9,-1,-1,-1,-1, },
	{ 95,	-1,-1,-1,-1,12, },	/* half-swung */
	{ 120,	 7,-1,-1,-1,-1, },
	{ 155,	-1, 4,-1,-1,-1, },	/* half-swung */
	{ 180,	 9,-1,-1,-1,-1, },
	{ 215,	-1,-1,-1,-1,12, },	/* half-swung */
	{ 240,	 7,-1,-1,-1,-1, },
	{ 275,	-1,-1, 3,-1,-1, },	/* half-swung */
	{ 300,	-1,-1, 2,-1,-1, },
	{ 360,	 7, 4, 0,-1,-1, },
	{ 480,	-1,-1,-1,-1,-1, },
};
static	BSEQ	End3[]	= {
	0,	 9, 4, 3,-1,-1,
	90,	 9, 4, 3,-1,-1,
	180,	 9, 4, 3,-1,-1,
	210,	-1,-1,-1,-1,12,
	240,	 7, 4, 0,-1,-1,
	360,	 7, 4, 0,-1,12,
	480,	-1,-1,-1,-1,-1,
};
static	BSEQ	End4[]	= {
	60,	19,-1,15,-1,-1,
	100,	-1,-1,14,-1,-1,
	160,	-1,16,12,-1,-1,
	180,	-1,-1,-1, 9,-1,
	240,	-1,-1,-1,12,-1,
	360,	19,-1,-1,-1,24,
	480,	-1,-1,-1,-1,-1,
};
static	BSEQ	End5[]	= {
	0,	-1,-1, 0,-1,-1,
	60,	-1,-1,-1,-1,12,
	120,	10,-1,-1,-1,-1,
	140,	 9,-1,-1,-1,-1,
	160,	 7,-1,-1,-1,-1,
	180,	-1, 4,-1,-1,-1,
	200,	-1,-1, 3,-1,-1,
	220,	-1,-1, 2,-1,-1,
	240,	-1,-1, 0,-1,-1,
	360,	 7, 4, 0,-1,12,
	480,	-1,-1,-1,-1,-1,
};
static	BSEQ	End6[]	= {
	0,	 7, 4, 0,-1,12,
	240,	-1,-1,-1,-1,-1,
};

static	RIFF	Bends[]	= {
	{ 480,   0, 12,  0, End0, },
	{ 480,   0, 12, 12, End1, },
	{ 480,   0, 12,  3, End2, },
	{ 480,   0, 12,  9, End3, },
	{ 480,   9, 24, 16, End4, },
	{ 480,   0, 12, 12, End5, },
	{ 240,   0, 12,  7, End6, },
	{ 0, },
};
#define	NRHPAT	(sizeof Rhpat / sizeof Rhpat[0])

// "pattern structure"
// Placeholder to store previously used pattern
PSEQ	Last	= {			/* can be preset (global) */
	1,				/* initial left hand location */
	0,				/* previous pattern */
	{ 0, },				/* all previous step info is 0 */
};

int	Mrhpat	= NRHPAT;	/* max number of patterns available (global) */
int	Nrhpat	= NRHPAT;	/* num of pats we can use this time (global) */
int	Nrhptry	= 3;		/* number of patterns to try each time */
int	Tuning[STRINGS]	= {	/* How the banjo is tuned (global) */
	62, 59, 55, 50, 67,	/* e.g. open G-Major */
};
static	int	Tunes[12][STRINGS] = {	/* tunings for each major key */
	62, 59, 55, 50, 67,	/* C (open G) */
	63, 60, 56, 51, 68,	/* C# (open G up 1) */
	62, 57, 54, 50, 66,	/* D (open D) */
	63, 58, 55, 51, 67,	/* D# (open D up 1) */
	64, 59, 56, 52, 68,	/* E (open D up 2) */
	65, 60, 57, 53, 69,	/* F (open D up 3) */
	66, 61, 58, 54, 70,	/* F# (open D up 4) */
	62, 59, 55, 50, 67,	/* G (open G) */
	63, 60, 56, 51, 68,	/* G# (open G up 1) */
	64, 61, 57, 52, 69,	/* A (open G up 2) */
	65, 62, 58, 53, 70,	/* A# (open G up 3) */
	66, 63, 59, 54, 71,	/* B (open G up 4) */
};
static	int	Beatval[]	= { 2,0,1,0,2,0,1,0, };
static	int	Chan[STRINGS];	/* lead instrument channel(s) */
static	int	Key;
static	int	Stones[12];

/* this should really be an extern, but the loader gets confused */
int	Debug;		/* debugging output (from main()) */

/* Input is the key of the song, scale of the piece (boolean array), and the number of channels for each of the 5 strings. 
 * Set scale and channel global arrays.
 * Initialize the step info for each finger in the Last PSEQ struct.
 * Tune the banjo to the appropriate key using the Tunes array.
 */
void
banjotune(key, stones, chans)
int	stones[12], chans[STRINGS];
{
	register int i, pt;

	Key = key;
	for (i = STRINGS; --i >= 0; Chan[i] = chans[i]);
	for (i = 12; --i >= 0; Stones[i] = (stones[i]? 1 : 0)); // Stones is a boolean array that denotes scale pitches.
	for (pt = PATLEN; --pt >= 0; ) {
	    for (i = DIGITS; --i >= THUMB; ) {
		Last.step[pt].key[i] = 0;
		Last.step[pt].rhs[i] = -1;
	    }
	}
	for (i = STRINGS; --i >= 0; Tuning[i] = Tunes[Key % 12][i]);
}

/* start: start time of the music in MPU clocks
 * stop: stop time of the music in MPU clocks
 * otones: chord tones for current chord
 * ntones: chord tones for next chord
 * vel: midi key velocity for notes
 * ofp: pointer to output file
 */
void
banjoplay(start, stop, otones, ntones, vel, ofp)
long	start, stop;
int	otones[12], ntones[12];
FILE	*ofp;
{
	int tstart, tstop, t;
	int pt, p, t0, nrhp, i, lhloc, maxlhloc, pat;
	int v, bv, tries;
	STEP *cstep, *pstep;
	PSEQ try, btry;
	static int tones[PATLEN][12];

	tstart = start / SIXTEENTH; // Convert the number of MPU clocks to the number of sixteenth notes.
	tstop = stop / SIXTEENTH;
DBG(stderr, "banjoplay(%ld, %ld, otones, ntones, %d, ofp) tstart,tstop = %d,%d\n", start, stop, vel, tstart, tstop);
	if (!otones[0] && !otones[1] && !otones[2] && !otones[3]
	 && !otones[4] && !otones[5] && !otones[6] && !otones[7]
	 && !otones[8] && !otones[9] && !otones[10] && !otones[11]) { /* rest */
	    t = tstop - tstart;
	    if (t > 8) {
		ending(start, stop - 8 * SIXTEENTH, vel, 60 + Key, ofp);
		start = stop - 8 * SIXTEENTH;
	    }
	    i = rand() % Nbegs;
	    seqout(&Bbegs[i], start, vel, ofp);
DBG(stderr, " used beg=%d\n", i);
	    return;
	}
	if (ntones[0] < 0) {			/* end of piece. Current chord is last chord */
	    for (pt = PATLEN; --pt >= 0; ) {
		for (i = DIGITS; --i >= THUMB; )
		    if (v = Last.step[pt].key[i])
			break;
		if (v)
		    break;
	    }
	    ending(start, stop, vel, v = v? v : (60 + Key), ofp);
	    return;
	}
	for (t = tstart; t < tstop; t++) {
	    pt = t % PATLEN;
DBG(stderr, "t = %d, pt = %d\n", t, pt);
	    for (p = 12; --p >= 0; )
		tones[pt][p] = 4 * otones[p] + 2 * Stones[p] + ntones[p];
	    if (pt != PATLEN - 1)
		continue;
	    t0 = t - pt;
DBG(stderr, "t0 = %d\n", t0);
	    bv= 0;
	    for (nrhp = 0; nrhp < Nrhptry; nrhp++) { /* try right-hand pats */
		pat = nrhp? (rand() % Nrhpat) : Last.pat;
		try.pat = pat;
		for (i = 0; i < PATLEN; i++)
		    try.step[i].rhd = Rhpat[pat][i];
		i = Last.lhloc;		/* try all left-hand locations */
		maxlhloc = i + (i < 9? 3 : (18 - i) / 3);
		for (lhloc = (i < 4)? 1 : (i - 3); lhloc < maxlhloc; lhloc++) {
		    try.lhloc = lhloc;
		    for (tries = 0; tries < 2; tries++) {
			for (pt = 0; pt < PATLEN; pt++ ) {
			    cstep = &try.step[pt];
			    pstep = pt? &try.step[pt-1] : &Last.step[PATLEN-1];
			    pickstring(cstep, pstep);
			    pickfret(lhloc, cstep, tones[pt], Beatval[pt]);
			}
			v = eval(&try, &Last, tones);
DBG(stderr, "pat %d = %d, lhloc = %d, eval = %d\n", nrhp, pat, lhloc, v);
			if (v > bv) {
			    btry = try;
			    bv = v;
			}
		    }
		}
	    }
DBG(stderr, "output pat=%d lhloc=%d\n", btry.pat, btry.lhloc);
	    output(t0, &btry, vel, ofp);
	    Last = btry;
	}
/****/if (Debug) { int pp,cc;
/****/ DBG(stderr, "end of banjoplay, tones =\n");
/****/ for (pp = 0; pp < PATLEN; pp++) {
/****/  for (cc = 0; cc < 12; cc++)
/****/   DBG(stderr, " %d", tones[pp][cc]);
/****/  DBG(stderr, "\n");
/****/ }
/****/}
}

void pickstring(cstep, pstep)	/* randomly associate strings with digits */
STEP	*cstep, *pstep;		/* avoid overlaps */
{
	register int lo, hi;

	if (cstep->rhd & M) {
	    lo = FIRST;
	    hi = Fi[MIDDLE].histr;
	    if (pstep->rhs[INDEX] >= 0)
		hi = MIN(hi, pstep->rhs[INDEX] - 1);
	    else if (pstep->rhs[THUMB] >= 0)
		hi = MIN(hi, pstep->rhs[THUMB] - 1);
	    cstep->rhs[MIDDLE] = RANDR(lo, hi);
	} else
	    cstep->rhs[MIDDLE] = -1;
	if (cstep->rhd & I) {
	    lo = Fi[INDEX].lostr;
	    if (pstep->rhs[MIDDLE] >= 0)
		lo = MAX(lo, pstep->rhs[MIDDLE] + 1);
	    if (cstep->rhs[MIDDLE] >= 0)
		lo = MAX(lo, cstep->rhs[MIDDLE] + 1);
	    hi = Fi[INDEX].histr;
	    if (pstep->rhs[THUMB] >= 0)
		hi = MIN(hi, pstep->rhs[THUMB] - 1);
	    cstep->rhs[INDEX] = RANDR(lo, hi);
	} else
	    cstep->rhs[INDEX] = -1;
	if (cstep->rhd & T) {
	    lo = Fi[THUMB].lostr;
	    if (pstep->rhs[INDEX] >= 0)
		lo = MAX(lo, pstep->rhs[INDEX] + 1);
	    else if (pstep->rhs[MIDDLE] >= 0)
		lo = MAX(lo, pstep->rhs[MIDDLE] + 1);
	    if (cstep->rhs[INDEX] >= 0)
		lo = MAX(lo, cstep->rhs[INDEX] + 1);
	    else if (cstep->rhs[MIDDLE] >= 0)
		lo = MAX(lo, cstep->rhs[MIDDLE] + 1);
	    hi = FIFTH;
	    cstep->rhs[THUMB] = RANDR(lo, hi);
	} else
	    cstep->rhs[THUMB] = -1;
}

/* randomly pick frets for played strings */
void pickfret(lhloc, cstep, tones, beatval)
STEP	*cstep;
int	tones[12];
{
	register int s, d, n, w, f;
	int fr[32], sp[32];

	for (d = DIGITS; --d >= THUMB; ) {
	    if ((s = cstep->rhs[d]) >= 0) {
		n = 0;
		sp[OPEN] = PITCHOF(s, OPEN);
		for (w = tones[sp[OPEN] % 12]; --w >= beatval; fr[n++] = OPEN);
		for (f = lhloc + MAXREACH; --f >= lhloc; ) {
		    if (s == FIFTH && f <= 5)
			continue;
		    sp[f] = PITCHOF(s, (s == FIFTH? f - 5 : f));
		    for (w = tones[sp[f] % 12]; --w >= beatval; fr[n++] = f);
		}
		f = n? fr[rand() % n] : OPEN;
		cstep->lhf[d] = f;
		cstep->key[d] = sp[f];
	    } else
		cstep->lhf[d] = cstep->key[d] = 0;
	}
}

void output(t0, tp, vel, ofp)
PSEQ	*tp;
FILE	*ofp;
{
	u_char mbuf[8];
	int pt, d, s;
	MCMD m;

	/* output the best */
	m.len = 3;
	m.cmd = mbuf;
	for (pt = 0; pt < PATLEN; pt++) {
	    m.when = (t0 + pt) * SIXTEENTH;
	    m.cmd[2] = vel;
	    for (d = DIGITS; --d >= THUMB; ) {
		if ((s = tp->step[pt].rhs[d]) >= 0) {
		    m.cmd[0] = CH_KEY_ON | Chan[s];
		    m.cmd[1] = tp->step[pt].key[d];
		    putmcmd(ofp, &m);
DBG(stderr, "%5ld: %s\n", m.when, key2name(m.cmd[1]));
		}
	    }
	    m.when += SIXTEENTH;
	    m.cmd[2] = 0;
	    for (d = DIGITS; --d >= THUMB; ) {
		if ((s = tp->step[pt].rhs[d]) >= 0) {
		    m.cmd[0] = CH_KEY_ON | Chan[s];
		    m.cmd[1] = tp->step[pt].key[d];
		    putmcmd(ofp, &m);
		}
	    }
	}
}

int eval(ctp, ptp, tones)	/* evaluate the PATLEN steps in *ctp */
PSEQ	*ctp, *ptp;	/* current & prior patterns */
int	tones[][12];
{
	register int v, pt, d, p, d2, p2, dp, mink, maxk, ptv, rtv;
	STEP *cstep, *pstep, *ppstep;

	v = 0;
	for (pt = 0; pt < PATLEN; pt++)		      /* chord/scale spectrum */
	    for (d = DIGITS; --d >= THUMB; )
		if (p = ctp->step[pt].key[d])
		    v += 1 + (Beatval[pt] + 1) * tones[pt][p % 12];
	cstep = &ptp->step[PATLEN - 1];				/* resolution */
	for (pt = 0; pt < PATLEN; pt++) {
	    pstep = cstep;
	    cstep = &ctp->step[pt];
	    for (d = DIGITS; --d >= THUMB; ) {
		if (p = pstep->key[d]) {
		    ptv = tones[pt][p % 12];
		    if (ptv < (rtv = tones[pt][(p + 1) % 12])) {
			for (d2 = DIGITS; --d2 >= THUMB; )     /* up resolves */
			    if (cstep->key[d2] == p + 1)
				v += (Beatval[pt] + 1) * rtv;
		    }
		    if (ptv < (rtv = tones[pt][(p - 1) % 12])) {
			for (d2 = DIGITS; --d2 >= THUMB; )   /* down resolves */
			    if (cstep->key[d2] == p - 1)
				v += (Beatval[pt] + 1) * rtv;
		    }
		}
	    }
	}
	for (pt = 0; pt < PATLEN; pt++) { /* left hand bonus (fretted string) */
	    cstep = &ctp->step[pt];
	    for (d = DIGITS; --d >= THUMB; )
		if (cstep->key[d] && cstep->lhf[d] > 0)
		    v += 3;
	}
	mink = 127;						    /* motion */
	maxk = 0;
	pstep = &ptp->step[PATLEN - 2];
	cstep = &ptp->step[PATLEN - 1];
	for (pt = 0; pt < PATLEN; pt++) {
	    ppstep = pstep;
	    pstep = cstep;
	    cstep = &ctp->step[pt];
	    for (d = DIGITS; --d >= THUMB; ) {
		if (p = cstep->key[d]) {
		    for (d2 = DIGITS; --d2 >= THUMB; ) {
			if (p2 = pstep->key[d2]) {	/* vs prior note */
			    dp = p - p2;
			    dp = ABS(dp);
			    if (dp == 0)
				v += 0;		/* used to be +2 */
			    else if (dp < 3)
				v += 6;
			    else if (dp < 7)
				v += 2;
			}
			if (p2 = ppstep->key[d2]) {	/* vs prior prior */
			    dp = p - p2;
			    dp = ABS(dp);
			    if (dp == 0)
				v += 3;
			    else if (dp < 3)
				v += 5;
			    else if (dp < 7)
				v += 3;
			}
		    }
		    if (p < mink)
			mink = p;
		    if (p > maxk)
			maxk = p;
		}
	    }
	}
	if ((p = maxk - mink) > 12)				    /* spread */
	    v += 10 - p;
	return(v);
}

void ending(start, stop, vel, lastkey, ofp)		/* play an ending riff */
long	start, stop;
FILE	*ofp;
{
	int n;
	int seq, bseq, dist, bdist, len, blen;

DBG(stderr, "ending(start=%ld, stop=%ld, vel=%d, lastkey=%d, ofp)\n",
 start, stop, vel, lastkey);
	while (stop - start > 0) {
	    bdist = 999;		/* choose an ending riff */
	    blen = 0;
	    for (seq = 0; Bends[seq].len; seq++) { // iterate through all ending sequences.
		len = Bends[seq].len;
		if (len > stop - start || len < blen)
		    continue;
		n = LOWPOS(Bends[seq].lo, Key); /* Transpose sequence lowest pos'n to Key */
		dist = lastkey - (n + Bends[seq].first); // Calculate distance between last note of prev sequence and first note of chosen Bend seq.
		dist = dist < 0? -dist : dist;
		// Choose new best ending sequence.
		if (len > blen // longer sequences are better
		 || dist < bdist // sequences with better voiceleading from prev sequence better
		 || (dist == bdist && PR50)) { // if all the same, then flip a 50-50 coin (PR50)
		    bseq = seq;
		    bdist = dist;
		    blen = len;
		}
	    }
	    lastkey = seqout(&Bends[bseq], start, vel, ofp);
DBG(stderr, " used end=%d, lastkey=%d, now start=%ld\n", bseq, lastkey, start);
	    start += blen;
	}
	return;
}

/* Output the sequence.
 * rp: Pointer to the riff in the Bends RIFF array
 * start: start time in MPU clocks
 * vel: key velocity for notes
 * ofp: output file pointer
 */
int seqout(rp, start, vel, ofp)
RIFF	*rp;
long	start;
FILE	*ofp;
{
	u_char mbuf[8];
	int s, tran, k[STRINGS], lastkey, slen;
	BSEQ *sp;
	MCMD m;

	slen = rp->len; //sequence length
	tran = LOWPOS(rp->lo, Key); /* lowest position on banjo */
	m.len = 3;
	m.cmd = mbuf;
	for (s = STRINGS; --s >= 0; k[s] = -1);
	for (sp = rp->seq; ; sp++) {
	    m.when = start + sp->start;
	    m.cmd[2] = 0;
	    for (s = 0; s < STRINGS; s++) {
		if (k[s] >= 0) {
		    m.cmd[0] = CH_KEY_ON | Chan[s];
		    m.cmd[1] = tran + k[s];
		    putmcmd(ofp, &m);
		}
	    }
	    if (m.when >= start + slen)
		break;
	    m.cmd[2] = vel;
	    for (s = 0; s < STRINGS; s++) {
		k[s] = sp->keys[s];
		if (k[s] >= 0) {
		    m.cmd[0] = CH_KEY_ON | Chan[s];
		    m.cmd[1] = tran + k[s];
		    putmcmd(ofp, &m);
		    lastkey = m.cmd[1];
		}
	    }
	}
	return(lastkey);
}
