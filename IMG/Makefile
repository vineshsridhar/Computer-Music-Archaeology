# Makefile for IMG (by SAU) Stand alone SPARC version and misc
# midi and audio tools written by PSL

# commonly used routines

# removed: b2i.o 
LIBOBJS=\
	key2name.o myatoi.o \
	sopen.o statinfo.o strcopy.o \
	strpeel.o putmcmd.o getmcmd.o 

#	commands needed to run IMG
BSALL=\
	bssa mkcc acca accl inst cntl mpuclean merge

# other sound processing tools
AUDIO=\
	 aecho agc avmap gate mpu2aud mumerge norm plunk reverb squelch

# midi processing tools
MIDI=\
	chmap da invert just mecho select transpose unjust

# conversion tools
# removed mpu2aud mpu2sun plunk smf2mpu
CONV=\
	mpu2smf

CFILES= \
	Gate.c acca.c accl.c aecho.c agc.c \
	avmap.c b2i.c banjoplay.c bscomp.c bssa.c \
	bssetup.c bsstyle.c chmap.c cntl.c db2d.c \
	gate.c getmcmd.c getsmcmd.c inst.c invert.c \
	just.c k2p.c key2name.c mecho.c merge.c \
	mkcc.c mozart.c mpu2aud.c mpu2smf.c mpu2sun.c \
	mpuclean.c mumerge.c myatoi.c norm.c pluck.c \
	plunk.c putamcmd.c putmcmd.c putsmcmd.c reverb.c \
	select.c smf2mpu.c smfvar.c sopen.c squelch.c \
	statinfo.c strcopy.c strpeel.c transpose.c unjust.c

OTHERSRC= \
	README Makefile audio.h

# removed mpu2aud.o mpu2sun.o plunk.o smf2mpu.o
OFILES= \
	Gate.o acca.o accl.o aecho.o agc.o \
	avmap.o b2i.o banjoplay.o bscomp.o bssa.o \
	bssetup.o bsstyle.o chmap.o cntl.o db2d.o \
	gate.o getmcmd.o getsmcmd.o inst.o invert.o \
	just.o k2p.o key2name.o mecho.o merge.o \
	mkcc.o mozart.o mpu2smf.o \
	mpuclean.o mumerge.o myatoi.o norm.o pluck.o \
	putamcmd.o putmcmd.o putsmcmd.o reverb.o \
	select.o smfvar.o sopen.o squelch.o \
	statinfo.o strcopy.o strpeel.o transpose.o unjust.o

SUBDIRS=etc include obj da tunes sys

# removed $(AUDIO):
ALL=$(BSALL) $(MIDI) $(CONV)
FLAG=-g
CFLAGS= $(FLAG) -I./include 
LIB=audlib.a

#	BASE sets the base of the installation directory

BASE=`pwd`
INSDIR=$(BASE)/bin
MKCCBASE=$(BASE)/etc
ACCABASE=$(BASE)/obj

all:	$(LIB) $(ALL)

$(LIB):	$(LIBOBJS)
			$(CC) -c $?
			ar rv $(LIB) $?
			ranlib $(LIB)

bssa: mkcc acca accl inst cntl mpuclean merge
bssa:	bssa.o bscomp.o bssetup.o bsstyle.o $(LIB)
		$(CC) -o bssa bssa.o bscomp.o bssetup.o bsstyle.o $(LIB)

Mkcc:	mkcc.o $(LIB)
		$(CC) -o mkcc mkcc.o $(LIB)

mkcc:	
		$(MAKE) Mkcc "CFLAGS=$(CFLAGS) -DBASE=\\\"$(MKCCBASE)\\\""

# removed from dependency and link: getmcmd.o putmcmd.o, but these are in $(LIB)!
Acca:	acca.o $(LIB)
		$(CC) -o acca acca.o $(LIB)

acca:	
		$(MAKE) Acca "CFLAGS=$(CFLAGS) -DBSDIR=\\\"$(ACCABASE)\\\""

# removed from dependencies and link: putmcmd.o
accl:	accl.o banjoplay.o mozart.o $(LIB)
		$(CC) -o accl accl.o banjoplay.o mozart.o $(LIB)

inst:	inst.o
		$(CC) -o inst inst.o

cntl:	cntl.o
		$(CC) -o cntl cntl.o

mpuclean:	mpuclean.o
		$(CC) -o mpuclean mpuclean.o

# removed from dependencies and link: getmcmd.o putmcmd.o, but these are in $(LIB)!
merge:	merge.o $(LIB)
		$(CC) -o merge merge.o $(LIB)

###################################

# removed from dependencies and link: getmcmd.o, but this is in $(LIB)!
# mpu2aud:	mpu2aud.o pluck.o db2d.o k2p.o $(LIB)
# 		$(CC) -o mpu2aud mpu2aud.o pluck.o db2d.o k2p.o $(LIB) -lm

agc:	agc.o $(LIB)
		$(CC) -o agc agc.o $(LIB) -lm

gate:	gate.o Gate.o $(LIB)
		$(CC) -o gate gate.o Gate.o $(LIB) -lm

mumerge:	mumerge.o $(LIB)
		$(CC) -o mumerge mumerge.o $(LIB) -lm

norm:	norm.o $(LIB)
		$(CC) -o norm norm.o $(LIB) -lm

#plunk:	plunk.o pluck.o k2p.o $(LIB)
#		$(CC) -o plunk plunk.o pluck.o k2p.o $(LIB)

avmap:	avmap.o  db2d.o $(LIB)
		$(CC) -o avmap avmap.o  db2d.o $(LIB) -lm

aecho:	aecho.o $(LIB)
		$(CC) -o aecho aecho.o $(LIB) -lm

reverb:	reverb.o $(LIB)
		$(CC) -o reverb reverb.o $(LIB) -lm

squelch:	squelch.o Gate.o $(LIB)
		$(CC) -o squelch squelch.o Gate.o $(LIB) -lm

select:	select.o $(LIB)
		$(CC) -o select select.o $(LIB)

transpose:	transpose.o $(LIB)
		$(CC) -o transpose transpose.o $(LIB)

chmap:	chmap.o $(LIB)
		$(CC) -o chmap chmap.o $(LIB)

invert:	invert.o $(LIB)
		$(CC) -o invert invert.o $(LIB)

mecho:	mecho.o $(LIB)
		$(CC) -o mecho mecho.o $(LIB)

unjust:	unjust.o putamcmd.o $(LIB)
		$(CC) -o unjust unjust.o putamcmd.o $(LIB)

just:	just.o $(LIB)
		$(CC) -o just just.o $(LIB)

mpu2smf:	mpu2smf.o putsmcmd.o $(LIB)
		$(CC) -o mpu2smf mpu2smf.o putsmcmd.o $(LIB)

#smf2mpu:	smf2mpu.o smfvar.o getsmcmd.o $(LIB)
#		$(CC) -o smf2mpu smf2mpu.o smfvar.o getsmcmd.o $(LIB)

#mpu2sun:	mpu2sun.o $(LIB)
#		$(CC) -o mpu2sun mpu2sun.o $(LIB)

da:
		cd Da; $(MAKE) FLAG=$(FLAG)
###################################

$(ALL):	Makefile

install:
	-mkdir $(INSDIR)
	cp $(ALL) $(INSDIR)

clean:
	rm -f *.o core foo*
	cd Da; $(MAKE) clean

clobber:
		rm -f $(ALL) $(LIB)
		cd Da;	$(MAKE) clobber

list_bin:
	@for i in $(ALL); do\
		echo $$i;\
	done
