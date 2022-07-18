# m2pub -- Music file to pic -- tiny for PUBlication, either clef (#CLEF)
# Music file format is m(5) with addition of the following:
# #CHORDSYM text (puts text over the staff)
# #CLEF	treble (default) or #CLEF bass
# #KEY C (default), #KEY Bb (for Bb or Gm), etc. (takes effect at next #BAR)
# #NEWSTAFF (flushes current staff & starts another)
# #SQUEEZE 1 (default), #SQUEEZE 1.5 will put 50% more notes on a line
# psl, last mod: 9/6/89
BEGIN	{
#set octadj = -21 for middle C == C3
#set octadj = -28 for middle C == C4
	octadj = -21
	loline = -35
	hiline = 39
	clef = "trebleclef(4)"
	barline = "tbarline()"
	midline = 6
	key = "C"
	nflats["F"] = 1; nflats["Bb"] = 2; nflats["Eb"] = 3; nflats["Ab"] = 4
	nflats["Db"] = 5; nflats["Gb"] = 6; nflats["Cb"] = 7 
	nshrps["G"] = 1; nshrps["D"] = 2; nshrps["A"] = 3; nshrps["E"] = 4
	nshrps["B"] = 5; nshrps["F#"] = 6; nshrps["C#"] = 7
	fline[1] = 6; fline[2] = 9; fline[3] = 5; fline[4] = 8
	fline[5] = 4; fline[6] = 7; fline[7] = 3
	sline[1] = 10; sline[2] = 7; sline[3] = 11; sline[4] = 8
	sline[5] = 5; sline[6] = 9; sline[7] = 6
	squeeze = 1
	scale = 1.5
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
# sw is cumulative width of symbol at time T
	sw = 0
# sym is cumulative symbol text for time T
	sym = ""
	T = 0
	title = ""
}
/^#/	{
	if ($1 == "#BAR") {
	    if (T > 0) {
		for (voice = 1; voice <= nv; voice++) {
		    nsym[T * nv + voice] = barline "; n=n+1.9*w"
		    nsw[T * nv + voice] = 1.9
		}
		T++
	    }
	    while (numaccs > 0)
		acc[acclist[--numaccs]] = "";
	    for (f = nflats[key]; f > 0; --f)
		for (i = loline + (fline[f] % 7); i < hiline; i += 7)
		    acc[i] = "b"
	    for (s = nshrps[key]; s > 0; --s)
		for (i = loline + (sline[s] % 7); i < hiline; i += 7)
			acc[i] = "#"
	} else if ($1 == "#CHORDSYM") {
	    sym = sym "chord(\"" $2 "\"); "
	} else if ($1 == "#CLEF") {
	    if ($2 == "treble") {
		clef = "trebleclef(4)"
		barline = "tbarline()"
		midline = 6
	    } else if ($2 == "bass") {
		clef = "bassclef(-4)"
		barline = "bbarline()"
		midline = -6
		if (sline[1] > 0) {
		    for (i = 1; i <= 7; i++) {
			sline[i] = sline[i] - 14
			fline[i] = fline[i] - 14
		    }
		}
	    }
	} else if ($1 == "#KEY") {
	    key = $2
	} else if ($1 == "#METER") {
	    meter1 = $2
	    meter2 = $3
	} else if ($1 == "#NEWSTAFF") {
	    sw = sw + 999
	} else if ($1 == "#SQUEEZE") {
	    squeeze = $2
	} else if ($1 == "#TEMPO") {
	    tempo = $2
	} else if ($1 == "#TITLE") {
	    for (i = 2; i <= NF; i++)
		title = title " " $i
	} else if ($1 == "#TRAN" || $1 == "#TRANS") {
	    for (voice = 1; voice <= nv; voice++)
		tr[voice] = $(voice + 1)
	} else if ($1 == "#VOICE" || $1 == "#VOICES") {
	    nv = NF - 1
	    for (voice = 1; voice <= nv; voice++)
		vn[voice] = $(voice + 1)
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
		nsym[T * nv + voice] = ""
		nsw[T * nv + voice] = 0
		continue
	    }
	    ac = ""
	    if (c == "R") {
		type = "rest"
		line = midline
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
	    if (type != "rest" && ac != acc[line]) {
		if (ac == "b")
		    sym = sym "flat(" line "); n=n+w; "
		else if (ac == "#")
		    sym = sym "sharp(" line "); n=n+w; "
		else
		    sym = sym "natural(" line "); n=n+w; "
		sw = sw + 1
		acc[line] = ac;
		acclist[numaccs++] = line
	    }
	    if (type != "rest" && lyric != "")
		sym = sym "lyric(" midline - 10 ", \"" lyric "\"); "
	    if (line >= midline + 6)
		sym = sym "ledgerlines(" midline+6 "," line ",2);" 
	    if (line <= midline - 6)
		sym = sym "ledgerlines(" midline-6 "," line ",-2);" 
	    sym = sym dur type "(" line ");" 
	    c = substr(code, j, 1)
	    if (c == ".") {
		dl = (line - midline) * (line - midline)
		if (dl == 0 || dl == 4 || dl == 16)
		    dl = line + 1
		else
		    dl = line
		while (c == ".") {
		    sym = sym " dtd(" dl "); n=n+0.33*w;"
		    sw = sw + 0.33
		    j++
		    c = substr(code, j, 1)
		}
	    } else if (c == "t")
		sym = sym " blet(n-w/2," line ", n+w/2," line ", \"3\"); "
	    nsym[T * nv + voice] = sym " n=n+2*w"
	    nsw[T * nv + voice] = sw + 2
	    sym = ""
	    sw = 0
	}
	T++
}
END	{
	print ".nr pS \\n(.s"
	print ".so /u/psl/midi/etc/mudef.p"
	print "scale = " scale
	if (squeeze != 1)
	    print "w = w / " squeeze
	minwid = 48 * squeeze
	maxwid = 55 * squeeze
	print "define clefetc X n=w; " clef "; n=n+3*w"
	sym = ""
	for (i = 1; i <= nflats[key]; i++)
	    sym = sym "flat(" fline[i] "); n=n+w; "
	for (i = 1; i <= nshrps[key]; i++)
	    sym = sym "sharp(" sline[i] "); n=n+w; "
	if (sym != "")
	    print sym "n=n+w"
	print "meter(" midline ", \"" meter1 "\", \"" meter2 "\"); n=n+3*w X"
	print ".PE"
	for (voice = 1; voice <= nv; voice++) {
	    if (voice > 1)
		print ".bp"
	    if (title != "") {
		print ".sp 0.75i"
		print ".ps 24"
		print ".ce 1"
		print title
		print ".ps 10"
		print ".sp 0.25i"
		print vn[voice] "		\\n(mo/\\n(dy/\\n(yr"
	    }
	    print ".ps 4"
	    print ".PS"
	    print "clefetc()"
	    x = 5
	    psw = 0
	    i = -1
	    for (time = 0; time < T; time++) {
		newline = 0
		if (x > maxwid * scale && nsw[i] != 1.9)
		    newline = 1
		if (x > minwid * scale && psw == 1.9)
		    newline = 1
		i = time * nv + voice
		if (nsw[i] >= 999)
		    newline = 1
		if (newline) {
		    if (psw == 1.9)
			print "n=n-1.9*w"
		    print "staff(" midline ",n)"
		    print ".PE"
		    print ".ne 1.0i"
		    print ".PS"
		    print "clefetc()"
		    x = 5
		}
		psw = nsw[i]
		if (psw >= 999)
		    psw -= 999
		q = nsym[i]
		if (q != "")
		    print q
		x = x + psw
	    }
	    if (psw == 1.9)
		print "n=n-1.9*w"
	    print "staff(" midline ",n)"
	    print ".PE"
	    print ".ps \\n(pS"
	}
}
