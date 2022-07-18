#include <stdio.h>
#include <libmidi.h>
#include <libmpu.h>
#include "da_midi_cmd.h"

#define Case	break; case
#define Default	break; default
char *sprintf();

#define F0(a,b) fprintf(f,f0,a,b)
#define F1(a,n,b) fprintf(f,f1,a,n,b)
#define F2(a,b) fprintf(f,f2,a,c[1],0,midi_pk(c[1]),b)
#define F3(a,n,b) fprintf(f,f3,a,c[1],n,midi_pk(c[1]),b)

/*
** Disassemble MIDI command 'c' and print it on 'f'.
** Return '0' if ok, '-1' on error (bad controller index, bad command). 
*/
int da_midi_cmd(f, c, synth_type, ttag)
FILE *f;		  /* output file */
unsigned char *c;	 /* command to be disassembled */
int synth_type;	 	 /* manufacturer's MIDI id. number. */
{
	register char *par_name;
	int chan;
	static int mcnt;
	char 	*f0 = "%-6s          %s\n",
		*f1 = "%-6s[%3d]     %s\n",
		*f2 = "%-6s[%3d]=%-4d%s\n",
		*f3 = "%-6s[%3d]=%-4d%s %s\n";
	extern char *midi_pk(), *get_ctl_name();

	if ((*c & M_CMD_MASK) != SX_CMD) {	/* if not system exclusive */
	    chan = (*c & M_CHAN_MASK) + 1;	/* save channel number */
	    *c &= M_CMD_MASK;			/* strip off channel number */
	}
	switch (*c) {
	Case CH_KEY_OFF:
	    F2("koff","key off");
	Case CH_KEY_ON:
	    if (c[2])
		F3("kon",c[2],"key on");
	    else
		F3("koff",0,"key off");
	Case CH_POLY_KPRS:
	    F2("polykp","polyphonic key pressure");
	Case CH_CTL:
	    if (par_name = get_ctl_name(f, synth_type, c[1]))
		fprintf(f,f2,"ctrl",c[1],c[2], par_name);
	    else
		fprintf(f,f2,"ctrl",c[1],c[2], "");
	Case CH_PRG:
	    fprintf(f, f2, "progc", chan, c[1] + 1, "program");
	Case CH_PRESSURE:
	    F1("press", c[1], "channel pressure");
	Case CH_P_BEND:
	    fprintf(f,
	     "%-6s[%5d]     %s\n", "pbend", (c[2] << 7) | c[1], "pitch bend");
	Case SX_CMD:
	    da_sx(f, c);
	Case SC_MSEL:
	    F0("msel","system common song position pointer");
	Case SC_SSEL:
	    F0("ssel","system common song select");
	Case SC_TSEL:
	    F0("tsel","system common tune request");
	Case SX_EOB:
	    F1("eob",c[1],"system end of block");
	Case MPU_NO_OP:
	    if (ttag == MPU_TCIP)
		F0("tcip","timing clock");
	    else
		F0("no-op","mpu no-op");
	Case 0xF9:
	    F1("tcwme",mcnt++,"mpu TCWME");
	Case RT_START:
	    F0("sa1m","midi start");
	Case RT_CONT:
	    F0("cont","midi continue");
	Case RT_STOP:
	    F0("tcis","midi stop");
	Case MPU_CLOCK_TO_HOST:
	    F0("clkh","mpu CLOCK-TO-HOST (shouldn't have time-tag!)");
	Case 0xFE:
	    F0("sense","active sensing");
	Case RT_RESET:
	    F0("reset","system reset");
	Default:
	    fprintf(f,
	     "%s= %2x %2x %2x\n", "da_cmd: unknown command:", c[0], c[1], c[2]);
	    return(-1);
	}
	return 0;
}
