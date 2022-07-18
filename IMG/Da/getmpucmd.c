#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libmpu.h>

extern	u_char	*read_ch();
/*
** Read a midi command in mpu-401 format from 'f' and put it in 'm'.
** Beware: the 'm->mpu_cmd' field points to static memory;
** save it if you are doing simultaneous 'GetMpuCmd()''s.
*/
MpuCmd *
GetMpuCmd(f, m)
FILE *f;
MpuCmd *m;
{
	u_char *cp;
	int c;
	u_long time = 0;

	if ((c=getc(f)) != EOF) {
		if (c==RT_RESET || c==SX_CMD || c==SX_EOB || c==MPU_DATA_END){
			m->time_tag = MPU_TCIP;	/* don't print time_tag */
			ungetc(c, f);
		} else if (c==MPU_TCIP){
			time += MPU_CLOCK_PERIOD;
			m->time_tag = c;	/* don't print time_tag */
			ungetc(c, f);
		} else if (c < MPU_CLOCK_PERIOD) {
			time += c;
			m->time_tag = c;
		}
		m->mpu_time = time;
		if ((cp = read_ch(f))) m->mpu_cmd = cp;
		m->arg_cnt = read_ch_arg_cnt();
		m->cmd_type = read_ch_cmd_type();
		m->cmd_cont = m->cmd_type == MPU_TCWME? 0 : read_ch_cont();
		return m;
	}
	return NULL;
}
