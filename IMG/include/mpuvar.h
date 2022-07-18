/* @(#)mpuvar.h 1.8 87/05/07
** $Header: mpuvar.h,v 1.6 89/02/21 19:27:51 psl Exp $
*/

/* mpu tracks */
#define MPU_DTR_NUM	8			/* number of data tracks */
#define MPU_CTR_COM	(MPU_DTR_NUM+0)		/* command queue	*/
#define MPU_CTR_CND	(MPU_DTR_NUM+1)		/* conductor queue	*/
#define MPU_TTR_MAX	(MPU_DTR_NUM+2)		/* total number of tracks */

#define MPU_TR_COM	MPU_CTR_COM	/* backwards compatibility */

/* mpu ioctls */
#define MPU_IOC_TRACK	_IOW(m, 0, int	)	/* set track		*/
#define MPU_IOC_PURGE	_IO(m,  1	)	/* purge track buffers	*/
#define MPU_IOC_RESID	_IOR(m, 2, int	)	/* resid count in tracks*/
#define MPU_IOC_LIMIT	_IOW(m, 3, int	)	/* set track buffer limit*/
#define MPU_IOC_EXCL	_IO(m,  4	)	/* set exclusive use	*/
#define MPU_IOC_NXCL	_IO(m,  5	)	/* clear exclusive use	*/
#define MPU_IOC_TWAIT	_IO(m,  6	)	/* wait for track empty	*/
#define MPU_IOC_WSELIM	_IO(m,  7	)	/* write sel < limit	*/
#define MPU_IOC_WSELMT	_IO(m,  8	)	/* write sel trk empty	*/
#define MPU_IOC_PEEK	_IOWR(m,16, int	)	/* peek spec reg	*/
#define MPU_IOC_POKE	_IOWR(m,17, int	)	/* poke spec reg	*/

/* mpud_blimit default value, set for 8k bytes/track with a minimum of 2 */
#define MPUD_DEFBLIMIT	((MAXBSIZE>=4096)?2:(1+8191/(MAXBSIZE)))
/* mpud_blimit max value is 256 * MAXBSIZE bytes/track, but so what? */
#define MPUD_MAXBLIMIT	256	

/* Data structures used only by the driver */
#ifdef KERNEL

/*
 * The following struct is used for user data that is to be sent to the mpu.
 * Data is arranged as linked lists (1 per track) of system buffers.
 * Each time the user issues a write(), the data is copied into a system
 * buffer and that buffer is linked onto the track list.  There is a
 * per-track buffer limit to prevent write()s from congesting the system.
 * Since the mpu issues an interrupt each time it needs more play data,
 * the play data is asynchronous.
 * Mpu command data is now asynchronous, but data is written synchronously
 * until an ACK is expected from the mpu.
 */
struct mpu_l {
	int		mpul_tcount;	/* number of bytes in track	*/
	int		mpul_bcount;	/* number of bytes in buffer	*/
	int		mpul_contin;	/* number of bytes in continuation */
	int		mpul_bused;	/* number of buffers allocated	*/
	unsigned char	*mpul_cp;	/* byte pointer into buffer	*/
	struct buf	mpul_head;	/* head of buffer list		*/
};

/*
 * Use a ring buffer, allocated at open(), for record data
 */
#define MPUBUFSIZ	8192		/* PAGESIZE in 4.0? */

struct mpu_rbuf {
	caddr_t		mr_buf;		/* malloc-ated ring buffer */
	unsigned	mr_ip;		/* input (interrupt data) offset */
	unsigned	mr_op;		/* output (copy to user) offset */
};

/* Macro to determine how many bytes are in the ring buffer */
#define Mpuinq(mr)	(((mr)->mr_ip >= (mr)->mr_op) ? 		\
			    ((mr)->mr_ip - (mr)->mr_op) :		\
			    ((MPUBUFSIZ - (mr)->mr_op) + (mr)->mr_ip))

/*
 * data and state information for the mpu.
 */
struct mpu_device {
	short		mpud_state;	/* state of mpu			*/
	short		mpud_track;	/* current active track		*/
	int		mpud_ocnt;	/* number of current opens	*/
	int		mpud_blimit;	/* # of bufs allowed (per track)*/
	int		mpud_cnt;	/* # of bytes for mpuintr	*/
	int		mpud_contin;	/* continuation bytes (mpuintr)	*/
	int		mpud_spl;	/* spl level of int priority	*/
	struct proc	*mpud_sel;	/* select pending		*/
	struct mpu_rbuf	mpud_ib;	/* incoming data		*/
	struct mpu_l	mpud_trackl[MPU_TTR_MAX];
					/* data, com, & cond track lists */
};

/* mpud_state bits per mpu */
#define MPUD_CLOSE	0		/* turn off all bits	*/
#define MPUD_OPEN	bit(0)		/* open			*/
#define MPUD_CMDTRACK	bit(1)		/* sending mpu commands	*/
#define MPUD_WACK	bit(2)		/* sleeping on ack	*/
#define MPUD_MPU_DATA	bit(3)		/* command w/mpu data	*/
#define MPUD_MIDI_DATA	bit(4)		/* command w/midi data	*/
#define MPUD_TT		bit(5)		/* time tag data	*/
#define MPUD_SMESS	bit(6)		/* system message	*/
#define MPUD_ERROR	bit(7)		/* data format error	*/
#define MPUD_SELWAIT	bit(8)		/* select waiting	*/
#define MPUD_WSELMT	bit(9)		/* write select option	*/
#define MPUD_NOMPU	bit(10)		/* no mpu hooked up	*/
#define MPUD_NOCSR	bit(11)		/* no csr--bad ctrlr	*/
#define MPUD_CLOSING	bit(12)		/* close() in-progress	*/
#define MPUD_EXCL	bit(13)		/* exclusive use	*/

#endif /*KERNEL*/

