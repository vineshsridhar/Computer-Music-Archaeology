.SUFFIXES:	.cc .awk $(SUFFIXES)
 
.awk.cc:
		awk -f $< >$@

AWK_SRC=accacc.awk accagc.awk accagg.awk
ALL=accacc.cc accagc.cc accagg.cc
DIR=etc

default: $(ALL)

clean:
			rm -f $(ALL)

