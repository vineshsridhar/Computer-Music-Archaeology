/*
**	MPUCLEAN -- Clean up mpu data files:
**	1) remove multiple mode commands
**	2) remove multiple note-ons (unless -m used)
**	3) remove unneeded note-offs
**	4) remove pressure change info (unless -p used)
**	5) remove mpu no-ops
**	6) remove Timing Clock In Stop messages (unless -tcis used)
**	7) insert TCWME commands every specified number of clocks (if -b used)
**	8) remove multiple TCWME commands (unless -tcwme used)
**	9) generate note-off commands for any notes left on
**	   & assure that a TCWME code is the last datum
**	psl 9/85
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>

#define	MAXCHAN		16
#define	DEFBARLEN	2*MPU_CLOCK_PERIOD
#define	USE_EXTRA	1
#define	ONLY_LAST	2

int putdt(int dt);


int	key[MAXCHAN][128];

main(argc, argv)
char	*argv[];
{
	register int i, k, v, dt, curmode, lastmode;
	int chan, mflg, pflg, tcisflg, tcwmeflg, barlen, status;
	long now, lasttcwme, nexttcwme;

	pflg = tcisflg = tcwmeflg = barlen = 0;
	mflg = USE_EXTRA | ONLY_LAST;
	while (--argc > 0) {
	    if (argv[argc][0] == '-') {
		switch (argv[argc][1]) {
		case 'L':
		    barlen = atoi(&argv[argc][2]);
		    if (barlen == 0)
			barlen = DEFBARLEN;
		    break;
		case 'M':
		    mflg ^= USE_EXTRA;
		    break;
		case 'm':
		    mflg ^= ONLY_LAST;
		    break;
		case 'p':
		    pflg++;
		    break;
		case 't':
		    if (argv[argc][2] != 'c')
			goto syntax;
		    if (argv[argc][3] == 'i')
			tcisflg++;
		    else if (argv[argc][3] == 'w')
			tcwmeflg++;
		    else
			goto syntax;
		    break;
		default:
		    goto syntax;
		}
	    } else {
syntax:
		fprintf(stderr, "Usage: %s [options] <old >new\n", argv[0]);
		fprintf(stderr, "options are:\n");
		fprintf(stderr, "-L#        put TCWMEs in every # clocks.\n");
		fprintf(stderr, "-L         put TCWMEs in every 480 clocks.\n");
		fprintf(stderr, "-Multiple  make duplicated notes 'long'.\n");
		fprintf(stderr, "-multiple  allow duplicated notes.\n");
		fprintf(stderr, "-M -m      make duplicated notes 'short'.\n");
		fprintf(stderr, "           (default retriggers on dups).\n");
		fprintf(stderr, "-pressure  allow after-touch.\n");
		fprintf(stderr, "-tcis      allow MPU_TCIS.\n");
		fprintf(stderr, "-tcwme     allow multiple MPU_TCWMEs.\n");
		exit(2);
	    }
	}
	curmode = lastmode = 0;
	lasttcwme = -1;
	dt = 0;
	now = 0;
	nexttcwme = barlen;
	while ((i = getc(stdin)) != EOF) {
	    if (i == MPU_TCIP || i == MPU_TCIS) {
		dt += MPU_CLOCK_PERIOD;
		now += MPU_CLOCK_PERIOD;
		if (barlen && now >= nexttcwme) {
		    i = now - nexttcwme;
		    putdt(dt - i);
		    putc(MPU_TCWME, stdout);
		    dt = i;
		    lasttcwme = nexttcwme;
		    nexttcwme += barlen;
		} else if (i == MPU_TCIS && tcisflg) {
		    putc(i, stdout);
		    dt -= MPU_CLOCK_PERIOD;
		}
		continue;
	    }
	    dt += i;
	    now += i;
	    if (barlen && now >= nexttcwme) {
		i = now - nexttcwme;
		putdt(dt - i);
		putc(MPU_TCWME, stdout);
		dt = i;
		lasttcwme = nexttcwme;
		nexttcwme += barlen;
	    }
	    if ((k = getc(stdin)) == EOF) {
		fprintf(stderr, "EOF after 0x%x (dt=%d)\n", i, dt);
		exit(1);
	    }
	    if (k & M_CMD) {
		if ((k & M_CMD_MASK) == 0xF0) {	/* those F# weirdos */
		    if (k < 0xF8) {		/* sys excl or sys common */
			dt = putdt(dt);
			putc(k, stdout);
			if (k == SX_CMD) {	/* sys excl */
			    do {
				k = getc(stdin);
				putc(k, stdout);
			    } while (k != SX_EOB);
			} else if (k == SC_MSEL) {	/* song position */
			    putc(getc(stdin), stdout);
			    putc(getc(stdin), stdout);
			} else if (k == SC_SSEL)	/* song select */
			    putc(getc(stdin), stdout);
		    } else if (k == MPU_TCWME) {
			if (!barlen) {
			    if (lasttcwme != now || tcwmeflg) {
				dt = putdt(dt);
				putc(k, stdout);
			    }
			    lasttcwme = now;
			}
		    } else if (k != MPU_NO_OP) {
			dt = putdt(dt);
			putc(k, stdout);
		    }
		    continue;
		}
		curmode = k;
		if ((k = getc(stdin)) == EOF) {
		    fprintf(stderr, "EOF after %x (mode)\n", curmode);
		    exit(1);
		}
	    }
	    status = (curmode & M_CMD_MASK);
	    if (status == CH_POLY_KPRS
	     || status == CH_CTL
	     || status == CH_P_BEND) {
		dt = putdt(dt);
		if (curmode != lastmode)
		    putc(lastmode = curmode, stdout);
		putc(k, stdout);
		if ((v = getc(stdin)) == EOF) {
		    fprintf(stderr, "EOF after %x (mode)\n", curmode);
		    exit(1);
		}
		putc(v, stdout);
		continue;
	    } else if (status == CH_PRG
		    || status == CH_PRESSURE) {
		if (status != CH_PRESSURE || pflg) {
		    dt = putdt(dt);
		    if (curmode != lastmode)
			putc(lastmode = curmode, stdout);
		    putc(k, stdout);
		}
		continue;
	    } else if (status == CH_KEY_ON
		    || status == CH_KEY_OFF) {
		chan = curmode % MAXCHAN;
		if ((v = getc(stdin)) == EOF) {
		    fprintf(stderr, "unexpected EOF in mode %x\n", curmode);
		    exit(1);
		}
		if (status == CH_KEY_OFF) {
		    status = CH_KEY_ON;
		    curmode = (status | chan);
		    v = 0;
		}
		if (v == 0) {
		    if (--key[chan][k] < 0) {	/* extra note-off */
			key[chan][k] = 0;
			continue;
		    }
		    if ((mflg & ONLY_LAST) && key[chan][k])
			continue;		/* not the last note-off */
		} else {
		    if (key[chan][k]++ > 0) {	/* extra note-on */
			if (!(mflg & USE_EXTRA)) {
			    if (!(mflg & ONLY_LAST))
				--key[chan][k];	/* use next note-off */
			    continue;		/* skip extra note-on */
			} else if (mflg & ONLY_LAST) {
			    dt = putdt(dt);	/* insert extra note-off */
			    if (curmode != lastmode)
				putc(lastmode = curmode, stdout);
			    putc(k, stdout);
			    putc(0, stdout);
			}
		    }
		}
		dt = putdt(dt);
		if (curmode != lastmode)
		    putc(lastmode = curmode, stdout);
		putc(k, stdout);
		putc(v, stdout);
	    } else
		fprintf(stderr, "mode=%x?\n", curmode);
	}
	for (chan = MAXCHAN; --chan >= 0; ) {	/* clear any stuck notes */
	    curmode = CH_KEY_ON | chan;
	    for (i = 0; i < 128; i++) {
		while (--key[chan][i] >= 0) {
		    dt = putdt(dt);
		    if (lastmode != curmode)
			putc(lastmode = curmode, stdout);
		    putc(i, stdout);
		    putc(0, stdout);
		}
	    }
	}
	if (lasttcwme != now) {
	    putdt(dt);			/* a final command */
	    putc(MPU_TCWME, stdout);
	}
	exit(0);
}

int putdt(dt)
{
	while (dt >= 240) {
	    putc(MPU_TCIP, stdout);
	    dt -= 240;
	}
	putc(dt, stdout);
	return(0);
}
