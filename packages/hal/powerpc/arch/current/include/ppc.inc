##=============================================================================
##
##	ppc.inc
##
##	PowerPC assembler header file
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
# by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1997-10-16
## Purpose:	PowerPC definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the PowerPC
## Usage:
##		#include <cyg/hal/ppc.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#------------------------------------------------------------------------------
# Easier to read names for the registers

	.equ	r0, 0
	.equ	r1, 1
	.equ	r2, 2
	.equ	r3, 3
	.equ	r4, 4
	.equ	r5, 5
	.equ	r6, 6
	.equ	r7, 7
	.equ	r8, 8
	.equ	r9, 9
	.equ	r10, 10
	.equ	r11, 11
	.equ	r12, 12
	.equ	r13, 13
	.equ	r14, 14
	.equ	r15, 15
	.equ	r16, 16
	.equ	r17, 17
	.equ	r18, 18
	.equ	r19, 19
	.equ	r20, 20
	.equ	r21, 21
	.equ	r22, 22
	.equ	r23, 23
	.equ	r24, 24
	.equ	r25, 25
	.equ	r26, 26
	.equ	r27, 27
	.equ	r28, 28
	.equ	r29, 29
	.equ	r30, 30
	.equ	r31, 31

	.equ	sp, r1

#------------------------------------------------------------------------------
# The names of some special purpose registers

	.equ	dsisr, 18
	.equ	dar  , 19
	.equ	dec  , 22
	.equ	sprg0, 272
	.equ	sprg1, 273
	.equ	sprg2, 274
	.equ	sprg3, 275
	.equ	pvr  , 287
	.equ	hid0 , 1008

#ifdef CYG_HAL_POWERPC_MP860
	.equ	ic_cst,560
	.equ	dc_cst,568
	.equ	der  , 149
#endif

#------------------------------------------------------------------------------
# Some useful coding macros

	# Load immediate word, has to be done with 2 instructions
	.macro	lwi	reg,val
	lis	\reg,\val@H
	ori	\reg,\reg,\val@L
	.endm

	# Move from HID0 hw control register
	.macro	mfhid0 reg
	mfspr	\reg,hid0
	.endm

#------------------------------------------------------------------------------
# Exception, interrupt and thread context save area layout

	# These first 36 words are common to all
	# saved states
	.equ	ppcreg_regs,		0	# 32 integer registers
	.equ	ppcreg_cr,		32*4
	.equ	ppcreg_xer,		33*4
	.equ	ppcreg_lr,		34*4
	.equ	ppcreg_ctr,		35*4
	.equ	ppcreg_context_size, 	36*4

	# The following are only saved on exceptions and interrupts
	.equ	ppcreg_vector,		36*4	# exception vector
	.equ	ppcreg_msr,		37*4	# MSR at time of exception
	.equ	ppcreg_pc,		38*4	# PC at time of exception

	# The following are only saved on exceptions, and are for information
	# only. They are not restored with the rest of the state.
	.equ	ppcreg_hid0,		39*4	# HID0 HW control register
	.equ	ppcreg_dar,		40*4	# data address register
	.equ	ppcreg_dsisr,		41*4	# DSI status register
	.equ	ppcreg_pvr,		42*4	# processor version
	.equ	ppcreg_exception_size, 	43*4

	# FIXME possibly need BAT and segment registers here too
	# FIXME also decide what to do with FP state.

	.equ	ppc_stack_frame_size,	56	# size of a stack frame

	.equ	ppc_exception_safety,	0x100	# safety net for exceptions
	.equ	ppc_exception_decrement, ppcreg_exception_size+ppc_exception_safety


#------------------------------------------------------------------------------
# end of ppc.inc
