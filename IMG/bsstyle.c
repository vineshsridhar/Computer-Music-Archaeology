/*	BSSTYLE -- Style globals for IMG
**	psl 2/90
*/
#define	BS	/* to include BS stuff in libamc.h */
#include <stdio.h>
#include <string.h>
#include	<midi.h>

STYLE	S[NUMS]	= {
	{ "bebop", 480,	960, { DCHAN0, },		"Bebop Jazz",
	    "BEBOP", 10, 1, "21:00", 183,
	    42,0,0, DEFLC0,0,0, 40,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 40,0,0, 60,25,0,"0",
	    89,DEFBC,99,2,0,0,0,99,
	    2,DEFCC,64,2,0,0,0,64,
	    0,DEFDC,64,0,0,0,0,64, },
	{ "grass", 480,	240, { 0, },			"Bluegrass Banjo",
	    "BLUEGRASS", 7, 1, "28:00", 150,
	    105,0,0, DEFLC0,0,0, 72,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 72,0,0, 50,50,0,"0",
	    93,DEFBC,64,2,0,0,0,64,
	    101,DEFCC,72,2,0,0,0,72,
	    0,0,0,0,0,0,0,0 },
	{ "boogi", 480,	960, { DCHAN0, },		"Boogie-Woogie Piano",
	    "BOOGIE", 0, 1, "36:00", 174,
	    1,0,0, DEFLC0,0,0, 64,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 64,0,0, 50,50,0,"0",
	    88,DEFBC,56,2,0,0,0,56,
	    1,DEFCC,88,2,0,0,0,88,
	    0,DEFDC,48,0,0,0,0,48 },
	{ "class", 480,	960, { 0, },			"Alberti Bass",
	    "CLASSICAL", 0, 1, "28:00", 120,
	    17,0,0, DEFLC0,0,0, 80,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 80,0,0, 50,50,0,"0",
	    17,DEFBC,64,2,0,0,0,64,
	    33,0,64,2,0,0,0,64,
	    0,0,0,0,0,0,0,0 },
	{ "march", 480,	960, { DCHAN0, },		"Strident March Music",
	    "MARCH", 8, 1, "24:00", 112,
	    41,0,0, DEFLC0,0,0, 80,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 80,0,0, 50,50,0,"0",
	    48,DEFBC,64,2,0,0,0,64,
	    38,DEFCC,64,2,0,0,0,64,
	    0,DEFDC,64,0,0,0,0,64 },
	{ "mozar", 360,	360, { 0, },			"Mozart Waltz",
	    "MOZART", 0, 1, "30:00", 96,
	    17,0,0, DEFLC0,0,0, 80,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 80,0,0, 50,50,0,"0",
	    17,0,64,2,0,0,0,64,
	    33,0,64,2,0,0,0,64,
	    0,0,64,0,0,0,0,64 },
	{ "samba", 480,	960, { DCHAN0, DCHAN1, DCHAN2, },	"Latin Samba",
	    "SAMBA", 4, 1, "48:00", 120,
	    70,0,0, DEFLC0,0,0, 99,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 99,0,0, 50,67,0,"0",
	    87,DEFBC,64,2,0,0,0,64,
	    7,DEFCC,64,2,0,0,0,64,
	    0,DEFDC,64,0,0,0,0,64 },
	{ "seque", 480,	120, { 0, },			"Pentatonic Wallpaper",
	    "SEQUENCE", 0, 1, "0:180", 170,
	    72,79,0, DEFLC0,DEFBC,0, 64,80,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 64,80,0, 50,50,0,"0",
	    87,0,80,2,0,0,0,80,
	    69,0,64,2,0,0,0,64,
	    0,0,64,0,0,0,0,64 },
	{ "swing", 480,	960, { DCHAN0, },		"Swing Combo Jazz",
	    "SWING", 10, 1, "60:00", 128,
	    60,0,0, DEFLC0,0,0, 80,0,0, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 80,0,0, 60,67,0,"0",
	    89,DEFBC,64,2,0,0,0,64,
	    2,DEFCC,64,2,0,0,0,64,
	    0,DEFDC,64,0,0,0,0,64 },
	{ "toner", 360,	360, { 0, },			"12-tone Sequence",
	    "TONE ROW", 0, 1, "21:00", 120,
	    30,70,1, DEFBC,DEFCC,DEFLC0, 96,80,80, 2,2,2,
	    0,0,0, 0,0,0, 0,0,0, 96,80,80, 74,67,0,"0",
	    89,0,64,2,0,0,0,64,
	    1,0,64,2,0,0,0,64,
	    0,0,64,0,0,0,0,64 },
#ifndef	LINT
	{ { 0, }, },
#endif
};

/* Look up a style in the repertoire provided.
** If not found and the first empty spot is < styles[limit], return empty spot
** If not found and first empty is >= styles[limit] return (STYLE *) 0
*/
STYLE	*
bsstyle(name, styles, limit)
char	*name;			/* style name */
STYLE	styles[];		/* style repertoire */
int	limit;			/* size of styles[] or 0 */
{
	register STYLE *sp;

	for (sp = styles; sp->name && *sp->name; sp++)
	    if (strcmp(name, sp->name) == 0)
		return(sp);
	return(sp >= &styles[limit]? (STYLE *) 0 : sp);
}
