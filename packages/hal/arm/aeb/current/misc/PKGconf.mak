#===============================================================================
#
#    makefile
#
#    hal/arm/aeb/misc
#
#===============================================================================
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
#===============================================================================

#
# This makefile is quite complicated because it needs to generate the resulting
# image file twice.  This is necessary to compute the FLASH/ROM checksum needed
# by the AEB-1 monitor module structure.
#

PACKAGE       := hal_arm_aeb
include ../../../../../pkgconf/pkgconf.mak

PROGS := gdb_module_ncs
OTHER_PROGS := gdb_module.img gdb_module.img.UU
WHEREAMI := misc

CKSUM = 0

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.prv

SRC := $(COMPONENT_REPOSITORY)/$($(PACKAGE)_DIR)/misc

gdb_module.cs: gdb_module_ncs.stamp
	$(OBJCOPY) --strip-all gdb_module_ncs gdb_module.img.XX
	$(OBJCOPY) --change-addresses=0xFBFF4000 gdb_module.img.XX  gdb_module.img
	$(OBJCOPY) -O binary gdb_module.img gdb_module.img.raw
	make $(PACKAGE)_gdb_module.o CKSUM=`$(SRC)/flash_cksum.tcl gdb_module.img.raw`
	$(RM) -f gdb_module.img.XX gdb_module.img.raw

$(PACKAGE)_gdb_module.o: $(SRC)/gdb_module.c gdb_module_ncs.stamp
	$(CC)  -c $(INCLUDE_PATH) $(CFLAGS) -o $(PACKAGE)_gdb_module.o \
		$(SRC)/gdb_module.c -DCHECKSUM=$(CKSUM)

gdb_module.img: gdb_module.cs gdb_module.stamp
	$(OBJCOPY) --strip-debug gdb_module gdb_module.img.XX
	$(OBJCOPY) --change-addresses=0xFBFF4000 gdb_module.img.XX  gdb_module.img
	$(OBJCOPY) -O binary gdb_module.img gdb_module.img.raw
	$(RM) -f gdb_module.img.XX

gdb_module.img.UU: gdb_module.img
	uuencode gdb_module.img.raw gdb_module.img.raw | tr '`' ' ' >gdb_module.img.UU

