#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>

da_dx7_voice(f, dx, v)
	FILE *f;	/* where to write output */
	register struct dx7 dx;
	register int v;	/* voice within structure */
/*
 * For each voice parameter, print its index, value, range,
 * and parameter name on 'f'.
 */
{
	register int i, j;
	char buf[VOX_NAME_LEN + 1];

	/* print header line *;
	fprintf(f, "%-10s%-10s%-10s%-10s\n", "index", "value", "range", "name");

	/* print operator values */
	for (i=0; i<VOX_NAME_BASE; i++){
		j = DX7_INI_IND(i);
		fprintf(f, "%-10d%-10d%-10d%s\n",
			i,
			dx.dx7_vox_val[v][i],
			dx7_ini_vox[j].par_ub,
			dx7_ini_vox[j].par_name);
	}

	/* print voice name */
	DX7_GET_VNAME(dx, buf, v);
	fprintf(f, "%-10d%-20s%s\n",
		VOX_NAME_BASE,
		buf,
		dx7_ini_vox[DX7_INI_IND(vox_voice_name)].par_name);

	/* print operator enable */
	fprintf(f, "%-9d0%-9o0%-10o%s\n",
		vox_op_ena,
		dx.dx7_vox_val[v][vox_op_ena],
		dx7_ini_vox[DX7_INI_IND(vox_op_ena)].par_ub,
		dx7_ini_vox[DX7_INI_IND(vox_op_ena)].par_name);
}
