#==============================================================================
#
#    makefile
#
#    hal/mips/tx39/src
#
#==============================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.0 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://sourceware.cygnus.com/ecos                                        
#                                                                          
# Software distributed under the License is distributed on an       
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
#==============================================================================

PACKAGE       := hal_mips_tx39
include ../../../../../pkgconf/pkgconf.mak

LIBRARY       := libtarget.a
COMPILE       := hal_diag.c var_misc.c variant.S
OTHER_OBJS    :=
OTHER_TARGETS := ldscript.stamp
OTHER_CLEAN   := ldscript.clean
OTHER_DEPS    := ldscript.d

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

.PHONY: ldscript.clean

EXTRAS = $(wildcard $(PREFIX)/lib/libextras.a)

ldscript.stamp: mips_tx39.ld
	$(CC) -E -P -Wp,-MD,ldscript.tmp -DEXTRAS=1 -xc $(INCLUDE_PATH) $(CFLAGS) -o $(PREFIX)/lib/target.ld $<
	@echo > ldscript.d
	@echo $@ ':' $< '\' >> ldscript.d
	@tail -n +2 ldscript.tmp >> ldscript.d
	@rm ldscript.tmp
	$(TOUCH) $@

ldscript.clean:
	$(RM) -f $(PREFIX)/lib/target.ld
	$(RM) -f ldscript.stamp
