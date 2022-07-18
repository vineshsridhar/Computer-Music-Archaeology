# Drum rhythm decoder	psl 8/87
#
# Input looks like: (ignore "# " at beginning of lines)
# #TEMPO 100
# #QUANT 16
# 2/43	5-3---5-5-3-----5-3-------5-----	Conga H Muted
# 2/44	----7-------5-5-----7-------5-5-	Conga H Open
# 2/45	----------------------5-5-------	Conga L
#
# First field is "key" or "channel/key", "43" = "1/43", both are lower-case HEX.
# Second field is drum rhythm, "-" for silence, "0"-"9" for soft-loud.
# Any other fields are ignored.
# Default quantum is 8th notes, "#QUANT 16" sets 16th notes.
# A line beginning "# " is discarded as comment.
#
# Output is in da(1) format, suitable for ra(1).
#
BEGIN	{
	qspec = 0
	quant = 8
	tspec = 0
	tempo = 100
	maxlen = 0
	inst = 0
	v["0"] = " 1"
	v["1"] = " e"
	v["2"] = "1c"
	v["3"] = "2a"
	v["4"] = "38"
	v["5"] = "47"
	v["6"] = "55"
	v["7"] = "63"
	v["8"] = "71"
	v["9"] = "7f"
}
/^#/	{
	if ($1 == "#QUANT") {
		if (qspec++) {
			print "ERROR: can't respecify QUANT"
			exit
		}
		quant = $2
	} else if ($1 == "#TEMPO") {
		if (tspec++) {
			print "ERROR: can't respecify TEMPO"
			exit
		}
		tempo = $2
	}
	next
}
NF > 1	{
	i = num[$1] - 1
	if (i < 0) {
		i = inst++
		num[$1] = i + 1
		chkey[i] = $1
	}
	pat[i] = pat[i] $2
	j = length(pat[i])
	plen[i] = j
	if (j > maxlen)
		maxlen = j
}
END	{
	for (i = 0; i < maxlen; i++) {
		t = (i * 4. * 100) / (tempo * quant)
		for (j = 0; j < inst; j++) {
			if (plen[j] < i)
				ii = i % plen[j]
			else
				ii = i
			vel = substr(pat[j], ii + 1, 1)
			if (vel < "0" || vel > "9")
				continue
			if (split(chkey[j], q, "/") == 1) {
				chan = 0
				key = q[1]
			} else {
				chan = q[1] - 1
				key = q[2]
			}
			printf " 0 9%s %s %s ; %8.3f\n", chan, key, v[vel], t
			printf " 0    %s  0 ; %8.3f\n", key, t
		}
	}
	printf " 0 f9       ; %8.3f\n", (maxlen*4.*100) / (tempo*quant) + 0.0004
}

