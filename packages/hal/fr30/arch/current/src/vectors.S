##=============================================================================
##
##      vectors.S
##
##      fr30 startup code and exception and interrupt vectors
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
## Copyright (C) 2007 eCosCentric Ltd.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
##
## Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
## at http://sources.redhat.com/ecos/ecos-license/
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):   larsi
## Contributors:larsi
## Date:        2006-06-26
## Purpose:     fr30 exception vectors
## Description: This file defines the code placed into the exception
##              vectors. It also contains the startup code and first
##              level default VSRs that save and restore state for
##              both exceptions and interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================


#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include CYGBLD_HAL_PLATFORM_H

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif /* CYGPKG_KERNEL */

#include <cyg/hal/arch.inc>

#==============================================================================

    .file   "vectors.S"

#==============================================================================
# Real startup code. We jump here from the various reset vectors to set up the
# world.

    .text
    .globl	_start

_start:

# disable interrupts and set priority to lowest (=disable)
    andccr  #0xef
    stilm   #0x0

    hal_diag_init_led
#ifdef CYGPKG_HAL_FR30_FLASH_INIT_DEFINED
    hal_flash_init
#endif
    hal_cpu_init

#ifdef CYGPKG_HAL_FR30_MEMC_INIT_DEFINED
    hal_memc_init
#endif

    hal_intc_init

    hal_cache_init

    hal_timer_init

# Zero the BSS. If the BSS is not a whole number of words
# long we will write up to 3 extra bytes at the end.
# (This should not be a problem usually).
    ldi:32  #__bss_end - 8,     r12
    ldi:32  #__bss_start - 4,   r13
    eor     r0, r0 ; zero r0
2:
    add     #0x4,   r13
    cmp     r12,    r13
    ble:d   2b
    st      r0,     @r13


#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)

# In a ROM booted system, we also need to copy the data section
# out to the RAM.
    ldi:32  #__rom_data_start - 4,  r11
    ldi:32  #__ram_data_start - 4,  r13
    ldi:32  #__ram_data_end - 8,    r12
3:
    add     #0x4,   r11
    add     #0x4,   r13
    ld      @r11,   r0
    cmp     r12,    r13
    ble:d   3b
    st      r0,     @r13

#endif

    # Set up the stacks
    # Begin with interrupt (system) stack

    ldi:32  #__interrupt_stack, r11
    mov     r11,    ssp

    # and now continue with user stack

    orccr   #0x20
    ldi:32  #__user_stack,  r11
    mov     r11,    usp

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMRAM)
# If we are here, initialize the hal_vsr_table. RAM startup
# configurations can assume that Redboot has already set it up.

    .extern hal_mon_init
    ldi:32  #hal_mon_init,  r11
    call    @r11

#endif

    .extern hal_variant_init
    ldi:32  #hal_variant_init,  r11
    call	@r11

    .extern hal_platform_init
    ldi:32  #hal_platform_init,  r11
    call    @r11


#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    // This is here so we can debug the constructors.
    .extern initialize_stub
    ldi:32  #initialize_stub,   r11
    call    @r11
#endif

    .extern cyg_hal_invoke_constructors
    ldi:32  #cyg_hal_invoke_constructors,  r11
    call    @r11

# TODO integrate into hal_intr.h
# set irq 25 priority (for reload timer 1)
    ldi:20  #0x449,     r4
    ldi:8   #0x10,      r5
    stb     r5,         @r4

#ifdef CYGDBG_HAL_DEBUG_GDB_INITIAL_BREAK
        .extern breakpoint
        call breakpoint
#endif

    .extern cyg_start
    ldi:32  #cyg_start,  r11
    call    @r11

# Hmm.  Not expecting to return from cyg_start, endless nop loop
1:  nop
    call   1b

#==============================================================================
# Default exception VSR

    .align  2, 0xcc
    .globl  __default_exception_vsr
__default_exception_vsr:

    ## We enter here with the CPU state still in the registers and:
    ##  @(ssp,8)    PS pushed by hardware
    ##  @(ssp,4)    PC pushed by hardware
    ##  @(ssp)      old register r0 content pushed by trampoline
    ##  r0 now contains the vector number

# at first switch to USP (set bit 5 in CCR in PS)
    orccr   #0x20

    st      r0,     @-r15
    st      mdl,    @-r15
    st      mdh,    @-r15

    mov     r0,     mdh         ; save exception/interrupt number
    mov     r15,    r0
    addn    #+12,   r0
    mov     r0,     mdl         ; save r15 also in mdl

    st      r0,     @-r15       ; store usp
    mov     ssp,    r0
    st      r0,     @-r15       ; store ssp (TODO maybe have to sub 12 before)
    st      rp,     @-r15
    st      tbr,    @-r15
    addsp   #-8                 ; skip 2 positions for PS and PC
    mov     r15,    r0          ; save stack position to later store PS and PC

    st      mdl,    @-r15       ; store original r15 here
    stm1    (r8, r9, r10, r11, r12, r13, r14)
    stm0    (r1, r2, r3, r4, r5, r6, r7)

    mov     ssp,    r14
    ld      @r14,   r1          ; get original r0 content
    st      r1,     @-r15       ; and store it

    ld      @(r14,4),   r2      ; get hardware pushed PC
    st      r2,     @r0         ; and store it
    ld      @(r14,8),   r3      ; get hardware pushed PS
    addn    #4,     r0          ; and
    st      r3,     @r0         ; store it

# we should be finished saving context here

# Call exception handler
    .extern hal_default_exception_handler

    ldi:32  #hal_default_exception_handler,  r11
    mov     r15,    r5          ; pointer to saved state as second argument
    call:d  @r11
    mov     mdh,    r4          ; exception number as first argument

__default_exception_vsr_return:


    ## At this point, the user stack (USP) contains:
    ## @(usp,0x60) trap number
    ## @(usp,0x5c) mdl
    ## @(usp,0x58) mdh
    ## @(usp,0x54) usp
    ## @(usp,0x50) ssp
    ## @(usp,0x4c) rp
    ## @(usp,0x48) tbr
    ## @(usp,0x44) ps
    ## @(usp,0x40) pc
    ## @(usp,0x3c) r15
    ## @(usp,0x38) r14
    ## @(usp,0x34) r13
    ## @(usp,0x30) r12
    ## @(usp,0x2c) r11
    ## @(usp,0x28) r10
    ## @(usp,0x24) r9
    ## @(usp,0x20) r8
    ## @(usp,0x1c) r7
    ## @(usp,0x18) r6
    ## @(usp,0x14) r5
    ## @(usp,0x10) r4
    ## @(usp,0xc)  r3
    ## @(usp,8)    r2
    ## @(usp,4)    r1
    ## @(usp)      r0
    ##
    ## and system stack (SSP) contains:
    ## @(ssp,8) PS
    ## @(ssp,4) PC
    ## @(ssp)   old r0 content

# we can reuse the code from __default_interrupt_vsr
    ldi:32  #hal_exception_return,  r0
    jmp    @r0

#==============================================================================
# Default interrupt VSR
#
#

    .align  2, 0xcc
    .globl  __default_interrupt_vsr
__default_interrupt_vsr:

    ## We enter here with the CPU state still in the registers and:
    ##  r0          vector number
    ##  @(ssp)      old register r0 content pushed by trampoline
    ##  @(ssp,4)    PS pushed by hardware
    ##  @(ssp,8)    PC pushed by hardware

    # begin to save registers (to USP)
    orccr   #0x20

    st      mdl,    @-r15
    st      mdh,    @-r15
    st      rp,     @-r15

    stm1    (r8, r9, r10, r11, r12, r13)
    stm0    (r1, r2, r3 , r4 , r5 , r6, r7)

    andccr  #0xdf
    ld      @r15+,  r8          ; read old r0 content from SSP
    ld      @r15+,  r9          ; read pc pushed by hardware
    ld      @r15+,  r10         ; read ps pushed by hardware
    orccr   #0x20

    st      r8,     @-r15       ; push old r0 content to USP

# we should be finished saving irq context here

#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)
# Increment scheduler lock
    .extern cyg_scheduler_sched_lock
    ldi:32  #cyg_scheduler_sched_lock,   r1
    ld      @r1,    r2
    addn    #1,     r2
    st      r2,     @r1
#endif

# Call hal_interrupt_handlers[vector](vector, cyg_hal_interrupt_data[vector])
    mov     r0,     r8                      ; copy the vector
    lsl     #2,     r8                      ; multiply vector by 4

    ldi:32  #hal_interrupt_handlers,    r13 ; load handlers table
    ld      @(r13, r8), r1                  ; current handler
    ldi:32  #hal_interrupt_data,    r13     ; load data table
    ld      @(r13, r8), r5                  ; current data, second argument
    call:d  @r1
    mov     r0,    r4                      ; exception number as argument

# At this point:
# r15 = stack pointer /*(should be usable as pointer to HAL_SavedRegisters)*/
# r4  = ISR return code (returned by call)
# r8  = ISR table offset (saved across call)
# r0  = vector number (NOT saved across call, but we don't need it)

/*
	# If we are returning from the last nested interrupt, move back
	# to the thread stack. interrupt_end() must be called on the
	# thread stack since it potentially causes a context switch.
	
	hal_from_intstack	
*/

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

# Call interrupt_end(return_value, hal_interrupt_objects[vector], regs)
# - r4 is the return value from the ISR
# - regs points to saved CPU context

    ldi:32  #hal_interrupt_objects,  r13    ; load objects table
    ld      @(r13, r8), r5                  ; current object, second argument
    ldi:32  #interrupt_end, r12
    call:d  @r12
    mov     r15,    r6                      ; third argument TODO is not in
                                            ; HAL_Saved_Registers format

#endif

##.globl hal_exception_return
hal_exception_return:

# Now pull saved state from stack and return to
# what thread was originally doing.

# at first store return values to SSP
    andccr  #0xdf
    st      r10,    @-r15                   ; store ps
    st      r9,     @-r15                   ; store pc
    orccr   #0x20

# and then restore normal registers from USP
    ldm0    (r0, r1, r2 , r3 , r4 , r5, r6, r7)
    ldm1    (r8, r9, r10, r11, r12, r13)

    ld      @r15+,  rp
    ld      @r15+,  mdh
    ld      @r15+,  mdl

    andccr  #0xdf
    reti

#==============================================================================
# Exception trampolines
# TBR table points to these short code sequences here that push the vector
# number on to the stack and then indirect via the VSR table to a handler.
# (__default_interrupt_vsr)

    .text

# macro to create exception handler (no error code)

.macro  hal_fr30_exception_noerr idx
    .globl hal_fr30_exception_noerr_\idx
hal_fr30_exception_noerr_\idx:
    st	r0,  @-r15
    ldi:32  #hal_vsr_table + \idx * 4,  r0
    ld  @r0,    r0
    jmp:d   @r0
    ldi:8   #\idx,    r0
.endm

    # Now generate all the default exception VSR trampolines.

#   hal_fr30_exception_noerr  1 no trampoline needed for reset and mode vector
    hal_fr30_exception_noerr  2
    hal_fr30_exception_noerr  3
    hal_fr30_exception_noerr  4
    hal_fr30_exception_noerr  5
    hal_fr30_exception_noerr  6
    hal_fr30_exception_noerr  7
    hal_fr30_exception_noerr  8
    hal_fr30_exception_noerr  9
    hal_fr30_exception_noerr 10
    hal_fr30_exception_noerr 11
    hal_fr30_exception_noerr 12
    hal_fr30_exception_noerr 13
    hal_fr30_exception_noerr 14

#==============================================================================
# IRQ handler trampolines

# macro to create exception handler (no error code)

.macro  hal_fr30_irq_handler idx
    .globl hal_fr30_irq_\idx
hal_fr30_irq_\idx:
    st  r0, @-r15
    ldi:32  #hal_vsr_table + \idx * 4,  r0
    ld  @r0,    r0
    jmp:d   @r0
    ldi:8   #\idx,    r0
.endm

    hal_fr30_irq_handler 15
    hal_fr30_irq_handler 16
    hal_fr30_irq_handler 17
    hal_fr30_irq_handler 18
    hal_fr30_irq_handler 19
    hal_fr30_irq_handler 20
    hal_fr30_irq_handler 21
    hal_fr30_irq_handler 22
    hal_fr30_irq_handler 23
    hal_fr30_irq_handler 24
    hal_fr30_irq_handler 25
    hal_fr30_irq_handler 26
    hal_fr30_irq_handler 27
    hal_fr30_irq_handler 28
    hal_fr30_irq_handler 29
    hal_fr30_irq_handler 30
    hal_fr30_irq_handler 31
    hal_fr30_irq_handler 32
    hal_fr30_irq_handler 33
    hal_fr30_irq_handler 34
    hal_fr30_irq_handler 35
    hal_fr30_irq_handler 36
    hal_fr30_irq_handler 37
    hal_fr30_irq_handler 38
    hal_fr30_irq_handler 39
    hal_fr30_irq_handler 40
    hal_fr30_irq_handler 41
    hal_fr30_irq_handler 42
    hal_fr30_irq_handler 43
    hal_fr30_irq_handler 44
    hal_fr30_irq_handler 45
    hal_fr30_irq_handler 46
    hal_fr30_irq_handler 47
    hal_fr30_irq_handler 48
    hal_fr30_irq_handler 49
    hal_fr30_irq_handler 50
    hal_fr30_irq_handler 51
    hal_fr30_irq_handler 52
    hal_fr30_irq_handler 53
    hal_fr30_irq_handler 54
    hal_fr30_irq_handler 55
    hal_fr30_irq_handler 56
    hal_fr30_irq_handler 57
    hal_fr30_irq_handler 58
    hal_fr30_irq_handler 59
    hal_fr30_irq_handler 60
    hal_fr30_irq_handler 61
    hal_fr30_irq_handler 62
    hal_fr30_irq_handler 63


    .data
//
// "Vectors" - fixed location data items
// This section contains any data which might be shared between
// an eCos application and any other environment, e.g. the debug
// ROM.
//
    .section ".fixed_vectors"
// Space for the virtual vectors        
    .balign 4
// Vectors used to communicate between eCos and ROM environments
    .globl  hal_virtual_vector_table
hal_virtual_vector_table:
    .rept   64                  // CYGNUM_CALL_IF_TABLE_SIZE
    .long   0
    .endr

    .globl  hal_vsr_table
hal_vsr_table:
    .rept   CYGNUM_HAL_VSR_COUNT            // exceptions & interrupts
    .long   0
    .endr

#==============================================================================
# Initial and interrupt stack

    .section ".bss"

    .balign 4
    .global cyg_interrupt_stack_base
cyg_interrupt_stack_base:
__interrupt_stack_base:
    .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
    .byte 0
    .endr
    .balign 4
    .global cyg_interrupt_stack
cyg_interrupt_stack:
__interrupt_stack:
    .long   0,0,0,0,0,0,0,0


#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    .global __stub_stack_base
__stub_stack_base:
    .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
    .byte 0
    .endr
    .balign 4
    .global __stub_stack
__stub_stack:

    .long   0,0,0,0,0,0,0,0
#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

    .balign 4
__user_stack_base:
//  FIXME TODO import symbolic constant from C-code
    .rept 4532
    .byte 0
    .endr
    .balign 4
__user_stack:
    .long   0

#------------------------------------------------------------------------------
# end of vectors.S
