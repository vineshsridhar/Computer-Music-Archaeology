/*
**	reverb -- revererator
**	psl 10/89
*/
#include	"audio.h"

#define	NT	4		/* number of taps used in reverb() */
#define	TNT	14		/* total number of taps in the table */
#define	MST	(TNT-NT-2)	/* last legal starting place in table */

struct	tapstr	{
	int	num;		/* number of cells */
	int	gain;		/* gain * UNITY */
	int	*dp;		/* current pointer into delay line */
	int	*dl;		/* the delay line */
	int	*dep;		/* end of the delay line + 1 */
} T[TNT]	= {
	{ 241, }, { 269, }, { 293, }, { 331, },
	{ 367, }, { 409, }, { 457, }, { 509, },
	{ 563, }, { 631, }, { 701, }, { 773, },
	{ 863, }, { 953, },
};

main(argc, argv)
char	*argv[];
{
	int i, gain, size, thresh;
	double revtim, rdb, tdb;
	struct tapstr t[4], *tp;
	extern double atof(), pow();

	tdb = 30.;
	rdb = 6.;
	revtim = 1.;
	size = 4;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'g':
		    rdb = atof(&argv[i][2]);
		    break;
		case 'r':
		    revtim = atof(&argv[i][2]);
		    break;
		case 's':
		    size = atoi(&argv[i][2]) - 1;
		    break;
		case 't':
		    tdb = atof(&argv[i][2]);
		    break;
		default:
		    goto syntax;
		}
	    } else
		goto syntax;
	}
	if (revtim < 0. || tdb <= 0. || size < 0 || size > MST) {
syntax:
	    fprintf(stderr,
	     "Usage: %s [-g#.#] [-r#.#] [-s#] [-t#.#]\n", argv[0]);
	    fprintf(stderr, "-g6\tReverberation is 6 dB down (1/2 volume)\n");
	    fprintf(stderr, "-r1.0\tReverb time is 1 second\n");
	    fprintf(stderr, "-s5\tRoom size (range is 1 to %d)\n", MST + 1);
	    fprintf(stderr, "-t30\tCutoff threshold is 30 dB down.\n");
	    exit(2);
	}
	for (i = 0; i < NT; i++) {
	    tp = &T[size + i + (i? 2 : 0)];	/* skip two taps */
	    t[i].num = tp->num;
	    t[i].gain = D2FP(pow(0.001, tp->num / (SPS * revtim)));
	}
	gain = D2FP(pow(10., -rdb / 10.));
	thresh = B13ZERO * pow(10., -tdb / 10.);
	thresh = Sb2i[-thresh];
	reverb(stdin, stdout, t, NT, gain, thresh);
	exit(0);
}

reverb(ifp, ofp, t, nt, gain, thresh)
FILE	*ifp, *ofp;
struct	tapstr *t;
int	nt, thresh;
{
	register uchar *ip, *op;
	uchar inbuf[SPS], outbuf[SPS];
	int out, in, i, silences, tn, x, n, eof;
	struct tapstr *tp;

	for (tn = nt; --tn >= 0; ) {
	    tp = &t[tn];
	    i = tp->num;
	    if ((n = (int) sbrk(i * sizeof (int))) < 0) {
		perror("sbrk()");
		exit(1);
	    }
	    tp->dl = tp->dp = (int *) n;
	    tp->dep = &tp->dl[i];
	}
	for (silences = 1024, eof = 0; !eof || silences < 1024; ) {
	    if (eof || (n = fread(ip = inbuf, sizeof *ip, SPS, ifp)) <= 0)
		n = eof = 1024;
	    for (op = outbuf; op < &outbuf[n]; ) {
		in = eof? 0 : I2sb[*ip++];
		out = 0;
		for (tp = &t[nt]; --tp >= t; ) {
		    x = FPMUL(tp->gain, *tp->dp);
		    out += x;
		    *tp->dp++ = in + x;
		    if (tp->dp >= tp->dep)
			tp->dp = tp->dl;
		}
		out = B13ZERO + in + FPMUL(out, gain);
		*op++ = out = Ub2i[B13LIMIT(out)];
		silences = ((out & 0x7F) > thresh)? (silences + 1) : 0;
	    }
	    fwrite(outbuf, sizeof *op, n, ofp);
	}
}

