/*
** Disassemble MIDI system exclusive commands and print them on 'fp'.
** Return '0' if ok, '-1' if no vector available for the synthesizer id.
** `Note`: This routine vectors the system exclusive command to the proper
** disassembler based on the decoded manufacturer's midi id number.
** To add additional synthesizers:
** .IP 1)
**     either add an '#include' statement in 'libdev.h' for the new device's
**     header file or, if no other definitions are required, add a '#define'
**     statement for the manufacturer's MIDI ID number,
** .IP 2)
**     insert a 'case...' statement below to call decipher system exclusive
**     commands.
** .LP
*/
#include <stdio.h>
#include <libdev.h>
#include <libmidi.h>

#define Case break; case
#define Default break; default
#define u_char unsigned char

extern	char	*sx_manuf();

da_sx(fp,c)
FILE *fp;	/* output file */
u_char *c;	/* array of MIDI commands */
{
	register int i, j;

	fprintf(fp, "system exclusive\n");
	switch (c[1]) {
	Case ID_DX7:
	    da_dx7_sx(fp,c);
	Case ID_ENSONIQ:
	    da_mirage_sx(fp,c);
	Case ID_ROLAND:
	    da_roland_sx(fp,c);
	Case ID_MISC:
	    switch (SUBID(c[2], c[3])) {
	    Case SUBID_HEADER:
		if ((j = da_head_sx(fp,c) + 1) > 1)
		    goto dump;
	    Case SUBID_IOTA:
		if ((j = da_iota_sx(fp,c) + 1) > 1)
		    goto dump;
	    Case SUBID_KMX:
		fprintf(fp,
		 "%5x %2x %2x ; da_sx: KMX mfg id=\t%d %d %d\n",
		 c[1], c[2], c[3], c[1], c[2], c[3]);
		j = 4;
		goto dump;
	    Default:
		fprintf(fp,
		 "%5x %2x %2x ; da_sx: %s misc mfg id=\t%d %d %d\n",
		 c[1], c[2], c[3], sx_manuf(&c[1]), c[1], c[2], c[3]);
		j = 4;
		goto dump;
	    }
	Default:
	    fprintf(fp,"%5x ; da_sx: %s mfg id=%d\n",
	     c[1], sx_manuf(&c[1]), c[1]);
	    j = 2;
dump:
	    for (i = 0; c[j + i] != SX_EOB; i++)
		fprintf(fp, "%s %02x", (i % 16)? "" : (i? "\n " : " "), c[j+i]);
	    fprintf(fp, "\n");
	}
	return 0;
}
