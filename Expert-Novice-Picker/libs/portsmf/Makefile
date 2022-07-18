# Makefile for portsmf - a library for Allegro music representation and
#     Standard MIDI File IO
#
# Roger B. Dannenberg
# July 2022

LIBOBJS = allegro.o allegrosmfrd.o allegrord.o mfmidi.o \
	  strparse.o allegrowr.o allegrosmfwr.o

all: libportsmf.a

$(LIBOBJS): %.o: %.cpp
	g++ -c -o $@ $<

libportsmf.a: $(LIBOBJS)
	ar rcs libportsmf.a $(LIBOBJS)

clean:
	rm $(LIBOBJS)
