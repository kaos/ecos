##=============================================================================
##
##      context.S
##
##      FR30 context switch and longjmp setjmp code
##
##=============================================================================
## ####ECOSGPLCOPYRIGHTBEGIN####                                            
## -------------------------------------------                              
## This file is part of eCos, the Embedded Configurable Operating System.   
## Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
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
## Author(s):   larsi
## Contributors:larsi
## Date:        2006-06-03
## Purpose:     fr30 context switch code
## Description: This file contains implementations of the thread context 
##              switch routines. It also contains the longjmp() and setjmp()
##              routines.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/fr30.inc>

#include <cyg/hal/arch.inc>
	
#------------------------------------------------------------------------------
# function declaration macro

#define FUNC_START(name)                        \
        .globl name;                            \
name:

#------------------------------------------------------------------------------
# hal_thread_switch_context
# Switch thread contexts
# 
# a timer interrupt should have caused this function to be called
# so interrupts are forbidden and we are in SSP mode (S flag=1)
#
# R4 and R5 contain our arguments.
# R5 is _fspptr_ (old USP), R4 is _tspptr_ (new USP).
# @(SSP) has the return address of the call to this function.
# In @(SSP,4) the PS and in @(SSP,8) the PC are saved by hardware.
# Before we RETI, we have to switch S flag in CCR to 0
# to use SSP for returning. Interrupts have to be
# reenabled before returning, but this is done by restoring PS.


    .globl  hal_thread_switch_context
hal_thread_switch_context:

# at first switch to USP (set bit 5 in CCR in PS)
    st      r0,     @-r15       ; push last_trap, cannot guarantee that it is
                                ; the right value, but that should not matter
                                ; as it is only for GDB
    st      mdl,    @-r15
    st      mdh,    @-r15

    st      r0,     @-r15       ; store usp

    st      r0,     @-r15       ; store ssp
    st      rp,     @-r15
    st      tbr,    @-r15
    st      ps,     @-r15
    st      rp,     @-r15       ; rp is our new pc when load_context executes

    st      r15,    @-r15       ; store original r15 here

    stm1    (r8, r9, r10, r11, r12, r13, r14)
    stm0    (r0, r1, r2, r3 , r4 , r5 , r6 , r7)

# we should be finished saving context here

    st      r15,    @r5         ; store pointer to saved context

#------------------------------------------------------------------------------
# hal_thread_load_context
# Load thread context
.globl  hal_thread_load_context
hal_thread_load_context:

    ld      @r4,     r15

    ldm0    (r0, r1, r2, r3, r4, r5, r6, r7)
    ldm1    (r8, r9, r10, r11, r12, r13, r14)
    ld      @r15+,  mdl
    ld      @r15+,  rp
    ld      @r15+,  ps
    ld      @r15+,  tbr
# TODO addsp to skip stack positions
    ld      @r15+,  mdl
    ld      @r15+,  mdl
    ld      @r15+,  mdl
    ld      @r15+,  mdh
    ld      @r15+,  mdl
    addsp   4
    ret

#------------------------------------------------------------------------------
# HAL longjmp, setjmp implementations
# hal_setjmp saves only to callee save registers r8, r9, r10, r11, r14, r15
# into buffer supplied in r4
# Note: These definitions are repeated in hal_arch.h. If changes are required
# remember to update both sets.
# setjmp/longjmp for FR30.  The jmpbuf looks like this:
#
# Register  jmpbuf offset
# r8        0x00
# r9        0x04
# r10       0x08
# r11       0x0c
# r14 (FP)  0x10
# r15 (SP)  0x14
# pc  (rp)  0x18

    .macro save reg
    st      \reg,   @r4
    addn    #4,     r4
    .endm

    .macro restore reg
    ld      @r4,    \reg
    addn    #4,     r4
    .endm


    .text
    .global hal_setjmp
    .type   hal_setjmp,@function
hal_setjmp:
    save    r8
    save    r9
    save    r10
    save    r11
    save    r14
    save    r15
    mov     rp,     r5
    st      r5,     @r4

# Return 0 to caller.
    ldi:8   #0,     r4
    ret

    .global hal_longjmp
hal_longjmp:
    restore r8
    restore r9
    restore r10
    restore r11
    restore r14
    restore r15
    ld      @r4,    r4
    mov     r4,     rp

# If caller attempted to return 0, return 1 instead.

    mov     r5,     r4
    or      r4,     r4
    bne     1f
    ldi:8   #1,     r4
1:
    ret

#-----------------------------------------------------------------------------
# End of context.S
