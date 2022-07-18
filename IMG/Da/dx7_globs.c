#include <sys/types.h>
#include <libmidi.h>

/*
 * dx7 voice initialization parameters
 * note: table contains only one copy of operator parameter information
 * 	First operator is number 6.
 */

struct midi_par dx7_ini_vox[] = {
/* 	index	value	range	name */
	{0,	0,	99,	"op amp rate[1]"},	/* operator 6 */
	{1,	0,	99,	"op amp rate[2]"},
	{2,	0,	99,	"op amp rate[3]"},
	{3,	0,	99,	"op amp rate[4]"},
	{4,	0,	99,	"op amp level[1]"},
	{5,	0,	99,	"op amp level[2]"},
	{6,	0,	99,	"op amp level[3]"},
	{7,	0,	99,	"op amp level[4]"},
	{8,	0,	99,	"kls breakpoint"},
	{9,	0,	99,	"kls left depth"},
	{10,	0,	99,	"kls right depth"},
	{11,	0,	3,	"kls left curve"},
	{12,	0,	3,	"kls right curve"},
	{13,	0,	7,	"op kbd rate scaling"},
	{14,	0,	3,	"op mod sensitivity amp"},
	{15,	0,	7,	"op key velocity sensitivity"},
	{16,	0,	99,	"op output level"},
	{17,	0,	1,	"op osc mode"},
	{18,	0,	31,	"op osc frq coarse"},
	{19,	0,	99,	"op osc frq fine"},
	{20,	0,	14,	"op detune"},
	/* parameters for indicies 21 to 125 are operators 5 through 1 */
	{126,	0,	99,	"vox pitch rate[1]"},
	{127,	0,	99,	"vox pitch rate[2]"},
	{128,	0,	99,	"vox pitch rate[3]"},
	{129,	0,	99,	"vox pitch rate[4]"},
	{130,	0,	99,	"vox pitch level[1]"},
	{131,	0,	99,	"vox pitch level[2]"},
	{132,	0,	99,	"vox pitch level[3]"},
	{133,	0,	99,	"vox pitch level[4]"},
	{134,	0,	31,	"vox algorithm"},
	{135,	0,	7,	"vox feedback"},
	{136,	0,	1,	"vox osc sync"},
	{137,	0,	99,	"lfo speed"},
	{138,	0,	99,	"lfo delay"},
	{139,	0,	99,	"lfo pmd"},
	{140,	0,	99,	"lfo amd"},
	{141,	0,	1,	"lfo sync"},
	{142,	0,	1,	"lfo wave"},
	{143,	0,	7,	"vox mod sensitivity pitch"},
	{144,	0,	48,	"vox transpose"},
	{145,	0,	0xFF,	"vox voice name[0]"},
	{146,	0,	0xFF,	"vox voice name[1]"},
	{147,	0,	0xFF,	"vox voice name[2]"},
	{148,	0,	0xFF,	"vox voice name[3]"},
	{149,	0,	0xFF,	"vox voice name[4]"},
	{150,	0,	0xFF,	"vox voice name[5]"},
	{151,	0,	0xFF,	"vox voice name[6]"},
	{152,	0,	0xFF,	"vox voice name[7]"},
	{153,	0,	0xFF,	"vox voice name[8]"},
	{154,	0,	0xFF,	"vox voice name[9]"},
	{155,	0,	0x3F,	"vox op ena"}
};
/* length of dx7_ini_vox table follows */
u_short	dx7_ini_vox_len = sizeof(dx7_ini_vox) / sizeof(struct midi_par);


/*
 * dx7 function initialization parameters
 * note: indicies 0..63, 77..127 undefined
 */

struct	midi_par dx7_ini_fun[] = {
	{64,	0,	1,	"fun mono poly"},
	{65,	0,	12,	"fun pitch bend range"},
	{66,	0,	12,	"fun pitch bend step"},
	{67,	0,	1,	"fun porta mode"},
	{68,	0,	1,	"fun porta gliss"},
	{69,	0,	99,	"fun porta time"},
	{70,	0,	99,	"fun mod wheel range"},
	{71,	0,	7,	"fun mod wheel assign"},
	{72,	0,	99,	"fun foot range"},
	{73,	0,	7,	"fun foot assign"},
	{74,	0,	99,	"fun breath range"},
	{75,	0,	7,	"fun breath assign"},
	{76,	0,	99,	"fun after touch range"},
	{77,	0,	7,	"fun after touch assign"}
};
/* length of dx7_ini_fun table follows */
u_short	dx7_ini_fun_len = sizeof(dx7_ini_fun) / sizeof(struct midi_par);

/*
 * dx7 channel command table
 */

struct midi_par dx7_ch_info[] = {
/*	command		value	max	name
	{CH_KEY_OFF,	0,	127,	"key off"},
	{CH_KEY_ON,	0,	127,	"key on"},
	{CH_CTL,	0,	127,	"control"},
	{CH_PRG,	0,	31,	"program"},
	/* pitch bend normal value is 64 */
	{CH_P_BEND,	64,	127,	"pitch bender"},
};
/* length of dx7_ch_info table follows */
u_short	dx7_ch_info_len = sizeof(dx7_ch_info) / sizeof(struct midi_par);

/*
 * dx7 transmit control command table
 * note: the dx7 can transmit these control commands via its midi
 */

struct	midi_par	dx7_xmit_ctl[] = {
/*	index	val,	max,	name */
	{1,	0,	127,	"modulation wheel"},
	{2,	0,	127,	"breath controller"},
	{3,	0,	127,	"after touch"},
	{4,	0,	127,	"foot controller"},
	{6,	0,	127,	"data entry knob"},
	{64,	0,	127,	"sustain foot switch"},
	{65,	0,	127,	"portamento foot switch"},
	{96,	0,	127,	"data entry +1"},
	{97,	0,	127,	"data entry -1"}
};
/* length of dx7_xmit_ctl table follows */
u_short	dx7_xmit_ctl_len = sizeof(dx7_xmit_ctl) / sizeof(struct midi_par);

/*
 * dx7 receive control command table
 * note: dx7 can receive these control commands via its midi
 */

struct	midi_par	dx7_rcv_ctl[] = {
	{1,	0,	127,	"modulation wheel"},
	{2,	0,	127,	"breath controller"},
	{3,	0,	127,	"after touch"},
	{4,	0,	127,	"foot controller"},
	{5,	0,	127,	"portamento time"},
	{6,	0,	127,	"data entry knob, master tune"},
	{7,	0,	127,	"volume (LS 4b ignored)"},
	{64,	0,	127,	"sustain foot switch"},
	{65,	0,	127,	"portamento foot switch"},
	{96,	0,	127,	"data entry +1"},
	{97,	0,	127,	"data entry -1"},
	{125,	0,	127,	"omni all key off (ignored)"},
	{126,	0,	127,	"mono all key off (ignored)"},
	{127,	0,	127,	"poly all key off (ignored)"}
};
/* length of dx7_rcv_ctl table follows */
u_short	dx7_rcv_ctl_len = sizeof(dx7_rcv_ctl) / sizeof(struct midi_par);

/*
 * dx7 32-voice initialization parameters
 * note: table contains only one copy of voice template and
 * operator parameter information.
 * 	First voice is number 1.
 * 	First operator is number 6.
 *	Note: midi_par.par_val holds the base of the field in the byte
 *	and  midi_par.par_ub holds a mask value.
 */

struct midi_par dx7_32v_vox[] = {
/* 	index	base	mask.	name */
	{0,	0,	0x7f,	"op amp rate[1]"},	/* operator 6 */
	{1,	0,	0x7f,	"op amp rate[2]"},
	{2,	0,	0x7f,	"op amp rate[3]"},
	{3,	0,	0x7f,	"op amp rate[4]"},
	{4,	0,	0x7f,	"op amp level[1]"},
	{5,	0,	0x7f,	"op amp level[2]"},
	{6,	0,	0x7f,	"op amp level[3]"},
	{7,	0,	0x7f,	"op amp level[4]"},
	{8,	0,	0x7f,	"kls breakpoint"},
	{9,	0,	0x7f,	"kls left depth"},
	{10,	0,	0x7f,	"kls right depth"},
	{11,	0,	0x3,	"kls left curve"},
	{11,	2,	0x3,	"kls right curve"},
	{12,	0,	0x7,	"op kbd rate scaling"},
	{12,	3,	0xf,	"op detune"},
	{13,	0,	0x3,	"op mod sensitivity amp"},
	{13,	2,	0x7,	"op key velocity sensitivity"},
	{14,	0,	0x7f,	"op output level"},
	{15,	0,	0x1,	"op osc mode"},
	{15,	1,	0x1f,	"op osc frq coarse"},
	{16,	0,	0x7f,	"op osc frq fine"},
	/* parameters for indicies 17 to 101 are operators 5 through 1 */
	{102,	0,	0x7f,	"vox pitch rate[1]"},
	{103,	0,	0x7f,	"vox pitch rate[2]"},
	{104,	0,	0x7f,	"vox pitch rate[3]"},
	{105,	0,	0x7f,	"vox pitch rate[4]"},
	{106,	0,	0x7f,	"vox pitch level[1]"},
	{107,	0,	0x7f,	"vox pitch level[2]"},
	{108,	0,	0x7f,	"vox pitch level[3]"},
	{109,	0,	0x7f,	"vox pitch level[4]"},
	{110,	0,	0x1f,	"vox algorithm"},
	{111,	0,	0x7,	"vox feedback"},
	{111,	3,	0x1,	"vox osc sync"},
	{112,	0,	0x7f,	"lfo speed"},
	{113,	0,	0x7f,	"lfo delay"},
	{114,	0,	0x7f,	"lfo pmd"},
	{115,	0,	0x7f,	"lfo amd"},
	{116,	0,	0x1,	"lfo sync"},
	{116,	1,	0x7,	"lfo wave"},
	{116,	4,	0x7,	"vox mod sensitivity pitch"},
	{117,	0,	0x7f,	"vox transpose"},
	{118,	0,	0xff,	"vox voice name[0]"},
	{119,	0,	0xff,	"vox voice name[1]"},
	{120,	0,	0xff,	"vox voice name[2]"},
	{121,	0,	0xff,	"vox voice name[3]"},
	{122,	0,	0xff,	"vox voice name[4]"},
	{123,	0,	0xff,	"vox voice name[5]"},
	{124,	0,	0xff,	"vox voice name[6]"},
	{125,	0,	0xff,	"vox voice name[7]"},
	{126,	0,	0xff,	"vox voice name[8]"},
	{127,	0,	0xff,	"vox voice name[9]"}
};
/* length of dx7_32v_vox table follows */
u_short	dx7_32v_vox_len = sizeof(dx7_32v_vox) / sizeof(struct midi_par);

