#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	SH/EDK7708 board assembler header file
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
## Contributors:jskov
## Date:	2000-02-02
## Purpose:	SH/EDK7708 board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the SH/EDK7708 board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/sh_regs.h>
#include <cyg/hal/sh_offsets.inc>

#------------------------------------------------------------------------------
# Hardware initialization.

        .macro  hal_hardware_init
        // Set up the Bus State Controller
        mova     BSC_settings_table,r0
        mov      r0,r3
1:      mov.w    @r3+,r0                // Address (or zero)
        cmp/eq   #0,r0
        bt       2f
        mov.w    @r3+,r1                // data
        bra      1b
         mov.w    r1,@r0                // delay slot

        .align  2
BSC_settings_table:
        # These are the settings set by the Hitachi ROM Monitor.

        # BCR2: Bus size of areas 1-6 to 32 bits
        .word   CYGARC_REG_BCR2
        .word   0x3ffc
        # BCR1: Areas 2 and 3 are SDRAM
        .word   CYGARC_REG_BCR1
        .word   0x080c
        # BCR2: Bus size of areas 1-6 to 32 bits [note: second write!]
        .word   CYGARC_REG_BCR2
        .word   0x3ffc
        # WCR1: 3 wait-state cycles inserted for all areas
        .word   CYGARC_REG_WCR1
        .word   0x3fff
        # WCR2: extra wait states and full pitch for burst
        .word   CYGARC_REG_WCR2
        .word   0xffd7
        # MCR: RAS/CAS & burst timing area 2/3
        .word   CYGARC_REG_MCR
        .word   0x963c
        # RTCNT: refresh counter (needs a5 in top byte to accept write)
        .word   CYGARC_REG_RTCNT
        .word   (0xa500 | 0x0000)
        # RTCOR: refresh time constant (needs a5 in top byte to accept write)
        .word   CYGARC_REG_RTCOR
        .word   (0xa500 | 0x003b)
        # RFCR:  refresh count register (needs a4 in top byte to accept write)
        .word   CYGARC_REG_RFCR
        .word   (0xa400 | 0x0000)
        # RTCSR: refresh timer control (needs a5 in top byte to accept write)
        .word   CYGARC_REG_RTCSR
        .word   (0xa500 | 0x0008)

        # Set SDMR to 0x220
        .word    0xd880
        .word    0       
        
        # Table end
        .word   0

        .align  2
2:      
        .endm

#------------------------------------------------------------------------------
# Monitor initialization.

#ifndef CYGPKG_HAL_SH_MON_DEFINED

#if	!defined(CYG_HAL_STARTUP_RAM) ||		\
	(	defined(CYG_HAL_STARTUP_RAM) &&		\
		!defined(CYGSEM_HAL_USE_ROM_MONITOR))

	# If we are not starting up from RAM, or we are starting in
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
