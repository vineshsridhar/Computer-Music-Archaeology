# Expert Novice Picker

Code and documentation for Peter Langston's *Expert Novice Picker*, aka lick.c, presented in his paper *Six Techniques for Algorithmic Music Composition*. The program accepts a data file that specifies the chord changes and length of the piece and outputs a banjo improvisation fitted to those parameters.

In this repo you will find two applications, the first is located in the folder `restoration`, which presents the program in a form closest to how it was presented in the original paper. The other folder, `reference`, contains a modified version of the program that produces output in multiple formats, contains more documentation and many stylistic changes to the code. The intention is to make the behavior of `reference` the same as the original, but to make the code more readable.

# Compiling and Running

After navigating to the `reference` directory, compile the program using the following command:

	make
    
You can then run the program with this command:

    ./lick foggymountaintop.txt
    
Use the `--help` flag for usage instructions. Two 'gc' format files that define chord progressions have been provided: `goldrush.txt` and `foggymountaintop.txt`.

Note that, by default, the program assumes that there is a directory named `tmp` in the same directory as the executable, used to store the program's output.

# Interpreting Output

`restoration/lick` produces a text file `tmp/lick.tab`, the program's banjo improvisation written in tablature. Each note, all of which are sixteenths, is represented with a fret number over a string. Chords are output to the right and the right-hand finger that plucks each note (Thumb, Index, or Middle) is output to the left. It optionally produces a `lick.trace` file that documents how the program chose the notes that ended up in `lick.tab`.

In addition to these two files, `reference/lick` produces:

- `tmp/lick.ly`, which can be compiled into a `pdf` file using lilypond. This allows us to view the improvisation in standard music notation.
- `tmp/lick.gio`, a file in the Adagio format, which is easy to read by both humans and code. We have also supplied `gio2mid.lsp`, a [Nyquist](https://www.cs.cmu.edu/~music/nyquist/) script which converts an Adagio file into a standard MIDI file so that you can listen to the improvisation set over the supplied chord changes.
- `tmp/lick.gro`, a file in the Allegro format, another text representation that is readable by [Audacity](https://audacityteam.org) and easy to process with editors or code.
- `tmp/lick.mid`, a standard MIDI file.

# The Guitar-Chord Format

Guitar-chord files provide Expert Novice Picker with chord progressions, and as a consequence, the length of the piece. It does this using four different types of statements. The `#SCALE` statement indicates what notes the program is allowed to pick from throughout the song, denoted using pitch classes. The `#MULT` and `#BEATS` statements tell us a chord's duration: each chord lasts for `MULT * 16 / BEATS` sixteenth notes. MULT and BEATS are basically the "numerator" and "denominator" of the time signature if each chord occupies one measure. You can also cut MULT in half and specify two chords per measure, etc.

The `#CHORDTONES` statement adds a chord to the song. Each `#CHORDTONES` statement first supplies a cosmetic label for the chord, such as 'A' or 'C#m7', and then provides the pitch classes that spell out that chord. The syntax is:

    #CHORDTONES<tab><label><tab><pitchclasslist>

Lilypond output assumes chord labels have the form

    <pitch><sharp><extra>

where `<pitch>` is in CDEFGAB, `<sharp>` is either "#" or "" (empty), `<extra>` is "" (empty), "m" (minor), "m7" (minor 7th), "M7" (major 7th), "*" (diminished) or "*7" (diminished 7th). Other chords *might* work, e.g. "6" or "m6".
    
`#CHORDTONES` statements must be followed by one one blank line for each time the chord is to be added to the song. Each blank line adds the duration of the chord.

Note that `#SCALE`, `#MULT`, and `#BEATS` statements must appear only once in the chord file and should be placed above any `#CHORDTONES` statements. Also, elements within statements must be delimited with tabs and pitch-class lists must be delimited with commas. Below we have included a simple chordfile (The | indicate newlines, and note that tabs may not be rendered faithfully by MarkDown, but in any case, spaces here have a single TAB character):

	#SCALE	9,11,1,2,4,6,8
	#MULT	4
	#BEATS	4
	#CHORDTONES	Bm7	11,2,6,9
	|
	#CHORDTONES	E7	4,8,11,2
	|
	#CHORDTONES	A	9,1,4
	|
	|

In the reference version, we have also implemented a `#SHARPKEY` statement that takes a 1 or a 0, depending on whether the key of the song uses mostly sharps or flats. This produces a nicer Lilypond output for songs in flat keys, but does not affect MIDI generation.

Here is what the program outputs when provided with the chordfile:

![sample chordfile output](https://raw.githubusercontent.com/vineshsridhar/Expert-Novice-Picker/main/figures/ii%20V.png)





