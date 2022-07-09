/* adagio.c -- functions to write Adagio format (text) files
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */

#include <stdio.h>

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


void adagio_chord_tone(int pitch_class)
{
    fprintf(aof, "S %s3 N0 V1 Z105\n", PC2NAME[pitch_class]);
}


// adagio_pitch - output a 16th note at the given MIDI pitch
//
void adagio_pitch(int pitch)
{
    fprintf(aof, "S %s V2 Z105\n", adagiopitchname(pitch));
}


void adagio_finish()
{
    fclose(aof);
}


