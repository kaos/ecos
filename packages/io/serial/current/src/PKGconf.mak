#===============================================================================
#
#    makefile
#
#    io/serial/src
#
#===============================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.1 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://www.redhat.com/                                                   
#                                                                          
# Software distributed under the License is distributed on an "AS IS"      
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

PACKAGE       := io_serial
include ../../../../pkgconf/pkgconf.mak
include ../../../../pkgconf/system.mak

LIBRARY         := libtarget.a
EXTRAS_COMPILE  := common/serial.c common/tty.c common/haldiag.c
ifdef CYGPKG_HAL_ARM_PID
EXTRAS_COMPILE  += arm/pid_serial_with_ints.c
endif
ifdef CYGPKG_HAL_ARM_AEB
EXTRAS_COMPILE  += arm/aeb_serial.c
endif
ifdef CYGPKG_HAL_ARM_EDB7XXX
EXTRAS_COMPILE  += arm/edb7xxx_serial.c
endif
ifdef CYGPKG_HAL_ARM_CMA230
EXTRAS_COMPILE  += arm/cma230_serial.c
endif
ifdef CYGPKG_HAL_MN10300_AM31
EXTRAS_COMPILE  += mn10300/mn10300_serial.c
endif
ifdef CYGPKG_HAL_MIPS_TX39
EXTRAS_COMPILE  += mips/tx3904_serial.c
endif
ifdef CYGPKG_HAL_POWERPC_COGENT
EXTRAS_COMPILE  += powerpc/cogent_serial_with_ints.c
endif
ifdef CYGPKG_HAL_QUICC
EXTRAS_COMPILE  += powerpc/quicc_smc_serial.c
endif
ifdef CYGPKG_HAL_SPARCLITE_SLEB
EXTRAS_COMPILE  += sparclite/sleb_sdtr.c
endif
OTHER_OBJS    :=
OTHER_CLEAN   :=

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

