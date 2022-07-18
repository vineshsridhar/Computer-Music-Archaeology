/*
** LICK -- Generate "random" banjo parts
** psl 7/87
** lick2 module: getchords()
*/
#include "lick.h"

int Cpat[MAXLEN]; /* chord indices, one per sixteenth note */
int Nchrds = 0; /* how many chords have been defined */
int Plen = 0; /* how many sixteenths in the piece */

struct chrdstr Cstr[MAXCHRD];

char *peel();

#define NSCALE 13

getchords(file)
char *file;
{
	register char *cp;
	register int i, j, k, pat;
	char buf[128], *bp, cname[32];

	int ctone[NTONES], scale[NSCALE], spc, beats, mult;
	FILE *cfp;

	if (!(cfp = fopen(file, "r"))) {
		perror(file);
		return(0);
	}
	Plen = 0; 
	beats = 4; 
	mult = 1;
	spc = (mult * 16) / beats;
	pat = scale[0] = ctone[0] = -1;
	while (fgets(buf, sizeof buf, cfp)) {
		if (*buf == '#') {
			bp = buf;
			cp = peel(&bp);
			if (strcmp(cp, "#BEATS") == 0) {
				beats = atoi(peel(&bp));
				spc = (mult * 16) / beats;
			} else if (strcmp(cp, "#MULT") == 0) {
				mult = atoi(peel(&bp));
				spc = (mult * 16) / beats;
			} else if (strcmp(cp, "#SCALE") == 0) {
				cnvlist(peel(&bp), scale, NSCALE);
			} else if (strcmp(cp, "#CHORDTONES") == 0) {
				strcpy(cname, peel(&bp));
				cname[NAMELEN] = ' ';
				cnvlist(peel(&bp), ctone, NTONES);
				pat = -1;
				for (i = 0; i < Nchrds; i++) {
					for (j = 0; Cstr[i].ctones[j] >= 0; j++)
						if (Cstr[i].ctones[j] != ctone[j])
							break;
					if (Cstr[i].ctones[j] == -1)
						break;
				}
				pat = i;
				if (pat == Nchrds) {
					Nchrds++;
					strcpy(Cstr[pat].name, cname);
					for (j = NTONES; --j >= 0; )	
						Cstr[pat].ctones[j] = ctone[j];
					k = 0;
					for (i = 0; i < NSCALE; i++) {
						for (j = 0; ctone[j] >= 0; j++)
							if (ctone[j] == scale[i])
								break;
						if (ctone[j] == -1 && k < NTONES - 1)
							Cstr[pat].ptones[k++] = scale[i];
					}
					Cstr[pat].ptones[k] = -1;
				}
			}
		} else {
						
			if (pat < 0) {
				fprintf(stderr, "No CHORDTONES line preceding data");
				exit(1);
			}
			if (spc < 1)
				fprintf(stderr, "#BEATS %d, #MULT %d is too fast.",
				  beats, mult);
			if (beats * spc != mult * 16)
				fprintf(stderr, "#BEATS %d, #MULT %d truncates.",
				  beats, mult);
			for (i = 0; i < spc; i++)
				Cpat[Plen++] = pat;
		}
	}
	return(Plen);
}

char *
peel(spp)
char **spp;
{
	register char *bp, *sp;

	bp = sp = *spp;
	while (*bp > ' ')
		bp++;
	if (*bp != ' ') {
		*bp++ = '\0';
		while (*bp && *bp <= ' ')
			bp++;
	}

	*spp = bp;
	return(sp);
}

cnvlist(str, list, len)
char *str;
int *list;
{
	register char *sp;
	register int i;
	
	for (i = 0; i < len - 1; ) {
		for (sp = str; *str && *str != ','; str++);
		list[i++] = atoi(sp) % 12;
		if (*str++ != ',')
			break;
	}
	list[i] = -1;
}
