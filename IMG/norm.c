/*
**	NORM -- Normalize gain level
**	psl 3/90
*/
#include	"audio.h"

int	Vflg	= 0;

main(argc, argv)
char	*argv[];
{
	char *infile, ifbuf[64], buf[4096];
	int i, rmf, fh, max, level;
	double dbdown;
	FILE *ifp;
	extern double atof(), pow();

	dbdown = 1.0;
	rmf = 1;
	infile = (char *) 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'd':
		    if ((rmf = SPS * atof(&argv[i][2]) / 1000.) <= 0)
			goto syntax;
		    break;
		case 'l':
		    if ((dbdown = atof(&argv[i][2])) < 0)
			goto syntax;
		    break;
		case 'v':
		    Vflg++;
		    break;
		default:
		    goto syntax;
		}
	    } else if (infile == (char *) 0)
		infile = argv[i];
	    else
		goto syntax;
	}
	if (!infile) {
	    sprintf(infile = ifbuf, "/tmp/norm%d", getpid());
	    if ((fh = creat(infile, 0644)) < 0) {
		perror(infile);
		exit(1);
	    }
	    while ((i = read(0, buf, sizeof buf)) > 0)
		write(fh, buf, i);
	    close(fh);
	}
	if (!(ifp = fopen(infile, "r"))) {
	    perror(infile);
syntax:
	    fprintf(stderr, "Usage: %s [-d#.#] [-l#.#] [-verbose]\n", argv[0]);
	    fprintf(stderr, "-d# is required duration of max (msec) [>0]\n");
	    fprintf(stderr, "-l#.# gives dBs down for output level [99..0]\n");
	    fprintf(stderr, "-l6.0 would be 1/2 the maximum level.\n");
	    fprintf(stderr, "defaults: -d%g -l1.0\n", 1000./SPS);
	    exit(2);
	}
	max = getmax(ifp, rmf);			/* max level (SB13) in file */
	if (Vflg)
	    fprintf(stderr, "Max value before is %d (%x)\n", max, Sb2i[max]);
	level = B13RNG * pow(10., -dbdown / 10.);	/* level we want */
	fseek(ifp, 0L, 0);
	gain(ifp, stdout, level, max);
	if (infile == ifbuf)
	    unlink(infile);
	if (Vflg)
	    fprintf(stderr, "Max value after is %d (%x)\n", level, Sb2i[level]);
	exit(0);
}

int Hist[128];

getmax(ifp, rmf)
FILE	*ifp;
int	rmf;
{
	register int in, max;

	for (; (in = getc(ifp)) != EOF; Hist[in & 0x7F]++);
	for (max = 0; (rmf -= Hist[max]) > 0 && max < 0x7F; max++);
	return(-I2sb[max]);
}

gain(ifp, ofp, level, max)
FILE	*ifp, *ofp;
register int	level, max;
{
	register int v;

	while ((v = getc(ifp)) != EOF) {
	    v = (I2sb[v] * level) / max;
	    putc(Sb2i[SB13LIMIT(v)], ofp);
	}
}

