/* adagio.h -- functions to write Adagio format (text) files
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */


int adagio_init(const char *filename);
void adagio_chord_tone(int pitch_class);
void adagio_pitch(int pitch);
void adagio_finish();
