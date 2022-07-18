.\"Macros for music notation, psl@LFL, 1983
.PS
### w = width of note, h = height of note, l = stem length
### s = line spacing/2, n = position of next note
### stem = 1 => stems up, = -1 => stems down, sbegx = sequence start
w = 0.1i
h = 0.079i
l = 0.28i
s = 0.042i
n = w
stem = 1
sbegx = 0
### staff(y, l) - 5 line staff, center line at y, l long
define staff X
	line from (0i, ($1 + 4) * s) right $2
	line from (0i, ($1 + 2) * s) right $2
	line from (0i, ($1 + 0) * s) right $2
	line from (0i, ($1 - 2) * s) right $2
	line from (0i, ($1 - 4) * s) right $2
	line from (0i, ($1 + 4) * s) to (0i, ($1 - 4) * s)
	X
### staves(l) - standard 10 line staff, l long
define staves X
	staff(6, $1); staff(-6, $1); line from (0i, 10 * s) to (0i, -10 * s)
	X
### trebleclef(y) - treble cleff centered on y
define trebleclef X
.ps +12
	q = ($1 - 3) * s
	circle rad 0.25 * w at (n + 0.25 * w, q)
	circle rad 0.15 * w at (n + 0.25 * w, q)
	arc at (n + 0.5 * w, q) from (n, q) to (n + w, q) rad w/2
	q = ($1 + 6) * s
	line to (n + 0.5 * w, q)
	arc cw \
	    from (n + 0.5 * w, q) to (n + 1.2 * w, q + 0.7 * w) rad 0.7 * w
	spline from (n + 1.2 * w, q + 0.7 * w) \
	         down w/2   right w/4 \
	    then down 4 * s left 1.5 * w \
	    then down 2 * s left w/4 \
	    then down 0.2 * w \
	    then down 1 * s right 0.2 * w \
	    then down 1 * s right 0.6 * w \
	    then            right 0.5 * w
	arc at (n + w, ($1)*s) \
	    from (n + w, ($1 - 2)*s) to (n + w + 2*s, ($1)*s) \
	    rad 2 * s
	arc at (n + w + 0.5*s, ($1)*s) \
	    from (n + w + 2*s, ($1)*s) to (n + w - s, ($1)*s) \
	    rad 1.5 * s
.ps -12
	X
### bassclef(y) - bass cleff centered on y
define bassclef X
.ps +12
	q = ($1) * s
	circle rad 0.25 * w at (n + 0.25 * w, q)
	circle rad 0.15 * w at (n + 0.25 * w, q)
	arc at (n + w, q) cw \
	    from (n, q) to (n + 2 * w, q) rad w
	arc at (n, q) cw \
	    from (n + 2 * w, q) to (n, q - 2 * w) rad 2 * w
	circle rad 0.1 * w at (n + 2.25 * w, q + s)
	circle rad 0.1 * w at (n + 2.25 * w, q - s)
.ps -12
	X
### pstaff(l, y) - percussion staff at y, l long
define pstaff X line from (0, ($2) * s) right ($1) X
### meter(y, "n", "d") - meter indication at y, n/d (e.g. 4/4)
define meter X
.ps +8
.ft B
	move to (n,  ($1 + 2.4) * s - 0.03i) $2
	move to (n,  ($1 - 2.4) * s - 0.03i) $3
.ft P
.ps -8
	X
### chord("name") - put chord name above the staff at standard height
define chord X
	chordy($1, 16)
	X
### chordy("name", y) - put chord name above the staff at specified height
define chordy X
.ps +4
	move to (n, $2 * s) $1 ljust
.ps -4
	X
### tbarline() - treble staff barline at current note position, n
define tbarline X
	move to (n, 2 * s)
	line to (n, 10 * s)
	X
### bbarline() - bass staff barline at current note position, n
define bbarline X
	move to (n, -10 * s)
	line to (n, -2 * s)
	X
### barline() - full barline at current note position, n
define barline X
	move to (n, -10 * s)
	line to (n, 10 * s)
	X
### sbarline(y0, y1) - special barline at current note position, n
define sbarline X
	move to (n, ($1) * s)
	line to (n, ($2) * s)
	X
### ledgerline(y) - ledger line at height y and current note position
define ledgerline X
.ps +12
	line from (n - 3*w/4, ($1) * s) right 3*w/2
.ps -12
	X
### ledgerlines(fy, ly, d) - ledger lines from fy to ly, direction d (2 or -2)
define ledgerlines X
	x = n - 3*w/4; dx = 3*w/2
.ps +18
	if (($3) < 0) then Y
	    for y = -($1) to -($2) by 2 do Z line from (x, -y * s) right dx Z
	Y else Y
	    for y = ($1) to ($2) by 2 do Z line from (x, y * s) right dx Z
	Y
.ps -18
	X
### wholenote(y) - whole note at height y and current note position
define wholenote X
.ps +2
	ellipse ht h wid w at (n, ($1) * s)
	ellipse ht 0.9 * h wid w at (n, ($1) * s)
.ps -2
	X
### wholerest(y) - whole rest at height y and current note position
define wholerest X
	box ht 0.5*h wid w at (n, ($1 + 1.5) * s)
	box ht 0.3*h wid w at (n, ($1 + 1.5) * s)
	box ht 0.1*h wid w at (n, ($1 + 1.5) * s)
	X
### notestem(y) - note stem at height y and current note position
define notestem X
.ps +8
	line up stem * l at (n+stem*w/2, ($1) * s)
Stem:	Here
.ps -8
	X
### halfnote(y) - half note at height y and current note position
define halfnote X
	notestem($1)
	wholenote($1)
	X
### halfrest(y) - half rest at height y and current note position
define halfrest X
.ps +8
	box ht 0.1*h wid w at (n, ($1 + 0.5) * s)
	box ht 0.3*h wid w at (n, ($1 + 0.5) * s)
	box ht 0.5*h wid w at (n, ($1 + 0.5) * s)
.ps -8
	X
### blacknote(y) - filled oval at height y and current note position
define blacknote X
.ps +4
	ellipse ht 0.2 * h wid 0.2 * w at (n, ($1) * s)
	ellipse ht 0.4 * h wid 0.4 * w at (n, ($1) * s)
	ellipse ht 0.6 * h wid 0.6 * w at (n, ($1) * s)
	ellipse ht 0.8 * h wid 0.8 * w at (n, ($1) * s)
	ellipse ht 1.0 * h wid 1.0 * w at (n, ($1) * s)
Stem: last ellipse.e
.ps -4
	X
### quarternote(y) - quarter note at height y and current note position
define quarternote X
	notestem($1)
	blacknote($1)
	X
### note(y) - synonym for quarternote(y)
define note X
	quarternote($1)
	X
### quarterrest(y) - thick 1/4 rest at height y and current note position
define quarterrest X
	n = n-0.035*w
	qrest($1+0.14)
	n = n+0.07*w
	qrest($1-0.14)
	n = n-0.035*w
	X
### qrest(y) - skinny quarter rest at height y and current note position
define qrest X
.ps +8
	line from (n, ($1 + 4) * s) down 2*s right w/2
	spline from (n + w/2, ($1 + 2) * s) \
	         down 1.8*s left w \
	    then down 2.2*s right w \
	    then up   s     left w/2 \
	    then down s     left w/2 \
	    then down s     right w/2
.ps -8
	X
### quarterperc(y) - quarter percussion note, height y, current note position
define quarterperc X
.ps +8
	line from (n - w/2, ($1) * s - h/2) up h right w
	line from (n - w/2, ($1) * s + h/2) down h right w
	line from (n + stem*w/2, ($1) * s + stem*h/2) up stem * (l  - h/2)
Stem:	Here
.ps -8
	X
### flag(y, dy) - flag (1/8, 1/16, etc. notes) for note at (n, y), dy from end
define flag X
.ps +8
	line from (n+stem*w/2, ($1) * s + stem * (l - ($2))) \
	         down stem*h/3 right w/6 \
	    then down stem*h/3 right w/3 \
	    then down stem*h/3 right w/9 \
	    then down stem*h/3 \
	    then down stem*h/3 left w/9
.ps -8
	X
### eighthnote(y) - eighth note at height y and current note position
define eighthnote X
	flag($1, 0.02)
	quarternote($1)
	X
### eighthrest(y) - eighth rest at height y and current note position
define eighthrest X
.ps +18
	circle radius 0.01i at (n, ($1 + 1.0) * s)
.ps -10
#	arc at (n + w/2, ($1 + 3.0) * s) \
#	    from last circle.s to (n + w, ($1 + 1.0) * s) rad 2 * s
#	line from (n + w, ($1 + 1) * s) down 3 * s left w/2
	arc from last circle.s to (n + w, ($1 + 1.5) * s) rad 2 * s
	line from (n + w, ($1 + 1.5) * s) down 3.5 * s left w/2
.ps -8
	X
### eighthperc(y) - eighth percussion note, height y, current note position
define eighthperc X
	flag($1, 0.02)
	quarterperc($1)
	X
### sixteenthnote(y) - sixteenth note at height y and current note position
define sixteenthnote X
	flag($1, 0.02 + w/2)
	eighthnote($1)
	X
### sixteenthrest(y) - sixteenth rest at height y and current note position
define sixteenthrest X
	eighthrest($1 - 2)
	n = n + 2 * w / 7
	eighthrest($1)
	X
### sixteenthperc(y) - sixteenth percussion note, height y, cur note position
define sixteenthperc X
	flag($1, 0.02 + w/2)
	eighthperc($1)
	X
### thirtysecondnote(y) - thirtysecond note at height y and current note position
define thirtysecondnote X
	flag($1, 0.02 + w)
	sixteenthnote($1)
	X
### thirtysecondrest(y) - thirtysecond rest, height y, current note position
define thirtysecondrest X
	sixteenthrest($1)
	n = n + 2 * w / 7
	eighthrest($1 + 2)
	X
### thirtysecondperc(y) - thirtysecond percussion note at y, cur note position
define thirtysecondperc X
	flag($1, 0.02 + w)
	sixteenthperc($1)
	X
### sixtyfourthnote(y) - sixtyfourth note at height y and current note position
define sixtyfourthnote X
	flag($1, 0.02 + 3*w/2)
	thirtysecondnote($1)
	X
### sixtyfourthrest(y) - sixtyfourth rest, height y, current note position
define sixtyfourthrest X
	eighthrest($1 - 4)
	n = n + 2 * w / 7
	thirtysecondrest($1)
	X
### sixtyfourthperc(y) - sixtyfourth percussion note at y, cur note position
define sixtyfourthperc X
	flag($1, 0.02 + 3*w/2)
	thirtysecondperc($1)
	X
### dtd(y) - dot for dotted notes at y, cur note position
define dtd X
.ps +12
	circle radius 0.015i at (n + w, ($1) * s)
	circle radius 0.005i at (n + w, ($1) * s)
.ps -12
	X
### sharp(y) - sharp sign at y, cur note position
define	sharp X
.ps +12
	line from (n - w/6, ($1 + 2) * s) down 4 * s
	line from (n + w/6, ($1 + 2) * s) down 4 * s
	line from (n - w/3, ($1 - 1.5) * s) right 2*w/3 up s
	line from (n - w/3, ($1 + 0.5) * s) right 2*w/3 up s
.ps -12
	X
### natural(y) - natural sign at y, cur note position
define	natural X
.ps +12
	line from (n - w/6, ($1 + 2) * s) down 3 * s then right w/3 up s/2
	line from (n + w/6, ($1 - 2) * s) up 3 * s then left w/3 down s/2
.ps -12
	X
### flat(y) - flat sign at y, cur note position
define	flat X
.ps +12
	line from (n - w/3, ($1 + 2.5) * s) down 3.75 * s \
		then right 2*w/3 up 1.25*s
	arc at (n, ($1) * s) \
		from (n + w/3, ($1) * s) to (n - w/3, ($1) * s) rad w/3
.ps -12
	X
### dtie(x1,y1, x2,y2, "3") tie that bows down
define dtie X
	dbow($1+w/2, ($2-1)*s, $3-w/2, ($4-1)*s, $5)
	X
### utie(x1,y1, x2,y2, "3") tie that bows up
define utie X
	ubow($1+w/2, ($2+1)*s, $3-w/2, ($4+1)*s, $5)
	X
### slur(x1,y1, x2,y2, "3") slurred, stem-up notes (bows down) (syn for dslur)
define slur X
	dbow($1-w/2, ($2-2)*s, $3+w/2, ($4-2)*s, $5)
	X
### dslur(x1,y1, x2,y2, "3") slurred, stem-up notes (bows down)
define dslur X
	dbow($1-w/2, ($2-2)*s, $3+w/2, ($4-2)*s, $5)
	X
### uslur(x1,y1, x2,y2, "3") slurred, stem-down notes (bows up)
define uslur X
	ubow($1-w/2, ($2+2)*s, $3+w/2, ($4+2)*s, $5)
	X
### slet(x1,y1, x2,y2, "3") arc over beamed triplet, quintuplet, etc. (bows up)
define slet X
	ubow($1-w/2, ($2+9)*s, $3+w/2, ($4+9)*s, $5)
	X
### ubow(slx1,sly1, slx2,sly2, "3") tie or slur that bows up
define ubow X
.ps +4
	sldx = $3 - $1
	slxmid = ($1 + $3) / 2
	slymid = ($2 + $4) / 2
# to make the arc be d high use slymid - (sldx*sldx/(8*d) + d/2); we use d=0.09
	arc at (slxmid, slymid - (sldx*sldx/0.72-0.045)) \
	    from ($3, $4) to ($1, $2)
	move to (slxmid, slymid) $5
.ps -4
	X
### dbow(slx1,sly1, slx2,sly2, "3") tie or slur that bows down
define dbow X
.ps +4
	sldx = $3 - $1
	slxmid = ($1 + $3) / 2
	slymid = ($2 + $4) / 2
# to make the arc be d low use slymid + (sldx*sldx/(8*d) + d/2); we use d=0.09
	arc at (slxmid, slymid + (sldx*sldx/0.72-0.045)) \
	    from ($1, $2) to ($3, $4)
	move to (slxmid, slymid) $5
.ps -4
	X
### blet(x1,y1, x2,y2, "3") bracket over triplet, quintuplet, etc. (unbeamed)
define blet X
.ps +4
	dy = 1.5 * h
	slx1 = $1 - w / 2; bly1 = ($2 + 8) * s
	slx2 = $3 + w / 2; bly2 = ($4 + 8) * s
	slxmid = (slx1 + slx2) / 2
	blymid = (bly1 + bly2) / 2
	line from (slx1, bly1) up dy then to (slx2, bly2 + dy) then down dy
	move to (slxmid, blymid) $5
.ps -4
	X
### bblet(x1,y1, x2,y2, "3") broken bracket over triplet, quintuplet, etc.
define bblet X
.ps +4
	dy = h
	slx1 = $1 - w / 2; bly1 = ($2 + 8) * s
	slx2 = $3 + w / 2; bly2 = ($4 + 8) * s
	slxmid = (slx1 + slx2) / 2
	blymid = (bly1 + bly2) / 2 + dy
	dx2 = 0.05i
	dy2 = ((bly2 - bly1) * dx2) / (slx2 - slx1)
	line from (slx1, bly1) up dy then to (slxmid - dx2, blymid - dy2)
	line from (slx2, bly2) up dy then to (slxmid + dx2, blymid + dy2)
	move to (slxmid, blymid - dy / 2) $5
.ps -4
	X

### seq(x, y) part of a slur, slet, or blet sequence
define seq X
	if (sbegx == 0) then Y
	    sbegx = $1; sbegy = $2; smaxy = $2
	    Y
	sendx = $1; sendy = $2
	if (sendy > smaxy) then Y
	    smaxy = sendy
	    Y
	X
### eseq(type, sym) end sequence, type=slur|slet|blet, sym = "3"
define eseq X
	if (sbegx > 0) then Y
	    if (smaxy > sbegy && smaxy > sendy) then Z
		dy = smaxy - sendy
		if (sbegy > sendy) then A
		    dy = smaxy - sbegy
		    A
		sbegy = sbegy + dy
		sendy = sendy + dy
		Z
	    $1(sbegx,sbegy, sendx, sendy, $2)
	    sbegx=0
	    Y
	X
### lyric(y, text) - lyric text at height y and current note position
define lyric X
.ps +4
	move to (n,  ($1) * s) $2
.ps -4
	X
### beam28(blh,brh) - Beam 2 8th notes with spec. end heights (uses N0 - N1)
# Each note should be a blacknote() and should be followed with "N0: Stem"
define	beam28 X
	Btl: (N0.x,($1-0.3)*s)
	Btr: (N1.x,($2-0.3)*s)
	bw=N1.x-N0.x
.ps +18
	line from Btl to Btr
.ps -10
	line from N0 to (N0.x-N0.x)/bw <Btl, Btr>
	line from N1 to (N1.x-N0.x)/bw <Btl, Btr>
.ps -8
X
### beam216(blh,brh) - Beam 2 16th notes with spec. end heights (uses N0 - N1)
# Each note should be a blacknote() and should be followed with "N0: Stem"
define	beam216 X
	beam28($1,$2)
	Bbl: (N0.x,($1-2.3)*s)
	Bbr: (N1.x,($2-2.3)*s)
	bw=N1.x-N0.x
.ps +18
	line from Bbl to Bbr
.ps -18
X
### beam232(blh,brh) - Beam 2 32nd notes with spec. end heights (uses N0 - N1)
define	beam232 X
	beam216($1,$2)
	Bbl: (N0.x,($1-4.3)*s)
	Bbr: (N1.x,($2-4.3)*s)
	bw=N1.x-N0.x
.ps +18
	line from Bbl to Bbr
.ps -18
X
### beam48(blh,brh) - Beam 4 8th notes with spec. end heights (uses N0 - N3)
# Each note should be a blacknote() and should be followed with "N0: Stem"
define	beam48 X
	Btl: (N0.x,($1-0.3)*s)
	Btr: (N3.x,($2-0.3)*s)
	bw=N3.x-N0.x
.ps +18
	line from Btl to Btr
.ps -10
	line from N0 to (N0.x-N0.x)/bw <Btl, Btr>
	line from N1 to (N1.x-N0.x)/bw <Btl, Btr>
	line from N2 to (N2.x-N0.x)/bw <Btl, Btr>
	line from N3 to (N3.x-N0.x)/bw <Btl, Btr>
.ps -8
X
### beam416(blh,brh) - Beam 4 16th notes with spec. end heights (uses N0 - N3)
define	beam416 X
	beam48($1,$2)
	Bbl: (N0.x,($1-2.3)*s)
	Bbr: (N3.x,($2-2.3)*s)
	bw=N3.x-N0.x
.ps +18
	line from Bbl to Bbr
.ps -18
X
### beam432(blh,brh) - Beam 4 32nd notes with spec. end heights (uses N0 - N3)
define	beam432 X
	beam416($1,$2)
	Bbl: (N0.x,($1-4.3)*s)
	Bbr: (N3.x,($2-4.3)*s)
	bw=N3.x-N0.x
.ps +18
	line from Bbl to Bbr
.ps -18
X
### beam88(blh,brh) - Beam 8 8th notes with spec. end heights (uses N0 - N7)
# Each note should be a blacknote() and should be followed with "N0: Stem"
define	beam88 X
	Btl: (N0.x,($1-0.3)*s)
	Btr: (N7.x,($2-0.3)*s)
	bw=N7.x-N0.x
.ps +18
	line from Btl to Btr
.ps -10
	line from N0 to (N0.x-N0.x)/bw <Btl, Btr>
	line from N1 to (N1.x-N0.x)/bw <Btl, Btr>
	line from N2 to (N2.x-N0.x)/bw <Btl, Btr>
	line from N3 to (N3.x-N0.x)/bw <Btl, Btr>
	line from N4 to (N4.x-N0.x)/bw <Btl, Btr>
	line from N5 to (N5.x-N0.x)/bw <Btl, Btr>
	line from N6 to (N6.x-N0.x)/bw <Btl, Btr>
	line from N7 to (N7.x-N0.x)/bw <Btl, Btr>
.ps -8
X
### beam816(blh,brh) - Beam 8 16th notes with spec. end heights (uses N0 - N7)
define	beam816 X
	beam88($1,$2)
	Bbl: (N0.x,($1-2.3)*s)
	Bbr: (N7.x,($2-2.3)*s)
	bw=N7.x-N0.x
.ps +18
	line from Bbl to (N3.x-N0.x)/bw <Bbl, Bbr>
	line from (N4.x-N0.x)/bw <Bbl, Bbr> to Bbr
.ps -18
X
### beam832(blh,brh) - Beam 8 32nd notes with spec. end heights (uses N0 - N7)
define	beam832 X
	beam816($1,$2)
	Bbl: (N0.x,($1-4.3)*s)
	Bbr: (N7.x,($2-4.3)*s)
	bw=N7.x-N0.x
.ps +18
	line from Bbl to (N3.x-N0.x)/bw <Bbl, Bbr>
	line from (N4.x-N0.x)/bw <Bbl, Bbr> to Bbr
.ps -18
X
### measrpt(y) - Measure repeat symbol at height y (6, -6) & cur. note position
define measrpt X
.ps +24
	line from (n - 2 * s, ($1 - 2) * s) up 4 * s right 4 * s
	box ht 0.5 * s wid 0.5 * s at (n - 1.75 * s, ($1 + 1.75) * s)
	box ht 0.5 * s wid 0.5 * s at (n + 1.75 * s, ($1 - 1.75) * s)
.ps -24
X
### trill(y) - Trill symbol at height y (6, -6) & cur. note position
define trill X
	"\fItr\fP" at (n, $1 * s)
.ps +24
	spline from (n + 2 * s, $1 * s) up s/2 right s/2 \
		then down s right s then up s right s \
		then down s right s then up s right s \
		then down s right s then up s right s \
		then down s right s then up s right s
.ps -24
X
