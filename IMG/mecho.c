/*
**	MECHO -- Copy input to output, adding echo
**	psl 10/85
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>

#define	MAXCHAN	16
#define	MAXMAP	16
#define	UNITY	1024

#define	MAXQ	1024
struct	qstr	{
	long	when;
	char	mode;
	char	key;
	char	vel;
} Queue[MAXQ];
struct	qstr	*Qip = Queue;
struct	qstr	*Qop = Queue;


int chanarg(char *arg, int new[], int *newlenp, int old[], int *oldlenp);
int addmap(int nchan, int ochan);
void mecho(FILE *ifp, int delay, int gain, int trans, int new[], int newlen,
           int old[], int oldlen, FILE *ofp);
void keyout(long when, int mode, int key, int vel);
int chanlist(char *list, int chans[MAXCHAN]);


int	Map[MAXCHAN][MAXMAP];
int	Maplen[MAXCHAN];

main(argc, argv)
char	*argv[];
{
	register int i, j, delay, gain, trans;
	int new[MAXCHAN], newlen, old[MAXCHAN], oldlen, chanspec;
	extern double atof();

	if (argc < 2) {
syntax:
	    fprintf(stderr, "Usage: %s [options] [echo=orig ...] <old >new",
	     argv[0]);
	    fprintf(stderr, "Options:\n");
	    fprintf(stderr, "\t-b#.# specifies a delay in bars (measures),\n");
	    fprintf(stderr, "\t-c# specifies a delay in MPU clocks.\n");
	    fprintf(stderr, "\t    (Delays must be non-negative).\n");
	    fprintf(stderr, "\t-g#.# specifies the gain for the echo.\n");
	    fprintf(stderr, "\t-t# specifies a transposition for the echo.\n");
	    fprintf(stderr, "\"echo\" & \"orig\" are lists of channel numbers");
	    fprintf(stderr, " in the format used by chmap(1).\n");
	    fprintf(stderr, "The lists must either be of the same length, or");
	    fprintf(stderr, " at least one list must be a single number.\n");
	    fprintf(stderr, "E.g. '%s -c10 3,4=1,2' echoes channels", argv[0]);
	    fprintf(stderr, " 1 & 2 on channels 3 & 4 respectively.\n");
	    fprintf(stderr, "'%s -b1 3=1,2 6,7=5' echoes 1 & 2 on 3,", argv[0]);
	    fprintf(stderr, " and 5 on 6 & 7.\n");
	    fprintf(stderr, "If ANY channels are specified, then ONLY those");
	    fprintf(stderr, " channels will be echoed.\n");
	    fprintf(stderr, "The default is -b0 -c0 -g1 1-16=1-16\n");
	    exit(2);
	}
	delay = trans = chanspec = 0;
	gain = UNITY;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'b':
		    delay = 2 * MPU_CLOCK_PERIOD * atof(&argv[i][2]);
		    break;
		case 'c':
		    delay = atoi(&argv[i][2]);
		    break;
		case 'g':
		    gain = UNITY * atof(&argv[i][2]);
		    break;
		case 't':
		    trans = atoi(&argv[i][2]);
		    break;
		default:
		    goto syntax;
		}
	    } else {
		if (chanarg(argv[i], new, &newlen, old, &oldlen) == -1)
		    goto syntax;
		if (newlen == oldlen) {
		    for (j = 0; j < newlen; j++)
			if (addmap(new[j], old[j]) == -1)
			    goto syntax;
		} else if (newlen == 1) {
		    for (j = 0; j < oldlen; j++)
			if (addmap(new[0], old[j]) == -1)
			    goto syntax;
		} else if (oldlen == 1) {
		    for (j = 0; j < newlen; j++)
			if (addmap(new[j], old[0]) == -1)
			    goto syntax;
		} else
		    goto syntax;
		chanspec = 1;	/* to indicate a channel arg found */
	    }
	}
	if (delay < 0)
	    goto syntax;
	if (!chanspec) {
	    for (i = 0; i < MAXCHAN; i++) {
		Maplen[i] = 1;
		Map[i][0] = i;
	    }
	}
	mecho(stdin, delay, gain, trans, new, newlen, old, oldlen, stdout);
	exit(0);
}

void mecho(ifp, delay, gain, trans, new, newlen, old, oldlen, ofp)
FILE	*ifp, *ofp;
int	new[], old[];
{
	register int i, vel, chan;
	long now;
	MCMD *mp;

	for (now = 0L; mp = getmcmd(stdin, now); ) {
	    now = mp->when;
	    if (mp->cmd[0] == MPU_NO_OP)
		continue;
	    while (Qop != Qip && Qop->when <= now) {
		keyout(Qop->when, Qop->mode, Qop->key, Qop->vel);
		if (++Qop == &Queue[MAXQ])
		    Qop = Queue;
	    }
	    if ((mp->cmd[0] & M_CMD_MASK) == CH_KEY_OFF) {
		mp->cmd[0] ^= (CH_KEY_OFF ^ CH_KEY_ON);
		mp->cmd[2] = 0;
	    }
	    putmcmd(stdout, mp);
	    if ((mp->cmd[0] & M_CMD_MASK) == CH_KEY_ON) {
		vel = mp->cmd[2];
		if (vel > 0) {
		    vel = (gain * vel) / UNITY;
		    vel = (vel < 1) ? 1 : ((vel > 127) ? 127 : vel);
		}
		chan = (mp->cmd[0] & M_CHAN_MASK);
		for (i = 0; i < Maplen[chan]; i ++) {
		    Qip->when = now + delay;
		    Qip->mode = CH_KEY_ON | Map[chan][i];
		    Qip->key = mp->cmd[1] + trans;
		    Qip->vel = vel;
		    if (++Qip == &Queue[MAXQ])
			Qip = Queue;
		    if (Qip == Qop) {
			fprintf(stderr, "Delay queue overflow\n");
			exit(1);
		    }
		}
	    }
	}
	while (Qop != Qip) {
	    keyout(Qop->when, Qop->mode, Qop->key, Qop->vel);
	    if (++Qop == &Queue[MAXQ])
		Qop = Queue;
	}
	Mpu_nop.when = now;
	putmcmd(stdout, &Mpu_nop);
}

void keyout(when, mode, key, vel)
long	when;
{
	u_char mbuf[3];
	MCMD m;

	m.when = when;
	m.len = 3;
	m.cmd = mbuf;
	m.cmd[0] = mode;
	m.cmd[1] = key;
	m.cmd[2] = vel;
	putmcmd(stdout, &m);
}

/*
**	Add ochan->nchan to the map for ochan.
*/

int addmap(nchan, ochan)
{
	if (Maplen[ochan] >= MAXMAP) {
	    fprintf(stderr, "Too many destinations for %d\n", ochan);
	    return(-1);
	}
	Map[ochan][Maplen[ochan]++] = nchan;
	return(0);
}

/*
**	Interpret channel arg of the form list=list
**	where "list" is of the form understood by chanlist().
**	Fill in two ordered lists of channels, "new" and "old",
**	and put their lengths in "newlenp" and "oldlenp".
**	Return 0 for success, -1 for syntax error.
*/
int chanarg(arg, new, newlenp, old, oldlenp)
char	*arg;
int	new[], *newlenp, old[], *oldlenp;
{
	register char *cp;

	for (cp = arg; *cp && *cp != '='; cp++);
	if (*cp != '=')
	    return(-1);
	*cp++ = '\0';
	if ((*newlenp = chanlist(arg, new)) == -1)
	    return(-1);
	if ((*oldlenp = chanlist(cp, old)) == -1)
	    return(-1);
	return(0);
}


/*
**	Interpret channel list of the form: #,#-#,...
**	# is a decimal number in the range 1 to 16; 3-5 == 3,4,5.
**	Save an ordered list of channels specified in "chans" array.
**	Return the number of channels specified, -1 for error.
*/

int chanlist(list, chans)
char	*list;
int	chans[MAXCHAN];
{
	register char *cp;
	register int i, j, n;

	n = 0;
	for (cp = list; *cp; ) {
	    i = j = atoi(cp) - 1;
	    while ('0' <= *cp && *cp <= '9')
		cp++;
	    if (*cp == '-') {
		j = atoi(++cp) - 1;
		while ('0' <= *cp && *cp <= '9')
		    cp++;
	    } else if (*cp == ',')
		cp++;
	    if (i < 0 || i >= MAXCHAN || j < 0 || j >= MAXCHAN || i > j)
		return(-1);
	    for (; i <= j; chans[n++] = i++);
	}
	return(n);
}
