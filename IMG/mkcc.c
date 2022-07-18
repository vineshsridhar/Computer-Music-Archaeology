/*
**	MKCC -- Create a chord chart for the specified "style"
**	psl 2/89
*/
#include <stdlib.h>
#include	<stdio.h>
#include <string.h>
#include <time.h>
#include	<midi.h>
#include "key2name.h"

#ifndef BASE
#define BASE "/usr/sau/psl/bs/etc"
#endif

#define	ONCE	1024
#define	STOP	2048

#define	WHOLE	(MPU_CLOCK_PERIOD<<1)
#define	WONCE	(WHOLE|ONCE)
#define	WSTOP	(WHOLE|STOP)
#define	HALF	(MPU_CLOCK_PERIOD)
#define	HONCE	(HALF|ONCE)
#define	HSTOP	(HALF|STOP)
#define	QUARTER	(MPU_CLOCK_PERIOD>>1)
#define	QONCE	(QUARTER|ONCE)
#define	QSTOP	(QUARTER|STOP)
#define	EIGHTH	(MPU_CLOCK_PERIOD>>2)
#define	EONCE	(EIGHTH|ONCE)
#define	ESTOP	(EIGHTH|STOP)
#define	NOTIME	(0)
#define	PR(PCNT)	((rand()%100)<(PCNT))
#define	PR50		(rand()&010)

void	grass(), boogi(), class(), mozar(), swing();
char	*genseq(), *cname(), *flush();
extern	char	*strcopy();

#define	DEFSTYLE	"class"
struct	stylstr	{		/* definitions of styles */
	char	*name;		/* style names (for arguments) */
	char	*defs;		/* chord definitions */
	char	*quant;		/* quantization (as a string) */
	int	cpq;		/* MPU clocks per quantum */
	int	barlen;		/* bar length in MPU clocks */
	void	(*comp)();	/* composition routine */
} S[]	= {
	{ "bebop", "accagc.cc", "quarter", 120, 480, swing, },
	{ "grass", "accagg.cc", "quarter", 120, 480, grass, },
	{ "boogi", "accagc.cc", "quarter", 120, 480, boogi, },
	{ "class", "accacc.cc", "quarter", 120, 480, class, },
	{ "march", "accacc.cc", "quarter", 120, 480, class, },
	{ "mozar", "accacc.cc", "quarter", 120, 360, mozar, },
	{ "samba", "accagc.cc", "quarter", 120, 480, swing, },
	{ "seque", "accagc.cc", "quarter", 120, 480, mozar, },
	{ "swing", "accagc.cc", "quarter", 120, 480, swing, },
	{ "toner", "accagc.cc", "quarter", 120, 360, mozar, },
	{ (char *) 0, },
};
#define	MAXNXT	4
typedef	struct	cseqstr	{
	int	dura;		/* in MPU clocks */
	int	root;		/* 0 = tonic, 5 = subdominant, etc */
	char	*type;		/* "m7", "o", "7", etc. */
	int	next[MAXNXT];	/* indices of possible successors */
	int	prob[MAXNXT];	/* probabilities of possible successors */
} CSEQ;
typedef	struct	sseqstr {
	int	rpts;		/* number of repeats */
	char	*name;		/* sequence name */
	CSEQ	*cseq;		/* sequence */
} SSEQ;

void put(char *str);
void putsong(SSEQ *sp, int key, int cpq, int rnd);

/* GRASS chord sequences */
CSEQ	Lmv[]	= {	/* Little Maggie (4 bars) */
	{ WHOLE, 0, "", 	{ 1,2, }, 	{ 50, 50, }, },
	{ HALF, 5, "",  	{ 3,4, }, 	{ 67, 33, }, },
	{ HALF, 10, "", 	{ 4,3, }, 	{ 67, 33, }, },
	{ HALF, 5, "",	 	{ 5, }, 	{ 100, }, },
	{ HALF, 10, "", 	{ 5, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 6,7, }, 	{ 67, 33, }, },
	{ HALF, 7, "7", 	{ 8, }, 	{ 100, }, },
	{ HALF, 10, "", 	{ 8, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Dwv[]	= {	/* Doo Wop (4 bars) */
	{ WHOLE, 0, "", 	{ 1,2,3, }, 	{ 33, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 4,5,6, }, 	{ 48, 48,  4, }, },
	{ WHOLE, 9, "7",  	{ 4,5,6, }, 	{ 33, 33, 33, }, },
	{ WHOLE, 10, "",  	{ 4,5,6, }, 	{ 33, 33, 33, }, },
	{ HALF, 5, "",	 	{ 7,8, }, 	{ 50, 50, }, },
	{ HALF, 2, "m",	 	{ 7,8, }, 	{ 50, 50, }, },
	{ HALF, 0, "",	 	{ 7,8, }, 	{ 50, 50, }, },
	{ HALF, 7, "7", 	{ 9, }, 	{ 100, }, },
	{ HALF, 10, "", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Lmwv[]	= {	/* Little Maggie weird (5 bars) */
	{ WHOLE, 0, "",  	{ 1,2, }, 	{ 50, 50, }, },
	{ WHOLE, 5, "",  	{ 3,4, }, 	{ 67, 33, }, },
	{ WHOLE, 10, "", 	{ 4,3, }, 	{ 67, 33, }, },
	{ WHOLE, 5, "",  	{ 5, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 5, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 6,7, }, 	{ 67, 33, }, },
	{ HALF, 7, "7", 	{ 8, }, 	{ 100, }, },
	{ HALF, 10, "", 	{ 8, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Dwwv[]	= {	/* Doo Wop weird (5 bars) */
	{ WHOLE, 0, "", 	{ 1,2,3, }, 	{ 33, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 4,5,6, }, 	{ 48, 48,  4, }, },
	{ WHOLE, 9, "7",  	{ 4,5,6, }, 	{ 33, 33, 33, }, },
	{ WHOLE, 10, "",  	{ 4,5,6, }, 	{ 33, 33, 33, }, },
	{ WHOLE, 5, "",	 	{ 7,8, }, 	{ 50, 50, }, },
	{ WHOLE, 2, "m", 	{ 7,8, }, 	{ 50, 50, }, },
	{ WHOLE, 0, "",	 	{ 7,8, }, 	{ 50, 50, }, },
	{ WHOLE, 7, "7", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 9, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 10,11, }, 	{ 60, 40, }, },
	{ HALF, 7, "7", 	{ -1, }, 	{ 100, }, },
	{ HALF, 10, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Fmbv[]	= {	/* Foggy Mtn Breakdown (8 bars) */
	{ WHOLE, 0, "", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 2,3,4 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 9, "m", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 6,7,8 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 9, "m", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 10, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 11, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Bmc[]	= {	/* Beside Me chorus (12 bars) */
	{ HALF, 10, "",  	{ 1, }, 	{ 100, }, },
	{ HALF, 5, "",  	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 5, "", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 7, }, 	{ 100, }, },
	{ WHOLE, 10, "", 	{ 8, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 4, "m", 	{ 10, }, 	{ 100, }, },
	{ WHOLE, 2, "m", 	{ 11, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 12, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Bbv[]	= {	/* Blackberry Blossom verse (4 bars) */
	{ QUARTER, 0, "",  	{ 1, }, 	{ 100, }, },
	{ QUARTER, 7, "",  	{ 2, }, 	{ 100, }, },
	{ QUARTER, 5, "",  	{ 3, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ 4, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ 5, }, 	{ 100, }, },
	{ QUARTER, 5, "",  	{ 6, }, 	{ 100, }, },
	{ QUARTER, 2, "7",  	{ 7, }, 	{ 100, }, },
	{ QUARTER, 7, "7",  	{ 8, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ 9, }, 	{ 100, }, },
	{ QUARTER, 7, "",  	{ 10, }, 	{ 100, }, },
	{ QUARTER, 5, "",  	{ 11, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ 12, }, 	{ 100, }, },
	{ QUARTER, 5, "",  	{ 13, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ 14, }, 	{ 100, }, },
	{ QUARTER, 7, "7",  	{ 15, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bbb[]	= {	/* Blackberry Blossom bridge (4 bars) */
	{ HALF, 9, "m",  	{ 1, }, 	{ 100, }, },
	{ HALF, 9, "m",  	{ 2, }, 	{ 100, }, },
	{ HALF, 9, "m",  	{ 3,4, }, 	{ 50, 50, }, },
	{ HALF, 4, "7",  	{ 6, }, 	{ 100, }, },
	{ QUARTER, 5, "7",  	{ 5, }, 	{ 100, }, },
	{ QUARTER, 4, "7",  	{ 6, }, 	{ 100, }, },
	{ HALF, 9, "m",  	{ 7, }, 	{ 100, }, },
	{ HALF, 9, "m",  	{ 8, }, 	{ 100, }, },
	{ QUARTER, 5, "",  	{ 9, }, 	{ 100, }, },
	{ QUARTER, 2, "7",  	{ 10, }, 	{ 100, }, },
	{ QUARTER, 7, "7",  	{ 11, }, 	{ 100, }, },
	{ QUARTER, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bmtv[]	= {	/* Beatin' My Time Verse (8 bars) */
	{ WHOLE, 0, "",  	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ 3, }, 	{ 100, }, },
	{ WHOLE, 7, "7",  	{ 4, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ 5, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 6, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 7, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ 8, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bmtb[]	= {	/* Beatin' My Time Bridge (8 bars) */
	{ WHOLE, 5, "",  	{ 1, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 2, }, 	{ 100, }, },
	{ HALF, 0, "7",  	{ 3, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 4, }, 	{ 100, }, },
	{ HALF, 2, "7",  	{ 5, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ 6, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 7, }, 	{ 100, }, },
	{ HALF, 0, "7",  	{ 8, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 9, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 10, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ 11, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	End1[]	= {	/* Ending vamp (1 bar) */
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
/* GRASS song sequences */
SSEQ	Lm[]	= {		/* Little Maggie, 4 bars */
	{ 1, "Lmv", Lmv, },		/* 4 bars */
	{ 0, },
};
SSEQ	Dw[]	= {		/* Doo Wop, 4 bars */
	{ 1, "Dwv", Dwv, },		/* 4 bars */
	{ 0, },
};
SSEQ	Lmw[]	= {		/* Little Maggie weird, 5 bars */
	{ 1, "Lmwv", Lmwv, },		/* 5 bars */
	{ 0, },
};
SSEQ	Dww[]	= {		/* Doo Wop weird, 5 bars */
	{ 1, "Dwwv", Dwwv, },		/* 5 bars */
	{ 0, },				/* (needs an End1) */
};
SSEQ	Fmb[]	= {		/* Foggy Mtn Breakdown, 8 bars */
	{ 1, "Fmbv", Fmbv, },		/* 8 bars */
	{ 0, },
};
SSEQ	Bmth[]	= {		/* Beatin' My Time half, 8 bars (or 8 + 1) */
	{ 1, "Bmtv", Bmtv, },		/* 8 bars */
	{ 0, },				/* (needs an End1) */
};
SSEQ	Bbh[]	= {		/* Blackberry Blossom verse, 8 + 1 bars */
	{ 2, "Bbv", Bbv, },		/* 8 bars */
	{ 0, },				/* (needs an End1) */
};
SSEQ	Bm[]	= {		/* Beside Me, 12 bars */
	{ 1, "Bmc", Bmc, },		/* 12 bars */
	{ 0, },
};
SSEQ	Bbs[]	= {		/* Blackberry Blossom short, 12 bars */
	{ 1, "Bbv", Bbv, },		/* 4 bars */
	{ 1, "Bbv", Bbv, },		/* 4 bars */
	{ 1, "Bbb", Bbb, },		/* 4 bars */
	{ 0, },
};
SSEQ	Bmt[]	= {		/* Beatin' My Time, 16 bars (or 16 + 1) */
	{ 1, "Bmtv", Bmtv, },		/* 8 bars */
	{ 1, "Bmtb", Bmtb, },		/* 8 bars */
	{ 0, },				/* (needs an End1) */
};
SSEQ	Bb[]	= {		/* Blackberry Blossom, 16 + 1 bars */
	{ 2, "Bbv", Bbv, },		/* 8 bars */
	{ 2, "Bbb", Bbb, },		/* 8 bars */
	{ 0, },				/* (needs an End1) */
};

/* BOOGI chord sequences */
CSEQ	Bw4[]	= {	/* 4-bar boogie */
	{ WHOLE, 0, "", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw8a[]	= {	/* 8-bar boogie */
	{ WHOLE, 0, "", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3,4, }, 	{ 50, 50, }, },
	{ WHOLE, 0, "", 	{ 5,7, }, 	{ 67, 33, }, },
	{ WHOLE, 0, "7", 	{ 7,5, }, 	{ 67, 33, }, },
	{ WHOLE, 7, "7", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 9,10, }, 	{ 67, 34, }, },
	{ WHOLE, 5, "", 	{ 8, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 10,9, }, 	{ 67, 34, }, },
	{ WHOLE, 0, "", 	{ 11, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 11, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw8b[]	= {	/* 8-bar boogie */
	{ WHOLE, 0, "", 	{ 1,2, }, 	{ 50, 50, }, },
	{ WHOLE, 0, "7", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 4, "7", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 4,5, }, 	{ 67, 33, }, },
	{ WHOLE, 0, "", 	{ 6,7,8, }, 	{ 50, 30, 20, }, },
	{ WHOLE, 5, "7", 	{ 6,7,8, }, 	{ 60, 25, 15, }, },
	{ WHOLE, 0, "", 	{ 9,11, }, 	{ 60, 40, }, },
	{ WHOLE, 7, "7", 	{ 11,9, }, 	{ 50, 50, }, },
	{ WHOLE, 2, "7", 	{ 9,11, }, 	{ 67, 33, }, },
	{ WHOLE, 7, "7", 	{ 10, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 2, "7", 	{ 12, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw12ad[]	= {	/* 12-bar boogie, dominant ending */
	{ NOTIME, 0, "", 	{ 1,5, }, 	{ 67, 33, }, },
	{ WHOLE, 0, "", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 12, }, 	{ 100, }, },
	{ WONCE, 0, "", 	{ 6, }, 	{ 100, }, },
	{ WSTOP, 0, "", 	{ 7,8, }, 	{ 67, 33, }, },
	{ WONCE, 0, "", 	{ 9, }, 	{ 100, }, },
	{ WSTOP, 0, "", 	{ 9, }, 	{ 100, }, },
	{ HSTOP, 0, "", 	{ 10, }, 	{ 100, }, },
	{ QSTOP, 0, "", 	{ 11, }, 	{ 100, }, },
	{ QUARTER, 0, "7", 	{ 12, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 14, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 15, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 16, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 17, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 18,19, }, 	{ 67,33, }, },
	{ WHOLE, 0, "", 	{ 23, }, 	{ 100, }, },
	{ QUARTER, 0, "", 	{ 20, }, 	{ 100, }, },
	{ QUARTER, 3, "7", 	{ 21, }, 	{ 100, }, },
	{ QUARTER, 2, "7", 	{ 22, }, 	{ 100, }, },
	{ QUARTER, 1, "7", 	{ 23, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 24, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw12at[]	= {	/* 12-bar boogie, tonic ending */
	{ NOTIME, 0, "", 	{ 1,5, }, 	{ 67, 33, }, },
	{ WHOLE, 0, "", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 12, }, 	{ 100, }, },
	{ WONCE, 0, "", 	{ 6, }, 	{ 100, }, },
	{ WSTOP, 0, "", 	{ 7,8, }, 	{ 50, 50, }, },
	{ WONCE, 0, "", 	{ 9, }, 	{ 100, }, },
	{ WSTOP, 0, "", 	{ 9, }, 	{ 100, }, },
	{ HSTOP, 0, "", 	{ 10, }, 	{ 100, }, },
	{ QSTOP, 0, "", 	{ 11, }, 	{ 100, }, },
	{ QUARTER, 0, "7", 	{ 12, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 14, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 15, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 16, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 17, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 18, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 19, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw12b[]	= {	/* 12-bar boogie */
	{ WHOLE, 0, "", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 7,8, }, 	{ 50,50, }, },
	{ WHOLE, 9, "7", 	{ 9, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 9,10, }, 	{ 50,50, }, },
	{ WHOLE, 2, "7", 	{ 11, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 11,12, }, 	{ 50,50, }, },
	{ WHOLE, 7, "7", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 13, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 14,15, }, 	{ 50, 50, }, },
	{ HALF, 0, "",  	{ 16, }, 	{ 100, }, },
	{ HALF, 5, "7",  	{ 16, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw16a[]	= {	/* 16-bar boogie (turn-around ending) */
	{ WHOLE, 5, "7", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 7, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 8,12, }, 	{ 60, 40, }, },
	{ WONCE, 0, "", 	{ 9, }, 	{ 100, }, },
	{ WONCE, 0, "7", 	{ 10, }, 	{ 100, }, },
	{ WONCE, 5, "7",  	{ 11, }, 	{ 100, }, },
	{ WONCE, 6, "o",  	{ 16, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 13, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 14, }, 	{ 100, }, },
	{ WHOLE, 5, "7",  	{ 15, }, 	{ 100, }, },
	{ WHOLE, 6, "o",  	{ 16, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 17, }, 	{ 100, }, },
	{ HALF, 9, "7",  	{ 18, }, 	{ 100, }, },
	{ HALF, 2, "7",  	{ 19, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ 20, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 21, }, 	{ 100, }, },
	{ HALF, 5, "",  	{ 22, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 23, }, 	{ 100, }, },
	{ QUARTER, 7, "7",  	{ 24,25, }, 	{ 50, 50, }, },
	{ QUARTER, 7, "7",  	{ -1, }, 	{ 100, }, },
	{ QUARTER, 6, "7",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Bw16b[]	= {	/* 16-bar boogie (tonic ending) */
	{ WHOLE, 5, "7", 	{ 1, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 2, }, 	{ 100, }, },
	{ WHOLE, 0, "", 	{ 3, }, 	{ 100, }, },
	{ WHOLE, 0, "7", 	{ 4, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 5, }, 	{ 100, }, },
	{ WHOLE, 5, "7", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 7, }, 	{ 100, }, },
	{ WHOLE, 7, "7", 	{ 8, }, 	{ 100, }, },
	{ WONCE, 0, "", 	{ 9, }, 	{ 100, }, },
	{ WONCE, 0, "7", 	{ 10, }, 	{ 100, }, },
	{ WONCE, 5, "7",  	{ 11, }, 	{ 100, }, },
	{ WONCE, 6, "o",  	{ 12, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 13, }, 	{ 100, }, },
	{ HALF, 9, "7",  	{ 14, }, 	{ 100, }, },
	{ HALF, 2, "7",  	{ 15, }, 	{ 100, }, },
	{ HALF, 7, "7",  	{ 16, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 17, }, 	{ 100, }, },
	{ HALF, 5, "",  	{ 18, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};

/* CLASS chord sequences */
CSEQ	Cl2[]	= {	/* Classical 2 bars */
	{ WHOLE, 7, "7",  	{ 1, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Cl4[]	= {	/* Classical 4 bars */
	{ WHOLE, 0, "",  	{ 1,2,3, }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 4, }, 	{ 100, }, },
	{ WHOLE, 5, "6",  	{ 4, }, 	{ 100, }, },
	{ WHOLE, 9, "m6",  	{ 4, }, 	{ 100, }, },
	{ WHOLE, 7, "7",  	{ 5, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Cl8[]	= {	/* Classical 8 bars */
	{ WHOLE, 0, "",  	{ 1,2,3, }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "64",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m64",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "42",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m64",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "64",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "7",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "7",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "64",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 19, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 19, }, 	{ 100, }, },
	{ WHOLE, 7, "7",  	{ 19, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Cl12[]	= {	/* Classical 12 bars */
	{ WHOLE, 0, "",  	{ 1,2,3, }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "64",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 22, }, 	{ 100, }, },
	{ WHOLE, 5, "",  	{ 22, }, 	{ 100, }, },
	{ WHOLE, 2, "m6",  	{ 22, }, 	{ 100, }, },
	{ WHOLE, 0, "64",  	{ 23, }, 	{ 100, }, },
	{ WHOLE, 7, "7",  	{ 24, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ 25, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};
CSEQ	Cl16[]	= {	/* Classical 16 bars */
	{ WHOLE, 0, "",  	{ 1,2,3, }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 4,5,6 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 7,8,9 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 10,11,12 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 13,14,15 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "64",  	{ 16,17,18 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 19,20,21 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "",  	{ 22,23,24 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 22,23,24 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 22,23,24 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m",  	{ 25,26,27 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 25,26,27 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 25,26,27 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 7, "",  	{ 28,29,30 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 2, "m6",  	{ 28,29,30 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 0, "6",  	{ 28,29,30 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m",  	{ 31,32,33 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 9, "m6",  	{ 31,32,33 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 31,32,33 }, 	{ 34, 33, 33, }, },
	{ WHOLE, 5, "",  	{ 34, }, 	{ 100, }, },
	{ WHOLE, 9, "o",  	{ 34, }, 	{ 100, }, },
	{ WHOLE, 2, "m6",  	{ 34, }, 	{ 100, }, },
	{ WHOLE, 0, "64",  	{ 35, }, 	{ 100, }, },
	{ WHOLE, 7, "7",  	{ 36, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ 37, }, 	{ 100, }, },
	{ WHOLE, 0, "",  	{ -1, }, 	{ 100, }, },
};

/* SWING chord sequences */
CSEQ	Igrva[]	= {	/* I Got Rhythm verse A part (2 bars) */
	{ HALF, 0, "",		{ 1,2,3, },	{ 50, 25, 25, }, },
	{ HALF, 9, "m7",	{ 4,5, },	{ 67, 33, }, },
	{ HALF, 1, "o",		{ 4,5, },	{ 67, 33, }, },
	{ HALF, 0, "o",		{ 4,5, },	{ 67, 33, }, },
	{ HALF, 2, "m",		{ 6, },		{ 100, }, },
	{ HALF, 5, "",		{ 6, },		{ 100, }, },
	{ HALF, 7, "7",		{ -1, },	{ 100, }, },
};
CSEQ	Igrvb[]	= {	/* I Got Rhythm verse B part (2 bars) */
	{ HALF, 0, "",		{ 1, },		{ 100, }, },
	{ HALF, 0, "7",		{ 2, },		{ 100, }, },
	{ HALF, 5, "",		{ 3,4,5, },	{ 50, 25, 25, }, },
	{ HALF, 6, "o",		{ -1, },	{ 100, }, },
	{ HALF, 8, "7",		{ -1, },	{ 100, }, },
	{ HALF, 5, "m",		{ -1, },	{ 100, }, },
};
CSEQ	Igrb[]	= {	/* I Got Rhythm bridge (8 bars) */
	{ HALF, 4, "7", 	{ 1,2, }, 	{ 50, 50, }, },
	{ HALF, 4, "7", 	{ 3,4, }, 	{ 50, 50, }, },
	{ HALF, 11, "m7", 	{ 3,4, }, 	{ 50, 50, }, },
	{ HALF, 4, "7", 	{ 5, }, 	{ 100, }, },
	{ HALF, 7, "m6", 	{ 5, }, 	{ 100, }, },
	{ HALF, 4, "7", 	{ 6, }, 	{ 100, }, },
	{ WHOLE, 9, "7",	{ 7,8, }, 	{ 50, 50, }, },
	{ HALF, 9, "7", 	{ 9, }, 	{ 100, }, },
	{ HALF, 4, "m7", 	{ 9, }, 	{ 100, }, },
	{ HALF, 9, "7", 	{ 10, }, 	{ 100, }, },
	{ HALF, 2, "7", 	{ 11,12, }, 	{ 50, 50, }, },
	{ HALF, 2, "7", 	{ 13,14, }, 	{ 50, 50, }, },
	{ HALF, 9, "m7", 	{ 13,14, }, 	{ 50, 50, }, },
	{ HALF, 2, "7", 	{ 15, }, 	{ 100, }, },
	{ HALF, 5, "m6", 	{ 15, }, 	{ 100, }, },
	{ HALF, 2, "7", 	{ 16, }, 	{ 100, }, },
	{ WHOLE, 2, "m7",	{ 17, }, 	{ 100, }, },
	{ WHOLE, 7, "7",	{ -1, },	{ 100, }, },
};
CSEQ	Lbgv[]	= {	/* Lady Be Good verse (6 bars) */
	{ WHOLE, 0, "",		{ 1, },		{ 100, }, },
	{ WHOLE, 5, "7",	{ 2, },		{ 100, }, },
	{ WHOLE, 0, "",		{ 3, },		{ 100, }, },
	{ HALF, 0, "",		{ 4,5, },	{ 60, 40, }, },
	{ HALF, 3, "o",		{ 6,7, },	{ 60, 40, }, },
	{ HALF, 0, "",		{ 6,7, },	{ 60, 40, }, },
	{ WHOLE, 2, "m",	{ 8, },		{ 100, }, },
	{ WHOLE, 2, "7",	{ 8, },		{ 100, }, },
	{ WHOLE, 7, "7",	{ -1, },	{ 100, }, },
};
CSEQ	Lbgb[]	= {	/* Lady Be Good bridge (8 bars) */
	{ WHOLE, 5, "",		{ 1, },		{ 100, }, },
	{ WHOLE, 6, "o",	{ 2, },		{ 100, }, },
	{ WHOLE, 0, "",		{ 3, },		{ 100, }, },
	{ HALF, 0, "",		{ 4,5,6, },	{ 40, 35, 25, }, },
	{ HALF, 4, "m",		{ 7, },		{ 100, }, },
	{ HALF, 4, "7",		{ 7, },		{ 100, }, },
	{ HALF, 0, "",		{ 7, },		{ 100, }, },
	{ WHOLE, 9, "m",	{ 8,9,10, },	{ 50, 30, 20, }, },
	{ WHOLE, 2, "7",	{ 11, },	{ 100, }, },
	{ WHOLE, 2, "m",	{ 11, },	{ 100, }, },
	{ WHOLE, 8, "7",	{ 11, },	{ 100, }, },
	{ WHOLE, 7, "7",	{ 12, },	{ 100, }, },
	{ WHOLE, 7, "7",	{ -1, },	{ 100, }, },
};
CSEQ	Tamv[]	= {	/* You Took Advantage of Me Verse (7 bars) */
	{ HALF, 0, "",		{ 1,2,3, },	{ 50, 25, 25, }, },
	{ HALF, 1, "o",		{ 4,5, },	{ 67, 33, }, },
	{ HALF, 9, "m7",	{ 4,5, },	{ 67, 33, }, },
	{ HALF, 0, "o",		{ 4,5, },	{ 67, 33, }, },
	{ HALF, 7, "7",		{ 6, },		{ 100, }, },
	{ HALF, 2, "m",		{ 6, },		{ 100, }, },
	{ HALF, 7, "7",		{ 7, }, 	{ 100, }, },
	{ HALF, 0, "",		{ 8,9,10, },	{ 50, 25, 25, }, },
	{ HALF, 0, "o",		{ 11,12, },	{ 67, 33, }, },
	{ HALF, 1, "o",		{ 11,12, },	{ 67, 33, }, },
	{ HALF, 9, "m7",	{ 11,12, },	{ 67, 33, }, },
	{ HALF, 2, "m",		{ 13, }, 	{ 100, }, },
	{ HALF, 6, "o",		{ 13, },	{ 100, }, },
	{ HALF, 7, "7",		{ 14, },	{ 100, }, },
	{ HALF, 0, "",		{ 15, }, 	{ 100, }, },
	{ HALF, 0, "7",		{ 16, }, 	{ 100, }, },
	{ HALF, 5, "",		{ 17,18,19, },	{ 50, 25, 25, }, },
	{ HALF, 5, "m",		{ -1, },	{ 100, }, },
	{ HALF, 6, "o",		{ -1, },	{ 100, }, },
	{ HALF, 8, "7",		{ -1, },	{ 100, }, },
};
CSEQ	Tamve1[] = {	/* You Took Advantage of Me Verse end 1 (1 bar) */
	{ HALF, 0, "",		{ 1,2, }, 	{ 67, 33, }, },
	{ HALF, 7, "7",		{ 4, }, 	{ 100, }, },
	{ QUARTER, 8, "7",	{ 3, }, 	{ 100, }, },
	{ QUARTER, 7, "7",	{ 4, }, 	{ 100, }, },
	{ WHOLE, 0, "",		{ -1, }, 	{ 100, }, },
};
CSEQ	Tamve2[] = {	/* You Took Advantage of Me Verse end 1 (1 bar) */
	{ HALF, 0, "",		{ 1,2, }, 	{ 50, 50, }, },
	{ HALF, 7, "7",		{ 4, }, 	{ 100, }, },
	{ QUARTER, 8, "7",	{ 3, }, 	{ 100, }, },
	{ QUARTER, 7, "7",	{ 4, }, 	{ 100, }, },
	{ HALF, 0, "",		{ 5, }, 	{ 100, }, },
	{ QUARTER, 11, "",	{ 6, }, 	{ 100, }, },
	{ QUARTER, 4, "7",	{ -1, }, 	{ 100, }, },
};
CSEQ	Tamb[]	= {	/* You Took Advantage of Me bridge (8 bars) */
	{ HALF, 9, "m", 	{ 1, }, 	{ 100, }, },
	{ HALF, 11, "7", 	{ 2, }, 	{ 100, }, },
	{ HALF, 4, "7", 	{ 3, }, 	{ 100, }, },
	{ HALF, 9, "7", 	{ 4, }, 	{ 100, }, },
	{ HALF, 2, "m", 	{ 5, }, 	{ 100, }, },
	{ HALF, 7, "7", 	{ 6, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 7, }, 	{ 100, }, },
	{ QUARTER, 11, "",	{ 8, }, 	{ 100, }, },
	{ QUARTER, 4, "7",	{ 9, }, 	{ 100, }, },
	{ HALF, 9, "m", 	{ 10, }, 	{ 100, }, },
	{ HALF, 11, "7", 	{ 11, }, 	{ 100, }, },
	{ HALF, 4, "7", 	{ 12, }, 	{ 100, }, },
	{ HALF, 9, "7", 	{ 13, }, 	{ 100, }, },
	{ HALF, 2, "m", 	{ 14, }, 	{ 100, }, },
	{ HALF, 7, "7", 	{ 15, }, 	{ 100, }, },
	{ HALF, 0, "",  	{ 16, }, 	{ 100, }, },
	{ HALF, 7, "7", 	{ -1, }, 	{ 100, }, },
};
CSEQ	Tbbv[]	= {	/* Twelve Bar Blues verse (10 bars) */
	{ WHOLE, 0, "",  	{ 1, },		{ 100, }, },
	{ WHOLE, 5, "",  	{ 2, },		{ 100, }, },
	{ WHOLE, 0, "",  	{ 3, },		{ 100, }, },
	{ WHOLE, 0, "7",	{ 4, },		{ 100, }, },
	{ WHOLE, 5, "",  	{ 5,6, },	{ 50, 50, }, },
	{ WHOLE, 5, "",  	{ 7, },		{ 100, }, },
	{ WHOLE, 6, "o",  	{ 7, },		{ 100, }, },
	{ HALF, 0, "",		{ 8,13, },	{ 50, 50, }, },
	{ HALF, 2, "m",		{ 9, }, 	{ 100, }, },
	{ HALF, 4, "m",		{ 10, }, 	{ 100, }, },
	{ HALF, 3, "o",		{ 11, }, 	{ 100, }, },
	{ WHOLE, 2, "m",	{ 12, }, 	{ 100, }, },
	{ WHOLE, 7, "7",	{ -1, }, 	{ 100, }, },
	{ HALF, 0, "",		{ 14, },	{ 100, }, },
	{ WHOLE, 0, "",  	{ 15, }, 	{ 100, }, },
	{ WHOLE, 7, "7",	{ 16, }, 	{ 100, }, },
	{ HALF, 5, "",		{ 17,18, },	{ 50, 50, }, },
	{ HALF, 7, "7", 	{ -1, },	{ 100, }, },
	{ HALF, 5, "",  	{ -1, },	{ 100, }, },
};
CSEQ	Tturn[]	= {	/* turnaround ending on the tonic */
	{ HALF, 0, "",		{ 1,2,3,4, },	{ 40, 30, 15, 15, }, },
	{ HALF, 7, "7",		{ 6, },		{ 100, }, },
	{ HALF, 0, "",		{ 6, },		{ 100, }, },
	{ QUARTER, 2, "m",	{ 5, },		{ 100, }, },
	{ QUARTER, 8, "7",	{ 5, },		{ 100, }, },
	{ QUARTER, 7, "7",	{ 6, },		{ 100, }, },
	{ WHOLE, 0, "",		{ -1, },	{ 100, }, },
};
CSEQ	Dturn[]	= {	/* turnaround ending on the dominant */
	{ HALF, 0, "",		{ 1,2,3,4, },	{ 50, 16, 16, 17, }, },
	{ HALF, 7, "7",		{ 5,6,7, },	{ 25, 50, 25, }, },
	{ HALF, 9, "m7",	{ 5,8, },	{ 60, 40, }, },
	{ HALF, 1, "o",		{ 5,8, },	{ 60, 40, }, },
	{ HALF, 3, "9",		{ 9, },		{ 100, }, },
	{ HALF, 2, "m",		{ 11, },	{ 100, }, },
	{ HALF, 1, "",		{ 11, },	{ 100, }, },
	{ HALF, 5, "",		{ 11, },	{ 100, }, },
	{ HALF, 8, "7",		{ 11, },	{ 100, }, },
	{ HALF, 2, "9",		{ 10, },	{ 100, }, },
	{ HALF, 1, "9",		{ -1, },	{ 100, }, },
	{ HALF, 7, "7",		{ -1, },	{ 100, }, },
};
/* SWING song sequences */
SSEQ	Igrq[]	= {		/* I Got Rhythm, quarter (8 bars) */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Igrh[]	= {		/* I Got Rhythm, half (16 bars) */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Dturn, },		/* 2 bars */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Igr[]	= {		/* I Got Rhythm, full 32 bars */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Dturn, },		/* 2 bars */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 1, "Igrb", Igrb, },		/* 8 bars */
	{ 2, "Igrva", Igrva, },		/* 4 bars */
	{ 1, "Igrvb", Igrvb, },		/* 2 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Lbgq[]	= {		/* Lady Be Good, quarter (8 bars) */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Lbgh[]	= {		/* Lady Be Good, half (16 bars) */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Dturn, },		/* 2 bars */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Lbg[]	= {		/* Lady Be Good, full 32 bars */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Dturn, },		/* 2 bars */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 1, "Lbgb", Lbgb, },		/* 8 bars */
	{ 1, "Lbgv", Lbgv, },		/* 6 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Tam[]	= {		/* You Took Advantage of Me, 24 bars */
	{ 1, "Tamv", Tamv, },		/* 7 bars */
	{ 1, "Tamve2", Tamve2, },	/* 1 bar */
	{ 1, "Tamb", Tamb, },		/* 8 bars */
	{ 1, "Tamv", Tamv, },		/* 7 bars */
	{ 1, "Tamve1", Tamve1, },	/* 1 bar */
	{ 0, },
};
SSEQ	Tbb[]	= {		/* Twelve Bar Blues, 12 bars */
	{ 1, "Tbbv", Tbbv, },		/* 10 bars */
	{ 1, 0, Dturn, },		/* 2 bars */
	{ 0, },
};
SSEQ	Tbbe[]	= {		/* Twelve Bar Blues w/end, 12 bars */
	{ 1, "Tbbv", Tbbv, },		/* 10 bars */
	{ 1, 0, Tturn, },		/* 2 bars */
	{ 0, },
};

char	*Pcname[]	= {
	"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B",
};
int	Mono	= 0;		/* don't change chord roots */
int	Style;

main(argc, argv)
char	*argv[];
{
	int i, num, key;
	double bars;
	extern double atof();

	key = 0;
	if (argc < 2) {
syntax:
	    fprintf(stderr,
	     "Usage: %s -bBARS [-kKEY] [-sSTYLE]\n", argv[0]);
	    fprintf(stderr, "BARS is the length in bars.\n");
	    fprintf(stderr, "KEY is C, or, C#, or Eb, or ...");
	    fprintf(stderr, "STYLE is one of:");
	    for (i = 0; S[i].name; i++)
		fprintf(stderr, " %s,", S[i].name);
	    exit(2);
	}
	for (Style = 0; S[Style].name; Style++)
	    if (strcmp(DEFSTYLE, S[Style].name) == 0)
		break;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'b':
		    bars = atof(&argv[i][2]);
		    if (bars <= 0)
			goto syntax;
		    break;
		case 'k':
		    key = pc2key(&argv[i][2]);
		    break;
		case 's':
		    for (Style = 0; S[Style].name; Style++)
			if (strcmp(&argv[i][2], S[Style].name) == 0)
			    break;
		    if (!S[Style].name)
			goto syntax;
		    break;
		default:
		    goto syntax;
		}
	    } else
		goto syntax;
	}
	srand(i = (int) time(0));
	printf("# Title\t%d\n", i);
	printf("#STYLE\t%s\n", S[Style].name);
	if (*S[Style].defs)
	    printf("#INCLUDE\t\"%s/%s\"\n", BASE,S[Style].defs);
	printf("#QUANT\t%s\n", S[Style].quant);
	num = (bars * S[Style].barlen) / S[Style].cpq;
	(*S[Style].comp)(num, key);
	exit(0);
}

void
grass(num, key)
{
	char buf[512];
	int cpq, qpb, i, j, bars, rnd;
	SSEQ *sp;
	static int intro;

	cpq = S[Style].cpq;
	qpb = WHOLE / cpq;
	if (!intro++) {				/* first bar or part of a bar */
	    put("\n#PART Intro\n");
	    if (i = ((num - 1) % qpb) + 1) {
		for (num -= i; --i >= 0; put("-   "));
		put("\n");				/* flush */
	    }
	}
        // note: original code was "rnd = rand(0)". Was the intention to call srand(0) 
        // to make it "reproducible" and then call rnd = rand()?
	rnd = rand();				/* to make it reproducible */
	bars = num / qpb;			/* may be odd */
	if ((bars % 16) == 9 && bars > 9) {	/* 16-bar + 9 form */
	    sp = Bmt;
	    for (i = bars / 16; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    sp = Bmth;
	    putsong(sp, key, cpq, 0);
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 16) == 1) {		/* 16-bar + 1 form */
	    sp = PR50? Bmt : Bb;
	    for (i = bars / 16; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 12) == 1) {		/* 12-bar + 1 form */
	    sp = PR50? Bm : Bbs;
	    for (i = bars / 12; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 12) == 0) {		/* 12-bar form */
	    sp = PR(80)? Bm : Bbs;
	    for (i = bars / 12; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	} else if ((bars % 8) == 2) {		/* 8-bar + 2 form */
	    sp = PR(33)? Fmb : (PR50? Bmth : Bbh);
	    for (i = bars / 9; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    put(genseq("End1", End1, key, cpq, buf));
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 8) == 0) {		/* 8-bar form */
	    sp = Fmb;
	    for (i = bars / 8; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	} else if ((bars % 4) == 1) {		/* 4-bar + 1 form */
	    sp = PR50? Lm : Dw;
	    for (i = bars / 4; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 4) == 0) {		/* 4-bar form */
	    sp = PR50? Lm : Dw;
	    for (i = bars / 4; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	} else if ((bars % 5) == 1) {		/* 5-bar + 1 form */
	    sp = PR50? Lmw : Dww;
	    for (i = bars / 5; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	    put(genseq("End1", End1, key, cpq, buf));
	} else if ((bars % 5) == 0) {		/* 5-bar form */
	    sp = Lmw;
	    for (i = bars / 5; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	} else {				/* whittle it down */
	    grass(num - qpb, key);
	    put(cname(key, ""));
	    for (j = qpb; --j > 0; put("/   "));
	}
	put("\n");
}

void
boogi(num, key)
{
	char buf[1024];
	int cpq, qpb, i, bars;
	CSEQ *sp;

	cpq = S[Style].cpq;
	qpb = WHOLE / cpq;
	bars = num / qpb;
	if (i = num % (2 * qpb)) {		/* residue mod 2-bars */
	    put("\n#PART Intro\n");
	    num -= i;
	    put(cname(key, ""));
	    for (; --i > 0; put("/   "));
	    put("\n");
	}
	bars = num / qpb;			/* guaranteed even */
	if ((bars % 12) == 0) {			/* 12 bar form (basic) */
	    sp = (rand() & 010)? Bw12ad : Bw12b;
	    for (i = bars / 12; --i >= 0; ) {
		if (i == 0 && sp == Bw12ad)
		    sp = Bw12at;
		put(genseq("Bw12", sp, key, cpq, buf));
	    }
	} else if ((bars % 16) == 0) {		/* 16 bar form */
	    for (i = bars / 16; --i >= 0; ) {
		sp = i? Bw16a : Bw16b;
		put(genseq("Bw16", sp, key, cpq, buf));
	    }
	} else if ((bars % 12) == 8) {		/* extra 8 bars */
	    sp = (rand() & 010)? Bw8a : Bw8b;
	    if (PR50) {				/* 8-bar intro */
		put(genseq("Bw8", sp, key, cpq, buf));
		put("\n");
		if ((i = num - 8 * qpb) > 0)
		    boogi(i, key);
	    } else {				/* 8-bar outro */
		if ((i = num - 8 * qpb) > 0)
		    boogi(i, key);
		put(genseq("Bw8", sp, key, cpq, buf));
		put("\n");
	    }
	} else if ((bars % 12) == 4) {		/* 2-bar outro */
	    if ((i = num - 2 * qpb) > 0)
		boogi(i, key);
	    put("\n#PART Outro\n");
	    put(cname(key, ""));
	    for (i = 8; --i > 0; put("/   "));
	    put("\n");
	} else if (bars >= 2) {			/* 2-bar intro */
	    put("\n#PART Vamp\n");
	    put(cname(key, ""));
	    for (i = 8; --i > 0; put("/   "));
	    put("\n");
	    if ((i = num - 2 * qpb) > 0)
		boogi(i, key);
	}
	put("\n");
}

void
class(num, key)
{
	char buf[512];
	int cpq, qpb, i, j, bars;

	cpq = S[Style].cpq;
	qpb = WHOLE / cpq;
	if (i = num % (2 * qpb)) {		/* residue mod 2-bars */
	    num -= i;
	    if (j = (i % qpb)) {		/* partial bar */
		put(cname(key, ""));
		for (; --j > 0; put("/   "));
	    }
	    if (i >= qpb) {			/* odd bar */
		put(cname(key + 7, "7"));
		for (j = qpb; --j > 0; put("/   "));
	    }
	    put("\n");				/* flush */
	}
	bars = num / qpb;			/* guaranteed even */
	if (bars <= 0)
	    return;
	if ((bars % 16) == 0) {			/* 16-bar form */
	    for (i = bars / 16; --i >= 0; )
		put(genseq("Cl16", Cl16, key, cpq, buf));
	} else if ((bars % 12) == 0) {		/* 12-bar form */
	    for (i = bars / 12; --i >= 0; )
		put(genseq("Cl12", Cl12, key, cpq, buf));
	} else if ((bars % 8) == 0) {		/* 8-bar form */
	    if (bars > 8)
		class(num - 8 * qpb, key);
	    put("\n");
	    put(genseq("Cl8", Cl8, key, cpq, buf));
	} else if ((bars % 4) == 0) {		/* 4-bar form */
	    if (bars > 4)
		class(num - 4 * qpb, key);
	    put("\n");
	    put(genseq("Cl4", Cl4, key, cpq, buf));
	} else {				/* whittle it down */
	    class(num - 2 * qpb, key);
	    put("\n");
	    put(genseq("Cl2", Cl2, key, cpq, buf));
	}
	put("\n");
}

void
mozar(num, key)
{
	int cpq, qpb, i, bars;

	cpq = S[Style].cpq;
	qpb = S[Style].barlen / cpq;		/* waltz time */
	if (i = num % qpb) {			/* residue mod one bar */
	    num -= i;
	    for (put(cname(key + 7, "")); --i > 0; put("/   "));
	    put("\n");				/* flush */
	}
	for (bars = num / qpb; --bars >= 0; ) {
	    put(cname(key, ""));
	    for (i = qpb; --i > 0; put("/   "));
	}
	put("\n");
}

void
swing(num, key)
{
	char buf[1024];
	int cpq, qpb, i, j, bars, rnd;
	SSEQ *sp;

	cpq = S[Style].cpq;
	qpb = WHOLE / cpq;
	bars = num / qpb;
	if (PR((16 - bars) * 2)) {
	    Mono++;				/* make it monotone */
	    num -= 2 * qpb;			/* with Tturn at end */
	}
	if (i = num % (2 * qpb)) {		/* residue mod 2-bars */
	    put("\n#PART Intro\n");
	    num -= i;
	    if (j = (i % qpb)) {		/* partial bar */
		put(cname((key + 7) % 12, "7"));
		for (; --j > 0; put("/   "));
	    }
	    if (i >= qpb)			/* odd bar */
		put(genseq((char *) 0, Dturn, key, 2 * cpq, buf));  /* fast */
	    put("\n");
	}
        // note: originally "rnd = rand(0)" -- see "reproducible" note above
	rnd = rand();				/* to make it reproducible */
	bars = num / qpb;			/* guaranteed even */
	if ((bars % 32) == 0) {			/* 32 bar form */
	    sp = (rand() & 010)? Igr : Lbg;
	    for (i = bars / 32; --i >= 0; )
		putsong(sp, key, cpq, rnd);
	} else if ((bars % 24) == 0) {		/* 24-bar form */
	    sp = (rand() & 010)? Tbb : Tam;
	    for (i = bars / (sp == Tbb? 12 : 24); --i >= 0; ) {
		sp = (sp == Tbb && i == 0)? Tbbe : sp;
		putsong(sp, key, cpq, rnd);
	    }
	} else if ((bars % 16) == 0) {		/* 16-bar form */
	    sp = (rand() & 010)? Igr : Lbg;
	    for (; bars >= 32; bars -= 32)	/* (2n+1) * 16? */
		putsong(sp, key, cpq, rnd);
	    put("\n");
	    sp = sp == Igr? Igrh : Lbgh;
	    putsong(sp, key, cpq, 0);
	} else if ((bars % 12) == 0) {		/* 12-bar form */
	    sp = Tbb;
	    for (i = bars / 12; --i >= 0; ) {
		sp = (i == 0)? Tbbe : sp;
		putsong(sp, key, cpq, rnd);
	    }
	} else if ((bars % 8) == 0) {		/* 8-bar form */
	    sp = (rand() & 010)? Igr : Lbg;
	    for (; bars >= 32; bars -= 32)	/* (4n+1) * 8? */
		putsong(sp, key, cpq, rnd);
	    put("\n");
	    sp = sp == Igr? Igrh : Lbgh;
	    for (; bars >= 16; bars -= 16)	/* (2n+1) * 8? */
		putsong(sp, key, cpq, rnd);
	    put("\n");
	    sp = sp == Igrh? Igrq : Lbgq;
	    putsong(sp, key, cpq, 0);
	} else if (bars > 2) {				/* whittle it down */
	    put(genseq("Intro", Dturn, key, cpq, buf));	/* vamp */
	    put("\n");
	    swing(num - 2 * qpb, key);		/* and try again */
	} else {				/* last two bars... */
	    put(genseq("Outro", Tturn, key, cpq, buf));
	}
	put("\n");
	if (Mono)
	    put(genseq("Outro", Tturn, key, cpq, buf));
}

void putsong(sp, key, cpq, rnd)
SSEQ	*sp;
{
	char buf[2048];
	int i;

	if (rnd)
	    srand(rnd);
	for (; sp->rpts; sp++) {
	    genseq(sp->name, sp->cseq, key, cpq, buf);
	    for (i = sp->rpts; --i >= 0; put(buf));
	}
}

char	*
genseq(head, csa, key, cpq, buf)      /* generate sequence *csa in spec. key */
char	*head, *buf;
CSEQ	csa[];
{
	register char *cp, *bp;
	register CSEQ *sp;
	int i, r, s;

	if (head) {
	    put("\n#PART ");
	    put(head);
	    put("\n");
	}
	cp = buf;
	for (s = 0; s >= 0;) {
	    sp = &csa[s];
	    if (sp->dura) {
		if (sp->dura & STOP)
		    bp = "-   ";
		else
		    bp = cname(key + (Mono? 0 : sp->root), sp->type);
		cp = strcopy(cp, bp);
		i = (sp->dura & ~(ONCE | STOP));
		bp = (i == sp->dura)? "/   " : "-   ";
		for (i = i / cpq; --i > 0; cp = strcopy(cp, bp));
	    }
	    r = rand() % 100;
	    for (i = 0; i < MAXNXT && (r -= sp->prob[i]) >= 0; i++);
	    s = i < MAXNXT? sp->next[i] : -1;
	}
	return(buf);
}

char	*
cname(root, type)
char	*type;
{
	register char *cp;
	static char buf[8];

	cp = strcopy(buf, Pcname[root % 12]);
	cp = strcopy(cp, type);
	for (*cp++ = ' '; cp - buf < 4; *cp++ = ' ');
	*cp = '\0';
	return(buf);
}

void put(str)
char	*str;
{
	static char buf[256], *bp, c;

	if (!bp)
	    bp = buf;
	if (*str == '\n') {
	    bp = flush(buf, bp);
	    str++;
	}
	while (*bp = *str++)
	    if ((c = *bp++) == '\n' || (bp - buf >= 63 && c <= ' '))
		bp = flush(buf, c == '\n'? bp - 1: bp);
}

char	*
flush(buf, bp)
char	*buf, *bp;
{
	if (bp <= buf)
	    return(buf);
	while (*--bp == ' ');
	bp[1] = '\n';
	bp[2] = '\0';
	for (bp = buf; *bp == ' '; bp++);
	fputs(bp, stdout);
	return(buf);
}
