# Mostly written by Jonathan Larmour, Red Hat, Inc.
# Reference to ecos.mak added by John Dallaway, eCosCentric Limited, 2003-01-20
# This file is in the public domain and may be used for any purpose

# Usage:   make ECOS_INSTALL=/path/to/ecos/install

ECOS_INSTALL=$$(ECOS_INSTALL) # override on make command line

include $(ECOS_INSTALL)/include/pkgconf/ecos.mak

XCC           = $(ECOS_COMMAND_PREFIX)gcc
XCXX          = $(XCC)
XLD           = $(XCC)

CFLAGS        = -I$(ECOS_INSTALL)/include
CXXFLAGS      = $(CFLAGS)
LDFLAGS       = -nostartfiles -L$(ECOS_INSTALL)/lib -Ttarget.ld

# RULES

.PHONY: all clean

all: hello twothreads simple-alarm serial

clean:
	-rm -f hello hello.o twothreads twothreads.o
	-rm -f simple-alarm simple-alarm.o serial serial.o
	-rm -f instrument-test instrument-test.o

%.o: %.c
	$(XCC) -c -o $*.o $(CFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cxx
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.C
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

%.o: %.cc
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(ECOS_GLOBAL_CFLAGS) $<

hello: hello.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o

twothreads: twothreads.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o

simple-alarm: simple-alarm.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o

serial: serial.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o

instrument-test: instrument-test.o
	$(XLD) $(LDFLAGS) $(ECOS_GLOBAL_LDFLAGS) -o $@ $@.o
