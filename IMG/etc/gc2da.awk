# generate da files for guitar chord accompaniment
# stdin provides data in the form:
# bass chrd1 chrd2 chrd3 [chrd4 [chrd5 [chrd6]]]
# e.g.:
# # Comments start with "#", as do special controls (see below)
# G2  Bb3 D4  G4
# D3  Bb3 D4  G4
# ...
#  etc.		Note: "-" or "R" in place of a note makes a rest
# and stdout gets a da-format file
BEGIN	{
	nn[0] = "C"; nn[2] = "D"; nn[4] = "E"; nn[5] = "F"
	nn[7] = "G"; nn[9] = "A"; nn[11] = "B"
	ns[1] = "C#"; ns[3] = "D#"; ns[6] = "F#"
	ns[8] = "G#"; ns[10] = "A#"
	nf[1] = "Db"; nf[3] = "Eb"; nf[6] = "Gb"
	nf[8] = "Ab"; nf[10] = "Bb"
	p["C"] = 0; p["D"] = 2; p["E"] = 4; p["F"] = 5;
	p["G"] = 7; p["A"] = 9; p["B"] = 11;
	for (k = 0; k < 128; k++) {
	    n = k % 12
	    o = int(k / 12) - 1
	    pn = nn[n]
	    if (pn != "") {
		nv[pn o] = k
	    } else {
		nv[ns[n] o] = k
		nv[nf[n] o] = k
	    }
	}
	nmult = 1
	t = 0
	beats = 4
	tempo = 100
	qpc = 1
	style = "bum-chang"
	pick = "split"
	artic[0] = 1
	nartic = 1
	cartic = 0
	dt = 0.025;
	mode = "90"
	skipping = 0
	rpt = 1
	wrongrpt = 0
}

/^#/	{
	if ($1 == "#")
	    next
	if (skipping && $1 != "#ENDSKIP")
	    next
	if (wrongrpt && $1 != "#ALLRPTS" && $1 != "#NOTRPT" && $1 != "#ONLYRPT")
	    next
	if ($1 == "#ALLRPTS")
	    wrongrpt = 0
	if ($1 == "#NOTRPT") {
	    wrongrpt = 0
	    for (i = 2; i <= NF; i++)
		if ($i == rpt)
		    wrongrpt = 1
	}
	if ($1 == "#ONLYRPT") {
	    wrongrpt = 1
	    for (i = 2; i <= NF; i++)
		if ($i == rpt)
		    wrongrpt = 0
	}
	if (wrongrpt)
	    next
	skipping = 0
	if ($1 == "#ARTIC" || $1 == "#ARTICULATION") {
	    for (i = 2; i <= NF; i++)
		artic[i - 2] = $i
	    nartic = NF - 1
	    cartic = 0
	} else if ($1 == "#BEATS") {
	    beats = $2
	    qpc = (100. / tempo) * (4. / beats)
	    cartic = 0
	} else if ($1 == "#MULT" || $1 == "#MULTIPLICITY") {
	    nmult = $2
	    cartic = 0
	} else if ($1 == "#PICK") {
	    pick = $2
	    cartic = 0
	    if (pick != "block" && pick != "split" && pick != "split3" && pick != "alt") {
		print "Unrecognized pick pattern: " style
		print "Known patterns are: block, split, split3, & alt"
		exit
	    }
	} else if ($1 == "#RPT") {
	    rpt = $2
	} else if ($1 == "#SKIP") {
	    skipping = 1
	} else if ($1 == "#STRUM") {
	    dt = $2
	} else if ($1 == "#STYLE") {
	    style = $2
	    cartic = 0
	    if (style == "bum-chang") {
		pick = "split"
		artic[0] = 1
		nartic = 1
		dt = 0.025
	    } else if (style == "swing") {
		pick = "block"
		artic[0] = 0.667
		artic[1] = 0.250
		nartic = 2
		dt = 0.0
	    } else if (style == "waltz") {
		pick = "split3"
		artic[0] = 1
		artic[1] = 0.5
		artic[2] = 0.5
		nartic = 3
		dt = 0.025
	    } else {
		print "Unrecognized style: " style
		print "Known styles are: bum-chang, swing, & waltz"
		exit
	    }
	} else if ($1 == "#TEMPO") {
	    tempo = $2
	    qpc = (100. / tempo) * (4. / beats)
	}
	next
}

NF > 1	{
	if (skipping || wrongrpt)
	    next
	for (nt = 1; nt <= nmult; nt++) {
	    nev = 0
	    if (pick == "block") {
		bt1 = t
		et1 = t + artic[cartic] * qpc;
		cartic = (cartic + 1) % nartic
		for (i = 1; i <= NF; i++) {
		    when[nev] = bt1
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et1
		    note[nev] = $i
		    vel[nev++] = 0
		}
	    } else if (pick == "split") {
		bt1 = t
		et1 = t + artic[cartic] * qpc / 2;
		cartic = (cartic + 1) % nartic
		bt2 = t + qpc / 2.
		et2 = t + (1 + artic[cartic] * qpc) / 2;
		cartic = (cartic + 1) % nartic
		when[nev] = bt1
		note[nev] = $1
		vel[nev++] = 64
		when[nev] = et1
		note[nev] = $1
		vel[nev++] = 0
		for (i = 2; i <= NF; i++) {
		    when[nev] = bt2 + (i - NF) * dt
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et2
		    note[nev] = $i
		    vel[nev++] = 0
		}
	    } else if (pick == "split3") {
		bt1 = 0
		et1 = t + artic[cartic] * qpc / 3;
		cartic = (cartic + 1) % nartic
		bt2 = t + qpc / 3
		et2 = t + (1 + artic[cartic]) * qpc / 3;
		cartic = (cartic + 1) % nartic
		bt3 = t + qpc * 2 / 3
		et3 = t + (2 + artic[cartic]) * qpc / 3;
		cartic = (cartic + 1) % nartic
		when[nev] = bt1
		note[nev] = $1
		vel[nev++] = 64
		when[nev] = et1
		note[nev] = $1
		vel[nev++] = 0
		for (i = 2; i <= NF; i++) {
		    when[nev] = bt2 + (i - NF) * dt
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et2
		    note[nev] = $i
		    vel[nev++] = 0
		    when[nev] = bt3 + (i - NF) * dt
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et3
		    note[nev] = $i
		    vel[nev++] = 0
		}
	    } else if (pick == "alt") {
		bt1 = 0
		et1 = t + artic[cartic] * qpc / 4;
		cartic = (cartic + 1) % nartic
		bt2 = t + qpc * 1 / 4
		et2 = t + (1 + artic[cartic]) * qpc / 4;
		cartic = (cartic + 1) % nartic
		bt3 = t + qpc * 2 / 4
		et3 = t + (2 + artic[cartic]) * qpc / 4;
		cartic = (cartic + 1) % nartic
		bt4 = t + qpc * 3 / 4
		et4 = t + (3 + artic[cartic]) * qpc / 4;
		cartic = (cartic + 1) % nartic
		when[nev] = bt1
		note[nev] = $1
		vel[nev++] = 64
		when[nev] = et1
		note[nev] = $1
		vel[nev++] = 0
		when[nev] = bt3
		note[nev] = $2
		vel[nev++] = 64
		when[nev] = et3
		note[nev] = $2
		vel[nev++] = 0
		for (i = 3; i <= NF; i++) {
		    when[nev] = bt2 + (i - NF) * dt
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et2
		    note[nev] = $i
		    vel[nev++] = 0
		    when[nev] = bt4 + (i - NF) * dt
		    note[nev] = $i
		    vel[nev++] = 64
		    when[nev] = et4
		    note[nev] = $i
		    vel[nev++] = 0
		}
	    }
	    t += qpc
	    for (i = 0; i < nev; i++)
		if (note[i] == "-" || note[i] == "R")
		    when[i] = 9999.0
	    for (;;) {
		tmin = 9999.0
		j = -1
		for (i = 0; i < nev; i++)
		    if (when[i] < tmin)
			tmin = when[j = i]
		if (j == -1)
		    break
		if (tmin > t)
		    tmin = t
		printf " 0 %s %2x %2x ;  %8.3f", mode,nv[note[j]],vel[j],tmin
		mode = "  "
		if (v > 0)
		    printf "\t%s", n
		printf "\n"
		when[j] = 9999.0
	    }
	}
	printf " 0 f9       ;  %8.3f   TCWME\n", t
}
