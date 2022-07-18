# Makefile for da (by SAU) Stand alone SPARC version

CFLAGS= $(FLAG) -I../include 
LIB=../audlib.a
INSDIR=../bin

ALL=da

OBJS= \
		checksum.o da.o da_dx7_sx.o da_dx7_voice.o da_head_sx.o \
		da_iota_sx.o da_midi_cmd.o da_mirage_sx.o da_roland_sx.o da_sx.o \
		da_vdx7.o dx7_globs.o dx7_reset.o midi_pk.o midierror.o \
		get_ctl_name.o sx_dx7_32v.o sx_manuf.o mpuset.o \
		sx_dx7_1v.o mpusettrack.o getdx7ccname.o \
		mirage_globs.o read_ch.o getmpucmd.o

all:	$(ALL)

da:	up $(OBJS) $(LIB)
		$(CC) -o da $(OBJS) $(LIB)
		mv da ..

up:	
		cd ..; $(MAKE) audlib.a

###################################

install:
	-mkdir $(INSDIR)
	cp $(ALL) $(INSDIR)

clean:
	rm -f *.o core foo*

clobber:
		rm -f $(ALL)
