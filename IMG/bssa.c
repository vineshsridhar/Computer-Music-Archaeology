/*	BSSA - IMG Stand-Alone
**	psl 2/90
*/
#define	BS

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include	<midi.h>
#include	<stdio.h>
#include "bssetup.h"
#include "bscomp.h"


int fcnv(char *cp);


char	*Keys[12]	= {
	"C, (with no flats or sharps)",
	"D flat, (that's 5 flats to you musicians)",
	"D, (or 2 sharps)",
	"E flat, (also known as 3 flats)",
	"E, (sometimes called, The People's Key)",
	"F, (also known as 1 flat)",
	"F sharp, (written with 6 sharps)",
	"G, (the key of 1 sharp)",
	"A flat, (written with 4 flats)",
	"A, (that's three sharps for you musicians)",
	"B flat, (sometimes called 2 flats)",
	"B, (and written with 5 sharps)",
};
char	*Tempadj[26]	= {
	"glacial",	"glacial",	/*   0:19  */
	"glacial",	"glacial",	/*  10:39  */
	"creeping",	"plodding",	/*  40:59  */
	"sluggish",	"leisurely",	/*  60:79  */
	"relaxed",	"moderate",	/*  80:99  */
	"moderate",	"cheerful",	/* 100:119 */
	"lively",	"sprightly",	/* 120:139 */
	"brisk",	"swift",	/* 140:159 */
	"fast",		"frenzied",	/* 160:179 */
	"demonic",	"demonic",	/* 180:199 */
	"ridiculous",	"ridiculous",	/* 200:219 */
	"ridiculous",	"ridiculous",	/* 220:239 */
	"ridiculous",	"ridiculous",	/* 240:259 */
};

main(argc, argv)
char	*argv[];
{
	char buf[2048], *cp, *outfile;
	int i, mypid, ifh, ofh, frames, seed;
	double secs;
	STYLE *sp;
	SYNTH synth;
	FILE *cmdfp, *setfp, *vrbfp;
	extern STYLE *bsstyle(), S[];

	cmdfp = setfp = vrbfp = (FILE *) 0;
	outfile = (char *) 0;
	mypid = getpid();
	sp = (STYLE *) 0;
	frames = -1;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'c':		/* create command file to play it */
		    if (argv[i][2]) {
			if (!(cmdfp = fopen(&argv[i][2], "w")))
			    perror(&argv[i][2]);
		    } else
			cmdfp = stderr;
		    break;
		case 'l':		/* piece length */
		    if ((frames = fcnv(&argv[i][2])) < 0)
			goto syntax;
		    break;
		case 'o':		/* output file */
		    if (!argv[i][2])
			goto syntax;
		    outfile = &argv[i][2];
		    break;
		case 's':		/* setup file */
		    if (argv[i][2]) {
			if (!(setfp = fopen(&argv[i][2], "r")))
			    perror(&argv[i][2]);
		    } else
			setfp = stdin;
		    if (setfp) {
			bssetload(setfp, S, &synth, NUMS);
			fclose(setfp);
		    }
		    break;
		case 'v':		/* create verbose description */
		    if (argv[i][2]) {
			if (!(vrbfp = fopen(&argv[i][2], "w")))
			    perror(&argv[i][2]);
		    } else
			vrbfp = stderr;
		    break;
		}
	    } else if (!sp) {
		if (!(sp = bsstyle(argv[i], S, 0))) {
		    fprintf(stderr, "Unrecognized style: %s\n", argv[i]);
		    fprintf(stderr, "Recognized styles are:\n");
		    for (sp = S; sp->name && *sp->name; sp++)
			fprintf(stderr, "\"%s\"   %s\n", sp->name, sp->desc);
		    exit(2);
		}
	    } else
		goto syntax;
	}
	if (!sp) {
syntax:
	    fprintf(stderr, "Usage: %s [OPTIONS] STYLE [>mpufile]\n", argv[0]);
	    fprintf(stderr, "OPTIONS include:\n");
	    fprintf(stderr, "\t(a missing [FILE] means stdin or stderr.)\n");
	    fprintf(stderr, "-c[FILE] create a command file to play it.\n");
	    fprintf(stderr, "-l##[f|s|:##] set length, the forms are:\n");
	    fprintf(stderr, "\t-l12\t12 seconds\n");
	    fprintf(stderr, "\t-l23f\t23 frames (23/30 seconds)\n");
	    fprintf(stderr, "\t-l34.5s\t34.5 seconds\n");
	    fprintf(stderr, "\t-l45:15\t45 seconds and 15 frames\n");
	    fprintf(stderr, "\t-l:123\t123 frames (4.1 seconds)\n");
	    fprintf(stderr, "-oFILE put output in FILE (else stdout)\n");
	    fprintf(stderr, "-s[FILE] read FILE for synth/style setup info\n");
	    fprintf(stderr, "-v[FILE] give a verbose description.\n");
	    fprintf(stderr, "All STYLE names are five characters long;");
	    fprintf(stderr, " run \"%s [-sFILE] help\" for a list.\n", argv[0]);
	    exit(2);
	}
	if (cmdfp) {
	    if (synth.init[0] > ' ')
		fprintf(cmdfp, "%s\n", synth.init);
	    fprintf(cmdfp, "play -t%d %s\n", sp->tempo, outfile? outfile : "");
	    fclose(cmdfp);
	}
	bsinit();
	if (frames < 0)
	    frames = fcnv(sp->leng);
	if (vrbfp) {
	    fprintf(vrbfp,
	     "This piece is an example of a style known as %s.\n", sp->desc);
	    fprintf(vrbfp, "It is in the key of %s, and,\n", Keys[sp->key]);
	    fprintf(vrbfp,
	     "when played at the %s tempo of %d beats per minute,\n",
	     Tempadj[sp->tempo / 10], sp->tempo);
	    fprintf(vrbfp, "will last about %g seconds.\n", frames / 30.);
	    fclose(vrbfp);
	}
	seed = time(0) + mypid;
	sprintf(buf, "/tmp/%d", mypid);
	sp->cnti = 0;
	if (bscomp(sp, frames, seed, 0, 0, 0, 0, 0, synth.kmap, buf) == -1) {
	    fprintf(stderr, "bscomp() failed\n");
	    exit(1);
	}
	sprintf(buf, "/tmp/%d.mpu", mypid);
	ofh = 1;
	if (outfile && *outfile) {
	    ofh = -1;				/* may not need to copy */
	    unlink(outfile);
	    if (link(buf, outfile) == -1)
		ofh = creat(outfile, 0644);	/* can't link, so copy */
	}
	if (ofh >= 0) {
	    if ((ifh = open(buf, 0)) < 0) {
		perror(buf);
		exit(1);
	    }
	    while ((i = read(ifh, buf, sizeof buf)) > 0)
		write(ofh, buf, i);
	}
	sprintf(buf, "rm -f /tmp/%d.*", mypid);
	system(buf);
	exit(0);
}

fcnv(cp)
char	*cp;
{
	double secs;
	extern double atof();

	secs = atof(cp);
	for (; ('0' <= *cp && *cp <= '9') || *cp == '.'; cp++);
	if (*cp == '\0' || *cp == 's')
	    return((int) (30 * secs));
	else if (*cp == ':')
	    return((int) (30 * secs + atof(&cp[1])));
	else if (*cp == 'f')
	    return((int) secs);
	return(-1);
}

