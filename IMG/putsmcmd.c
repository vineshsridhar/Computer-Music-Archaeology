/*
**	PUTSMCMD -- routines to write MIDI File Standard commands
**	NOTE: does not write header chunk.
**		
**	long putsmcmd(ofp, mp)		FILE *ofp; MCMD *mp;
**		Write command *mp to ofp (single stream)
**		Return mp_>when
**	iputsmcmds(sid, ofp, when)	FILE *ofp; long when;
**		Set file pointer and initial time for stream sid
**	long putsmcmds(sid, mp)		FILE *ofp; MCMD *mp;
**		Write command *mp to stream sid
**		Return mp_>when
**	chunkify(ifp, ofp)		FILE *ifp, *ofp;
**		Copy ifp to ofp, adding chunk header
**		Return number of bytes written (including header), 0 for error
**	
**	Common usage:
**		now = 0L;
**		fp = fopen(TEMPFILE, "w");
**		putsmcmd(0, 0);			** sets output "now" to 0 **
**		while (mp = getmcmd(stdin, now)) {	** reads MPU command **
**			now = mp->when;
**			...
**			putsmcmd(fp, mp);	** writes SMF command **
**		}
**		fp = freopen(TEMPFILE, "r", fp);
**		chunkify(fp, stdout);		** add SMF chunk header **
**	psl 2/87, gl 4/89, psl 5/89
*/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<stdio.h>
#include	<midi.h>
#include "putsmcmd.h"

void putsmfvar(int n, FILE *ofp);

long	Onow;				/* output clock (single stream) */
long	Sonow[MAXSID];			/* output clocks (mult stream) */
FILE	*Sofp[MAXSID];			/* output FILE *s (mult streams) */

long
putsmcmd(ofp, mp)
FILE	*ofp;
MCMD	*mp;
{
	register int dt, i;
	u_char buf[8], *cp;

	if (ofp == (FILE *) 0)
	    return(Onow = (mp == (MCMD *) 0)? 0L : mp->when);
	dt = mp->when - Onow;
	if (dt < 0)
	    return(mp->when);
	putsmfvar(dt, ofp);	/* write variable-length delta-time */
	for (i = 0; i < mp->len; i++)
	    putc(mp->cmd[i], ofp);
	return(Onow = mp->when);
}

iputsmcmds(sid, ofp, when)
FILE	*ofp;
long	when;
{
	if (0 <= sid && sid < MAXSID) {
	    Sofp[sid] = ofp;
	    Sonow[sid] = when;
	}
}

long
putsmcmds(sid, mp)
MCMD	*mp;
{
	long onow, ret;

	if (0 <= sid && sid < MAXSID) {
	    onow = Onow;		/* save single stream value */
	    Onow = Sonow[sid];
	    ret = putmcmd(Sofp[sid], mp);
	    Sonow[sid] = Onow;
	    Onow = onow;
	    return(ret);
	}
	return(0L);
}

chunkify(ifp, ofp)
FILE	*ifp, *ofp;
{
	register int ifh, ofh, n, len;
	char buf[1024];
	struct stat statb;
	SMF_CH chdr;

	ifh = fileno(ifp);
	ofh = fileno(ofp);
	if (fstat(ifh, &statb) == -1) {
	    perror("chunkify; fstat() failed");
	    return(0);
	}
	L2B(chdr.type, SMF_TRK);
	L2B(chdr.leng, statb.st_size);
	if (fwrite(&chdr, len = SMF_CH_SIZ, 1, ofp) != 1) {
	    perror("chunkify; SMF chunk header write failed");
	    return(0);
	}
	for (; (n = fread(buf, 1, sizeof buf, ifp)) > 0; len += n)
	    if (fwrite(buf, 1, n, ofp) != n)
		perror("chunkify; SMF track data write failed");
	return(len);
}

void putsmfvar(n, ofp)		/* write variable-length number */
FILE	*ofp;
{
	char buf[8], *cp;

	cp = buf;
	for (*cp++ = (n & 0x7F); n >>= 7; *cp++ = 0x80 + (n & 0x7F));
	for (; cp > buf; putc(*--cp, ofp));
}
