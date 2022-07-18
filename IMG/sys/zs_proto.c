#ifndef lint
static	char sccsid[] = "@(#)zs_proto.c 1.1 89/10/11 Copyr 1989 Sun Micro";
#endif

/*
 * Copyright (c) 1989 by Sun Microsystems, Inc.
 */

#include "zs.h"
#include "zsi.h"
#include "zsb.h"
#include "zss.h"
#include "zsm.h"
#if NZS > 0
#include <sys/types.h>
#include <sundev/zscom.h>

extern struct zsops zsops_null;
extern struct zsops zsops_async;

#if NZSS > 0
extern struct zsops zsops_sdlc;
#endif

#if NZSI > 0
extern struct zsops zsops_isdlc;
#endif

#if NZSB > 0
extern struct zsops zsops_bsc;
#endif

#if NZSM > 0
extern struct zsops zsops_midi;
#endif


struct zsops *zs_proto[] = {
	&zsops_null,			/* must be first */
	&zsops_async,
	/* new entries go here */
#if NZSS > 0
	&zsops_sdlc,
#endif
#if NZSI > 0
	&zsops_isdlc,
#endif
#if NZSB > 0
	&zsops_bsc,
#endif
#if NZSM > 0
	&zsops_midi,
#endif

	0,				/* must be last */
};
#endif
