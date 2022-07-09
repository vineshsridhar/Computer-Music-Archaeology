// smf.cpp -- simple interface to write SMF using allegro library.
//     allegro is C++, but we'll expose a simple C interface
//
// Keeping things simple, you start by calling smf_start(n), where
// n is the number of tracks. It should be at least 2 because the
// first track is for tempo control.
//
// We assume one channel per track, so call smf_channel(track, chan)
// for each track (other than track 0) to set the channel before 
// anything else. The limit is 32 tracks.
//
// Besides tempo and setting midi program, you can insert a note
// at arbitrary times with smf_note(). Alternatively, start a 
// sequence of notes with smf_seq_start(). Then add notes with
// smf_seq_append(), which inserts each note at the end time of
// the previous note.
//
// Roger B. Dannenberg
// July 2022

#include "smf.h"
#include "allegro.h"

Alg_seq_ptr alg_seq;

static int seq_track = 0;
static double seq_start_beat = 0;
#define MAX_TRACKS 32
static int channel[32]; 

void smf_start(int n)
{
    for (int i = 0; i < MAX_TRACKS; i++) {
        channel[i] = 0;
    }
    alg_seq = new Alg_seq();
    alg_seq->set_time_map(new Alg_time_map());
    for (int i = 0; i < n; i++) {
        alg_seq->add_track(i);
    }
    alg_seq->convert_to_beats();
}


void smf_channel(int track, int chan)
{
    assert(0 <= track && track < MAX_TRACKS);
    channel[track] = chan;
}


void smf_tempo(double bpm, double when)
{
    alg_seq->get_time_map()->insert_tempo(bpm, when);
}


void smf_program_change(int track, int program, double when)
{
    Alg_update_ptr update = new Alg_update;
    update->time = when;
    update->chan = channel[track];
    update->set_identifier(-1);
    update->parameter.set_attr(symbol_table.insert_string("programi"));
    update->parameter.i = program;
    alg_seq->track(track)->insert(update);
}


void smf_note(int track, double start, int key, int vel, double dur)
{
    Alg_note_ptr note = new Alg_note();
    note->time = start;
    note->chan = 0;
    note->pitch = key;
    note->loud = vel;
    note->dur = dur;
    alg_seq->track(1)->insert(note);
}



void smf_write_smf(const char *filename)
{
    if (!alg_seq->smf_write(filename)) {
        fprintf(stderr, "MIDI file write to %s failed.\n", filename);
    }
}


void smf_write(const char *filename)
{
    if (!alg_seq->write(filename)) {
        fprintf(stderr, "Allegro file write to %s failed.\n", filename);
    }
}


void smf_seq_start(int track, double start_beat)
{
    seq_track = track;
    seq_start_beat = start_beat;
}


void smf_seq_append(int key, int vel, double dur)
{
    smf_note(seq_track, seq_start_beat, key, vel, dur);
    seq_start_beat += dur;
}


void smf_chord_tone(int key, int vel, double dur)
{
    smf_note(seq_track + 1, seq_start_beat, key, vel, dur);
    // do not update seq_start_beat
}


void smf_finish()
{
    if (alg_seq) {
        delete alg_seq;
        alg_seq = NULL;
    }
}
