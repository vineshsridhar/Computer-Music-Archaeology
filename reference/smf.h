// smf.h -- simple interface to write SMF using allegro library.
//     allegro is C++, but we'll expose a simple C interface
//
// Roger B. Dannenberg
// July 2022

#ifdef __cplusplus
extern "C" {
#endif

// start a file with n tracks -- normally reserve track 0 for time track
// so use at least n=2
void smf_start(int n);
// set the channel for a track
void smf_channel(int track, int chan);
// change tempo at when (in beats) to bpm; intial tempo is 100bpm
void smf_tempo(double bpm, double when);
// insert MIDI program change
void smf_program_change(int track, int program, double when);
// add a note to the given track with start time and dur in beats
void smf_note(int track, double start, int key, int vel, double dur);
// write the sequence as standard midi file
void smf_write_smf(const char *filename);
// write the sequence as allegro file
void smf_write(const char *filename);
// delete and free the whole sequence 
void smf_finish();

#ifdef __cplusplus
}
#endif
