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
PROGS         := flash prog_flash
OTHER_PROGS   := prog_flash.img
# And for BE support
PROGS         += prog_flash_BE_image_LE_system
OTHER_PROGS   += prog_flash_BE_image_LE_system.img
endif
WHEREAMI      := misc

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.prv

ifdef CYG_HAL_STARTUP_RAM
prog_flash.img: prog_flash.stamp
	$(OBJCOPY) --strip-debug prog_flash$(EXEEXT) prog_flash.img
endif

ifdef CYG_HAL_STARTUP_STUBS
gdb_module.img: gdb_module.stamp
	$(OBJCOPY) --strip-debug gdb_module$(EXEEXT) gdb_module.img.XX
	$(OBJCOPY) --change-addresses=0xFC060000 gdb_module.img.XX  gdb_module.img
	$(OBJCOPY) -O binary gdb_module.img gdb_module.bin
	$(RM) -f gdb_module.img.XX
endif

ifdef CYG_HAL_STARTUP_RAM
prog_flash_BE_image_LE_system.img: prog_flash_BE_image_LE_system.stamp
	$(OBJCOPY) --strip-debug prog_flash_BE_image_LE_system$(EXEEXT) prog_flash_BE_image_LE_system.img

hal_arm_pid_prog_flash_BE_image_LE_system.o: prog_flash.c
	$(CC) -DBE_IMAGE -c $(INCLUDE_PATH) $(CFLAGS) $($(notdir $<)-CFLAGS)  -Wp,-MD,$(@:.o=.tmp) -o $@ $< 
	@echo > $(@:.o=.d)
	@echo $@ ':' $< '\' >> $(@:.o=.d)
	@$(TAIL) +2 $(@:.o=.tmp) >> $(@:.o=.d)
	@$(RM) $(@:.o=.tmp)
endif
