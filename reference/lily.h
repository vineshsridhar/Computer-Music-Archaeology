/* lily.h -- functions to write Lilypond notation file
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */
int lily_init(const char *filename);
void lily_tab_start();
void lily_pitch(int pitch, int sharp_key);
void lily_rest();
void lily_finish();
void lily_add_chord(const char *chord, int spc);
void lily_end_chords();
