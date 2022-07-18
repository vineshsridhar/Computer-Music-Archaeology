# create accacc.cc - figured bass chords based on C
BEGIN	{
	n["Cb"] = -1; n["C"] =  0; n["C#"] =  1
	n["Db"] =  1; n["D"] =  2; n["D#"] =  3
	n["Eb"] =  3; n["E"] =  4; n["E#"] =  5
	n["Fb"] =  4; n["F"] =  5; n["F#"] =  6
	n["Gb"] =  6; n["G"] =  7; n["G#"] =  8
	n["Ab"] =  8; n["A"] =  9; n["A#"] = 10
	n["Bb"] = 10; n["B"] = 11; n["B#"] = 12
	c[0] = "C"
	c[1] = "Db"; a[1] = "C#"
	c[2] = "D"
	c[3] = "Eb"; a[3] = "D#"
	c[4] = "E"
	c[5] = "F"
	c[6] = "Gb"; a[6] = "F#"
	c[7] = "G"
	c[8] = "Ab"; a[8] = "G#"
	c[9] = "A"
	c[10] = "Bb"; a[10] = "A#"
	c[11] = "B"
#							   C D EF G A B
# 3-note						   1   3  5    
# 4-note						   1   3  5  7 
	typ[ 0]  = "";		grp[ 0] = 1;	trn[ 0] = "000000000000"
	typ[ 1]  = "m";		grp[ 1] = 1;	trn[ 1] = "0000a0000a00"
	typ[ 2]  = "6";		grp[ 2] = 1;	trn[ 2] = "443334455553"
	typ[ 3]  = "m6";	grp[ 3] = 1;	trn[ 3] = "333333355553"
	typ[ 4]  = "64";	grp[ 4] = 1;	trn[ 4] = "777789999886"
	typ[ 5]  = "m64";	grp[ 5] = 1;	trn[ 5] = "776689988886"
	typ[ 6]  = "7";		grp[ 6] = 2;	trn[ 6] = "000000000000"
	typ[ 7]  = "65";	grp[ 7] = 2;	trn[ 7] = "443334433322"
	typ[ 8]  = "43";	grp[ 8] = 2;	trn[ 8] = "777766655566"
	typ[ 9]  = "42";	grp[ 9] = 2;	trn[ 9] = "AAAA89999999"
	typ[10]  = "o";		grp[10] = 2;	trn[10] = "0000a00a00a0"
	typ[11] = "/"
	codes = "lkjihgfedcba0123456789ABC"; off = 13
	state = 0
	print "# \"Classical\" (figured bass) chords for acca based on C"
	print "#     name    group    trans"
	for (pc = 0; pc < 12; pc++) {
		for (i = 0; typ[i] != "/"; i++) {
			l = sprintf "%s\t%d\t", typ[i], grp[i]
			p = pc
			if (p + index(codes, substr(trn[i], 1, 1)) - off > 7)
				p = p - 12
			for (j = 1; j <= 12; j++) {
				tm = substr(trn[i], j, 1)
				if ((t = index(codes, tm) - off) == - off)
					print "Error in: " typ[i] " " trn[i]
				sep = ","
				if (j == 12)
					sep = "\n"
				x = sprintf "%d%s", p + t, sep
				l = l x
			}
			if (a[pc] != "")
				printf "#CHORD\t%s%s", a[pc], l
			printf "#CHORD\t%s%s", c[pc], l
		}
	}
	exit
}
