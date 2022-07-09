/* adagio.h -- functions to write Adagio format (text) files
 *
 * Vinesh Sridhar and Roger B. Dannenberg
 * 2022
 */


int adagio_init(const char *filename);
void adagio_chord_tone(int chan, int pitch_class, int vel, double dur);
void adagio_pitch(int chan, int pitch, int vel);
void adagio_finish();
