/*
**	char *key2name((int) key)
**		return ascii name of the specified MIDI key
**	int name2key((char) *name)
**		return MIDI key number for the ascii key name
**		return -1 for pitch class letter error, -2 for octave error
**	char *key2pc((int) key)
**		return ascii pitch class name of the specified MIDI key
**	int pc2key((char) *name)
**		return MIDI key number for the ascii pitch class name
**		return -1 for pitch class letter error
**	char *sh2ks((int) sharps)
**		return ascii pitch class name of the specified key signature;
**		expressed as a number of sharps (negative for flats).
**	Ascii key name can be 'C#3', 'Db3', '61', '075', or '0x3d';
**	(generated ascii key names will be in the form 'Db3').
**	Ascii pitch class name can be 'C#', 'Db', '61', '075', or '0x3d'.
**	(generated ascii pitch class names will be in the form 'Db').
**	Changed 1/1/89 to make middle C == C3 == 60.
**	psl 9/88
*/

#include "myatoi.h"
#include "key2name.h"

static	char	*Nn[]	= {
	"C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B", 0,
};
static	char	*Ks[]	= {
	"Dbb",
	"Abb", "Ebb", "Bbb", "Fb", "Cb", "Gb", "Db", "Ab", "Eb", "Bb", "F",
	"C",
	"G", "D", "A", "E", "B", "F#", "C#", "G#", "D#", "A#", "E#", "B#",
};

char	*
key2name(key)
{
	register char *bp, *cp, oct;
	static char buf[5];

	if (key < 0 || key > 127)
	    return("???");
	for (bp = buf, cp = Nn[key%12]; *bp = *cp++; bp++);
	oct = key / 12 - 2;
	if (oct < 0) {
	    *bp++ = '-';
	    oct = -oct;
	}
	*bp++ = '0' + oct;
	*bp++ = '\0';
	return(buf);
}

int name2key(n)
char	*n;
{
	register int pc, o;

	if (('0' <= *n && *n <= '9') || *n == '-' || *n == '+')
	    return(myatoi(n));
	for (pc = 0; Nn[pc] && (Nn[pc][0] != n[0] || Nn[pc][1] == 'b'); pc++);
	if (!Nn[pc])
	    return(-1);		/* bad pitch class letter */
	for (o = 1; n[o] == 'b' || n[o] == '#'; o++)
	    pc += n[o] == '#' ? 1 : -1;
	for (; n[o] == '+'; o++);
	o = (n[o] == '-')? ('0' - n[o+1]) : (n[o] - '0');
	if (-2 <= 0 && 0 <= 8)
	    return(pc + o * 12 + 24);
	return(-2);		/* bad octave */
}

char	*
key2pc(key)
{
	register char *bp, *cp;
	static char buf[3];

	for (bp = buf, cp = Nn[key%12]; *bp = *cp++; bp++);
	return(buf);
}

char	*
sh2ks(keysig)
{
	register char *bp, *cp;
	static char buf[4];

	for (keysig += 12; keysig < 0; keysig += 12);
	for (; keysig > 24; keysig -= 12);
	for (bp = buf, cp = Ks[keysig]; *bp = *cp++; bp++);
	return(buf);
}

pc2key(n)
char	*n;
{
	register int pc, o;

	if (('0' <= *n && *n <= '9') || *n == '-' || *n == '+')
	    return(myatoi(n) % 12);
	for (pc = 0; Nn[pc] && (Nn[pc][0] != n[0] || Nn[pc][1] == 'b'); pc++);
	if (!Nn[pc])
	    return(-1);
	for (o = 1; n[o] == 'b' || n[o] == '#'; o++)
	    pc += n[o] == '#' ? 1 : -1;
	return((pc + 12) % 12);
}
