#==============================================================================
#
#    makefile
#
#    hal/powerpc/arch/src
#
#==============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
#==============================================================================

PACKAGE       := hal_arm
include ../../../../../pkgconf/pkgconf.mak

LIBRARY       := libtarget.a
COMPILE       := hal_misc.c vectors.S context.S arm_stub.c
OTHER_OBJS    :=
OTHER_TARGETS := vectors.stamp ldscript.stamp
OTHER_DEPS    := $(PACKAGE)_vectors.d ldscript.d
OTHER_CLEAN   := vectors.clean ldscript.clean

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

.PHONY: ldscript.clean

$(PACKAGE)_context.o $(PACKAGE)_vectors.o: arm.inc 

arm.inc: hal_mk_defs.c ../include/hal_arch.h
	$(CC) ${CFLAGS} $(INCLUDE_PATH) -S $<
	grep -w 'equ' hal_mk_defs.s | sed -e's/#//' >arm.inc
	$(RM) hal_mk_defs.s

vectors.stamp: $(PACKAGE)_vectors.o
	$(CP) $< $(PREFIX)/lib/vectors.o
	@$(TOUCH) $@

vectors.clean:
	$(RM) $(PREFIX)/lib/vectors.o $(PACKAGE)_vectors.o
	$(RM) vectors.stamp

ldscript.stamp: arm.ld
	$(CC) -E -P -Wp,-MD,ldscript.tmp -DEXTRAS=1 -xc $(INCLUDE_PATH) $(CFLAGS) -o $(PREFIX)/lib/target.ld $<
	@echo > ldscript.d
	@echo $@ ':' $< '\' >> ldscript.d
	@tail -n +2 ldscript.tmp >> ldscript.d
	@rm ldscript.tmp
	$(TOUCH) $@

ldscript.clean:
	$(RM) -f $(PREFIX)/lib/target.ld
	$(RM) -f ldscript.stamp



