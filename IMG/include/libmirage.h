/*
**	Defines for Ensoniq Mirage
**	All defines marked '$' apply to both MASOS & the "normal" OS/
**	All other defines apply only to MASOS.
*/

#define MIR_SBPP	256		/* Mirage sample bytes per page */
#define MIR_SNPP	2*MIR_SBPP	/* Mirage sample nybbles per page */

#define ID_ENSONIQ	0x0F		/*$ Ensoniq manufacturer MIDI id # */
#define ID_MIRAGE	0x01		/*$ Ensoniq code for the Mirage */

/* System Exclusive format numbers */
#define MIR_SXW_LOWER	  0x00	/*$ used in defines marked '%' and elsewhere */
#define MIR_SXW_UPPER	  0x10	/*$ used in formats marked '%' and elsewhere */
#define MIR_SXF_CC		0x01	/*$ command code (front panel buttons, cf) */
#define MIR_CC_END		  0x7F	/*$    command end code */
#define MIR_SXF_REQ_PROG	0x03	/*$ % (lower or upper) program dump request */
#define MIR_SXF_DAT_PROG	0x05	/*$ % (lower or upper) program dump data */
#define MIR_SXF_REQ_CPAR	0x00	/* Mirage config. param dump request */
#define MIR_SXF_DAT_CPAR	0x02	/* Mirage config. param dump data */
#define MIR_SXF_REQ_WAVE	0x04	/*$ wavesample dump request */
#define MIR_SXF_DAT_WAVE	0x06	/*$ wavesample dump data */
#define MIR_SXF_PROG		0x07	/* front panel program change */
#define MIR_SXF_WAVE		0x08	/* front panel wavesample change */
#define MIR_SXF_WACK		0x09	/*$ wavesample dump checksum is good */
#define MIR_SXF_WNAK		0x0A	/*$ wavesample dump checksum is bad */
#define MIR_SXF_REQ_AWAVE	0x0B	/* wavesample dump absolute request */
#define MIR_SXF_DAT_AWAVE	0x0C	/* wavesample dump absolute data */
#define MIR_SXF_PPAR		0x0D	/* program parameter change */
#define MIR_SXF_WPAR		0x0E	/* wavesample parameter change */
#define MIR_SXF_WSFUNC	0x0F	/* wavesample manipulation function */
#define MIR_WSFUNC_COPY	  0x00	/*    % copy source to destination */
#define MIR_WSFUNC_SCALE	  0x01	/*    % scale source */
#define MIR_WSFUNC_ADD	  0x02	/*    % destination = destination + source */
#define MIR_WSFUNC_INVERT	  0x03	/*    % invert source */
#define MIR_WSFUNC_REVERSE	  0x04	/*    % reverse source */
#define MIR_WSFUNC_REPL	  0x05	/*    % replicate source page */
#define MIR_WSFUNC_LROT	  0x06	/*    % left rotate current wavesample */
#define MIR_WSFUNC_RROT	  0x07	/*    % right rotate current wavesample */
#define MIR_PROG_LEN		4	/* length of prog change mesg */
#define MIR_WAVE_LEN		4	/* length of wavesample change mesg */
#define MIR_WACK_LEN		3	/* length of wavesample checksum ack mesg */
#define MIR_WNAK_LEN		3	/* length of wavesample checksum nak mesg */
#define MIR_PPAR_LEN		7	/* length of prog param change mesg */
#define MIR_WPAR_LEN		7	/* length of wavesample param change mesg */

/*$ Mirage front panel buttons */
#define MIRBUT_PROG		0x00
#define MIRBUT_0		0x00
#define MIRBUT_1		0x01
#define MIRBUT_2		0x02
#define MIRBUT_3		0x03
#define MIRBUT_4		0x04
#define MIRBUT_5		0x05
#define MIRBUT_6		0x06
#define MIRBUT_7		0x07
#define MIRBUT_8		0x08
#define MIRBUT_9		0x09
#define MIRBUT_ENTER		0x0A
#define MIRBUT_START		0x0A
#define MIRBUT_CANCEL	0x0B
#define MIRBUT_STOP		0x0B
#define MIRBUT_PARAM		0x0C
#define MIRBUT_VALUE		0x0D
#define MIRBUT_ON		0x0E
#define MIRBUT_UP		0x0E
#define MIRBUT_OFF		0x0F
#define MIRBUT_DOWN		0x0F
#define MIRBUT_LOADUPPER	0x10
#define MIRBUT_LOADLOWER	0x11
#define MIRBUT_SAMPLEUPPER	0x12
#define MIRBUT_SAMPLELOWER	0x13
#define MIRBUT_RECSEQ	0x14
#define MIRBUT_PLAYSEQ	0x15
#define MIRBUT_LOADSEQ	0x16
#define MIRBUT_SAVESEQ	0x17
#define MIRBUT_MAX		0x17

/*$ Mirage parameter display formats */
#define MP_BOOL	1		/* 'of' or 'on' */
#define MP_DEC	2		/* decimal */
#define MP_HEX	3		/* hexadecimal */

/*$ Mirage parameter types */
#define MP_UNUSED	0		/* the default if nothing specified */
#define MP_PROG	MIR_SXF_PPAR	/* program, sends this when changed */
#define MP_WAVE	MIR_SXF_WPAR	/* wavesample, sends this when changed */
#define MP_WSEL	MIR_SXF_WAVE	/* sample select, sends this when changed */
#define MP_ACT	99		/* action sequence, not a stored value */

struct	mirpstr	{		/* Mirage parameter characteristics */
	char	*pname;		/* parameter name */
	char	*punit;		/* "octaves", "bytes", etc. */
	u_char	ptype;		/* MP_ACT, MP_PROG, MP_WAVE, MP_GLOB, */
	u_char	pfmt;		/* display format, MP_BOOL, MP_DEC, MP_HEX, */
	u_char	pbase;		/* 0, or 1, based numbers */
	u_char	pfact;		/* mult. factor for storage, 1, 2, */
	u_char	lolim;		/* lowest value */
	u_char	hilim;		/* highest value */
};

extern	char	*Mir_cdn[];	/* Mirage configuration dump names */
extern	char	*Mir_ppbn[];	/* Mirage program parameter block names */
extern	char	*Mir_wcbn[];	/* Mirage wavesample control block names */
extern	char	*Mir_but[];	/* Mirage button names */
extern	struct	mirpstr	Masos_par[];	/* parameters for MASOS */
