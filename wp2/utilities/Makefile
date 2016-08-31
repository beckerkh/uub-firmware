CFLAGS = -g3 -Wall -O0 
DEBUG = -DDEBUG
LIBS = -lm

INCPATH		= -I.
LIBPATH		= -L.

# Adding ROOT files

#LIBS		+= $(shell root-config --glibs)
#INCPATH		+= -I$(ROOTSYS)/include -I$(ROOTSYS)/include/root


all:	single_muon_pulse steps_pulse


# Object files

single_muon_pulse.o:	$(SRCDIR)single_muon_pulse.c
	gcc $(DEBUG) $(CFLAGS) $(INCPATH) -c -o single_muon_pulse.o single_muon_pulse.c

steps_pulse.o:	$(SRCDIR)steps_pulse.c
	gcc $(DEBUG) $(CFLAGS) $(INCPATH) -c -o steps_pulse.o steps_pulse.c



# Executables

single_muon_pulse: single_muon_pulse.o
	gcc $(CFLAGS)  $(LIBS) -o single_muon_pulse single_muon_pulse.o

steps_pulse: steps_pulse.o
	gcc $(CFLAGS)  $(LIBS) -o steps_pulse steps_pulse.o

clean:
	 rm -f *.o


