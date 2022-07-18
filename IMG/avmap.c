/*
**	AVMAP -- Audio Volume Map
**	psl 10/89
*/
#include	"audio.h"

#define	MAXM	64
struct	mapstr	{
	double	ratio;		/* gain ratio at end */
	long	beg;		/* when the change begins (seconds * SPS) */
	long	end;		/* when the change ends (seconds * SPS) */
} Map[MAXM]	= {
	UNITY,	0,	0,
};
int	Nmap	= 1;		/* how many entries in Map[] */
int	Lflg;			/* use logarithmic fade */

extern double atof(), db2d();

main(argc, argv)
char	*argv[];
{
	char *cp;
	int i, n;
	double q;
	FILE *ifp;

	if (argc < 2) {
syntax:
	    fprintf(stderr,
	     "Usage: %s [-l] VSPEC [...] [files or stdin] >new\n", argv[0]);
	    fprintf(stderr, "-l uses a logarithmic (versus linear) fade.\n");
	    fprintf(stderr, "The VSPECs are in the form:\n");
	    fprintf(stderr, " -v#:##:###\n");
	    fprintf(stderr, "\t# is the volume after the fade (in dBs).\n");
	    fprintf(stderr, "\t## is the fade beginning point.\n");
	    fprintf(stderr, "\t### is the length of the fade.\n");
	    fprintf(stderr, "Both beginning and length are in seconds.");
	    fprintf(stderr, "e.g. -v-6:12:2 will fade the volume down to");
	    fprintf(stderr, " half over 2 seconds starting 12 seconds in.\n");
	    exit(2);
	}
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'l':
		    Lflg++;
		    break;
		case 'v':
		    cp = &argv[i][2];
		    Map[Nmap].ratio = db2d(atof(cp));
		    for (; *cp && *cp != ':'; cp++);
		    if (*cp++ != ':')
			goto syntax;
		    Map[Nmap].beg = SPS * atof(cp);
		    for (; *cp && *cp != ':'; cp++);
		    if (*cp++ != ':')
			goto syntax;
		    Map[Nmap].end = Map[Nmap].beg + SPS * atof(cp);
		    if (Map[Nmap].end < Map[Nmap].beg)
			goto syntax;
		    Nmap++;
		    break;
		default:
		    goto syntax;
		}
	    }
	}
/****/for (i = 0; i < Nmap; i++)
/****/ fprintf(stderr, "%g\t%d\t%d\n", Map[i].ratio, Map[i].beg, Map[i].end);
/****/
	n = 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if (ifp = fopen(argv[i], "r")) {
		    avmap(ifp, stdout);
		    fclose(ifp);
		    n++;
		} else {
		    perror(argv[i]);
		    goto syntax;
		}
	    }
	}
	if (n == 0)
	    avmap(stdin, stdout);
	exit(0);
}

avmap(ifp, ofp)
FILE	*ifp, *ofp;
{
	register uchar *bp, *bep;
	uchar buf[2 * SPS];
	int len, d, dur, dt, i;
	double or, nr, r, dr;
	long now, next;

	now = 0L;
	for (i = 1; i <= Nmap; i++) {
	    /* get to the start of the next fade */
	    or = Map[i - 1].ratio;
	    next = (i < Nmap)? Map[i].beg : 0x40000000;
	    for (; now < next; now += len) {
		len = ((now + sizeof buf) > next)? (next - now) : sizeof buf;
		if ((len = fread(buf, sizeof buf[0], len, ifp)) <= 0)
		    return;
		if (or != 1.0) {
		    for (bp = buf, bep = &buf[len]; bp < bep; ) {
			if (Lflg) {
			    d = or * ((*bp & 0x7F) ^ 0x7F) + 0.5;
			    d = (d < 0)? 0 : ((d > 0x7F)? 0x7F : d);
			    d = (d ^ 0x7F) | (*bp & 0x80);
			} else {
			    d = or * I2sb[*bp];
			    d = Sb2i[SB13LIMIT(d)];
			}
			*bp++ = d;
		    }
		}
		fwrite(buf, sizeof buf[0], len, ofp);
	    }
	    /* go through fade */
	    nr = Map[i].ratio;				/* new ratio */
	    dr = nr - or;				/* change in ratio */
	    dur = Map[i].end - Map[i].beg;		/* duration of fade */
	    dt = 0;					/* position in fade */
	    for (next = Map[i].end; now < next; now += len) {
		len = ((now + sizeof buf) > next)? (next - now) : sizeof buf;
		if ((len = fread(buf, sizeof buf[0], len, ifp)) <= 0)
		    return;
		for (bp = buf, bep = &buf[len]; bp < bep; dt++) {
		    r = or + (dr * dt) / dur;
		    if (Lflg) {
			d = r * ((*bp & 0x7F) ^ 0x7F) + 0.5;
			d = (d < 0)? 0 : ((d > 0x7F)? 0x7F : d);
			d = (d ^ 0x7F) | (*bp & 0x80);
		    } else {
			d = r * I2sb[*bp];
			d = Sb2i[SB13LIMIT(d)];
		    }
		    *bp++ = d;
		}
		fwrite(buf, sizeof buf[0], len, ofp);
	    }
	}
	/*NOTREACHED*/
}
