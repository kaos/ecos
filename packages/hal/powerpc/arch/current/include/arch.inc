##=============================================================================
##
##	arch.inc
##
##	PowerPC architecture assembler header file
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
## Contributors:nickg
## Date:	1997-10-16
## Purpose:	PowerPC definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the PowerPC
##              It also includes the variant assembly header file.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/variant.inc>
#include <cyg/hal/ppc_offsets.inc>
        
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
# Some useful coding macros

	# Load immediate word, has to be done with 2 instructions
	.macro	lwi	reg,val
	lis	\reg,\val@H
	ori	\reg,\reg,\val@L
	.endm

#ifdef CYGPKG_HAL_POWERPC_PPC603
	# Move from HID0 hw control register
	.macro	mfhid0 reg
	mfspr	\reg,HID0
	.endm
#endif

#define FUNC_START(name)        \
        .type name,@function;   \
        .globl name;            \
name:   

#define FUNC_END(name)          \
	/* nothing for now */

##-----------------------------------------------------------------------------
## PowerPC FPU state handling
## Empty for now.

        .macro	hal_fpu_save regs
        .endm

        .macro	hal_fpu_load regs
        .endm

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		

	# Enable interrupts
	.macro hal_cpu_int_enable
        mfmsr   r0
        ori     r3,r3,0x8000
        rlwimi  r0,r3,0,16,16
        sync
        mtmsr   r0
        sync
	.endm		

	# Disable interrupts
	.macro hal_cpu_int_disable
        mfmsr   r0
        li      r3,0
        rlwimi  r0,r3,0,16,16
        sync
        mtmsr   r0
        sync
	.endm		

	# Merge the interrupt enable state of the status register in
	# \sr with the current sr.
	.macro	hal_cpu_int_merge sr
        mfmsr   r0
        rlwimi  r0,\sr,0,16,16
        sync
        mtmsr   r0
        sync
	.endm

#------------------------------------------------------------------------------
# end of arch.inc
