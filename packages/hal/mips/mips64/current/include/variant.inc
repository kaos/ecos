#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	MIPS 32 family assembler header file
##
##=============================================================================
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	dmoseley
## Contributors:	dmoseley
## Date:	2001-01-30
## Purpose:	MIPS64 family definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the MIPS64 CPU family.
## Usage:
##		#include <cyg/hal/variant.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
	
#include <cyg/hal/mips.inc>

#include <cyg/hal/platform.inc>

#define CYGARC_HAL_COMMON_EXPORT_CPU_MACROS
#include <cyg/hal/mips-regs.h>
#include <cyg/hal/var_arch.h>

##-----------------------------------------------------------------------------
## Define CPU variant for architecture HAL.
			
#define	CYG_HAL_MIPS_MIPS64

# Set the KX bit to use 64 bit addressing in kernel mode.

#define INITIAL_SR_VAR 0x00000080

#------------------------------------------------------------------------------
# Cache macros.
        
#ifndef CYGPKG_HAL_MIPS_CACHE_DEFINED

        .macro  hal_cache_init

	# Setup a temporary stack pointer for running C code.
	la	a0,__interrupt_stack
	move	sp,a0
        CYGARC_ADDRESS_REG_UNCACHED(sp)
        
        # Read the CONFIG1 register into a0
        mfc0	a0, C0_CONFIG, 1
	nop
	nop
	nop

	# Jump to C-code to initialize caches (uncached)
	lar	k0, hal_c_cache_init
	CYGARC_ADDRESS_REG_UNCACHED(k0)
	jalr	k0
	nop
        .endm

#define CYGPKG_HAL_MIPS_CACHE_DEFINED

#endif  

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_MIPS_MON_DEFINED

#if	defined(CYG_HAL_STARTUP_ROM) ||			\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYGSEM_HAL_USE_ROM_MONITOR))
	# If we are starting up from ROM, or we are starting in
	# RAM and NOT using a ROM monitor, initialize the VSR table.

	.macro	hal_mon_init
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
	.endm
	
#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

	# Initialize the VSR table entries
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	.macro	hal_mon_init
	la	a0,__default_interrupt_vsr
	la	a3,hal_vsr_table
	sw	a0,0(a3)
	.endm

#else

	.macro	hal_mon_init
	.endm

#endif
			

#define CYGPKG_HAL_MIPS_MON_DEFINED

#endif	

#------------------------------------------------------------------------------
# Decide whether the VSR table is defined externally, or is to be defined
# here.

#if defined(CYGPKG_HAL_MIPS_SIM) ||        \
    ( defined(CYGPKG_HAL_MIPS_ATLAS) &&    \
      defined(CYG_HAL_STARTUP_RAM) &&      \
      !defined(CYGSEM_HAL_USE_ROM_MONITOR) \
    )

## VSR table defined in linker script

#else

#define CYG_HAL_MIPS_VSR_TABLE_DEFINED

#endif

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
