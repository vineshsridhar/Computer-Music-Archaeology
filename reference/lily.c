/* lily.c -- functions to write Lilypond notation file
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */

#include <stdio.h>

// (letter - 'A') to pitch class:
static const int PC[] = { 9, 11, 0, 2, 4, 5, 7 }; 

static const char *PC2SHARPS[] = {  // lilypond pitch names
    "c", "cis", "d", "dis", "e", "f", "fis", "g", "gis", "a", "ais", "b",
};

static const char *PC2FLATS[] = {
    "c", "des", "d", "ees", "e", "f", "ges", "g", "aes", "a", "bes", "b",
};

static FILE *lof = 0;  // lilypond output file


int lily_init(const char *filename)
{
    if (!(lof = fopen(filename, "w"))) {
        perror(filename);
        return -1;
    }
    fprintf(lof, "%s", "<<\n\\new ChordNames {\n"
            "  \\set chordChanges = ##t\n"
            "  \\chordmode {\n");
    return 0;
}
    

// lilychord - returns Lilypond chord name in a static buffer
// 
// chord - chord name in the form pitch-letter + sharp + alt + ext, where
//    pitch-letter is a letter in "CDEFGAB", sharp is "" (empty) or "#"
//    and alt is "" (empty), "m", "M" (major 7th) or "*" (diminished)
//    and ext is "" (empty) or a digit, e.g. "7" for 7th chord
// spc - duration in 16ths
// sharp_key - whether to use sharps (1) or flats (0) in chord names
//
static char *lilychord(const char *chord, int spc, int sharp_key)
{
    static char buf[64];
    int chordlen;

    switch (spc) {

        case 16:
            chordlen = 1;
            break;
        case 8:
            chordlen = 2;
            break;
        case 4:
            chordlen = 4;
            break;
        case 2:
            chordlen = 8;
            break;
        case 1:
            chordlen = 16;
            break;
        default:
            return "";
    }

    // invalid chord is NULL or empty string; just return a C chord
    if (!chord || !chord[0]) {
        sprintf(buf, "C%d", chordlen);
        return buf;
    }

    char key = chord[0];

    int pitch_class = ((chord[0] >= 'A' && chord[0] <= 'G') ?
                       PC[chord[0] - 'A'] : 0);
    int i = 1;  // index to alteration (m,M,*) and extension (6,7,...)

    if (chord[1] == '#') {
        pitch_class += 1;
        i = 2;
    }

    const char *alt = "";  // e.g., Cm or CM7 or C*
    // check for m, M, or *
    switch (chord[i]) {
        case 'm':
            alt = "m";
            i++;
            break;
        case 'M':
            alt = "maj";
            i++;
            break;
        case '*':
            alt = "dim";
            i++;
            break;
        default:
            break;
    }
        
    // get extension (digit): e.g. from Cm7 or CM7 or Cm6 or C6
    const char *ext = chord + i;  // whatever is left (maybe empty)

    // need colon after chord duration if there's an alteration or extension
    // E.g. C => "C2", but CM7 => "C2:maj7"
    const char *colon = (alt[0] || ext[0] ? ":" : "");

    // Finally, construct a full Lilypond chord name and duration string:
    sprintf(buf, "%s%d%s%s%s", (sharp_key ? PC2SHARPS : PC2FLATS)[pitch_class],
            chordlen, colon, alt, ext);

    return buf;
}


// lilypitchname - convert midi key number to Lilypond pitch string
// sharp_key - if true, accidentals are notated with sharp signs, o.w. flats
//
static char *lilypitchname(int p, int sharp_key)
{
    static char buf[64];
    const char **keynotes = sharp_key ? PC2SHARPS : PC2FLATS;

    switch (p / 12 - 1) {
        case 0:
            sprintf(buf, "%s,,,", keynotes[p % 12]);
            break;
        case 1:
            sprintf(buf, "%s,,", keynotes[p % 12]);
            break;
        case 2:
            sprintf(buf, "%s,", keynotes[p % 12]);
            break;
        case 3:
            sprintf(buf, "%s", keynotes[p % 12]);
            break;
        case 4:
            sprintf(buf, "%s'", keynotes[p % 12]);
            break;
        case 5:
            sprintf(buf, "%s''", keynotes[p % 12]);
            break;
        case 6:
            sprintf(buf, "%s'''", keynotes[p % 12]);
            break;
        case 7:
            sprintf(buf, "%s''''", keynotes[p % 12]);
            break;
        case 8:
            sprintf(buf, "%s'''''", keynotes[p % 12]);
            break;
    }

    return buf;
}


void lily_tab_start()
{
    fprintf(lof, "\\new Staff {\n");
}


// lily_pitch -- output a 16th note of the given pitch to Lilypond score
// pitch - MIDI key number (60 = middle C)
//
void lily_pitch(int pitch, int sharp_key)
{
    fprintf(lof, "%s16 ", lilypitchname(pitch, sharp_key));
}


void lily_rest()
{
    fprintf(lof, "r16");
}


void lily_finish()
{
    fprintf(lof, "\\bar \"|.\" \n}\n>>");
    fclose(lof);
}


void lily_add_chord(const char *chord, int spc, int sharp_key)
{
    fprintf(lof, "%s ", lilychord(chord, spc, sharp_key));
}


void lily_end_chords()
{
    fprintf(lof, "\n}}\n");
}
