#define SizeofDx7Voice	155	/* 155 bytes in a Dx7Voice */

typedef struct {
	u_char
	     rate[4],		/* 0 ~ 99 */
	     level[4],		/* 0 ~ 99 */
	     kbdBreakPoint,
	     kbdLDepth,
	     kbdRDepth,
	     kbdLCurve,		/* 0 ~ 3 */
	     kbdRCurve,
	     kbdRateScale,	/* 0 ~ 7 */
	     modSensitivity,	/* 0 ~ 3 */
	     keyVelSensitivity,	/* 0 ~ 7 */
	     outputLevel,	/* 0 ~ 99 */
	     oscMode,		/* 0 ~ 1 */
	     oscFreqCoarse,	/* 0 ~ 31 */
	     oscFreqFine,	/* 0 ~ 99 */
	     oscDetune;		/* 0 ~ 14 */
} Dx7Operator;

typedef struct {
	char op[6][21];	/* data for 6 operators; cast each as a Dx7Operator */
	char pitchRate[4],
	     pitchLevel[4],
	     algorithm,		/* 0 ~ 31 */
	     feedback,		/* 0 ~ 7 */
	     oscSync,		/* 0 ~ 1 */
	     lfoSpeed,		/* 0 ~ 99 */
	     lfoDelay,
	     lfoPmd,
	     lfoAmd,
	     lfoSync,		/* 0 ~ 1 */
	     lfoWave,		/* 0 ~ 4 */
	     modSensitivityPitch,	/* 0 ~ 7 */
	     transpose,		/* 0 ~ 48 */
	     name[11];		/* NB- leave room for \0 */
} Dx7Voice;

/*
typedef struct { char range, assign; } Dx7Slider;

typedef struct {
	char monoPoly,
	     pitchBendRange,
	     pitchBendStep,
	     portMode,
	     portGliss,
	     portTime;
	Dx7Slider mod,
	          foot,
	          breath,
	          afterTouch;
} Dx7Function;
*/
