#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	STB board assembler header file
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
## Purpose:	STB board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the STB board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>


#------------------------------------------------------------------------------
# Diagnostics macros.

#define CYG_HAL_DIAG_LED_ADDRESS1 0x83f90000
#define CYG_HAL_DIAG_LED_ADDRESS2 0x81f90000
#define CYG_HAL_DIAG_LED_ADDRESS3 hal_diag_led_state

	.macro	hal_diag_data
	.globl	hal_diag_led_state
hal_diag_led_state:	.long	0xffffffff
hal_diag_intr_count:	.long	0
	.endm

	.macro	hal_diag_init
	mov	0xFFFFFFFF,d0
	mov	CYG_HAL_DIAG_LED_ADDRESS1,a0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS2,a0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS3,a0
	mov	d0,(a0)

	mov	0xd4002000,a0	# SR control regs
	mov	0xd4003000,a1	# timer control regs
	mov	99,d0
	movbu	d0,(16,a1)
	mov	0x80,d0
	movbu	d0,(a1)
	clr	d0
	movbu	d0,(4,a0)
	mov	0xc084,d0
	movhu	d0,(a0)

	jmp	1f

hal_diag_digits:
	.byte	0x81	# 0
	.byte	0xf3	# 1
	.byte	0x49	# 2
	.byte	0x61	# 3
	.byte	0x33	# 4
	.byte	0x25	# 5
	.byte	0x05	# 6
	.byte	0xf1	# 7
	.byte	0x01	# 8
	.byte	0x21	# 9
	.byte	0x11	# A
	.byte	0x07	# B
	.byte	0x8d	# C
	.byte	0x43	# D
	.byte	0x0d	# E
	.byte	0x1d	# F

hal_diag_hex_digits:
	.ascii	"0123456789ABCDEF"
1:

#	hal_diag_led 0
#	hal_diag_led 1
#	hal_diag_led 2
#	hal_diag_led 3
#	hal_diag_led 4
#	hal_diag_led 5
#	hal_diag_led 6
#	hal_diag_led 7
#	hal_diag_led 8
#	hal_diag_led 9
#	hal_diag_led 0xa
#	hal_diag_led 0xb
#	hal_diag_led 0xc
#	hal_diag_led 0xd
#	hal_diag_led 0xe
#	hal_diag_led 0xf

	.endm

	.macro	hal_diag_excpt_start
	mov	CYG_HAL_DIAG_LED_ADDRESS3,a0
	mov	(a0),d0
	xor	0x01000000,d0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS1,a0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS2,a0
	mov	d0,(a0)
	.endm

	.macro	hal_diag_intr_start
	mov	(hal_diag_intr_count),d1
	inc	d1
	cmp	100,d1
	bne	x\@
	clr	d1
	mov	CYG_HAL_DIAG_LED_ADDRESS3,a0
	mov	(a0),d0
	xor	0x00010000,d0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS1,a0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS2,a0
	mov	d0,(a0)
x\@:
	mov	d1,(hal_diag_intr_count)
	.endm

#if 0
	.macro	hal_diag_restore
	mov	(hal_diag_intr_count),d0
	and	0x1000,d0
	beq	x\@
	mov	CYG_HAL_DIAG_LED_ADDRESS3,a0
	mov	(a0),d0
	and	0xfffeffff,d0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS1,a0
	mov	d0,(a0)
	mov	CYG_HAL_DIAG_LED_ADDRESS2,a0
	mov	d0,(a0)
x\@:
	.endm
#else
	.macro	hal_diag_restore
	.endm
#endif

#if 0
	.macro	hal_diag_led val
	movm	[d2,d3,a2],(sp)
	mov	hal_diag_digits,a2
	mov	\val,d2
	and	0xf,d2
	add	d2,a2
	movbu	(a2),d3
	mov	CYG_HAL_DIAG_LED_ADDRESS3,a2
	mov	(a2),d2
	asl	8,d2
	or	d3,d2
	mov	d2,(a2)
	mov	CYG_HAL_DIAG_LED_ADDRESS1,a2
	mov	d2,(a2)
	mov	CYG_HAL_DIAG_LED_ADDRESS2,a2
	mov	d2,(a2)

#	mov	\val,d3
#	and	0xf,d3
#	mov	hal_diag_hex_digits,a2
#	add	d3,a2
#	movbu	(a2),d3
#	mov	0xd4002000,a2
#x2:	movhu	(0,a2),d2
#	and	0x20,d2
#	bne	2b
#	movbu	d3,(8,a2)

	mov	200000,d2
1:
	add	-1,d2
	cmp	0,d2
	bne	1b

	movm	(sp),[d2,d3,a2]
	.endm
#else
	.macro	hal_diag_led val
	.endm
#endif

#define CYGPKG_HAL_MN10300_DIAG_DEFINED


#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
