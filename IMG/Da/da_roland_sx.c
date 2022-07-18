/* Disassemble Roland system exclusive midi data in 'ch' and print it on 'fp'.
** Return '-1' on error, else 0.
** psl 3/90
*/
#include <stdio.h>
#include <libmidi.h>
#include <libroland.h>

#define u_char unsigned char

da_roland_sx(fp, ch)
FILE *fp;	/* where to write output */
u_char *ch;	/* array of system-exclusive code */
{
	register int i;

	fprintf(fp, "system exclusive\n");
	if (ch[1] != ID_ROLAND) {
	    fprintf(fp, "%5x\t; da_roland_sx: not Roland code!\n", ch[1]);
	    return -1;
	}
	fprintf(fp, "%2x\t; mfg. id.    =\t%d Roland\n", ch[1], ch[1]);
	fprintf(fp, "%5x\t; dev. id.    =\t%d\n", ch[2], ch[2]);
	fprintf(fp, "%5x\t; model id.   =\t%d\n", ch[3], ch[3]);
	fprintf(fp, "%5x\t; command     =\t%d\n", ch[4], ch[4]);
	for (i = 0; ch[5 + i] != SX_EOB; i++)
	    fprintf(fp, "%s %02x", (i % 16)? "" : (i? "\n " : " "), ch[5 + i]);
	fprintf(fp, "\n");
	return 0;
}
