/*
**	INST -- Select MIDI instrument		psl 9/85
*/
#include <stdlib.h>
#include	<stdio.h>
#include <unistd.h>
#include	<midi.h>

#define	MAXCHAN	MIDI_MAX_CHANS
#define	MAXINST	256
#define	DEFDEL	(MPU_CLOCK_PERIOD / 2)

main(argc, argv)
char	*argv[];
{
	char buf[512], *cp, *ap;
	int i, chan, inst, delay;

	if (argc == 1) {
syntax:
	    fprintf(stderr, "Usage: %s [-d#] chan=inst [...]\n", argv[0]);
	    fprintf(stderr, "-d# appends a # clock long pause (%d default)\n",
	     DEFDEL);
	    fprintf(stderr, "-d0 supresses appending any pause\n");
	    fprintf(stderr, "'chan' must be a decimal number 1..%d\n", MAXCHAN);
	    fprintf(stderr, "'inst' must be a decimal number 1..%d\n", MAXINST);
	    exit(2);
	}
	delay = DEFDEL;
	cp = buf;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-' && argv[i][1] == 'd') {
		delay = atoi(&argv[i][2]);
		continue;
	    }
	    for (ap = argv[i]; *ap && *ap != '='; ap++);
	    if (*ap++ != '=')
		goto syntax;
	    chan = atoi(argv[i]);
	    if (chan < 1 || chan > MAXCHAN)
		goto syntax;
	    inst = atoi(ap);
	    if (inst < 1 || inst > MAXINST)
		goto syntax;
	    *cp++ = 0;
	    *cp++ = CH_PRG | (chan - 1);
	    *cp++ = inst - 1;
	}
	while (delay >= MPU_CLOCK_PERIOD) {
	    *cp++ = MPU_TCIP;
	    delay -= MPU_CLOCK_PERIOD;
	}
	*cp++ = delay;
	*cp++ = MPU_TCWME;
	write(1, buf, cp - buf);
	exit(0);
}
