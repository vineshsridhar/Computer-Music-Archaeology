/*
**	TRANSPOSE -- Transpose Midi note on/off events
**	psl 2/87
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>

#define	MAXC	MIDI_MAX_CHANS
#define	MAXT	16	/* max simultaneous transpositions */


int steps(char *cp);


int	Tr[MAXC][MAXT];		/* per channel transpositions */
int	Numtr[MAXC];

main(argc, argv)
char	*argv[];
{
	char *cp;
	int i, n, k, stat, chan;
	long now;
	MCMD *mp;

	if (argc < 2) {
syntax:
	    fprintf(stderr,
	     "Usage: %s [C=]#[o][,#[o]]... [...] <file\n", argv[0]);
	    fprintf(stderr, "C is a channel number [1:16].\n");
	    fprintf(stderr, "# is the transposition in half steps.\n");
	    fprintf(stderr, "#o is the transposition in octaves.\n");
	    fprintf(stderr, "12, +12, 1o, & +1o all mean up one octave.\n");
	    fprintf(stderr, "-12 & -1o both mean down one octave.\n");
	    fprintf(stderr, "Comma-separated numbers are simultaneous;\n");
	    fprintf(stderr, " '2=0,-1o' doubles channel 2 an octave lower.\n");
	    fprintf(stderr, "A number with no C= affects all channels.\n");
	    fprintf(stderr, "Evaluation is left-to-right;\n");
	    fprintf(stderr, " '+12 2=0' means all chans but 2 up an octave;\n");
	    fprintf(stderr, " '2=0 +12' is identical to '+12'.\n");
	    exit(2);
	}
	for (i = 1; i < argc; i++) {
	    for (cp = argv[i]; *cp && *cp != '='; cp++);
	    if (*cp++ == '=') {
		chan = atoi(argv[i]) - 1;
		if (chan < 0 || chan >= MAXC)
		    goto syntax;
		for (n = 0; n < MAXT && *cp; n++) {
		    Tr[chan][n] = steps(cp);
		    for (; *cp && *cp != ','; cp++);
		}
		Numtr[chan] = n;
	    } else {
		for (n = 0, cp = argv[i]; n < MAXT && *cp; n++) {
		    Tr[0][n] = steps(cp);
		    while (*cp && *cp++ != ',');
		}
		Numtr[0] = n;
		for (chan = 1; chan < MAXC; Numtr[chan++] = Numtr[0])
		    for (n = Numtr[0]; --n >= 0; Tr[chan][n] = Tr[0][n]);
	    }
	}
	for (now = 0L; mp = getmcmd(stdin, now); ) {
	    now = mp->when;
	    stat = (mp->cmd[0] & M_CMD_MASK);
	    if (stat != CH_KEY_ON && stat != CH_KEY_OFF)
		putmcmd(stdout, mp);
	    else {
		chan = (mp->cmd[0] & M_CHAN_MASK);
		if (!Numtr[chan])
		    putmcmd(stdout, mp);
		else {
		    k = mp->cmd[1];
		    for (n = 0; n < Numtr[chan]; n++) {
			i = k + Tr[chan][n];
			mp->cmd[1] = i < 1? 1 : (i > 127? 127 : i);
			putmcmd(stdout, mp);
		    }
		}
	    }
	}
}

int steps(cp)
register char *cp;
{
	int trans;

	if (*cp == '+')
	    cp++;
	trans = atoi(cp);
	while (*cp == '-' || ('0' <= *cp && *cp <= '9'))
	    cp++;
	if (*cp == 'o')
	    trans *= 12;
	return(trans);
}
