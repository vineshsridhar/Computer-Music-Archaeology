#define signed
#define Type typedef struct
#ifndef u_char
#defineefine u_char	unsigned char
#defineefine u_short unsigned short
#endif
#ifndef	UCH
#define UCH(ch)	((u_char) (ch))
#endif
#define MIDI_MAX_CHANS	16	/* maximum midi channels */
#define MIDI_NUM_KEYS	128	/* number of midi key values */

/* masks */
#define M_CMD	0x80		/* midi command bit mask */
#define M_CMD_MASK	0xF0		/* midi command mask */
#define M_CHAN_MASK	0x0F		/* midi channel mask */
#define M_VAL_MASK	0x7F		/* midi value mask */

/* command ids */
/* CH_... channel commands */
#define CH_KEY_OFF	0x80		/* channel key off */
#define CH_KEY_ON	0x90		/* channel key on */
#define CH_POLY_KPRS	0xA0		/* polyphonic key pressure */
#define CH_CTL	0xB0		/* channel control */
#define CH_PRG	0xC0		/* channel program change */
#define CH_PRESSURE	0xD0		/* channel pressure */
#define CH_P_BEND	0xE0		/* channel pitch bend */

/* SX_... system exclusive commands */
#define SX_CMD	0xF0		/* system exclusive command */
#define SX_EOB	0xF7		/* system exclusive end of block */

/* SC_... system common commands */
#define SC_MSEL	0xF2		/* system common measure select */
#define SC_SSEL	0xF3		/* system common song select */
#define SC_TSEL	0xF6		/* system common tune request */

/* RT_... realtime commands */
#define RT_TCLOCK	0xF8		/* timing clock */
#define RT_START	0xFA		/* start (at first measure) */
#define RT_CONT	0xFB		/* continue */
#define RT_STOP	0xFC		/* stop */
#define RT_ASENSE	0xFE		/* active sensing */
#define RT_RESET	0xFF		/* system reset */

#define ID_MISC	0x00		/* manuf ID for misc companies (needs SUBID) */
#define SUBID(x,y)	(((x)<<8)|(y))
#define SUBID_HEADER	SUBID(0x7F,0x7F) /* fake manuf SUBID for headers */

/* default for reading/writing under regular UNIX */

/* template for all midi parametric data */
/* note: watch out for synth. parameters that exceed u_char range! */
Type midi_par {
	u_char par_index;	/* index in hardware table */
	u_char par_val;		/* current value */
	u_char par_ub;		/* upper boundary */
	char *par_name;		/* name */
} MidiPar;

/* same as above, but for parameters requiring u_short storage */
Type midi_spar {
	u_short par_index;	/* index in hardware table */
	u_short par_val;	/* current value */
	u_short par_ub;		/* upper boundary */
	char *par_name;		/* name */
} MidiSpar;

#define TRW_SEL_TTY	1
#define TRW_SEL_RFD	2
#define TRW_SEL_WFD	3

/* struct for status/command byte information in statinfo.c */
struct	statstr	{
	signed char	clen;	/* number of command bytes */
	u_char	flgs;		/* misc. info, see below */
};			/* bits for flgs */
#define RSSET	1		/* Running Status SET by this command */
#define RSCLR	2		/* Running Status CLeaRed by this command */
#define RSNOP	4		/* Running Status unaffected by this command */
#define UNDF		8		/* Not defined in the MIDI spec */

#ifndef	MIDI_EOX
#define MIDI_EOX(b)		(((b)&M_CMD)&&(b)<0xF8&&(b)!=SX_CMD)
#endif
#define MIDI_RT(b)		((b)>=0xF8)
