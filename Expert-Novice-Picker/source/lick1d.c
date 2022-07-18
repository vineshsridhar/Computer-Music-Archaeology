/*
** LICK -- Generate "random" banjo parts
** lick1d module: a "smart" part for "melodic" playing
** psl 10/87
*/
#include "lick.h"

#undef d
#define NUMTRIES 12 /* how many to choose from */
#define TOPFRET 12

#define D4 62 /* default average pitch for eval() */

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

compose()
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
		fprintf(Trace, "Plen=%d0, Plen);
	lastpat = 0;
	lastlhloc = Ilhloc;
	for (t = 0; t < Plen; t += PATLEN) {
		cp = &Cstr[Cpat[t]];
		if (Trace) { int *lp;
			fprintf(Trace, "ctones: ");
			for (lp=cp->ctones; *lp > -1; fprintf(Trace, "%d ", *lp++));
			fprintf(Trace, "0tones: ");
			for (lp=cp->ptones; *lp > -1; fprintf(Trace, "%d ", *lp++));
			fprintf(Trace, "0);
		}
		bval = -9999;
		for (try1 = 1; try1 <= NUMTRIES; try1++) {
			/* pick new left hand location */
			if (lastlhloc > rand() % 19)
				d = -3;
			else
				d = (rand() % 7) - 3;

			lhloc = lastlhloc + d;
			lhloc = lhloc < OPEN? OPEN : (lhloc > 19? 19 : lhloc);
			/* pick frets on each string */
			for (s = STRINGS; --s >= FIRST; ) {
				n = 0;
				for (f = lhloc; f < lhloc + MAXREACH; f++) {
					i = f;
					if (s == FIFTH) {
						if (f <= 5)
							continue;
						i -= 5;
					}
					p = pitchof(s, i);
					if (ontlist(p, cp->ctones)) {
						fr[n++] = f;
						fr[n++] = f;
						fr[n++] = f;
					} else if (ontlist(p, cp->ptones))
						fr[n++] = f;
				}
				if (n > 0)
					fret[s] = fr[rand() % n];
				else
					fret[s] = OPEN;
			}
			/* pick some right-hand patterns */
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
						for (try3 = 3; --try3 >= 0; ) {
							pickstring(fings, strings[ti]);
							if (ti == 0
							  || !overlap(strings[ti], strings[ti - 1]))
								break; /* no overlap */
						}
						if (try3 < 0) /* couldn’t find one */
							break;
					}
					if (i >= PATLEN)
						break;
				}
				/* find the best fretted/unfretted arrangement */
				for (i = 0; i < PATLEN; i++) {
					ti = t + i;
					ap1 = ap2 = n = 0;
					for (d = DIGITS; --d >= THUMB; ) {
						s = strings[ti][d];
						if (s != -1) {
							ap1 += pitchof(s, fret[s]);
							ap2 += pitchof(s, OPEN);
							n++;
						}
					}
					if (n > 0) {
						ap1 /= n; /* average pitch if fretted */
						ap2 /= n; /* average pitch if open */
						if (ap1 == ap2)
							f = 0;
						else {
/****/tsave=Trace;Trace=(FILE *)0; /* avoid ridiculous trace output */
							f = meval(ti, ap1, n);
							f = (f > meval(ti, ap2, n));
/****/Trace=tsave;
						}
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
					  "	t:%d-%d, lhloc:%d, pat:%d, val:%d0,
					  t, t + PATLEN - 1, lhloc, pat, val);
				if (val > bval) {
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
		}
		if (Trace)
			fprintf(Trace, "	bestpat=%d (val=%d)0, bpat, bval);
		n = 0;
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
			fprintf(Trace, "0);
	}
}

eval(t0, strings, frets) /* return an evaluation of PATLEN events */
int strings[MAXLEN][DIGITS], frets[MAXLEN][STRINGS];
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
					fprintf(Trace, " s=%d, d=%d, f=%d, p=%d (%s)0,
					  s, d, frets[t][s], p, pitchname(p));
				lf=0;
				if (ontlist(p, cp->ctones)) {
					nic++;
					lastoc = 0;
				} else if (ontlist(p, cp->ptones)) { 
					noc += lastoc;
					lastoc = 1;
				} else {
					noc += 1 + lastoc; 
					lastoc = 2;
				}
			}
		}
		if (Misc[t].ns > 0) {
			mv += meval(t, Misc[t].ap, Misc[t].ns);
			nt++;
		} else
			miscv -= 20; /* the cost of silence */
		if (Trace && lf)
			fprintf(Trace, "0);
	}
	harmv = 12 * nic - 16 * noc;
	if (noc > nic / 2)
		harmv -= 20;
	if (Trace)
		fprintf(Trace, " nic=%d noc=%d harmv=%d ", nic, noc, harmv);
	movev = (32 * mv) / nt;
	if (Trace)
		fprintf(Trace, " mv=%d nt=%d movev=%d ", mv, nt, movev);
	val = harmv + movev + miscv;
	if (Trace)
		fprintf(Trace, " miscv=%d eval()=%d0, miscv, val);
	return(val);
}

meval(t, ap, ns) /* motion eval - how good is ap,ns at time t? */
{
	register int v, pns, pap, d; 

	v = 0;
	if (t >= 1) {
		pns = Misc[t - 1].ns;
		pap = Misc[t - 1].ap;
	} else {
		pns = 1;
		pap = ap;
	}
	if (pns == 1 && ns == 1) {
		d = abs(ap - pap);
		if (d == 0)
			v -= 5;
		else if (d <= 2)
			v += 10;
		else if (d <= 12)
			v += 12 - d;
		else
			v += 24 - 2 * d;
	}
	if (t >= 2) {
		pns = Misc[t - 2].ns;
		pap = Misc[t - 2].ap;
	} else {
		pns = 1;
		pap = ap;
	}
	if (pns == 1 && ns == 1) { 
		d = abs(ap - pap);
		if (d == 0)
			v -= 4;
		else if (d <= 4)
			v += 5;
		else if (d <= 12)
			v += 9 - d;
		else
			v += 22 - 2 * d; 
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

pickstring(fpat, strngs) /* select string(s) for finger(s) in fpat */
int strngs[DIGITS];
{
	struct fistr *fip;
	
	strngs[THUMB] = strngs[INDEX] = strngs[MIDDLE] = -1;
	for (;;) {
		fip = &Fi[THUMB];
		if (fpat & fip->mask)
			strngs[THUMB] = randr(fip->lostr, fip->histr);
		fip = &Fi[INDEX];
		if (fpat & fip->mask)
			strngs[INDEX] = randr(fip->lostr, fip->histr);
		fip = &Fi[MIDDLE];
		if (fpat & fip->mask)
			strngs[MIDDLE] = randr(fip->lostr, fip->histr);
		if (!overlap(strngs, strngs))
			break;
	}
}

