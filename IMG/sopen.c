#include <stdio.h>

#ifndef _NFILE
#define	_NFILE	30
#endif
static int IsPipe[_NFILE];	/* true if FILE f is a pipe (not a file) */
#define sp(c) (c == ' ' || c == '\t')
#define skipspace(s) while sp(*s) ++s;

FILE *
sopen(name, mode)
	char *name, *mode;
/*
** Open 'name', which is a file or pipe, in the given mode,
** and return a pointer to the opened stream, or NULL on error.
** If 'name' begins with a bar symbol '|' it is taken to be
** a `pipe`, and 'popen()' is used.  Otherwise it is taken to
** be a file, and 'fopen()' is used.  If 'mode' is NULL
** "r" is used by default.  The stream should be close using 'sclose()'.
*/
{
	FILE *f, *fopen(), *popen();
	int p = 0;

	if (!mode || !*mode)
		mode = "r";
	skipspace(name);
	if (*name == '|') {
		++name;
		skipspace(name);
		if (f = popen(name,mode))
			p = 1;
	} else
		f = fopen(name,mode);
	if (f) IsPipe[fileno(f)] = p;
	return f;
}

int sclose(f)
	FILE *f;
/*
** Close stream 'f', which was opened with 'sopen()'.
*/
{
	return (f && IsPipe[fileno(f)])?
		IsPipe[fileno(f)]=0, pclose(f) : fclose(f);
}

isapipe(f)
/*
** True if 'f' is a i{pipe} (not a plain i{file}).
** i{N.B.}- 'f' is an integer file descriptor, not a 'FILE' pointer.
*/
{
	return IsPipe[f];
}
