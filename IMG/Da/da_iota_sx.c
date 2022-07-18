/*
** Disassemble Iota system exclusive midi data in 'ch' and print it on 'f'.
** Return '-1' on error: not Iota code, no such format, no such param,
**    no such group, no such sub status.
** else 0 for no hex dump, or N, for hex dump starting at N+1
** psl 10/87
*/
#include <stdio.h>
#include <libmidi.h>
#include <libiota.h>

#define u_char unsigned char

da_iota_sx(f, ch)
FILE	*f;	/* where to write output */
u_char	*ch;	/* array of system-exclusive code */
{
	register int i, fmt;

	fprintf(f, "%5x %2x %2x; ", ch[1], ch[2], ch[3]);
	if (ch[1] != ID_MISC || SUBID(ch[2],ch[3]) != SUBID_IOTA) {
	    fprintf(f, "da_iota_sx: not Iota code!\n");
	    return(-1);
	}
	fprintf(f, "mfg. id.    =\t%d %d %d Iota\n", ch[1], ch[2], ch[3]);
	if (ch[4] != ID_IMF) {
	    fprintf(f, "%5x\t; da_iota_sx: not Midi-Fader code!\n", ch[4]);
	    return(-1);
	}
	fprintf(f, "%5x\t   ; dev. id.    =\t%d  Midi-Fader\n", ch[4], ch[4]);
	fprintf(f, "%5x\t   ; unit num.   =\t%d\n", ch[5], ch[5]);
	fmt = ch[6];
	fprintf(f, "%5x\t   ; Command     =\t%d  ", fmt, fmt);
	switch (fmt) {
	case IMF_SXC_DAT_PROGS:
	    fprintf(f, "all program dump\n");
	    for (i = 0; i < 127; i++)
		progdump(f, i, &ch[10 * i + 7]);
	    break;
	case IMF_SXC_REQ_PROGS:
	    fprintf(f, "all program dump request\n");
	    break;
	case IMF_SXC_DAT_PROG:
	    fprintf(f, "one program dump\n");
	    fprintf(f, "%5x\t   ; Program num =\t%d  ", ch[7], ch[7]);
	    progdump(f, (int) ch[7], &ch[8]);
	    break;
	case IMF_SXC_REQ_PROG:
	    fprintf(f, "one program dump request\n");
	    fprintf(f, "%5x\t   ; Program num =\t%d  ", ch[7], ch[7]);
	    break;
	case IMF_SXC_DAT_ABUF:
	    fprintf(f, "active buffer dump\n");
	    progdump(f, -1, &ch[7]);
	    break;
	case IMF_SXC_REQ_ABUF:
	    fprintf(f, "active buffer dump request\n");
	    break;
	case IMF_SXC_DAT_SETUP:
	    fprintf(f, "setup data dump\n");
	    setupdump(f, &ch[7]);
	    break;
	case IMF_SXC_REQ_SETUP:
	    fprintf(f, "setup data dump request\n");
	    break;
	default:
	    fprintf(f, "da_iota_sx: no such Iota SysEx format: %d", fmt);
	}
	return(0);
}

progdump(f, n, cp)
FILE	*f;
u_char	*cp;
{
	register int i;

	fprintf(f, "  ");
	for (i = 0; i < 10; i++)
	    fprintf(f, "%3x", cp[i]);
	fprintf(f, "; P%d, faders=%3d", n, cp[0]);
	for (i = 1; i < 8; i++)
	    fprintf(f, ",%3d", cp[i]);
	fprintf(f, " mutes=");
	for (i = 0; i < 4; i++)
	    putc((cp[8] & (1 << i))? '1' + i : 'M', f);
	for (i = 0; i < 4; i++)
	    putc((cp[9] & (1 << i))? '5' + i : 'M', f);
	fprintf(f, "\n");
}

setupdump(f, cp)
FILE	*f;
u_char	*cp;
{
	register int i;

	fprintf(f, "%5x%3x   ; receive/send fader levels over MIDI?\t%s/%s\n",
	 cp[0], cp[1], cp[0]? "YES" : "NO", cp[1]? "YES" : "NO");
	fprintf(f, "%5x%3x   ; accept/send program cmnds over MIDI?\t%s/%s\n",
	 cp[2], cp[3], cp[2]? "YES" : "NO", cp[3]? "YES" : "NO");
	fprintf(f, "%5x      ; program change MIDI channel\t= %d\n",
	 cp[4], cp[4]);
	fprintf(f, "%5x      ; send levels every second?\t%s\n",
	 cp[5], cp[5]? "YES" : "NO");
	fprintf(f, "%5x      ; fader change rate\t= %d ms\n",
	 cp[6], (1 << cp[6]));
	fprintf(f, "%5x      ; dial sensitivity\t= %d\n", cp[7], cp[7]);
	for (i = 0; i < 8; i++)
	    fprintf(f,
	     "%5x%3x%3x%3x; fader %d, chan=%d, gain(%s)=%d mute(ctrl)=%d\n",
	     cp[4 * i + 8], cp[4 * i + 9], cp[4 * i + 10], cp[4 * i + 11],
	     i + 1, cp[4 * i + 8], cp[4 * i + 9]? "note" : "ctrl",
	     cp[4 * i + 10], cp[4 * i + 11]);
}
