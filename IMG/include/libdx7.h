/*
**	Defines for Yamaha DX/TX devices
*/

#ifndef u_char
#define u_char	unsigned char
#define u_short unsigned short
#endif
#define ID_YAMAHA	67	/* Yamaha manufacturer MIDI id # */
#define ID_DX7	ID_YAMAHA

/* dx7 buttons (for dx7Button() et al. */
#define DX7store			32
#define DX7mem_protect_internal	33
#define DX7mem_protect_cartridge	34
#define DX7operator_select		35
#define DX7edit			36
#define DX7mem_select_internal	37
#define DX7mem_select_cartridge	38
#define DX7function			39
#define DX7no			40
#define DX7yes			41
#define dx7yes(n)		(n)? DX7yes : DX7no
#define DX7sysinfo			 7

#define DX7VOXLEN	155	/* length of 1 voice in dx7 single voice table */
#define DX732VOXLEN	128	/* length of 1 voice in dx7 32-voice table */
#define DX732VTOTLEN	(32*DX732VOXLEN)	/* total length of 32-voice dump */
#define DX7COMLEN	4960	/* length of dx7 common voice table */
#define TX1PVLEN	30	/* length of 1 "voice" in tx7 single performance dump */
#define TX64PVLEN	16	/* length of 1 "voice" in tx7 64 performance dump */
#define TX1PLEN	94	/* length of 1 perf. in tx7 single performance dump */
#define TX64PLEN	64	/* length of 1 perf. in tx7 64 performance dump */
#define TX64PTOTLEN	(64*TX64PLEN)	/* total length of 64 performance dump */
#define DX7FUNLEN	14	/* length of dx7 function table */
#define DX7CTLLEN	127	/* length of dx7 control table */
#define DX7KBDLEN	127	/* length of dx7 keyboard table */
#define DX7NVOX	32	/* number of dx7 voices */
#define DX7NVOICES	16	/* 16 simultaneous voices in poly mode*/
#define DX7OPLEN	21	/* length of dx7 operator table */
#define DX732VOPLEN	17	/* length of 32-voice operator table */
#define DX7NOPS	6	/* number of dx7 operators per voice */
#define DX7OPSLEN	(DX7OPLEN*DX7NOPS)	/* total len of op tbl*/
#define DX732VOPSLEN	(DX732VOPLEN * DX7NOPS)	/* 32-voice op tbl len*/

/*  SXSS - SYSEX SUB-STATUS; high-order nybble of sub-status/channel(dev #) */
#define DX7_SXSS_BD		0	/* dx7 bulk data sub-status */
#define DX7_SXSS_PC		1	/* dx7 parameter change sub-status */
#define DX7_SXSS_DR		2	/* dx7 dump request sub-status */
/*  SXF - SYSEX FORMAT */
#define DX7_SXF_1V		0	/* dx7 1 voice format */
#define TX_SXF_1P		1	/* tx7 1 performance format */
#define DX1_SXF_1P		2	/* dx1 1 parameter format [should be 1] */
#define TX_SXF_64P		2	/* tx7 64 performance format */
#define DX7_SXF_1S		5	/* dx7 supplement edit format */
#define DX7_SXF_32S		6	/* dx7 packed 32 supplement format */
#define DX7_SXF_32V		9	/* dx7 32 voice format */
#define RX11_SXF_P		11	/* rx11 parameter format */
#define TX_SXF_CA		125	/* tx7 condition ack. format */
#define YAM_SXF_UBD		126	/* universal bulk dump format */
#define RX11_SXF_PS		127	/* rx11 patterm/song/chain dump format */
/*  SXPG - SYSEX PARAMETER GROUP; high-order 6 bits of param. group. num. */
#define DX7_SXPG_V		0	/* dx7 voice parameter group */
#define DX_SXPG_P		1	/* dx performance parameter group */
#define DX7_SXPG_F		2	/* dx7 function parameter group */
#define DX9_SXPG_F		3	/* dx9 function parameter group */
#define TX_SXPG_F		4	/* tx function parameter group */
#define DX7_SXPG_S		6	/* dx7II supplement parameter group */

/*  CTL - CONTROL; from control change (0xBn) commands */
#define DX7_CTL_MOD_WHEEL	1	/* dx7 chan mod wheel control */
#define DX7_CTL_BREATH	2	/* dx7 chan breath control */
#define DX7_CTL_AFTER	3	/* dx7 chan after touch control */
#define DX7_CTL_FCONT	4	/* dx7 chan foot pedal control */
#define DX7_CTL_PORTA_TIME	5	/* dx7 chan portamento control */
#define DX7_CTL_DE_KNOB	6	/* dx7 chan data entry knob control */
#define DX7_CTL_VOLUME	7	/* dx7 chan volume control */
#define DX7_CTL_SUST_FSW	64	/* dx7 chan sustain pedal control */
#define DX7_CTL_PORTA_FSW	65	/* dx7 chan portamento control */
#define DX7_CTL_SOST_FSW	66	/* dx7 chan sostenuto pedal control */
#define DX7_CTL_SOFT_FSW	67	/* dx7 chan soft pedal control */
#define DX7_CTL_DE_PLUS	96	/* dx7 chan data entry + sw. control */
#define DX7_CTL_DE_MINUS	97	/* dx7 chan data entry - sw. control */
#define DX7_CTL_OMNI_AKO	125	/* dx7 chan omni all keys off control */
#define DX7_CTL_MONO_AKO	126	/* dx7 chan mono all keys off control */
#define DX7_CTL_POLY_AKO	127	/* dx7 chan poly all keys off control */

extern struct midi_par
	dx7_ini_vox[], dx7_ini_fun[], dx7_ch_info[], dx7_xmit_ctl[],
	dx7_rcv_ctl[], dx7_32v_vox[];
extern u_short dx7_ini_vox_len, dx7_ini_fun_len, dx7_ch_info_len,
	dx7_xmit_ctl_len, dx7_rcv_ctl_len, dx7_32v_vox_len;

struct dx7 {	/* one dx7 contains storage for all the following */
	u_char dx7_vox_val[DX7NVOX][DX7VOXLEN];	/* voice data */
	u_char dx7_fun_val[DX7FUNLEN];		/* function data */
	u_char dx7_kbd_val[DX7CTLLEN];		/* keyboard data */
	u_char dx7_ctl_val[DX7CTLLEN];		/* controller data */
	u_char dx7_nsv;				/* voices on */
	u_char dx7_prg_val;			/* program */
	u_short dx7_pbd_val;			/* pitch bender */
};

/* DX7_... == macros to stuff dx7 structure */
/* set voice parameter */
#define DX7_VP(d, voice, param) 	((d).dx7_vox_val[voice][param])
/* set function parameter */
#define DX7_FP(d, param) 		((d).dx7_fun_val[param])
/* set control parameter */
#define DX7_CP(d, param) 		((d).dx7_ctl_val[param])
/* set keyboard velocity parameter */
#define DX7_KP(d, param) 		((d).dx7_kbd_val[param])
/* set program parameter */
#define DX7_PP(d) 			((d).dx7_prg_val)
/* set pitch bend parameter */
#define DX7_BP(d) 			((d).dx7_pbd_val)

/* 
 * translate common voice parameter index in the range
 * 0 to DX7VOXLEN to ini_... array field 
 */
#define DX7_INI_IND(i) ((i)<DX7OPSLEN? (i)%DX7OPLEN : ((i)%DX7OPSLEN) + DX7OPLEN)

/* 
 * set voice parameter with bounds checking - expression evaluates to -1
 * if val < 0 || val > upper bound for that parameter
 */
#define DX7_SET_VP(d, voice, param, v) 	\
    (DX7_VP(d,voice,param) =		\
	((v)<0 || (v)>dx7_ini_vox[DX7_INI_IND(param)].par_ub) ? -1 : (val))

/* dx7 voice operator parameter symbolic names */
/* operator rate amplitude envelope array */
#define OP_AMP_RATE_BASE	0
#define OP_AMP_RATE_LEN	4
#define op_amp_rate(i)	(OP_AMP_RATE_BASE + i)
/* operator level amplitude envelope array */
#define OP_AMP_LEVEL_BASE	4
#define OP_AMP_LEVEL_LEN	4
#define op_amp_level(i)	(OP_AMP_LEVEL_BASE + i)
#define op_kls_breakpoint	8	/* keyboard level sense */
#define op_kls_left_depth	9
#define op_kls_right_depth	10
#define op_kls_left_curve	11
#define op_kls_right_curve	12
#define op_kbd_rate_sc	13	/* keyboard rate scale */
#define op_mod_sens_amp	14	/* amp. mod. sensitivity */
#define op_key_vel_sens	15	/* key velocity sensitivity */
#define op_output_level	16	/* output level */
#define op_osc_mode		17	/* osc. mode */
#define op_osc_frq_coarse	18	/* osc. coarse freq. */
#define op_osc_frq_fine	19	/* osc. fine freq. */
#define p_detune		20	/* osc. detune */

/* OP == set operator in voice */
#define OP(n, p)		((DX7NOPS - n) * DX7OPLEN + p)
/* Note: for each voice, operator table goes from 5 -> 0 */
/* set operator n to parameter p */

/* voice parameters other than operators */
#define VOX_PITCH_RATE_BASE	126
#define VOX_PITCH_RATE_LEN	4
#define vox_pitch_rate(i)	(VOX_PITCH_RATE_BASE + i)
#define VOX_PITCH_LEVEL_BASE	130
#define VOX_PITCH_LEVEL_LEN	4
#define vox_pitch_level(i)	(VOX_PITCH_LEVEL_BASE + i)
#define vox_alg		134	/* algorithm select */
#define vox_feedback		135	/* oscillator feedback ena. */
#define vox_osc_sync		136	/* osc. sync. */
#define lfo_speed		137	/* lfo speed */
#define lfo_delay		138	/* lfo delay */
#define lfo_pmd		139	/* lfo pitch mod. depth */
#define lfo_amd		140	/* lfo amplitude mod. depth */
#define lfo_sync		141	/* lfo sync. ena. */
#define lfo_wave		142	/* lfo waveform select */
#define vox_mod_sens_pitch	143	/* mod. sens. pitch */
#define vox_transpose	144	/* transpose oscillator */
#define VOX_NAME_BASE	145
#define VOX_NAME_LEN		10
#define VOX_32V_NAME_BASE	118
#define vox_voice_name	VOX_NAME_BASE
#define vox_op_ena		155	/* operator enable table */

/* function parameters */
#define fun_mono_poly		0	/* mono/poly mode change */
#define fun_pitch_bend_range		1	/* pitch bend range */
#define fun_pitch_bend_step		2	/* pitch bend step */
#define fun_porta_mode		3	/* portamento ena. */
#define fun_porta_gliss		4	/* portamento glissando ena. */
#define fun_porta_time		5	/* portamento time */
#define fun_mod_wheel_range		6	/* mod. wheel range */
#define fun_mod_wheel_assign		7	/* mod. wheel assign */
#define fun_foot_range		8	/* foot controller range */
#define fun_foot_assign		9	/* foot controller asign */
#define fun_breath_range		10	/* breath controller range */
#define fun_breath_assign		11	/* breath controller assign */
#define fun_after_touch_range	12	/* after touch range */
#define fun_after_touch_assign	13	/* after touch assign */

/* copy voice name field into dx7 structure */
#define DX7_SET_VNAME(dx, voice, name) strncpy(&DX7_VP(dx,voice,vox_voice_name),name,VOX_NAME_LEN)

/* get voice name address */
#define DX7_VNAME_ADDR(dx, voice) (&(dx).dx7_vox_val[voice][VOX_NAME_BASE])

/* copy voice name field out of dx7 structure */
#define DX7_GET_VNAME(dx, buf, voice) strncpy(buf, DX7_VNAME_ADDR(dx, voice), VOX_NAME_LEN)

#define dx7_MIN	36		/* low C2 on the dx7 keyboard */
#define dx7_MAX	96		/* high C7 on the dx7 keyboard */

#ifdef	UNDEF		/* The idiots at Sun defined max(), so this dies */
/* dx7 voice param specifier, stuff needed for talking about a voice param  */
struct	dx7_vs {
	char	*name;	/* name of parameter, osc. sync., lfo speed, etc. */
	int	max;	/* maximum value */
};
#endif

#define TX1PKMODLOC	60	/* where we're stuffing key-assign-mode (KMOD) from */
			/* the 64-perf dump - not documented for the 1-perf */
			/* It would be nice to know where this really goes... */
