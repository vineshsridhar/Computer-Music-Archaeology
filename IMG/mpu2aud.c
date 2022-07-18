/*
**	MPU2AUD -- Convert MPU data to data for SPARCstation /dev/audio
**	psl 10/89
*/
#include <stdlib.h>
#include <stdio.h>
#include	"midi.h"
#include	"audio.h"

#define	MAXN	64

struct	notestr	{
	int	chan;
	int	key;
	int	vel;
	long	start;
	long	stop;
	int	*data;
} Note[MAXN];

struct	chanstr	{		/* channel instrument characteristics */
	int	damp;		/* note damping (energy loss rate */
	int	twang;		/* note twanginess (lack of low-pass) */
} Chch[MIDI_MAX_CHANS];

struct	notestr	*Ulist[MAXN];	/* note structs in use */
int	Nul;
struct	notestr	*Flist[MAXN];	/* note structs free */
int	Nfl;

int	Gain;			/* gain scaling (* UNITY) */
int	Vflg;			/* verbosity */
int	Clip;			/* how many samples were clipped */

extern	double	K2p[], atof();

main(argc, argv)
char	*argv[];
{
	char *cp;
	int i, tempo, chan;

	tempo = 120;
	for (i = MIDI_MAX_CHANS; --i >= 0; ) {
	    Chch[i].damp = 2;
	    Chch[i].twang = 7;
	}
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 't':
		    tempo = atoi(&argv[i][2]);
		    break;
		case 'c':		/* channel damp/twang */
		    chan = atoi(cp = &argv[i][2]) - 1;
		    while (*cp && *cp++ != '=');
		    Chch[chan].damp = atoi(cp);
		    while (*cp && *cp++ != '/');
		    Chch[chan].twang = atoi(cp);
		    break;
		case 'g':		/* gain */
		    Gain = db2fp(atof(&argv[i][2]));
		    break;
		case 'v':		/* verbose */
		    Vflg++;
		    break;
		default:
		    fprintf(stderr, "Usage: %s [options]\n", argv[0]);
		    fprintf(stderr, "-c#=#/#\tset damp/twang for channel #\n");
		    fprintf(stderr, "\tdefault is 2/7 for all channels\n");
		    fprintf(stderr, "-g#.#\tadjust gain by #.# dBs\n");
		    fprintf(stderr, "\tdefault is -g0\n");
		    fprintf(stderr, "-t#\tset tempo to # beats/minute\n");
		    fprintf(stderr, "\tdefault is -t120\n");
		    fprintf(stderr, "-v\tverbose; warn of clipping\n");
		    exit(2);
		}
	    }
	}
	mpu2aud(stdin, tempo, stdout);
	exit(0);
}

mpu2aud(ifp, tempo, ofp)
FILE	*ifp, *ofp;
{
	int cps, tix, nt, i;
	int stat, chan, key, dur;
	long now;
	struct notestr *np;
	MCMD *mp;

	cps = 2 * tempo;
	for (Nfl = 0; Nfl < MAXN; Nfl++)	/* build free list */
	    Flist[Nfl] = &Note[Nfl];
	for (now = tix = 0; mp = getmcmd(ifp, now); ) {
	    now = mp->when;
	    nt = (now * SPS) / cps;
	    stat = (mp->cmd[0] & M_CMD_MASK);
	    if (stat == CH_KEY_OFF) {
		stat = CH_KEY_ON;
		mp->cmd[2] = 0;
	    }
	    if (stat == CH_KEY_ON) {
		chan = (mp->cmd[0] & M_CHAN_MASK);
		key = mp->cmd[1];
		if (mp->cmd[2]) {	/* key-on */
		    if (Nul >= MAXN) {
			fprintf(stderr, "Too many notes at clock %d\n", now);
			continue;
		    }
		    np = Ulist[Nul++] = Flist[--Nfl];
		    np->chan = chan;
		    np->key = key;
		    np->vel = mp->cmd[2];
		    np->start = nt;
		    np->data = (int *) 0;
		} else {		/* key-off */
		    for (i = Nul; --i >= 0; ) {
			np = Ulist[i];
			if (np->chan == chan && np->key == key && !np->data) {
			    np->stop = nt;
			    dur = nt - np->start;
			    pluck(K2p[np->key], dur / (float) SPS, np->vel,
			     Chch[np->chan].damp, Chch[np->chan].twang,
			     np->data = (int *) malloc(sizeof (int) * dur));
/****/if(Vflg>1)fprintf(stderr, "%d: %s %d/%d start,stop=%d,%d data=%x\n",
 nt, key2name(np->key), Chch[np->chan].damp, Chch[np->chan].twang,
 np->start, np->stop, np->data);
			    break;
			}
		    }
		}
	    }
	    /* output any time that we can */
	    for (i = Nul; --i >= 0; ) {
		np = Ulist[i];
		if (!np->data && np->start < nt)
		    nt = np->start;
	    }
	    if (tix < nt)
		tix = aflush(tix, nt, ofp);
	}
	aflush(tix, (int) ((now * SPS) / cps + 1), ofp);
	if (Vflg)
	    fprintf(stderr, "%d sample%s clipped\n", Clip, Clip == 1? "" : "s");
}

aflush(tix, nt, ofp)	/* flush out data from tix to nt */
FILE	*ofp;
{
	register int i, sum;
	register struct notestr *np;

	for (; tix < nt; tix++) {
	    sum = 0;
	    for (i = Nul; --i >= 0; ) {
		np = Ulist[i];
		if (np->start <= tix && tix < np->stop)
		    sum += np->data[tix - np->start];

	    }
	    if (Gain)
		sum = FPMUL(Gain, sum);
	    sum += B13ZERO;
	    if (Vflg && sum < 0 || sum > B13MAX)
		Clip++;
	    i = (sum < 0)? 0 : ((sum > B13MAX)? B13MAX : sum);
	    putc(Ub2i[i], ofp);
	}
	for (i = Nul; --i >= 0; ) {
	    np = Ulist[i];
	    if (np->data && np->stop <= nt) {
		Ulist[i] = Ulist[--Nul];
		Flist[Nfl++] = np;
		free(np->data);
/****/if(Vflg>1)fprintf(stderr, "%d: free data=%x\n", nt, np->data);
	    }
	}
	return(tix);
}
