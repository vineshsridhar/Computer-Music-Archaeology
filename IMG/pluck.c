/*
**	PLUCK -- Karplus/Strong plucked string algorithm
**
**	pluck(dper, vel, ddur, damp, bp)
**		double	dper;	pitch period in seconds
**		double	ddur;	duration in seconds
**		int	vel;	velocity 0..127
**		int	damp;	damping factor, 0 for none, SBITS for max
**		int	twang;	twanginess, 0 for none, 127 for max
**		int	*bp;	buffer for the output
**	Pluck() provides the specified note; the output is signed,
**	13-bit binary.  The maximum period is 0.125 (8 Hz).
**
**	ipluck(dper, ddur, vel, damp, twang, bp)
**		double	dper;	pitch period in seconds
**		double	ddur;	duration in seconds
**		int	vel;	velocity 0..127
**		int	damp;	damping factor, 0 for none, SBITS for max
**		int	twang;	twanginess, 0 for none, 127 for max
**		char	*bp;	buffer for the output
**	Ipluck() provides the specified note; the output is unsigned,
**	AMD u-law coded bytes.  The maximum period is 0.125 (8 Hz).
**
** This is a technique based on a loop containing a shift register and
** a low-pass filter originally described by Karplus & Strong.
** The shift register starts with random noise in it and loses energy
** both from the low-pass filter as it runs ("heat death") and from
** damping specified by "damp".
** If the shift register (delay line) shifts at rate R and is N long,
** the resulting frequency is:  freq = R / (N + 1 / (twang + 1)).
** In our case we use a circular buffer and a sneaky technique
** to simulate a non-integer length.
** For a low pass filter we simply average adjoining samples with a weighting
** determined by "twang".
**
** We initialize the shift register to noise and then, knowing the total
** offset from zero, go back and adjust all the values by -offset/per.
** This assures zero offset after heat death.
**
** Res is BIG * (the amount we'd like to stretch each sample).
** We keep track of the cumulative stretch error in q; when it gets to be
** greater than 1/2 (HALFBIG), then we duplicate a sample and adjust
** the accumulation by subtracting BIG.
**
**					psl 10/89
*/
#include	"audio.h"

#define	MAXPER	1000	/* maximum period (8 Hz, below C-2) */
#define	BIG	100000000
#define	HALFBIG	 50000000
#define	BSHFT	24		/* x>>BSHFT == x/BIG */
#define	SBITS	10		/* bits of fixed point (beyond 13-bit ints) */
#define	NBITS	(SBITS+13)	/* bits in use */
#define	RSHFT	(32+7-NBITS)	/* assuming 32-bit integers & 7-bit velocity */

#define	GUTS(OUT) \
{ \
	dur = SPS * ddur + 0.5; \
	damp = SBITS - damp; \
	/* for this design, freq = SPS / (dper + 1 / (twang + 1)) */ \
	dper = SPS * dper - 1. / (twang + 1);	/* adjust dper for twang */ \
	per = dper; \
	if (per > MAXPER) \
	    return; \
	d = 0;						/* sum of noise */ \
	for (rp = &r[per]; --rp >= r; ) { \
	    *rp = x = vel * ((rand() << 1) >> RSHFT);	/* noise */ \
	    d += x; \
	} \
	d = d / per;				/* offset for each tick */ \
	for (rp = &r[per]; --rp >= r; ) \
	    *rp -= d; \
	res = BIG * (dper - per) / per;		/* stretch for each tick */ \
	q = 0;					/* cumulative stretch */ \
	for (ep = &bp[dur]; ; --d) {	/* keep looping thru shift register */ \
	    for (rp = &r[per], ox = r[0]; --rp >= r; ox = x) {	/* loop */ \
		if (bp >= ep)			/* until... */ \
		    return;			/* ... no more data needed */ \
		x = *rp; \
		*bp++ = d = OUT;		/* output a sample */ \
		if ((q += res) >= HALFBIG && bp < ep) {	/* check cum */ \
		    *bp++ = d;			/* output an extra sample */ \
		    q -= BIG;			/* and adjust accumulation */ \
		} \
		d = (twang * x + ox) / (twang + 1); \
		*rp = d - (d >> damp); \
	    } \
	} \
}

pluck(dper, ddur, vel, damp, twang, bp)
double	dper, ddur;
register int	*bp;
{
	register int x, *rp, ox, d, q, res, *ep;
	int r[MAXPER + 1], per, dur;

/****fprintf(stderr, "pluck(%g, %g, %d, %d, %d, %x)\n",
 dper, ddur, vel, damp, twang, bp);
/****/
	GUTS(x>>SBITS);
	/*NOTREACHED*/
}

ipluck(dper, ddur, vel, damp, twang, bp)
double	dper, ddur;
register char	*bp;
{
	register int x, *rp, ox, d, q, res;
	register char *ep;
	int r[MAXPER + 1], per, dur;

/****fprintf(stderr, "ipluck(%g, %g, %d, %d, %d, %x)\n",
 dper, ddur, vel, damp, twang, bp);
/****/
	GUTS(Sb2i[x>>SBITS]);
	/*NOTREACHED*/
}

