#include	"audio.h"

#define	MAXPER	1024	/* maximum period (~8 Hz, below C-2) */

extern	double	K2p[];

main(argc, argv)
char	*argv[];
{
	char obuf[10 * SPS];
	int i, damp, seed, twang, vel;
	double dper, ddur;
	extern double atof();

	damp = 1;
	ddur = 1.0;
	dper = K2p[60];
	seed = 12345;
	twang = 15;
	vel = 80;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'd':
		    damp = atoi(&argv[i][2]);
		    break;
		case 'k':
		    dper = K2p[name2key(&argv[i][2])];
		    break;
		case 'l':
		    ddur = atof(&argv[i][2]);
		    break;
		case 'p':
		    dper = atof(&argv[i][2]);
		    break;
		case 's':
		    seed = atoi(&argv[i][2]);
		    break;
		case 't':
		    twang = atoi(&argv[i][2]);
		    break;
		case 'v':
		    vel = atoi(&argv[i][2]);
		    break;
		default:
		    goto syntax;
		}
	    } else {
syntax:
		fprintf(stderr, "Usage: %s [options] >file\n", argv[0]);
		fprintf(stderr, "-d#\tset damping [0..9]\n");
		fprintf(stderr, "-k#\tset MIDI key number [0..127|C-2..G8]\n");
		fprintf(stderr, "-l#.#\tset length (seconds)\n");
		fprintf(stderr, "-p#.#\tset period (seconds)\n");
		fprintf(stderr, "-s#\tset random number seed\n");
		fprintf(stderr, "-t#\tset twang [0..127]\n");
		fprintf(stderr, "-v#\tset velocity [0..127]\n");
		fprintf(stderr, "defaults: -d%d -l%g -p%g -s%d -t%d -v%d\n",
		 damp, ddur, dper, seed, twang, vel);
		exit(2);
	    }
	}
	srand(seed);
	ipluck(dper, ddur, vel, damp, twang, obuf);
	write(1, obuf, (int) (SPS * ddur));
}
