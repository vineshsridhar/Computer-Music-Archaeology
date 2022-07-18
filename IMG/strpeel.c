/*
**	STRPEEL -- isolate delimited "word" and return pointer to next.
**	'dels' is a null-terminated string of delimiters;
**	if 'del' == (char *) 0, delimiters are chars <= ' '.
**	Returned pointer stops at <NUL> (e.g. when nothing left in string).
**	Typical usage:
**		register char *bp, *cp;
**		for (bp = buf; *bp; ) {
**		    bp = strpeel(cp = bp, 0);
**				cp points to <NUL> terminated word
**				bp points to unterminated next word
**		}
**	SPEEL -- return pointer to isolated, delimited "word" and set pointer
**	to next.  'dels' is a null-terminated string of delimiters;
**	if 'del' == (char *) 0, delimiters are chars <= ' '.
**	Returned pointer stops at <NUL>.
**	Typical usage:
**		register char *cp;
**		char *bp;
**		for (bp = buf; *(cp = speel(&bp, 0)); ) {
**				cp points to <NUL> terminated word
**				bp points to unterminated next word
**		}
**	psl 1/89
*/

static int isdel(char c, char *dels);

char	*
strpeel(sp, dels)
char	*sp, *dels;
{
	for (; *sp && !isdel(*sp, dels); sp++);
	if (*sp != '\0')	
	    for (*sp++ = '\0'; *sp && isdel(*sp, dels); sp++);
	return(sp);
}

char	*
speel(spp, dels)
char	**spp, *dels;
{
	register char *bp, *sp;

	for (sp = bp = *spp; *sp && !isdel(*sp, dels); sp++);
	if (*sp != '\0')	
	    for (*sp++ = '\0'; *sp && isdel(*sp, dels); sp++);
	*spp = sp;
	return(bp);
}

static
isdel(c, dels)
register char	c, *dels;
{
	if (dels) {
	    for (; *dels && c != *dels; dels++);
	    return(c == *dels);		/* <NUL> is always a delimiter */
	}
	return(c <= ' ');
}


