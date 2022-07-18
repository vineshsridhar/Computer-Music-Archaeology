/*
** LICK -- Generate "random" banjo parts
** lick0 module: main(), output(), misc functions
** psl 7/87
*/
#include "lick.h"

// functions to write standard midi file and Allegro file:
#include "smf.h"

// functions to write Lilypond notation file:
#include "lily.h"

// functions to write Adagio file
#include "adagio.h"

static char *chord_pat_file = 0; /* file with chord pattern for getchords() */

static char *Knams[] = {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B",
};

static int tuning[STRINGS] = { /* How the banjo is tuned */
        62, 59, 55, 50, 67, /* open G-Major */
};

static int string_chan[STRINGS] = {
        0, 1, 2, 3, 4, /* default channel numbers for 5 strings */
};
static int chord_output_chan = 5;
static int enable_chord_output = 1;

int ilhloc = OPEN; /* initial left hand position */

#define DEFTAB "./tmp/lick.tab"
#define DEFTRC "./tmp/lick.trace"

FILE *tab_outfile = 0;
FILE *trace_outfile = 0;

int main(int argc, char *argv[])
{
    register int i, f, s;
    char *cp;
    long now;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
            case 'c': /* set midi channels */
                f = atoi(&argv[i][2]) - 1;
                if (f < 0) f = 0;
                if (f > 14) f = 14;
                // set all string_chan's to f
                for (s = 0; s < STRINGS; s++) string_chan[s] = f;
                chord_output_chan = f + 1;
                break;
            case 'l': /* specify initial left hand pos */
                ilhloc = atoi(&argv[i][2]);
                break;
            case 'r': /* limit rhpat choices */
                s = atoi(&argv[i][2]);
                if (s <= 0 || s > nrhpat) {
                    fprintf(stderr, "rhpat range is 1..%d", nrhpat);
                    goto syntax;
                }
                nrhpat = s;
                break;
            case 's': /* -s#=# (re)tunes a string */
                s = atoi(&argv[i][2]);
                f = atoi(&argv[i][4]);
                if (argv[i][3] != '=' || s < 1 || s > 5 || f < 0 || f > 127) {
                    goto syntax;
                }
                tuning[s - 1] = f;
                break;
            case 't': /* -tFILE puts tablature in FILE */
                if (argv[i][2] <= ' ' ||
                    !(tab_outfile = fopen(&argv[i][2], "w"))) {
                    perror(&argv[i][2]);
                    goto syntax;
                }
                break;
            case 'T': /* output trace info in DEFTRC */
                if (!(trace_outfile = fopen(DEFTRC, "w")))
                    perror(DEFTRC);
                break;
            case 'X': /* -X suppress chord output */
                enable_chord_output = 0;
                break;
            case 'C': /* -C enables chord-parsing bug */
                chord_parsing_bug = 1;
                break;
            default:
                goto syntax;
            }
        } else if (chord_pat_file) {
            goto syntax;
        } else {
            chord_pat_file = argv[i];
        }
    }
    if (!chord_pat_file) {
syntax:
        fprintf(stderr, "Usage: %s CFILE [options]\n", argv[0]);
        fprintf(stderr, " CFILE contains chord specs in 'gc' format\n");
        fprintf(stderr, " -c# specifies MIDI chan for all strings,\n"
                        "     1 <= # <= 15 (16 reserved for chord output)\n");
        fprintf(stderr, " -l# specifies (approx) left hand location\n");
        fprintf(stderr, " -r# limits right hand patterns\n");
        fprintf(stderr, " -s#=# tunes a string\n");
        fprintf(stderr, " -tFILE puts tablature in FILE\n");
        fprintf(stderr, " -T outputs trace info in '%s'\n", DEFTRC);
        fprintf(stderr, " -X suppress chord output, otherwise output chords to next\n"
                     "    channel above the highest string channel, default 6\n");
        fprintf(stderr, " -C enables chord-parsing bug (see Readme.md)\n");
        fprintf(stderr, "Defaults: -l%d -r%d", OPEN, nrhpat);
        fprintf(stderr, " -s1=%d -s2=%d -s3=%d -s4=%d -s5=%d -t%s\n",
                tuning[0], tuning[1], tuning[2], tuning[3], tuning[4], DEFTAB);
        fprintf(stderr, "Each string defaults to its own channel, 1-5\n");
        fprintf(stderr, "Channels numbers are in the range 1-16.\n");
        exit(2);
    }
    // initialize random number generator with current system time:
    time(&now);
    srand((int) now);

    // open file for tablature if -tFILE was NOT specified
    if (!tab_outfile && !(tab_outfile = fopen(DEFTAB, "w")))
        perror(DEFTAB);

    // read chord specs
    if (!getchords(chord_pat_file))
        exit(1);

    // initialize writing an Adagio (.gio) file
    if (adagio_init(DEFADAGIO) != 0) {
        exit(1);
    }

    // initialize writing Allegro (.gro) and standard MIDI (.mid) file:
    // for .gro & .mid, track 0 is tempo track, track 1 has chan 0, 
    // track 2 has chan 1, etc. So number of tracks is 2 + highest channel
    // number, which is either chord_output_chan if chord output is enabled,
    // otherwise chord_output_chan - 1. We take advantage of 
    // enable_chord_output being an integer 0 or 1 to get number of tracks:
    int track_num = chord_output_chan + enable_chord_output + 1;
    smf_start(track_num);
    for (int i = 1; i < track_num; i++) {
        smf_channel(i, i - 1);  // track n set to MIDI channel n-1
        // all tracks play Banjo sound (105) except chords use piano (1):
        smf_program_change(i, (i - 1 == chord_output_chan ? 1 : 105), 0.0);
    }

    compose();

    // finish writing Lilypond notation file
    lily_finish();

    // close tablature file (tab_outfile is never NULL):
    if (tab_outfile) {
        fclose(tab_outfile);
    }

    // close trace file if -T was specified
    if (trace_outfile) {
        fclose(trace_outfile);
    }

    // finish writing Adagio, Allegro and Std. MIDI File:
    adagio_finish();
    smf_write_smf(DEFSMF);
    smf_write(DEFALLEGRO);
}


int pitchof(int s, int f) /* return MIDI-style pitch for string s on fret f */
{
    return tuning[s] + f;
}


int ontlist(int p, int *lp) /* return 1 iff p is on list *lp */
// list lp is terminated by -1
{
    p %= 12;
    while (*lp != -1) {
        if (p == *lp++) {
            return 1;
        }
    }
    return 0;
}


int overlap(int s1[DIGITS], int s2[DIGITS]) /* check for string crossing */
{
    return ((s1[THUMB] >= 0 && s2[INDEX] >= 0 && s1[THUMB] <= s2[INDEX])
        || (s1[THUMB] >= 0 && s2[MIDDLE] >= 0 && s1[THUMB] <= s2[MIDDLE])
        || (s1[INDEX] >= 0 && s2[THUMB] >= 0 && s1[INDEX] >= s2[THUMB])
        || (s1[INDEX] >= 0 && s2[MIDDLE] >= 0 && s1[INDEX] <= s2[MIDDLE])
        || (s1[MIDDLE] >= 0 && s2[THUMB] >= 0 && s1[MIDDLE] >= s2[THUMB])
        || (s1[MIDDLE] >= 0 && s2[INDEX] >= 0 && s1[MIDDLE] >= s2[INDEX]));
}


int randr(int lo, int hi) /* return uniform dist. rand # in range lo - hi (inclusive) */
{
    register int i;

    i = rand() % 0x1777;
    return lo + i % (hi + 1 - lo);
}


int abs(register int i)
{
    return (i < 0 ? -i : i);
}


char *pitchname(int p)
{
    static char buf[64];

    sprintf(buf, "%s%d", Knams[p % 12], p / 12 - 1);
    return buf;
}


// output - output one note
// t - count of 16th notes from the beginnning of the piece
// str - a length 3 array: which string is played by each finger?
//       -1 means the finger does not play. Only one string will
//       be played at any one time, so 2 entries will be -1.
// frt - fret to play on each string; 0 for open string. All
//       strings may be open, and there are cases where at least
//       4 strings are fretted.
//
void output(int t, int str[DIGITS], int frt[STRINGS])
{
    register int d, s, dt, f, n;
    int sf[STRINGS];
    static int init = 0;  // init is only 0 the first time output is called
    char *name[64];

    if (tab_outfile) {
        // print output for this note to Tablature file
        // format of each line is "T  |  |  5  |  |  C" where "T" can be
        //   T(humb), I(ndex), M(iddle), numbers are fret numbers, and C
        //   is an optional chord name.
        if (init == 0) {  // do these actions the first time output is called
            fprintf(tab_outfile, "#TUNING ");
            for (s = STRINGS; --s >= 0; ) {  // print in reverse order
                fprintf(tab_outfile, "%2d ", tuning[s]);
            }
            fprintf(tab_outfile, "\n#NUT 5 0 0 0 0\n#SPEED 16\n");
            lily_tab_start();
            init++;
        }

        // calculate sf[]: fret position for each string
        // first, set all string fret positions to -1:
        for (s = 0; s < STRINGS; s++) sf[s] = -1;
        // next, find the finger d that plays a string s (if any),
        // and copy that finger's fret to sf[]:
        for (d = 0; d < DIGITS; d++) {
            if ((s = str[d]) >= 0) {
                fprintf(tab_outfile, "%c", "TIM"[d]);
                sf[s] = frt[s];
            }
        }

        // now sf[s] gives the fret (if any) for string s
        fprintf(tab_outfile, " ");
        for (s = STRINGS; --s >= 0; ) {  // print in reverse order
            if (sf[s] >= 0) {
                fprintf(tab_outfile, "%2d ", sf[s]);
            } else {
                fprintf(tab_outfile, " | ");
            }
        }

        // print chord change on tabs only when chord changes (as in
        // original code) but play the chord in Adagio file every
        // measure even if it does not change:
        int chord_change = (t <= 0 || chord_pat[t] != chord_pat[t - 1]);
        if (chord_change || t % 16 == 0) {
            if (chord_change) {  // print to tab only if there's really a change
                fprintf(tab_outfile, " %s", chord_info[chord_pat[t]].name);
            }
            if (enable_chord_output) {
                // enumerate chords and output to Adagio, Allegro, MIDI:
                int *ctones = chord_info[chord_pat[t]].ctones;
                // compute duration by scanning ahead
                int i;
                for (i = t + 1; i < piece_len && chord_pat[t] == chord_pat[i] &&
                                i % 16 != 0; i++) /* keep searching */ ;
                double dur = (i - t) * 0.25;
                for (int *lp = ctones; *lp > -1; lp++) {
                    adagio_chord_tone(chord_output_chan, *lp, KVEL - 20, dur);
                    // track number is chan + 1
                    smf_note(chord_output_chan + 1, t * 0.25, *lp + 48, KVEL - 20, dur);
                }
            }
        }
        fprintf(tab_outfile, "\n");
    }

    // output the notes, using str to determine if note is played on string
    n = 0;  // number of notes played: see if we need a rest in Lilypond
    for (d = 0; d < DIGITS; d++) {  // for each finger
        if ((s = str[d]) >= 0) {  // if the finger plays a string
            n++;
            f = frt[s];
            if (trace_outfile) {
                fprintf(trace_outfile, "d=%d, s=%d, f=%d, p=%d (%s)\n",
                        d, s, f, pitchof(s, f), pitchname(pitchof(s, f)));
            }
            lily_pitch(pitchof(s, f), sharp_key);
            adagio_pitch(string_chan[s], pitchof(s, f), KVEL);
            // SMF and Allegro output: track is channel + 1
            smf_note(string_chan[s] + 1, t * 0.25, pitchof(s, f), KVEL, 0.25);
        }
    }

    // If no note, output a rest for Lilypond notation:
    if (n == 0) {
        lily_rest();
    }
}
