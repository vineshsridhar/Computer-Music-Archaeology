/*
**	MPU2SMF -- Convert MPU files into Standard MIDI Files
**	psl 5/89
*/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <midi.h>
#include "putsmcmd.h"
#include "sopen.h"

void cvt(char *file, FILE *ifp, FILE *ofp, char *tmpfile);


int	Reso	= 120;		/* default, ticks/quarter (MPU clocks) */
int	Tempo	= 100;		/* default, mm = 100 */
int	Vflg	= 0;		/* verbose output */

syntax(prog)
char	*prog;
{
	fprintf(stderr, "Usage: %s [options] <mpu-file >smf-file\n", prog);
	fprintf(stderr, "   or: %s [options] mpu-file ... >smf-file\n", prog);
	fprintf(stderr, "-r120 gets 120 ticks per quarter note\n");
	fprintf(stderr, "-t100 gets 100 quarter notes per minute\n");
	fprintf(stderr, "-v gets verbose output\n");
	exit(2);
}

main(argc, argv)
char *argv[];
{
	char tmpfile[64];
	int i, n;
	FILE *f;

	for (i = 1; i < argc; i++) {
	    if (argv[i][0] == '-') {
		switch (argv[i][1]) {
		case 'r':
		    Reso = atoi(&argv[i][2]);
		    break;
		case 't':
		    Tempo = atoi(&argv[i][2]);
		    break;
		case 'v':
		    Vflg++;
		    break;
		default:
		    syntax(argv[0]);
		}
	    }
	}
	sprintf(tmpfile, "/tmp/mpu2smf%d", getpid());
	n = 0;
	for (i = 1; i < argc; i++) {
	    if (argv[i][0] != '-') {
		if ((f = sopen(argv[i], "r")) != NULL) {
		    cvt(argv[i], f, stdout, tmpfile);
		    sclose(f);
		    n++;
		} else
		    perror(argv[i]);
	    }
	}
	if (n == 0)
	    cvt("stdin", stdin, stdout, tmpfile);
	exit(0);
}

void cvt(file, ifp, ofp, tmpfile)
char	*file, *tmpfile;
FILE	*ifp, *ofp;
{
	u_char buf[128], *bp;
	int dmspq, i;
	long now;
	double stpm, dtpm;
	MCMD *mp, m;
	SMF_HD hdr;
	FILE *tfp;

	stpm = 100 * 120;		/* src ticks/min=tempo*ticks/quarter */
	dmspq = (60 * 1000000) / Tempo;	/* dst usec/quarter (mm = Tempo) */
	dtpm = (Reso * 60000000.) / dmspq;	/* dst ticks/min */
	if (Vflg) {
	    fprintf(stderr, "stpm (src ticks/min) = %g\n", stpm);
	    fprintf(stderr, "dmspq (dst usec/quarter) = %d\n", dmspq);
	    fprintf(stderr, "dtpm (dst ticks/min) = %g\n", dtpm);
	}
	if (!(tfp = fopen(tmpfile, "w"))) {	/* create temp file */
	    perror(tmpfile);
	    exit(1);
	}
	bp = buf;
	*bp++ = 0;
	*bp++ = SMF_META;			/* create tempo meta event */
	*bp++ = SMF_M_TEMPO;
	*bp++ = 3;
	*bp++ = ((dmspq >> 16) & 0xFF);
	*bp++ = ((dmspq >> 8) & 0xFF);
	*bp++ = (dmspq & 0xFF);
	fwrite(buf, 1, bp - buf, tfp);	/* write tempo at start of temp file */
	if (Vflg)
	    fprintf(stderr, "Wrote %d-byte tempo meta-cmd\n", bp - buf);
	putsmcmd(0, 0);
	for (now = 0L; mp = getmcmd(ifp, now); ) {
	    now = mp->when;
	    if (Vflg > 1)
		fprintf(stderr, "when=%d len=%d cmd[0]=%02x\n",
		 mp->when, mp->len, mp->cmd[0]);
	    if (mp->cmd[0] != MPU_NO_OP && mp->cmd[0] != MPU_TCWME) {
		mp->when = (now * dtpm) / stpm;
		putsmcmd(tfp, mp);		/* write MIDI events */
		if (Vflg > 1)
		    fprintf(stderr, "wrote cmd with when=%d\n", mp->when);
	    }
	}
	m.when = now;			/* add an end of track meta event */
	m.cmd = bp = buf;
	*bp++ = SMF_META;
	*bp++ = SMF_M_EOT;
	*bp++ = 0;
	m.len = bp - buf;
	putsmcmd(tfp, &m);
	if (Vflg)
	    fprintf(stderr, "Wrote %d-byte EOT meta-cmd with when=%d\n",
	     bp - buf, m.when);
	tfp = freopen(tmpfile, "r", tfp);	/* close & reopen temp file */
	L2B(hdr.chdr.type, SMF_HDR);		/* write header chunk */
	L2B(hdr.chdr.leng, 6);
	S2B(hdr.format, SMF_FMT_0);
	S2B(hdr.ntrks, 1);
	S2B(hdr.divis, SMF_DIV_TPQ(Reso));
	if (fwrite(&hdr, SMF_HD_SIZ, 1, ofp) != 1) {
	    fprintf(stderr, "Error writing %s header chunk", file);
	    perror("");
	    return;
	}
	if (Vflg)
	    fprintf(stderr, "Wrote %d byte header chunk\n", SMF_HD_SIZ);
	i = chunkify(tfp, ofp);			/* copy track chunk */
	if (Vflg)
	    fprintf(stderr, "Wrote %d bytes of data\n", i);
	unlink(tmpfile);
}
