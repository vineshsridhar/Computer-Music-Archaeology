/*
**	checksum(buf, len)
**	Return the 7-bit standard checksum for the len-many bytes in buf.
**	psl 4/88
*/
#include <stdio.h>
#include	<midi.h>

checksum(buf, len)
u_char	*buf;
{
	register int i, s = 0;

	for (i = len; --i >= 0; s -= buf[i]);
	return(s & 0x7F);
}

