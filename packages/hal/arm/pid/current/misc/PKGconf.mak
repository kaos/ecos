#===============================================================================
#
#    makefile
#
#    hal/arm/pid/misc
#
#===============================================================================
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
#===============================================================================

PACKAGE       := hal_arm_pid
include ../../../../../pkgconf/pkgconf.mak
include ../../../../../pkgconf/system.mak

ifdef CYG_HAL_STARTUP_STUBS
PROGS	      := gdb_module
OTHER_PROGS   := gdb_module.img
endif
ifdef CYG_HAL_STARTUP_RAM
PROGS	      := flash prog_flash
endif
WHEREAMI      := misc

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.prv

ifdef CYG_HAL_STARTUP_STUBS
gdb_module.img: gdb_module.stamp
	$(OBJCOPY) --strip-all gdb_module$(EXEEXT) gdb_module.img.XX
	$(OBJCOPY) --change-addresses=0xFC060000 gdb_module.img.XX  gdb_module.img
	$(OBJCOPY) -O binary gdb_module.img gdb_module.bin
	$(RM) -f gdb_module.img.XX
endif

