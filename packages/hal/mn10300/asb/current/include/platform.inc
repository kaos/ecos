#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##	platform.inc
##
##	ASB2303 board assembler header file
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
# -------------------------------------------
# This file is part of eCos, the Embedded Configurable Operating System.
# Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
#
# eCos is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 or (at your option) any later version.
#
# eCos is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with eCos; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
#
# As a special exception, if other files instantiate templates or use macros
# or inline functions from this file, or you compile this file and link it
# with other works to produce a work based on this file, this file does not
# by itself cause the resulting work to be covered by the GNU General Public
# License. However the source code for this file must still be made available
# in accordance with section (3) of the GNU General Public License.
#
# This exception does not invalidate any other reasons why a work based on
# this file might be covered by the GNU General Public License.
#
# Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
# at http://sources.redhat.com/ecos/ecos-license
# -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	dmoseley
## Contributors:dmoseley
## Date:	2000-08-11
## Purpose:	ASB board definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the ASB board.
## Usage:
##		#include <cyg/hal/platform.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/plf_io.h>

#------------------------------------------------------------------------------
# Diagnostics macros.

#include <cyg/hal/hal_io.h>

	.macro	hal_diag_data
	.globl	hal_diag_led_state
hal_diag_led_state:	.long	0xffffffff
hal_diag_intr_count:	.long	0
	.endm

#define HAL_EARLY_INIT hal_led_init
.macro hal_led_init
	# Setup Port 0 as all output
	# Do this the very first thing so we have LED debugging available early
	mov	HAL_GPIO_MODE_ALL_OUTPUT,d0
	mov	HAL_GPIO_0_MODE,a0
	mov	d0,(a0)
.endm

	.macro	hal_diag_init
	hal_led_init
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
	hal_diag_led 0
	.endm

	.macro	hal_diag_excpt_start
	mov	HAL_LED_ADDRESS,a0
	mov	(a0),d0
	xor	0x01000000,d0
	mov	d0,(a0)
	.endm

	.macro	hal_diag_intr_start
	mov	(hal_diag_intr_count),d1
	inc	d1
	cmp	100,d1
	bne	x\@
	clr	d1
	mov	HAL_LED_ADDRESS,a0
	mov	(a0),d0
	xor	0x00010000,d0
	mov	d0,(a0)
x\@:
	mov	d1,(hal_diag_intr_count)
	.endm

	.macro	hal_diag_restore
	mov	(hal_diag_intr_count),d0
	and	0x1000,d0
	beq	x\@
	mov	HAL_LED_ADDRESS,a0
	mov	(a0),d0
	and	0xfffeffff,d0
	mov	d0,(a0)
x\@:
	.endm

	.macro	hal_diag_led val
	movm	[d2,d3,a2],(sp)
0:	mov	hal_diag_digits,a2
	mov	\val,d2
	and	0xf,d2
	add	d2,a2
	movbu	(a2),d3
	mov	HAL_LED_ADDRESS,a2
	mov	(a2),d2
	asl	8,d2
	or	d3,d2
	mov	d2,(a2)
	movm	(sp),[d2,d3,a2]
	.endm

#define CYGPKG_HAL_MN10300_DIAG_DEFINED

#------------------------------------------------------------------------------
# MEMC macros.
	
#ifndef CYGPKG_HAL_MN10300_MEMC_DEFINED

// These settings follow the recommended settings in the
// "MN103E010 Evaluation Board User's Guide"

#define BCCR                    0xC0002000
#define BCCR_INIT               0x12040580

#define SBBASE0                 0xD8C00100
#define SBBASE0_INIT            0x8000FC01
#define SBBASE1                 0xD8C00110
#define SBBASE1_INIT            0x8400FC01
#define SBBASE2                 0xD8C00120
#define SBBASE2_INIT            0x8600FFF1
#define SBBASE3                 0xD8C00130
#define SBBASE3_INIT            0x00000000
#define SBBASE4                 0xD8C00140
#define SBBASE4_INIT            0x00000000
#define SBBASE5                 0xD8C00150
#define SBBASE5_INIT            0x00000000
#define SBBASE6                 0xD8C00160
#define SBBASE6_INIT            0x00000000
#define SBBASE7                 0xD8C00170
#define SBBASE7_INIT            0x00000000

#define SBCTRL00                0xD8C00200
#define SBCTRL00_INIT           0x21111000
#define SBCTRL10                0xD8C00210
#define SBCTRL10_INIT           0x21111000
#define SBCTRL20                0xD8C00220
#define SBCTRL20_INIT           0x21111000
#define SBCTRL30                0xD8C00230
#define SBCTRL30_INIT           0x00000000
#define SBCTRL40                0xD8C00240
#define SBCTRL40_INIT           0x00000000
#define SBCTRL50                0xD8C00250
#define SBCTRL50_INIT           0x00000000
#define SBCTRL60                0xD8C00260
#define SBCTRL60_INIT           0x00000000
#define SBCTRL70                0xD8C00270
#define SBCTRL70_INIT           0x00000000

#define SBCTRL01                0xD8C00204
#define SBCTRL01_INIT           0x00100200
#define SBCTRL11                0xD8C00214
#define SBCTRL11_INIT           0x00100200
#define SBCTRL21                0xD8C00224
#define SBCTRL21_INIT           0x00100200
#define SBCTRL31                0xD8C00234
#define SBCTRL31_INIT           0x00000000
#define SBCTRL41                0xD8C00244
#define SBCTRL41_INIT           0x00000000
#define SBCTRL51                0xD8C00254
#define SBCTRL51_INIT           0x00000000
#define SBCTRL61                0xD8C00264
#define SBCTRL61_INIT           0x00000000
#define SBCTRL71                0xD8C00274
#define SBCTRL71_INIT           0x00000000

#define SBCTRL02                0xD8C00208
#define SBCTRL02_INIT           0x00000004
#define SBCTRL12                0xD8C00218
#define SBCTRL12_INIT           0x00000004
#define SBCTRL22                0xD8C00228
#define SBCTRL22_INIT           0x00000004
#define SBCTRL32                0xD8C00238
#define SBCTRL32_INIT           0x00000000
#define SBCTRL42                0xD8C00248
#define SBCTRL42_INIT           0x00000000
#define SBCTRL52                0xD8C00258
#define SBCTRL52_INIT           0x00000000
#define SBCTRL62                0xD8C00268
#define SBCTRL62_INIT           0x00000000
#define SBCTRL72                0xD8C00278
#define SBCTRL72_INIT           0x00000000

#define SDBASE0                 0xDA000008
#define SDBASE0_INIT            0x9000FF81
#define SDBASE1                 0xDA00000C
#define SDBASE1_INIT            0x9080FF81
#define SDRAMBUS                0xDA000000
#define SDRAMBUS_INIT           0xA8990654

	.macro	hal_memc_init
	mov	BCCR,a0
	mov	BCCR_INIT,d0
	mov	d0,(a0)

	mov	SBBASE0,a0
	mov	SBBASE0_INIT,d0
	mov	d0,(a0)

	mov	SBBASE1,a0
	mov	SBBASE1_INIT,d0
	mov	d0,(a0)

	mov	SBBASE2,a0
	mov	SBBASE2_INIT,d0
	mov	d0,(a0)

	mov	SBBASE3,a0
	mov	SBBASE3_INIT,d0
	mov	d0,(a0)

	mov	SBBASE4,a0
	mov	SBBASE4_INIT,d0
	mov	d0,(a0)

	mov	SBBASE5,a0
	mov	SBBASE5_INIT,d0
	mov	d0,(a0)

	mov	SBBASE6,a0
	mov	SBBASE6_INIT,d0
	mov	d0,(a0)

	mov	SBBASE7,a0
	mov	SBBASE7_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL00,a0
	mov	SBCTRL00_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL10,a0
	mov	SBCTRL10_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL20,a0
	mov	SBCTRL20_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL30,a0
	mov	SBCTRL30_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL40,a0
	mov	SBCTRL40_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL50,a0
	mov	SBCTRL50_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL60,a0
	mov	SBCTRL60_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL70,a0
	mov	SBCTRL70_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL01,a0
	mov	SBCTRL01_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL11,a0
	mov	SBCTRL11_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL21,a0
	mov	SBCTRL21_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL31,a0
	mov	SBCTRL31_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL41,a0
	mov	SBCTRL41_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL51,a0
	mov	SBCTRL51_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL61,a0
	mov	SBCTRL61_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL71,a0
	mov	SBCTRL71_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL02,a0
	mov	SBCTRL02_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL12,a0
	mov	SBCTRL12_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL22,a0
	mov	SBCTRL22_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL32,a0
	mov	SBCTRL32_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL42,a0
	mov	SBCTRL42_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL52,a0
	mov	SBCTRL52_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL62,a0
	mov	SBCTRL62_INIT,d0
	mov	d0,(a0)

	mov	SBCTRL72,a0
	mov	SBCTRL72_INIT,d0
	mov	d0,(a0)

	mov	SDBASE0,a0
	mov	SDBASE0_INIT,d0
	mov	d0,(a0)

	mov	SDBASE1,a0
	mov	SDBASE1_INIT,d0
	mov	d0,(a0)

	mov	SDRAMBUS,a0
	mov	SDRAMBUS_INIT,d0
	mov	d0,(a0)

	# Jump forward so we start running from the 80000000 base address
        mov     0f,a0
	jmp	(a0)
0:
	.endm

#define CYGPKG_HAL_MN10300_MEMC_DEFINED

#endif	


//-----------------------------------------------------------------------------
// Syscall support.
#ifdef CYGPKG_CYGMON
// Cygmon provides syscall handling for this board
// These must be kept in sync with the rest of the tree.
#define SIGSYS     12
#define SIGSYSCALL SIGSYS
#endif

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLATFORM_INC
# end of platform.inc
