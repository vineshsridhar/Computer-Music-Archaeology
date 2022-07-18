#include <sys/ioctl.h>
#include <mpuvar.h>

MpuSetTrack(f,n)
/*
** Set the current track on 'f' (a midi device) to 'n'.
** 'n' is 0-7, or 'MPU_TR_COM' (midi command track),
** or 'MPU_TR_COND' (conductor track).
** Tracks 0-7 are used for play data.  See 'midiplayinit'
** and '/usr/midi/src/bin/play.c'.
*/
{
	return ioctl(f,MPU_IOC_TRACK,&n);
}
