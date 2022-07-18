#define Case break; case
#define Default break; default
#define Int register int
#define Char register char
static char *__arg, *__argp; /* use by 'for_each_argument */
static char *av0;	/* will hold the name of the command */
#define argument  (__arg=(*__argp? __argp : av[++i==ac? --i : i]),__argp+=strlen(__argp), __arg)
#define for_each_argument av0 = av[0]; for (i=1;i<ac && *av[i]=='-';i++)\
			for (__argp = &av[i][1]; *__argp;)\
				switch(*__argp++)
#ifndef Alloc
#define Alloc(x) (x *)malloc(sizeof(x))
#endif
#define loop(i,j) for(i=0;i<j;i++)
#define Type typedef struct
#ifndef u_char
#define u_char	unsigned char
#define u_short unsigned short
#endif
#ifdef	FILE
FILE *sopen(), *OpenTune();
#endif
#define TOGGLE(X)		((X)^=1)
#define INRANGE(L,X,H)	((L)<=(X)&&(X)<=(H))
