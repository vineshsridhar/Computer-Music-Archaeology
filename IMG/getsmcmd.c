/*
**	GETSMCMD -- get an MPU command from midi file standard format
**	    For MIDI events return data in mp->cmd and length in mp->len.
**	    For sysex & meta events return data in mp->cmd & 0 in mp->len.
**	MCMD *getsmcmdb(ifp, time, buf, len, statp) -- uses supplied buffers
**	MCMD *getsmcmd(ifp, time) -- uses own static buffer
**	Return 0 for error or EOF, else struct pointer.
**	Common usage:
**		for (now = 0l; mp = getsmcmd(stdin, now); now = mp->when) {
**			if (mp->len) {
**				...	** process data **
**			}
**		}
**	psl 5/89
*/
#include	<stdio.h>
#include	<midi.h>
#define		ERREOF	((MCMD *) 0)

static	MCMD	M;		/* returned by getsmcmdb() and getsmcmd() */

char	*vcpy();

MCMD	*
getsmcmdb(ifp, inow, b, len, statp)
FILE	*ifp;
long	inow;
u_char	*b;
int	*statp;
{
	register unsigned char *bp;
	register int c, n;

	if ((n = getsmfvar(ifp)) == -1)	/* read variable-length delta-time */
/****/{fprintf(stderr, "EOF looking for delta time\n");
	    return(ERREOF);		/* EOF */
/****/}
	M.when = inow + n;
	M.cmd = bp = b;
	if ((c = getc(ifp)) == EOF)	/* c can be status */
/****/{fprintf(stderr, "EOF looking for status\n");
	    return(ERREOF);		/* EOF */
/****/}
	if (c == SMF_SYSEX		/* sysex event */
	 || c == SMF_SYSESC) {		/* escape event */
	    *bp++ = c;			/* save SMF_SYSE{X,SC} */
	    if (!vcpy(ifp, bp))		/* copy {length} and <bytes> */
		return(ERREOF);		/* error; EOF */
	    M.len = 0;			/* indicate raw SMF event */
	} else if (c == SMF_META) {	/* meta event */
	    *bp++ = c;			/* save SMF_META */
	    if ((c = getc(ifp)) == EOF)	/* read <type> */
/****/{fprintf(stderr, "EOF looking for meta type\n");
		return(ERREOF);		/* error; EOF */
/****/}
	    *bp++ = c;			/* save <type> */
	    if (!vcpy(ifp, bp))		/* copy {length} and <bytes> */
/****/{fprintf(stderr, "EOF copying meta length & bytes\n");
		return(ERREOF);		/* error; EOF */
/****/}
	    M.len = 0;			/* indicate raw SMF event */
	} else {			/* MIDI event */
	    n = statproc(statp, c);	/* get length & do running status */
	    if (n == 0)
/****/{fprintf(stderr, "statproc(%x, %x) returns 0\n", statp, c);
		return(ERREOF);		/* error; bad status */
/****/}
	    if ((c & M_CMD) == 0)	/* running status */
		*bp++ = *statp;		/* re-insert status */
	    if (n == -1) {		/* system exclusive ("can't happen") */
		*bp++ = c;		/* status (couldn't be running) */
		for (c = 0; --len > 0; *bp++ = c) {
		    if ((c = getc(ifp)) == EOF)
			return(ERREOF);	/* error; unterminated sysex */
		    if (MIDI_EOX(c))
			break;
		}
		if (!MIDI_EOX(c))
		    return(ERREOF);	/* error; buffer overflow */
		*bp++ = SX_EOB;
		if (c != SX_EOB)
		    ungetc(c, ifp);	/* next status byte */
	    } else {
		for (*bp++ = c; --n > 0 && (c = getc(ifp)) != EOF; *bp++ = c);
		if (n)
/****/{fprintf(stderr, "EOF in MIDI command\n");
		    return(ERREOF);	/* error; unterminated MIDI command */
/****/}
	    }
	    M.len = bp - M.cmd;
	}
	return(&M);
}

static	int	Status;		/* handles running status */
static	u_char	B[4096];

MCMD	*
getsmcmd(ifp, inow)
FILE	*ifp;
long	inow;
{
	return(getsmcmdb(ifp, inow, B, sizeof B, &Status));
}

char	*
vcpy(ifp, bp)		/* read and copy {length} & <bytes> */
FILE	*ifp;		/* return 0 on EOF, else updated bp */
char	*bp;
{
	register int n, c;
	char *i2smfvar();

	if ((n = getsmfvar(ifp)) == -1)	/* read {length} */
/****/{fprintf(stderr, "EOF in vcpy() looking for length\n");
	    return((char *) 0);		/* error; EOF */
/****/}
	bp = i2smfvar(n, bp);		/* copy into buffer */
	while (--n >= 0 && (c = getc(ifp)) != EOF)	/* copy <bytes> */
	    *bp++ = c;
/****/if(n>=0)fprintf(stderr, "EOF in vcpy() looking for bytes\n");
	return(n>=0? (char *) 0 : bp);
}
