typedef struct mpu_cmd {
	u_char	time_tag;
	u_char	arg_cnt;
	u_char	cmd_cont;
	u_char	cmd_type;
	long	mpu_time;
	u_char	*mpu_cmd;
} MpuCmd;
extern MpuCmd *GetMpuCmd();

typedef	struct	mcmdstr	{
	long	when;		/* absolute time, in MIDI clocks */
	int	len;		/* length of command stored at cmd */
	u_char	*cmd;		/* pointer to midi data (no time tag) */
} MCMD;
extern	MCMD	Rt_tcwme;	/* MPU_TCWME command */
extern	MCMD	Mpu_nop;	/* MPU_NO_OP command */
extern	MCMD	*getmcmd();	/* read MPU commands */
extern	MCMD	*getmcmdb();	/* getmcmd() with user-supplied buffer */
long	putmcmd(FILE *ofp, MCMD *mp);	/* write MPU commands (single stream) */
extern	long	Onow;		/* output clock (single stream) */
extern	long	putmcmds();	/* write MPU commands (multiple stream) */
#define	MAXSID	16		/* max simultaneous output streams */
extern	long	Sonow[MAXSID];	/* output clocks (mult stream) */

/* The following definitions taken from the MPU technical reference manual */
#define MPU_DEV_0			"/dev/mpu0"
#define MPU_DEV_1			"/dev/mpu1"
#define MPU_DEV_2			"/dev/mpu2"
#define MPU_DEV_3			"/dev/mpu3"
#define MidiDevice			MPU_DEV_0
#define MPU_CLOCK_PERIOD 		240
#define MPU_DEFAULT_TIME_BASE	120
#define MPU_DEFAULT_TEMPO		(60.0)	/* beats/minute */
#define MPU_TPS(tempo, time_base)	(((time_base)*tempo)/60.0)
#define MPU_DEFAULT_TICS_PER_SEC	MPU_TPS(MPU_DEFAULT_TEMPO,MPU_DEFAULT_TIME_BASE)
#define MPU_STOP_PLAY		0x05
#define MPU_START_PLAY		0x0A
#define MPU_CONTINUE_PLAY		0x0B
#define MPU_STOP_RECORD		0x11
#define MPU_STOP_OVERDUB		0x15
#define MPU_RECORD_STANDBY		0x20
#define MPU_START_RECORD		0x22
#define MPU_START_OVERDUB		0x2A
#define MPU_ALL_NOTE_OFF		0x30	/* turn all notes off */
#define MPU_NO_REAL_TIME		0x32	/* no real time messages to MIDI OUT */
#define MPU_THRU_OFF			0x33	/* disable THRU function */
#define MPU_TIMING_ON		0x34
#define MPU_MODE_MESS_ON		0x35	/* let host see mode msgs */
#define MPU_THRU_ACTIVE_SENSING	0x36	/* ??? */
#define MPU_THRU_EXCLU_ON		0x37	/* allow sysex msgs from MIDI in->out */
#define MPU_COMMON_TO_HOST_ON	0x38	/* let host see system-common msgs */
#define MPU_REAL_TIME_TO_HOST_ON	0x39	/* let host see real-time msgs */
#define MPU_UART_MODE		0x3F	/* simple uart-mode; MPU_RESET ends */
#define MPU_CHAN_REF_TABLE_A		0x40	/* +chan, set table A to control chan */
#define MPU_CHAN_REF_TABLE_B		0x50	/* +chan, set table B to control chan */
#define MPU_CHAN_REF_TABLE_C		0x60	/* +chan, set table C to control chan */
#define MPU_CHAN_REF_TABLE_D		0x70	/* +chan, set table D to control chan */
#define MPU_INT_CLOCK		0x80	/* enable mpu clock; cf C0-C8, E0-E2 */
#define MPU_FSK_CLOCK		0x81	/* sync to FSK TONE @ TAPE IN jack */
#define MPU_MIDI_CLOCK		0x82	/* sync to MIDI clock @ MIDI IN jack */
#define MPU_METRO_NO_ACC		0x83	/* turn mpu metronome on, no accent */
#define MPU_METRO_OFF		0x84	/* turn mpu metronome off */
#define MPU_METRO_ACC		0x85	/* turn mpu metronome on, with accent */
#define MPU_BENDER_OFF		0x86	/* no continuous control msgs to host */
#define MPU_BENDER_ON		0x87	/* enable continuous control msgs */
#define MPU_MIDI_THRU_OFF		0x88	/* disable MPU MIDI IN -> MIDI OUT */
#define MPU_MIDI_THRU_ON		0x89	/* enable MPU MIDI IN -> MIDI OUT */
#define MPU_DATA_STOP_RECORD_OFF	0x8A	/* no MIDI IN -> host while in stop */
#define MPU_DATA_STOP_RECORD_ON	0x8B	/* send MIDI IN -> host while in stop */
#define MPU_SEND_MEASURE_END_OFF	0x8C	/* no MPU_TCWME sent while in record */
#define MPU_SEND_MEASURE_END_ON	0x8D	/* send MPU_TCWME while in record */
#define MPU_CONDUCTOR_ON		0x8E	/* allow CONDUCTOR cmnds in play mode */
#define MPU_CONDUCTOR_OFF		0x8F	/* no CONDUCTOR cmnds in play mode */
#define MPU_RT_AFFECT_OFF		0x90	/* ignore real-time msgs from MIDI IN */
#define MPU_RT_AFFECT_ON		0x91	/* act on real-time msgs from MIDI IN */
#define MPU_FSK_TO_INT		0x92	/* set tape sync res to int TIMEBASE */
#define MPU_FSK_TO_MIDI		0x93	/* set tape sync res to 24/beat */
#define MPU_CLOCK_TO_HOST_OFF	0x94	/* don't send CLOCK-TO-HOST (0xFD) */
#define MPU_CLOCK_TO_HOST_ON		0x95	/* send 0xFD at rate set with 0xE7 */
#define MPU_EXCLUSIVE_TO_HOST_OFF	0x96	/* no SysExcl from MIDI IN to host */
#define MPU_EXCLUSIVE_TO_HOST_ON	0x97	/* send SysExcl from MIDI IN to host */
#define MPU_CHAN_REF_TABLE_A_OFF	0x98	/* disable channel reference table A */
#define MPU_CHAN_REF_TABLE_A_ON	0x99	/* enable channel reference table A */
#define MPU_CHAN_REF_TABLE_B_OFF	0x9A	/* disable channel reference table B */
#define MPU_CHAN_REF_TABLE_B_ON	0x9B	/* enable channel reference table B */
#define MPU_CHAN_REF_TABLE_C_OFF	0x9C	/* disable channel reference table C */
#define MPU_CHAN_REF_TABLE_C_ON	0x9D	/* enable channel reference table C */
#define MPU_CHAN_REF_TABLE_D_OFF	0x9E	/* disable channel reference table D */
#define MPU_CHAN_REF_TABLE_D_ON	0x9F	/* enable channel reference table D */
#define MPU_REQUEST_PLAY_CTR		0xA0	/* +track num, request play counter */
#define MPU_REQUEST_RECORD_CTR	0xAB	/* request then clear, rec counter */
#define MPU_REQUEST_VERSION		0xAC	/* request MPU version number */
#define MPU_REQUEST_REVISION		0xAD	/* request MPU revision number */
#define MPU_REQUEST_TEMPO		0xAF	/* request MPU tempo setting */
#define MPU_RESET_TEMPO		0xB1	/* reset tempo to 1/1 (== 0xE1 0x40) */
#define MPU_CLEAR_PLAY_COUNTERS	0xB8	/* clear play counters */
#define MPU_CLEAR_PLAY_MAP		0xB9	/* clear play map */
#define MPU_CLEAR_RECORD_CTR		0xBA	/* clear record counter */
#define MPU_TIMEBASE_48		0xC2	/* set timebase to 48 */
#define MPU_TIMEBASE_72		0xC3	/* set timebase to 72 */
#define MPU_TIMEBASE_96		0xC4	/* set timebase to 96 */
#define MPU_TIMEBASE_120		0xC5	/* set timebase to 120 (recommended) */
#define MPU_TIMEBASE_144		0xC6	/* set timebase to 144 */
#define MPU_TIMEBASE_168		0xC7	/* set timebase to 168 */
#define MPU_TIMEBASE_192		0xC8	/* set timebase to 192 */
#define MPU_WANT_TO_SEND_DATA	0xD0	/* +chan, send raw MIDI data */
#define MPU_SEND_SYSTEM_MESSAGE	0xDF	/* send sys (excl or common) msg */
#define MPU_TEMPO			0xE0	/* set beats/minute with next byte */
#define MPU_RELATIVE_TEMPO		0xE1	/* set tempo ratio with next byte */
#define MPU_GRADUATION		0xE2	/* set rate of change with next byte */
#define MPU_MIDI_METRO		0xE4	/* set MIDI clocks/beep (next byte) */
#define MPU_METRO_MEAS		0xE6	/* set beats/measure with next byte */
#define MPU_SET_INT_CLOCK		0xE7	/* set CLOCK_TO_HOST rate (next byte) */
#define MPU_ACTIVE_TRACKS		0xEC	/* en/disable tracks with next byte */
#define MPU_SEND_PLAY_CTR		0xED	/* select tracks with next byte */
#define MPU_CHANNELS_1to8		0xEE	/* en/disable chans 1-8 w/next byte */
#define MPU_CHANNELS_9to16		0xEF	/* en/disable chans 9-16 w/next byte */
#define MPU_TRACK_DATA_REQ		0xF0	/* +track, ready for more play data */
#define MPU_NO_OP			0xF8	/* w time-tag, kill time */
#define MPU_TIMING_OVERFLOW		0xF8	/* w/o time-tag, aka MPU_TCIP */
#define MPU_TCIP			0xF8	/* Timing Clock in Play (synonym) */
#define MPU_MEASURE_END		0xF9	/* aka MPU_TCWME */
#define MPU_TCWME			0xF9	/* synonym for MPU_MEASURE_END */
#define MPU_COND_DATA_REQ		0xF9	/* ready for conductor data */
#define MPU_DATA_END			0xFC	/* end of track data */
#define MPU_TCIS			0xFC	/* Timing Clock in Stop (synonym) */
#define MPU_ALL_END			0xFC	/* w/o time-tag, all tracks played */
#define MPU_CLOCK_TO_HOST		0xFD	/* w/o time-tag, see 0x95 & 0xE7 */
#define MPU_ACK			0xFE	/* w/o time-tag, command acknowledge */
#define MPU_RESET			0xFF	/* reset MPU to power up condition */
#ifndef Alloc
#defineefine Alloc(x) (x *)malloc(sizeof(x))
#endif
#ifndef loop
#define loop(i,j) for(i=0;i<j;i++)
#endif
