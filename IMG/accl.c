/*
**	ACCL -- Automated Chord Chart Lead Player
**	psl 1/89
*/
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include	<stdio.h>

#include "key2name.h"
#include "myatoi.h"
#include "mozart.h"

#define	BANJO		/* for #ifdef in libamc.h */
#include	<midi.h>
#include	<notedefs.h>
#include "banjotune.h"

// #define	BOOGIDUMP /* def this to dump all the boogi patterns and exit */

#define	MAXCH		512
#define	NCHANS		16
#define	NKEYS		128
#define	NAMELEN		20
#define	STYLELEN	6
#define	GRPLEN		8
		/* nearest() flags */
#define	UPOK	1
#define	DNOK	2
#define	OLDOK	4
#define	UPONLY	UPOK
#define	DNONLY	DNOK
#define	NEW	(UPOK|DNOK)
#define	ANY	(UPOK|DNOK|OLDOK)
		/* time values (in MPU clocks) */
#define	BREVE		(MPU_CLOCK_PERIOD<<2)
#define	WHOLE		(MPU_CLOCK_PERIOD<<1)
#define	HALF		(MPU_CLOCK_PERIOD)
#define	QUARTER		(MPU_CLOCK_PERIOD>>1)
#define	EIGHTH		(MPU_CLOCK_PERIOD>>2)
#define	SIXTEENTH	(MPU_CLOCK_PERIOD>>3)
#define	THIRTYSECOND	(MPU_CLOCK_PERIOD>>4)
#define	PR(PCNT)	(((rand()>>9)%100)<(PCNT)) /* "random" probability fcn */
#define	PR50		(rand()&01000)		/* 50-50 fcn */
#define	RNUM(RNG)	((rand()>>11)%(RNG))	/* random range 0..RNG-1 */
#define	PC(N)		(((N)+1200)%12)		/* Pitch class (mod 12) */
#define	DBG		if(Debug)fprintf
#define	DBG2		if(Debug>1)fprintf
#define	SAVHIST(hp)	{(hp)->k=Lastnotek;(hp)->d=Lastnoted;}

void	bebop(), grass(), boogi(), class(), march(), mozar(), seque(), swing();
void	toner();
char	*speel(), *strcopy(), oneof();
void procntl(char *cp, char *bp, long now);
void syntax(char *msg);
long snip(long start, long stop, int ocn, int ncn);
int wrdcmp(char *ap, char *bp);
long process(FILE *ifp, long now);
int find(char *name);
int nearest(int k, int tones[12], int flg);
void noteonoff(int k, long start, long stop);
void noteon(int c, int k, long when);
void noteoff(int c, int k, long when);
long putmcmdadagio(FILE *ofp, MCMD *mp, int noteon);


#define	DEFSTYLE	"class"
struct	stylstr	{		/* definitions of styles */
	char	*name;		/* style names (for arguments) */
	int	key;		/* standard chord key offset */
	void	(*comp)();	/* composition routine */
} S[]	= {
	{ "bebop", 0, bebop, },
	{ "grass", 7, grass, },
	{ "boogi", 0, boogi, },
	{ "class", 0, class, },
	{ "march", 0, march, },
	{ "mozar", 0, mozar, },
	{ "samba", 0, swing, },
	{ "seque", 0, seque, },
	{ "swing", 0, swing, },
	{ "toner", 0, toner, },
#ifndef	LINT
	{ 0, },
#endif
};

struct	chstr	{
	char	name[NAMELEN];	/* chord name */
	int	trans[12];	/* transpositions from standard chord */
	int	group;		/* standard chord group */
	int	ctone[12];	/* chord tones */
} Ch[MAXCH]	= {
	"-", { 0, }, 0, { 0, },	/* dummy to allow "-" to be used as a rest */
};

#define	T_VAMP	0
#define	T_TINY	1
#define	T_I_II	2
#define	T_I_IV	3
#define	T_I_V	4
#define	T_I_VI	5
#define	T_END	6

struct	typstr	{
	int	bars;
} Tpat[]	= {		/* transition pattern */
	2,			/* vamp */
	1,			/* tiny */
	1,			/* I-II */
	1,			/* I-IV */
	1,			/* I-V */
	1,			/* I-VI */
	2,			/* end */
};

int	Motion[12]	= {
	T_VAMP,	T_VAMP,	T_I_II,	T_VAMP,	T_I_II,	T_I_IV,
	T_VAMP,	T_I_V,	T_VAMP,	T_I_VI,	T_VAMP,	T_VAMP,
};
char	Part[128];			/* an arbitrary name? */
int	Cpq;				/* MPU clocks per quantum */
int	Ctones[4][10]	= {		/* chord notes in each chord group */
	{ 0, 4, 7, -1, },		/* (based on C) */
	{ 0, 4, 7, 10, -1, },
	{ 0, 4, 8, -1, },
	{ 0, 3, 6, 9, -1, },
};
int	Sctones[4][10]	= {		/* special chord notes for bebop */
	{ 0, 4, 7, -1, },		/* (based on C) */
	{ 0, 2, 4, 7, 9, 10, -1, },
	{ 0, 2, 4, 6, 8, 10, -1, },
	{ 0, 3, 6, 9, -1, },
};
int	Numchords	= 1;		/* "-" already defined */
int	Stones[12]	= {		/* derived from Key */
	1,0,1,0,1,1,0,1,0,1,0,1,
};
/* Data available to all styles */
int	Bars		= 0;		/* from args, to help plan */
int	Chan[16];			/* lead instrument channels */
int	Debug		= 0;		/* debugging output */
int	Energy		= 50;		/* how frenzied */
int	Key		= 0;		/* from args, to help plan */
int	Lastnotek, Lastnoted;		/* used by noteonoff & SAVHIST() */
long	Lstart		= 0;		/* time of current input line start */
int	Nchans		= 0;		/* how many output channels defined */
int	Predict		= 50;		/* how much repeating */
long	Pstart		= 0;		/* time of most recent "Part" line */
int	Seed		= 0;		/* random number seed */
int	Style;				/* index into S[] */
int	Vol[16]		= { 80, };	/* key velocities for output */

/* This struct contains all historical/continuity info */
/* it should contain enough to reproduce the output... */
typedef	struct	histr {
	int	k;			/* last note (pitch) played */
	int	d;			/* last pitch delta */
	int	s;			/* starting rand() seed */
} HIST;

HIST	Hist	= { C3, 0, 0, };

extern	MCMD	Mpu_nop;	/* MPU_NO_OP command */
// extern	char	*key2name();

main(argc, argv)
char	*argv[];
{
	register int i, j, files;
	char *cp;
	long now;
	FILE *ifp;

	Debug = (getenv("DEBUG") != NULL);
/****/setbuf(stderr, 0);
	files = 0;
	now = 0L;
	for (i = 1; i < 16; Chan[i++] = -1);	/* only 0 defaults to 1 (0) */
	for (Style = 0; S[Style].name; Style++)
	    if (strcmp(DEFSTYLE, S[Style].name) == 0)
		break;
	Seed = time(0) + getpid();
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		files++;
		continue;
	    }
	    switch (argv[i][1]) {
	    case 'b':
		Bars = atoi(&argv[i][2]);
		break;
	    case 'c':
		if ((j = atoi(&argv[i][2]) - 1) >= 0 && j < NCHANS) {
		    Chan[Nchans] = j;
		    for (cp = &argv[i][2]; *cp && *cp != '='; cp++);
		    if (*cp++ == '=') {
			j = atoi(cp);
			Vol[Nchans] = (j < 1? 1 : (j > 127? 127 : j));
		    } else
			Vol[Nchans] = 80;
		    Nchans++;
		}
		break;
	    case 'd':
		Debug++;
		break;
	    case 'e':
		Energy = atoi(&argv[i][2]);
		break;
	    case 'k':
		Key = pc2key(&argv[i][2]);
		for (j = 12; --j >= 0; )
		    Stones[PC(Key+j)] = (j!=1&&j!=3&&j!=6&&j!=8&&j!=10);
		break;
	    case 'p':
		Predict = atoi(&argv[i][2]);
		break;
	    case 's':
		srand(Seed = atoi(&argv[i][2]));
		break;
	    default:
		fprintf(stderr, "Usage: %s [options] [files or stdin]\n",
		 argv[0]);
		fprintf(stderr, "-b# gives bar count (for planning)\n");
		fprintf(stderr, "-c# adds an output channel [1..16]\n");
		fprintf(stderr, "-c#=# also sets key velocity [1..127]\n");
		fprintf(stderr, "-d turns on debugging info\n");
		fprintf(stderr, "-e# sets solo \"energy\" [0..100]\n");
		fprintf(stderr, "-kKEY sets solo key\n");
		fprintf(stderr,
		 "-p# sets solo \"predictability\" [0..100]\n");
		fprintf(stderr, "-s# sets the random number seed\n");
		exit(2);
	    }
	}
#ifdef	BOOGIDUMP
	if (Nchans == 0)
	    Nchans++;
	boogi(0L, 0L, 0, 0);
#endif	// BOOGIDUMP
	if (files) {
	    for (i = 1; i < argc; i++) {
		if (argv[i][0] != '-') {
		    if ((ifp = fopen(argv[i], "r")) == NULL)
			perror(argv[i]);
		    else {
			now = process(ifp, now);
			fclose(ifp);
		    }
		}
	    }
	} else
	    now = process(stdin, now);
	exit(0);
}

long process(ifp, now)
FILE	*ifp;
long	now;
{
	char *bp, *cp;
	int ncn, ocn;		/* new & old chord numbers */
	long nlstart;		/* next line start time (MPU clocks) */
	long start;		/* when ocn started */
	char buf[512];
	HIST hsave;		/* saved to repeat a section */

DBG(stderr, "process(%p, %ld) seed=%d\n", ifp, now, Seed);
	srand(Hist.s = rand());
	Hist.k = C2 + RNUM(25);
	hsave = Hist;
	ocn = -1;
	start = Lstart = Pstart = 0L;
	for (; fgets(buf, sizeof buf, ifp) != NULL; ) {
	    for (bp = buf; *bp && *bp <= ' '; bp++);
	    cp = speel(&bp, 0);
	    if (*cp == '\n' || (cp[0] == '#' && cp[1] == '\0'))
		;
	    else if (*cp == '#')
		procntl(cp, bp, now);
	    else {
DBG(stderr, "input: %s %s", cp, bp);
		for (nlstart = now; *cp; now += Cpq) {
		    if (*cp == '/') {
			if ((ncn = ocn) == -1)
			    syntax("First line begins with '/'");
		    } else {
			if ((ncn = find(cp)) < 0)
			    syntax(cp);
		    }
		    if (ncn != ocn || nlstart == now) {
			if (now > start)
			    start = snip(start, now, ocn, ncn);
			ocn = ncn;
			Lstart = nlstart;
		    }
		    if (now == nlstart) {	/* first chord in line next */
			srand(now + rand());	/* avoid getting stuck! */
			if (now && PR(Predict)) { /* repeat last solo line? */
DBG(stderr, "restore params to encourage repeat; seed=%d.\n", Hist.s);
			    Hist = hsave;
			    srand(Hist.s);
			} else {
			    srand(Hist.s = rand());
			    hsave = Hist;
DBG(stderr, "save params to allow repeat; seed=%d.\n", Hist.s);
			}
		    }
		    cp = speel(&bp, 0);
		}
	    }
	}
	snip(start, now, ocn, -1);
	return(now);
}

void procntl(cp, bp, now)
char	*cp, *bp;
long	now;
{
	int cn, i, j, *ctp;
	FILE *fp;

	if (wrdcmp(cp, "#CHORD") == 0) {
	    cp = speel(&bp, 0);
	    printf("found at %d\n", find(cp));
	    if ((cn = find(cp)) == -1 && (cn = Numchords++) >= MAXCH)
		syntax("Too many chords defined");
	    printf("cn: %d\n", cn);
	    strcopy(Ch[cn].name, cp);
	    cp = speel(&bp, 0);
		
	    Ch[cn].group = myatoi(cp);
	    for (i = 0; i < 12; i++) {
		Ch[cn].trans[i] = myatoi(bp);
		while (*bp && *bp++ != ',');
		Ch[cn].ctone[i] = 0;
		printf("trans %d: %d\n", i, Ch[cn].trans[i]);
	    }
	    for (ctp = Ctones[Ch[cn].group - 1]; *ctp >= 0; ctp++) {
		i = PC(*ctp + S[Style].key);
		Ch[cn].ctone[PC(i + Ch[cn].trans[i])]++;
		printf("ctone[%d] = %d\n", PC(i + Ch[cn].trans[i]), Ch[cn].ctone[PC(i + Ch[cn].trans[i])]);
	    }

	    for (i = 0; i < Numchords; i++)
	        printf("Chord: %s\n", Ch[i].name);
	} else if (wrdcmp(cp, "#INCLUDE") == 0) {
	    if (*bp++ != '"')
		syntax("#INCLUDE must use quotes (\")");
	    for (cp = bp; *cp && *cp != '"'; cp++);
	    if (*cp != '"')
		syntax("#INCLUDE must use quotes (\")");
	    *cp = '\0';
	    if ((fp = fopen(bp, "r")) == NULL) {
		perror(bp);
		exit(1);
	    }
	    now = process(fp, now);
	    fclose(fp);
	} else if (wrdcmp(cp, "#PART") == 0) {
	    Pstart = now;
DBG(stderr, "Pstart=%ld\n", Pstart);
	    strcopy(Part, speel(&bp, 0));
	} else if (wrdcmp(cp, "#QUANT") == 0) {
	    if (wrdcmp(bp, "whole") == 0)
		i = 1;
	    else if (wrdcmp(bp, "half") == 0)
		i = 2;
	    else if (wrdcmp(bp, "quarter") == 0)
		i = 4;
	    else if (wrdcmp(bp, "eighth") == 0)
		i = 8;
	    else if (wrdcmp(bp, "sixteenth") == 0)
		i = 16;
	    else if ('0' <= *bp && *bp <= '9')
		i = atoi(bp);
	    else
		syntax("Unrecognized #QUANT arg");
	    Cpq = WHOLE / i;
	} else if (wrdcmp(cp, "#STYLE") == 0) {
	    cp = speel(&bp, 0);
	    if (strlen(cp) >= STYLELEN)
		cp[STYLELEN - 1] = '\0';
	    for (i = 0; S[i].name && strcmp(S[i].name, cp); i++);
	    if (!S[i].name)
		syntax("Unrecognized Style");
	    Style = i;
	    if (strcmp(cp, "bebop") == 0)
		for (i = 4; --i >= 0; )
		    for (j = 0; Sctones[i][j] >= 0; j++)
			Ctones[i][j] = Sctones[i][j];
	} else {
DBG(stderr, "Unrecognized control: %s %s\n", cp, bp);
	    ;
	}
}

long snip(start, stop, ocn, ncn)
long	start, stop;
{
	int nct, i;
	long ctime, vstop;

DBG(stderr, "snip(Lstart=%ld Pstart=%ld start=%ld stop=%ld ocn=%d %s ncn=%d %s)\n",
 Lstart, Pstart, start, stop,
 ocn, ocn>=0? Ch[ocn].name : "", ncn, ncn>=0? Ch[ncn].name : "");
	if (start >= stop)
	    return(stop);
	if (ncn == -1)
	    nct = T_END;
	else if (stop - start <= QUARTER)	/* tiny piece */
	    nct = T_TINY;
	else if (Ch[ocn].name[0] == '-')	/* treat rests as vamp */
	    nct = T_VAMP;
	else
	    nct = Motion[PC(Ch[ncn].trans[0] - Ch[ocn].trans[0])];
	if (nct == T_VAMP) {
	    vstop = stop;
	} else {
	    i = Tpat[nct].bars * WHOLE;
	    vstop = Lstart + i * ((stop - Lstart - 1) / i);
	}
DBG(stderr, " start=%ld vstop=%ld stop=%ld\n", start, vstop, stop);
	ctime = start;
	if (ctime < vstop) {
#ifndef LINT
if(Debug) { int i;
DBG(stderr,"%s(%ld, %ld, ocn(%d), ocn(%d))  ",
 S[Style].name, ctime, vstop, ocn, ocn);
DBG(stderr,"o:");for(i=0;i<12;fprintf(stderr,"%d",Ch[ocn].ctone[i++]));
DBG(stderr,"  S:");for(i=0;i<12;fprintf(stderr,"%d",Stones[i++]));
DBG(stderr, "\n");
}
#endif
	    (*S[Style].comp)(ctime, vstop, ocn, ocn);
	    ctime = vstop;
	}
	if (ctime < stop) {
#ifndef	LINT
if(Debug) { int i;
DBG(stderr,"%s(%ld, %ld, ocn(%d), ncn(%d))  ",
 S[Style].name, ctime, stop, ocn, ncn);
DBG(stderr,"o:");for(i=0;i<12;fprintf(stderr,"%d",Ch[ocn].ctone[i++]));
if (ncn >= 0)
 DBG(stderr,"  n:");for(i=0;i<12;fprintf(stderr,"%d",Ch[ncn].ctone[i++]));
DBG(stderr,"  S:");for(i=0;i<12;fprintf(stderr,"%d",Stones[i++]));
DBG(stderr, "\n");
}
#endif
	    (*S[Style].comp)(ctime, stop, ocn, ncn);
	}
	return(stop);
}

void
bebop(start, stop, ocn, ncn)
long	start, stop;
{
	register int i, dk, dur, dir;
	int nk, nk2, nk3, needforce, *tp, *otones, *ntones, energy;
	int mindur, nnotes;
	long lt, lt2;
	static int force;	/* key that needs to be played (not a rest) */
	static int depth;	/* recursion depth */
	static int swung;	/* avoid multiple swung subdivisions */
	static int onnotes = 8;	/* a little history */

DBG(stderr, "   bebop(%ld, %ld) depth=%d, swung=%d\n", start, stop, depth, swung);
	if (depth++ == 0)
	    swung = 999;
	energy = Ch[ocn].name[0] == '-'? 100 : Energy;	/* go nuts in a rest */
	needforce = 0;
	otones = Ch[ocn].ctone;
	ntones = Ch[ncn].ctone;
	if (!force && (Hist.k <= C1 || Hist.k >= C5)) {
DBG(stderr, "== Hist.k=%d Hist.d=%d\n", Hist.k, Hist.d);
	    Hist.k = G2 + (Key + 5) % 12;
	    Hist.d = 0;
	    if (stop - start > QUARTER)
		start = (start + stop) / 2;
	}
	if (ncn < 0) {					/* ending */
DBG(stderr, " ending note\n");
	    if (force) {
		Hist.k = force;
		Hist.d = 0;
DBG(stderr, "  trying to force %d\n", force);
	    }
	    nk = nearest(Hist.k + Hist.d / 2, otones, ANY);
	    lt = (stop - start) >= WHOLE? stop - HALF : stop;
	    noteonoff(nk, start, lt);			/* chord */
	    SAVHIST(&Hist);
	    --depth;
	    return;
	}
	mindur = ((20 + rand() % 83) * (200 - energy)) / (energy + 100);
	dur = stop - start;
	nnotes = 1 + dur / mindur;
	nnotes = (RNUM(nnotes) + 1 + onnotes) >> 1;
	onnotes = nnotes;
DBG(stderr, " dur=%d, mindur=%d, nnotes = %d\n", dur, mindur, nnotes);
	if (nnotes > 4) {
	    for (i = 15; i + i < dur; i <<= 1);	/* find biggest piece */
	    bebop(start, start + i, ocn, ocn);
	    bebop(start + i, stop, ocn, ocn);
	    --depth;
	    return;
	}
	dir = Hist.d? (Hist.d > 0 ? UPONLY : DNONLY) : NEW;
	dir = Hist.k <= C2? UPONLY : (Hist.k >= 72? DNONLY : dir);
	dur = stop - start;
						/*=== 1 note possibilities ===*/
	if (nnotes == 1) {
DBG(stderr, " one note\n");
	    if (force) {
		nk = force;
DBG(stderr, " forced; nk=%d\n", nk);
	    } else {
		nk = nearest(Hist.k, otones, NEW);	/* look for pivot */
		if (ntones[PC(nk)]) {
DBG(stderr, "  pivot nk=%d\n", nk);
		} else {
		    nk2 = nearest(nk, ntones, ANY);
		    if (otones[PC(nk2)] > 0) {
			i = nk2 - Hist.k;
			if (Hist.d * i >= 0
			 && -2 < (i - Hist.d) && (i - Hist.d) < 2) {
DBG(stderr, "  pivot? nk=%d, nk2=%d\n", nk, nk2);
			    nk = nk2;			/* found one */
			}
		    }
		}
	    }
	    lt = stop - (dur >= HALF? EIGHTH : 0);
	    noteonoff(nk, start, lt);			/* chord */
						/*=== 2 note possibilities ===*/
	} else if (nnotes == 2) {
	    if (swung < depth) {		/* already swung above us */
		lt = (start + stop) / 2;
DBG(stderr, " straight two-way (depth=%d swung=%d)\n", depth, swung);
	    } else if (swung == depth		/* already swung @ this depth */
	     || (dur >= 3 * mindur && PR(67))) {
		lt = (start + stop + stop) / 3;
DBG(stderr, " swung two-way (depth=%d swung=%d)\n", depth, swung);
		swung = depth;
	    } else {
		lt = (start + stop) / 2;
DBG(stderr, " straight two-way\n");
	    }
	    if (PR50) {					/* recurse */
DBG(stderr, "  recursion\n");
		if (force || PR(67))
		    bebop(start, lt, ocn, ocn);
		else {
DBG(stderr, "   (with a rest)\n");
		}
		bebop(lt, stop, ocn, ncn);
	    } else {					/* two note split */
		nk = force? force : nearest(Hist.k, Stones, dir);
		nk2 = nearest(nk, ntones, dir);
		dk = nk2 - nk;
DBG(stderr, "   nk=%d, nk2=%d\n", nk, nk2);
		if (dk == 1 || dk == -1) {	/* nk2 is 1 step from nk */
		    noteonoff(nk, start, lt);		/* scale */
		    nk3 = nearest(nk2, Stones, dir);	/* one step beyond */
		    noteonoff(Hist.k = nk3, lt, stop);	/* scale */
		    needforce = nk2;			/* not critical */
		} else if (dk == 2 || dk == -2) { /* nk2 is 2 steps from nk */
		    noteonoff(nk, start, lt);		/* scale */
		    if (stop - lt > EIGHTH) {
			nk3 = nearest(nk2, Stones, dir); /* one step beyond */
			noteonoff(Hist.k = nk3, lt, stop); /* scale */
		    } else
			noteonoff(nk2 - dk / 2, lt, stop); /* chromatic */
		    needforce = nk2;
		} else if (dk == 3 || dk == -3) {	/* nk2 is 3|4|5 steps */
		    noteonoff(nk, start, lt);		/* scale */
		    nk3 = nk2 - dk / 3;
		    noteonoff(nk3, lt, stop);		/* whole step */
		    if (nk3 != nk2)
			needforce = nk2;		/* not critical */
		} else if (dk && -5 <= dk && dk <= 5) {	/* nk2 is 4|5 steps */
		    noteonoff(nk, start, lt);		/* scale */
		    nk3 = nearest(nk, Stones, dir);
		    noteonoff(nk3, lt, stop);		/* scale */
		    if (nk3 != nk2)
			needforce = nk2;		/* not critical */
		} else {				/* who knows? */
		    noteonoff(nk, start, lt);		/* scale */
		    nk = nearest(nk, Stones, dir);
		    noteonoff(nk, lt, stop);		/* scale */
		}
	    }
						/*=== 3 note possibilities ===*/
	} else if (nnotes == 3) {
	    if (swung < depth) {		/* already swung above us */
		lt = (start + start + start + stop) / 4;
		lt2 = (start + stop) / 2;	/* quarters & half */
DBG(stderr, " 1/4, 1/4, 1/2  three-way (depth=%d swung=%d)\n", depth, swung);
	    } else if (swung == depth		/* already swung @ this depth */
	     || (dur >= 3 * mindur && PR(50))) {
		lt = (start + stop + stop) / 3;
		lt2 = (start + stop + stop) / 3;
DBG(stderr, " swung three-way (depth=%d swung=%d)\n", depth, swung);
		swung = depth;
	    } else {				/* 50% half & quarters */
		lt = (start + stop) / 2;
		lt2 = (start + stop + stop + stop) / 4;
DBG(stderr, " 1/2, 1/4, 1/4  three-way (depth=%d swung=%d)\n", depth, swung);
	    }
	    if (PR50) {					/* recurse */
DBG(stderr, "  recursion\n");
		bebop(start, lt, ocn, ocn);
		if (PR(67))
		    bebop(lt, lt2, ocn, ocn);
		else {
DBG(stderr, "   (with a rest)\n");
		}
		bebop(lt2, stop, ocn, ncn);
	    } else {
		nk = force? force : nearest(Hist.k, otones, dir);
		noteonoff(nk, start, lt);			/* chord */
		dk = (nk < (Gb2 + RNUM(13)))? UPONLY : DNONLY;
DBG(stderr, "   Hist.k=%d, Hist.d=%d, nk=%d, dk=%d\n", Hist.k, Hist.d, nk, dk);
		if (PR(25 + energy / 2)) {			/* rest? */
		    nk = nearest(nk, Stones, dk);
		    noteonoff(nk, lt, lt2);			/* scale */
		}
		nk2 = nearest(nk, ntones, dk);
		dk = nk2 - nk;
		if ((dk == -2 || dk == 2) && stop - lt2 < EIGHTH)
		    nk3 = nk2 - dk / 2;				/* chromatic */
		else if ((dk == -3 || dk == 3) && stop - lt2 < EIGHTH)
		    nk3 = nk2 - dk / 3;				/* whole step */
		else if (stop - lt2 < QUARTER)
		    nk3 = nearest(nk, Stones, dk);		/* scale step */
		else
		    nk3 = nk2;					/* chord */
		noteonoff(nk3, lt2, stop);
		if (nk3 != nk2)
		    needforce = nk2;
	    }

						/*=== 4 note possibilities ===*/
	} else if (nnotes == 4) {
DBG(stderr, " even four-way\n");
	    lt = (start + start + start + stop) / 4;	/* even split */
	    dk = (Hist.k < (Ab2 + RNUM(9)))? UPONLY : DNONLY;
	    tp = (48 < Hist.k && Hist.k < 72 && dur < HALF)? Stones : otones;
DBG(stderr, "   Hist.k=%d, Hist.d=%d, dk=%d\n", Hist.k, Hist.d, dk);
	    if (!force && PR(50 - energy))		/* possible rest */
		nk = Hist.k;
	    else {
		nk = force? force : nearest(Hist.k, tp, dk);
		noteonoff(nk, start, lt);		/* scale or chord */
	    }
	    lt2 = (start + stop) / 2;
	    nk = nearest(nk, tp, dk);
	    noteonoff(nk, lt, lt2);			/* scale or chord */
	    lt = (start + stop + stop + stop) / 4;
	    if (!PR(50 - energy / 3)) {			/* possible rest */
		nk = nearest(nk, tp, dk);
		noteonoff(nk, lt2, lt);			/* scale or chord */
	    }
	    nk3 = nearest(nk, otones, NEW);
	    noteonoff(nk3, lt, stop);			/* chord */
	}
	force = needforce;
	SAVHIST(&Hist);
	--depth;
}

void
grass(start, stop, ocn, ncn)
long	start, stop;
{
	int i, *ntones;
	static int init;
	static int lastml = 1;	/* last modified lh location */
	static int etones[12] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1, };
	static long lastmt = 0;	/* time of last lh loc modification */

	if (!init) {
	    i = 0;
	    for (init = 1; init < 5; init++)
		if (Chan[init] < 0)
		    Chan[init] = Chan[i++];
	    banjotune(Key, Stones, Chan);
	}
	ntones = (ncn == -1)? etones : Ch[ncn].ctone;
	if (start > lastmt + 8 * WHOLE) {	/* been a while */
	    if (start == Pstart			/* new "part" */
	     && (lastml < 5) ^ (Last.lhloc > 5)) {	/* !! */
DBG(stderr, "grass(%d,%d,%d,%d) lastml=%d\n", start, stop, ocn, ncn, lastml);
		Last.lhloc = lastml = lastml < 5? 12 : 1;
		lastmt = start;
	    }
	}
	banjoplay(start, stop, Ch[ocn].ctone, ntones, Vol[0], stdout);
}

/* BOOGI data */
#define	RST	0,999
#define	END	999,0,999
#define	CNT	0
#define	REL	999
#define	_w	480
#define	_h	240
#define	ht	160
#define	_q	120
#define	qt	80
#define	_e	60
#define	et	40
#define	_s	30
#define	st	20
#define	tt	10
int	Bw0[]	= {
	C4,64,et, RST,ht, E3,64,CNT, G3,64,CNT, A3,64,CNT, C4,64,et,
	RST,qt, Eb3,64,et, E3,64,et, G3,64,et, A3,64,et, END, };
int	Bw1[]	= {
	Eb3,64,CNT, Gb3,64,qt, E3,64,CNT, G3,64,et, C3,64,qt,
	Eb3,64,CNT, Gb3,64,et, E3,64,CNT, G3,64,qt, C3,64,et, END, };
int	Bw2[]	= {
	C4,80,CNT, E4,80,et, RST,qt, A3,64,qt, G3,64,et, D4,80,CNT, F4,80,et,
	RST,qt, A3,64,qt, G3,64,et, C4,80,CNT, E4,80,qt, G3,64,et, A3,64,qt,
	C4,64,et, Eb4,80,CNT, G4,80,qt, D4,64,CNT, F4,64,et, C4,64,CNT,
	Eb4,64,qt, A3,64,CNT, C4,64,et, END, };
int	Bw3[]	= {
	E3,64,qt, C4,80,et, RST,_q, C4,72,CNT, A3,69,CNT, G3,67,CNT, E3,64,et,
	RST,_h-et, C4,72,CNT, A3,69,CNT, G3,67,CNT, E3,64,et, RST,ht, C4,72,CNT,
	A3,69,CNT, G3,67,CNT, E3,64,et, RST,_e, C4,72,st, A3,69,st, G3,67,st,
	Eb3,80,_q, END, };
int	Bw4[]	= {
	RST,qt, C3,64,et, Eb3,64,CNT, G3,64,_q, F3,64,CNT, A3,64,_q,
	Eb3,64,CNT, G3,64,_q, END, };
int	Bw5[]	= {
	G3,80,CNT, Eb4,80,_w, E3,64,CNT, C4,64,et, RST,_w-qt,
	E3,64,CNT, C4,64,et, G3,80,CNT, E4,80,qt, F3,80,CNT, D4,80,et,
	E3,80,CNT, C4,80,qt, C3,80,CNT, A3,80,et, Bb2,80,CNT, G3,80,_q,
	A2,80,CNT, F3,80,qt, Gb2,80,CNT, Eb3,80,et, G2,80,CNT, E3,80,_h,
	RST,_h, END, };
int	Bw6[]	= {
	Bb4,64,et, B4,64,et, C5,64,et, END, };
int	Bw7[]	= {
	E4,64,CNT, G4,64,CNT, Bb4,64,et, C4,64,et,
	E4,64,CNT, G4,64,CNT, Bb4,64,et, C4,64,et, RST,qt, END, };
int	Bw8[]	= {
	RST,qt, C4,64,_s, Eb4,48,CNT, Gb4,48,tt,
	E4,64,CNT, G4,64,qt-tt, Eb4,48,CNT, Gb4,48,tt, E4,64,CNT, G4,64,et,
	RST,_h, END, };
int	Bw9[]	= {
	C3,64,CNT, G3,64,CNT, Bb3,64,_h, RST,qt, C3,64,et, F3,64,CNT, A3,64,qt,
	C3,64,CNT, E3,64,CNT, G3,64,ht, RST,qt, C3,64,et, E3,64,CNT, G3,64,_q,
	F3,64,CNT, A3,64,_q, END, };
int	Bw10[]	= {
	C3,72,CNT, G3,72,qt, C3,72,CNT, G3,72,et, Eb3,64,qt, E3,64,et,
	C3,72,CNT, G3,72,qt, C3,72,CNT, G3,72,et, F3,64,qt, E3,64,et, END, };
int	Bw11[]	= {
	E3,72,CNT, C4,72,qt, E3,72,CNT, C4,72,et, G3,64,qt, A3,64,et,
	E3,72,CNT, C4,72,qt, E3,72,CNT, C4,72,et, A3,64,qt, G3,64,et, END, };
int	Bw12[]	= {
	Eb4,80,tt, E4,64,CNT, C5,64,_e-tt, E4,64,CNT, C5,64,_e,
	E4,64,CNT, C5,64,_e, Eb4,80,tt, E4,64,CNT, C5,64,_e-tt,
	E4,64,CNT, C5,64,_e, E4,64,CNT, C5,64,_e,
	Eb4,80,tt, E4,64,CNT, C5,64,_e-tt, E4,64,CNT, C5,64,_e, END, };
int	Bw13[]	= {
	E4,80,CNT, C5,80,_e, E4,64,CNT, C5,64,_e, E4,64,CNT, C5,64,_e, END, };
int	Bw14[]	= {
	E3,72,et, G3,64,CNT, C4,64,et, END, };
int	Bw15[]	= {
	E3,72,CNT, G3,72,et, RST,et, E3,64,CNT, G3,64,et,
	F3,72,CNT, A3,72,et, RST,et, F3,64,CNT, A3,64,et, END, };
int	Bw16[]	= {
	E3,56,st, C4,56,st, END, };
int	Bw17[]	= {
	RST,_q, E3,40,tt, F3,40,tt, G3,40,tt, A3,40,tt, B3,40,tt,
	C4,40,tt, D4,40,tt, E4,40,tt, F4,40,tt, G4,40,tt, A4,40,tt, B4,40,tt,
	C5,72,_q, E5,64,CNT, G5,56,_s, RST,_q-_s, END, };
int	Bw18[]	= {
	C4,64,qt, A3,64,st, G3,64,st, Eb3,64,qt, E3,64,et,
	RST,qt, G3,64,et, A3,64,qt, G3,64,et,
	C4,64,qt, A3,64,st, G3,64,st, Eb3,64,qt, E3,64,et,
	RST,qt, G3,64,et, A3,64,qt, G3,64,et,
	C4,80,et, RST,et, C4,64,st, A3,64,tt, G3,64,tt, Eb3,80,qt, E3,64,et,
	C4,80,et, RST,et, C4,64,st, A3,64,tt, G3,64,tt, Eb3,80,qt, E3,64,et,
	C4,80,et, RST,et, C4,64,st, A3,64,tt, G3,64,tt, Eb3,80,qt, E3,64,et,
	RST,qt, G3,64,et, G3,64,et, A3,64,et, G3,64,et,
	END, };
int	Bw19[]	= {
	C4,80,et, RST,et, C4,64,st, A3,64,tt, G3,64,tt, Eb3,80,qt, E3,64,et,
	END, };
int	Bw20[]	= {
	C4,64,CNT, Eb4,72,CNT, G4,64,qt, G4,64,st, E4,72,tt, C4,64,tt,
	F4,72,CNT, A4,64,et, RST,et, Eb4,64,et,
	E4,72,CNT, G4,64,qt, C4,64,et,
	RST,qt, C4,64,et,
	END, };
int	Bw21[]	= {
	D5,64,CNT, Eb5,56,et, Eb5,72,st, D5,68,st, C5,64,st, RST,st,
	A4,64,CNT, Bb4,56,et, Bb4,72,st, A4,68,st, G4,64,st, RST,st,
	D4,64,CNT, Eb4,56,et, Eb4,72,st, D4,68,st, C4,64,st, RST,st,
	A3,64,CNT, Bb3,56,et, Bb3,72,st, A3,68,st, G3,64,st, RST,st,
	END, };
int	Bw22[]	= {
	Eb4,72,CNT, G4,64,et, E4,72,CNT, G4,48,et, F4,72,CNT, A4,68,et,
	END, };

int	Bwle0[]	= {					/* 2-bar ending */
	Eb3,64,tt, E3,64,CNT, G3,64,CNT, Bb3,64,_q-tt,
	Eb3,64,CNT, F3,64,CNT, A3,64,et, RST,qt, Eb3,64,tt,
	E3,64,CNT, G3,64,CNT, Bb3,64,_q-tt, Eb3,64,CNT, F3,64,CNT, A3,64,et,
	RST,qt-tt, Eb3,64,CNT, Gb3,64,tt,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,_q, RST,_q, END, };
int	Bwle1[]	= {					/* 2-bar ending */
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, Eb3,64,CNT, G3,64,CNT, A3,64,et, RST,et,
	C3,64,CNT, Eb3,64,CNT, G3,64,CNT, A3,64,et, RST,et,
	C3,64,CNT, Eb3,64,CNT, G3,64,CNT, A3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, C4,96,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,96,CNT, C4,64,et, RST,et,
	C3,64,CNT, E3,96,CNT, G3,64,CNT, C4,64,et, RST,et,
	C3,96,CNT, E3,64,CNT, G3,64,CNT, C4,64,_q, RST,_q, END, };
int	Bwle2[]	= {					/* 2-bar ending */
	C3,80,CNT, C4,80,et, Db3,80,CNT, Db4,80,et, D3,80,CNT, D4,80,et,
	Eb3,80,CNT, Eb4,80,et, E3,80,CNT, E4,80,et, F3,80,CNT, F4,80,et,
	Gb3,80,CNT, Gb4,80,et, G3,80,CNT, G4,80,et, Ab3,80,CNT, Ab4,80,et,
	A3,80,CNT, A4,80,et, Bb3,80,CNT, Bb4,80,et, B3,80,CNT, B4,80,et,
	C4,96,et, RST,et,
	G3,96,CNT, C4,64,et, RST,et,
	E3,96,CNT, G3,64,CNT, C4,64,et, RST,et,
	C3,96,CNT, E3,64,CNT, G3,64,CNT, C4,64,_q, RST,_q, END, };
int	Bwle3[]	= {					/* 2-bar ending */
	C3,80,CNT, Bb4,80,CNT, D5,80,_s, G3,80,CNT, E4,80,CNT, G4,80,_s,
	C4,80,CNT, Bb4,80,CNT, D5,80,_s, C3,80,CNT, E4,80,CNT, G4,80,_s,
	G3,80,CNT, Bb4,80,CNT, D5,80,_s, C4,80,CNT, E4,80,CNT, G4,80,_s,
	C3,80,CNT, Bb4,80,CNT, D5,80,_s, G3,80,CNT, E4,80,CNT, G4,80,_s,
	C4,80,CNT, Bb4,80,CNT, D5,80,_s, C3,80,CNT, E4,80,CNT, G4,80,_s,
	G3,80,CNT, Bb4,80,CNT, D5,80,_s, C4,80,CNT, E4,80,CNT, G4,80,_s,
	C3,80,CNT, Bb4,80,CNT, D5,80,_s, G3,80,CNT, E4,80,CNT, G4,80,_s,
	C4,80,CNT, Bb4,80,CNT, D5,80,_s, C3,80,CNT, E4,80,CNT, G4,80,_s,
	C3,80,CNT, E4,80,CNT, G4,80,CNT, Bb4,80,CNT, D5,80,et, RST,et,
	C3,80,CNT, E4,80,CNT, G4,80,CNT, Bb4,80,CNT, D5,80,et, RST,et,
	C3,80,CNT, E4,80,CNT, G4,80,CNT, Bb4,80,CNT, D5,80,et, RST,et,
	C3,80,CNT, E4,80,CNT, G4,80,CNT, Bb4,80,CNT, D5,80,_q, RST,_q, END, };
int	Bwse[]	= {					/* 1-bar ending */
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, RST,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,_q, RST,_q, END, };
int	Bwte[]	= {					/* tiny? ending */
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et,
	C3,64,CNT, E3,64,CNT, G3,64,CNT, Bb3,64,et, C2,80,et, END, };
#undef	_w
#undef	_h
#undef	ht
#undef	_q
#undef	qt
#undef	_e
#undef	et
#undef	_s
#undef	st
#undef	tt
typedef	struct	bwsstr	{
	int	*seq;		/* (key,vel,dur) tuples */
	int	len;		/* pattern length (MPU clocks) */
	int	tpr;		/* % chance that trn is used */
	int	sip;		/* % chance the seq is not repeated at BREVE */
	int	trn;		/* REL => relative trans, else absolute */
	long	use;		/* last use time */
} BWSSTR;
BWSSTR Bws[]	= {
	Bw0,	480,	 0,  0, 0,   -99999,
	Bw1,	360,	 0, 10, 0,   -99999,
	Bw2,	960,	 0,  0, 0,   -99999,
	Bw3,	960,	 0,  0, 0,   -99999,
	Bw4,	480,	 0, 10, 0,   -99999,
	Bw5,	1920,	 0, 50, 0,   -99999,
	Bw6,	120,	60, 10, REL, 0,	/* zero to avoid starting with it */
	Bw7,	240,	50, 20, REL, 0,
	Bw8,	480,	99, 20, 3,   0,
	Bw9,	960,	67,  0, REL, -99999,
	Bw10,	480,	33, 10, REL, -99999,
	Bw11,	480,	 0, 10, 0,   -99999,
	Bw12,	480,	25, 30, REL, -99999,
	Bw13,	180,	25, 20, 3,   -99999,
	Bw14,	80,	25, 10, REL, -99999,
	Bw15,	240,	50, 15, REL, -99999,
	Bw16,	40,	33, 10, REL, -99999,
	Bw17,	480,	67, 67, REL, -99999,
	Bw18,	1920,	25, 15, REL, -99999,
	Bw19,	240,	33, 10, REL, -99999,
	Bw20,	480,	20, 25, REL, -99999,
	Bw21,	480,	50, 25, REL, -99999,
	Bw22,	120,	50, 25, REL, -99999,
};
#define	NBWS	((sizeof Bws) / (sizeof Bws[0]))
BWSSTR Bwe[]	= {
	Bwle0,	960,	100, 0, REL, 0,		/* 2-bar */
	Bwle1,	960,	100, 0, REL, 0,		/* 2-bar */
	Bwle2,	960,	100, 0, REL, 0,		/* 2-bar */
	Bwle3,	960,	100, 0, REL, 0,		/* 2-bar */
	Bwse,	480,	100, 0, REL, 0,		/* 1-bar */
	Bwte,	480,	100, 0, REL, 0,		/* small change */
};

void
boogi(start, stop, ocn, ncn)
long	start, stop;
{
	u_char ubuf[4];
	int otones[12], keyson[16], nkeyson, k, v, d, j, trn, contin;
	MCMD m;
	static int tones7[12] = {1,0,1,0,1,1,0,1,0,1,1,0,};
	static int *dp, lastseq = -1, curseq = -1;
	static int *dp0, residue = 0;
	static long lsstart = -1L;
	static BWSSTR *sp;
#ifdef	BOOGIDUMP
	static bdump = 0;
#endif	// BOOGIDUMP

DBG(stderr, "boogi(%ld,%ld,%d,%d) residue=%d\n", start, stop, ocn, ncn, residue);
	m.cmd = ubuf;
	m.len = 3;
#ifdef	BOOGIDUMP
	if (!bdump) {
	    bdump++;
	    fprintf(stderr, "%d TCWME-delimited sequences, 4 measures each\n",
	     NBWS);
	    start = residue = Pstart = 0;
	    m.len = 1;
	    m.cmd[0] = MPU_TCWME;
	    for (k = 0; k < NBWS; k++) {
		d = (k + 1) * 4 * WHOLE;
		for (j = 0; start < d; j++) {
		    sp = &Bws[k];
		    dp0 = dp = sp->seq;
		    boogi(lsstart = start, stop = start + sp->len, 0, 0);
		    m.when = start = stop;
		    putmcmd(stdout, &m);		/* tcwme */
		}
		fprintf(stderr, "Dumped %d rep%s of Bws[%d]\n",
		 j, j==1? "" : "s", k);
	    }
	    exit(0);
	}
#endif	// BOOGIDUMP
	if (ncn == -1) {		/* ending */
	    if (stop - start > BREVE) {
		k = ((stop - 1) / BREVE) * BREVE;
		boogi(start, k, ocn, ocn);
		start = k;
	    }
	    if (stop - start == BREVE)
		sp = &Bwe[RNUM(4)];		/* 2 bar */
	    else if (stop - start == WHOLE)
		sp = &Bwe[4];			/* 1 bar */
	    else
		sp = &Bwe[5];			/* fragment */
DBG(stderr, " %ld: ending until %ld, use pattern at %p\n", start, stop, dp);
	    dp = sp->seq;
	    lsstart = Pstart;
	    residue = 0;
	}
	if (PC(Ch[ocn].trans[0]) == Key)		/* root == key */
	    for (k = 12; --k >= 0; otones[k] = 1);	/* anything goes */
	else {
	    for (k = 12; --k >= 0; otones[k] = 0);
	    for (k = 12; --k >= 0; )
		otones[PC(k + Ch[ocn].trans[k])] = tones7[k];
	}
	if (sp) {
	    trn = sp->trn == REL? Ch[ocn].trans[0] : Key + sp->trn;
	    trn = PR(sp->tpr)? trn : Key;
	}
	nkeyson = 0;
	if (residue > 0)
	    dp = dp0;
	else if (dp && start + WHOLE == Pstart)	/* short turnaround */
	    dp = (int *) 0;			/* force new riff */
	for (contin = 0; start < stop; ) {
	    if (dp) {				/* a sequence exists */
		if (dp[1] == 0) {		/* end of riff sequence */
		    dp = sp->seq;		/* loop back NB: residue==0 */
		    if ((start % BREVE) == 0 && PR(sp->sip))	/* or switch */
			dp = (int *) 0;		/* force new riff */
		}
		if (residue > 0 || lsstart == start)	/* don't change */
		    ;
		else if (start + BREVE == Pstart)	/* turnaround */
		    dp = (int *) 0;			/* force new riff */
		else if (start == Pstart && lsstart < Pstart) /* end of above */
		    dp = (int *) 0;			/* force new riff */
	    }
	    while (!dp) {			/* pick a new riff sequence */
		k = RNUM(NBWS);
DBG(stderr, "rand() suggests %d, curseq=%d, lastseq=%d\n", k, curseq, lastseq);
		if (k == curseq || k == lastseq)
		    continue;
		sp = &Bws[k];
		if (sp->use > start - 8 * 480) { /* used recently (8 bars) */
DBG(stderr, "%d was used %g bars ago\n", k, (start - sp->use) / 480.);
		    continue;
		}
		if ((Pstart - start) % sp->len) {	/* bad alignment */
DBG(stderr, "%d' needs the rest of the part to be a multiple of %d long\n",
k, sp->len);
		    continue;
		}
		lastseq = curseq;
		curseq = k;
		dp = sp->seq;
		lsstart = start;
		trn = sp->trn == REL? Ch[ocn].trans[0] : Key + sp->trn;
		trn = PR(sp->tpr)? trn : Key;
DBG(stderr, "New sequence is %d\n", curseq);
	    }
	    if (!contin)
		dp0 = dp;
	    k = *dp++;
	    v = *dp++;
	    d = *dp++;
	    if (d != CNT) {
		if (residue > 0) {
DBG(stderr, " use dur=%d (already played with dur=%d)\n", residue, d - residue);
		    d = residue;
		}
		residue = start + d - stop;
		if (residue > 0) {	/* not enough time, save it */
		    d -= residue;	/* (= stop - start) */
DBG(stderr, " too long (%d), set dur=%d & save %d\n", d + residue, d, residue);
		    dp = dp0;
		}
	    }
DBG(stderr, " k=%d trn=%d k+trn=%d(%s), v=%d, d=%d\n",
 k, trn, k + trn, k>0? key2name(k + trn) : "", v, d);
	    if (!k)		/* rest */
		start += d;
	    else {
		k = nearest(k + trn, otones, ANY);
		m.when = start;
		m.cmd[1] = k;
		for (j = 0; j < Nchans; j++) {
		    m.cmd[0] = CH_KEY_ON | Chan[j];
		    m.cmd[2] = Vol[j];
		    putmcmd(stdout, &m);
		}
		keyson[nkeyson++] = k;
DBG(stderr, " %4ld: %d key-on (%s)\n", start, k, key2name(k));
		if (d == CNT)
		    contin = 1;
		else {
		    start += d;
		    m.when = start;
		    m.cmd[2] = 0;
		    do {			/* nkeyson is > 0 */
			m.cmd[1] = keyson[--nkeyson];
			for (j = 0; j < Nchans; j++) {
			    m.cmd[0] = CH_KEY_ON | Chan[j];
			    putmcmd(stdout, &m);
			}
DBG(stderr, " %4ld: %d key-off (%s)\n",
 start, m.cmd[1], key2name((int)m.cmd[1]));
		    } while (nkeyson > 0);	/* avoid going negative */
		    contin = 0;
		    sp->use = start;
		}
	    }
	}
}

/* CLASS data */
char	Rp[][64]	= {	/* melodic rhythm patterns (in 32nds) */
	/*		 |   &   |   &   |   &   |   &   | */
	/* A */		"h...............q.......q.......",
	/* B */		"q.......E.....s.q.......q.......",
	/* C */		"s.s.s.s.e...e...q.......e...e...",
	/* D */		"q.......E.....s.q.......r.......",
	/* E */		"Q...........e...h...............",
	/* F */		"h...............r.......q.......",
	/* G */		"w...............................",
};
char	Arp[64]	= {	/* alternate rhythm pattern for march endings */
	/* G */		"h...............r.......s.r.....",
};

void
class(start, stop, ocn, ncn)
long	start, stop;
{
	register int i, dk, dur;
	char *pp, p, m;
	int nn, nk, begbar, endbar, beg32, udopt, *otones;
	long st;
	static char plan[512];
	static int planlen, mcnt[7], bbegan = 0, bnomove = 0, bdir, corn;

	otones = Ch[ocn].ctone;
	if (planlen == 0) {			/* make a rhythmic plan */
	    planlen = Bars? Bars : 32;		/* fake it if no bars arg */
	    for (pp = plan; dk = planlen - (pp - plan); ) {
		if (dk >= 12) {
		    *pp++ = p = oneof("ABC", 'G');
		    *pp++ = p = oneof("BCE", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = p = oneof("EF", p);
		    *pp++ = p = oneof("ABCF", p);
		    *pp++ = p = oneof("ACEF", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = p = oneof("AF", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = p = oneof("BCC", p);
		    *pp++ = 'G';
		} else if (dk >= 8) {
		    *pp++ = p = oneof("AB", 'G');
		    *pp++ = p = oneof("ABE", p);
		    *pp++ = p = oneof("BCE", p);
		    *pp++ = p = oneof("ADEF", p);
		    *pp++ = p = oneof("BCE", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = 'G';
		} else if (dk >= 4) {
		    *pp++ = p = 'A';
		    *pp++ = p = oneof("ABE", p);
		    *pp++ = p = oneof("ABC", p);
		    *pp++ = 'G';
		} else if (dk >= 2) {
		    *pp++ = 'B';
		    *pp++ = 'D';
		} else {
		    *pp++ = 'D';
		}
	    }
	    *pp = '\0';
DBG(stderr, " plan: %s\n", plan);
	}
	begbar = start / WHOLE;
	for (endbar = (stop - 1) / WHOLE; begbar < endbar; start = st)
	    class(start, st = (++begbar) * WHOLE, ocn, ocn);
	while (begbar >= planlen)		/* ran past end? */
	    begbar -= planlen;
	m = plan[begbar];
	mcnt[m - 'A']++;
	if (ncn < 0 && begbar != planlen - 1) {		/* ending */
DBG(stderr, " unforeseen ending, begbar=%d, planlen=%d\n", begbar, planlen);
	    m = 'G';
	}
	for (i = planlen; i > 16; i >>= 1);	/* determine cycle length */
	udopt = (begbar % i) < (i >> 1)? UPOK : DNOK;
	beg32 = (start % WHOLE) / THIRTYSECOND;
	nn = 0;
DBG(stderr, " PLAN CHAR: %c, udopt=%d, beg32=%d\n", m, udopt, beg32);
	for (pp = &Rp[m - 'A'][beg32]; start < stop; start += THIRTYSECOND) {
	    if ((p = *pp++) == 'w')
		dur = WHOLE;
	    else if (p == 'h')
		dur = HALF;
	    else if (p == 'Q')
		dur = QUARTER + EIGHTH;
	    else if (p == 'q')
		dur = QUARTER;
	    else if (p == 'E')
		dur = EIGHTH + SIXTEENTH;
	    else if (p == 'e')
		dur = EIGHTH;
	    else if (p == 's')
		dur = SIXTEENTH;
	    else {
		if (p == 'r' && Hist.k >= 72)
		    Hist.k -= 12;
		continue;
	    }
DBG(stderr, "  PATTERN CHAR: %c\n", *pp);
	    dur = (dur > stop - start? (stop - start) : dur);
	    nn++;
	    if (Hist.k <= G2) {
DBG(stderr, "== Hist.k=%d\n", Hist.k);
		udopt = UPONLY;
		while (Hist.k <= C2)
		    Hist.k += 12;
	    }
	    if (Hist.k >= C5) {
DBG(stderr, "== Hist.k=%d\n", Hist.k);
		udopt = DNONLY;
		while (Hist.k >= Gb5)
		    Hist.k -= 12;
	    }
	    if (m == 'A') {
		if (mcnt[m - 'A'] & 1) {
		    i = PR(33)? UPOK | OLDOK : UPOK;
		    nk = nearest(Hist.k, otones, i);
		} else {
		    if (nn == 1)
			nk = nearest(Hist.k, otones, ANY);
		    else if (nn == 2) {
			i = PR(33)? UPOK | udopt : udopt;
			nk = nearest(Hist.k, Stones, i);
		    } else
			nk = nearest(Hist.k, Stones, udopt);
		}
	    } else if (m == 'B') {
		if (start == 0)		/* beginning of piece */
		    bbegan = 1;
		if (nn == 1) {
		    nk = nearest(Hist.k, otones, ANY);
		    if (bbegan || PR50)
			bnomove = 1;
		    else
			bdir = PR50? UPONLY : udopt;
		} else if (bbegan || bnomove || nn < 4)
		    nk = Hist.k;
		else
		    nk = nearest(Hist.k, otones, bdir);
	    } else if (m == 'C') {
		if (nn == 1)
		    corn = PR(25);
		if (corn) {
		    if (nn == 1)	/* make ornament turn on chord tone */
			Hist.k = nearest(Hist.k, otones, ANY);
		    if (nn == 1 || nn == 4)
			nk = nearest(Hist.k, Stones, UPONLY);
		    else if (nn == 2 || nn == 3)
			nk = nearest(Hist.k, Stones, DNONLY);
		    else if (nn == 5) {
			nk = nearest(Hist.k, otones, udopt);
			if (PR50)
			    nk = nearest(nk, otones, udopt);
			else if (PR50) {
			    nk = nearest(nk, otones, udopt);
			    nk = nearest(nk, otones, udopt);
			}
		    } else if (nn == 7) {
			if (PR50)
			    nk = Hist.k;
			else
			    nk = nearest(Hist.k, otones, udopt ^ (UPOK|DNOK));
		    } else if (nn == 6 || nn == 8 || nn == 9)
			nk = Hist.k;
		} else {
		    if (nn == 1)
			nk = nearest(Hist.k, otones, ANY);
		    else if (nn == 2 || nn == 5 || nn == 8)
			nk = nearest(Hist.k, Stones, UPONLY);
		    else if (nn == 3 || nn == 4)
			nk = nearest(Hist.k, Stones, DNONLY);
		    else if (nn == 6)
			nk = Hist.k;
		    else if (nn == 7) {
			if (PR(75)) {
			    nk = nearest(Hist.k, otones, DNONLY);
			    nk = nearest(nk, otones, DNONLY);
			} else
			    nk = Hist.k - 12;
		    } else if (nn == 9)
			nk = nearest(Hist.k, otones, UPONLY);
		}
	    } else if (m == 'D') {
		if (begbar == planlen - 1)
		    nk = C3 + Key;
		else {
		    if (nn == 1)
			nk = nearest(Hist.k, otones, ANY);
		    if (nn == 2)
			nk = Hist.k;
		    else
			nk = nearest(Hist.k, otones, udopt);
		}
	    } else if (m == 'E') {
		if (begbar == planlen - 1)
		    nk = C3 + Key;
		else if (mcnt[m - 'A'] & 1) {
		    if (begbar == 0)
			nk = nearest(Hist.k, otones, UPONLY);
		    else
			nk = nearest(Hist.k, otones, udopt);
		} else {
		    if (nn == 2)
			nk = nearest(Hist.k, Stones, udopt);
		    else
			nk = nearest(Hist.k, otones, udopt);
		}
	    } else if (m == 'F') {
		if (begbar == 0)
		    nk = nearest(Hist.k, otones, UPONLY);
		else
		    nk = nearest(Hist.k, otones, udopt);
	    } else if (m == 'G') {
		if (PC(Hist.k - Key - 3) < 6)
		    nk = Hist.k + 6 - PC(Hist.k - Key + 2);
		else
		    nk = Hist.k + 6 - PC(Hist.k - Key - 6);
		if (otones[PC(nk)] <= 0) {		/* not a I chord */
		    nk = nearest(Hist.k, otones, udopt);
		    if (otones[PC(nk)] <= 0)
			nk = nearest(Hist.k, otones, ANY);
		}
	    } else
		fprintf(stderr, "motive rhythm character is '%c'?\n", m);
	    if (PC(Hist.k + 1) == PC(Key)
	     && nk != Hist.k + 1
	     && otones[PC(Key)] > 0)
		nk = Hist.k + 1;
	    if (20 < nk && nk < 120)		/* "always happens" */
		noteonoff(Hist.k = nk, start, start + dur);
	}
}

char
oneof(str, avoid)	/* return one of the chars in str, but not 'avoid' */
char	*str, avoid;
{
	char c;
	int i, n;

	n = strlen(str);
	for (i = 10; --i && (c = str[RNUM(n)]) == avoid; );
	return(c);
}

void
march(start, stop, ocn, ncn)
long	start, stop;
{
	register int i;

	if (ncn < 0)				/* ending coming up */
	    for (i = 64; --i >= 0; Rp['G'-'A'][i] = Arp[i]);
	class(start, stop, ocn, ncn);
}

#define	MOZGEN(ss,es) \
	for (bp = buf, s = (ss); s < (es); s++) \
	    bp += mozgen(bp, s, RNUM(6) + RNUM(6), 1, -Nchans)

void
mozar(start, stop, ocn, ncn)
long	start, stop;
{
	register int i, bars, s;
	char buf[2048], *bp;	/* plenty of room for 4 bars */
	extern int Mozchan[4], Mozvel[4];

	if (ncn >= 0)		/* wait until last call */
	    return;
	mozinit(Key);
	if (Nchans > 4)
	    Nchans = 4;
	for (i = Nchans; --i >= 0; ) {
	    Mozchan[i] = Chan[i];
	    Mozvel[i] = Vol[i];
	}
	start = 0L;
	bars = (stop - start) / 360;	/* waltz time */
	i = (stop - start) % 360;	/* partial bar */
	if (i % 120) {			/* partial beat */
	    noteonoff(60 + Key, start, start + i % 120);
	    i -= (i % 120);
	}
	if (i)
	    noteonoff(60 + Key, start, start + i);
	if (bars <= 0)
	    return;
	if (i = (bars % 4)) {
	    if (bars < 4)
		MOZGEN(16 - 2 * i, 16);
	    else
		MOZGEN(0, 2 * i);
	    write(1, buf, bp - buf);
	    bars -= i;
	}
	if (bars <= 0)
	    return;
	for (bars -= 4; bars > 0; bars -= 4) {
	    MOZGEN(0, 8);
	    write(1, buf, bp - buf);
	    if ((bars -= 4) <= 0)
		break;
	    write(1, buf, bp - buf);
	    if ((bars -= 4) <= 0)
		break;
	    MOZGEN(8, 16);
	    write(1, buf, bp - buf);
	}
	MOZGEN(8, 16);
	write(1, buf, bp - buf);
}

#define	PLEN	32
#define	SLEN	5

void
seque(start, stop, ocn, ncn)
long	start, stop;
{
	int i, j, cnss, pcss, maxss, rng, buildur;
	int pat[PLEN], scale[SLEN];
	long end, t;

	if (ncn >= 0)		/* wait until last call */
	    return;
	scale[0] = Key + 0;
	scale[1] = Key + 2;
	scale[2] = Key + 4;
	scale[3] = Key + 7;
	scale[4] = Key + 9;
	start = 0L;
	if (stop - start >= 6 * WHOLE)
	    end = stop - 2 * WHOLE;	/* room for chord at end */
	else if (stop - start >= WHOLE)
	    end = stop - WHOLE;		/* room for chord at end */
	else
	    end = start;		/* room for chord at end */
	buildur = (stop - end) / 2;
DBG(stderr, "start=%d end=%d buildur=%d stop=%d\n",
 start, end, buildur, stop);
	cnss = 4 * SLEN;		/* current note scale step */
	pcss = cnss + SLEN + 2;		/* pitch center scale step */
	maxss = 10 * SLEN;
	rng = ((120 - Predict) * SLEN) / 70;
	for (i = 0; i < PLEN; i++) {		/* create sequence */
	    pat[i] = (cnss / SLEN) * 12 + scale[cnss % SLEN];
	    j = (cnss + cnss + pcss) / 3;
	    do {
		j += RNUM(rng) - RNUM(rng);
	    } while (j == cnss && rng > 1);
	    cnss = (j <= 0 || j >= maxss)? pcss : j;
	}
	for (t = 0; t < end; t += SIXTEENTH) {	/* repeat sequence */
	    i = (t / SIXTEENTH) % PLEN;
	    if (i == 0 && t)
		noteoff(0, C1 + Key, t - 1);
	    if (t > SIXTEENTH)
		for (j = 1; j < Nchans; j++)
		    noteoff(j, pat[(i + PLEN - 2) % PLEN], t - 1);
	    for (j = 1; j < Nchans; j++)
		if (PR(Energy))
		    noteon(j, pat[i], t);
	    if (i == 0)
		noteon(0, C1 + Key, t);
	}
	i = (end / SIXTEENTH) % PLEN;
	for (j = 1; j < Nchans; j++) {
	    noteoff(j, pat[(i + PLEN - 2) % PLEN], end - 1);
	    noteoff(j, pat[(i + PLEN - 1) % PLEN], end - 1);
	}
	for (t = end; t < end + buildur; t += SIXTEENTH)
	    for (j = 1; j < Nchans; j++)
		if (PR(Energy))
		    noteon(j, pat[(t / SIXTEENTH) % PLEN], t);
	for (t = end; t < end + buildur; t += SIXTEENTH)
	    for (j = 1; j < Nchans; j++)
		noteoff(j, pat[(t / SIXTEENTH) % PLEN], stop);
	noteoff(0, C1 + Key, stop);
}

void
swing(start, stop, ocn, ncn)
long	start, stop;
{
	register int i, dk, dur, dir;
	int nk, nk2, nk3, needforce, *tp, *otones, *ntones, energy, mindur;
	long lt, lt2;
	static int force;	/* key that needs to be played (not a rest) */
	static int breath;	/* last time we took a breath (end of breath) */
	static int depth;	/* recursion depth */
	static int swung;	/* avoid multiple swung subdivisions */

DBG(stderr, "   swing(%ld, %ld) depth=%d, swung=%d\n", start, stop, depth, swung);
	if (depth++ == 0)
	    swung = 999;
	energy = Ch[ocn].name[0] == '-'? 10 : Energy;	/* mellow in a rest */
	mindur = ((20 + rand() % 83) * (200 - energy)) / (energy + 100);
	needforce = 0;
	otones = Ch[ocn].ctone;
	ntones = Ch[ncn].ctone;
	if (!force && (Hist.k <= C1 || Hist.k >= C5)) {
DBG(stderr, "== Hist.k=%d Hist.d=%d\n", Hist.k, Hist.d);
	    Hist.k = C3;
	    Hist.d = 0;
	    if (stop - start > QUARTER) {
		start = (start + stop) / 2;
		breath = start;
	    }
	}
	if (ncn < 0) {					/* ending */
DBG(stderr, " ending note\n");
	    if (force) {
		Hist.k = force - Hist.d / 2;
DBG(stderr, " trying to force %d\n", force);
	    }
	    nk = nearest(Hist.k + Hist.d / 2, otones, ANY);
	    lt = (stop - start) >= WHOLE? stop - HALF : stop;
	    noteonoff(nk, start, lt);			/* chord */
	    SAVHIST(&Hist);
	    --depth;
	    return;
	}
	dur = stop - start;
	if (!force
	 && dur >= QUARTER && start - breath > 3 * WHOLE) {
	    start += QUARTER;
	    breath = start;
DBG(stderr, " breathe for a quarter from %ld to %d\n", start - QUARTER, breath);
	    dur = stop - start;
	    if (dur <= 0)
		--depth;
		return;
	}
	if (dur >= 4 * mindur && PR(50 + energy / 2)) {
	    for (i = WHOLE; i >= dur; i >>= 1);
	    lt = start + i;
DBG(stderr, " recurse %ld to %ld to %ld\n", start, lt, stop);
	    swing(start, lt, ocn, ocn);
	    swing(lt, stop, ocn, ncn);
	    --depth;
	    return;
	}
	dir = Hist.d? (Hist.d > 0 ? UPONLY : DNONLY) : NEW;
	dir = Hist.k <= C2? UPONLY : (Hist.k >= 72? DNONLY : dir);
	dur = stop - start;
	i = RNUM(50 + energy);
	if ((i -= 25) < 0 || (force && PR(50))) {	/* one note */
DBG(stderr, " one note\n");
	    if (force) {
		nk = force;
DBG(stderr, "  forcing: %d\n", force);
	    } else {
		nk = nearest(Hist.k, otones, NEW);	/* look for pivot */
		if (ntones[PC(nk)]) {
DBG(stderr, "  pivot nk=%d\n", nk);
		} else {
		    nk2 = nearest(nk, ntones, ANY);
		    if (otones[PC(nk2)] > 0) {
			i = nk2 - Hist.k;
			if (Hist.d * i >= 0
			 && -2 < (i - Hist.d) && (i - Hist.d) < 2) {
DBG(stderr, "  pivot? nk=%d, nk2=%d\n", nk, nk2);
			    nk = nk2;			/* found one */
			}
		    }
		}
	    }
	    lt = stop - (dur >= HALF? EIGHTH : 0);
	    if (lt != stop)
		breath = stop;
	    noteonoff(nk, start, lt);			/* chord */
	} else if ((i -= 25) < 0) {		/* two note split */
	    if (swung < depth) {		/* already swung above us */
		lt = (start + stop) / 2;
DBG(stderr, " straight two-way (depth=%d swung=%d)\n", depth, swung);
	    } else if (swung == depth		/* already swung @ this depth */
	     || (dur >= 3 * mindur && PR(67))) {
		lt = (start + stop + stop) / 3;
DBG(stderr, " swung two-way (depth=%d swung=%d)\n", depth, swung);
		swung = depth;
	    } else {
		lt = (start + stop) / 2;
DBG(stderr, " straight two-way\n");
	    }
	    nk = force? force : nearest(Hist.k, Stones, dir);
	    nk2 = nearest(nk, ntones, dir);
	    dk = nk2 - nk;
DBG(stderr, "   nk=%d, nk2=%d\n", nk, nk2);
	    if (!force
	     && (start - breath) > (3 * WHOLE)		/* we need a breath */
	     && (stop - start) >= QUARTER
	     && PR(50 - energy / 2)) {
		breath = lt;
		noteonoff(Hist.k = nk, lt, stop);	/* scale */
		needforce = nk2;			/* not critical */
DBG(stderr, " breathe from %ld to %l\n", start, breath);
	    } else if (dk == 1 || dk == -1) {	/* nk2 is 1 step from nk */
		noteonoff(nk, start, lt);		/* scale */
		nk3 = nearest(nk2, Stones, dir);	/* one step beyond */
		noteonoff(Hist.k = nk3, lt, stop);	/* scale */
		needforce = nk2;			/* not critical */
	    } else if (dk == 2 || dk == -2) {	/* nk2 is 2 steps from nk */
		noteonoff(nk, start, lt);		/* scale */
		if (stop - lt > EIGHTH) {
		    nk3 = nearest(nk2, Stones, dir);	/* one step beyond */
		    noteonoff(Hist.k = nk3, lt, stop);	/* scale */
		} else
		    noteonoff(nk2 - dk / 2, lt, stop);	/* chromatic */
		needforce = nk2;
	    } else if (dk && -5 <= dk && dk <= 5) {	/* nk2 is 3|4|5 steps */
		noteonoff(nk, start, lt);		/* scale */
		nk3 = nearest(nk, Stones, dir);
		noteonoff(nk3, lt, stop);		/* scale */
		if (nk3 != nk2)
		    needforce = nk2;			/* not critical */
	    } else {					/* who knows? */
		noteonoff(nk, start, lt);		/* scale */
		nk = nearest(nk, otones, dir);
		noteonoff(nk, lt, stop);		/* chord */
	    }
	} else if ((i -= 25) < 0) {		/* three note split */
	    if (swung < depth) {		/* already swung above us */
		lt = (start + start + start + stop) / 4;
		lt2 = (start + stop) / 2;	/* quarters & half */
DBG(stderr, " 1/4, 1/4, 1/2  three-way (depth=%d swung=%d)\n", depth, swung);
	    } else if (swung == depth		/* already swung @ this depth */
	     || (dur >= 3 * mindur && PR(50))) {
		lt = (start + stop + stop) / 3;
		lt2 = (start + stop + stop) / 3;
DBG(stderr, " swung three-way (depth=%d swung=%d)\n", depth, swung);
		swung = depth;
	    } else {				/* 50% half & quarters */
		lt = (start + stop) / 2;
		lt2 = (start + stop + stop + stop) / 4;
DBG(stderr, " 1/2, 1/4, 1/4  three-way (depth=%d swung=%d)\n", depth, swung);
	    }
	    if (!force
	     && (start - breath) > (3 * WHOLE)		/* we need a breath */
	     && (stop - start) > QUARTER
	     && PR(50 - energy / 2)) {
		breath = lt;
DBG(stderr, " breathe from %ld to %d\n", start, breath);
		nk = Hist.k;
	    } else {
		nk = force? force : nearest(Hist.k, otones, dir);
		noteonoff(nk, start, lt);			/* chord */
	    }
	    dk = (nk < (Gb2 + RNUM(13)))? UPONLY : DNONLY;
DBG(stderr, "   Hist.k=%d, Hist.d=%d, nk=%d, dk=%d\n", Hist.k, Hist.d, nk, dk);
	    if ((lt - breath) > HALF
	     && PR(50 - energy / 2)) {		/* possible rest */
		breath = lt2;
DBG(stderr, " breathe from %ld to %d\n", lt, breath);
	    } else {
		nk = nearest(nk, Stones, dk);
		noteonoff(nk, lt, lt2);			/* scale */
	    }
	    nk2 = nearest(nk, ntones, dk);
	    dk = nk2 - nk;
	    if ((dk == -2 || dk == 2) && stop - lt2 < QUARTER)
		nk3 = nk2 - dk / 2;			/* chromatic */
	    else
		nk3 = nearest(nk, Stones, dk);		/* scale step */
	    noteonoff(nk3, lt2, stop);
	    if (nk3 != nk2)
		needforce = nk2;
	} else if ((i -= 25) < 0 && dur >= EIGHTH) {	/* four note split */
DBG(stderr, " even 4-way\n");
	    lt = (start + start + start + stop) / 4;	/* even split */
	    dk = (Hist.k < (Ab2 + RNUM(9)))? UPONLY : DNONLY;
	    tp = (53 < Hist.k && Hist.k < 67)? Stones : otones;
DBG(stderr, "   Hist.k=%d, Hist.d=%d, dk=%d\n", Hist.k, Hist.d, dk);
	    if (!force
	     && (start - breath) >= WHOLE
	     && PR(25 - energy / 4)) {			/* possible rest */
		nk = Hist.k;
		breath = lt;
DBG(stderr, " breathe from %ld to %d\n", start, lt);
	    } else {
		nk = force? force : nearest(Hist.k, tp, dk);
		noteonoff(nk, start, lt);		/* scale or chord */
	    }
	    lt2 = (start + stop) / 2;
	    nk = nearest(nk, tp, dk);
	    noteonoff(nk, lt, lt2);			/* scale or chord */
	    lt = (start + stop + stop + stop) / 4;
	    if (lt2 - breath >= HALF
	     && PR(25 - energy / 4)) {			/* possible rest */
		breath = lt;
DBG(stderr, " breathe from %ld to %d\n", lt2, lt);
	    } else {
		nk = nearest(nk, tp, dk);
		noteonoff(nk, lt2, lt);			/* scale or chord */
	    }
	    nk3 = nearest(nk, otones, NEW);
	    noteonoff(nk3, lt, stop);			/* chord */
	} else if (force) {				/* play forced note */
DBG(stderr, " forced note\n");
	    noteonoff(force, start, stop);		/* forced (chord) */
	} else {					/* move toward C3 */
DBG(stderr, " move toward center\n");
	    dir = (Hist.k < C3)? UPONLY : DNONLY;
	    nk = nearest(Hist.k, otones, dir);
	    noteonoff(nk, start, stop);			/* chord */
	}
	force = needforce;
	SAVHIST(&Hist);
	--depth;
}

void
toner(start, stop, ocn, ncn)
long	start, stop;
{
	int i, j, k;
	int Row[12], rowlen, kon[3], seed;
	long t, end;

	if (ncn >= 0)		/* wait until last call */
	    return;
	for (i = 12; --i >= 0; Row[i] = C3 + Key + i);	/* compose Row */
	for (i = 12; --i > 0; ) {
	    j = RNUM(i + 1);
	    k = Row[i]; Row[i] = Row[j]; Row[j] = k;
	}
	rowlen = 12 * SIXTEENTH;
	start = 0L;
	end = stop - rowlen;			/* room for chord at end */
DBG(stderr, "start=%ld end=%ld stop=%ld\n", start, end, stop);
	kon[0] = kon[1] = kon[2] = 0;
	srand(seed = rand());
	for (t = start; t < end; t += SIXTEENTH) {
	    i = (t / SIXTEENTH) % 12;
	    if (i == 0) {
		srand(t + rand());	/* avoid endless repeat cycle */
		if (!PR(Predict))
		    seed = rand();
		srand(seed);
	    }
	    if (Chan[0] >= 0)		/* first line (straight) */
		noteon(0, kon[0] = Row[i] - 12, t);
	    if (Chan[2] >= 0		/* third line (accented) */
	     && t >= start + 5 * rowlen
	     && PR((Energy * 2) / 3))
		noteon(2, kon[2] = Row[11 - i], t);
	    else if (Chan[1] >= 0	/* second line (inverted) */
	     && t >= start + 2 * rowlen)
		noteon(1, kon[1] = 126 - Row[i], t);
	    for (i = 3; --i >= 0; kon[i] = 0)
		if (kon[i])
		    noteoff(i, kon[i], t + SIXTEENTH - 1);
	}
	for (i = 0; i < 12; i++) {		/* chord at end */
	    noteon(0, Row[i] - 12, end + i * SIXTEENTH);
	    noteon(i % 2 + 1, Row[i], end + i * SIXTEENTH);
	}
	for (i = 12; --i >= 0; ) {
	    noteoff(0, Row[i] - 12, stop);
	    noteoff(i % 2 + 1, Row[i], stop);
	}
}

int nearest(k, tones, flg)		/* find note nearest k in tones */
int	tones[12];
{
	register int i, j;

	for (i = (flg & OLDOK)? 0 : 1; i < 7; i++)
	    if (((flg & UPOK) && tones[PC(j = k + i)])
	     || ((flg & DNOK) && tones[PC(j = k - i)]))
		break;
	if (i < 7)
	    k = j;
	if (k < C1) {
	    if (k < 30 || (flg & UPOK))
		k += 12;
	} else if (k > C5) {
	    if (k > Gb5 || (flg & DNOK))
		k -= 12;
	}
	return(k);
}

/* emit key-on & key-off (on all Nchans) */
void noteonoff(k, start, stop)
long	start, stop;
{
	register int j;
int dbgsav;
DBG(stderr, "  %s (%d) @%ld until %ld\n", key2name(k), k, start, stop);
dbgsav=Debug; Debug=0;
	for (j = 0; j < Nchans; j++)
	    noteon(j, k, start);
	for (j = 0; j < Nchans; j++)
	    noteoff(j, k, stop);
Debug=dbgsav;
	Lastnoted = k - Lastnotek;
	Lastnotek = k;
}

void noteon(c, k, when)
long	when;
{
	u_char ubuf[4];
	MCMD m;

DBG(stderr, "  %s (%d) key-on @%ld\n", key2name(k), k, when);
	if (k < 0 || k > 127)
	    return;
	m.cmd = ubuf;
	m.len = 3;
	m.when = when;
	m.cmd[0] = CH_KEY_ON | Chan[c];
	m.cmd[1] = k;
	m.cmd[2] = Vol[c];
	//putmcmd(stdout, &m);
	putmcmdadagio(stdout, &m, 1);
}

void noteoff(c, k, when)
long	when;
{
	u_char ubuf[4];
	MCMD m;

DBG(stderr, "  %s (%d) key-off @%ld\n", key2name(k), k, when);
	if (k < 0 || k > 127)
	    return;
	m.cmd = ubuf;
	m.len = 3;
	m.when = when;
	m.cmd[0] = CH_KEY_ON | Chan[c];
	m.cmd[1] = k;
	m.cmd[2] = 0;
	//putmcmd(stdout, &m);
	putmcmdadagio(stdout, &m, 0);
}

int wrdcmp(ap, bp)
char	*ap, *bp;
{
	while (*bp && *ap == *bp) {
	    ap++;
	    bp++;
	}
	return ((*ap <= ' ' && *bp == '\0')? 0 : *ap - *bp);
}

void syntax(msg)
char	*msg;
{
	fprintf(stderr, "Chord chart syntax error: %s\n", msg);
	exit(1);
}

int find(name)
char	*name;
{
	register int i;

	for (i = 0; i < Numchords; i++)
	    if (wrdcmp(name, Ch[i].name) == 0)
		return(i);
	return(-1);
}

sys(buf)
char	*buf;
{
DBG(stderr, "%s\n", buf);
	system(buf);
}
