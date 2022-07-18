/*
**	gate(ifp, ofp, sflg, thresh, tix) -- gate/squelch	psl 10/89
**	FILE	*ifp, *ofp;	where the data comes from and goes to
**	int	sflg;		non-zero => delete silences, else zero them
**	int	thresh;		u-law threshold for silence
**	int	tix;		attack/release time in 1/8000 second units
*/
#include	"audio.h"

gate(ifp, ofp, sflg, thresh, tix)
FILE	*ifp, *ofp;
int	sflg, thresh, tix;
{
	char *buf;
	int i, in, bc, state, skip, silences;

	if ((i =  sbrk(tix)) == -1) {
	    perror("sbrk()");
	    exit(1);
	}
	buf = (char *) i;
	sflg = sflg? 0 : Sb2i[0];
	bc = skip = state = 0;
	silences = tix;
	while ((in = getc(stdin)) != EOF) {
	    if (state) {				/* not squelched */
		if ((in & 0x7F) <= thresh)
		    silences = 0;
		else if (++silences >= tix)
		    state = skip = bc = silences = 0;	/* too many silences */
		putc(in, stdout);
	    } else {					/* squelched */
		if ((in & 0x7F) <= thresh) {		/* got a loud one */
		    i = (skip < tix)? skip : tix;
		    bc = (i > bc)? (bc + tix - i) : (bc - i);
		    while (--i >= 0) {
			putc(buf[bc++], stdout);
			if (bc >= tix)
			    bc = 0;
		    }
		    putc(in, stdout);
		    state = 1;
		} else {				/* still quiet */
		    buf[bc++] = in;
		    if (bc >= tix)
			bc = 0;
		    skip++;
		    if (sflg && skip > tix)
			putc(sflg, stdout);
		}
	    }
	}
	exit(0);
}

