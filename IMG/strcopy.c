/*
**	STRCOPY, SCOPY -- Copy text string & return pointer to NUL
**	psl ?/74
*/

char	*
strcopy(t, f)
register char *t, *f;
{
	while (*t++ = *f++);
	return(--t);
}

char	*
scopy(f, t)
register char *f, *t;
{
	while (*t++ = *f++);
	return(--t);
}
