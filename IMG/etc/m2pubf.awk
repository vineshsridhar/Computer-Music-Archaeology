# m2pubf -- Music file to pic -- tiny for PUBlication, fixed spacing
# SLEAZY HACK - NOT FINISHED!
# Music file format is described in m(5) with addition of #CLEF:
# #CLEF	treble (default) or #CLEF bass
# psl, last mod: 10/19/88
BEGIN	{
#set octadj = -21 for middle C == C3
#set octadj = -28 for middle C == C4
	octadj = -21
	clef = "trebleclef(4)"
	barline = "tbarline()"
	centerline = 6
	scale = 1.5
	nn["C"] = 0
	nn["D"] = 1
	nn["E"] = 2
	nn["F"] = 3
	nn["G"] = 4
	nn["A"] = 5
	nn["B"] = 6
	nw["w"] = 16; tv["w"] = "whole"
	nw["h"] =  8; tv["h"] = "half"
	nw["q"] =  4; tv["q"] = "quarter"
	nw["e"] =  2; tv["e"] = "eighth"
	nv = 0
	t = 0
	title = ""
}
/^#/	{
	if ($1 == "#BAR") {
		if (x == 5)
			next
		for (voice = 1; voice <= nv; voice++) {
			nsym[t * nv + voice] = barline "; n=n+1.9*w"
			nsw[t * nv + voice] = 1.9
		}
		for (line = -12; line <= 12; line++)
			acc[line] = "";
		t++
	} else if ($1 == "#CLEF") {
		if ($2 == "treble") {
			clef = "trebleclef(4)"
			barline = "tbarline()"
			centerline = 6
		} else if ($2 == "bass") {
			clef = "bassclef(-4)"
			barline = "bbarline()"
			centerline = -6
		}
	} else if ($1 == "#ENDSKIP") {
		skip = 0
	} else if ($1 == "#METER") {
		meter1 = $2
		meter2 = $3
	} else if ($1 == "#SKIP") {
		skip = 1
	} else if ($1 == "#TEMPO") {
		tempo = $2
	} else if ($1 == "#TITLE") {
		for (i = 2; i <= NF; i++)
			title = title " " $i
	} else if ($1 == "#TRANS") {
		for (voice = 1; voice <= nv; voice++)
			tr[voice] = $(voice + 1)
	} else if ($1 == "#VOICES") {
		nv = NF - 1
		for (voice = 1; voice <= nv; voice++)
			vn[voice] = $(voice + 1)
	}
	next
}
skip==1	{
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
		if (c == "R") {
			type = "rest"
			line = centerline
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
		wid = nw[c]
		if (ac == acc[line]) {
			sym = ""
			sw = 0
		} else {
			if (ac == "b")
				sym = "flat(" line "); n=n+w; "
			else if (ac == "#")
				sym = "sharp(" line "); n=n+w; "
			else
				sym = "natural(" line "); n=n+w; "
			sw = 1
			acc[line] = ac;
		}
		if (type != "rest" && lyric != "")
			sym = sym "lyric(" centerline - 10 ", \"" lyric "\"); "
		if (line >= centerline + 6)
			for (i = centerline + 6; i <= line; i += 2)
				sym = sym "ledgerline(" i ");" 
		if (line <= centerline - 6)
			for (i = centerline - 6; i >= line; i -= 2)
				sym = sym "ledgerline(" i ");" 
		sym = sym dur type "(" line ");" 
		c = substr(code, j, 1)
		if (c == ".") {
			while (c == ".") {
				sym = sym " dtd(" line "); n=n+0.5*w;"
				sw = sw + 0.5
				j++
				c = substr(code, j, 1)
			}
		} else if (c == "t")
			sym = sym " slet(n-w/2," line ", n+w/2," line ", \"3\")"
		nsym[t * nv + voice] = sym " n=n+" wid "*w"
		nsw[t * nv + voice] = sw + wid
	}
	t++
}
END	{
	print ".PS"
	print "scale = ", scale
	print ".PE"
	print ".so /u/psl/midi/etc/mudef.p"
	print "define clefetc X"
	print "n=w; " clef "; n=n+4*w"
	print "meter(" centerline ", \"" meter1 "\", \"" meter2 "\")"
	print "n=n+3*w"
	print "X"
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
		for (time = 0; time < t; time++) {
			newline = 0
			if (x > 55 * scale && nsw[i] != 1.9)
				newline = 1
			if (x > 48 * scale && psw == 1.9)
				newline = 1
			if (newline) {
				if (psw == 1.9)
					print "n=n-1.9*w"
				print "staff(" centerline ",n)"
				print ".PE"
				print ".ne 1.0i"
				print ".PS"
				print "clefetc()"
				x = 5
			}
			i = time * nv + voice
			q = nsym[i]
			if (q != "")
				print q
			psw = nsw[i]
			x = x + psw
		}
		if (psw == 1.9)
			print "n=n-1.9*w"
		print "staff(" centerline ",n)"
		print ".PE"
		print ".ps 10"
	}
}
