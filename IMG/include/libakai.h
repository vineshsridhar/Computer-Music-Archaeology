/*
**	Defines for Akai products
*/

#define ID_AKAI	0x47		/* Manufacturer's ID */
#define ID_AKAI_S612	0x20		/* Akai S612 ID */
#define ID_AKAI_MPX	0x42		/* Akai MPX-820 ID */
#define ID_AKAI_S900	0x40		/* Akai S900 ID */
#define ID_AKAI_MB76	0x44		/* Akai MB76 ID */

#define AKAI_REQ_DATA	0x41	/* bank data dump request */
#define AKAI_SET_DATA	0x40	/* bank data set request */

/* Data Coding
** Each 8-bit byte is encoded as in two bytes as:
**	00 d6 d5 d4 d3 d2 d1 d0      00 00 00 00 00 00 00 d7
** where d0 is least-significant bit.
*/

/* MPX820 formats */

/* Request for bank data (all numbers hex):
**	F0	sysex
**	47	ID_AKAI
**	CC	channel number (0..F)
**	42	ID_AKAI_MPX
**	41	AKAI_REQ_DATA
**	BB	bank number (00..63 hex)
**	F7	EOX
*/

/* Setting bank data (all numbers hex):
**	F0	sysex
**	47	ID_AKAI
**	CC	channel number (0..F)
**	42	ID_AKAI_MPX
**	40	AKAI_SET_DATA
**	BB	bank number (00..63 hex)
**	...	8 2-byte parameters for each of 8 channels
**		fader1, efx1, pan1, aux2-pan, low-eq1, mon1, mid-eq1, hi-eq1
**		fader2, efx2, pan2, aux2-lvl, low-eq2, mon2, mid-eq2, hi-eq2
**		fader3, efx3, pan3, aux1-pan, low-eq3, mon3, mid-eq3, hi-eq3
**		fader4, efx4, pan4, aux1-lvl, low-eq4, mon4, mid-eq4, hi-eq4
**		fader5, efx5, pan5, main-lvl, low-eq5, mon5, mid-eq5, hi-eq5
**		fader6, efx6, pan6, fx-rcv-R, low-eq6, mon6, mid-eq6, hi-eq6
**		fader7, efx7, pan7, fx-rcv-L, low-eq7, mon7, mid-eq7, hi-eq7
**		fader8, efx8, pan8, fadetime, low-eq8, mon8, mid-eq8, hi-eq8
**	F7	EOX
*/

/* MB76 formats (much guesswork here) */

/* Request for bank data (all numbers hex):
**	F0	sysex
**	47	ID_AKAI
**	CC	channel number (0..F)
**	44	ID_AKAI_MB76
**	41	AKAI_REQ_DATA
**	BB	bank number (00..20 hex)
**	F7	EOX
*/

/* Setting bank data (all numbers hex):
**	F0	sysex
**	47	ID_AKAI
**	CC	channel number (0..F)
**	44	ID_AKAI_MB76
**	40	AKAI_SET_DATA
**	BB	bank number, 00..20 00 is current, 01..20 are 01..32(dec)
**	...	7 1-byte gain parameters for each of 6 outputs, range is 00..0C
**		Inputs 1..7 going to Output 1
**		Inputs 1..7 going to Output 2
**		...
**		Inputs 1..7 going to Output 6
**	F7	EOX
*/

