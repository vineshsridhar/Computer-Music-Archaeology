/*
**	SMF2MPU -- Convert SMF files to MPU files
**	discard all but track data.
**	psl 5/89
*/

#include <stdio.h>
#include <midi.h>
#define	MAXT	16		/* max # of track chunks */
#define	MBSIZ	4096		/* size of MIDI data buffer */
#define	MAXTM	1024		/* max tempo map segments */

struct	trkchnk	{
	FILE	*ifp;
	long	len;
	long	now;
	MCMD	m;		/* current MPU command */
	int	stat;		/* running status, -1 for suppressed track */
	u_char	mb[MBSIZ];	/* won't hold extremely large sysex dumps */
} Tchnk[MAXT];


struct	tmapstr	{		/* global tempo map */
	long	when;		/* when it takes effect (src units) */
	long	suspq;		/* src usec per quarter */
	long	et;		/* length of map section (dst units) */
} Tmap[MAXTM]	= {
	0, 500000, 0,		/* default src musec / quarter (mm = 120) */
};
int	Ntm	= 1;		/* number of Tmap segments */

#define	TSEG(now,i) \
    ((((now)-Tmap[i].when)*(Dcpm)*(Tmap[i].suspq))/((Scpq)*60000000.))

int	Vflg	= 0;	/* verbose output */
#define	VPR	if(Vflg)fprintf
#define	V2PR	if(Vflg>1)fprintf

double	Dcpm;			/* dst clox per minute */
long	Scpq;			/* src clox per quarter */

FILE	*copyfile();
long	mputime();

syntax(prog)
char	*prog;
{
	fprintf(stderr, "Usage: %s [-t#] [-v] <smf-file >mpu-file\n", prog);
	fprintf(stderr, "   or: %s [-t#] [-v] smf-file ... >mpu-file\n", prog);
	fprintf(stderr, "-t# suppresses track #.\n");
	fprintf(stderr, "-v  gets verbose output\n");
	fprintf(stderr, "Only converts track data (the rest flushed).\n");
	exit(2);
}

main(argc, argv)
char *argv[];
{
	int i, n;
	FILE *ifp;

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 't':			/* suppress track */
		    n = atoi(&argv[i][2]);
		    if (n < 0 || n > MAXT)
			syntax(argv[0]);
		    Tchnk[n].stat = -1;
		    break;
		case 'v':			/* verbose */
		    Vflg++;
		    break;
		default:
		    syntax(argv[0]);
		}
	    }
	}
	n = 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if (ifp = sopen(argv[i], "r")) {
		    cvt(argv[i], ifp, stdout);
		    sclose(ifp);
		    n++;
		} else
		    perror(argv[i]);
	    }
	}
	if (n == 0)
	    cvt("stdin", stdin, stdout);
	exit(0);
}

cvt(file, ifp, ofp)
char	*file;
FILE	*ifp, *ofp;
{
	char realfile[64];
	int i, ntrks, nxt;
	long first;
	SMF_HD hdr;

	Scpq = 96;		/* src clox / quarter */
	Dcpm = 100 * 120;	/* dest clox / min = tempo * clox / quarter */
	for (;;) {
	    if (fread(&hdr, SMF_HD_SIZ, 1, ifp) != 1) {
		fprintf(stderr, "Error reading %s header chunk", file);
		perror("");
		return;
	    }
	    i = B2L(hdr.chdr.leng);
	    if (B2L(hdr.chdr.type) == SMF_HDR)
		break;
	    VPR(stderr, "Skipping alien chunk type (%4.4s) in %s.\n",
	     hdr.chdr.type, file);
	    fseek(ifp, i, 1);
	}
	if (i != 6) {
	    VPR(stderr, "%s header chunk has length %d, not 6.\n", file, i);
	    fseek(ifp, i - 6, 1);
	}
	ntrks = B2S(hdr.ntrks);
	VPR(stderr, "%s is a format %d file with %d track%s.\n",
	 file, B2S(hdr.format), ntrks, ntrks == 1? "" : "s");
	if (hdr.divis[0] & 0x80) {
	    i = ((hdr.divis[0] & 0xFF) ^ 0xFF) + 1;
	    Scpq = (Tmap[0].suspq * i * hdr.divis[1]) / 1000000.;
	    VPR(stderr, "%d clocks/frame, %d frames/sec\n", i, hdr.divis[1]);
	} else {
	    Scpq = B2S(hdr.divis);
	    VPR(stderr, "%d clocks/beat\n", Scpq);
	}
	if (B2S(hdr.format) == SMF_FMT_0) {
	    if (Tchnk[0].stat == -1)
		return;
	    putmcmd((FILE *) 0, (MCMD *) 0);
	    if (trkinit(&Tchnk[0], file, ifp))
		while (trkdata(&Tchnk[0]))
		    doevent(&Tchnk[0], ofp);
	} else if (B2S(hdr.format) == SMF_FMT_1) {
	    putmcmd((FILE *) 0, (MCMD *) 0);
	    sprintf(realfile, "/tmp/smf%d", getpid());
	    ifp = copyfile(ifp, realfile);
	    for (i = 0; i < ntrks; i++) {
		if (!trkinit(&Tchnk[i], file, ifp) || Tchnk[i].stat == -1)
		    Tchnk[i].ifp = 0;
		else {
		    Tchnk[i].ifp = fopen(realfile, "r");
		    fseek(Tchnk[i].ifp, ftell(ifp), 0);
		}
		fseek(ifp, Tchnk[i].len, 1);
		trkdata(&Tchnk[i]);		/* read first command */
	    }
	    for (;;) {
		nxt = -1;
		first = 999999999;
		for (i = 0; i < ntrks; i++) {
		    if (Tchnk[i].ifp && Tchnk[i].now < first) {
			nxt = i;
			first = Tchnk[nxt].now;
		    }
		}
		if (nxt < 0)
		    break;
		doevent(&Tchnk[nxt], ofp);
		trkdata(&Tchnk[nxt]);
	    }
	    unlink(realfile);
	} else if (B2S(hdr.format) == SMF_FMT_2) {
	    for (i = 0; i < ntrks; i++) {
		putmcmd((FILE *) 0, (MCMD *) 0);
		if (trkinit(&Tchnk[0], file, ifp))
		    while (trkdata(&Tchnk[0]))
			doevent(&Tchnk[0], ofp);
	    }
	}
}

trkinit(tcp, file, ifp)
struct	trkchnk	*tcp;
char	*file;
FILE	*ifp;
{
	int i, t;
	SMF_CH chdr;

	t = tcp - Tchnk;
	for (;;) {
	    if (fread(&chdr, SMF_CH_SIZ, 1, ifp) != 1) {
		fprintf(stderr, "Error in %s track %d chunk header", file, t);
		perror("");
		return(0);
	    }
	    i = B2L(chdr.leng);
	    if (B2L(chdr.type) == SMF_TRK)
		break;
	    VPR(stderr, "Skipping track %d alien chunk type (%4.4s) in %s.\n",
	     t, chdr.type, file);
	    fseek(ifp, i, 1);
	}
	if (Vflg || i < 0 || i > 1000000)
	    fprintf(stderr, "%s: track %d chunk length = %d\n", file, t, i);
	tcp->ifp = ifp;
	tcp->len = i;
	tcp->now = 0;
	return(i);
}

trkdata(tcp)
struct	trkchnk	*tcp;
{
	MCMD *mp, *getsmcmdb();

	if (!tcp->ifp)		/* EOF or error */
	    return(0);
	mp = getsmcmdb(tcp->ifp, tcp->now, tcp->mb, MBSIZ, &tcp->stat);
	if (!mp) {
	    VPR(stderr, "%d@%d: EOF in trkdata()\n",
	     tcp - Tchnk, mputime(tcp->now));
	    tcp->ifp = 0;
	    return(0);
	}
	tcp->now = mp->when;				/* smf time */
	tcp->m = *mp;
	return(1);
}

doevent(tcp, ofp)
struct	trkchnk	*tcp;
FILE	*ofp;
{
	u_char *cp;
	int i, ev, tn;
	MCMD *mp, *getsmcmdb();

	if (!tcp->ifp)		/* EOF or error */
	    return;
	tn = tcp - Tchnk;
	mp = &tcp->m;
	if (mp->len > 0) {
	    if (Vflg > 1) {
		fprintf(stderr, "%d@%d:", tcp - Tchnk, mp->when);
		for (i = 0; i < mp->len; i++)
		    fprintf(stderr, " %02x", mp->cmd[i]);
		fprintf(stderr, "\n");
	    }
	    mp->when = mputime(tcp->now);		/* mpu time */
	    putmcmd(ofp, mp);
	    return;
	}
	if (mp->cmd[0] == SMF_META) {
	    ev = mp->cmd[1];
	    VPR(stderr, "%d@%d: META %02x", tn, mp->when, ev);
	    if (ev == SMF_M_SEQ) {
		if (mp->cmd[2] == 2)
		    VPR(stderr, " SEQ %d\n", C2S(mp->cmd[3], mp->cmd[4]));
		else
		    VPR(stderr, " SEQ\n");
	    } else if (ev >= SMF_M_TEXT && ev <= SMF_M_CUETXT) {
		VPR(stderr, " (text)\n");
		if (Vflg) {
		    cp = &mp->cmd[2];
		    i = smfvar2i(&cp);
		    cp[i] = '\0';
		    fprintf(stderr, "\t%s\n", cp);
		}
	    } else if (ev == SMF_M_CHAN) {
		VPR(stderr, " CHAN %d\n", mp->cmd[3]);
	    } else if (ev == SMF_M_EOT) {
		mp->cmd[0] = MPU_NO_OP;
		mp->len = 1;
		mp->when = mputime(tcp->now);		/* mpu time */
		putmcmd(ofp, mp);
		tcp->ifp = 0;
		VPR(stderr, " EOT\n");
	    } else if (ev == SMF_M_TEMPO) {
		Tmap[Ntm].when = mp->when;
		Tmap[Ntm].suspq = C2L(0, mp->cmd[3], mp->cmd[4], mp->cmd[5]);
		Tmap[Ntm - 1].et = TSEG(mp->when, Ntm-1);
		VPR(stderr, " TEMPO %d usec/beat\n", Tmap[Ntm].suspq);
		Ntm++;
	    } else if (ev == SMF_M_SMPTE) {
		VPR(stderr, " SMPTE %3d:%02d:%02d %3d.%02d\n",
		 mp->cmd[3], mp->cmd[4], mp->cmd[5], mp->cmd[6], mp->cmd[7]);
	    } else if (ev == SMF_M_TIMSIG) {
		VPR(stderr, " TIMSIG %d/%d %d %d\n",
		 mp->cmd[3], 1 << mp->cmd[4], mp->cmd[5], mp->cmd[6]);
	    } else if (ev == SMF_M_KEYSIG) {
		i = (char) mp->cmd[3];
		VPR(stderr, " KEYSIG %d %d (%s %s)\n",
		 i, mp->cmd[4], sh2ks(i), mp->cmd[4]? "minor" : "major");
	    } else if (ev == SMF_M_SEQSPEC) {
		VPR(stderr, " SEQSPEC\n");
		if (Vflg) {
		    cp = &mp->cmd[2];
		    tn = smfvar2i(&cp);
		    for (i = 0; i < tn; i++)
			fprintf(stderr, "%s%02x%s",
			 (i%16)==0? "\t" : " ", cp[i], (i%16)==15? "\n" : "");
		    if (tn%16)
			fprintf(stderr, "\n");
		}
	    } else
		VPR(stderr, " unrecognized\n");
	}
}

FILE	*
copyfile(ifp, realfile)
FILE	*ifp;
char	*realfile;
{
	char buf[1024];
	int i;
	FILE *ofp;

	if (!(ofp = fopen(realfile, "w"))) {
	    perror("temporary file");
	    exit(1);
	}
	while (i = fread(buf, 1, sizeof buf, ifp))
	    fwrite(buf, 1, i, ofp);
	fclose(ifp);
	fclose(ofp);
	if (!(ifp = fopen(realfile, "r"))) {
	    perror("temporary file");
	    exit(1);
	}
	return(ifp);
}

long
mputime(smftime)
long	smftime;
{
	int i;
	long clox;

	clox = 0;
	for (i = 0; i < Ntm - 1 && Tmap[i + 1].when <= smftime; i++)
	    clox += Tmap[i].et;
	clox += TSEG(smftime, i);
	return(clox);
}
