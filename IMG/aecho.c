/*
**	aecho -- Echoes for /dev/audio data
**	psl 10/89
*/
#include	"audio.h"

main(argc, argv)
char	*argv[];
{
	int i, delay, feedback, gain, thresh;
	double edb, fdb, tdb;
	extern double atof(), pow();

	edb = 6.;
	fdb = 99.;
	tdb = 30.;
	delay = SPS / 10;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'd':
		    delay = SPS * atof(&argv[i][2]);
		    break;
		case 'e':
		    edb = atof(&argv[i][2]);
		    break;
		case 'f':
		    fdb = atof(&argv[i][2]);
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
	if (edb < 0 || fdb < 0 || tdb < 0) {
syntax:
	    fprintf(stderr, "Usage: %s [-d#.#] [-e#.#] [-f#.#] [-t#.#]\n",
	     argv[0]);
	    fprintf(stderr, "-d0.1\tEcho delay is 0.1 seconds\n");
	    fprintf(stderr, "-e6\tEcho is 6 dB down (1/2 volume)\n");
	    fprintf(stderr, "-f99\tEcho feedback is 99 dB down.\n");
	    fprintf(stderr, "-t30\tCutoff threshold is 30 dB down.\n");
	    exit(2);
	}
	gain = UNITY * pow(10., -edb / 10.);
	feedback = UNITY * pow(10., -fdb / 10.);
	thresh = B13RNG * pow(10., -tdb / 10.);
	thresh = Sb2i[-thresh];
	echo(stdin, stdout, delay, gain, feedback, thresh);
	exit(0);
}

echo(ifp, ofp, delay, gain, feedback, thresh)
FILE	*ifp, *ofp;
int	delay, gain, feedback, thresh;
{
	register int out, in, *dlp, i, silences, *dl;

	if ((dl = (int *) sbrk(delay * sizeof *dl)) < 0) {
	    perror("sbrk()");
	    exit(1);
	}
	for (dlp = &dl[delay]; --dlp >= dl; *dlp = 0);
	silences = 1024;
	for (i = 0; (in = getc(ifp)) != EOF || silences < 1024; i++) {
	    dlp = &dl[i % delay];
	    in = (in == EOF)? B13ZERO : I2ub[in];
	    out = in + *dlp;
	    out = (out < 0)? 0 : ((out > B13MAX)? B13MAX : out);
	    out = Ub2i[out];
	    putc(out, ofp);
	    silences = ((out & 0x7F) > thresh)? (silences + 1) : 0;
	    in -= B13ZERO;
	    *dlp = (in * gain + *dlp * feedback) / UNITY;
	}
}

