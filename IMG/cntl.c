/*
**	CNTL -- Set MIDI channel control(s)		psl 3/86
*/
#include <stdlib.h>
#include	<stdio.h>
#include <unistd.h>
#include	<midi.h>

#define	MAXCHAN	MIDI_MAX_CHANS
#define	MAXCNTL	127
#define	MAXVALU	127
#define	DEFDEL	(MPU_CLOCK_PERIOD / 2)

main(argc, argv)
char	*argv[];
{
	char buf[512], *cp, *ap, *bp;
	int i, chan, cntl, valu, delay;

	delay = DEFDEL;
	cp = buf;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-' && argv[i][1] == 'd') {
		delay = atoi(&argv[i][2]);
		continue;
	    }
	    for (ap = bp = argv[i]; '0' <= *ap && *ap <= '9'; ap++);
	    if (*ap++ != ',')
		goto syntax;
	    chan = atoi(bp);
	    if (chan < 1 || chan > MAXCHAN)
		goto syntax;
	    for (bp = ap; '0' <= *ap && *ap <= '9'; ap++);
	    if (*ap++ != '=')
		goto syntax;
	    cntl = atoi(bp);
	    if (cntl < 0 || cntl > MAXCNTL)
		goto syntax;
	    valu = atoi(ap);
	    if (valu < 0 || valu > MAXVALU)
		goto syntax;
	    *cp++ = 0;
	    *cp++ = CH_CTL | (chan - 1);
	    *cp++ = cntl;
	    *cp++ = valu;
	}
	if (cp == buf && delay == 0) {
	    fprintf(stderr, "No control & no delay?\n");
syntax:
	    fprintf(stderr, "Usage: %s [-d#] chan,cntl=valu [...]\n", argv[0]);
	    fprintf(stderr, "-d# appends a # clock long pause (%d default)\n",
	     DEFDEL);
	    fprintf(stderr, "-d0 supresses appending any pause\n");
	    fprintf(stderr, "'chan' must be a decimal number 1..%d\n", MAXCHAN);
	    fprintf(stderr, "'cntl' must be a decimal number 0..%d\n", MAXCNTL);
	    fprintf(stderr, "'valu' must be a decimal number 0..%d\n", MAXVALU);
	    exit(2);
	}

	while (delay >= MPU_CLOCK_PERIOD) {
	    *cp++ = MPU_TCIP;
	    delay -= MPU_CLOCK_PERIOD;
	}
	*cp++ = delay;
	*cp++ = MPU_NO_OP;
	write(1, buf, cp - buf);
	exit(0);
}
