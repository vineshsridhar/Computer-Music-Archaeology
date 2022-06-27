# Expert Novice Picker

Code and documentation for Peter Langston's *Expert Novice Picker*, aka lick.c, presented in his paper *Six Techniques for Algorithmic Music Composition*. The program accepts a data file that specifies the chord changes and length of the piece and outputs a banjo improvisation fitted to those parameters. 

In this repo you will find two applications, the first is located in the folder `ENP classic`, which presents the program in a form closest to how it was presented in the original paper. The other folder, `ENP`, contains a modified version of the program that produces a lilypond file for standard music notation and an Allegro file for midi output. 

# Compiling and Running

After navigating to the appropriate directory, compile the program using the following command:

	gcc lick.h lick0.c lick1d.c lick2.c -D NOMIDI

The NOMIDI flag is needed because the program relies on an old midi header file that does not seem to be in the C standard library. Use the `--help` flag for usage instructions. Two CFILE/gc files have been provided: `goldrush.txt` and `foggymountaintop.txt`. Note that, by default, the program assumes that there is a file named 'tmp' in the same directory as the executable, used to store the program's output.

# Interpreting Output

`ENP Classic` produces a text file called `lick.tab`, the program's banjo improvisation written in tablature. Each note, all of which are sixteenths, is represented with a fret number over a string. Chords are output to the right and the right-hand finger that plucks each note (Thumb, Index, or Middle) is output to the left. It optionally produces a `lick.trace` file that documents how the program chose the notes that ended up in `lick.tab`. 

In addition to these two files, `ENP` produces `lick.ly`, which can be compiled into a pdf using lilypond. This allows us to view the improvisation in standard music notation. It also produces `lick_a.gro`, a file in the Allegro format. We have supplied `gro2mid.lsp`, a Nyquist script which converts an Allegro file into a midi file so that you can listen to the improvisation set over the supplied chord changes.

# The Guitar-Chord Format

Guitar-chord files tell Expert Novice Picker what the chord progression of the piece is, and as a consequence, the piece's length. It does this using four different types of statements. The `#SCALE` statement indicates what notes the program is allowed to pick from throughout the song, denoted using pitch classes. The `#MULT` and `#BEATS` statements tells us a chord's duration: each chord lasts for `MULT * 16 / BEATS` sixteenth notes. The `#CHORDTONES` statement adds a chord to the song. Each `#CHORDTONES` statement first supplies a cosmetic label for the chord, such as 'A' or 'C#m7', and then provides the pitch classes that spell out that chord. `#CHORDTONES` statements must be followed by at least one blank line for the chord to be added to the song. Any additional newlines add to that chord's duration. Note that `#SCALE`, `#MULT`, and `#BEATS` statements must appear only once in the chord file and should be placed above any `#CHORDTONES` statements. Also, elements in within statements must be delimited with tabs and pitch-class lists must be delimited with commas. Below we have included a simple chordfile (The | indicate newlines):

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
In ENP, we have also implemented a `#SHARPKEY` statement that takes a 0 or 1, depending on whether the key of the song uses mostly sharps or flats. This produces a nicer lilypond output for songs in flat keys.
Here is what the program output when provided with the chordfile:

![sample chordfile output](https://raw.githubusercontent.com/vineshsridhar/Expert-Novice-Picker/main/figures/ii%20V.png)





