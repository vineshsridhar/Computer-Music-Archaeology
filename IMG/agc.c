/*
**	AGC -- Automatic Gain Control
**	psl 3/90
*/
#include	"audio.h"

#define	MAXWIN	5*SPS

struct	cbstr	{
	short	*bsp, *bep;	/* physical start and end pointers */
	short	*bip, *bop;	/* input and output pointers */
	short	maxv, *bmp;	/* max value & loc of max value */
	int	minmax;		/* lowest legal maximum */
} Buf;

main(argc, argv)
char	*argv[];
{
	int i, tix;
	double dlevel, damp, dbdown, dbmax, dmaxg;
	extern double atof(), pow();

	damp = 0.5;
	dbdown = 1.0;
	dbmax = 3.0;
	tix = SPS;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'd':
		    damp = atof(&argv[i][2]);
		    if (damp < 0. || damp > 1.)
			goto syntax;
		    break;
		case 'l':
		    dbdown = atof(&argv[i][2]);
		    if (dbdown < 0)
			goto syntax;
		    break;
		case 'm':
		    dbmax = atof(&argv[i][2]);
		    if (dbmax < 0)
			goto syntax;
		    break;
		case 'w':
		    tix = SPS * atof(&argv[i][2]);
		    if (tix < 0 || tix > MAXWIN)
			goto syntax;
		    break;
		default:
		    goto syntax;
		}
	    } else {
syntax:
		fprintf(stderr, "Usage: %s [-d#.#] [-l#.#] [-w#.#]\n", argv[0]);
		fprintf(stderr, "-d#.# gives the response damping [0..1]\n");
		fprintf(stderr, "-l#.# gives dBs down for level [99..0]\n");
		fprintf(stderr, "-l6.0 would be 1/2 the maximum level.\n");
		fprintf(stderr, "-m#.# gives maximum gain in dBs [0..99]\n");
		fprintf(stderr, "-w#.# is window width in seconds [0..10]\n");
		fprintf(stderr, "defaults: -d0.5 -l1.0 -m3.0 -w1.0\n");
		exit(2);
	    }
	}
	dlevel = pow(10., -dbdown / 10.);
	dmaxg = pow(10., dbmax / 10.);
	agc(stdin, stdout, dlevel, dmaxg, damp, tix);
	exit(0);
}

agc(ifp, ofp, dlevel, dmaxg, damp, tix)
FILE	*ifp, *ofp;
int	tix;
double	dlevel, dmaxg, damp;
{
	int i, in, out, avgval, level;

	level = B13RNG * dlevel;
	if ((i =  sbrk((tix + 2) * sizeof (short))) == -1) {
	    perror("sbrk()");
	    exit(1);
	}
	Buf.bsp = Buf.bip = Buf.bop = Buf.bmp = (short *) i;
	Buf.bep = &Buf.bsp[tix + 2];
	Buf.maxv = Buf.minmax = level / dmaxg;
	avgval = level;
	/* first, fill buffer */
	for (i = tix; --i >= 0 && (in = getc(ifp)) != EOF; ) {
	    buffin(I2sb[in], &Buf);
	    avgval = (damp * avgval + (1. - damp) * Buf.maxv);
	}
	/* normal in & out */
	while ((in = getc(ifp)) != EOF) {
	    buffin(I2sb[in], &Buf);
	    avgval = (damp * avgval + (1. - damp) * Buf.maxv);
	    out = (buffout(&Buf) * level) / avgval;
	    putc(Sb2i[SB13LIMIT(out)], ofp);
	}
	/* done, empty buffer */
	avgval = (damp * avgval + (1. - damp) * Buf.maxv);
	while (Buf.bop != Buf.bip) {
	    out = (buffout(&Buf) * level) / avgval;
	    putc(Sb2i[SB13LIMIT(out)], ofp);
	}
	exit(0);
}

buffin(in, bp)
struct	cbstr	*bp;
{
	if (bp->bip >= bp->bep)
	    bp->bip = bp->bsp;
	*bp->bip++ = in;
	if (in < 0)			/* test for new max */
	    in = -in;
	if (in >= bp->maxv) {
	    bp->maxv = in;
	    bp->bmp = bp->bip - 1;
	}
}

buffout(bp)
struct	cbstr	*bp;
{
	register short *vp;
	register int i, ret, maxv;

	if (bp->bop >= bp->bep)
	    bp->bop = bp->bsp;
	ret = *bp->bop;			/* return value */
	*bp->bop = 0;			/* zero out buffer */
	if (bp->bop++ != bp->bmp)	/* compare with max address */
	    return(ret);
	/* losing current max */
	maxv = bp->minmax;
	for (vp = bp->bep; --vp >= bp->bsp; ) {
	    if ((i = *vp) < 0)
		i = -i;
	    if (i > maxv) {
		maxv = i;
		bp->bmp = vp;
	    }
	}
	bp->maxv = maxv;
}

