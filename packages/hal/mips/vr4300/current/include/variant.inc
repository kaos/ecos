#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	VR4300 assembler header file
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
## Contributors:	nickg
## Date:	1999-04-06
## Purpose:	VR4300 definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the VR4300 CPU.
## Usage:
##		#include <cyg/hal/variant.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

        .set    mips3

#include <cyg/hal/mips.inc>

#include <cyg/hal/platform.inc>

#include <pkgconf/hal.h>

#------------------------------------------------------------------------------
# Initial SR value
# Set the KX bit to enable 64bit accesses in kernel mode.

#define INITIAL_SR_VAR	0x00000080

#------------------------------------------------------------------------------
# Set up initial value for config register. Sets endian mode and
# disables the cache on kseg0.

#if defined(CYGPKG_HAL_MIPS_MSBFIRST)
# define	INITIAL_CONFIG0	0x00008002
#elif defined(CYGPKG_HAL_MIPS_LSBFIRST)
# define	INITIAL_CONFIG0	0x00000002
#else
# error MIPS endianness not set by configuration
#endif

#------------------------------------------------------------------------------
# Set up initial value for FPU FCR31 register. We set the FS bit to flush
# denormalized results to zero.

#ifndef CYG_HAL_MIPS_FCSR_INIT
#define CYG_HAL_MIPS_FCSR_INIT 0x01000000
#endif

#------------------------------------------------------------------------------
# Cache macros.
	
#ifndef CYGPKG_HAL_MIPS_CACHE_DEFINED

	.macro	hal_cache_init

#ifndef CYG_HAL_STARTUP_RAM

	mfc0	v0,config0		# disable Kseg0 caching in config0 register
	nop
	nop
	la	v1,0xfffffff8
	and	v0,v0,v1
	ori	v0,v0,2
	mtc0	v0,config0
	nop
	nop
	nop

        .set mips3                      # Set ISA to MIPS 3 to allow cache insns

        # Now ensure the caches are invalidated. The caches are NOT cleared or
	# invalidated on non-power-up resets and may come up in a random state
	# on power-up. Hence they may contain stale or randomly bogus data.
	# Here we use the index-store-tag cache operation to clear all the cache
	# tags and states to zero. This will render them all invalid on the
	# VR4300.

        # D-cache:
        la      t0,0x80000000
        addi    t1,t0,0x2000
1:
	mtc0	zero,$28
	mtc0	zero,$29
        cache   0x09,0(t0)
        addi    t0,t0,0x10
        sub     v0,t1,t0
        bgez    v0,1b
        nop                             # delay slot

	# I-cache:
        la      a0,0x80000000
        addi    a1,a0,0x4000
1:
	mtc0	zero,$28
	mtc0	zero,$29
        cache   0x08,0(a0)
        addi    a0,a0,0x20
        sub     v0,a1,a0
        bgez    v0,1b
        nop                             # delay slot

        .set mips0                      # reset ISA to default
#endif

	.endm

#define CYGPKG_HAL_MIPS_CACHE_DEFINED

#endif	

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
