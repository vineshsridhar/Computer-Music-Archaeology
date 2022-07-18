/* @(#)midi_ioctl.h 1.1 89/10/11 */
/*
 * Copyright (c) 1989 by Sun Microsystems, Inc.
 */

#ifndef _sundev_midi_ioctl_h
#define	_sundev_midi_ioctl_h

#include <sys/ioccom.h>

struct midi_ioctl {
	int	tick;		/* uSecs per time unit */
	int	speed;		/* baud rate of line */
	int	rdsize;		/* read buffer size */
	int	wrsize;		/* write buffer size */
};

#define	MIDI_SET	_IOW(M, 0, struct midi_ioctl)
#define	MIDI_GET	_IOR(M, 1, struct midi_ioctl)
#define	MIDI_DRAIN	_IO(M, 2)
#define	MIDI_FLUSH	_IO(M, 3)
#define	MIDI_PAUSE	_IOW(M, 4, int)

/* MIDI_PAUSE bits */
#define MIDI_READING           0x01    /* read is in-progress */
#define MIDI_READ_PAUSE        0x02    /* read is paused */
#define MIDI_WRITING           0x04    /* write is in-progress */
#define MIDI_WRITE_PAUSE       0x08    /* write is paused */

#define	MIDI_HDRSIZE	3	/* 2 bytes delay, 1 byte count */
#define	MIDI_MAXREC	255	/* max for 1 byte count */

#endif /* !_sundev_midi_ioctl_h */
