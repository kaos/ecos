#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	REF4955/TX4955 board assembler header file
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
## Author(s): 	nickg
## Contributors:nickg,jskov
## Date:	2000-05-15
## Purpose:	REF4955/TX4955 board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the REF4955/TX4955 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/mips.inc>
#include <cyg/hal/plf_defs.inc>


#------------------------------------------------------------------------------
# Macro for copying vectors to RAM if necessary.
#if !defined(CYGSEM_HAL_USE_ROM_MONITOR)

	.macro	hal_vectors_init
        # If we don~t play nice with a ROM monitor, copy the required
        # vectors into the proper location.
	la	t0,0x80000000		# dest addr
	la	t1,utlb_vector		# source addr
	la	t3,utlb_vector_end	# end dest addr
1:	
	lw	v0,0(t1)		# get word
	addi	t1,t1,4
	sw	v0,0(t0)		# write word
	addi	t0,t0,4
	bne	t1,t3,1b
	 nop

	la	t0,0x80000180		# dest addr
	la	t1,other_vector		# source addr
	la	t3,other_vector_end	# end dest addr
1:	
	lw	v0,0(t1)		# get word
	addi	t1,t1,4
	sw	v0,0(t0)		# write word
	addi	t0,t0,4
	bne	t1,t3,1b
	 nop

        .set mips3                      # Set ISA to MIPS 3 to allow cache insns
	# Now clear the region in the caches
	la	t0,0x80000000		# dest addr
	ori	t1,t0,0x200		# source addr
1:	cache	0x01,0(t0)	        # Flush word from data cache
	cache	0x01,1(t0)
	cache	0x01,2(t0)
	cache	0x01,3(t0)
	nop
	cache	0x00,0(t0)		# Invalidate icache for word
	cache	0x00,1(t0)
	cache	0x00,2(t0)
	cache	0x00,3(t0)
	nop
	addi	t0,t0,0x20
	bne	t0,t1,1b
	 nop
        .set mips0                      # reset ISA to default

	.endm

#else

	.macro	hal_vectors_init
	.endm

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
	hal_vectors_init
        # Set default exception VSR for all vectors
        ori     a0,zero,CYGNUM_HAL_VSR_COUNT
	la	a1,__default_exception_vsr
	la	a2,hal_vsr_table
1:      sw      a1,0(a2)
        addi    a2,a2,4
        addi    a0,a0,-1
        bne     a0,zero,1b
         nop

        # Now set special VSRs
       	la	a0,hal_vsr_table
        # Set interrupt VSR
	la	a1,__default_interrupt_vsr
        sw      a1,CYGNUM_HAL_VECTOR_INTERRUPT*4(a0)
        # Add special handler on breakpoint vector to allow GDB and
        # GCC to both use 'break' without conflicts.
	la	a1,__break_vsr_springboard
	sw	a1,CYGNUM_HAL_VECTOR_BREAKPOINT*4(a0)
        # Set exception handler on special vectors
        # FIXME: Should use proper definitions
	la	a1,__default_exception_vsr
	sw	a1,32*4(a0)             # debug
	sw	a1,33*4(a0)             # utlb
	sw	a1,34*4(a0)             # nmi
	.endm
	
#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

	# Initialize the VSR table entries
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	.macro	hal_mon_init
	hal_vectors_init
        # Set interrupt VSR
       	la	a0,hal_vsr_table
        la	a1,__default_interrupt_vsr
        sw      a1,CYGNUM_HAL_VECTOR_INTERRUPT*4(a0)
	.endm

#else

	.macro	hal_mon_init
	hal_vectors_init
	.endm

#endif

#define CYGPKG_HAL_MIPS_MON_DEFINED
#endif	

#------------------------------------------------------------------------------

#if !defined(CYG_HAL_STARTUP_RAM)
	.macro	hal_memc_init
        // Only initialize the SDRAM controller when running in ROM
        .extern hal_memc_setup
        lar     t0,hal_memc_setup
        jalr    t0
        nop
        .endm
#define CYGPKG_HAL_MIPS_MEMC_DEFINED        
#endif

#------------------------------------------------------------------------------
# Decide whether the VSR table is defined externally, or is to be defined
# here.

## ISR tables are defined in platform.S
#define CYG_HAL_MIPS_ISR_TABLES_DEFINED

## VSR table is at a fixed RAM address defined by the linker script
#define CYG_HAL_MIPS_VSR_TABLE_DEFINED

##-----------------------------------------------------------------------------
## For chaining, use the calculated cause vector number.

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
	.macro	hal_intc_translate inum,vnum
	move	\vnum,\inum			# Vector == interrupt number
	.endm
#define CYGPKG_HAL_MIPS_INTC_TRANSLATE_DEFINED
#endif		

##-----------------------------------------------------------------------------
#ifdef CYG_STARTUP_ROM

## Initial SR value for use in ROM:
## CP0 usable
## Vectors in RAM
## FP registers are 32 bit
## All hw ints disabled
#define INITIAL_SR	0x30000000

#else

## Initial SR value for use standalone:
## CP0 usable
## Vectors to RAM
## FP registers are 32 bit
## All hw ints disabled
#define INITIAL_SR	0x30000000

#endif                   

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
