/*
** LICK -- Generate "random" banjo parts
** lick1d module: a "smart" part for "melodic" playing
** psl 10/87
*/
#include "lick.h"

#undef d
#define NUMTRIES 12 /* how many to choose from */
// #define TOPFRET 12 /* unused */

// #define D4 62 /* default average pitch for eval() */

#define T 1 /* masks for Rhpat */
#define I 2
#define M 4

int Rhpat[][PATLEN] = {
	T, I, M, T, I, M, T, M, /* forward */
	T, M, I, T, M, I, T, M, /* backward */
	T, I, M, T, M, I, T, M, /* backup */
	T, I, M, T, T, I, M, T, /* dbld thumb */
	T, I, M, T, M, T, M, I, /* ? */
	T, M, T, M, T, M, T, M, /* flat-pick */
	I, M, I, M, T, I, M, T, /* foggymtn */
	T, I, T, M, T, I, T, M, /* double thumb */
	T, I, T, M, T, 0, T|M, 0, /* John Hickman */
	I, 0, T|M, 0, I, 0, T|M, 0, /* bum-chang */
};
#define NRHPAT (sizeof Rhpat / sizeof Rhpat[0])
int Nrhpat = NRHPAT; /* number of patterns we can use */
struct fistr Fi[DIGITS] = { /* finger info */
/*    code lowest highest favorite */
	{ T, THIRD, FIFTH, FIFTH, }, /* thumb (numbers are -1) */
	{ I, SECOND, FOURTH, THIRD, }, /* index */
	{ M, FIRST, THIRD, FIRST, }, /* middle */
};

struct miscstr {
	int ap;
	int ns;
} Misc[MAXLEN];

void compose()
{
	register int t, i, f, s, d, pat;
	int try1, try2, try3, ti, n, p, ap1, ap2;
	int strings[MAXLEN][DIGITS], frets[MAXLEN][STRINGS], val;
	int bstrings[PATLEN][DIGITS], bfrets[PATLEN][STRINGS], bval;
	int fr[16], fret[STRINGS];
	int bpat, lastpat, fings, lhloc, blhloc, lastlhloc;
	struct miscstr bmisc[PATLEN];
	struct chrdstr *cp;
	FILE *tsave;

	if (Trace)
		fprintf(Trace, "Plen=%d\n", Plen);
	lastpat = 0;
	lastlhloc = Ilhloc;
	for (t = 0; t < Plen; t += PATLEN) { // Generates the piece, one pattern at a time.
		cp = &Cstr[Cpat[t]];
		if (Trace) { int *lp; // Print current chord's debug information.
			fprintf(Trace, "ctones: ");
			for (lp=cp->ctones; *lp > -1; fprintf(Trace, "%d ", *lp++));
			fprintf(Trace, "ptones: ");
			for (lp=cp->ptones; *lp > -1; fprintf(Trace, "%d ", *lp++));
			fprintf(Trace, "\n");
		}
		bval = -9999;
		for (try1 = 1; try1 <= NUMTRIES; try1++) {
			/* pick new left hand location */
			if (lastlhloc > rand() % 19)
				d = -3;
			else
				d = (rand() % 7) - 3;

			lhloc = lastlhloc + d;

			if (Trace)
				fprintf(Trace, "lastlhloc: %d, lhloc: %d, d: %d\n", lastlhloc, lhloc, d);
			/* If lhloc < 0, then set to "fret" of open string. Else if > 19, set to 19.*/
			lhloc = lhloc < OPEN? OPEN : (lhloc > 19? 19 : lhloc);
			/* pick frets on each string */
			for (s = STRINGS; --s >= FIRST; ) {
				n = 0;
				for (f = lhloc; f < lhloc + MAXREACH; f++) { // how far down can we reach from our current LH loc?
					i = f;
					if (s == FIFTH) { // fifth string has fewer frets.
						if (f <= 5)
							continue;
						i -= 5;
					}
					p = pitchof(s, i);
					/* frets that correspond to chord tones are roughly 3 times as likely to be chosen */
					if (ontlist(p, cp->ctones)) {
						fr[n++] = f;
						fr[n++] = f;
						fr[n++] = f;
					} else if (ontlist(p, cp->ptones))
						fr[n++] = f;
				}
				/* randomly choose a reachable fret for this string. */
				if (n > 0)
					fret[s] = fr[rand() % n];
				/* No valid reachable frets. */
				else
					fret[s] = OPEN;
			}
			/* pick 3 right-hand patterns */
			for (try2 = 3; --try2 >= 0; ) {
				pat = try2==0? lastpat : (rand() % Nrhpat);
				/* pick strings */
				for (;;) {
					for (i = 0; i < PATLEN; i++) {
						ti = t + i;
						fings = Rhpat[pat][i];
						if (fings == 0) {
							for (d = DIGITS; --d >= THUMB; )
								strings[ti][d] = -1;
							continue;
						}
						// Try 3 times to assign RH fingers to strings that do not overlap with previous RH assignment.
						for (try3 = 3; --try3 >= 0; ) {
							pickstring(fings, strings[ti]);
							if (ti == 0
							  || !overlap(strings[ti], strings[ti - 1]))
								break; /* no overlap */
						}
						if (try3 < 0) /* couldn’t find one */
							break;
					}
					// Successfully found RH assignments for all notes in the pattern.
					if (i >= PATLEN)
						break;
					// Otherwise, reroll all 8 assignments again.
				}
				/* find the best fretted/unfretted arrangement */
				for (i = 0; i < PATLEN; i++) {
					ti = t + i;
					ap1 = ap2 = n = 0;
					// Compute number of fingers allocated in this RH arrangement at time t and the avg pitch.
					for (d = DIGITS; --d >= THUMB; ) {
						s = strings[ti][d];
						if (s != -1) {
							ap1 += pitchof(s, fret[s]);
							ap2 += pitchof(s, OPEN);
							n++;
						}
					}
					if (n > 0) { // not a rest
						ap1 /= n; /* average pitch if fretted */
						ap2 /= n; /* average pitch if open */
						if (ap1 == ap2)
							f = 0;
						else {
/****/tsave=Trace;Trace=(FILE *)0; /* avoid ridiculous trace output */
							f = meval(ti, ap1, n); // These pitches are meaningless, so the trace output wouldn't make sense.
							f = (f > meval(ti, ap2, n)); // 0 if fretted better, 1 if open better.
/****/Trace=tsave;
						}

						// Choose frets based on whether the fretted or open arrangement was better.
						for (d = DIGITS; --d >= THUMB; )
							if ((s = strings[ti][d]) != -1)
								frets[ti][s] = f? fret[s] : OPEN;
						Misc[ti].ap = f? ap1 : ap2;
					} else
						Misc[ti].ap = 0; /* won't be used */
					Misc[ti].ns = n;
				}
				/* evaluate & save, if best */
				val = eval(t, strings, frets);
				if (Trace)
					fprintf(Trace,
					  "	t:%d-%d, lhloc:%d, pat:%d, val:%d\n",
					  t, t + PATLEN - 1, lhloc, pat, val);
				if (val > bval) { // update best pattern found.
					bval = val;
					for (i = PATLEN; --i >= 0; ) {
						ti = t + i;
						for (d = DIGITS; --d >= THUMB; )
							bstrings[i][d] = strings[ti][d];
						for (s = STRINGS; --s >= FIRST; )
							bfrets[i][s] = frets[ti][s];
						bmisc[i] = Misc[ti];
					}
					bpat = pat;
					blhloc = lhloc;
				}
			}

			// Was this forgotten?? (also forgot lastpat)
			// lastlhloc = lhloc;
		}
		if (Trace)
			fprintf(Trace, "	bestpat=%d (val=%d)\n", bpat, bval);
		n = 0;
		// Save and output best pattern.
		for (i = 0; i < PATLEN; i++) {
			ti = t + i;
			for (f = DIGITS; --f >= THUMB; )
				if ((strings[ti][f] = bstrings[i][f]) != -1)
					n++;
			for (s = STRINGS; --s >= FIRST; )
				frets[ti][s] = bfrets[i][s];
			output(ti, strings[ti], frets[ti]);
			Misc[ti].ap = bmisc[i].ap;
			Misc[ti].ns = bmisc[i].ns;
		}
		lastpat = bpat;
		lastlhloc = blhloc;

		if (Trace)
			fprintf(Trace, "\n");
	}
}

int eval(int t0, int strings[MAXLEN][DIGITS], int frets[MAXLEN][STRINGS]) /* return an evaluation of PATLEN events */
{
	register int val, t, d, s, p, i;
	int mv, nt, nic, noc, lastoc, harmv, movev, miscv;
	struct chrdstr *cp;
	int lf;

	mv = nt = miscv = lastoc = nic = noc = 0;
	for (i = 0; i < PATLEN; i++) {
		t = t0 + i;
		if (Trace)
			fprintf(Trace, " t=%d", t);
		lf=1;

		cp = &Cstr[Cpat[t]];
		for (d = DIGITS; --d >= THUMB; ) {
			if ((s = strings[t][d]) >= FIRST) {
				if (Fi[d].bstr == s) /* finger's favorite string */
					miscv += 2;
				p = pitchof(s, frets[t][s]);
				if (Trace)
					fprintf(Trace, " s=%d, d=%d, f=%d, p=%d (%s)\n",
					  s, d, frets[t][s], p, pitchname(p));
				lf=0;
				if (ontlist(p, cp->ctones)) {
					nic++;
					lastoc = 0;
				} else if (ontlist(p, cp->ptones)) { 
					noc += lastoc; // only penalize adjacent non-chord tones.
					lastoc = 1;
				} else {
					noc += 1 + lastoc; // always penalized even if a neighbor/passing tone.
					lastoc = 2;
				}
			}
		}
		if (Misc[t].ns > 0) {
			mv += meval(t, Misc[t].ap, Misc[t].ns);
			nt++;
		} else
			miscv -= 20; /* the cost of silence */
		if (Trace && lf) // newline for a sixteenth rest.
			fprintf(Trace, "\n");
		}

	harmv = 12 * nic - 16 * noc;
	if (noc > nic / 2)
		harmv -= 20;
	if (Trace)
		fprintf(Trace, " nic=%d noc=%d harmv=%d ", nic, noc, harmv);
	movev = (32 * mv) / nt; // (sum of mevals * 32/(number of mevals))
	if (Trace)
		fprintf(Trace, " mv=%d nt=%d movev=%d ", mv, nt, movev);
	val = harmv + movev + miscv;
	if (Trace)
		fprintf(Trace, " miscv=%d eval()=%d ", miscv, val);
	return(val);
}

int meval(int t, int ap, int ns) /* motion eval - how good is ap,ns at time t? */
{
	// value, previous number of strings, previous average pitch, difference.
	register int v, pns, pap, d; 

	v = 0;
	if (t >= 1) {
		pns = Misc[t - 1].ns;
		pap = Misc[t - 1].ap;
	} else {
		pns = 1;
		pap = ap;
	}
	// Note that if ns and pns = 1, then there is only one pitch in consideration, 
	// so the "average" pitch is just that pitch 
	if (pns == 1 && ns == 1) {
		d = abs(ap - pap);
		if (d == 0) // penalize not changing pitch.
			v -= 5;
		else if (d <= 2) // best: stepwise movement.
			v += 10;
		else if (d <= 12) // ok: w/in an octave
			v += 12 - d;
		else
			v += 24 - 2 * d; // Jumps larger than an octave are bad.
	}
	if (t >= 2) { // do the same for two steps back.
		pns = Misc[t - 2].ns;
		pap = Misc[t - 2].ap;
	} else {
		pns = 1;
		pap = ap;
	}
	if (pns == 1 && ns == 1) { 
		d = abs(ap - pap);
		if (d == 0) // less influence on v than one step back.
			v -= 4;
		else if (d <= 4)
			v += 5;
		else if (d <= 12)
			v += 9 - d; // more penalty here than one step back.
		else
			v += 22 - 2 * d; // more penalty here than one step back.
	// more strings incurs a higher penalty, presumably because the averaging actually takes affect, 
	// smoothing out the differences
	} else if (pns > 1 && ns > 1) { 
		d = abs(ap - Misc[t - 2].ap);
		if (d == 0)
			v -= 4;
		else if (d <= 5)
			v += 1;
		else if (d <= 12)
			v += 6 - d;
		else
			v += 18 - 2 * d;
	}
	return(v);
}

void pickstring(int fpat, int strngs[DIGITS]) /* select string(s) for finger(s) in fpat */
{
	struct fistr *fip;
	
	strngs[THUMB] = strngs[INDEX] = strngs[MIDDLE] = -1;
	// fpat can either be 0, T=1, I=2, M=4 or T|M=5.
	// in binary: 0000, 0001, 0010, 0100, or 0101.
	// fip->mask is either T=1, I=2, or M=4, depending on what it points to. 
	// Thus, (fpat & fip->mask) is a clever way of testing equality.
	for (;;) {
		fip = &Fi[THUMB];
		if (fpat & fip->mask) // fpat == T or fpat == T|M
			strngs[THUMB] = randr(fip->lostr, fip->histr);
		fip = &Fi[INDEX];
		if (fpat & fip->mask) // fpat == I
			strngs[INDEX] = randr(fip->lostr, fip->histr);
		fip = &Fi[MIDDLE];
		if (fpat & fip->mask) // fpat == M or fpat == T|M
			strngs[MIDDLE] = randr(fip->lostr, fip->histr);
		if (!overlap(strngs, strngs)) // needed for the T|M case.
			break;
	}
}

