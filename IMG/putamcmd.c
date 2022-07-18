/*
**	PUTAMCMD -- routine to write an asynchronous mpu command
**		
**	long putamcmd(ofp, mp)	FILE *ofp; MCMD *mp;
**		Write command *mp to ofp (single stream)
**		return earliest legal output time (in MPU clocks)
**		if ofp == 0,
**			if mp == 0, set earliest legal output time to 0
**			else, set earliest legal output time to mp->when
**	flushamcmd(ofp)
**		Flush buffered up commands in chronological order.
**	
**	Example usage:
**		now = 0L;
**		limit = 0L;
**		putamcmd(0, 0);			** sets output "now" to 0 **
**		while (mp = getmcmd(stdin, now)) {
**			now = mp->when;
**			mp->when = limit + rand() % 1000;
**			limit = putamcmd(stdout, mp);
**		}
**		flushamcmd(stdout);
**	psl 10/88
*/
#include	<stdio.h>
#include	<midi.h>
#include "putamcmd.h"

static comp(MCMD *mp1, MCMD *mp2);


long	Onowa;				/* output clock */

#define	HMAX	2048			/* max buffered headers */
#define	DMAX	2048			/* max buffered data characters */

MCMD	Hbuf[HMAX];			/* command headers */
int	Ncmd	= 0;			/* number of headers in Hbuf */
unsigned char	Dbuf[DMAX];		/* command data */
int	Nchr	= 0;			/* number of characters in Dbuf */

long
putamcmd(ofp, mp)
FILE	*ofp;
MCMD	*mp;
{
	register int i;

	if (ofp == (FILE *) 0)
	    return(Onow = (mp == (MCMD *) 0)? 0L : mp->when);
	if (Ncmd >= HMAX || Nchr + mp->len > DMAX)
	    flushamcmd(ofp);
	Hbuf[Ncmd] = *mp;
	Hbuf[Ncmd++].cmd = &Dbuf[Nchr];
	for (i = 0; i < mp->len; Dbuf[Nchr++] = mp->cmd[i++]);
	return(Onowa);
}

void flushamcmd(FILE *ofp)
{
	register int i;
//	extern int comp();

	qsort(Hbuf, Ncmd, sizeof Hbuf[0], comp);
	for (i = 0; i < Ncmd; i++)
	    Onowa = putmcmd(ofp, &Hbuf[i]);
	Ncmd = Nchr = 0;
}

static
comp(mp1, mp2)
MCMD	*mp1, *mp2;
{
	long dt;

	dt = mp1->when - mp2->when;
	return(dt < 0? -1 : (dt > 0? 1 : 0));
}
