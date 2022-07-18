#include <stdio.h>

MidiError(fmt, ...)
	char *fmt;
/*
** fprintf(stderr,fmt,arg)
*/
{
    va_list ap;
//	_doprnt(fmt, &arg, stderr);
    fvprintf(stderr, fmt, ap);
}

MidiExit(code,fmt, ...)
	char *fmt;
{
    va_list ap;
//	_doprnt(fmt,arg,stderr);
    fvprintf(stderr, fmt, ap);
	exit(code);
}
