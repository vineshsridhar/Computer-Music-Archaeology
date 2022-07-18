# m2pa -- Music file to pic  -- combined score
# All the voices overlayed; big notes.  See m(5) for input format.
BEGIN	{
#set octadj = -21 for middle C == C3
#set octadj = -28 for middle C == C4
	octadj = -21
	nn["C"] = 0
	nn["D"] = 1
	nn["E"] = 2
	nn["F"] = 3
	nn["G"] = 4
	nn["A"] = 5
	nn["B"] = 6
	tv["w"] = "whole"
	tv["h"] = "half"
	tv["q"] = "quarter"
	tv["e"] = "eighth"
	tv["s"] = "sixteenth"
	tv["t"] = "thirtysecond"
	tv["f"] = "sixtyfourth"
	numaccs = 0
	nv = 0
	t = 0
	title = ""
}
/^#/	{
	if ($1 == "#TITLE") {
		for (i = 2; i <= NF; i++)
			title = title " " $i
	} else if ($1 == "#VOICES") {
		nv = NF - 1
		for (voice = 1; voice <= nv; voice++)
			vn[voice] = $(voice + 1)
	} else if ($1 == "#TEMPO") {
		tempo = $2
	} else if ($1 == "#METER") {
		meter1 = $2
		meter2 = $3
	} else if ($1 == "#BAR") {
		if (x == 5)
			next
		nsym[t] = "barline(); n=n+2*w"
		nsw[t] = 2
		while (numaccs > 0)
			acc[acclist[--numaccs]] = ""
		t++
	}
	next
}
NF >= nv	{
	if ($1 == "/")
		lyric = ""
	else
		lyric = $1
	nsym[t] = ""
	maxsw = 0
	for (voice = 1; voice <= nv; voice++) {
		dn = 4
		j = 1
		code = $(voice+1)
		c = substr(code, j++, 1)
		if (c == "-")
			continue
		ac = ""
		if (c == "R") {
			type = "rest"
			line = -6
		} else {
			if (lyric == "X") {
				type = "perc"
				lyric = ""
			} else
				type = "note"
			note = nn[c]
			c = substr(code, j, 1)
			if (c == "b" || c == "#") {
				ac = c
				j++
			}
			oct = substr(code, j++, 1)
			line = 7 * oct + note + octadj + tr[voice]
		}
		c = substr(code, j++, 1)
		dur = tv[c]
		sw = 0
		if (ac == acc[line])
			sym = ""
		else {
			if (ac == "b")
				sym = "n=n-w;flat(" line ");n=n+w; "
			else if (ac == "#")
				sym = "n=n-w;sharp(" line ");n=n+w; "
			else
				sym = "n=n-w;natural(" line ");n=n+w; "
			acc[line] = ac;
			acclist[numaccs++] = line
		}
		if (type != "rest" && lyric != "")
			sym = sym "lyric(-14, \"" lyric "\"); "
		if (line >= 12)
			for (i = 12; i <= line; i += 2)
				sym = sym "ledgerline(" i ");" 
		if (line == 0)
			sym = sym "ledgerline(0); "
		if (line <= -12)
			for (i = -12; i >= line; i -= 2)
				sym = sym "ledgerline(" i ");" 
		sym = sym dur type "(" line ");" 
		c = substr(code, j, 1)
		if (c == ".") {
			while (c == ".") {
				sym = sym " dtd(" line "); n=n+0.33*w;"
				sw = sw + 0.33
				j++
				c = substr(code, j, 1)
			}
		} else if (c == "t")
			sym = sym " slet(n-w/2," line ", n+w/2," line ", \"3\")"
		nsym[t] = nsym[t] sym " n=n-" sw "*w; "
		if (sw > maxsw)
			maxsw = sw
	}
	nsym[t] = nsym[t] " n=n+" maxsw+4 "*w"
	nsw[t] = maxsw + 4
	t++
}
END	{
	print ".so /u/psl/midi/etc/mudef.p"
	print "define clefetc X"
	print "n=w; trebleclef(4); bassclef(-4); n=n+4*w"
	print "meter(6, \"" meter1 "\", \"" meter2 "\")"
	print "meter(-6, \"" meter1 "\", \"" meter2 "\")"
	print "n=n+3*w"
	print "X"
	print ".PE"
	print ".bp"
	print ".sp 0.75i"
	print ".ps +10"
	print ".ce 1"
	print title
	print ".ps -10"
	print ".sp 0.25i"
	print "All parts		\\n(mo/\\n(dy/\\n(yr"
	print ".PS"
	print "clefetc()"
	x = 5
	for (time = 0; time < t; time++) {
		if (x > 65) {
			print "staves(n)"
			print ".PE"
			print ".ne 1.8i"
			print ".PS"
			print "clefetc()"
			x = 5
		}
		q = nsym[time]
		if (q != "")
			print q
		x = x + nsw[time]
	}
	print "staves(n)"
	print ".PE"
}
