#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>

#define	MAXCMDLEN	65600		/* needs to be big for SysExcl */
u_char	pds[MAXCMDLEN];
static int	pdi;
static int	pdl = MAXCMDLEN;
static int	lvl, cnt, dim;
static int	status, cmd, cont;

#define d define
#d START	0
#d GET		1

#d CH_LVL	0
#d SC_LVL	1
#d RT_LVL	2
#d SX_LVL	3
#d RS_LVL	4
#d Int register int
#d Uchar register u_char
#d lvl_err(x) MidiError("level error= %s\n",x)

read_ch_arg_cnt() { return dim; } /* return #of args to last midi command */
read_ch_cont(){ return cont; } /* true if command was a continuation */
read_ch_cmd_type(){ return cmd; }

static
read_ch_push(c) u_char c;{
	if (pdi>= pdl) MidiError("read_ch_push: overflow\n"), exit(1);
	pds[pdi++] = c;
}

static u_char smc[20];

/*
** Read and parse midi command from 'f',
** and return a pointer to a static, '0'-terminated array of commands.
** Handles full MIDI 1.0 specification, including the ability of
** midi commands at different levels to interrupt each other.
** Also handles unterminated system exclusive commands,
** e.g., as produced by Yamaha dx7.
*/
u_char *
read_ch(f)
FILE *f;
{
	Int x; 
	Uchar *rtn;
	cont = 0;
	while ((x=getc(f)) != EOF) {
	    Uchar c = x & 0xFF;
	    if (status==START && (c&M_CMD)==0 && smc[fileno(f)]) {
		ungetc(c, f);
		c = smc[fileno(f)];
		cont++;	/* mark as continuation cmd */
	    }
    again:   switch (status) {
		case START:
		    cmd = c;
		    if ((c&M_CMD_MASK) != SX_CMD) c &= M_CMD_MASK;
		    switch (c){
			/* three-byte midi channel commands */
			case CH_KEY_OFF:
			case CH_KEY_ON:
			case CH_POLY_KPRS:
			case CH_CTL:
			case CH_P_BEND:
			    if (lvl > CH_LVL) lvl_err("CH");
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 2;
			    lvl = CH_LVL;
			    smc[fileno(f)] = c = cmd;
			    goto again;
			/* two-byte midi channel command */
			case CH_PRESSURE:
			case CH_PRG:
			    if (lvl > CH_LVL) lvl_err("CH");
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 1;
			    lvl = CH_LVL;
			    smc[fileno(f)] = c = cmd;
			    goto again;
			/* three-byte system common commands */
			case SC_MSEL:
			    if (lvl > SC_LVL) lvl_err("SC");
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 2;
			    lvl = SC_LVL;
			    smc[fileno(f)] = c = cmd;
			    goto again;
			/* two-byte system common commands */
			case SC_SSEL:
			    if (lvl > SC_LVL) lvl_err("SC");
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 1;
			    lvl = SC_LVL;
			    smc[fileno(f)] = c = cmd;
			    goto again;
			/* one-byte system common commands */
			case SC_TSEL:
			    if (lvl > SC_LVL) lvl_err("SC");
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 0;
			    lvl = SC_LVL;
			    smc[fileno(f)] = c = cmd;
			    goto again;
			/* one-byte real-time commands */
			case RT_TCLOCK:
			case 0xF9:	/* MPU_TCWME */
			case RT_START:
			case RT_CONT:
			case RT_STOP:
			    if (lvl > RT_LVL) lvl_err("RT");
			case RT_RESET:	/* highest priority */
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 0;
			    lvl = RT_LVL;
			    c = cmd;
			    goto again;
			case SX_CMD:
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 2;
			    lvl = SX_LVL;
			    goto again;
			case SX_EOB:
			    status = GET;
			    read_ch_push(cnt);
			    read_ch_push(lvl);
			    dim = cnt = 0;
			    lvl = SX_LVL;
			    c = cmd;
			    goto again;
			default:
			    printf("%s%x %s\n",
				"read_ch: no such midi command=0x", 
				cmd, ", skipping 1 byte");
			    status = START;
			    dim = cnt = 0;
			    lvl = 0;
			    continue;
		    }
		case GET:
		    if (cnt-- > 0) {
			read_ch_push(c);
			rtn = NULL;
		    } else if (cmd == SX_CMD) {
			if (!(c & M_CMD)) {
				++dim;
				read_ch_push(c);
				rtn = NULL;
			} else {
				ungetc(c, f);
				cmd = -cmd;
				goto again;
			}
		    } else {
			read_ch_push(c);
			rtn = &pds[pdi - (dim+1)];
			lvl = pds[pdi - (dim+2)];
			cnt = pds[pdi - (dim+3)];
			pdi -= dim+3;
			status = START;
			if (cmd < 0) {
				dim -= 2;
				cmd = -cmd;
			}
			break;
		    }
		}
		if (rtn) return rtn;
	    }
	return NULL;
}
