#include <stdio.h>
#include <libmidi.h>
#include <libdx7.h>

#define range(x, base, len) (x >= base && x < base + len)
#define Case	break; case
#define Default	break; default
#define u_char unsigned char
#define Error(a,b,c) return MidiError(a,b,c), -1

static int p1(), p2(), p3();

/*
** Disassemble Yamaha system exclusive midi data in 'ch'
** and print it on 'f'.
** Try to fake it (and return -1) on the following format errors:
**    not Yamaha code,
**    no such format,
**    no such param,
**    no such group,
**    no such sub status.
*/
da_dx7_sx(f, ch)
FILE *f;		/* where to write output */
u_char *ch;		/* array of system-exclusive code */
{
	register int i, j;
	int sub_status, channel, format;
	int group, param, data;

	sub_status = (ch[2] >> 4) & 07;
	channel = ch[2] & 0xF;
	format = ch[3];
	if (ch[1] != ID_DX7) {
	    p3(f, ch, sub_status, channel, format, "", "mfg. id.");
	    return(-1);
	}
	switch (sub_status) {
	Case DX7_SXSS_BD:
	    switch (format){
	    Case DX7_SXF_1V: 
		p1(f, ch, sub_status, channel, format, "dx7 1 voice bulk dump");
		sx_dx7_1v(f, ch);
	    Case TX_SXF_1P: 
		p2(f, ch, sub_status, channel, format, "tx7 1 perf. bulk dump");
	    Case TX_SXF_64P: 	/* NB: big trouble if it's really DX1_SXF_1P */
		p2(f, ch, sub_status, channel, format,
		 "tx7 32 perf. bulk dump");
	    Case DX7_SXF_1S:
		p3(f, ch, sub_status, channel, format,
		 "dx7 supplement edit buffer", (char *) 0);
	    Case DX7_SXF_32S:
		p3(f, ch, sub_status, channel, format,
		 "dx7 packed 32 supplement", (char *) 0);
	    Case DX7_SXF_32V:
		p1(f, ch, sub_status, channel, format,
		 "dx7 32 voice bulk dump");
		sx_dx7_32v(f, ch);
	    Case RX11_SXF_P:
		p2(f, ch, sub_status, channel, format, "rx11 param. bulk dump");
		break;
	    Case TX_SXF_CA:
		p2(f, ch, sub_status, channel, format, "tx7 condition ack.");
	    Case YAM_SXF_UBD:
		p2(f, ch, sub_status, channel, format, "universal bulk dump");
	    Case RX11_SXF_PS:
		p2(f, ch, sub_status, channel, format,
		 "rx11 pattern/song bulk dump");
	    Default:
		p3(f, ch, sub_status, channel, format,
		 "unrecognized format", (char *) 0);
		return(-1);
	    }
	Case DX7_SXSS_PC:
	    group = ch[3] >> 2 & 0x1F;
	    param = ((ch[3] & 3) << 7) | ch[4];
	    data = ch[5];
	    switch (group) {
	    Case DX7_SXPG_V:
		p0(f, ch, sub_status, channel);
		fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		j = DX7_INI_IND(param % DX7NVOX);
		fprintf(f, "%-8d%-8d%-8d%-8d%s\n", param, j, data,
		dx7_ini_vox[j].par_ub, dx7_ini_vox[j].par_name);
	    Case DX_SXPG_P:
		p0(f, ch, sub_status, channel);
		fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		fprintf(f, "dx perf. param.\n");
	    Case DX7_SXPG_F:
		p0(f, ch, sub_status, channel);
		fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		for (i = 0; i < dx7_ini_fun_len; i++)
		    if (param == dx7_ini_fun[i].par_index)
			break;
		if (i < dx7_ini_fun_len)
		    fprintf(f,"%-10d%-10d%s\n", data,
			dx7_ini_fun[i].par_ub, dx7_ini_fun[i].par_name);
		else
		    fprintf(f,"%-10d%-10s%s\n", data, "???", "unknown param.");
	    Case DX9_SXPG_F:
		p0(f, ch, sub_status, channel);
		fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		fprintf(f, "dx9 func. param.\n");
	    Case TX_SXPG_F:
		p0(f, ch, sub_status, channel);
		fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		fprintf(f, "tx func. param.\n");
	    Case DX7_SXPG_S:
		p0(f, ch, sub_status, channel);
		if (param <= 73) {
		    fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		    fprintf(f, "dx7II supplement param.\n");
		} else if (param == 126) {
		    fprintf(f, "%3x%3x; ", ch[3], ch[4]);
		    fprintf(f, "dx7II microtuning param.\n");
		    fprintf(f, "%3x%3x%3x; ", ch[5], ch[6], ch[7]);
		    fprintf(f, "key=%d, data=%d\n", ch[5], (ch[6]<<7) + ch[7]);
		} else if (param == 127) {
		    fprintf(f, "%3x%3x; ", ch[3], ch[4]);
		    fprintf(f, "dx7II fract. scaling param.\n");
		    fprintf(f, "%3x%3x%3x%3x; ", ch[5], ch[6], ch[7], ch[8]);
		    fprintf(f, "op=%d, keygrp=%d, data=%d\n",
		     ch[5], ch[6], (ch[7]<<7) + ch[8]);
		} else if (param <= 180) {
		    fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		    fprintf(f, "dx7II performance param.\n");
		} else if (param >= 192) {
		    fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		    fprintf(f, "dx7II system set-up param.\n");
		} else {
		    fprintf(f, "%3x%3x%3x; ", ch[3], ch[4], ch[5]);
		    fprintf(f, "unrecognized dx7II param.\n");
		}
	    Default:
		p3(f, ch, sub_status, channel, format, "", "parameter group");
		return(-1);
	    }
	Default:
	    p3(f, ch, sub_status, channel, format, "", "sub-status");
	    return(-1);
	}
	return(0);
}

static
p0(f, ch, sub_status, channel)
FILE	*f;
u_char	*ch;
{
	fprintf(f, " %3x\t; mfg. id.    =\t%d\tYamaha\n", ch[1], ch[1]);
	fprintf(f, " %3x\t; sub-status  =\t%d\tchannel     =\t%d\n",
	 ch[2], sub_status, channel);
}

static
p1(f, ch, sub_status, channel, format, fmtnam)
FILE	*f;
u_char	*ch;
char	*fmtnam;
{
	p0(f, ch, sub_status, channel);
	fprintf(f, " %3x\t; format      =\t%d\t%s\n", format, format, fmtnam);
	fprintf(f, " %3x%3x\t; byte count  =\t%d\n",
	 ch[4], ch[5], ch[4] << 7 | ch[5]);
}

static
p2(f, ch, sub_status, channel, format, fmtnam)
FILE	*f;
u_char	*ch;
char	*fmtnam;
{
	u_char *cp;
	int byte_count;

	p1(f, ch, sub_status, channel, format, fmtnam);
	byte_count = ch[4] << 7 | ch[5];
	for (cp = &ch[6]; byte_count; cp += 2) {
	    p2b(f, cp, byte_count);
	    cp += byte_count + 1;
	    if (format != YAM_SXF_UBD || MIDI_EOX(cp[0]))
		break;
	    byte_count = cp[0] << 7 | cp[1];
	    fprintf(f, " %3x%3x\t; byte count  =\t%d\n",
	     cp[0], cp[1], byte_count);
	}
}

p2b(f, ch, byte_count)
FILE	*f;
u_char	*ch;
{
	int n;
	u_char data;

	for (n = 0; n < byte_count; n++) {
	    if ((n % 16) == 0)
		fprintf(f, n? " ;\n " : " ");
	    data = ch[n];
	    fprintf(f, " %02x", data);
	}
	fprintf(f, " ;\n");
	data = ch[byte_count];
	fprintf(f, "  %2x\t; check-sum=\t%d", data, data);
	n = checksum(ch, byte_count);
	if (data != n)
	    fprintf(f, "\t\t%s %x != %x\n","INVALID", data, n);
	else
	    fprintf(f, "\t\tvalid check-sum\n");
}

static
p3(f, ch, sub_status, channel, format, fmtnam, thing)
FILE	*f;
u_char	*ch;
char	*fmtnam, *thing;
{
	int i, byte_count;

	if (thing)
	    fprintf(f, "\t;\tunrecognized Yamaha sys. ex. %s\n", thing);
	byte_count = ch[4] << 7 | ch[5];
	if (ch[byte_count + 7] == SX_EOB)
	    p2(f, ch, sub_status, channel, format, fmtnam);
	else {
	    for (i = 1; ch[i] != SX_EOB; i++)
		fprintf(f, "%3x%s", ch[i], (i % 8)? "" : "\n");
	    if ((i % 8) != 1)
		fprintf(f, "\n");
	}
}
