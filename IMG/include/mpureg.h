/*
** MPU header file for hardware related definitions
** @(#)mpureg.h 1.9 89/01/09
** $Header: mpureg.h,v 1.8 89/04/12 12:29:58 psl Exp $
** See comment at beginning of mpu.c for MPUAV/MPUAT/... definitions.
*/

#ifdef MPUAV
#define MPUS_PER_BOARD	1	/* Each MultiBus board supports 1 MPU */
#define MPU_SIZE	0x100	/* MultiBus memory space required */
#elif	MPUAT
#define MPUS_PER_BOARD	1	/* Each AtBus board supports 1 MPU */
#define MPU_SIZE	0x002	/* AtBus memory space required */
#elif	MPUDC
#define MPUS_PER_BOARD	4	/* Each VME board supports 4 MPU interfaces */
#define MPU_SIZE	0x020	/* VMEBus memory space required */
#define MPU_INTMASK	0x0F	/* interrupt bit mask (in mpuc_intmask) */
#elif	MPUDJ
#define MPUS_PER_BOARD	4	/* Each VME board supports 4 MPU interfaces */
#define MPU_SIZE	0x020	/* VMEBus memory space required */
#define MPU_INTMASK	0x0F	/* interrupt bit mask (in mpuc_intmask) */
#endif

/* registers of each roland mpu-401 interface */
struct mpu_reg {
#ifdef MPUAV
	unsigned char	mpur_csr;	/* 00101 */
	unsigned char	mpur_data;	/* 00100 */
#elif MPUAT
	unsigned char	mpur_data;	/* 00000 */
	unsigned char	mpur_csr;	/* 00001 */
#elif MPUDC|MPUDJ
	char _x0;			/* 00000 */
	unsigned char	mpur_data;	/* 00001 */
	char _x2;			/* 00010 */
	unsigned char	mpur_csr;	/* 00011 */
#else NOBUS
	No interface definition; naughty, naughty,...
	Oh my, this won't make an error, will it?
#endif NOBUS
};

/* registers of the mpu-401 interface card */
struct mpu_ctlr {
	struct mpu_reg	mpur[MPUS_PER_BOARD];		/* each MPU's regs */
#if MPUDC|MPUDJ
	char		_x16;		/* 10000 */
	unsigned char	mpuc_intmask;	/* 10001 */
#endif MPUDC|MPUDJ
};

/* where do we look to see if the interface card is there at all? */
#if MPUAV|MPUAT
#define mpu_alive	mpur[0].mpur_csr
#elif MPUDC|MPUDJ
#define mpu_alive	mpuc_intmask
#define mpu_reset	mpuc_intmask
#endif MPUDC|MPUDJ

#define bit(n)			(1 << (n))
#ifndef UCH
#define UCH(ch)			((unsigned char) ch)
#endif UCH

/* mpur_csr status bits */
#define MPU_STAT_DRR		bit(6)
#define MPU_STAT_DSR		bit(7)

/* mpu commands (on command track) */
#define MPU_COM_EXCL_THRU	UCH(0x37)
#define MPU_COM_MIDI_THRU	UCH(0x89)
#define MPU_COM_CLRPLAYCNTRS	UCH(0xB8)
#define MPU_COM_CLRPLAYMAP	UCH(0xB9)
#define MPU_COM_RESET		UCH(0xFF)

/* mpu time tags (on play or conductor tracks) */
#define	MPU_TT_MAX		UCH(0xEF)
#define	MPU_TT_TCIP		UCH(0xF8)

/* mpu data (on play or conductor tracks) */
#define MPU_DAT_NOP		UCH(0xF8)
#define MPU_DAT_TCWME		UCH(0xF9)
#define MPU_DAT_DATAEND		UCH(0xFC)

/* mpu messages (sent by the MPU) */
#define	MPU_MESS_TRAKREQ0	UCH(0xF0)
#define	MPU_MESS_TRAKREQ7	UCH(0xF7)
#define	MPU_MESS_TIMOFLO	UCH(0xF8)
#define MPU_MESS_CONDREQ	UCH(0xF9)
#define MPU_MESS_DATAEND	UCH(0xFC)
#define MPU_MESS_CLK2HST	UCH(0xFD)
#define MPU_MESS_ACK		UCH(0xFE)
#define MPU_MESS_SYS		UCH(0xFF)

/* midi misc */
#define	MIDI_CMD_MASK		UCH(0xF0)
#define	MIDI_SX			UCH(0xF0)
#define	MIDI_END_SX		UCH(0xF7)

/* anything with bit 7 on ends sys excl, except RT and MIDI_SX itself */
#ifndef MIDI_EOX
#define	MIDI_EOX(b)		(((b)&bit(7))&&(b)<UCH(0xF8)&&(b)!=MIDI_SX)
#endif MIDI_EOX
