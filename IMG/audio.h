/*
**	AUDIO.H -- Header file for SPARCstation audio routines	psl 10/89
*/
#include	<sys/ioctl.h>
#include	<sun/audioio.h>
#include	<stdio.h>

#define	UNSET		-999
#define	AUDIODEV	"/dev/audio"
#define	AUDIOLOCK	"/tmp/audio.lock"
#define	AOUTLOCK	"/tmp/audio.lock"
#define	AINLOCK		"/tmp/audioin.lock"

/* DEFINES FOR THE AMD Am79C30A CHIP */

#define	SPS		8000	/* Am79C30A data rate in samples per second */

/* DEFINES FOR u-LAW AND BINARY DATA FORMATS */

#define	IZERO	0x00FF		/* zero offset in u-code (ISDN) tables */
#define	B8ZERO	0x0080		/* zero offset in unsigned 8-bit tables */
#define	B8RNG	0x0080		/* range from min to zero */
#define	B8MAX	0x00FF		/* largest offset in unsigned 8-bit tables */
#define	B8LIMIT(a)	(((a)<0)?0:(((a)>B8MAX)?B8MAX:(a)))

#define	B13ZERO	0x1000		/* zero offset in unsigned 13-bit tables */
#define	B13RNG	0x1000		/* range from min to zero */
#define	B13MAX	0x1FFF		/* largest offset in unsigned 13-bit tables */
#define	B13LIMIT(a)	(((a)<0)?0:(((a)>B13MAX)?B13MAX:(a)))
#define	SB13LIMIT(a)	(((a)<-B13RNG)?-B13RNG:(((a)>=B13RNG)?B13RNG-1:(a)))

#define	UNITY	0x10000		/* fixed-point number scaling with B13 */
#define	UNISHFT	16		/* 4*(B13MAX<<UNISHFT) fits in a signed int */
#define	FPMUL(a,b)	(((a)*(b))>>UNISHFT)	/* B13 fixed-point multiply */
#define	FPDIV(a,b)	(((a)<<UNISHFT)/(b))	/* B13 fixed-point multiply */
#define	D2FP(a)		((int)((a)*UNITY))	/* double to B13 fixed-point */
#define	FP2D(a)		((a)/(double)UNITY)	/* B13 fixed-point to double */


/* MISCELLANEOUS DEFINES AND GLOBALS */

#define	uchar	unsigned char
#define	ushort	unsigned short

#define	PI	3.141592653589793238462643
#define	TWOPI	6.283185307179586476925287

extern	int	Avflg;		/* verbose error messages */

/* original values, saved so they can be reset by audioclose() */

extern	uchar	Ub2i[];		/* unsigned 13-bit binary to 8-bit ISDN code */
extern	ushort	I2ub[];		/* 8-bit ISDN to unsigned 13-bit binary code */
extern	uchar	*Sb2i;		/* signed 13-bit binary to 8-bit ISDN code */
extern	short	I2sb[];		/* 8-bit ISDN to signed 13-bit binary code */

#ifdef	DELAY_LINE
struct	dlstr	{		/* delay line */
	int	*data;		/* pointer to the start of the storage */
	int	*dp;		/* next location */
	int	*dep;		/* pointer to the end+1 of the storage */
};
extern	struct	dlstr	*dlinit();
#endif	DELAY_LINE

#ifdef	EKSA
typedef	struct	ksastr	{	/* struct for extended Karplus-Strong */
	int	blen;		/* length of delay line */
	double	*dl;		/* delay line */
	double	*dp;		/* next data pointer */
	double	*dep;		/* data end pointer */
	double	ac;		/* coefficient for weighted average */
	double	cc;		/* coefficient for tuning filter */
	double	lao;		/* previous lowpass output */
	double	lbo;		/* previous delay line output */
	double	lco;		/* previous allpass output */
	double	lg;		/* loop gain (0 for max decay, 1 for none) */
} KSASTR;
extern	double	eksa();
extern	KSASTR	*eksainit();
#endif	EKSA
