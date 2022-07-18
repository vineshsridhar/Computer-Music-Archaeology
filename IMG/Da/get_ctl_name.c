#include <stdio.h>
#include <sys/types.h>
#include <libdx7.h>
#include <libmirage.h>
#include <libmidi.h>

#define Case break; case
#define Default break; default

char *
get_ctl_name(f, synth, index)
	FILE *f;	/* for diagnostic output */
	int synth;	/* manufacturer's midi id # */
	int index;	/* controller index */
/*
 * Get and return channel controller name.
 * Vectors the index to the proper disassembler as per the 'synth' id number.
 * This routine must be modified to add controller name disassembly
 * for additional synthesizers.
 */
{
/* To add new synthesizers:
 *     1)
 *    add an '#include' statement for its library header file,
 *    which must contain a macro for the manufacturer's midi ID number,
 *     2)
 *    insert a 'case' statement below to call the routine.
 *     3)
 *    the routine should return a static string value
 */
	extern char *dx7ccname();
	switch (synth) {
	Case ID_DX7: return dx7ccname(index);
	Case ID_ENSONIQ: return dx7ccname(index);	/* ??? */
	Default: fprintf(f, "get_ch_ctl_name: bad synthesizer id = %d\n",synth);
	}
	return NULL;
}
