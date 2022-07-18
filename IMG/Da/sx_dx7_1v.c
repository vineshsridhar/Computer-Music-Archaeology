#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>

#define range(x, base, len) (x >= base && x < base + len)

sx_dx7_1v(f, ch)
	FILE *f;
	u_char *ch;
{
	register int i, j, opct;
	int check_sum = 0; 
	register u_char	val;

	fprintf(f, "\t; voxname=\t");
	for (j = 0; j < VOX_NAME_LEN; j++)
	    fprintf(f, "%c", ch[6 + VOX_NAME_BASE + j]);
	fprintf(f, "\n");

	for (opct = 6, i = 0; i < DX7VOXLEN; i++) {
	    j = DX7_INI_IND(i);
	    val = ch[i + 6];
	    fprintf(f, "  %2x\t; vox    [%3d]=\t", val, i);
	    fprintf(f, (range(i, VOX_NAME_BASE, VOX_NAME_LEN))?
	      "%-10c%-10d%s" : "%-10d%-10d%s",
	     val, dx7_ini_vox[j].par_ub, dx7_ini_vox[j].par_name);
	    if (j == 0)
		fprintf(f, "\top%d\n", opct--);
	    else
		fprintf(f, "\n");
	    check_sum += val;
	}
	val = ch[DX7VOXLEN + 6];
	fprintf(f, "  %2x\t; check-sum=\t%d", val, val);
	check_sum = 0x80 - (0x7f & check_sum);
	if (val == check_sum)
	    fprintf(f, "\t\tvalid check-sum\n");
	else
	    fprintf(f, "\t\t%s %x != %x\n","invalid check-sum",val,check_sum);
}
