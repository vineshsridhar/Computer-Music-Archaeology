# include <stdio.h>
# include <signal.h>
# include <sys/types.h>
# include <sys/ioctl.h>
# include <mpuvar.h>
# include <libmidi.h>
# include <libmpu.h>
# include <libdx7.h>
#define d define
#d Int register int
#d Exit(s, n) perror(s), exit(n)
#d loop(i,j) for(i=0;i<j;i++)

dx7_clear_chan(channel)
/*
** Clear the given 'channel' (appending the necessary
** midi commands to the buffer, 'mc').
*/
{
#d	set	MpuSet
#ifdef	OBSOLETE	/* if not just plain wrong */
	set(MPU_SEND_SYSTEM_MESSAGE);
	set(SX_CMD);
			/* Yamaha kludge to clear hanging notes?? */
	set(ID_DX7);
	set((unsigned char)(0x10|channel));
	set(0x08);	/* CH_KEY_OFF?? (0x80)? */
	set(0x40);	/* function mono/poly (cf dx7_globs.c) */
	set(0x00);	/* YES/NO? */
	set(SX_EOB);
#else
	set(MPU_WANT_TO_SEND_DATA);
	set((unsigned char)(0xB0|channel));	/* control change */
	set(127);	/* Poly, all keys off */
	set(0);		/* ignored */
#endif
}

dx7_clear()
/*
** Clear all 16 dx7 channels.
** A hack necessary to guarantee that all dx7 notes
** really `are` turned off.
*/
{
	Int i;
	loop(i,16) dx7_clear_chan(i);
}

dx7_reset(sig)
/*
** Exit-time or interrupt-level reset of dx7.
** Re-opens the midi device (`N.B.-` it must first be closed),
** clears all the channels.
*/
{
	Int cnt, f=open(MPU_DEV_0,2);
	int track;

	/* open for reading and writing */
	if (f == -1) Exit("dx7_reset:open", 10);
	ZeroMpuCmdCnt();
	set(MPU_RESET);
	set(MPU_MIDI_THRU_OFF);
	dx7_clear();
	if (MpuSetTrack(f,MPU_TR_COM)) Exit("dx7_reset:ioctl", 11);
	if (MpuFlush(f)) Exit("dx7_reset:write", 12);
	close(f);
	if (sig != -1) exit(1);
}
