#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	SH/CQ7750 board assembler header file
##
##=============================================================================
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
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	jskov
## Contributors:Ryozaburo Suzuki
## Date:	2000-04-18
## Purpose:	SH/CQ7750 board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the SH/CQ7750 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/sh_offsets.inc>
#include <cyg/hal/sh_regs.h>

#------------------------------------------------------------------------------
# Hardware initialization.

        .macro  hal_hardware_init
        // Set up the Bus State Controller
        mova     BSC_settings_table,r0
        mov      r0,r3
1:      mov.l    @r3+,r0                // Address (or zero)
        mov      r0,r2
        cmp/eq   #0,r0
        bt       4f
        mov.l    @r3+,r1                // data
        mov.l    @r3+,r0                // byte or word or long?

        cmp/eq   #0,r0   // byte
        bt       2f

        cmp/eq   #1,r0   // word
        bt       3f

        bra      1b      // long
        mov.l    r1,@r2  // delay slot

2:      bra      1b
        mov.b    r1,@r2  // delay slot

3:      bra      1b
        mov.w    r1,@r2  // delay slot

        .align  2

BSC_settings_table:
        # FRQCR:
	.long   CYGARC_REG_FRQCR
	.long   CYGARC_REG_FRQCR_INIT
	.long	2
	
        # BCR1:
        .long   CYGARC_REG_BCR1
        .long   0x0020000c
        .long   2
        # BCR2:
        .long   CYGARC_REG_BCR2
        .long   0x35F8
        .long   1
        # WCR1:
        .long   CYGARC_REG_WCR1
        .long   0x77117121
        .long   2
        # WCR2:
        .long   CYGARC_REG_WCR2
        .long   0xFC8AE520
        .long   2
        # WCR3:
        .long   CYGARC_REG_WCR3
        .long   0x07117101
        .long   2
        # PCR:
        .long   CYGARC_REG_PCR
        .long   0x0000
        .long   1
        # RTCNT:
        .long   CYGARC_REG_RTCNT
        .long   0xa500
        .long   1
        # RTCOR:
        .long   CYGARC_REG_RTCOR
        .long   0xa53b
        .long   1
        # RFCR:
        .long   CYGARC_REG_RFCR
        .long   0xa400
        .long   1
        # RTCSR:
        .long   CYGARC_REG_RTCSR
        .long   0xa508
        .long   1
        # MCR:
        .long   CYGARC_REG_MCR
        .long   0x10192194
        .long   2
        # SDMR2:
        .long   CYGARC_REG_SDMR2
        .long   0xaa
        .long   0
        # MCR:
        .long   CYGARC_REG_MCR
        .long   0x50192194
        .long   2
        # SDMR2:
        .long   CYGARC_REG_SDMR2
        .long   0xaa
        .long   0
        # Table end
        .long   0
        
        .align  2
4:
        .endm

#------------------------------------------------------------------------------
# Monitor initialization.

#ifndef CYGPKG_HAL_SH_MON_DEFINED

#if	defined(CYG_HAL_STARTUP_ROM) ||			\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYGSEM_HAL_USE_ROM_MONITOR))

	# If we are starting up from ROM, or we are starting in
	# RAM and NOT using a ROM monitor, initialize the VSR table.

	.macro	hal_mon_init
        mov.l   $_hal_vsr_table_p,r3
	# Write exception vectors
	mov.l   $_cyg_hal_default_exception_vsr_p,r4
	mov     #CYGNUM_HAL_VSR_EXCEPTION_COUNT,r5
1:	mov.l   r4,@r3
        add     #4,r3
	dt	r5
	bf	1b
	# Write interrupt vector
	mov.l   $_cyg_hal_default_interrupt_vsr_p,r4
	mov.l   r4,@r3
	bra	2f
	 nop
        .align  2

$_cyg_hal_default_exception_vsr_p:
	.long	_cyg_hal_default_exception_vsr
$_cyg_hal_default_interrupt_vsr_p:
	.long	_cyg_hal_default_interrupt_vsr

	.extern _hal_vsr_table
$_hal_vsr_table_p:
	.long _hal_vsr_table

2:
	.endm

#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

	# Initialize the VSR table entries
	# We only take control of the interrupt vector,
	# the rest are left to the ROM for now...

	.macro	hal_mon_init
	# Write interrupt vector
        mov.l   $_hal_vsr_table_p,r3
	mov.l   $_cyg_hal_default_interrupt_vsr_p,r4
	mov.l   r4,@r3
	bra	2f
	 nop
        .align  2

$_cyg_hal_default_interrupt_vsr_p:
	.long	_cyg_hal_default_interrupt_vsr

	.extern _hal_vsr_table
$_hal_vsr_table_p:
	.long _hal_vsr_table+CYGNUM_HAL_VECTOR_INTERRUPT*4

2:
	.endm

#else

	.macro	hal_mon_init
	.endm

#endif


#define CYGPKG_HAL_SH_MON_DEFINED

#endif // CYGPKG_HAL_SH_MON_DEFINED

#endif // CYGONCE_HAL_PLATFORM_INC
