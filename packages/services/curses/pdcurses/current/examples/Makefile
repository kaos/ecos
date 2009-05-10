#
# Makefile for PDCurses eCos Application
#

.PHONY: all clean

# Platform specific setups
include Make.params

# Simple build rules

%.o: %.c
	$(CC) -c $*.c $(ACTUAL_CFLAGS) -I$(PREFIX)/include -o $@

# PDCurses CFLAGS
ACTUAL_CFLAGS := $(subst , -D__ECOS__,$(ACTUAL_CFLAGS))
ACTUAL_CFLAGS := $(subst , -D_LP64=0,$(ACTUAL_CFLAGS))

OBJS           = pdcecos_app.o pdcecos_init.o pdcecos_thread.o 
DST            = pdcecos_app

all: ${DST}

clean:
	-rm -f ${DST} ${OBJS}

${DST}: ${OBJS}
	$(CC) $(ACTUAL_LDFLAGS) -L$(PREFIX)/lib -Ttarget.ld -o $@ $^

