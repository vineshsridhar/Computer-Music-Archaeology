/*
**	Defines for Iota Systems products (esp. Midi-Fader)
*/

#define ID1_IOTA	0x00		/* Iota Systems' MIDI sub-id first byte */
#define ID2_IOTA	0x08		/* Iota Systems' MIDI sub-id second byte */
#define SUBID_IOTA	((ID1_IOTA<<8)|ID2_IOTA) /* Iota Systems' MIDI sub-id # */
			/* uses ID_MISC, followed by SUBID_IOTA (2 bytes) */
#define ID_IMF	0x01		/* Iota Systems' code for the Midi-Fader */

/* Iota Midi-Fader command numbers */
#define IMF_SXC_DAT_PROGS	0x00	/* all program dump data */
#define IMF_SXC_REQ_PROGS	0x01	/* all program dump request */
#define IMF_SXC_DAT_PROG	0x02	/* one program dump data */
#define IMF_SXC_REQ_PROG	0x03	/* one program dump request */
#define IMF_SXC_DAT_ABUF	0x04	/* active buffer dump data */
#define IMF_SXC_REQ_ABUF	0x05	/* active buffer dump request */
#define IMF_SXC_DAT_SETUP	0x06	/* setup dump data */
#define IMF_SXC_REQ_SETUP	0x07	/* setup dump request */

