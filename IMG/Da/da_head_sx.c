/*
** Disassemble text heading sys ex midi data in 'ch' and print it on 'f'.
** This uses a fake manufacturer's subid (SUBID_HEADER).
** Return '-1' on error: not text subid, no such format, no such param,
** else 0 for no hex dump, or N, for hex dump starting at N+1
** psl 10/87
*/
#include <stdio.h>
#include <libmidi.h>

#define u_char unsigned char

da_head_sx(f, ch)
FILE	*f;	/* where to write output */
u_char	*ch;	/* array of system-exclusive code */
{
	register int i, j;

	fprintf(f, "%5x %2x %2x ; ", ch[1], ch[2], ch[3]);
	if (ch[1] != ID_MISC || SUBID(ch[2],ch[3]) != SUBID_HEADER) {
	    fprintf(f, "da_head_sx: not text header code!\n");
	    return(-1);
	}
	fprintf(f, "text header id & subid\n");
	for (j = 4;; ) {
	    for (i = j; ch[i] != SX_EOB && ch[i] != '\n'; i++);
	    if (ch[i] == SX_EOB && i == j)
		break;
	    fprintf(f, "            ;\t");
	    for (; j < i; j++)
		fprintf(f, "%c", (ch[j]<'\t' || ch[j]>'~')? '?' : ch[j]);
	    fprintf(f, "\n");
	    if (ch[j] == '\n')
		j++;
	}
	return(4);
}
