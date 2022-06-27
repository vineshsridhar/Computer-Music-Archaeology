# Expert Novice Picker

Code and documentation for Peter Langston's *Expert Novice Picker*, aka lick.c, presented in his paper *Six Techniques for Algorithmic Music Composition*. The program accepts a data file that specifies the chord changes and length of the piece and outputs a banjo improvisation fitted to those parameters. 

In this repo you will find two applications, the first is located in the folder "ENP classic", which presents the program in a form closest to how it was presented in the original paper. The other folder, "ENP", contains a modified version of the program that produces a lilypond file for standard music notation and an Allegro file for midi output. 

# Compiling and Running

After navigating to the appropriate directory, compile the program using the following command:

	gcc lick.h lick0.c lick1d.c lick2.c -D NOMIDI

Use the `--help` flag for usage instructions. Two CFILE/gc files have been provided: `goldrush.txt` and `foggymountaintop.txt`. Note that, by default, the program assumes that there is a file named 'tmp' in the same directory as the C files, used to store the program's output.