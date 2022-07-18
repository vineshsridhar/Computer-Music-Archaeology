#include <stdio.h>
#include <sys/types.h>
#include <libmidi.h>
#include <libdx7.h>

char *
dx7ccname(index)
	int index;	/* index number of dx7 midi channel-type controller */
/*
 * Get dx7 channel-type control name from 'index',
 * return static string name of controller.
 */
{
    register int i;

    for (i = 0; i < dx7_rcv_ctl_len; i++)
	if (dx7_rcv_ctl[i].par_index == index)
		return dx7_rcv_ctl[i].par_name;
    return NULL;
}
