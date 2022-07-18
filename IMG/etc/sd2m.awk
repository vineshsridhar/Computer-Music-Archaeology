BEGIN	{
	coding = "abcdefg1234567ABCDEFG"	# must be 21 chars
	for (i = 1; i <= 21; i++) {
		code = substr(coding, i, 1)
		degree[code] = ((i - 1) % 7) + 1
		pitcheffect[code] = (int((i - 1) / 7) - 1) * 6
	}
	scale[1] = 0; scale[2] = 2; scale[3] = 4; scale[4] = 5
	scale[5] = 7; scale[6] = 9; scale[7] = 11
	qtv[1] = "w"
	qtv[2] = "h"; qtv[3] = "ht"
	qtv[4] = "q"; qtv[5] = "e."; qtv[6] = "qt";
	qtv[8] = "e"; qtv[10] = "s."; qtv[12] = "et"
	qtv[16] = "s"; qtv[20] = "t."; qtv[24] = "st"
	qtv[32] = "t"; qtv[48] = "tt"
	qtv[64] = "f"
	nname[1] = "C"; nname[2] = "C#"; nname[3] = "D"; nname[4] = "D#"
	nname[5] = "E"; nname[6] = "F"; nname[7] = "F#"; nname[8] = "G"
	nname[9] = "G#"; nname[10] = "A"; nname[11] = "A#"; nname[12] = "B"
	for (i = 0; i < 12; i++) {
		nnum[nname[i + 1]] = i
		nnum[i] = i
	}
	mnum = 4		# 4/4 default
	mden = 4
	quant = 8		# eighth notes default
	tv = qtv[quant]
	q[0] = (quant * mnum) / mden
}

/^#/	{			# control line
	if (substr($1, length($1), 1) == "S")
		$1 = substr($1, 1, length($1) - 1)
	if ($1 == "#CODING") {		# define scale-degree codes (stripped)
		coding = $2		# must be 21 chars
		if (length(coding) != 21) {
			print "SD error: #CODING arg must be 21 chars long"
			next
		}
		for (i = 1; i <= 21; i++) {
			code = substr(coding, i, 1)
			degree[code] = ((i - 1) % 7) + 1
			pitcheffect[code] = (int((i - 1) / 7) - 1) * 6
		}
	} else if ($1 == "#INIT") {		# starting notes (stripped)
		if (nv > 0) {		# (VOICES must appear first)
			for (i = 0; i < nv; i++) {
				l = length($(i+2))
				oct = substr($(i+2), l, 1)
				nn = substr($(i+2), 1, l - 1)
				pitch[i] = (oct + 1) * 12 + nnum[nn]
			}
		}
	} else if ($1 == "#METER") {		# define measure length
		mnum = $2
		mden = $3
		q[time[0]] = (quant * mnum) / mden
		print
	} else if ($1 == "#QUANT") {		# quantization (stripped)
		if (nv > 0) {		# (VOICES must appear first)
			quant = $2
			tv = qtv[quant]
			q[time[0]] = (quant * mnum) / mden
		}
	} else if ($1 == "#SCALE") {		# define the scale (stripped)
		slen = split($2, a, ",")
		if (slen <= 1)
			print "SD error: #SCALE format"
		for (i = 1; i <= slen; i++)
			scale[i] = nnum[a[i]]
	} else if ($1 == "#VOICE") {		# define output voices
		nv = NF - 1
		for (i = 0; i < nv; i++) {
			voice[i] = $(i+2)
			vn[$(i+2)] = i + 1;
			time[i] = 0
		}
		print
	} else {				# controls for m-format
		print
	}
	next
}

vn[$1] > 0	{		# melodic input
	v = vn[$1] - 1
	t = time[v]
	for (i = 1; (d = substr($2, i, 1)) != ""; i++) {
		if (d == "|") {
			--t		# barline takes no time
		} else if (d == "^") {
			pitch[v] += 7
		} else if (d == "v") {
			pitch[v] -= 7
		} else if (d == "(") {
			p[t * nv + v] = sprintf "(%s", tv
		} else if (degree[d]) {
			pitch[v] += pitcheffect[d]
			s = scale[degree[d]]
			o = int((pitch[v] - s - 6) / 12)
			pitch[v] = 12 * o + s + 12
			p[t * nv + v] = sprintf "%s%d%s", nname[s+1], o, tv
		} else
			p[t * nv + v] = sprintf "R%s", tv
		t++
	}
	time[v] = t
}

END	{
	printf "#BAR\n"
	qib = 0
	for (t = 0; t < time[0]; t++) {
		if (q[t])
			qpm = q[t]
		printf "x"
		for (v = 0; v < nv; v++)
			printf "\t%s", p[t * nv + v]
		printf "\n"
		if (++qib >= qpm) {
			printf "#BAR\n"
			qib = 0
		}
	}
	if (qib > 0)
		printf "#BAR\n"
}
