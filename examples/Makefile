# Mostly written by Jonathan Larmour, Red Hat, Inc.
# This file is in the public domain and may be used for any purpose

# PKG_INSTALL_DIR might need to be edited.  Right now it is set
# assuming that a user ran pkgconf.tcl in /cygdrive/c/ecos-work on Windows NT,
# or used the Configuration Tool with C:\ecos-work as a build-tree.
#
# You can also override it on the make command-line, e.g.:
#   make PKG_INSTALL_DIR=/myecc/install
# or you can set it in your environment

PKG_INSTALL_DIR = /cygdrive/c/ecos-work/install

# You must also set XCC to the name of your cross-compiler, including any
# options it needs.

# Uncomment one of the below, or invoke make with the name of the compiler
# you want, e.g.:
#   make XCC="sparclite-elf-gcc -mcpu=sparclite -msoft-float"
# You can also set XCC in your environment

#XCC = mn10300-elf-gcc                                 # MN10300/AM31
#XCC = mn10300-elf-gcc -mam33                          # AM33
#XCC = mips-tx39-elf-gcc                               # MIPS TX39
#XCC = mips-tx49-elf-gcc -mips2 -EL                    # MIPS TX49 LITTLE-ENDIAN
#XCC = mips-tx49-elf-gcc -mips2 -EB                    # MIPS TX49 BIG-ENDIAN
#XCC = mips64vr4300-elf-gcc -mgp32 -EB                 # MIPS VR4300
#XCC = sh-elf-gcc -m3 -mb                              # SH3 BIG-ENDIAN
#XCC = sh-elf-gcc -m3 -ml                              # SH3 LITTLE-ENDIAN
#XCC = powerpc-eabi-gcc -msoft-float -mcpu=860         # POWERPC
#XCC = arm-elf-gcc -mcpu=arm7di                        # AEB
#XCC = arm-elf-gcc -mcpu=arm7tdmi                      # PID7
#XCC = arm-elf-gcc -mcpu=arm7tdmi -mbig-endian         # PID7 BIG-ENDIAN
#XCC = arm-elf-gcc -mcpu=arm9                          # PID9
#XCC = thumb-elf-gcc -mthumb-interwork                 # PID7 THUMB
#XCC = thumb-elf-gcc -mthumb-interwork -mbig-endian    # PID7 THUMB BIG-ENDIAN
#XCC = arm-elf-gcc -mcpu=arm710c -D__CL7111            # CIRRUS LOGIC CL7111
#XCC = arm-elf-gcc -mcpu=arm7tdmi -D__EDB7211          # CIRRUS LOGIC EDB7211
#XCC = arm-elf-gcc -mcpu=arm7tdmi -D__EDB7209          # CIRRUS LOGIC EDB7209
#XCC = arm-elf-gcc -mcpu=arm7tdmi -D__EDB7209 -D__EDB7212 # CIRRUS LOGIC EDB7212
#XCC = arm-elf-gcc -mcpu=arm7tdmi                      # ARM COGENT CMA230
#XCC = arm-elf-gcc -mcpu=arm7tdmi -D__CMA222           # ARM COGENT CMA222
#XCC = thumb-elf-gcc                                   # ARM COGENT CMA230 THUMB
#XCC = arm-elf-gcc -mcpu=strongarm                     # EBSA285
#XCC = arm-elf-gcc -mcpu=strongarm1100                 # ASSABET/BRUTUS
#XCC = sparclite-elf-gcc -msoft-float -mcpu=sparclite  # SPARCLITE
#XCC = i686-pc-linux-gnu-gcc                           # SYNTHETIC LINUX
#XCC = i386-elf-gcc                                    # i386 PC
#XCC = v850-elf-gcc                                    # NEC v850

###### VARIABLES
# Any of these can be overriden on the command-line or in your environment

ifeq ($(findstring sh-elf-gcc,$(XCC)),sh-elf-gcc)
CFLAGS        = -ggdb
else
CFLAGS        = -g
endif

CXXFLAGS      = $(CFLAGS)

EXTRACFLAGS   = -Wall -I$(PKG_INSTALL_DIR)/include -ffunction-sections -fdata-sections

EXTRACXXFLAGS = $(EXTRACFLAGS) -fno-exceptions -fno-rtti -fvtable-gc -finit-priority

LDFLAGS       = -nostartfiles -L$(PKG_INSTALL_DIR)/lib -Wl,--gc-sections
LIBS          = -Ttarget.ld -nostdlib

LD            = $(XCC)
XCXX          = $(XCC)

###### RULES

.PHONY: all clean CCCHECK

all: hello twothreads simple-alarm serial

clean:
	-rm -f hello hello.o twothreads twothreads.o
	-rm -f simple-alarm simple-alarm.o serial serial.o
	-rm -f instrument-test instrument-test.o

CCCHECK:
ifeq ($(XCC),)
	@echo You must set XCC to the name of your cross-compiler
	@false
endif


%.o: %.c
	$(XCC) -c -o $*.o $(CFLAGS) $(EXTRACFLAGS) $<

%.o: %.cxx
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(EXTRACXXFLAGS) $<

%.o: %.C
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(EXTRACXXFLAGS) $<

%.o: %.cc
	$(XCXX) -c -o $*.o $(CXXFLAGS) $(EXTRACXXFLAGS) $<

hello: CCCHECK hello.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

twothreads: CCCHECK twothreads.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

simple-alarm: CCCHECK simple-alarm.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

serial: CCCHECK serial.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

instrument-test: CCCHECK instrument-test.o
	$(LD) $(LDFLAGS) -o $@ $@.o $(LIBS)

