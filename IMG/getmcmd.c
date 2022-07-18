/*
**	GETMCMD -- get an absolute time MIDI command from MPU format
**	MCMD *getmcmdb(ifp, time, buf, len, statp) -- uses supplied buffers
**		Return 0 for error or EOF, else static struct pointer.
**	MCMD *getmcmd(ifp, time) -- uses own static buffer
**		Return 0 for error or EOF, else static struct pointer.
**	Common usage:
**		now = 0L;
**		while (mp = getmcmd(stdin, now)) {
**			now = mp->when;
**			...
**		}
**	MPU_TCIP commands are not returned, but when an MPU_TCIP ends a file,
**	an extra MPU_NO_OP will be returned (to carry the ending time).
**	psl 2/87
*/
#include	<stdio.h>
#include	<midi.h>
#include "statproc.h"

#define		ERREOF	((MCMD *) 0)

static	MCMD	M;		/* returned by getmcmdb() (and getmcmd()) */

MCMD	*
getmcmdb(ifp, inow, b, len, statp)
FILE		*ifp;
long		inow;
unsigned char	*b;
int		*statp;
{
	register unsigned char *bp;
	register int c, n;
	long last;

	for (last = inow; ; ) {		/* look for MPU time tag byte */
	    if ((c = getc(ifp)) == EOF) {
		if (inow <= last)
		    return(ERREOF);	/* normal EOF */
		M.when = inow;
		M.len = 1;
		M.cmd = b;
		M.cmd[0] = MPU_NO_OP;
		return(&M);		/* normal EOF with extra time */
	    }
	    if (c == MPU_ALL_END)	/* throw this away */
		continue;
	    if (c == MPU_TCIP)
		inow += MPU_CLOCK_PERIOD;
	    else
		break;
	}
	if (c == SX_CMD)		/* doesn't need time tag */
	    M.when = inow;		/* c is not timing */
	else {
	    M.when = inow + c;		/* c is timing */
	    if ((c = getc(ifp)) == EOF)	/* c can be status */
		return(ERREOF);
	}
	M.cmd = bp = b;
	n = statproc(statp, c);	/* get length & do running status */
	if (n == 0)
	    return(ERREOF);		/* bad status */
	if ((c & M_CMD) == 0)		/* running status */
	    *bp++ = *statp;		/* re-insert status */
	if (n == -1) {			/* system exclusive */
	    *bp++ = c;			/* status (couldn't be running) */
	    for (c = 0; (c = getc(ifp)) != EOF && !MIDI_EOX(c); )
		if (--len > 1)
		    *bp++ = c;
	    if (c == EOF)
		return(ERREOF);
	    *bp++ = SX_EOB;
	    if (len <= 1)
		return(ERREOF);		/* buffer overflow */
	    if (c != SX_EOB)
		ungetc(c, ifp);		/* next status */
	} else {
	    for (*bp++ = c; --n > 0 && (c = getc(ifp)) != EOF; *bp++ = c);
	    if (n)
		return(ERREOF);		/* premature EOF */
	}
	M.len = bp - M.cmd;
	return(&M);
}

static	int	Status;		/* running status */
static	u_char	B[4096];	/* buffer for command bytes */

MCMD	*
getmcmd(ifp, inow)
FILE	*ifp;
long	inow;
{
	return(getmcmdb(ifp, inow, B, sizeof B, &Status));
}
