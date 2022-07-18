/*
**	MERGE -- Merge two or more streams of MIDI data
**	psl 5/86
*/
#include <stdlib.h>
#include	<stdio.h>
#include <unistd.h>
#include	<midi.h>

#define	BIGTIME		0x7FFFFFFF
#define	MAXFILES	16
#define	MAXMIDILEN	4	/* longest non-sysex MIDI command */

struct	mfstr	{
	FILE	*fp;
	MCMD	m;
	char	*mbuf;
	int	stat;
} Mf[MAXFILES];

int openmf(char *file);
void readmf(struct mfstr *p);
void writemf(struct mfstr *p);


char	Mbuf[MAXFILES * MAXMIDILEN];
int	Nfiles	= 0;
int	Sysex	= 0;
int	Stat[MAXFILES];
int	Blen;

main(argc, argv)
char	*argv[];
{
	register int i, f, minf;
	long mintime;

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case '\0':			/* stdin */
		    if (!openmf((char *) 0))
			exit(1);
		    break;
		case 'x':			/* allow sysex */
		    if (Nfiles) {
			fprintf(stderr, "-x must precede file names\n");
			exit(2);
		    }
		    Sysex = argv[i][2]? atoi(&argv[i][2]) : 1024;
		    break;
		default:
		    goto syntax;
		}
	    } else {
		if (!openmf(argv[i]))
		    exit(1);
	    }
	}
	if (Nfiles < 2) {
syntax:
	    fprintf(stderr,
	     "Usage: %s [-x[#]] file1 file2 [file3 ...] > new", argv[0]);
	    fprintf(stderr, "-\tcan be used as a file name to mean stdin.\n");
	    fprintf(stderr, "-x\tallows sys excl up to ~1020 chars long\n");
	    fprintf(stderr, "-x#\tallows sys excl up to ~#-4 chars long\n");
	    fprintf(stderr, "\t(else sys excl is silently ignored.)\n");
	    exit(2);
	}
	Blen = Sysex? Sysex : MAXMIDILEN;
	for (f = Nfiles; --f >= 0; readmf(&Mf[f]));	/* prime the pump */
	for (mintime = BIGTIME; ; mintime = Mf[minf].m.when) {
	    for (f = Nfiles; --f >= 0; ) {
		if (Mf[f].m.when < mintime) {
		    mintime = Mf[f].m.when;
		    minf = f;
		}
	    }
	    if (mintime == BIGTIME)
		break;
	    writemf(&Mf[minf]);
	    readmf(&Mf[minf]);
	}
	exit(0);
}

int openmf(file)
char	*file;
{
	if (Nfiles >= MAXFILES) {
	    fprintf(stderr, "Too many files; max is %d\n", MAXFILES);
	    exit(2);
	}
	if (file) {
	    if (!(Mf[Nfiles].fp = fopen(file, "r"))) {
		perror(file);
		return(0);
	    }
	} else
	    Mf[Nfiles].fp = stdin;
	Mf[Nfiles].m.when = 0L;
	Mf[Nfiles].stat = 0;
	if (!Sysex)
	    Mf[Nfiles].mbuf = &Mbuf[MAXMIDILEN * Nfiles];
// replacement for sbrk:
        else if (!(Mf[Nfiles].mbuf = (char *) malloc(Sysex))) {
            fprintf(stderr, "malloc(%d) failed\n", Sysex);
            exit(1);
/*
 *	else if ((Mf[Nfiles].mbuf = (char *) sbrk(Sysex)) <= 0) {
 *	    fprintf(stderr, "sbrk(%d) failed\n", Sysex);
 *	    exit(1);
 */
	}
	Nfiles++;
	return(1);
}

void readmf(p)	/* read the next event from *p */
struct	mfstr	*p;
{
	register MCMD *mp;

	while (!(mp = getmcmdb(p->fp, p->m.when, p->mbuf, Blen, &p->stat))) {
	    if (feof(p->fp)) {
		fclose(p->fp);
		p->m.when = BIGTIME;
		return;
	    }
	}
	p->m = *mp;
}

void writemf(p)
struct	mfstr	*p;
{
	putmcmd(stdout, &p->m);
}
