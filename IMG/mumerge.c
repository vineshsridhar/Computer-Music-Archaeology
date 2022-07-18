/*	MUMERGE -- Merge mu data files (as used by /dev/audio)
**	psl 10/89
*/

#include	<stdio.h>
#include	<sys/types.h>
#include	<sys/stat.h>
#include	"audio.h"

#define	MAXF	16

main(argc, argv)
char	*argv[];
{
	int i, j, nfiles, gain, sum, done;
	FILE *tmp, *fp[MAXF];
	double db;
	extern double atof(), pow();

	nfiles = 0;
	db = 0.;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case '\0':		/* use stdin */
		    fp[nfiles++] = stdin;
		    break;
		case 'a':		/* set gain */
		    db = atof(&argv[i][2]);
		    break;
		default:
		    goto syntax;
		}
	    } else if (tmp = fopen(argv[i], "r")) {
		if (nfiles >= MAXF) {
		    fprintf(stderr, "Too many files, limit is %d\n", MAXF);
		    goto syntax;
		}
		fp[nfiles++] = tmp;
	    } else {
		perror(argv[i]);
		goto syntax;
	    }
	}
	if (nfiles == 0) {
		
syntax:
	    fprintf(stderr, "Usage: %s [-] [-a#.#] [files]\n", argv[0]);
	    fprintf(stderr, "-\tUse standard input.\n");
	    fprintf(stderr, "-a6\tAttenuate the output by 6 dB.\n");
	    exit(2);
	}
	gain = UNITY * pow(10., -db / 10.);
	for (;;) {
	    done = 1;
	    sum = 0;
	    for (i = 0; i < nfiles; i++) {
		if ((j = fgetc(fp[i])) != EOF) {
		    done = 0;
		    sum += I2sb[j & 0xFF];
		}
	    }
	    if (done)
		break;
	    if (gain != UNITY)
		sum = (sum * gain) / UNITY;
	    if (sum >= B13RNG)
		sum = B13RNG;
	    else if (sum < -B13RNG)
		sum = -B13RNG;
	    fputc(Sb2i[sum], stdout);
	}
	exit(0);
}
