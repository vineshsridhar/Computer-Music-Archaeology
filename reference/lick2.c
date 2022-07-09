/*
** LICK -- Generate "random" banjo parts
** psl 7/87
** lick2 module: getchords()
*/
#include "lick.h"
#include "lily.h"

static void cnvlist(char *str, int *list, int len);
static char *peel(char **spp);

int chord_pat[MAXLEN]; /* chord indices, one per sixteenth note */
int num_chrds = 0; /* how many chords have been defined */
int piece_len = 0; /* how many sixteenths in the piece */
int sharp_key = 0; /* Default to flat key. */

int chord_parsing_bug = 0;  /* restore buggy behavior of original code */

struct Chordstr chord_info[MAXCHRD];

#define NSCALE 13


int getchords(char *file)
{
    register char *cp;
    register int i, j, k, pat;
    char buf[128], *bp, cname[32];

    int ctone[NTONES], scale[NSCALE], spc, beats, mult;
    FILE *cfp;

    if (!(cfp = fopen(file, "r"))) {
        perror(file);
        return 0;
    }

    if (lily_init(DEFLILY) != 0) {
        return 0;
    }

    piece_len = 0; 
    beats = 4; 
    mult = 1;
    spc = (mult * 16) / beats; // sixteenths-per-chord
    pat = scale[0] = ctone[0] = -1;
    while (fgets(buf, sizeof buf, cfp)) {
        //fprintf(stdout, "%s", buf);
        if (*buf == '#') {
            bp = buf;
            cp = peel(&bp);
            if (strcmp(cp, "#BEATS") == 0) {
                beats = atoi(peel(&bp));
                spc = (mult * 16) / beats;
            } else if (strcmp(cp, "#MULT") == 0) {
                mult = atoi(peel(&bp));
                spc = (mult * 16) / beats;
            // Choose a scale (the group of notes from which we are
            // allowed to pick). Only 1 #SCALE field should be present
            // in the chord file.
            } else if (strcmp(cp, "#SCALE") == 0) {
                cnvlist(peel(&bp), scale, NSCALE);
            } else if (strcmp(cp, "#CHORDTONES") == 0) {
                strcpy(cname, peel(&bp));
                cname[NAMELEN] = '\0';
                cnvlist(peel(&bp), ctone, NTONES);
                pat = -1;
                // Determine whether this chord has been added before.
                // Compare is trickier than it needs to be:
                for (i = 0; i < num_chrds; i++) {
                    for (j = 0; chord_info[i].ctones[j] >= 0; j++) {
                        if (chord_info[i].ctones[j] != ctone[j]) {
                            break;
                        }
                    }
                    // either we finished comparing all j pitches in 
                    // chord_info[i] (chord_info[i].ctones[j] == -1) or 
                    // we found a mismatch (... != -1). 
                    if (chord_info[i].ctones[j] == -1 ) {
                        // original code considered this a match:
                        if (chord_parsing_bug) {
                            break;  // we (think we) found a match
                        } else {    
                            // correct implementation must make sure there are
                            // no additional unmatched pitches in ctone[]:
                            if (ctone[j] == -1) {
                                break;  // we correctly found a match
                            }
                        }
                    }
                }
                pat = i;
                // If a new chord is added, add a new element to the
                // chord structure array.
                if (pat == num_chrds) {
                    num_chrds++;
                    printf("add chord #%d: %s\n", num_chrds, cname);
                    strcpy(chord_info[pat].name, cname);
                    for (j = NTONES; --j >= 0; )    
                        chord_info[pat].ctones[j] = ctone[j];
                    k = 0;
                    // Using our scale, determine the chord tones and
                    // passing tones.
                    for (i = 0; i < NSCALE; i++) {
                        for (j = 0; ctone[j] >= 0; j++)
                            if (ctone[j] == scale[i])
                                break;
                        if (ctone[j] == -1 && k < NTONES - 1)
                            chord_info[pat].ptones[k++] = scale[i];
                    }
                    chord_info[pat].ptones[k] = -1;
                }
            } else if (strcmp(cp, "#SHARPKEY") == 0) {
                sharp_key = atoi(peel(&bp));
            }
        } else {
            if (pat < 0) {
                fprintf(stderr, "No CHORDTONES line preceding data\n");
                exit(1);
            }
            if (spc < 1)
                fprintf(stderr, "#BEATS %d, #MULT %d is too fast.\n",
                  beats, mult);
            if (beats * spc != mult * 16)
                fprintf(stderr, "#BEATS %d, #MULT %d truncates.\n",
                  beats, mult);
            // for each blank line, add spc sixteenth notes of the
            // current chord specified by a previous CHORDTONES line.
            lily_add_chord(chord_info[pat].name, spc);
            for (i = 0; i < spc; i++)
                chord_pat[piece_len++] = pat;
        }
    }
    //fprintf(stdout, "\npiece_len: %d\n", piece_len);
    //fprintf(stdout, "num_chrds: %d\n", num_chrds);

    lily_end_chords();
    return piece_len;
}


// peel - parse non-blank field from a string.
// spp - pointer to pointer to string to parse, where the first
//       character is the beginning of the non-blank field
// advance to an end-of-string or non-printing character
// write end-of-string at the end-of-string or non-printing character
//       (only if it is not a blank -- is this a bug?)
// position *spp to point to the next printing character or end-of-string,
//       whichever comes first
// returns the original value of *spp, the address of the non-blank field
//
static char *peel(char **spp)
{
    register char *bp, *sp;
    bp = sp = *spp;
    while (*bp > ' ') {
        bp++;
    }
    if (*bp != ' ') {
        *bp++ = '\0';
        while (*bp && *bp <= ' ') {
            bp++;
        }
    }
    *spp = bp;
    return sp;
}


// cnvlist - parse a list of comma-separated numbers as pitch classes
// str - string to parse containing MIDI key numbers
// list - resulting list of pitch classes from key numbers (duplicates are
//        retained)
// len - maximum length of list including terminating -1
// returns parsed numbers in list, terminated by -1
// at most len - 1 pitch classes are returned
//
static void cnvlist(char *str, int *list, int len)
{
    register char *sp;
    register int i = 0;
    
    while (i < len - 1) {
        sp = str;
        // scan to comma or end-of-string:
        while (*str && *str != ',') str++;
        list[i++] = atoi(sp) % 12;  // 
        if (*str++ != ',') {
            break;
        }
    }
    list[i] = -1;
}

