/*
**	SMFVAR -- Standard MIDI File variable length number routines
** int	getsmfvar(ifp)		FILE *ifp;
**			read var-length number, return -1 for EOF
** int	smfvar2i(cpp)		char **cpp;
**			cvt from var-length number @@cpp; set *cpp
** void	putsmfvar(n, ofp)	int n; FILE *ofp;
**			write var-length number
** char * i2smfvar(n, cp)	int n; char *cp;
**			cvt to var-length number, store @cp, return new cp
**	psl 5/89
*/
#include	<stdio.h>

int
getsmfvar(ifp)		/* read variable-length number, return -1 for EOF */
FILE	*ifp;
{
	register int c, n = 0;

	do {			
	    if ((c = getc(ifp)) == EOF)
		return(-1);		/* EOF */
	    n = (n << 7) + (c & 0x7f);
	} while (c & 0x80);
	return(n);
}

int
smfvar2i(cpp)		/* convert variable-length number @@cpp to an int */
char	**cpp;
{
	register char *cp = *cpp;
	register int c, n = 0;

	do {			
	    c = *cp++;
	    n = (n << 7) + (c & 0x7f);
	} while (c & 0x80);
	*cpp = cp;		/* return pointer to next character */
	return(n);
}

void
putsmfvar(n, ofp)	/* write variable-length number */
FILE	*ofp;
{
	char buf[8], *cp;

	cp = buf;
	for (*cp++ = (n & 0x7F); n >>= 7; *cp++ = 0x80 + (n & 0x7F));
	for (; cp > buf; putc(*--cp, ofp));
}

char	*
i2smfvar(n, cp)		/* convert to variable-length number @cp, return cp */
char	*cp;
{
	char buf[8], *bp;

	bp = buf;
	for (*bp++ = (n & 0x7F); n >>= 7; *bp++ = 0x80 + (n & 0x7F));
	for (; bp > buf; *cp++ = *--bp);
	return(cp);
}
