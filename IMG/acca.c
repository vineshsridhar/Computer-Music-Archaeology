/*
**	ACCA -- Automated Chord Chart Accompanist
**	psl 1/89
*/
#include <stdlib.h>
#include	<stdio.h>
#include <string.h>
#include	"midi.h"
#include "myatoi.h"

#define	MAXCH		512
#define	NCHANS		MIDI_MAX_CHANS
#define	NKEYS		128
#define	NAMELEN		20
#define	GRPLEN		8

#ifndef  BSDIR
#define	BSDIR		"/usr/sau/psl/bs/obj"
#endif

#define	DEFSTYLE	"class"
#define	STYLELEN	6
#define	DBG		if(Debug)fprintf

#define	BARLEN		(MPU_CLOCK_PERIOD<<1)
#define	QUARTER		(MPU_CLOCK_PERIOD>>1)

struct	chstr	{
	char	name[NAMELEN];
	int	trans[12];
	char	group[GRPLEN];
} Ch[MAXCH]	= {
	"-", { 0, }, "",	/* dummy to allow "-" to be used as a rest */
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
	char	*name;
} Tpat[]	= {		/* transition pattern */
	2,	"vamp",
	1,	"tiny",
	1,	"I-II",
	1,	"I-IV",
	1,	"I-V",
	1,	"I-VI",
	2,	"end",
};

int	Motion[12]	= {
	T_VAMP,	T_VAMP,	T_I_II,	T_VAMP,	T_I_II,	T_I_IV,
	T_VAMP,	T_I_V,	T_VAMP,	T_I_VI,	T_VAMP,	T_VAMP,
};

long process(FILE *ifp, long now);
void procntl(char *cp, char *bp, long now);
int find(char *name);
void syntax(char *msg);
long snip(long start, long stop, int ocn, int ncn);
int wrdcmp(char	*ap, char *bp);
void keystate(long when, int v[NCHANS][NKEYS], int keyon);
void snippet(FILE *ifp, long onow, long istart, long istop, int trans[]);

char	Ebuf[128];
char	Part[128];		/* an arbitrary name? */
char	Style[STYLELEN]	= DEFSTYLE;	/* "swing", "samba", etc */
int	Chmap[NCHANS];		/* channel mapping */
int	Cpq;			/* MPU clocks per quantum */
int	Debug		= 0;	/* debugging output */
int	Numchords	= 1;	/* "-" already defined */
int	Tchan[NCHANS];		/* channels to transpose */
int	Vchan[NCHANS];		/* channel key velocity factors (scaled * 64) */
long	Lstart		= 0;	/* time of current input line start */
long	Pstart		= 0;	/* time of most recent "Part" line */

extern	MCMD	Mpu_nop;	/* MPU_NO_OP command */

char	*speel(), *strcopy();

main(argc, argv)
char	*argv[];
{
	register int i, c, v, files;
	char *cp;
	long now;
	FILE *ifp;

	Debug = getenv("DEBUG") != NULL;
	setbuf(stderr, 0);
	files = 0;
	now = 0L;
	for (i = NCHANS; --i >= 0;) {
	    Chmap[i] = i;
	    Tchan[i] = 1;
	    Vchan[i] = 64;
	}
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'c':		/* map channel */
		    for (cp = &argv[i][2]; *cp && *cp != '='; cp++);
		    if (*cp++ != '=')
			goto syntax;
		    v = myatoi(&argv[i][2]) - 1;
		    c = myatoi(cp) - 1;
		    if (c < 0 || c >= NCHANS || v < -1 || v >= NCHANS)
			goto syntax;
		    Chmap[c] = v;
		    break;
		case 'd':		/* debug */
		    Debug++;
		    break;
		case 't':		/* untransposed channel (e.g. drums) */
		    c = myatoi(&argv[i][2]) - 1;
		    if (c < -1 || c >= NCHANS)
			goto syntax;
		    if (c >= 0)
			Tchan[c] = 0;
		    break;
		case 'v':		/* key velocity */
		    for (cp = &argv[i][2]; *cp && *cp != '='; cp++);
		    if (*cp++ != '=') {
			v = atoi(&argv[i][2]);
			for (c = NCHANS; --c >= 0; Vchan[c] = v);
		    } else {
			c = myatoi(&argv[i][2]) - 1;
			if (c < -1 || c >= NCHANS)
			    goto syntax;
			if (c >= 0)
			    Vchan[c] = myatoi(cp);
		    }
		    break;
		default:
syntax:
		    fprintf(stderr,
		     "Usage: %s [-c#=#] [-d] [-t#] [-v[#=]#] [files or stdin]\n",
		     argv[0]);
		    fprintf(stderr, "-cNEW=OLD maps channels\n");
		    fprintf(stderr, "-d turns on debugging output\n");
		    fprintf(stderr, "-tCHAN doesn't transpose CHAN\n");
		    fprintf(stderr, "-vCHAN=F sets channel vel factor\n");
		    fprintf(stderr, "-vF sets global vel factor (64=unity)\n");
		    exit(2);
		}
	    } else {
		if ((ifp = fopen(argv[i], "r")) == NULL)
		    perror(argv[i]);
		else {
		    now = process(ifp, now);
		    fclose(ifp);
		}
		files++;
	    }
	}
	if (!files)
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

	ocn = -1;
	start = Lstart = 0L;
	for (; fgets(buf, sizeof buf, ifp) != NULL; ) {
	    if (Debug > 1)
		fprintf(stderr, "%s", buf);
	    for (bp = buf; *bp && *bp <= ' '; bp++);
	    cp = speel(&bp, 0);
	    if (*cp == '\n' || (cp[0] == '#' && cp[1] == '\0'))
		;
	    else if (*cp == '#')
		procntl(cp, bp, now);
	    else {
		if (Debug == 1)
		    fprintf(stderr, "%s %s", cp, bp);
		for (nlstart = now; *cp; ) {
		    if (*cp == '/') {
			if ((ncn = ocn) == -1)
			    syntax("First line begins with '/'");
		    } else {
			if ((ncn = find(cp)) < 0) {
			    sprintf(Ebuf, "Undefined chord: %s", cp);
			    syntax(Ebuf);
			}
		    }
		    if (ncn != ocn || nlstart == now) {
			start = snip(start, now, ocn, ncn);
			ocn = ncn;
			Lstart = nlstart;
		    }
		    now += Cpq;
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
	int cn, i;
	FILE *fp;

	if (wrdcmp(cp, "#CHORD") == 0) {
	    cp = speel(&bp, 0);
	    if ((cn = find(cp)) == -1 && (cn = Numchords++) >= MAXCH)
		syntax("Too many chords defined");
	    strcopy(Ch[cn].name, cp);
	    cp = speel(&bp, 0);
	    strcopy(Ch[cn].group, cp);
	    for (i = 0; i < 12; i++) {
		Ch[cn].trans[i] = myatoi(bp);
		while (*bp && *bp++ != ',');
	    }
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
	    Cpq = BARLEN / i;
	} else if (wrdcmp(cp, "#STYLE") == 0) {
	    cp = speel(&bp, 0);
	    if (strlen(cp) >= STYLELEN)
		cp[STYLELEN - 1] = '\0';
	    strcopy(Style, cp);
	}
}

long snip(start, stop, ocn, ncn)
long	start, stop;
{
	char file[128];
	int nct, tlen, vlen;
	long time, etime, vstop;
	FILE *fp;

DBG(stderr, "snip(%ld, %ld, %d, %d)\n", start, stop, ocn, ncn);
	if (start >= stop)
	    return(stop);
	if (ncn == -1)
	    nct = T_END;
	else if (stop - start <= QUARTER)	/* tiny piece */
	    nct = T_TINY;
	else
	    nct = Motion[(Ch[ncn].trans[0] - Ch[ocn].trans[0] + 120) % 12];
	if (strcmp(Style, "mozar") == 0		/* no accomp for musical dice */
	 || strcmp(Style, "seque") == 0		/* or pointy sequences */
	 || strcmp(Style, "toner") == 0)	/* or tone rows */
	    return(stop);
	if (Ch[ocn].name[0] == '-') {		/* rests are special */
	    if (nct == T_END) {
		Mpu_nop.when = stop;
		putmcmd(stdout, &Mpu_nop);
	    }
	    return(stop);
	}
	vlen = Tpat[T_VAMP].bars * BARLEN;
	tlen = Tpat[nct].bars * BARLEN;
	vstop = Lstart + tlen * ((stop - Lstart - 1) / tlen);
DBG(stderr, "start=%ld, vstop=%ld, stop=%ld\n", start, vstop, stop);
	time = start;
	if (time < vstop) {
	    sprintf(file,"%s/%s/vamp.%s", BSDIR, Style, Ch[ocn].group);
DBG(stderr, "get time %ld:%ld from %s\n", time, vstop, file);
	    if (!(fp = fopen(file, "r"))) {
		perror(file);
		return(stop);
	    }
	    for (time = start; time < vstop; time = etime) {
		etime = Lstart + ((time - Lstart) / vlen + 1) * vlen;
		if (etime > vstop)
		    etime = vstop;
		snippet(fp, time,
		 (time - Lstart) % vlen, (etime - Lstart - 1) % vlen + 1,
		 Ch[ocn].trans);
	    }
	    fclose(fp);
	}
	if (time < stop) {
	    sprintf(file, "%s/%s/%s.%s",
	     BSDIR, Style, Tpat[nct].name, Ch[ocn].group);
DBG(stderr, "get time %ld:%ld from %s\n", time, stop, file);
	    if (!(fp = fopen(file, "r"))) {
		perror(file);
		return(stop);
	    }
	    snippet(fp, time,
	     (time - Lstart) % tlen, (stop - Lstart - 1) % tlen + 1,
	     Ch[ocn].trans);
	    fclose(fp);
	}
	return(stop);
}

void snippet(ifp, onow, istart, istop, trans)
FILE	*ifp;
long	onow, istart, istop;
int	trans[];
{
	int c, k, dt, cmd, state, v[NCHANS][NKEYS], q;
	long inow;
	MCMD *mp;

DBG(stderr, "snippet(ifp, %ld, %ld, %ld, trans)\n", onow, istart, istop);
	fseek(ifp, 0L, 0);
	for (c = NCHANS; --c >= 0; )
	    for (k = NKEYS; --k >= 0; v[c][k] = 0);
	dt = onow - istart;
	state = 0;
	for (inow = 0L; mp = getmcmd(ifp, inow); ) {
	    inow = mp->when;
	    if (inow >= istop)
		break;
	    cmd = (mp->cmd[0] & M_CMD_MASK);
	    c = (mp->cmd[0] & M_CHAN_MASK);
	    if (cmd < 0xF0) {
		if (Chmap[c] < 0)
		    continue;
		c = Chmap[c];
		mp->cmd[0] = cmd | c;
	    }
	    if (cmd == CH_KEY_OFF) {
		mp->cmd[0] = (CH_KEY_ON | c);
		mp->cmd[2] = 0;
		cmd = CH_KEY_ON;
	    }
	    if (cmd == CH_KEY_ON) {
		k = mp->cmd[1];
		if (Tchan[c])
		    mp->cmd[1] = (k += trans[mp->cmd[1] % 12]);
		if (mp->cmd[2]) {
		    q = (Vchan[c] * mp->cmd[2]) >> 6;
		    mp->cmd[2] = q < 1? 1 : (q > 127? 127 : q);
		}
	    }
	    if (inow >= istart) {
		if (state == 0) {
		    keystate(istart + dt, v, 1);
		    state = 1;
		}
		mp->when += dt;
		putmcmd(stdout, mp);
	    }
	    if (cmd == CH_KEY_ON)
		v[c][k] = mp->cmd[2];
	}
	if (state == 0)
	    keystate(istart + dt, v, 1);
	keystate(istop + dt, v, 0);
}

void keystate(when, v, keyon)
long	when;
int	v[NCHANS][NKEYS];
{
	register int c, k;
	u_char ubuf[4];
	MCMD m;

	m.when = when;
	m.cmd = ubuf;
	m.len = 3;
	for (c = NCHANS; --c >= 0; ) {
	    m.cmd[0] = CH_KEY_ON | c;
	    for (k = NKEYS; --k >= 0; ) {
		if (v[c][k]) {
		    m.cmd[1] = k;
		    m.cmd[2] = keyon? v[c][k] : 0;
		    putmcmd(stdout, &m);
		}
	    }
	}
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

system(cp)			/* for debugging */
const char	*cp;
{
	printf("%s\n", cp);
	return(0);
}
