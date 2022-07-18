/*
**	CHMAP -- Channel Map; remap Midi channels
**	psl 10/85, revised 12/85
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>

#define	NUMCHAN	MIDI_MAX_CHANS
#define	NUMMAP	NUMCHAN

int chanarg(char *arg, int new[], int *newlenp, int old[], int *oldlenp);
int addmap(int nchan, int ochan);
void syntax(char *a0);
int chanlist(char *list, int chans[NUMCHAN]);

int	map[NUMCHAN][NUMMAP];
int	maplen[NUMCHAN];

main(argc, argv)
char	*argv[];
{
	register int i, chan, stat;
	int new[NUMCHAN], old[NUMCHAN], newlen, oldlen;
	int chin[NUMCHAN], oflg;
	long now;
	MCMD *mp;

	oflg = 0;
	while (--argc > 0) {
	    if (chanarg(argv[argc], new, &newlen, old, &oldlen) == -1)
		syntax(argv[0]);
	    if (newlen == oldlen) {
		for (i = 0; i < newlen; i++)
		    if (addmap(new[i], old[i]) == -1)
			syntax(argv[0]);
	    } else if (newlen == 1) {
		for (i = 0; i < oldlen; i++)
		    if (addmap(new[0], old[i]) == -1)
			syntax(argv[0]);
	    } else if (oldlen == 1) {
		for (i = 0; i < newlen; i++)
		    if (addmap(new[i], old[0]) == -1)
			syntax(argv[0]);
	    } else
		syntax(argv[0]);
	    oflg++;
	}
	for (now = 0L; mp = getmcmd(stdin, now); now = mp->when) {
	    if (mp->len < 2 || mp->len > 4) {
		if (oflg)
		    putmcmd(stdout, mp);
	    } else {
		chan = (mp->cmd[0] & M_CHAN_MASK);
		if (oflg) {
		    stat = (mp->cmd[0] & M_CMD_MASK);
		    for (i = 0; i < maplen[chan]; i++) {
			mp->cmd[0] = stat | map[chan][i];
			putmcmd(stdout, mp);
		    }
		} else
		    chin[chan]++;	
	    }
	}
	if (!oflg) {
	    for (i = 0; i < NUMCHAN; i++)
		if (chin[i])
		    fprintf(stderr, "%d event%s from chan %d\n",
		     chin[i], chin[i] == 1? "" : "s", i + 1);
	}
	exit(0);
}

void syntax(a0)
char	*a0;
{
	fprintf(stderr, "Usage: %s [NEW=OLD] ... <file1 >file2\n", a0);
	fprintf(stderr, "where NEW and OLD are comma separated lists");
	fprintf(stderr, " of channel numbers,\nusing '-' for ranges.");
	fprintf(stderr, "  Channel numbers are decimal numbers");
	fprintf(stderr, " in the range 1-16.\n");
	fprintf(stderr, "NEW and OLD must either contain the same number");
	fprintf(stderr, " of channels (one to one),\n");
	fprintf(stderr, "or NEW must be a single channel (many to one),\n");
	fprintf(stderr, "or OLD must be a single channel (one to many).\n");
	fprintf(stderr, "E.g. %s 1-3=1 4=2 copies channel 1 to", a0);
	fprintf(stderr, " 1, 2, and 3, and channel 2 to 4.\n");
	fprintf(stderr, "Note: only channels mentioned are output.\n");
	fprintf(stderr, "If no channel arguments are given,");
	fprintf(stderr, " print statistics on channel usage.\n");
	exit(2);
}

/*
**	Add ochan->nchan to the map for ochan.
*/

int addmap(nchan, ochan)
{
	if (maplen[ochan] >= NUMMAP) {
	    fprintf(stderr, "Too many destinations for %d\n", ochan);
	    return(-1);
	}
	map[ochan][maplen[ochan]++] = nchan;
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
**	# is a decimal number in the range 1 to 16, and "3-5" == "3,4,5".
**	Save an ordered list of channels specified in "chans" array.
**	Return the number of channels specified, -1 for error.
*/

int chanlist(list, chans)
char	*list;
int	chans[NUMCHAN];
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
	    }
	    if (i < 0 || i >= NUMCHAN || j < 0 || j >= NUMCHAN)
		return(-1);
	    if (i <= j)
		for (; i <= j; chans[n++] = i++);
	    else
		for (; i >= j; chans[n++] = i--);
	    if (*cp++ != ',')
		break;
	}
	return(n);
}
