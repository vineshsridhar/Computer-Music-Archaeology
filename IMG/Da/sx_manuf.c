/*
**	char *Sx_man[]	-- single-byte id manufacturer names
**	char *Sx_man_ex[]	-- three-byte id manufacturer names
**	char *sx_manuf(u_char *cp) -- return manufacturer name or "unknown"
**	psl 1/90
*/
#include <libmidi.h>

char	*Sx_man[]	= {
	0, "Sequential", "IDP", "Octave-Plateau",
	"Moog", "Passport Designs", "Lexicon", "Kurzweil",
	"Fender", 0, "AKG Acoustics", "Voyce Music",
	"Waveframe Corp", "ADA", "Garfield Elec.", "Ensoniq",
	"Oberheim", "Apple Computer", "Grey Matter Response", 0,
	"Palm Tree Inst.", "JL Cooper", "Lowrey", "Adams-Smith",
	"Emu Systems", "Harmony Systems", "ART", "Baldwin",
	"Eventide", "Inventronics", 0, "Clarity",
	"Passac", "SIEL", "Synthaxe", 0,
	"Hohner", "Twister", "Solton", "Jellinghaus MS",
	"Southworth", "PPG", "JEN", "SSL Limited",
	"Audio Veritrieb", 0, 0, "Elka",
	"Dynacord", 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	"Kawai", "Roland", "Korg", "Yamaha",
	"Casio", 0, "Kamiya Studio", "Akai",
	"Japan Victor", "Meisosha", "Hoshino Gakki", "Fujitsu Elect",
	"Sony", "Nisshin Onpa", "TEAC Corp.", "System Product",
	"Matsushita Electric", "Fostex", 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
};
char	*Sx_man_ex[]	= {
	0, 0, 0, 0,
	0, 0, 0, "Digital Music Corp.",
	0, 0, 0, "IVL Technologies",
	"Southern Music Systems", "Lake Butler Sound", "Alesis", 0,
	"DOD Electronics", 0, 0, 0,
	"Perfect Fretworks", 0, "Opcode", 0,
	"Spatial Sound", "KMX", 0, 0,
	0, 0, 0, 0,
	"Axxes", 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,
};

char	*
sx_manuf(cp)
u_char	*cp;
{
	if (*cp) {
	    if (Sx_man[*cp & 0x7F])
		return(Sx_man[*cp & 0x7F]);
	    return("Unknown");
	}
	if (*++cp)	/* we don't know any with 2nd byte non-zero */
	    return("Unknown");
	if (Sx_man_ex[*++cp & 0x7F])
	    return(Sx_man_ex[*cp & 0x7F]);
	return("Unknown");
}
