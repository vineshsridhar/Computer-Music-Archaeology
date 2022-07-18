# create accagc.cc - Guitar-style chords based on C
BEGIN	{
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
	norm = "=-0+#"; noff = 3
	xtra = "lkjihgfedcba.123456789ABC"; xoff = 13
#							   C D EF G A B
	typ[0]  = "";		grp[ 0] = 1;	trn[ 0] = "000000000000"
	typ[1]  = "m";		grp[ 1] = 1;	trn[ 1] = "0000-0000-00"
	typ[2]  = "7";		grp[ 2] = 2;	trn[ 2] = "000000000000"
	typ[3]  = "M7";		grp[ 3] = 2;	trn[ 3] = "0000000000+0"
	typ[4]  = "m7";		grp[ 4] = 2;	trn[ 4] = "0000-0000-00"
	typ[5]  = "mM7";	grp[ 5] = 2;	trn[ 5] = "0000-00000+0"
	typ[6]  = "s4";		grp[ 6] = 2;	trn[ 6] = "0000+0000000"
	typ[7]  = "6";		grp[ 7] = 2;	trn[ 7] = "0000000000-0"
	typ[8]  = "m6";		grp[ 8] = 2;	trn[ 8] = "0000-00000-0"
	typ[9]  = "9";		grp[ 9] = 2;	trn[ 9] = "0000=0000000"
	typ[10] = "h";		grp[10] = 2;	trn[10] = "0000-00-0-00"
	typ[11] = "o";		grp[11] = 2;	trn[11] = "0000-00-00-0"
	typ[12] = "+5";		grp[12] = 3;	trn[12] = "000000000000"
	typ[13] = "m+5";	grp[13] = 3;	trn[13] = "0000-0000000"
	typ[14] = "-5";		grp[14] = 4;	trn[14] = "000000000000"
	typ[15] = "m-5";	grp[15] = 4;	trn[15] = "0000-0000000"
	typ[16] = "/"
	state = 0
	print "# \"Guitar-style\" chords for acca based on C"
	print "#     name    group    trans"
	for (pc = 0; pc < 12; pc++) {
		p = pc
		if (p > 8)
			p = pc - 12
		for (i = 0; typ[i] != "/"; i++) {
			l = sprintf "%s\t%d\t", typ[i], grp[i]
			for (j = 1; j <= 12; j++) {
				tm = substr(trn[i], j, 1)
				if ((t = index(norm, tm) - noff) == - noff) {
				    if ((t = index(xtra, tm) - xoff) == - xoff)
					print "Error in: " typ[i] " " trn[i]
				}
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
