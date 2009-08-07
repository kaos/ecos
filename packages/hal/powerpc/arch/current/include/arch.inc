#ifndef _CYGONCE_HAL_POWERPC_ARCH_INC_
#define _CYGONCE_HAL_POWERPC_ARCH_INC_
##=============================================================================
##
##	arch.inc
##
##	PowerPC architecture assembler header file
##
##=============================================================================
## ####ECOSGPLCOPYRIGHTBEGIN####                                            
## -------------------------------------------                              
## This file is part of eCos, the Embedded Configurable Operating System.   
## Copyright (C) 1998, 1999, 2000, 2001, 2002, 2005, 2007 Free Software Foundation, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under    
## the terms of the GNU General Public License as published by the Free     
## Software Foundation; either version 2 or (at your option) any later      
## version.                                                                 
##
## eCos is distributed in the hope that it will be useful, but WITHOUT      
## ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
## for more details.                                                        
##
## You should have received a copy of the GNU General Public License        
## along with eCos; if not, write to the Free Software Foundation, Inc.,    
## 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
##
## As a special exception, if other files instantiate templates or use      
## macros or inline functions from this file, or you compile this file      
## and link it with other works to produce a work based on this file,       
## this file does not by itself cause the resulting work to be covered by   
## the GNU General Public License. However the source code for this file    
## must still be made available in accordance with section (3) of the GNU   
## General Public License v2.                                               
##
## This exception does not invalidate any other reasons why a work based    
## on this file might be covered by the GNU General Public License.         
## -------------------------------------------                              
## ####ECOSGPLCOPYRIGHTEND####                                              
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
#include <cyg/hal/ppc_regs.h>
        
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

#ifdef CYGHWR_HAL_POWERPC_FPU
        .equ    f0, 0
        .equ    f1, 1
        .equ    f2, 2
        .equ    f3, 3
        .equ    f4, 4
        .equ    f5, 5
        .equ    f6, 6
        .equ    f7, 7
        .equ    f8, 8
        .equ    f9, 9
        .equ    f10, 10
        .equ    f11, 11
        .equ    f12, 12
        .equ    f13, 13
        .equ    f14, 14
        .equ    f15, 15
        .equ    f16, 16
        .equ    f17, 17
        .equ    f18, 18
        .equ    f19, 19
        .equ    f20, 20
        .equ    f21, 21
        .equ    f22, 22
        .equ    f23, 23
        .equ    f24, 24
        .equ    f25, 25
        .equ    f26, 26
        .equ    f27, 27
        .equ    f28, 28
        .equ    f29, 29
        .equ    f30, 30
        .equ    f31, 31
#endif


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
## The PowerPC ABI defines f14-f31 as callee saved. There is thus no need to 
## save them when calling C functions

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
# define MAX_SAVE_FREG    13
#else
# define MAX_SAVE_FREG    31
#endif                  

        .macro	hal_fpu_save regs
#ifdef CYGHWR_HAL_POWERPC_FPU
        .set _freg, 0
        .rept MAX_SAVE_FREG+1
        stfd _freg,(CYGARC_PPCREG_FREGS+_freg*8)(\regs)
        .set _freg, _freg+1
        .endr
#endif
        .endm

        .macro	hal_fpu_load regs
#ifdef CYGHWR_HAL_POWERPC_FPU
        .set _freg, 0
        .rept MAX_SAVE_FREG+1
        lfd _freg,(CYGARC_PPCREG_FREGS+_freg*8)(\regs)
        .set _freg, _freg+1
        .endr
#endif
        .endm

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		

#if !defined(CYGHWR_HAL_POWERPC_BOOK_E)
#define CYGARC_REG_MSR_INTBITS     CYGARC_REG_MSR_EE
#else
#define CYGARC_REG_MSR_INTBITS     (CYGARC_REG_MSR_EE|CYGARC_REG_MSR_CE)
#endif

	# Enable interrupts
	.macro hal_cpu_int_enable
        mfmsr   r0
        lwi      r3,CYGARC_REG_MSR_INTBITS
        or       r0,r0,r3
        sync
        mtmsr   r0
        sync
	.endm		

	# Disable interrupts
	.macro hal_cpu_int_disable
        mfmsr   r0
        lwi      r3,~CYGARC_REG_MSR_INTBITS
        and      r0,r0,r3
        sync
        mtmsr   r0
        sync
	.endm		

	# Merge the interrupt enable state of the status register in
	# \sr with the current sr.
	.macro	hal_cpu_int_merge sr,wr
        mfmsr   r0
        lwi      \wr,CYGARC_REG_MSR_INTBITS
        and      \wr,\wr,\sr
        or       r0,r0,\wr
        sync
        mtmsr   r0
        sync
	.endm

#------------------------------------------------------------------------------
# Book E variations
#
# These macros implement things that vary between the AIM architecture and
# Book E.

#if !defined(CYGHWR_HAL_POWERPC_BOOK_E)

#ifdef CYGSEM_HAL_POWERPC_COPY_VECTORS

        .macro  hal_vectors_init
        lwi     r3,rom_vectors-4
        lwi     r4,((CYGHWR_HAL_POWERPC_VECTOR_BASE)-4)
        lwi     r5,rom_vectors_end-4
        sub     r5,r5,r3                # compute number of words to copy
        srwi    r5,r5,2
        mtctr   r5
0:      lwzu    r0,4(r3)
        stwu    r0,4(r4)
        bdnz    0b
        .endm

#define HAL_VECTORS_INIT_DEFINED

#endif

#else // !defined(CYGHWR_HAL_POWERPC_BOOK_E)

#if defined(CYGSEM_HAL_POWERPC_COPY_VECTORS)


#if defined(CYGHWR_HAL_POWERPC_BOOK_E_FIXED_VECTORS)
        .macro  hal_ivor_init   ivor,handler
        .endm
#else	
        .macro  hal_ivor_init   ivor,handler
        lwi     r3,__exception_\handler
        and     r3,r3,r4
        mtspr   \ivor,r3
        .endm
#endif	

        .macro  hal_vectors_init

#if defined(CYGHWR_HAL_POWERPC_BOOK_E_FIXED_VECTORS)
        lwi     r3,hal_fixed_vectors_base
        lwi     r4,0xFFFFF000
#else
        lwi     r3,rom_vectors
        lwi     r4,0xFFFF0000
#endif
        and     r3,r3,r4
        mtspr   63,r3
#if defined(CYGHWR_HAL_POWERPC_NEED_VECTORS)
        lwi     r4,0x0000FFF0
        hal_ivor_init   400,critical_input
        hal_ivor_init   401,machine_check
        hal_ivor_init   402,data_storage
        hal_ivor_init   403,instruction_storage
        hal_ivor_init   404,external
        hal_ivor_init   405,alignment
        hal_ivor_init   406,program
        hal_ivor_init   407,floatingpoint_unavailable
        hal_ivor_init   408,system_call
        hal_ivor_init   409,ap_unavailable
        hal_ivor_init   410,decrementer
        hal_ivor_init   411,interval_timer
        hal_ivor_init   412,watchdog
        hal_ivor_init   413,data_tlb_miss
        hal_ivor_init   414,instruction_tlb_miss
        hal_ivor_init   415,debug
#endif        
        .endm

#define HAL_VECTORS_INIT_DEFINED

#endif
		
#if defined(CYGHWR_HAL_POWERPC_BOOK_E_FIXED_VECTORS)

        .macro  exception_vector_fixed name
        .p2align 4
        .globl   exception_vector_fixed\name
exception_vector_fixed\name:
        b        __exception_\name
        .endm

        .macro  hal_fixed_vectors
hal_fixed_vectors_base:
        exception_vector_fixed          critical_input
        exception_vector_fixed          machine_check
        exception_vector_fixed          data_storage
        exception_vector_fixed          instruction_storage
        exception_vector_fixed          external
        exception_vector_fixed          alignment
        exception_vector_fixed          program
        exception_vector_fixed          floatingpoint_unavailable
        exception_vector_fixed          system_call
        exception_vector_fixed          ap_unavailable 
        exception_vector_fixed          decrementer
        exception_vector_fixed          interval_timer
        exception_vector_fixed          watchdog
        exception_vector_fixed          data_tlb_miss
        exception_vector_fixed          instruction_tlb_miss
        exception_vector_fixed          debug  
        .endm

#define HAL_FIXED_VECTORS_DEFINED

#endif  


#endif // !defined(CYGHWR_HAL_POWERPC_BOOK_E)


#if !defined(HAL_VECTORS_INIT_DEFINED)

        .macro  hal_vectors_init
        .endm

#endif        

#ifndef HAL_FIXED_VECTORS_DEFINED

        .macro   hal_fixed_vectors
        .endm
        
#endif        

#------------------------------------------------------------------------------
# end of arch.inc

#endif // _CYGONCE_HAL_POWERPC_ARCH_INC_
