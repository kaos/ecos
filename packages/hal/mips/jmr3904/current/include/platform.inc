#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	JMR-TX3904 board assembler header file
##
##=============================================================================
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1999-04-06
## Purpose:	JMR-TX3904 board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the JMR-TX3904 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/mips.inc>

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_MIPS_MON_DEFINED

	.macro	hal_mon_init
#if	defined(CYG_HAL_STARTUP_ROM) ||			\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYG_HAL_USE_ROM_MONITOR))
	
	# If we are starting up from ROM, or we are starting in
	# RAM and NOT using a ROM monitor, initialize the VSR table.

	la	a0,__default_interrupt_vsr
	la	a1,__default_exception_vsr
	la	a3,hal_vsr_table
	sw	a0,0(a3)
	sw	a1,1*4(a3)
	sw	a1,2*4(a3)
	sw	a1,3*4(a3)
	sw	a1,4*4(a3)
	sw	a1,5*4(a3)
	sw	a1,6*4(a3)
	sw	a1,7*4(a3)
	sw	a1,8*4(a3)
	sw	a1,9*4(a3)
	sw	a1,10*4(a3)
	sw	a1,11*4(a3)
	sw	a1,12*4(a3)
	sw	a1,13*4(a3)
	sw	a1,14*4(a3)
	sw	a1,15*4(a3)

	sw	a1,32*4(a3)
	sw	a1,33*4(a3)
	
#endif
		
#if defined(CYG_HAL_STARTUP_RAM) && defined(CYG_HAL_USE_ROM_MONITOR)

	# Initialize the VSR table entries
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	la	a0,__default_interrupt_vsr
	la	a3,hal_vsr_table
	sw	a0,0(a3)
		
#endif
			
	.endm

#define CYGPKG_HAL_MIPS_MON_DEFINED

#endif	

#------------------------------------------------------------------------------
# Decide whether the VSR table is defined externally, or is to be defined
# here.

#if defined(CYG_HAL_MIPS_SIM) ||	\
    ( defined(CYG_HAL_MIPS_TX3904) &&	\
      defined(CYG_HAL_STARTUP_RAM) &&	\
      !defined(CYG_HAL_USE_ROM_MONITOR) \
    )

## VSR table defined in linker script

#else

#define CYG_HAL_MIPS_VSR_TABLE_DEFINED

#endif

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
