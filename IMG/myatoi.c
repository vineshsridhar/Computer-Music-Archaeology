/* Berzerkeley atoi.c hacked to recognize octal & hex - psl 10/88
** 0*[xX] => hex, else 0* => octal, else decimal.
** Decimal numbers may be signed, octal and hex may not.
*/

int myatoi(cp)
register char *cp;
{
	register int n, f, base;

	base = 10;
	for (f = 0; *cp == ' ' || *cp == '\t' || *cp == '0'; )
	    if (*cp++ == '0')
		base = 8;
	if (base == 8 && (*cp == 'x' || *cp == 'X')) {
	    base = 16;
	    cp++;
	} else if (*cp == '-')
	    f = *cp++;
	else if (*cp == '+')
	    cp++;
	n = 0;
	if (base == 16) {
	    while (('0' <= *cp && *cp <= '9')
	     || ('A' <= *cp && *cp <= 'F')
	     || ('a' <= *cp && *cp <= 'f')) {
		if ('0' <= *cp && *cp <= '9')
		    n = (n << 4) + *cp++ - '0';
		else if ('A' <= *cp && *cp <= 'F')
		    n = (n << 4) + 10 + *cp++ - 'A';
		else
		    n = (n << 4) + 10 + *cp++ - 'a';
	    }
	} else
	    while (*cp >= '0' && *cp <= '9')
		n = n * base + *cp++ - '0';
	return (f? -n : n);
}
