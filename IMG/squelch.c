/*
**	SQUELCH -- delete very quiet data
**	psl 10/89
*/
#include	"audio.h"

main(argc, argv)
char	*argv[];
{
	int i, thresh, tix;
	double db;
	extern double atof(), pow();

	db = 18.;
	tix = 819;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 's':
		    tix = SPS * atof(&argv[i][2]);
		    if (tix < 0)
			goto syntax;
		    break;
		case 't':
		    db = atof(&argv[i][2]);
		    if (db < 0)
			goto syntax;
		    break;
		default:
		    goto syntax;
		}
	    } else {
syntax:
		fprintf(stderr, "Usage: %s [-s#.#] [-t#.#]\n", argv[0]);
		fprintf(stderr, "-s is attack/release time in seconds.\n");
		fprintf(stderr, "-t gives decibels down for threshold\n");
		fprintf(stderr, "-t6 would be 1/2 the maximum level.\n");
		fprintf(stderr, "defaults: -s0.1 -t18  (1/8 the max level).\n");
		exit(2);
	    }
	}
	thresh = B13ZERO * pow(10., -db / 10.);
	thresh = Sb2i[-thresh];
	gate(stdin, stdout, 1, thresh, tix);
	exit(0);
}
