#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>
#define loop(i,j) for(i=0; i<j; i++)

da_vdx7(dx)
	struct dx7 *dx;
/*
 * Print a disassembly of dx7 state on 'stdout', including:
 * all keys with velocity > 0 (keys which are "on");
 * control parameters;
 * function parameters;
 * number of sounding voices (determined from keys down);
 * current program index;
 * pitch bend.
 */
{
	register i;

	printf("cmd[ind]=\t%-10s%-10s%-10s\n", "value", "u_bound", "name");

	loop(i,DX7KBDLEN)
		if (dx->dx7_kbd_val[i])
			printf("kbd[%3d]=\t%-10d%-10d%s\n", 
				i, dx->dx7_kbd_val[i], 127, "kbd & velocity");
	loop(i,dx7_rcv_ctl_len)
		printf("ctl[%3d]=\t%-10d%-10d%s\n", 
			dx7_rcv_ctl[i].par_index,
			dx->dx7_ctl_val[dx7_rcv_ctl[i].par_index],
			dx7_rcv_ctl[i].par_ub,
			dx7_rcv_ctl[i].par_name);
	loop(i,dx7_ini_fun_len)
		printf("fun[%3d]=\t%-10d%-10d%s\n", 
			dx7_ini_fun[i].par_index,
			dx->dx7_fun_val[dx7_ini_fun[i].par_index],
			dx7_ini_fun[i].par_ub,
			dx7_ini_fun[i].par_name);
	printf("nsv    =\t%-20d%s\n", dx->dx7_nsv, "number of sounding voices");
	printf("prg    =\t%-20d%s\n", dx->dx7_prg_val, "program");
	printf("pbd    =\t%-20d%s\n", dx->dx7_pbd_val, "pitch bend");
}
