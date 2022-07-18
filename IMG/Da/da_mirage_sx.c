/* Disassemble Mirage system exclusive midi data in 'ch' and print it on 'fp'.
** Return '-1' on error, else 0.
** psl ?/86
*/
#include <stdio.h>
#include <libmidi.h>
#include <libmirage.h>

#define u_char unsigned char

da_mirage_sx(fp, ch)
FILE *fp;	/* where to write output */
u_char *ch;	/* array of system-exclusive code */
{
	register int i, j, k, pc, csum;
	u_char *dp;
	int fmt;

	fprintf(fp, "system exclusive\n");
	if (ch[1] != ID_ENSONIQ) {
	    fprintf(fp, "%5x\t; da_mirage_sx: not Ensoniq code!\n", ch[1]);
	    return -1;
	}
	fprintf(fp, "%2x\t; mfg. id.    =\t%d Ensoniq\n", ch[1], ch[1]);
	if (ch[2] != ID_MIRAGE) {
	    fprintf(fp, "%5x\t; da_mirage_sx: not Mirage code!\n", ch[2]);
	    return -1;
	}
	fprintf(fp, "%5x\t; dev. id.    =\t%d  Mirage\n", ch[2], ch[2]);
	fmt = ch[3];
	fprintf(fp, "%5x\t; format      =\t%d  ", fmt, fmt);
	switch (fmt) {
	case MIR_SXF_CC:
	    fprintf(fp, "Mirage command code\n");
	    for (i = 4; i < 10 && ch[i] != MIR_CC_END; i++) {
		if (0 <= ch[i] && ch[i] <= MIRBUT_MAX)
		    fprintf(fp, "%5x\t; button      =\t%2d   {%s}\n",
		     ch[i], ch[i], Mir_but[ch[i]]);
		else
		    fprintf(fp, "%5x\t; button      =\t%d ?\n", ch[i], ch[i]);
	    }
	    if (ch[i] == MIR_CC_END)
		fprintf(fp, "%5x\t; Mirage command code end\n", ch[i]);
	    break;
	case MIR_SXF_REQ_PROG | MIR_SXW_LOWER:
	case MIR_SXF_REQ_PROG | MIR_SXW_UPPER:
	    fprintf(fp, "Mirage %s program dump request\n",
	     (fmt & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    break;
	case MIR_SXF_DAT_PROG | MIR_SXW_LOWER:
	case MIR_SXF_DAT_PROG | MIR_SXW_UPPER:
	    fprintf(fp, "Mirage %s program dump data\n",
	     (fmt & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    dp = &ch[4];
	    fprintf(fp, "%5x%3x\t; sound rev level  =\t%d\n",
	     dp[0], dp[1], (dp[1] << 4) + dp[0]);
	    for (j = 0; j < 8; j++) {	/* 8 wavesample control blocks */
		fprintf(fp, "\t\t; Wavesample Control Block %d\n", j + 1);
		dp = &ch[6 + j * 48];
		for (i = 0; i < 24; i++)
		    fprintf(fp, "%5x%3x\t; %-32.32s=\t%d\n",
		     dp[i+i], dp[i+i+1], Mir_wcbn[i],
		     (dp[i+i+1] << 4) + dp[i+i]);
	    }
	    for (j = 0; j < 9; j++) {	/* 9 wavesample segment lists */
		if (j < 8)
		    fprintf(fp, "\t\t; Wavesample Segment List %d\n", j + 1);
		else
		    fprintf(fp, "\t\t; Wavesample Segment List (spare)\n");
		dp = &ch[6 + 8 * 48 + j * 64];
		fprintf(fp, "  ");
		for (i = 0; i < 64; i++) {
		    fprintf(fp, "%3x", dp[i]);
		    if ((i % 16) == 15)
			fprintf(fp, "\n  ", j + 1);
		}
	    }
	    for (j = 0; j < 4; j++) {	/* 4 program parameter blocks */
		fprintf(fp, "\t\t; Program Parameter Block %d\n", j + 1);
		dp = &ch[6 + 8 * 48 + 9 * 64 + j * 72];
		for (i = 0; i < 36; i++)
		    fprintf(fp, "%5x%3x\t; %-32.32s=\t%d\n",
		     dp[i+i], dp[i+i+1], Mir_ppbn[i],
		     (dp[i+i+1] << 4) + dp[i+i]);
	    }
	    break;
	case MIR_SXF_REQ_CPAR:
	    fprintf(fp, "MASOS configuration parameters dump request\n");
	    break;
	case MIR_SXF_DAT_CPAR:
	    fprintf(fp, "MASOS configuration parameters dump data\n");
	    dp = &ch[4];
	    for (i = 0; i < 29; i++)
		fprintf(fp, "%5x%3x\t; %-32.32s=\t%d\n",
		 dp[i+i], dp[i+i+1], Mir_cdn[i], (dp[i+i+1] << 4) + dp[i+i]);
	    break;
	case MIR_SXF_REQ_WAVE:
	    fprintf(fp, "Mirage wavesample dump request\n");
	    break;
	case MIR_SXF_DAT_WAVE:
	    fprintf(fp, "Mirage wavesample dump data\n");
	    dp = &ch[4];
	    pc = (dp[1] << 4) + dp[0];
	    fprintf(fp, "%5x%3x\t; page count       =\t%d\n",
	     dp[0], dp[1], pc);
	    csum = dp[0] + dp[1];
	    for (j = 0; j < pc; j++) {
		for (i = 0; i < MIR_SNPP; i++) {
		    if ((i % 32) == 0)
			fprintf(fp, "  ");
		    fprintf(fp, " %x", k = dp[2 + j * MIR_SNPP + i]);
		    if ((i % 32) == 31)
			fprintf(fp, "; P%d\n", j + 1);
		    csum += k;
		}
		csum = (csum & 0x7F);
	    }
	    i = dp[2 + pc * MIR_SNPP];
	    if (i == csum)
		fprintf(fp, "%5x\t; checksum  =\t%d (correct)\n", i, i);
	    else
		fprintf(fp, "%5x\t; bad checksum  =\t%d (should be %d)\n",
		 i, i, csum);
	    break;
	case MIR_SXF_REQ_AWAVE:
	    fprintf(fp, "MASOS wavesample dump absolute request\n");
	    dp = &ch[4];
	    fprintf(fp, "%5x%3x%3x%3x\t; start address    =\t%d (%s)\n",
	     dp[0], dp[1], dp[2], dp[3],
	     (dp[3] << 12) + (dp[2] << 8) + (dp[1] << 4) + (dp[0] & 0x0F),
	     (dp[0] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    fprintf(fp, "%5x%3x%3x%3x\t; end address      =\t%d\n",
	     dp[4], dp[5], dp[6], dp[7],
	     (dp[7] << 12) + (dp[6] << 8) + (dp[5] << 4) + dp[4]);
	    break;
	case MIR_SXF_DAT_AWAVE:
	    fprintf(fp, "MASOS wavesample dump absolute data\n");
	    dp = &ch[4];
	    fprintf(fp, "%5x%3x%3x%3x\t; start address    =\t%d (%s)\n",
	     dp[0], dp[1], dp[2], dp[3],
	     i = (dp[3] << 12) + (dp[2] << 8) + (dp[1] << 4) + (dp[0] & 0x0F),
	     (dp[0] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    fprintf(fp, "%5x%3x%3x%3x\t; end address      =\t%d\n",
	     dp[4], dp[5], dp[6], dp[7],
	     j = (dp[7] << 12) + (dp[6] << 8) + (dp[5] << 4) + dp[4]);
	    pc = j - i;
	    csum = dp[0] + dp[1] + dp[2] + dp[3]
	     + dp[4] + dp[5] + dp[6] + dp[7];
	    for (i = 0; i < pc; i++) {
		if ((i % 16) == 0)
		    fprintf(fp, "  ");
		fprintf(fp, " %x %x", j = dp[8 + i + i], k = dp[8 + i + i + 1]);
		if ((i % 16) == 15)
		    fprintf(fp, "; %04x-%04x\n", i & 0xFFF0, i);
		csum += j + k;
	    }
	    if (i % 16)
		fprintf(fp, "; %04x-%04x\n", i & 0xFFF0, i);
	    csum = (csum & 0x7F);
	    i = dp[8 + 2 * pc];
	    if (i == csum)
		fprintf(fp, "%5x\t; checksum  =\t%d (correct)\n", i, i);
	    else
		fprintf(fp, "%5x\t; bad checksum  =\t%d (should be %d)\n",
		 i, i, csum);
	    break;
	case MIR_SXF_WSFUNC:
	    fprintf(fp, "MASOS wavesample manipulation function\n");
	    fprintf(fp, "%5x\t; function    =\t%d (%s)\n",
	     ch[4], ch[4] & 0x0F,
	     (ch[4] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    dp = &ch[5];
	    fprintf(fp, "%5x%3x%3x%3x\t; src start addr   =\t%d\n",
	     dp[0], dp[1], dp[2], dp[3],
	     (dp[3] << 12) + (dp[2] << 8) + (dp[1] << 4) + dp[0]);
	    dp = &ch[9];
	    fprintf(fp, "%5x%3x%3x%3x\t; src end addr     =\t%d\n",
	     dp[0], dp[1], dp[2], dp[3],
	     (dp[3] << 12) + (dp[2] << 8) + (dp[1] << 4) + dp[0]);
	    dp = &ch[13];
	    fprintf(fp, "%5x%3x\t\t; scale start fact.=\t%d\n",
	     dp[0], dp[1], (dp[1] << 4) + dp[0]);
	    dp = &ch[15];
	    fprintf(fp, "%5x%3x\t\t; scale end fact.  =\t%d\n",
	     dp[0], dp[1], (dp[1] << 4) + dp[0]);
	    dp = &ch[17];
	    fprintf(fp, "%5x%3x%3x%3x\t; dest start addr  =\t%d (%s)\n",
	     dp[0], dp[1], dp[2], dp[3],
	     ((dp[3] & 0x0F) << 12) + (dp[2] << 8) + (dp[1] << 4) + dp[0],
	     (dp[3] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    break;
	case MIR_SXF_PPAR:
	    fprintf(fp, "MASOS program parameter change\n");
	    fprintf(fp, "%5x\t; program     =\t%d (%s)\n",
	     ch[4], ch[4] & 0x0F,
	     (ch[4] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    fprintf(fp, "%5x\t; parameter   =\t%d\n", ch[5], ch[5]);
	    dp = &ch[6];
	    fprintf(fp, "%5x%3x\t; value       =\t%d\n",
	     dp[0], dp[1], (dp[1] << 4) + dp[0]);
	    break;
	case MIR_SXF_WPAR:
	    fprintf(fp, "MASOS wavesample parameter change\n");
	    fprintf(fp, "%5x\t; wavesample  =\t%d (%s)\n",
	     ch[4], ch[4] & 0x0F,
	     (ch[4] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    fprintf(fp, "%5x\t; parameter   =\t%d\n", ch[5], ch[5]);
	    dp = &ch[6];
	    fprintf(fp, "%5x%3x\t; value       =\t%d\n",
	     dp[0], dp[1], (dp[1] << 4) + dp[0]);
	    break;
	case MIR_SXF_PROG:
	    fprintf(fp, "MASOS program change\n");
	    fprintf(fp, "%5x\t; program     =\t%d (%s)\n",
	     ch[4], ch[4] & 0x0F,
	     (ch[4] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    break;
	case MIR_SXF_WAVE:
	    fprintf(fp, "MASOS wavesample parameter change\n");
	    fprintf(fp, "%5x\t; wavesample  =\t%d (%s)\n",
	     ch[4], ch[4] & 0x0F,
	     (ch[4] & MIR_SXW_UPPER) != MIR_SXW_UPPER? "lower" : "upper");
	    break;
	case MIR_SXF_WACK:
	    fprintf(fp, "Mirage wavesample received without error\n");
	    break;
	case MIR_SXF_WNAK:
	    fprintf(fp, "Mirage wavesample received with error\n");
	    break;
	default:
	    fprintf(fp, "da_mirage_sx: no such Mirage SysEx format: %d", fmt);
	}
	return 0;
}
