#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	TX39 family assembler header file
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
## Purpose:	TX39 family definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the TX39 CPU family.
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

##-----------------------------------------------------------------------------
## Define CPU variant for architecture HAL.
			
#define	CYG_HAL_MIPS_R3000A
#define	CYG_HAL_MIPS_R3900

##-----------------------------------------------------------------------------
## Indicate that the ISR tables are defined in variant.S

#define CYG_HAL_MIPS_ISR_TABLES_DEFINED

##-----------------------------------------------------------------------------
## TX39 Memory controller.

#ifndef CYGPKG_HAL_MIPS_MEMC_DEFINED

## ROM timing characteristics are dependent on the clock speed.
		
#if (CYGHWR_HAL_MIPS_CPU_FREQ == 50)
#define ROM_CCR0_INIT	0x00000420
#define DRAM_DREFC_INIT	0x00000180
#define DRAM_DWR0_INIT  0x00111111
#elif (CYGHWR_HAL_MIPS_CPU_FREQ == 66)
#define ROM_CCR0_INIT	0x00000520
#define DRAM_DREFC_INIT	0x00000200
#define DRAM_DWR0_INIT  0x00332222
#else
#error Unsupported clock frequency		
#endif

## DRAM configuration is dependent on the DRAM device used.
## for 16MByte (4MBit (x4bit) x 8)           0x08024030
## for 4MByte (1MBit (x4bit) x 8)            0x08013020
## for 8MByte (1MBit (x4bit) x 8 x 2 banks)  0x08013020

#if defined CYGHWR_HAL_TX39_JMR3904_DRAM_CONFIG_INIT
#define DRAM_CONFIG_INIT CYGHWR_HAL_TX39_JMR3904_DRAM_CONFIG_INIT
#else
#define DRAM_CONFIG_INIT 0x08024030
#endif

## Enabling timeout exceptions can result in bogus exceptions under the
## following conditions:
##    o half speed bus mode (JMR board uses this mode)
##    o code resides on 0 wait SRAM
##    o I- and D- caches are enabled
##    o a very narrow timing condition of cache refill cycle (not
##      descibed here)
## The simple solution is to configure 1 cycle wait state SRAM rather
## than zero.

#ifdef CYGHWR_HAL_MIPS_TX39_JMR3904_ENABLE_TOE
#define SRAM_WAIT_INIT 0x00000100
#else
#define SRAM_WAIT_INIT 0x00000000
#endif

	.macro	hal_memc_init

	# These mappings need to be set up before we
	# can use the stack and make calls to other
	# functions

	# If we have been started from Cygmon, it should have
	# already done a lot of this, but it should do no harm
	# to reinitialize the following registers.
			
	# SCS0,1 base addr of ISA & PCI
	la	v0,0xffffe010
	la	v1,0x20201410
	sw	v1,0(v0)	

	la	v0,0xffffe014
	la	v1,0xfffffcfc
	sw	v1,0(v0)	

	# ROM configuration
	.set	at
	la	v0,0xffff9000
	lw	v1,0(v0)
	and	v1,v1,0xffff0004	# keep hardware defaults
	or	v1,ROM_CCR0_INIT	# install our values
	sw	v1,0(v0)	
	.set	noat

	# SRAM config
	la	v0,0xffff9100
	la	v1,SRAM_WAIT_INIT
	sw	v1,0(v0)	

	# ISA bus setup
	la	v0,0xb2100000
	la	v1,4
	sb	v1,0(v0)	
	
	# Clear IMR (to cope with JMON)
	la	v0,0xffffc004
	la	v1,0x00000000
	sw	v1,0(v0)

	hal_memc_init_dram
	
	.endm


	.macro hal_memc_init_dram	
	# DRAM Configuration
	la	v0, 0xffff8000
	la	v1, DRAM_CONFIG_INIT
	sw	v1, 0(v0)
	
	# DBMR0
	la	v0, 0xffff8004
	la	v1, 0x00000000	
	sw	v1, 0(v0)
	
	# DWR0
	la	v0, 0xffff8008
	la	v1, DRAM_DWR0_INIT
	sw	v1, 0(v0)
	
        # DREFC - Depends on clock requency
	la	v0, 0xffff8800
	la	v1, DRAM_DREFC_INIT
	sw	v1, 0(v0)
	.endm

#define CYGPKG_HAL_MIPS_MEMC_DEFINED
		
#endif	
	
##-----------------------------------------------------------------------------
## TX39 interrupt handling.

#ifndef CYGPKG_HAL_MIPS_INTC_DEFINED

#ifdef CYGPKG_HAL_MIPS_TX3904

	# Set all ILRX registers to 0, masking all external interrupts.
	.macro	hal_intc_init

#ifndef CYG_HAL_STARTUP_RAM
	la	v0,0xFFFFC010
	move	v1,zero
	sw	v1,0(v0)
	sw	v1,4(v0)
	sw	v1,8(v0)
	sw	v1,12(v0)
#endif
	.endm

	.macro	hal_intc_decode vnum
	mfc0	v1,cause			# get cause register
	nop
	srl	v1,v1,10			# shift IP bits to ls bits
	andi	v1,v1,0x7F			# isolate IP bits
	la	v0,hal_intc_translation_table	# address of translation table
	add	v0,v0,v1			# offset of index byte
	lb	\vnum,0(v0)			# load it
	.endm

#ifndef CYGPKG_HAL_MIPS_INTC_TRANSLATE_DEFINED
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
	.macro	hal_intc_translate inum,vnum
	move	\vnum,zero			# Just vector zero is supported
	.endm
#else			
	.macro	hal_intc_translate inum,vnum
	move	\vnum,\inum			# Vector == interrupt number
	.endm
#endif
#endif

# This table translates from the 6 bit value supplied in the IP bits
# of the cause register into a 0..16 offset into the ISR tables.
	.macro	hal_intc_decode_data
hal_intc_translation_table:	
	.byte	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	.byte	0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
	.byte	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
	.byte	16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16
	.endm

#define CYGPKG_HAL_MIPS_INTC_DEFINED

#else

#error Unknown TX39 variant

#endif	

#endif

#------------------------------------------------------------------------------
# Diagnostics macros.
	
#if 0	
#ifndef CYGPKG_HAL_MIPS_DIAG_DEFINED

	# Set up PIO0 for debugging output
	.macro	hal_diag_init
	la	v0,0xfffff500
	la	v1,0xff
	sb	v1,0(v0)
	la	v1,0
	sb	v1,4(v0)	
	.endm

#define CYGPKG_HAL_MIPS_DIAG_DEFINED
		
#endif	
#endif

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
