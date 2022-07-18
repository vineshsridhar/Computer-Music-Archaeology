/*
**	Routines to convert from decibels to ratios.	psl 10/89
**	    double db2d(db)		convert to a (double) gain ratio
**		double db;		positive => gain, negative => loss
**
**	    int db2fp(db)		convert to a fixed point ratio
**		double db;		(see defines in audio.h)
**
*/
#include	"audio.h"

extern double pow();

double
db2d(db)
double	db;
{
	return(pow(10., db / 10.));
}

db2fp(db)
double	db;
{
	return(D2FP(pow(10., db / 10.)));
}

