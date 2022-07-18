/*
**	Defines for Standard MIDI File (SMF) format
**
**	In formats, braces around a value indicate a variable length number
*/
#define	BM(a)		((a)&0xFF)
#define	S2C(a,b,s)	(a)=BM((s)>>8),(b)=BM(s)
#define	S2B(b,s)	S2C((b)[0],(b)[1],s)
#define	L2C(a,b,c,d,l)	(a)=BM((l)>>24),(b)=BM((l)>>16),(c)=BM((l)>>8),(d)=BM(l)
#define	L2B(b,l)	L2C((b)[0],(b)[1],(b)[2],(b)[3],l)
#define	C2S(a,b)	((BM(a)<<8)|BM(b))
#define	C2L(a,b,c,d)	((BM(a)<<24)|(BM(b)<<16)|(BM(c)<<8)|BM(d))
#define	B2S(b)		C2S((b)[0],(b)[1])
#define	B2L(b)		C2L((b)[0],(b)[1],(b)[2],(b)[3])

/* format values */
#define	SMF_FMT_0	0		/* single multi-channel track */
#define	SMF_FMT_1	1		/* multiple simultaneous tracks */
#define	SMF_FMT_2	2		/* seq. indep. single-track chunks */
/* divis values */
#define	SMF_DIV_TPQ(T)	T		/* T ticks/quarter */
#define	SMF_DIV_FT(f,t)	C2S(-(f),t)	/* f=frames/sec, t=ticks/frame */
#define	SMF_DIV_120	120		/* 120 clocks/quarter, MPU-401 */
#define	SMF_DIV_96	96		/* 96 clocks/quarter, pretty standard */
#define	SMF_DIV_MSEC	SMF_DIV(25,40)	/* 1000 clocks/second */

/* chunk header types */
#define	SMF_HDR	C2L('M','T','h','d')	/* header chunk */
#define	SMF_TRK	C2L('M','T','r','k')	/* track chunk */

typedef	struct	sm_chunk_hdr {	/* chunk header (not "header chunk"!) */
	u_char	type[4];	/* see defines above */
	u_char	leng[4];	/* length of rest of chunk (high byte first) */
} SMF_CH;
#define	SMF_CH_SIZ	8

typedef	struct	sm_hdr {	/* header chunk header */
	SMF_CH	chdr;		/* SMF_CHD with chdr.type == SMF_HDR */
	u_char	format[2];	/* S2B() of one of the defines above */
	u_char	ntrks[2];	/* number of track chunks, high byte first */
	u_char	divis[2];	/* delta-time divisions, see defines above */
} SMF_HD;
#define	SMF_HD_SIZ	14	/* != sizeof (SMF_HD) !!! */

/* event types (anything else is a MIDI event) */
#define	SMF_SYSEX	0xF0	/* SMF "normal" sysex event */
			/* format:  SMF_SYSEX {length} <bytes>+ */
				/* 0xF0 is transmitted before <bytes> */
#define	SMF_SYSESC	0xF7	/* SMF "escape" sysex event */
			/* format:  SMF_SYSESC {length} <bytes>+ */
				/* 0xF0 is NOT transmitted before <bytes> */
#define	SMF_META	0xFF	/* SMF meta event */
			/* format:  SMF_META <type> {length} <bytes>+ */

/* meta event type values */
#define	SMF_M_SEQ	0x00	/* Sequence number meta event */
			/* format:  SMF_META SMF_M_SEQ 02 hiseq loseq */
			/* format:  SMF_META SMF_M_SEQ 00  (sequential nums) */
#define	SMF_M_TEXT	0x01	/* Text meta event */
			/* format:  SMF_META SMF_M_TEXT {length} <text> */
#define	SMF_M_COPY	0x02	/* Copyright notice text meta event */
#define	SMF_M_TNAME	0x03	/* Track or sequence name text meta event */
#define	SMF_M_INAME	0x04	/* Instrument name text meta event */
#define	SMF_M_LYRIC	0x05	/* Lyric text meta event */
#define	SMF_M_MARKER	0x06	/* Marker text meta event */
#define	SMF_M_CUETXT	0x07	/* Cue point text meta event */
#define	SMF_M_CHAN	0x20	/* MIDI channel prefix meta event */
#define	SMF_M_EOT	0x2F	/* End of track meta event */
			/* format:  SMF_META SMF_M_EOT 00 */
#define	SMF_M_TEMPO	0x51	/* Set tempo meta event */
			/* format:  SMF_META SMF_M_TEMPO 03 tt tt tt */
#define	SMF_M_SMPTE	0x54	/* SMPTE offset meta event */
			/* format:  SMF_META SMF_M_SMPTE 05 hr mn se fr ff */
#define	SMF_M_TIMSIG	0x58	/* Time signature meta event */
			/* format:  SMF_META SMF_M_TIMSIG 04 nn dd cc bb */
#define	SMF_M_KEYSIG	0x59	/* Key signature meta event */
			/* format:  SMF_META SMF_M_KEYSIG 02 sf mi */
#define	SMF_M_SEQSPEC	0x7F	/* Sequencer specific meta event */
			/* format:  SMF_META SMF_M_SEQSPEC {length} <data>+ */
