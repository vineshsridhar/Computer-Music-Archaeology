#include <stdlib.h>
#include <stdio.h>
#include <midi.h>
#include "midierror.h"
#include "da_midi_cmd.h"

#define RAW	0
#define COOKED	1

int Decimal = 0;
extern	u_char	*read_ch();

use(n)
{
	MidiError("use: %s [flags] [files or stdin]\n", av0);
	MidiError("%s%s%s%s%s%s%s",
		"flags:\n",
		"\t-t N\tset tempo (beats/minute, default=100)\n",
		"\t-B N\tset time base (ticks/second, default=120)\n",
		"\t-T\tprint tick time, not realtime\n",
		"\t-m N\tset id number for controller disassembly\n",
		"\t-r\traw midi data, without time tags\n",
		"\t-d\tprint time tag in decimal (not hex)\n",
	0);
	exit(n);
}

raw_midi(in, out, mfg_id)
FILE *in, *out;
{
	u_char 	*cp, *bp;

	while (cp = read_ch(in)) {
	    printf("   ");		/* no time tags */
	    for (bp = cp; *bp; bp++)
		printf("%02x%s", *bp, (bp-cp)%8 == 7? "\n   " : " ");
	    printf("; ");
	    da_midi_cmd(out, cp, mfg_id, 0);
	}
}

format(in, out, timefac, tic_mode, id)
FILE *in, *out;
float timefac;
{
	MpuCmd *mcp = Alloc(MpuCmd);
	long n = 0;

	if (!mcp) perror("malloc"), exit(1);

	while ((mcp = GetMpuCmd(in,mcp))) {
		register int i, limit;
		static long time;

		if (mcp->time_tag == MPU_TCIP) fprintf(out, "   ");
		else fprintf(out, Decimal? "%3d " : "%2x ", mcp->time_tag);
		limit = (mcp->cmd_type == SX_CMD)? 1 : 3;
		loop(i,limit) {
			if (i==0 && mcp->cmd_cont) fprintf(out,"   ");
			else if (i<=mcp->arg_cnt)
				fprintf(out,"%2x ",mcp->mpu_cmd[i]);
			else fprintf(out, "   ");
		}
		if (limit == 1) fprintf(out, "      ");
		fprintf(out, "; ");
		time += mcp->mpu_time;
		if (tic_mode) fprintf(out, "%6d ", time);
		else fprintf(out, "%8.3f ", time * timefac);
		fprintf(out, "%6d ", n++);
		da_midi_cmd(out, mcp->mpu_cmd, id, mcp->time_tag);
	}
	free(mcp);
}

filter(fmt, in, out, timefac, ticmode, id)
FILE *in, *out;
float timefac;
{
	switch(fmt){
	Case RAW: raw_midi(in,out,id);
	Default : format(in,out,timefac,ticmode,id);
	}
}

#define tf(a) 1.0/MPU_TPS(a,tb)

main(ac, av)
char *av[];
{
	int i, mfg_id = ID_DX7, tic_mode = 0, fmt = COOKED; 
	float tb = MPU_DEFAULT_TIME_BASE, timefac = tf(MPU_DEFAULT_TEMPO);
	for_each_argument {
	   Case 'B': tb = atoi(argument);
	   Case 't': timefac = tf(atoi(argument));
	   Case 'T': tic_mode = 1;	/* print time in MPU ticks */
	   Case 'm': mfg_id = atoi(argument);
	   Case 'r': fmt = RAW;
	   Case 'd': Decimal = 1;
	   Default : use(1);
	}

	if (i==ac) filter(fmt,stdin,stdout,timefac,tic_mode,mfg_id);
	else while (i < ac) { /* read files */
		FILE *f = sopen(av[i++],"r");
		if (f) filter(fmt,f,stdout,timefac,tic_mode,mfg_id),sclose(f);
		else perror(av[i-1]);
	}
	exit(0);
}
