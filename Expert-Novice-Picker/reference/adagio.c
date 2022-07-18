/* adagio.c -- functions to write Adagio format (text) files
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */

// This interface assumes increasing time order of all notes and is limited
// to banjo notes (all 16ths) and chord notes of any duration. (Chord notes
// do not advance time and should be output before the banjo note.)

#include <stdio.h>

// keep track of current program. In practice, only the first note on any
// given channel will output a program (Z) attribute. This is recommended
// over giving every note a program change, which might output a new program
// change with every note.
static int programs[16] = { -1, -1, -1, -1, -1, -1, -1, -1, 
                            -1, -1, -1, -1, -1, -1, -1, -1,
};

static const char *PC2NAME[] = {  // names of Adagio pitch classes
	"C", "CS", "D", "DS", "E", "F", "FS", "G", "GS", "A", "AS", "B",
};

static FILE *aof = 0;  // adagio output file


int adagio_init(const char *filename)
{
    if (!(aof = fopen(filename, "w"))) {
        perror(filename);
        return -1;
    }
    return 0;
}
    

static char *adagiopitchname(int p)
{
    static char buf[64];

    sprintf(buf, "%s%d", PC2NAME[p % 12], p / 12 - 1);
    return buf;
}


void adagio_chord_tone(int chan, int pitch_class, int vel, double dur)
{
    int sixteenths = (int) (dur * 4) + 0.5;  /* round to whole number */
    // output program change only on the first note of the channel:
    const char *pgrm = (programs[chan] != 1 ? " Z1" : "");
    programs[chan] = 1;  // Piano sound
    fprintf(aof, "S%d %s3 N0 V%d L%d%s\n", 
            sixteenths, PC2NAME[pitch_class], chan + 1, vel, pgrm);
}


// adagio_pitch - output a 16th note at the given MIDI pitch
//
void adagio_pitch(int chan, int pitch, int vel)
{
    // output program change only on the first note of the channel:
    const char *pgrm = (programs[chan] != 106 ? " Z106" : "");
    programs[chan] = 106;  // Banjo sound
    fprintf(aof, "S %s V%d L%d%s\n", adagiopitchname(pitch), chan + 1, vel, pgrm);
}


void adagio_finish()
{
    fclose(aof);
}


