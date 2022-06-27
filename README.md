# Expert Novice Picker

Code and documentation for Peter Langston's *Expert Novice Picker*, aka lick.c, presented in his paper *Six Techniques for Algorithmic Music Composition*. The program accepts a data file that specifies the chord changes and length of the piece and outputs a banjo improvisation fitted to those parameters. 

In this repo you will find two applications, the first is located in the folder `ENP classic`, which presents the program in a form closest to how it was presented in the original paper. The other folder, `ENP`, contains a modified version of the program that produces a lilypond file for standard music notation and an Allegro file for midi output. 

# Compiling and Running

After navigating to the appropriate directory, compile the program using the following command:

	gcc lick.h lick0.c lick1d.c lick2.c -D NOMIDI

The NOMIDI flag is needed because the program relies on an old midi header file that does not seem to be in the C standard library. Use the `--help` flag for usage instructions. Two CFILE/gc files have been provided: `goldrush.txt` and `foggymountaintop.txt`. Note that, by default, the program assumes that there is a file named 'tmp' in the same directory as the C files, used to store the program's output.

# Interpreting Output

'ENP Classic' produces a text file called `lick.tab`, the program's banjo improvisation written in tablature. Each note, all of which are sixteenths, is represented with a fret number over a string. Chords are output to the right and the right-hand finger that plucks each note (Thumb, Index, or Middle) is output to the left. It optionally produces a `lick.trace` file that documents how the program chose the notes that ended up in `lick.tab`. 

In addition to these two files, 'ENP' produces `lick.ly`, which can be compiled into a pdf using lilypond. This allows us to view the improvisation in standard music notation. It also produces `lick_a.gro`, a file in the Allegro format. We have supplied a Nyquist script called `gro2mid.lsp` that converts an Allegro file into a midi file so that you can listen to the improvisation set over the supplied chord changes.

