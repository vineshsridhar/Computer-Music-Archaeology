/*
**	Defines for KMX products (esp. MIDI-Central)
*/

#define ID1_KMX	0x00		/* KMX's MIDI sub-id first byte */
#define ID2_KMX	0x19		/* KMX's MIDI sub-id second byte */
#define SUBID_KMX	((ID1_KMX<<8)|ID2_KMX) /* KMX's MIDI sub-id # */
			/* uses ID_MISC, followed by SUBID_KMX (2 bytes) */
#define ID_KMX_MC	0x01		/* KMX's code for the MIDI-Central */

/* KMX MIDI-Central message numbers */
#define KMX_MC_CUR		0x00		/* current patch configuration */
#define KMX_MC_PRE(p)	(p)		/* patch configurations [1-99] */
#define KMX_MC_LIM		0x64		/* MIDI program change limits */

/* the 16 data bytes that follow KMX_MC_CUR or KMX_MC_PRE(1-99) */
struct	kmxpreset	{
	u_char	output[16];		/* inputs for each output */
};
/* values for output[] */
#define KMX_MC_OFF		0x00		/* no input */
#define KMX_MC_IN(i)		(i)		/* from input i [1-16] */
#define KMX_MC_MRG		0x10		/* merged inputs 1 & 2 */

/* the 4 data bytes that follow KMX_MC_LIM */
struct	kmxlimits	{
	u_char	hi1, lo1;		/* hi & lo prog limits for input 1 */
	u_char	hi2, lo2;		/* hi & lo prog limits for input 2 */
};

#define KMX_MC_NI	15			/* number of inputs */
#define KMX_MC_NO	16			/* number of outputs */
#define KMX_MC_NP	99			/* number of presets */

#undef d
