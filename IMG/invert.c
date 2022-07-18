/*
**	INVERT -- Invert a MIDI data around a key
**	psl 1/89
*/
#include <stdlib.h>
#include	<stdio.h>
#include	<midi.h>
#include "key2name.h"

main(argc, argv)
char	*argv[];
{
	register int i, key, mode;
	long now;
	MCMD *mp;

	if (argc != 2) {
syntax:
	    fprintf(stderr, "Usage: %s pitch\n", argv[0]);
	    fprintf(stderr, "\"pitch\" can be a note name, hex, or dec.\n");
	    fprintf(stderr, "e.g. \"F3\", \"0x41\", & \"65\" are equivalent\n");
	    exit(2);
	}
	key = name2key(argv[1]);
	for (now = 0L; mp = getmcmd(stdin, now); now = mp->when) {
	    mode = mp->cmd[0] & M_CMD_MASK;
	    if (mode == CH_KEY_ON || mode == CH_KEY_OFF) {
		i = key + key - mp->cmd[1];
		mp->cmd[1] = i < 1? 1 : (i > 255? 255 : i);
	    }
	    putmcmd(stdout, mp);
	}
	exit(0);
}
