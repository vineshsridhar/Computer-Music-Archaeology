# Makefile for Expert-Novice-Picker
#
# This software derived from Peter Langston's original software, modified
# by Vinesh Sridhar and Roger B. Dannneberg

PORTSMF = ../libs/portsmf

LICKOBJS = lick0.o lick1d.o lick2.o lily.o adagio.o

LICKINCL = lick.h smf.h lily.h adagio.h

all: lick

smf.o: smf.cpp smf.h
	gcc -o smf.o -I$(PORTSMF) -c smf.cpp

$(LICKOBJS): %.o: %.c
	gcc -c -o $@ $< 

$(PORTSMF)/libportsmf.a:
	cd $(PORTSMF); make

lick: $(LICKOBJS) $(LICKINCL) smf.o $(PORTSMF)/libportsmf.a
	g++ -o lick -I$(PORTSMF) -L$(PORTSMF) $(LICKOBJS) smf.o -lportsmf

clean:
	rm -f $(LICKOBJS) smf.o
