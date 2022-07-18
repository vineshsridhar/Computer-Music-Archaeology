/*
**      LIBAMC.H -- Defines for Algorithmic Music Composition routines
**      psl 1/89
*/


/*	Definitions required by both banjoplay & lick routines */

#if defined(BANJO) || defined(LICK)
#define	PATLEN		8	/* # of sixteenths / pattern */
#define	MAXREACH	4	/* how big your hands are */
#define	STRINGS		5
#define	FIRST		0	/* index of first string */
#define	SECOND		1
#define	THIRD		2
#define	FOURTH		3
#define	FIFTH		4	/* index of fifth string */
#define	DIGITS		3
#define	THUMB		0
#define	INDEX		1
#define	MIDDLE		2
#define	OPEN		0	/* fret of open string */
#define	T		1	/* masks for Rhpat[] & Fi[] */
#define	I		2
#define	M		4

static struct	banfstr	{	/* finger info */
	int	mask;		/* Rhpat mask */
	int	lostr;		/* lowest (numbered) string it can reach */
	int	histr;		/* highest (numbered) string it can reach */
	int	bstr;		/* best (favorite) string */
};
extern	int	Tuning[STRINGS];	/* How the banjo is tuned */
extern	int	Nrhpat;		/* number of patterns we can use */
#endif


/*	Definitions required by banjoplay routines */

#ifdef BANJO
typedef	struct	bseqstr {
	int	start;		/* note start time in MPU clocks */
	int	keys[STRINGS];	/* notes (relative to lowest root of key) */
} BSEQ;
typedef	struct	riffstr	{
	int	len;		/* length in MPU clocks */
	int	lo, hi;		/* range of key values */
	int	first;		/* key to use for distance calc */
	BSEQ	*seq;		/* BSEQ str with notes */
} RIFF;
typedef	struct	stepstr	{
	int	rhd;			/* rh digits for this step */
	int	rhs[DIGITS];		/* rh strings for each digit */
	int	lhf[DIGITS];		/* lh frets for each digit's string */
	int	key[DIGITS];		/* pitch for each digit's string */
} STEP;
typedef	struct	patstr	{
	int	lhloc;
	int	pat;
	STEP	step[PATLEN];		/* step info for each step */
} PSEQ;
extern	int	Rhpat[][PATLEN];	/* right-hand patterns */
extern	PSEQ	Last;			/* previous state */
extern	int	Mrhpat;			/* max number of patterns available */
extern	int	Nrhptry;		/* number of pats to try each time */
#endif	// BANJO


/*	Definitions required by lick routines */

#ifdef LICK
#define	MAXCHRD	128		/* max number of unique chords */
#define	MAXLEN	4096		/* max piece length, in sixteenths */
#define	NAMELEN	8
#define	NTONES	13
#define	STOP	0		/* index into chrdstr for first entry */
struct	chrdstr	{
	char	name[NAMELEN];	/* "C", "C#m", "C7", etc. (* => diminished) */
	int	ctones[NTONES];	/* chord tones (0=C, 1=C#, 11=B, -1=end) */
	int	ptones[NTONES];	/* passing tones (0=C, 1=C#, 11=B, -1=end) */
};
extern	char	*Cparm;		/* misc composition parameter for lick1 */
extern	int	Cpat[MAXLEN];	/* chord indices, one per sixteenth note */
extern	int	Plen;		/* how many sixteenths in the piece */
extern	int	Ilhloc;		/* initial left hand location */
extern	struct	chrdstr	Cstr[MAXCHRD];
extern	struct	banfstr	Fi[DIGITS];
extern	FILE	*Trace;
#endif	// LICK


/*	Definitions required by IMG routines */

#ifdef	BS
#define	ACCA	"acca"
#define	ACCL	"accl"
#define	BARS	"bars"
#define	CHMAP	"chmap"
#define	CNTL	"cntl"
#define	INST	"inst"
#define	KMAP	"kmap"
#define	MCLEAN	"mpuclean"
#define	MERGE	"merge"
#define	MKCC	"mkcc"
#define	SINST	"sinst"
#define	TRANS	"transpose"
#define	MAXNDP	4	/* max number of drum parts */
#define	NUMC	16	/* max number of MIDI channels */
#define	NUMS	32	/* max number of styles */
#define	NUMV	128	/* max number of voices (programs) */
#define	NLEN	32	/* name lengths (cnam, vnam) */
#define	SNLEN	32	/* synth/setup name length SYNTH.name */
#define KMLEN	256	/* key map length Synth.kmap */
#define DBG	if(Debug)fprintf

/* channel numbers in the BS accompaniment files */
#define	BCHAN	2
#define	CCHAN	3
#define	DCHAN0	10
#define	DCHAN1	11
#define	DCHAN2	12

/* Dbug, Lock, & Opts toggle masks */
#define	M_MKCC	0x0001
#define	M_ACCA	0x0002
#define	M_ACCL	0x0004
#define	M_BS	0x0008
#define	M_LEN	0x0008		/* note overlap */

#define	UNAVAIL		0	/* used in tempo field of stylstr */

/* D110 default channels */
#define	DEFBC	2
#define	DEFCC	3
#define	DEFDC	10
#define	DEFLC0	4
#define	DEFLC1	5
#define	DEFLC2	6

typedef	struct	stylstr	{
	char	name[6];	/* for programs */
	int	barlen;		/* bar length (MPU clocks) */
	int	units;		/* length quantum (MPU clocks) */
	int	dpchn[4];	/* drum part source chans (1:16) */
	char	desc[32];	/* description (for menu) */
	char	lname[16];	/* for people (files & button labels)*/
	int	key;		/* current key (0:11) */
	int	cnti;		/* current count-in (0:2) */
	char	leng[10];	/* current length */
	int	tempo;		/* current tempo (20:240) */
/* synth-specific */
	int	linst[3];	/* lead instruments (1:128) */
	int	lchan[3];	/* lead chans (1:16) */
	int	lvelo[3];	/* lead velocities (1:127) */
	int	ltrns[3];	/* lead transpositions (0:4) */
	int	lmctl[3];	/* lead modwheel control (1:127) */
	int	lbctl[3];	/* lead breath control (1:127) */
	int	lfctl[3];	/* lead foot control (1:127) */
	int	lvctl[3];	/* lead volume control (1:127) */
	int	lener;		/* lead energy (0:100) */
	int	lpred;		/* lead predictability */
	int	lsfix;		/* lead seed fixed or random */
	char	lseed[16];	/* lead seed */
	int	binst;		/* bass inst (1:128) */
	int	bchan;		/* bass chan (1:16) */
	int	bvelo;		/* bass velocity (1:127) */
	int	btrns;		/* bass transposition (0:4) */
	int	bmctl;		/* bass modwheel control (1:127) */
	int	bbctl;		/* bass breath control (1:127) */
	int	bfctl;		/* bass foot control (1:127) */
	int	bvctl;		/* bass volume control (1:127) */
	int	cinst;		/* chording inst (1:128) */
	int	cchan;		/* chord chan (1:16) */
	int	cvelo;		/* chord velocity (1:127) */
	int	ctrns;		/* chord transposition (0:4) */
	int	cmctl;		/* chord modwheel control (1:127) */
	int	cbctl;		/* chord breath control (1:127) */
	int	cfctl;		/* chord foot control (1:127) */
	int	cvctl;		/* chord volume control (1:127) */
	int	dinst;		/* drum inst (1:128) */
	int	dchan;		/* drum chan (1:16) */
	int	dvelo;		/* drum velocity (1:127) */
	int	dpart;		/* drum part (0:3) */
	int	dmctl;		/* drum modwheel control (1:127) */
	int	dbctl;		/* drum breath control (1:127) */
	int	dfctl;		/* drum foot control (1:127) */
	int	dvctl;		/* drum volume control (1:127) */
} STYLE;

/* synth-specific information (largely unneeded for composition) */
typedef	struct	synthstr	{
	char	name[SNLEN];	/* what the synth setup is called */
	char	misc[128];	/* miscellaneous comment */
	char	init[512];	/* command to initialize the synth(s) */
	char	fini[512];	/* command to reset the synth(s) */
	char	play[128];	/* command to "play" MPU data */
	int	mchn, mkey;	/* default channel & key for metronome */
	char	*kmap[NUMC];	/* MIDI key mapping (for drums) */
#define	NOKMAP		(char *) 0
	char	cnam[NUMC][NLEN];	/* channel names for all channels */
	char	vnam[NUMC][NUMV][NLEN];	/* voice names for all channels */
		/* if vnam[c][0][0] == 0, no voices available */
#define	NOVOICES	{ 0, }
		/* if vnam[c][0][0] > 127, use vnam[vnam[c][0][0] & 0x0F] */
#define	USEC(c)		(0x80|((c)-1))
} SYNTH;

/* These are instantiated in bscomp.c */
extern	char	*Tmpfile;	/* root for intermediate file names */
extern	char	Sysbuf[];	/* for sys() calls */
extern	int	Debug;		/* for trace output */
extern	int	Accaok;		/* is Tmpfile.acc up to date? */
extern	int	Acclok;		/* is Tmpfile.lead up to date? */
extern	int	Mkccok;		/* is Tmpfile.cc up to date? */
#endif	// BS


/*	Definitions required by Stochastic Binary Subdivision routines */

#ifdef	SBSD
/* SBSD (stochastic binary subdivision) defines */
#define MAXI	256		/* max number of instrument defs */
#define MAXRES  64
typedef	struct  INST   {
	int     inum;           /* instrument # or key (note) # */
	int     density;        /* as a % */
	int     up;             /* up- or down- beat */
	int     res;            /* ticks per quantum */
	int     dur;            /* note duration */
	int     vel;            /* key velocity */
	int     chan;           /* MIDI channel for output */
	char    pat[MAXRES];    /* current pattern */
} INST;
typedef	struct	SBSDP	{	/* sbsd parameters */
	int	lolim, hilim;	/* low/high key numbers */
	int	scaled;		/* 0 => not scaled */
	int	scale[12];	/* booleans, starting at C */
	int	lastnote;	/* prior note, for relative motion */
	int	ninst;		/* number of instruments in use */
	INST	*inst;		/* pointer to first instr struct */
} SBSDP;
extern	int	Iflg;		/* != 0 => independent, all can play at once */
extern	int	Jflg;		/* != 0 => join overlapped repeated notes */
#endif	// SBSD
