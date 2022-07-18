# m2ps -- Music file to pic -- "small" -- treble & bass clefs
# One page per voice; small notes.  See m(5) for input format.
BEGIN	{
#set octadj = -21 for middle C == C3
#set octadj = -28 for middle C == C4
	octadj = -21
	scale = 1.4
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
		for (voice = 1; voice <= nv; voice++) {
			vn[voice] = $(voice + 1)
			lolim[voice] = 0
			hilim[voice] = 0
			trips[voice] = 0
		}
	} else if ($1 == "#TRANS") {
		for (voice = 1; voice <= nv; voice++)
			tr[voice] = $(voice + 1)
	} else if ($1 == "#TEMPO") {
		tempo = $2
	} else if ($1 == "#METER") {
		meter1 = $2
		meter2 = $3
	} else if ($1 == "#BAR") {
		if (x == 5)
			next
		for (voice = 1; voice <= nv; voice++) {
			if (trips[voice]) {
				sym = "eseq(blet,\"3\")\n"
				trips[voice] = 0
			} else
				sym = ""
			nsym[t * nv + voice] = sym "sbarline(bb,bt); n=n+1.9*w"
			nsw[t * nv + voice] = 1.9
		}
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
	for (voice = 1; voice <= nv; voice++) {
		dn = 4
		j = 1
		code = $(voice+1)
		c = substr(code, j++, 1)
		if (c == "-") {
			nsym[t * nv + voice] = ""
			nsw[t * nv + voice] = 0
			continue
		}
		ac = ""
		sym = ""
		if (c == "R") {
			type = "rest"
			line = "r"
			c = substr(code, j++, 1)
			dur = tv[c]
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
			c = substr(code, j++, 1)
			dur = tv[c]
			if (ac == acc[line])
				sw = 0
			else {
				if (ac == "b")
					sym = "flat(" line "); n=n+w; "
				else if (ac == "#")
					sym = "sharp(" line "); n=n+w; "
				else
					sym = "natural(" line "); n=n+w; "
				sw = 1
				acc[line] = ac;
				acclist[numaccs++] = line
			}
			if (lyric != "")
				sym = sym "lyric(ll, \"" lyric "\"); "
			if (line >= 12)
				for (i = 12; i <= line; i += 2)
					sym = sym "ledgerline(" i ");" 
			if (line == 0)
				sym = sym "ledgerline(0); "
			if (line <= -12)
				for (i = -12; i >= line; i -= 2)
					sym = sym "ledgerline(" i ");" 
			if (line < lolim[voice])
				lolim[voice] = line
			if (line > hilim[voice])
				hilim[voice] = line
		}
		sym = sym dur type "(" line ");" 
		c = substr(code, j, 1)
		if (c == "t") {
			if (type == "rest")
				sym = sym " seq(n,r-3);"
			else
				sym = sym " seq(n," line ");"
			trips[voice] = 1
		} else if (trips[voice]) {
			sym = sym " eseq(blet, \"3\");"
			trips[voice] = 0
		}
		if (c == ".") {
			while (c == ".") {
				sym = sym " dtd(" line "); n=n+0.33*w;"
				sw = sw + 0.33
				j++
				c = substr(code, j, 1)
			}
		}
		nsym[t * nv + voice] = sym " n=n+3*w"
		nsw[t * nv + voice] = sw + 3
	}
	t++
}
END	{
	print ".PS"
	print "scale = ", scale
	print ".PE"
	print ".so /u/psl/midi/etc/mudef.p"
	print ".PE"
	for (voice = 1; voice <= nv; voice++) {
		if (voice > 1)
			print ".bp"
		if (title != "") {
			print ".sp 0.75i"
			print ".ps +10"
			print ".ce 1"
			print title
			print ".ps -10"
			print ".sp 0.25i"
			print vn[voice] "		\\n(mo/\\n(dy/\\n(yr"
		}
		print ".PS"
		if (lolim[voice] < -1) {
			ll = lolim[voice] - 2
			if (ll > -10)
				ll = -12
			if (hilim[voice] > 1) {
				print "r=6; bb=-10; bt=10; ll=", ll
				staves = "staves(n)"
				q = "trebleclef(4);bassclef(-4);n=n+4*w;"
				q = q "meter(6,\"" meter1 "\",\"" meter2 "\");"
				q = q "meter(-6,\"" meter1 "\",\"" meter2 "\");"
			} else {
				print "r=-6; bb=-10; bt=-2; ll=", ll
				staves = "staff(-6,n)"
				q = "bassclef(-4);n=n+4*w;"
				q = q "meter(-6,\"" meter1 "\",\"" meter2 "\");"
			}
		} else {
			ll = lolim[voice] - 2
			if (ll > 2)
				ll = 0
			print "r=6; bb=2; bt=10; ll=", ll
			staves = "staff(6,n)"
			q = "trebleclef(4);n=n+4*w;"
			q = q "meter(6,\"" meter1 "\",\"" meter2 "\");"
		}
		clefetc = "n=w; " q " n=n+3*w"
		print clefetc
		x = 5
		psw = 0
		for (time = 0; time < t; time++) {
			if (x > 65 * scale || (x > 56 * scale && psw == 1.9)) {
				print "eseq(blet, \"3\")"
				if (psw == 1.9)
					print "n=n-1.9*w"
				print staves
				print ".PE"
				print ".ne 1.5i"
				print ".PS"
				print clefetc
				x = 5
			}
			i = time * nv + voice
			q = nsym[i]
			if (q != "")
				print q
			psw = nsw[i]
			x = x + psw
		}
		print "eseq(blet, \"3\")"
		if (psw == 1.9)
			print "n=n-1.9*w"
		print staves
		print ".PE"
		print ".ps 10"
	}
}
