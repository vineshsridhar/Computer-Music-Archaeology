#include	"audio.h"

/* unsigned 13-bit binary to 8-bit ISDN */
uchar	Ub2i[8192]	= {
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
	  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
	  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,  2,
	  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,
	  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  3,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,  4,
	  4,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,
	  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,  6,
	  6,  6,  6,  6,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,  7,
	  7,  7,  7,  7,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,  8,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,  9,
	  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10, 10,
	 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	 15, 15, 15, 15, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
	 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
	 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18, 18,
	 18, 18, 18, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,
	 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	 20, 20, 20, 20, 20, 20, 20, 20, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21,
	 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22,
	 22, 22, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23, 23,
	 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26,
	 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28, 28, 28, 28, 28, 28,
	 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29,
	 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 30,
	 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
	 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30,
	 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 30, 31,
	 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
	 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31,
	 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 32, 32, 32, 32,
	 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
	 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32, 32,
	 32, 32, 32, 32, 32, 32, 32, 32, 33, 33, 33, 33, 33, 33, 33, 33,
	 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
	 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33, 33,
	 33, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
	 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34, 34,
	 34, 34, 34, 34, 34, 34, 34, 34, 34, 35, 35, 35, 35, 35, 35, 35,
	 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
	 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35, 35,
	 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
	 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36, 36,
	 36, 36, 36, 36, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
	 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37, 37,
	 37, 37, 37, 37, 37, 37, 37, 38, 38, 38, 38, 38, 38, 38, 38, 38,
	 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38, 38,
	 38, 38, 38, 38, 38, 38, 38, 38, 38, 39, 39, 39, 39, 39, 39, 39,
	 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39, 39,
	 39, 39, 39, 39, 39, 39, 39, 39, 39, 40, 40, 40, 40, 40, 40, 40,
	 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40, 40,
	 40, 40, 40, 40, 40, 40, 40, 40, 41, 41, 41, 41, 41, 41, 41, 41,
	 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41, 41,
	 41, 41, 41, 41, 41, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42, 42,
	 42, 42, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43,
	 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 43, 44, 44, 44,
	 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44, 44,
	 44, 44, 44, 44, 44, 44, 44, 45, 45, 45, 45, 45, 45, 45, 45, 45,
	 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45, 45,
	 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46, 46,
	 46, 46, 46, 46, 46, 46, 46, 47, 47, 47, 47, 47, 47, 47, 47, 47,
	 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 47, 48, 48,
	 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48, 48,
	 48, 48, 48, 48, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49, 49,
	 49, 49, 49, 49, 49, 49, 49, 49, 49, 50, 50, 50, 50, 50, 50, 50,
	 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 50, 51, 51, 51,
	 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51, 51,
	 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52, 52,
	 52, 52, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53, 53,
	 53, 53, 53, 53, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54, 54,
	 54, 54, 54, 54, 54, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55, 55,
	 55, 55, 55, 55, 55, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56, 56,
	 56, 56, 56, 56, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57, 57,
	 57, 57, 57, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58, 58,
	 58, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 60, 60,
	 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 60, 61, 61, 61, 61, 61,
	 61, 61, 61, 61, 61, 61, 61, 61, 62, 62, 62, 62, 62, 62, 62, 62,
	 62, 62, 62, 62, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 63, 64,
	 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 65, 65, 65, 65, 65, 65,
	 65, 65, 65, 65, 66, 66, 66, 66, 66, 66, 66, 66, 66, 66, 67, 67,
	 67, 67, 67, 67, 67, 67, 67, 67, 68, 68, 68, 68, 68, 68, 68, 68,
	 68, 69, 69, 69, 69, 69, 69, 69, 69, 69, 70, 70, 70, 70, 70, 70,
	 70, 70, 71, 71, 71, 71, 71, 71, 71, 71, 72, 72, 72, 72, 72, 72,
	 72, 72, 73, 73, 73, 73, 73, 73, 73, 74, 74, 74, 74, 74, 74, 74,
	 75, 75, 75, 75, 75, 75, 75, 76, 76, 76, 76, 76, 76, 76, 77, 77,
	 77, 77, 77, 77, 78, 78, 78, 78, 78, 78, 79, 79, 79, 79, 79, 79,
	 80, 80, 80, 80, 80, 81, 81, 81, 81, 81, 82, 82, 82, 82, 82, 83,
	 83, 83, 83, 83, 84, 84, 84, 84, 84, 85, 85, 85, 85, 86, 86, 86,
	 86, 87, 87, 87, 87, 88, 88, 88, 88, 89, 89, 89, 89, 90, 90, 90,
	 91, 91, 91, 91, 92, 92, 92, 93, 93, 93, 94, 94, 94, 95, 95, 95,
	 96, 96, 96, 97, 97, 98, 98, 98, 99, 99,100,100,101,101,101,102,
	102,103,103,104,104,105,106,106,107,107,108,109,109,110,110,111,
	112,113,113,114,115,116,117,118,119,120,121,122,123,124,125,127,
	255,253,252,251,250,249,248,247,246,245,244,243,242,241,241,240,
	239,238,238,237,237,236,235,235,234,234,233,232,232,231,231,230,
	230,229,229,229,228,228,227,227,226,226,226,225,225,224,224,224,
	223,223,223,222,222,222,221,221,221,220,220,220,219,219,219,219,
	218,218,218,217,217,217,217,216,216,216,216,215,215,215,215,214,
	214,214,214,213,213,213,213,212,212,212,212,212,211,211,211,211,
	211,210,210,210,210,210,209,209,209,209,209,208,208,208,208,208,
	207,207,207,207,207,207,206,206,206,206,206,206,205,205,205,205,
	205,205,204,204,204,204,204,204,204,203,203,203,203,203,203,203,
	202,202,202,202,202,202,202,201,201,201,201,201,201,201,200,200,
	200,200,200,200,200,200,199,199,199,199,199,199,199,199,198,198,
	198,198,198,198,198,198,197,197,197,197,197,197,197,197,197,196,
	196,196,196,196,196,196,196,196,195,195,195,195,195,195,195,195,
	195,195,194,194,194,194,194,194,194,194,194,194,193,193,193,193,
	193,193,193,193,193,193,192,192,192,192,192,192,192,192,192,192,
	192,191,191,191,191,191,191,191,191,191,191,191,190,190,190,190,
	190,190,190,190,190,190,190,190,189,189,189,189,189,189,189,189,
	189,189,189,189,189,188,188,188,188,188,188,188,188,188,188,188,
	188,188,187,187,187,187,187,187,187,187,187,187,187,187,187,186,
	186,186,186,186,186,186,186,186,186,186,186,186,186,185,185,185,
	185,185,185,185,185,185,185,185,185,185,185,185,184,184,184,184,
	184,184,184,184,184,184,184,184,184,184,184,183,183,183,183,183,
	183,183,183,183,183,183,183,183,183,183,183,182,182,182,182,182,
	182,182,182,182,182,182,182,182,182,182,182,182,181,181,181,181,
	181,181,181,181,181,181,181,181,181,181,181,181,181,181,180,180,
	180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,
	179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,
	179,179,179,178,178,178,178,178,178,178,178,178,178,178,178,178,
	178,178,178,178,178,178,178,177,177,177,177,177,177,177,177,177,
	177,177,177,177,177,177,177,177,177,177,177,177,176,176,176,176,
	176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,
	176,176,175,175,175,175,175,175,175,175,175,175,175,175,175,175,
	175,175,175,175,175,175,175,175,175,174,174,174,174,174,174,174,
	174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,
	173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,173,
	173,173,173,173,173,173,173,173,173,172,172,172,172,172,172,172,
	172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,172,
	172,172,172,171,171,171,171,171,171,171,171,171,171,171,171,171,
	171,171,171,171,171,171,171,171,171,171,171,171,171,171,170,170,
	170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,
	170,170,170,170,170,170,170,170,170,170,170,169,169,169,169,169,
	169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,169,
	169,169,169,169,169,169,169,169,168,168,168,168,168,168,168,168,
	168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,168,
	168,168,168,168,168,168,168,167,167,167,167,167,167,167,167,167,
	167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,
	167,167,167,167,167,167,167,166,166,166,166,166,166,166,166,166,
	166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,
	166,166,166,166,166,166,166,166,166,165,165,165,165,165,165,165,
	165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,165,
	165,165,165,165,165,165,165,165,165,165,165,165,164,164,164,164,
	164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
	164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,
	163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,
	163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,
	163,163,163,163,163,163,163,162,162,162,162,162,162,162,162,162,
	162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,
	162,162,162,162,162,162,162,162,162,162,162,162,162,162,162,161,
	161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
	161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,161,
	161,161,161,161,161,161,161,161,160,160,160,160,160,160,160,160,
	160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,
	160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,
	160,160,160,160,159,159,159,159,159,159,159,159,159,159,159,159,
	159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,
	159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,
	159,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
	158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
	158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,158,
	158,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,157,
	157,157,157,156,156,156,156,156,156,156,156,156,156,156,156,156,
	156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,
	156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,156,
	156,156,156,156,156,156,155,155,155,155,155,155,155,155,155,155,
	155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,
	155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,155,
	155,155,155,155,155,155,155,155,155,155,155,155,155,154,154,154,
	154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
	154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
	154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,154,
	154,154,154,154,154,153,153,153,153,153,153,153,153,153,153,153,
	153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
	153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
	153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,153,
	152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,
	152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,
	152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,152,
	152,152,152,152,152,152,152,152,152,152,152,152,152,152,151,151,
	151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,
	151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,
	151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,
	151,151,151,151,151,151,151,151,151,151,151,151,151,151,150,150,
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
	150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,150,
	150,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
	149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
	149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
	149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,149,
	149,149,149,149,149,149,149,149,148,148,148,148,148,148,148,148,
	148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
	148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
	148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
	148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,148,
	148,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
	147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
	147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
	147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,147,
	147,147,147,147,147,147,147,147,147,147,147,147,147,146,146,146,
	146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
	146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
	146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
	146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,
	146,146,146,146,146,146,146,146,146,146,146,146,146,145,145,145,
	145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
	145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
	145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
	145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
	145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,145,
	145,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
	144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
	144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
	144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
	144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,144,
	144,144,144,144,144,144,144,144,143,143,143,143,143,143,143,143,
	143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
	143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
	143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
	143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
	143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,
	143,143,143,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,
	142,142,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,
	141,141,141,141,141,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,
	140,140,140,140,140,140,140,140,140,140,140,140,140,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,
	139,139,139,139,139,139,139,139,139,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,
	138,138,138,138,138,138,138,138,138,138,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,
	136,136,136,136,136,136,136,136,136,136,136,136,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,
	135,135,135,135,135,135,135,135,135,135,135,135,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,
	134,134,134,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,
	133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,
	132,132,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,
	131,131,131,131,131,131,131,131,131,131,131,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,
	130,130,130,130,130,130,130,130,130,130,130,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,129,
	129,129,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
	128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,
};

/* 8-bit ISDN to unsigned 13-bit binary */
ushort	I2ub[256]	= {
	0x0058, 0x0102, 0x01a6, 0x0242, 0x02d8, 0x0367, 0x03f1, 0x0474,
	0x04f2, 0x056b, 0x05de, 0x064d, 0x06b7, 0x071c, 0x077d, 0x07db,
	0x0834, 0x0889, 0x08db, 0x0929, 0x0974, 0x09bb, 0x0a00, 0x0a42,
	0x0a81, 0x0abd, 0x0af7, 0x0b2e, 0x0b63, 0x0b96, 0x0bc7, 0x0bf5,
	0x0c22, 0x0c4c, 0x0c75, 0x0c9c, 0x0cc2, 0x0ce5, 0x0d08, 0x0d29,
	0x0d48, 0x0d66, 0x0d83, 0x0d9f, 0x0db9, 0x0dd3, 0x0deb, 0x0e02,
	0x0e19, 0x0e2e, 0x0e42, 0x0e56, 0x0e69, 0x0e7b, 0x0e8c, 0x0e9c,
	0x0eac, 0x0ebb, 0x0ec9, 0x0ed7, 0x0ee4, 0x0ef1, 0x0efd, 0x0f09,
	0x0f14, 0x0f1f, 0x0f29, 0x0f33, 0x0f3c, 0x0f45, 0x0f4e, 0x0f56,
	0x0f5e, 0x0f65, 0x0f6c, 0x0f73, 0x0f7a, 0x0f80, 0x0f86, 0x0f8c,
	0x0f92, 0x0f97, 0x0f9c, 0x0fa1, 0x0fa6, 0x0faa, 0x0faf, 0x0fb3,
	0x0fb7, 0x0fba, 0x0fbe, 0x0fc1, 0x0fc5, 0x0fc8, 0x0fcb, 0x0fce,
	0x0fd1, 0x0fd3, 0x0fd6, 0x0fd8, 0x0fdb, 0x0fdd, 0x0fdf, 0x0fe1,
	0x0fe3, 0x0fe5, 0x0fe7, 0x0fe9, 0x0fea, 0x0fec, 0x0fed, 0x0fef,
	0x0ff0, 0x0ff1, 0x0ff3, 0x0ff4, 0x0ff5, 0x0ff6, 0x0ff7, 0x0ff8,
	0x0ff9, 0x0ffa, 0x0ffb, 0x0ffc, 0x0ffd, 0x0ffe, 0x0ffe, 0x0fff,
	0x1fa7, 0x1efd, 0x1e59, 0x1dbd, 0x1d27, 0x1c98, 0x1c0e, 0x1b8b,
	0x1b0d, 0x1a94, 0x1a21, 0x19b2, 0x1948, 0x18e3, 0x1882, 0x1824,
	0x17cb, 0x1776, 0x1724, 0x16d6, 0x168b, 0x1644, 0x15ff, 0x15bd,
	0x157e, 0x1542, 0x1508, 0x14d1, 0x149c, 0x1469, 0x1438, 0x140a,
	0x13dd, 0x13b3, 0x138a, 0x1363, 0x133d, 0x131a, 0x12f7, 0x12d6,
	0x12b7, 0x1299, 0x127c, 0x1260, 0x1246, 0x122c, 0x1214, 0x11fd,
	0x11e6, 0x11d1, 0x11bd, 0x11a9, 0x1196, 0x1184, 0x1173, 0x1163,
	0x1153, 0x1144, 0x1136, 0x1128, 0x111b, 0x110e, 0x1102, 0x10f6,
	0x10eb, 0x10e0, 0x10d6, 0x10cc, 0x10c3, 0x10ba, 0x10b1, 0x10a9,
	0x10a1, 0x109a, 0x1093, 0x108c, 0x1085, 0x107f, 0x1079, 0x1073,
	0x106d, 0x1068, 0x1063, 0x105e, 0x1059, 0x1055, 0x1050, 0x104c,
	0x1048, 0x1045, 0x1041, 0x103e, 0x103a, 0x1037, 0x1034, 0x1031,
	0x102e, 0x102c, 0x1029, 0x1027, 0x1024, 0x1022, 0x1020, 0x101e,
	0x101c, 0x101a, 0x1018, 0x1016, 0x1015, 0x1013, 0x1012, 0x1010,
	0x100f, 0x100e, 0x100c, 0x100b, 0x100a, 0x1009, 0x1008, 0x1007,
	0x1006, 0x1005, 0x1004, 0x1003, 0x1002, 0x1001, 0x1001, 0x1000,
};

/* signed 13-bit binary to 8-bit ISDN */
uchar	*Sb2i	= &Ub2i[B13ZERO];

/* 8-bit ISDN to signed 13-bit binary */
short	I2sb[256]	= {
	-4007, -3837, -3673, -3517, -3367, -3224, -3086, -2955,
	-2829, -2708, -2593, -2482, -2376, -2275, -2178, -2084,
	-1995, -1910, -1828, -1750, -1675, -1604, -1535, -1469,
	-1406, -1346, -1288, -1233, -1180, -1129, -1080, -1034,
	 -989,  -947,  -906,  -867,  -829,  -794,  -759,  -726,
	 -695,  -665,  -636,  -608,  -582,  -556,  -532,  -509,
	 -486,  -465,  -445,  -425,  -406,  -388,  -371,  -355,
	 -339,  -324,  -310,  -296,  -283,  -270,  -258,  -246,
	 -235,  -224,  -214,  -204,  -195,  -186,  -177,  -169,
	 -161,  -154,  -147,  -140,  -133,  -127,  -121,  -115,
	 -109,  -104,   -99,   -94,   -89,   -85,   -80,   -76,
	  -72,   -69,   -65,   -62,   -58,   -55,   -52,   -49,
	  -46,   -44,   -41,   -39,   -36,   -34,   -32,   -30,
	  -28,   -26,   -24,   -22,   -21,   -19,   -18,   -16,
	  -15,   -14,   -12,   -11,   -10,    -9,    -8,    -7,
	   -6,    -5,    -4,    -3,    -2,    -1,    -1,     0,
	 4007,  3837,  3673,  3517,  3367,  3224,  3086,  2955,
	 2829,  2708,  2593,  2482,  2376,  2275,  2178,  2084,
	 1995,  1910,  1828,  1750,  1675,  1604,  1535,  1469,
	 1406,  1346,  1288,  1233,  1180,  1129,  1080,  1034,
	  989,   947,   906,   867,   829,   794,   759,   726,
	  695,   665,   636,   608,   582,   556,   532,   509,
	  486,   465,   445,   425,   406,   388,   371,   355,
	  339,   324,   310,   296,   283,   270,   258,   246,
	  235,   224,   214,   204,   195,   186,   177,   169,
	  161,   154,   147,   140,   133,   127,   121,   115,
	  109,   104,    99,    94,    89,    85,    80,    76,
	   72,    69,    65,    62,    58,    55,    52,    49,
	   46,    44,    41,    39,    36,    34,    32,    30,
	   28,    26,    24,    22,    21,    19,    18,    16,
	   15,    14,    12,    11,    10,     9,     8,     7,
	    6,     5,     4,     3,     2,     1,     1,     0,
};
