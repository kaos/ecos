#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	Atlas board assembler header file
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
## Date:	2000-06-06
## Purpose:	Atlas board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the Atlas board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/mips.inc>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/plf_io.h>

#------------------------------------------------------------------------------
# Additional bits for status register.
# We set the IM[0] bit to accept all interrupts.

#define INITIAL_SR_PLF 0x00000400

#------------------------------------------------------------------------------

#define CYGPKG_HAL_RESET_VECTOR_FIRST_CODE
	.macro hal_reset_vector_first_code
        # Branch forward past the board ID register.
        b 1f
	nop
	nop
	nop
	nop
	nop
1:
	.endm

#define CYGPKG_HAL_EARLY_INIT
	.macro	hal_early_init
	# Do these initializations early (rather than in hal_memc_init) so
	# we have access to the LEDs on the board for debugging purposes.

        #
        # WARNING: THESE WRITES NEED TO HANDLE BYTE-SWAPPING PROPERLY WHEN DOING BIG-ENDIAN
        #

	#
        # Setup Galileo CPU Interface Register
        #
        # Set the WriteRate bit - ie Accept 'DDD' back-to-back transfers (see CoreLV "Users Manual")
        # All other bits stay the same
	#
 	li	k0, CYGARC_UNCACHED_ADDRESS(HAL_GALILEO_REGISTER_BASE)
	lw	k1, HAL_GALILEO_CPU_INTERFACE_CONFIG_OFFSET(k0)
	li	k0, HAL_GALILEO_CPU_WRITERATE_MASK
	or	k1, k1, k0
	li	k0, CYGARC_UNCACHED_ADDRESS(HAL_GALILEO_REGISTER_BASE)
	sw	k1, HAL_GALILEO_CPU_INTERFACE_CONFIG_OFFSET(k0)

	#
	# Use BootCS chip-select for the entire device bus region
	#
	li	k1, 0
	sw	k1, HAL_GALILEO_CS3_HIGH_DECODE_OFFSET(k0)
	li	k1, 0xf0
	sw	k1, HAL_GALILEO_CSBOOT_LOW_DECODE_OFFSET(k0)
	li	k1, 0xff
	sw	k1, HAL_GALILEO_CSBOOT_HIGH_DECODE_OFFSET(k0)
	.endm

#------------------------------------------------------------------------------
# Interrupt decode macros

# Only INTN[0] is connected on the Atlas board. We need to access the
# interrupt controller to get the actual vector number.

#ifndef CYGPKG_HAL_MIPS_INTC_INIT_DEFINED

	.macro	hal_intc_init
	mfc0	v0,status
	nop
	lui	v1,0xFFFF
	ori	v1,v1,0x04FF
	and	v0,v0,v1	# Clear the IntMask bits except IM[0]
	mtc0	v0,status
	nop
	nop
	nop
	.endm

#define CYGPKG_HAL_MIPS_INTC_INIT_DEFINED

#endif


#ifndef CYGPKG_HAL_MIPS_INTC_DECODE_DEFINED
	.macro	hal_intc_decode vnum

	lw	v0,HAL_ATLAS_INTSTATUS	# Get interrupt status reg
	clz	v1,v0			# count leading zeros into v1
	la	v0,31			# v0 = 31
	sub	\vnum,v0,v1		# vnum = 31-v1 == vector number	
	.endm

	.macro	hal_intc_decode_data
	.endm

#define CYGPKG_HAL_MIPS_INTC_DECODE_DEFINED

// We also define our own interrupt tables in platform.S...
#define CYG_HAL_MIPS_ISR_TABLES_DEFINED
			
#endif

#------------------------------------------------------------------------------
# Diagnostic macros

#ifndef CYGPKG_HAL_MIPS_DIAG_DEFINED

	.macro	hal_diag_init
	.endm

	.macro	hal_diag_excpt_start
	.endm

	.macro	hal_diag_intr_start
	.endm

	.macro	hal_diag_restore
	.endm

#define CYGPKG_HAL_MIPS_DIAG_DEFINED

#endif // ifndef CYGPKG_HAL_MIPS_DIAG_DEFINED
	
#------------------------------------------------------------------------------
# MEMC macros.
# 
	
#if defined(CYG_HAL_STARTUP_ROM)

#------------------------------------------------------------------------------
# Make sure the jump to _start in vectors.S is done uncached
#

#define CYGARC_START_FUNC_UNCACHED

	.macro	hal_memc_init
	.extern hal_atlas_init_sdram

	lar	k0,hal_atlas_init_sdram
	CYGARC_ADDRESS_REG_UNCACHED(k0)
	jalr	k0
	nop

	beqz	v0, 2f
	nop

	# Error in sizing memory
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS0, 'M')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS1, 'E')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS2, 'M')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS3, 'E')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS4, 'R')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS5, 'R')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS6, 'O')
 	DEBUG_ASCII_DISPLAY(HAL_DISPLAY_ASCIIPOS7, 'R')
1:	b	1b
	nop

2:	# No error in sizing memory

	# Store the memory size at the base of RAM for later
	la	k0, 0x80000000
        move	k1, k0
        CYGARC_ADDRESS_REG_UNCACHED(k1)
	sw	v1, 0(k1)
	nop
	nop
	nop
	.endm

#define CYGPKG_HAL_MIPS_MEMC_DEFINED

#elif defined(CYG_HAL_STARTUP_ROMRAM)
#error ROMRAM STARTUP NOT YET IMPLEMENTED
	.macro	hal_memc_init
	.extern	hal_memc_setup

	lar	k0,hal_memc_setup
	jalr	k0
	nop

	# Having got the RAM working, we must now relocate the Entire
	# ROM into it and then continue execution from RAM.

	la	t0,0x88000000		# dest addr
	la	t1,0x80000000		# source addr
	la	t3,__ram_data_end	# end dest addr
1:	
	lw	v0,0(t1)		# get word
	addi	t1,t1,4
	sw	v0,0(t0)		# write word
	addi	t0,t0,4
	bne	t0,t3,1b
	nop

	la	v0,2f			# RAM address to go to
	jr	v0
	nop
2:	
	# We are now executing out of RAM!

	.endm

#define CYGPKG_HAL_MIPS_MEMC_DEFINED

#endif	

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
