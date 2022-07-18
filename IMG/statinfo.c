/*
**	STATINFO -- Routines to process MIDI & MPU status/command bytes
**	P. Langston 10/87
*/
#include <stdio.h>
#include	<midi.h>

struct	statstr	Cvstat[]	= {	/* Channel Voice Messages */
	{ 3,	RSSET,	},		/* 8 CH_KEY_OFF */	
	{ 3,	RSSET,	},		/* 9 CH_KEY_ON */	
	{ 3,	RSSET,	},		/* A CH_POLY_KPRS */	
	{ 3,	RSSET,	},		/* B CH_CTL */	
	{ 2,	RSSET,	},		/* C CH_PRG */	
	{ 2,	RSSET,	},		/* D CH_PRESSURE */	
	{ 3,	RSSET,	},		/* E CH_P_BEND */	
	{ 0,	0,	},		/* F Sys common & real-time */
};

struct	statstr	Systat[]	= {	/* System Messages (mpu use) */
	{ -1,	RSCLR,		},	/* F0, SX_CMD */
	{ 1,	RSCLR|UNDF,	},	/* F1, undefined */
	{ 3,	RSCLR,		},	/* F2, SC_MSEL */
	{ 2,	RSCLR,		},	/* F3, SC_SSEL */
	{ 1,	RSCLR|UNDF,	},	/* F4, undefined */
	{ 1,	RSCLR|UNDF,	},	/* F5, undefined */
	{ 1,	RSCLR,		},	/* F6, SC_TSEL */
	{ 1,	RSCLR,		},	/* F7, SX_EOB */
	{ 1,	RSNOP,		},	/* F8, MPU_TCIP */
	{ 1,	RSNOP|UNDF,	},	/* F9, undefined (RT_TCWME) */
	{ 1,	RSNOP,		},	/* FA, RT_SA1M */
	{ 1,	RSNOP,		},	/* FB, RT_CONT */
	{ 1,	RSNOP,		},	/* FC, MPU_TCIS */
	{ 1,	RSNOP|UNDF,	},	/* FD, undefined (clock to host) */
	{ 1,	RSNOP|UNDF,	},	/* FE, undefined (active sensing) */
	{ 1,	RSNOP,		},	/* FF, RT_RESET (mpu reset) */
};

/*
** struct statstr *statinfo(status) - Return a pointer to a statstr entry
** describing the length of commands started with ``status'' and other
** miscellaneous info such as their effect on ``running status''
*/
struct	statstr	*
statinfo(status)
{
	register int i;

	if ((status & M_CMD) == 0)
	    return(0);
	i = (status >> 4) & 0x07;
	return(Cvstat[i].clen? &Cvstat[i] : &Systat[status&0x0F]);
}

/*
** cmdlen(status) - Return the length of a command.
**	-1	system exclusive (commands terminated by SX_EOB, 0xFF)
**	0	illegal ``status'' (outside 0x80 to 0xFF range).
**	1-3	length (including status)
*/
cmdlen(status)
{
	register int i;

	if ((status & M_CMD) == 0)
	    return(0);
	i = (status >> 4) & 0x07;
	return(Cvstat[i].clen? Cvstat[i].clen : Systat[status&0x0F].clen);
}

/*
** statproc(&oldstatus, newbyte) - Process the first byte of a command,
** keeping track of running status, and return:
**	-1	system exclusive (commands terminated by SX_EOB, 0xFF)
**	0	illegal ``status'' (outside 0x80 to 0xFF range).
**	1-3	actual length (including status byte, if present)
**		e.g. for key-on using running status this is 2.
** For example:
**	for (oldstat = 0; read(ifd, buf, 1) == 1; ) {
**		n = statproc(&oldstat, buf[0]) - 1;
**		if (read(ifd, buf + 1, n) == n)
**			write(ofd, buf, n + 1);
**	}
** In this loop each MIDI command (N.B. system exclusive is not handled)
** will be written with a single write, and the status of the current command
** will be: ((*buf & M_CMD)? *buf : oldstat).
** Note that, for MPU data, the routine would have to also pass along timing
** bytes, perhaps in this fashion:
**	for (oldstat = 0; read(ifd, buf, 1) == 1; ) {
**		write(ofd, buf, 1);
**		if (*buf != MPU_TCIP && read(ifd, buf, 1) == 1) {
**			n = statproc(&oldstat, buf[0]);
**			if (read(ifd, buf + 1, n - 1) == n - 1)
**				write(ofd, buf, n);
**		}
**	}
** Neither of these deals with System Exclusive (return value -1) in which
** the command runs until the next SX_EOB (or other status byte).  Nor do
** they deal with errors (return value 0) from bad data.
*/
int statproc(statp, newbyte)
int	*statp;			/* and return actual command length */
{				/* -1 => SysEx, 0 => really bad status */
	register int stat;
	register struct statstr *sp;

	stat = (newbyte & M_CMD)? newbyte : *statp;
	if ((stat & M_CMD) == 0)
	    return(0);			/* really bad status */
	sp = &Cvstat[(stat >> 4) & 0x07];
	if (sp->clen == 0)		/* a system message */
	    sp = &Systat[stat & 0x0F];
	if (sp->flgs & RSSET)
	    *statp = stat;
	else if (sp->flgs & RSCLR)
	    *statp = 0;
	return(sp->clen + ((newbyte & M_CMD)? 0 : -1));	/* for running stat */
}
