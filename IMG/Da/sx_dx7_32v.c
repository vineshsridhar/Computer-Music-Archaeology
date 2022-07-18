#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>

#define range(x, base, len) (x >= base && x < base + len)
#define NDOUBLES (27)

sx_dx7_32v(f, ch)
	FILE *f;
	u_char *ch;
{
	register int	b, i, j, k, l, m, opct;
	int check_sum = 0, lm; 
	register u_char	val;

	for (k = 0, m = 0; k < DX7NVOX; k++, m++) {
	    fprintf(f, "\t; voxname[%3d]=\t", k);
	    for (i = 0; i < VOX_NAME_LEN; i++)
		fprintf(f, "%c", ch[6 + m + VOX_32V_NAME_BASE + i]);
	    fprintf(f, "\n");
	    lm = m % DX732VOXLEN;
	    for (opct = 6, i = 0; i < DX732VOXLEN+NDOUBLES; i++) {
		j = DX7_INI_IND(i);
		b = dx7_32v_vox[j].par_val;
		l = dx7_32v_vox[j].par_ub;
		val = (ch[m + 6] << b) & l;
		if (b == 0) {	/* start of single or compound field */
		    check_sum += ch[m + 6];
		    fprintf(f, "  %2x", val);
		}
		fprintf(f, "\t; vox    [%3d]=\t", lm);
		fprintf(f, (range(lm, VOX_32V_NAME_BASE, VOX_NAME_LEN))?
		 "%-10c;%s" : "%-10d;%s", val, dx7_32v_vox[j].par_name);
		if (dx7_32v_vox[j+1].par_val == 0)
			m++;
		if (j == 0)
			fprintf(f, "\top%d\n", opct--);
		else
			fprintf(f, "\n");
	    }
	}
	val = ch[DX7COMLEN + 6];
	fprintf(f, "  %2x\t; check-sum=%d", val, val);
	check_sum = 0x80 - (0x7f & check_sum);
	if (val == check_sum)
	    fprintf(f, "\tvalid check-sum\n");
	else
	    fprintf(f, "\tinvalid check-sum %x != %x\n", val, check_sum);
}
