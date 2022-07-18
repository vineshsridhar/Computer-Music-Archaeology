/*	BSSETUP - read/write style & synth setup information
**	psl 2/90
**	bssetload(fp, stylep, synthp, nums)
**	bssetsave(fp, stylep, synthp, header)
*/
#define	BS
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include	<midi.h>
#include	<stdio.h>
#include "strcopy.h"

# define SVNPAT \
	"\t`%[^`]` `%[^`]` `%[^`]` `%[^`]` `%[^`]` `%[^`]` `%[^`]` `%[^`]`\n"

bssetload(ifp, styp, synp, nums)
FILE	*ifp;		/* input file pointer */
STYLE	*styp;		/* pointer to STYLE vector */
SYNTH	*synp;		/* pointer to SYNTH struct */
int	nums;		/* max number of styles */
{
	STYLE st, *sp;
	char buf[600], kwd[64], temp[512];
	int i, n, c, v;
	extern STYLE *bsstyle();

	if (synp)
	    for (c = NUMC; --c >= 0; )
		if (synp->kmap[c])
		    free(synp->kmap[c]);
	while (fgets(buf, sizeof buf, ifp)) {
	    if (buf[0] != '#' || buf[1] <= ' ')
		continue;
	    for (c = 0; (kwd[c] = buf[c]) > ' '; c++);
	    kwd[c] = '\0';
	    if (strcmp(kwd, "#SYNTH") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SYNTH `%[^`]`\n", temp) != 1) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		temp[sizeof synp->name  - 1] = '\0';
		strcopy(synp->name, temp);
		for (c = NUMC; --c >= 0; )
		    for (v = NUMV; --v >= 0; synp->vnam[c][v][0] = '\0');
		synp->misc[0] = synp->init[0] = synp->fini[0] = '\0';
		strcopy(synp->play, "play");
	    } else if (strcmp(kwd, "#SMISC") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SMISC `%[^`]`\n", temp) != 1) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		temp[sizeof synp->misc  - 1] = '\0';
		strcopy(synp->misc, temp);
	    } else if (strcmp(kwd, "#SINIT") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SINIT `%[^`]`\n", temp) != 1) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		temp[sizeof synp->init  - 1] = '\0';
		strcopy(synp->init, temp);
	    } else if (strcmp(kwd, "#SFINI") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SFINI `%[^`]`\n", temp) != 1) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		temp[sizeof synp->fini  - 1] = '\0';
		strcopy(synp->fini, temp);
	    } else if (strcmp(kwd, "#SPLAY") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SPLAY `%[^`]`\n", temp) != 1) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		temp[sizeof synp->play  - 1] = '\0';
		strcopy(synp->play, temp);
	    } else if (strcmp(kwd, "#SCLICK") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SCLICK\tchan=%d key=%d\n",
		 &synp->mchn, &synp->mkey) != 2)
		    fprintf(stderr, "Error in %s", buf);
	    } else if (strcmp(kwd, "#SKMAP") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SKMAP chan=%d `%[^`]`\n", &c, temp) != 2
		 || (--c) < 0 || c >= NUMC) {
		    fprintf(stderr, "Channel number error in %s", buf);
		    continue;
		}
		temp[KMLEN - 1] = '\0';
		synp->kmap[c] = (char *) malloc(strlen(temp));
		strcopy(synp->kmap[c], temp);
	    } else if (strcmp(kwd, "#SVNAM") == 0) {
		if (!synp)
		    continue;
		if (sscanf(buf, "#SVNAM chan=%d %s %d `%[^`]`\n",
		 &c, kwd, &n, temp) != 4
		 || (--c) < 0 || c >= NUMC) {
		    fprintf(stderr, "Channel number error in %s", buf);
		    continue;
		}
		temp[NLEN - 1] = '\0';
		strcopy(synp->cnam[c], temp);
		if (strcmp(kwd, "like") == 0) {
		    if (n < 1 || n > NUMC)
			fprintf(stderr, "Chan %d: Error in %s", c, buf);
		    else
			synp->vnam[c][0][0] = USEC(n);
		    continue;
		} else if (strcmp(kwd, "list") == 0) {
		    if (n > NUMV) {
			fprintf(stderr, "Chan %d: too many voices: %s", c, buf);
			continue;
		    }
		    for (v = 0; v < n && fgets(buf, sizeof buf, ifp); v += i) {
			i = sscanf(buf, SVNPAT,
			 synp->vnam[c][v + 0], synp->vnam[c][v + 1],
			 synp->vnam[c][v + 2], synp->vnam[c][v + 3],
			 synp->vnam[c][v + 4], synp->vnam[c][v + 5],
			 synp->vnam[c][v + 6], synp->vnam[c][v + 7]);
			if (i < 1) {
			    fprintf(stderr, "Chan %d: Error in %s", c, buf);
			    break;
			}
		    }
		    if (v < NUMV)
			synp->vnam[c][v][0] = '\0';
		}
	    } else if (strcmp(kwd, "#STYLE") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#STYLE %s `%[^`]` %d %d, %d,%d,%d,%d `%[^`]`\n",
		 st.name, st.lname, &st.barlen, &st.units,
		 &st.dpchn[0], &st.dpchn[1], &st.dpchn[2], &st.dpchn[3],
		 st.desc) != 9) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, nums))) {
		    fprintf(stderr, "Too many styles at \"%s\".\n", st.name);
		    continue;
		}
		strcopy(sp->name, st.name);
		strcopy(sp->lname, st.lname);
		sp->barlen = st.barlen;
		sp->units = st.units;
		sp->dpchn[0] = st.dpchn[0];
		sp->dpchn[1] = st.dpchn[1];
		sp->dpchn[2] = st.dpchn[2];
		sp->dpchn[3] = st.dpchn[3];
		strcopy(sp->desc, st.desc);
	    } else if (strcmp(kwd, "#PARMS") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#PARMS %s key=%d cnti=%d len=`%[^`]` MM=%d ener=%d pred=%d seed=`%1s`\n",
		 st.name, &st.key, &st.cnti, st.leng, &st.tempo,
		 &st.lener, &st.lpred, st.lseed) != 8) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, 0))) {
		    fprintf(stderr, "Unknown style: %s.\n", st.name);
		    continue;
		}
		sp->key = st.key;
		sp->cnti = st.cnti;
		strcopy(sp->leng, st.leng);
		sp->tempo = st.tempo;
		sp->lener = st.lener;
		sp->lpred = st.lpred;
		sp->lsfix = atoi(st.lseed)? 1 : 0;
		strcopy(sp->lseed, st.lseed);
	    } else if (strcmp(kwd, "#LEAD") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#LEAD %s %d,%d,%d,%d,%d,%d,%d,%d %d,%d,%d,%d,%d,%d,%d,%d %d,%d,%d,%d,%d,%d,%d,%d\n",
		 st.name,
		 &st.linst[0], &st.lchan[0], &st.lvelo[0], &st.ltrns[0],
		 &st.lmctl[0], &st.lbctl[0], &st.lfctl[0], &st.lvctl[0],
		 &st.linst[1], &st.lchan[1], &st.lvelo[1], &st.ltrns[1],
		 &st.lmctl[1], &st.lbctl[1], &st.lfctl[1], &st.lvctl[1],
		 &st.linst[2], &st.lchan[2], &st.lvelo[2], &st.ltrns[2],
		 &st.lmctl[2], &st.lbctl[2], &st.lfctl[2], &st.lvctl[2]
		 ) != 25) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, 0))) {
		    fprintf(stderr, "Unknown style: %s.\n", st.name);
		    continue;
		}
		for (v = 3; --v >= 0; ) {
		    sp->linst[v] = st.linst[v];
		    sp->lchan[v] = st.lchan[v];
		    sp->lvelo[v] = st.lvelo[v];
		    sp->ltrns[v] = st.ltrns[v];
		    sp->lmctl[v] = st.lmctl[v];
		    sp->lbctl[v] = st.lbctl[v];
		    sp->lfctl[v] = st.lfctl[v];
		    sp->lvctl[v] = st.lvctl[v];
		}
	    } else if (strcmp(kwd, "#BASS") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#BASS %s inst=%d chan=%d vel=%d oct=%d ctl=%d,%d,%d,%d\n",
		 st.name,
		 &st.binst, &st.bchan, &st.bvelo, &st.btrns,
		 &st.bmctl, &st.bbctl, &st.bfctl, &st.bvctl) != 9) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, 0))) {
		    fprintf(stderr, "Unknown style: %s.\n", st.name);
		    continue;
		}
		sp->binst = st.binst;
		sp->bchan = st.bchan;
		sp->bvelo = st.bvelo;
		sp->btrns = st.btrns;
		sp->bmctl = st.bmctl;
		sp->bbctl = st.bbctl;
		sp->bfctl = st.bfctl;
		sp->bvctl = st.bvctl;
	    } else if (strcmp(kwd, "#CHORD") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#CHORD %s inst=%d chan=%d vel=%d oct=%d ctl=%d,%d,%d,%d\n",
		 st.name,
		 &st.cinst, &st.cchan, &st.cvelo, &st.ctrns,
		 &st.cmctl, &st.cbctl, &st.cfctl, &st.cvctl) != 9) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, 0))) {
		    fprintf(stderr, "Unknown style: %s.\n", st.name);
		    continue;
		}
		sp->cinst = st.cinst;
		sp->cchan = st.cchan;
		sp->cvelo = st.cvelo;
		sp->ctrns = st.ctrns;
		sp->cmctl = st.cmctl;
		sp->cbctl = st.cbctl;
		sp->cfctl = st.cfctl;
		sp->cvctl = st.cvctl;
	    } else if (strcmp(kwd, "#DRUM") == 0) {
		if (!styp)
		    continue;
		if (sscanf(buf,
		 "#DRUM %s inst=%d chan=%d vel=%d part=%d ctl=%d,%d,%d,%d\n",
		 st.name,
		 &st.dinst, &st.dchan, &st.dvelo, &st.dpart,
		 &st.dmctl, &st.dbctl, &st.dfctl, &st.dvctl) != 9) {
		    fprintf(stderr, "Error in %s", buf);
		    continue;
		}
		if (!(sp = bsstyle(st.name, styp, 0))) {
		    fprintf(stderr, "Unknown style: %s.\n", st.name);
		    continue;
		}
		sp->dinst = st.dinst;
		sp->dchan = st.dchan;
		sp->dvelo = st.dvelo;
		sp->dpart = st.dpart;
		sp->dmctl = st.dmctl;
		sp->dbctl = st.dbctl;
		sp->dfctl = st.dfctl;
		sp->dvctl = st.dvctl;
	    } else
		fprintf(stderr, "Unrecognized keyword in: %s", buf);
	}
	if (synp) {
	    for (c = NUMC; --c >= 0; ) { /* do special codes for channels */
		v = synp->vnam[c][0][0];
		if (!v)
		    for (v = NUMV; --v >= 0; synp->vnam[c][v][0] = '\0');
		else if (v & 0x80) {
		    n = v & 0x0F;
		    for (v = NUMV; --v >= 0; )
			strcopy(synp->vnam[c][v], synp->vnam[n][v]);
		}
	    }
	}
}

bssetsave(ofp, styp, synp, header)
FILE	*ofp;		/* output file pointer */
STYLE	*styp;		/* pointer to STYLE vector */
SYNTH	*synp;		/* pointer to SYNTH struct */
char	*header;	/* header text */
{
	register int c, v, i;
	register STYLE *sp;
	long now;

	time(&now);
	fprintf(ofp, "# %s - created: %s", header, ctime(&now));
	fprintf(ofp, "#\n");
	if (synp) {
	    fprintf(ofp, "# Synth-specific information\n");
	    fprintf(ofp, "#SYNTH\t`%1s`\n", synp->name);
	    fprintf(ofp, "#SMISC\t`%1s`\n", synp->misc);
	    fprintf(ofp, "#SINIT\t`%1s`\n", synp->init);
	    fprintf(ofp, "#SFINI\t`%1s`\n", synp->fini);
	    fprintf(ofp, "#SPLAY\t`%1s`\n", synp->play);
	    fprintf(ofp, "#SCLICK\tchan=%d key=%d\n", synp->mchn, synp->mkey);
	    for (c = 0; c < NUMC; c++) {
		if (synp->kmap[c] && synp->kmap[c][0])
		    fprintf(ofp, "#SKMAP\tchan=%d `%1s`\n", c+1, synp->kmap[c]);
		if (synp->vnam[c][0][0] == '\0') {	/* no voices for c */
		    fprintf(ofp, "#SVNAM\tchan=%d list 0\t`%1s`\n",
		     c + 1, synp->cnam[c]);
		    continue;
		}
		for (i = 0; i < c; i++) {
		    for (v = NUMV; --v >= 0; )
			if (strcmp(synp->vnam[c][v], synp->vnam[i][v]))
			    break;
		    if (v < 0)
			break;
		}
		if (i < c) {			/* c identical to i */
		    fprintf(ofp, "#SVNAM\tchan=%d like %d\t`%1s`\n",
		     c + 1, i + 1, synp->cnam[c]);
		    continue;
		}
		for (i = NUMV; --i > 1 && synp->vnam[c][i][0] <= ' '; );
/****		i |= 3;	/**** not needed any more? ****/
		fprintf(ofp, "#SVNAM\tchan=%d list %d\t`%1s`\n",
		 c + 1, i + 1, synp->cnam[c]);
		for (v = 0; v <= i; v += 4)
		    fprintf(ofp, "\t`%1s` `%1s` `%1s` `%1s`\n",
		     synp->vnam[c][v + 0], synp->vnam[c][v + 1],
		     synp->vnam[c][v + 2], synp->vnam[c][v + 3]);
	    }
	    fprintf(ofp, "#\n");
	}
	if (styp) {
	    fprintf(ofp, "# Style-specific information\n");
	    for (sp = styp; sp->name && *sp->name; sp++) {
		fprintf(ofp, "#STYLE\t%1s\t`%1s` %d %d, %d,%d,%d,%d `%1s`\n",
		 sp->name,
		 sp->lname, sp->barlen, sp->units,
		 sp->dpchn[0], sp->dpchn[1], sp->dpchn[2], sp->dpchn[3],
		 sp->desc);
		fprintf(ofp,
		 "#PARMS\t%1s\tkey=%d cnti=%d len=`%1s` MM=%d",
		 sp->name,
		 sp->key, sp->cnti, sp->leng, sp->tempo);
		fprintf(ofp, " ener=%d pred=%d seed=`%1s`\n",
		 sp->lener, sp->lpred, sp->lsfix? sp->lseed : "0");
		fprintf(ofp, "#LEAD\t%1s\t%d,%d,%d,%d,%d,%d,%d,%d",
		 sp->name,
		 sp->linst[0], sp->lchan[0], sp->lvelo[0], sp->ltrns[0],
		 sp->lmctl[0], sp->lbctl[0], sp->lfctl[0], sp->lvctl[0]);
		fprintf(ofp, " %d,%d,%d,%d,%d,%d,%d,%d %d,%d,%d,%d,%d,%d,%d,%d",
		 sp->linst[1], sp->lchan[1], sp->lvelo[1], sp->ltrns[1],
		 sp->lmctl[1], sp->lbctl[1], sp->lfctl[1], sp->lvctl[1],
		 sp->linst[2], sp->lchan[2], sp->lvelo[2], sp->ltrns[2],
		 sp->lmctl[2], sp->lbctl[2], sp->lfctl[2], sp->lvctl[2]);
		fprintf(ofp, "\n");
		fprintf(ofp,
		 "#BASS\t%s\tinst=%d chan=%d vel=%d oct=%d ctl=%d,%d,%d,%d\n",
		 sp->name,
		 sp->binst, sp->bchan, sp->bvelo, sp->btrns,
		 sp->bmctl, sp->bbctl, sp->bfctl, sp->bvctl);
		fprintf(ofp,
		 "#CHORD\t%s\tinst=%d chan=%d vel=%d oct=%d ctl=%d,%d,%d,%d\n",
		 sp->name,
		 sp->cinst, sp->cchan, sp->cvelo, sp->ctrns,
		 sp->cmctl, sp->cbctl, sp->cfctl, sp->cvctl);
		fprintf(ofp,
		 "#DRUM\t%s\tinst=%d chan=%d vel=%d part=%d ctl=%d,%d,%d,%d\n",
		 sp->name,
		 sp->dinst, sp->dchan, sp->dvelo, sp->dpart,
		 sp->dmctl, sp->dbctl, sp->dfctl, sp->dvctl);
	    }
	}
	fclose(ofp);
}
