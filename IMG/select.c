/*
**	SELECT -- Select specified parts of MIDI input
**	psl 11/85
*/
#include <stdlib.h>
#include	<stdio.h>
#include	"midi.h"

#define	MAXCHAN	MIDI_MAX_CHANS
#define	MAXKEY	128
#define	MAXVEL	128
#define	MAXCNT	128
#define	MAXPRG	128
#define	MAXSYS	16


void argval(char *arg, int map[], int max, int offset);


char	*Progname;
	/* message types */
int	Nall	= 1;			/* note-on/note-off */
int	Aall	= 1;			/* after touch */
int	Call	= 1, Cmap[MAXCNT];	/* control changes */
int	Pall	= 1, Pmap[MAXPRG];	/* program changes */
int	Ball	= 1;			/* pitch bend */
int	Sall	= 1, Smap[MAXSYS];	/* sys & misc */
	/* secondary criteria (requires Nall) */
int	Kall	= 1, Kmap[MAXKEY];	/* key numbers */
int	Vall	= 1, Vmap[MAXVEL];	/* key velocities */
	/* orthogonal criterion */
int	Chall	= 1, Chmap[MAXCHAN];	/* channels */

syntax(f, s)
char	*f, *s;
{
	fprintf(stderr, f, s);
	fprintf(stderr, "Usage: %s [options] <old >new\n", Progname);
	fprintf(stderr, "Options are:\n");
	fprintf(stderr, "-c[#[-#]]\t-c3 for channel 3, -c for no channels\n");
	fprintf(stderr, "-k[#[-#]]\t-k61 for C#4, -k for no key events\n");
	fprintf(stderr, "-v[#[-#]]\t-v64 for key vel 64, -v0 for key-off\n");
	fprintf(stderr, "-C[#[-#]]\t-C7 for vol, -C for no control changes\n");
	fprintf(stderr, "-P[#[-#]]\t-P7 for prog 7, -P for no prog changes\n");
	fprintf(stderr, "-A\t\t-A for no after-touch\n");
	fprintf(stderr, "-B\t\t-B for no pitch-bend\n");
	fprintf(stderr, "-S[#[-#]]\t-S0 for sys excl, -S for no sys or realtime\n");
	fprintf(stderr, "-allbut\t\tto complement criteria (except -only)\n");
	fprintf(stderr, "-only\t\tto change defaults (see below)\n");
	fprintf(stderr, "Defaults without '-only':\n\t");
	fprintf(stderr, "-c1-16  -k0-127  -v0-127  -C0-127  -P1-128  -S0-15\n");
	fprintf(stderr, "Defaults with '-only':\n\t");
	fprintf(stderr, "-c  -k  -v  -C  -P  -S\n");
	exit(2);
}

main(argc, argv)
char	*argv[];
{
	register int status, type, i, allbut;
	long now;
	MCMD *mp;

	Progname = argv[0];
	for (i = 1; i < argc; i++) {		/* pre-pass for only */
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'o':		/* ONLY */
		    Nall = Call = Pall = Aall = Ball = Sall = 0;
		    break;
		}
	    }
	}
	allbut = 0;
	for (i = 1; i < argc; i++) {		/* main pass */
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'A':		/* after-touch */
		    Aall = 0;
		    break;
		case 'a':		/* ALLBUT */
		    allbut ^= 1;
		    break;
		case 'B':		/* pitch-bend */
		    Ball = 0;
		    break;
		case 'C':		/* control change(s) */
		    argval(&argv[i][2], Cmap, MAXCNT, 0);
		    Call = 0;
		    break;
		case 'c':		/* channel number(s) */
		    argval(&argv[i][2], Chmap, MAXCHAN, -1);
		    Chall = 0;
		    break;
		case 'k':		/* key number(s) */
		    argval(&argv[i][2], Kmap, MAXKEY, 0);
		    Kall = 0;
		    Nall = 1;
		    break;
		case 'o':		/* ONLY */
		    break;
		case 'P':		/* program change(s) */
		    argval(&argv[i][2], Pmap, MAXPRG, -1);
		    Pall = 0;
		    break;
		case 'S':		/* system & misc */
		    argval(&argv[i][2], Smap, MAXSYS, 0);
		    Sall = 0;
		    break;
		case 'v':		/* keyvel number(s) */
		    argval(&argv[i][2], Vmap, MAXVEL, 0);
		    Vall = 0;
		    Nall = 1;
		    break;
		default:
		    syntax("Unrecognized option arg: %s\n", argv[i]);
		}
	    } else
		syntax("Unrecognized arg: %s\n", argv[i]);
	}
	if (allbut) {
	    for (i = MAXCHAN; --i >= 0; Chmap[i] ^= 1);
	    for (i = MAXKEY; --i >= 0; Kmap[i] ^= 1);
	    for (i = MAXVEL; --i >= 0; Vmap[i] ^= 1);
	    for (i = MAXCNT; --i >= 0; Cmap[i] ^= 1);
	    for (i = MAXPRG; --i >= 0; Pmap[i] ^= 1);
	    for (i = MAXSYS; --i >= 0; Smap[i] ^= 1);
	}
	now = 0L;
	while (mp = getmcmd(stdin, now)) {
	    now = mp->when;
	    status = mp->cmd[0];
	    type = (status & M_CMD_MASK);
	    if (type == 0xF0) {			/* these have no channel */
		if (Sall || Smap[status & 0x0F])
		    putmcmd(stdout, mp);
		continue;
	    }
	    if (!Chall && !Chmap[status & M_CHAN_MASK])
		continue;
	    if (type == CH_KEY_OFF) {
		if (Nall
		 && (Kall || Kmap[mp->cmd[1]])
		 && (Vall || Vmap[0]))
		    putmcmd(stdout, mp);
	    } else if (type == CH_KEY_ON) {
		if (Nall
		 && (Kall || Kmap[mp->cmd[1]])
		 && (Vall || Vmap[mp->cmd[2]]))
		    putmcmd(stdout, mp);
	    } else if (type == CH_POLY_KPRS) {
		if (Aall)
		    putmcmd(stdout, mp);
	    } else if (type == CH_CTL) {
		if (Call || Cmap[mp->cmd[1]])
		    putmcmd(stdout, mp);
	    } else if (type == CH_PRG) {
		if (Pall || Pmap[mp->cmd[1]])
		    putmcmd(stdout, mp);
	    } else if (type == CH_PRESSURE) {
		if (Aall)
		    putmcmd(stdout, mp);
	    } else if (type == CH_P_BEND) {
		if (Ball)
		    putmcmd(stdout, mp);
	    } else {
		fprintf(stderr, "Weird MIDI status : %02x\n", status);
		putmcmd(stdout, mp);
	    }
	}
	exit(0);
}

void argval(arg, map, max, offset)
char	*arg;
int	map[];
{
	register char *cp;
	register int i, lo, hi;

	if (*arg == '\0')
	    return;
	lo = hi = atoi(arg) + offset;
	for (cp = arg; '0' <= *cp && *cp <= '9'; cp++);
	if (*cp == ':' || *cp == '-') {
	    if ((i = atoi(++cp) + offset) < lo)
		hi = max - 1;
	    else
		hi = i;
        }
	if (lo < 0 || hi >= max)
	    syntax("Value in '%s' out of range\n", arg);
	for (i = lo; i <= hi; map[i++] = 1);
}
