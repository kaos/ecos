##==========================================================================
##
##      vectors.S
##
##      SH exception vectors
##
##==========================================================================
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
##==========================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):    jskov
## Contributors: jskov, gthomas
## Date:         1999-05-01
## Purpose:      SH exception vectors
## Description:  This file defines the code placed into the exception
##               vectors. It also contains the first level default VSRs
##               that save and restore state for both exceptions and
##               interrupts.
## Notes:        The SH architecture files are still more SH3 specific than
##               architecture neutral, hence the sub-variant configury.
######DESCRIPTIONEND####
##
##==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_sh.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>     // CYGPKG_KERNEL_INSTRUMENT
#endif

#include <cyg/hal/arch.inc>
#include <cyg/hal/sh_regs.h>
#include <cyg/hal/sh_offsets.inc>

#include <cyg/hal/hal_intr.h>


#===========================================================================

        .file   "vectors.S"

#undef __DEBUG
        
#===========================================================================
# SR initialization value
# zero all bits except:
# MD = Processor operation mode field (privileged mode)
# I0-3 = Mask out all interrupts but NMI.
#
# When saving or restoring the state of an exception or interrupt, the bit
# CYGARC_REG_SR_RB is also set, switching the register bank. When this bit
# is set, interrupts must be disabled.
#
# Note: We could also use the BL bit to prevent interrupts, but that would
#       also prevent the use of breakpoints.

#define CYG_SR (CYGARC_REG_SR_MD|CYGARC_REG_SR_IMASK)
#define CYG_SR_BANK1 (CYGARC_REG_SR_MD|CYGARC_REG_SR_IMASK|CYGARC_REG_SR_RB)

#===========================================================================
# Start by defining the exceptions vectors.
        .section ".vectors","ax"

# reset            0xa0000000
# other exceptions VBR+0x100
# tlb miss         VBR+0x400
# interrupts       VBR+0x600


#===========================================================================
# Reset code must be PC relative so it can be executed out of the shadow
# area during startup. Not until after hal_hardware_init can the system
# be expected to provide the proper address space (at that time we
# jump to the VMA base of the code).

        .org    0x000
        .globl __reset;
__reset:
        mov.l   $__reset_platform,r0
        jmp     @r0
         nop

# Note: this is the unmapped, shadow address of the start of code
# Sadly, it is too far to just branch to.                        
        .align   2
$__reset_platform:      
#ifdef CYG_HAL_STARTUP_RAM
        .long   __reset_platform
#else
        .long   __reset_platform+0x20000000  // Uncached "shadow" address
#endif
                
#---------------------------------------------------------------------------
# Exception entry
        
        .org    0x100
__exception:

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov     #1,r7
#endif

        mov     #CYGARC_REG_EXCEVT,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $_hal_vsr_table,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop
        
        .align   2
$_hal_vsr_table:
        .long   _hal_vsr_table
        
#---------------------------------------------------------------------------
# This code handles the common part of all exception handlers.
# It saves the machine state onto the stack  and then calls
# a "C" routine to do the rest of the work. This work may result
# in thread switches, and changes to the saved state. When we return
# here the saved state is restored and execution is continued.

        .globl  _cyg_hal_default_exception_vsr
_cyg_hal_default_exception_vsr:
        
        # We come here with all register containing their
        # pre-exception values except:
        # r0_b-r7_b  = saved r0-r7
        # r7    = vector #
        # spc   = saved pc
        # ssr   = saved sr
      
        # Save away some registers
        mov     r15,r1                  ! entry sp

#ifdef __DEBUG
        mov.l   $exc_state_marker,r0
        mov.l   r0,@-r15
        mov     #CYGARC_REG_EXCEVT,r0
        mov.l   @r0,r0
        mov.l   r0,@-r15
#endif

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        add     #-12,r15                ! Space for cause, gbr, and vbr
#else
        add     #-4,r15                 ! Space for cause

        stc     gbr,r0                  ! GBR
        mov.l   r0,@-r15
        stc     vbr,r0                  ! VBR
        mov.l   r0,@-r15
#endif

        stc     spc,r0
        mov.l   r0,@-r15                ! PC
        stc     ssr,r0
        mov.l   r0,@-r15                ! SR

        sts.l   pr,@-r15                ! PR
        sts.l   mach,@-r15              ! mach
        sts.l   macl,@-r15              ! macl

        mov.l   r1,@-r15                ! entry sp
        mov.l   r14,@-r15               ! r14-r0
        mov.l   r13,@-r15
        mov.l   r12,@-r15
        mov.l   r11,@-r15
        mov.l   r10,@-r15
        mov.l   r9,@-r15
        mov.l   r8,@-r15
        stc.l   r7_bank,@-r15
        stc.l   r6_bank,@-r15
        stc.l   r5_bank,@-r15
        stc.l   r4_bank,@-r15
        stc.l   r3_bank,@-r15
        stc.l   r2_bank,@-r15
        stc.l   r1_bank,@-r15
        stc.l   r0_bank,@-r15

        # After setting the SR it will be possible to use breakpoints.
        mov.l   $nCYG_SR1,r1
        ldc     r1,sr
        # ------------------------------------------------------------
        # Register bank has changed now. 

#ifdef __DEBUG
        # It is safe to use breakpoints below this point.
        .globl  _cyg_hal_default_exception_vsr_bp_safe
_cyg_hal_default_exception_vsr_bp_safe:
#endif
        mov     #CYGARC_REG_EXCEVT,r0
        mov.l   @r0,r4
        mov     #-5,r0                  ! divide cause by 0x20
        shld    r0,r4
        mov     #CYGARC_SHREG_EVENT,r0
        mov.l   r4,@(r0,r15)            ! store decoded vector number back

        # The entire CPU state is now stashed on the stack,
        # call into C to do something with it.

        mov      r15,r4                 ! R4 = register dump

        mov.l   $restore_state,r0       ! get return link
        lds     r0,pr                   ! to procedure register

        mov.l   $_cyg_hal_exception_handler,r0
        jmp     @r0                     ! call C code, r4 = registers
         nop

        .align  2
$restore_state:
        .long   restore_state
$nCYG_SR1:
        .long   CYG_SR

#ifdef __DEBUG
$exc_state_marker:
        .long   0x77777770
#endif
        
        .extern _cyg_hal_exception_handler

$_cyg_hal_exception_handler:
        .long _cyg_hal_exception_handler

#---------------------------------------------------------------------------
# TLB miss entry

        .org    0x400
__tlb_miss:
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov      #2,r7
#endif

        mov     #CYGARC_REG_EXCEVT,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $_hal_vsr_table2,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop

        .align   2
$_hal_vsr_table2:
        .long    _hal_vsr_table

#---------------------------------------------------------------------------
# Interrupt entry

        .org    0x600
__interrupt:        

#ifdef CYGHWR_HAL_SH_IRQ_HANDLE_SPURIOUS_INTERRUPTS
        # A spurious interrupt with INTEVT=0 may be caused by
        # clearing of BL. Those interrupts need to be ignored.
        mov     #CYGARC_REG_INTEVT,r0
        mov.l   @r0,r0
        cmp/eq  #0,r0
        bf      1f
        rte
         nop
1:      
#endif        


#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov      #3,r7
#endif

        mov.l    $_hal_vsr_table_int,r1
        mov.l    @r1,r1
        jmp      @r1
         nop

        .align   2
$_hal_vsr_table_int:
        .long    _hal_vsr_table+CYGNUM_HAL_VECTOR_INTERRUPT*4

#---------------------------------------------------------------------------
# Common interrupt handling code.

        .globl _cyg_hal_default_interrupt_vsr
_cyg_hal_default_interrupt_vsr:

        # We come here with all register containing their
        # pre-exception values except:
        # r0_b-r7_b  = saved r0-r7
        # r7    = vector #
        # spc   = saved pc
        # ssr   = saved sr
        
        # Save away some registers
        mov     r15,r1                  ! entry sp

#ifdef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        add     #-12,r15                ! Space for cause, gbr, and vbr
#else
        add     #-4,r15                 ! Space for cause

        stc     gbr,r0                  ! GBR
        mov.l   r0,@-r15
        stc     vbr,r0                  ! VBR
        mov.l   r0,@-r15
#endif

        stc     spc,r0
        mov.l   r0,@-r15                ! PC
        stc     ssr,r0
        mov.l   r0,@-r15                ! SR

        sts.l   pr,@-r15                ! PR
        sts.l   mach,@-r15              ! mach
        sts.l   macl,@-r15              ! macl

        mov.l   r1,@-r15                ! entry sp
        mov.l   r14,@-r15               ! r14-r0
        mov.l   r13,@-r15
        mov.l   r12,@-r15
        mov.l   r11,@-r15
        mov.l   r10,@-r15
        mov.l   r9,@-r15
        mov.l   r8,@-r15
        stc.l   r7_bank,@-r15
        stc.l   r6_bank,@-r15
        stc.l   r5_bank,@-r15
        stc.l   r4_bank,@-r15
        stc.l   r3_bank,@-r15
        stc.l   r2_bank,@-r15
        stc.l   r1_bank,@-r15
        stc.l   r0_bank,@-r15

        # After setting the SR it will be possible to use breakpoints.
        mov.l   $nCYG_SR2,r1
        ldc     r1,sr
        # ------------------------------------------------------------
        # Register bank has changed now. 

#ifdef __DEBUG
        # It is safe to use breakpoints below this point.
        .globl  _cyg_hal_default_interrupt_vsr_bp_safe
_cyg_hal_default_interrupt_vsr_bp_safe:
#endif

        # The entire CPU state is now stashed on the stack,
        # call into C to do something with it.

        # The entire CPU state is now stashed on the stack,
        # increment the scheduler lock and call the ISR
        # for this vector.

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT                 
        mov.l   $cyg_scheduler_sched_lock,r0
        mov.l   @r0,r1
        add     #1,r1
        mov.l   r1,@r0
#endif

        mov     r15,r8                  ! R8 = register dump

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 

        mov.l   $__interrupt_stack_base,r0
        mov.l   $__interrupt_stack,r1
        cmp/hi  r15,r0                  ! if r0 > r15 or
        bt      2f
        cmp/hi  r1,r15                  ! if r15 > r1
        bf      1f
2:      mov     r1,r15                  ! change to supervisor stack
1:      mov.l   r8,@-r15                ! save old stack pointer

#endif

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov.l   $n0301,r4               ! arg0 = type = INTR,RAISE
        mov     r7,r5                   ! arg1 = vector number
        mov.l   $_cyg_instrument,r0
        mov     #0,r6                   ! arg2 = 0
        jsr      @r0                    ! call instrument function
         nop
        bra     1f
         nop

        .align  2
$n0301: 
        .long   0x0301
$_cyg_instrument:
        .extern _cyg_instrument
        .long   _cyg_instrument
1:
#endif

        # Decode the interrupt vector
#ifdef CYGARC_SH_MOD_INTC_V2
        mov.l   $nCYGARC_REG_INTEVT2,r0
#else
        mov     #CYGARC_REG_INTEVT,r0
#endif
        mov.l   @r0,r4
        mov     #-5,r0                  ! divide cause by 0x20
        shld    r0,r4
        add     #-14,r4                 ! adjust so NMI becomes 0

        mov     #CYGARC_SHREG_EVENT,r0
        mov.l   r4,@(r0,r8)             ! store decoded vector number back
                                        ! to saved state.

        // Find ISR index.
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
#define CYG_ISR_TABLE_SIZE    1
        mov     #0,r9                   ! only one ISR in the table
#else //  CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
#define CYG_ISR_TABLE_SIZE    CYGNUM_HAL_ISR_COUNT
        mov     r4,r9
        shll2   r9                      ! get from vector number to ISR index
#endif //  CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN


#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
	# If we are supporting Ctrl-C interrupts from GDB, we must squirrel
	# away a pointer to the save interrupt state here so that we can
	# plant a breakpoint at some later time.
	
	mov.l	$_hal_saved_interrupt_state,r1
	mov.l	r8,@r1
#endif

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

#if defined(CYGARC_SH_SOFTWARE_IP_UPDATE)
        # The interrupt mask bits in the SR are not updated by the
        # CPU. Proper nested operation requires the level to be
        # found and put in the SR.

        # R4 contains the vector number:
        # CYGNUM_HAL_INTERRUPT_NMI:     
        #  Ix = 15
        # CYGNUM_HAL_INTERRUPT_LVL0-CYGNUM_HAL_INTERRUPT_LVL14:
        #  Ix = 15-(CYGNUM_HAL_INTERRUPT_LVL0-R4)
        # IRA sources:
        #  Get level from IRA
        # IRB sources:
        #  Get level from IRB

        # However, doing mutiple checks and branches here is not smart.
        # Instead rely on alternative implementation where all programmed
        # priorities are also kept in a table.

        mov.l   $cyg_hal_ILVL_table,r0
        mov.b   @(r0,r4),r0
        shll2   r0
        shll2   r0
        mov.l   $unmasked_SR,r1
        or      r0,r1
        ldc     r1,sr
#endif

#endif

        mov     r9,r0
        mov.l   $_hal_interrupt_handlers,r1 ! get interrupt handler
        mov.l   @(r0,r1),r1

        mov.l   $_hal_interrupt_data,r5 ! get interrupt data
        mov.l   @(r0,r5),r5

        jsr     @r1                     ! r4=vector, r5=data
         nop

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

        # If we are returning from the last nested interrupt, move back
        # to the thread stack. interrupt_end() must be called on the
        # thread stack since it potentially causes a context switch.
        # Since we have arranged for the top of stack location to
        # contain the sp we need to go back to here, just pop it off
        # and put it in SP.

        mov.l   @r15,r15
#endif

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT

        # We only need to call _interrupt_end() when there is a kernel
        # present to do any tidying up.
        
        # on return r0 bit 1 will indicate whether a DSR is
        # to be posted. Pass this together with a pointer to
        # the interrupt object we have just used to the
        # interrupt tidy up routine.
        mov     r0,r4                   ! arg1 = isr_ret

        # Note that r8 and r9 are defined to be preserved across
        # calls by the calling convention, so they still contain
        # the register dump and the vector table index respectively.

        mov.l   $_hal_interrupt_objects,r0  ! get interrupt object table
        mov.l   @(r0,r9),r5             ! arg2 = interrupt object
        mov.l   $_interrupt_end,r0
        mov     r8,r6                   ! arg3 = saved register dump
        jsr     @r0                     ! call into C to finish off
         nop
#endif

restore_state:  
        # All done, restore CPU state and continue

        # Disable interrupts and switch register bank during the restore 
        # operation
        mov.l   $nCYG_SR_BANK1,r1
        ldc     r1,sr
        # ------------------------------------------------------------
        # Register bank has changed now. 

        #
        # Single step debugging becomes impossible after this point!
        #

        mov     r15,r0
        ldc.l   @r0+,r0_bank
        ldc.l   @r0+,r1_bank
        ldc.l   @r0+,r2_bank
        ldc.l   @r0+,r3_bank
        ldc.l   @r0+,r4_bank
        ldc.l   @r0+,r5_bank
        ldc.l   @r0+,r6_bank
        ldc.l   @r0+,r7_bank
        mov.l   @r0+,r8
        mov.l   @r0+,r9
        mov.l   @r0+,r10
        mov.l   @r0+,r11
        mov.l   @r0+,r12
        mov.l   @r0+,r13
        mov.l   @r0+,r14
        mov.l   @r0+,r15                ! return SP

        lds.l   @r0+,macl               ! macl
        lds.l   @r0+,mach               ! mach
        lds.l   @r0+,pr                 ! PR

        mov.l   @r0+,r2
        ldc     r2,ssr                  ! return SR
        mov.l   @r0+,r2
        ldc     r2,spc                  ! return PC

#ifndef CYGDBG_HAL_COMMON_INTERRUPTS_SAVE_MINIMUM_CONTEXT
        mov.l   @r0+,r2
        ldc     r2,vbr                  ! return VBR
        mov.l   @r0+,r2
        ldc     r2,gbr                  ! return GBR
#endif

        rte
         nop


#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
##-----------------------------------------------------------------------------
## Execute pending DSRs on the interrupt stack with interrupts enabled.
## Note: this can only be called from code running on a thread stack
        .globl _hal_interrupt_stack_call_pending_DSRs
_hal_interrupt_stack_call_pending_DSRs: 
        # Change to interrupt stack
        mov     r15,r2
        mov.l   $__interrupt_stack,r15

        mov.l   r2,@-r15                ! save old stack pointer
        sts.l   pr,@-r15                ! save pr on stack
        stc     sr,r3
        mov.l   r3,@-r15                ! save sr on stack

        # enable interrupts
        hal_cpu_int_enable r0,r1

        # Call into kernel which will execute DSRs
        mov.l   $_cyg_interrupt_call_pending_DSRs,r0
        jsr     @r0
         nop

        # Get old sr, pr, and stack values
        mov.l   @r15+,r3                ! get old sr
        lds.l   @r15+,pr                ! get old pr
        mov.l   @r15+,r15               ! get old stack pointer

        # Restore SR interrupt state
        hal_cpu_int_merge r3,r0,r1
        rts
         nop

#endif // CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
#endif // CYGFUN_HAL_COMMON_KERNEL_SUPPORT

        .align  2        
#ifdef __DEBUG
$int_state_marker:
        .long   0x77777771
#endif

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        .extern _hal_saved_interrupt_state
$_hal_saved_interrupt_state:
        .long   _hal_saved_interrupt_state
#endif

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 
$__interrupt_stack:
        .long   __interrupt_stack
$__interrupt_stack_base:
        .long   __interrupt_stack_base
#endif
$nCYG_SR2:
        .long   CYG_SR
$nCYG_SR_BANK1:
        .long   CYG_SR_BANK1
#ifdef CYGARC_SH_MOD_INTC_V2
$nCYGARC_REG_INTEVT2:
        .long   CYGARC_REG_INTEVT2
#endif
$_hal_interrupt_handlers:
        .long   _hal_interrupt_handlers
$_hal_interrupt_data:
        .long   _hal_interrupt_data
$_hal_interrupt_objects:
        .long   _hal_interrupt_objects

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
$cyg_hal_ILVL_table:
        .long   _cyg_hal_ILVL_table
$unmasked_SR:
        .long   (CYG_SR & ~CYGARC_REG_SR_IMASK)
#endif // CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
        
        
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
        .extern _cyg_scheduler_sched_lock
        .extern _interrupt_end
        .extern _cyg_interrupt_call_pending_DSRs

$_cyg_interrupt_call_pending_DSRs:
        .long   _cyg_interrupt_call_pending_DSRs
$cyg_scheduler_sched_lock:
        .long   cyg_scheduler_sched_lock
$_interrupt_end:
        .long   _interrupt_end
#endif

#---------------------------------------------------------------------------
# Platform initialization (reset)
__reset_platform:               
        # Initialize CPU
        mov.l   $nCYG_SR,r1             ! Put CPU in a well-known state
        ldc     r1,sr
        mov     #0,r0
        mov     #CYGARC_REG_CCR,r1      ! Disable cache
        mov.l   r0,@r1
        mov     #CYGARC_REG_MMUCR,r1    ! Disable MMU
        mov.l   r0,@r1
        mov     #CYGARC_REG_BBRA,r1     ! Disable UBC Channel A
        mov.w   r0,@r1
        mov     #CYGARC_REG_BBRB,r1     ! Disable UBC Channel B
        mov.w   r0,@r1
        mov     #CYGARC_REG_BRCR,r1     ! Reset UBC common register
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_TSTR,r1     ! Disable timers
        mov.b   r0,@r1
        mov.l   $CYGARC_REG_IPRA,r1     ! Disable interrupt request sources
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRB,r1
        mov.w   r0,@r1
#ifdef CYGARC_SH_MOD_INTC_V2
        mov.l   $CYGARC_REG_IPRC,r1
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRD,r1
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRE,r1
        mov.w   r0,@r1
#ifdef CYGARC_SH_MOD_INTC_V3
        mov.l   $CYGARC_REG_IPRF,r1
        mov.w   r0,@r1
#endif
	mov.w   $nCYG_ICR1_INIT,r0
        mov.l   $CYGARC_REG_ICR1,r1     ! Set interrupt controller to IRQ mode
        mov.w   r0,@r1
#endif
	mov.w	$nCYG_WTCSR,r0          ! Clear watchdog
	mov	#CYGARC_REG_WTCSR,r1
	mov.w	r0,@r1

        # Initialize VBR if necessary
#if     !defined(CYG_HAL_STARTUP_RAM) ||                \
        (       defined(CYG_HAL_STARTUP_RAM) &&         \
                !defined(CYGSEM_HAL_USE_ROM_MONITOR))
        mov.l   $__reset,r1             ! Set VBR
        ldc     r1,vbr
#endif

        
#ifdef __DEBUG
        mov     #0,r0
        mov     #1,r1
        mov     #2,r2
        mov     #3,r3
        mov     #4,r4
        mov     #5,r5
        mov     #6,r6
        mov     #7,r7
        mov     #8,r8
        mov     #9,r9
        mov     #10,r10
        mov     #11,r11
        mov     #12,r12
        mov     #13,r13
        mov     #14,r14
#endif

         bra    hal_hw_init
          nop

$nCYG_WTCSR:
	.word	0xa500		! clear all CSR bits

	.align	2

$nCYG_SR:
        .long   CYG_SR
$CYGARC_REG_TSTR:
        .long   CYGARC_REG_TSTR
$CYGARC_REG_IPRA:
        .long   CYGARC_REG_IPRA
$CYGARC_REG_IPRB:
        .long   CYGARC_REG_IPRB
#ifdef CYGARC_SH_MOD_INTC_V2
$CYGARC_REG_IPRC:
        .long   CYGARC_REG_IPRC
$CYGARC_REG_IPRD:
        .long   CYGARC_REG_IPRD
$CYGARC_REG_IPRE:
        .long   CYGARC_REG_IPRE
$CYGARC_REG_ICR1:
        .long   CYGARC_REG_ICR1
#endif
#ifdef CYGARC_SH_MOD_INTC_V2
$CYGARC_REG_IPRF:
        .long   CYGARC_REG_IPRF
$nCYG_ICR1_INIT:
	.word	CYGARC_REG_ICR1_INIT
	.align	2
#endif
$__reset:
        .long   __reset
	
hal_hw_init:
        
        # Call platform specific hardware initialization
        # This may include memory controller initialization. It is not
        # safe to access RAM until after this point.
        # Note that caches must not be enabled until after this point,
        # since we may be fiddling the FRQCR which cannot be safely done
        # by code in burst/cachable memory (errata SH7-184e).
        hal_hardware_init

        # Now copy necessary bits to RAM and jump to the VMA base

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_SUBSETROMRAM)

        # Copy data from ROM to ram
        mov.l   $__rom_data_start,r3    ! r3 = rom start
        mov.l   $__ram_data_start,r4    ! r4 = ram start
        mov.l   $__ram_data_end,r5      ! r5 = ram end
        
        cmp/eq  r4,r5                   ! skip if no data
        bt      2f
1:      mov.l   @r3+,r0                 ! get word from ROM
        mov.l   r0,@r4                  ! store in RAM
        add     #4,r4
        cmp/eq  r4,r5                   ! compare
        bf      1b                      ! loop if not yet done
2:


        # Jump to the proper VMA base of the code.
        mov.l   $_complete_setup,r0
        jmp     @r0
         nop
        .align 2

        .extern __rom_data_start
        .extern __ram_data_start
        .extern __ram_data_end
$__rom_data_start:
        .long   __rom_data_start
$__ram_data_start:
        .long   __ram_data_start
$__ram_data_end:
        .long   __ram_data_end


$_complete_setup:
        .long _complete_setup

#elif defined(CYG_HAL_STARTUP_ROMRAM)

        # Copy everything to the proper VMA base and jump to it.
        mov.l   $_vectors_lma,r0
        mov.l   $_vectors_vma,r1
        mov.l   $_end,r2
1:      mov.l   @r0+,r3                 ! get word from ROM
        mov.l   r3,@r1                  ! store in RAM
        add     #4,r1
        cmp/eq  r1,r2                   ! compare
        bf      1b                      ! loop if not yet done
        mov.l   $_complete_setup,r0
        jmp     @r0
         nop
        .align  2

        .extern __vector_code_lma

$_vectors_lma:
        .long __vector_code_lma
$_vectors_vma:
        .long __reset
$_end:
        .long _end
$_complete_setup:
        .long _complete_setup

#else

        # Jump to remaining setup code. Relative branch is OK since VMA=LMA.
        bra   _complete_setup
         nop
                
#endif

#-----------------------------------------------------------------------------
# Complete target initialization and setup.
# After this point we can use absolute addressing modes and access all the
# memory in the system.

_complete_setup:
                
        # Set up monitor related stuff (vectors primarily)
        hal_mon_init

        # set up stack
        mov.l    $__startup_stack,r15

        # clear BSS
        mov.l   $__bss_start,r3         ! r3 = start
        mov.l   $__bss_end,r4           ! r4 = end
        mov     #0,r0                   ! r0 = 0
1:      cmp/eq  r3,r4                   ! skip if no bss
        bt      2f
        mov.l   r0,@r3                  ! store zero
        add     #4,r3
        bra     1b                      ! loop
         nop
2:

        # It is now safe to call C functions which may rely on initialized
        # data.
        
#        # Initialize MMU.
#        .extern hal_MMU_init
#        jsr      hal_MMU_init
#         nop
        
        # Enable caches
        mov.l    $_cyg_hal_enable_caches,r1
        jsr      @r1
         nop

        # Variant HALs may need to do something special before we continue
        mov.l    $_hal_variant_init,r1
        jsr      @r1
         nop

        # Platform initialization
        mov.l    $_hal_platform_init,r1
        jsr      @r1
         nop
        
        # call c++ constructors
        mov.l    $_cyg_hal_invoke_constructors,r1
        jsr      @r1
         nop

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        mov.l    $_initialize_stub,r1
        jsr      @r1
         nop
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        mov.l    $_hal_ctrlc_isr_init,r1
        jsr      @r1
         nop
#endif

        mov.l    $_cyg_start,r1
        jsr      @r1
         nop
9:      
        bra      9b                     ! if we return, loop
         nop

        .align  2
$__startup_stack:
        .long   __startup_stack

        .extern __bss_start
        .extern __bss_end
        .extern _cyg_hal_invoke_constructors
        .extern _hal_variant_init
        .extern _hal_platform_init
        .extern _cyg_start

$__bss_start:
        .long   __bss_start
$__bss_end:
        .long   __bss_end
$_cyg_hal_invoke_constructors:
        .long   _cyg_hal_invoke_constructors
$_cyg_hal_enable_caches:
        .long   _cyg_hal_enable_caches
$_hal_variant_init:
        .long   _hal_variant_init
$_hal_platform_init:
        .long   _hal_platform_init
$_cyg_start:
        .long   _cyg_start

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern _initialize_stub
$_initialize_stub:
        .long   _initialize_stub
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        .extern _hal_ctrlc_isr_init
$_hal_ctrlc_isr_init:
        .long   _hal_ctrlc_isr_init
#endif

#---------------------------------------------------------------------------
# Cache operations
# These need to be written in assembly to ensure they do not rely on data
# in cachable space (i.e., code must use registers exclusively, not the stack).

# This macro must be used at the top of each cache function. It ensures
# that the code gets executed from a shadow region where caching is disabled
# (0xA0000000).
        .macro GOTO_NONCACHED_SHADOW
        mova    10f,r0
        mov.l   $MASK,r1
        and     r1,r0
        mov.l   $BASE,r1
        or      r1,r0
        jmp     @r0
         nop
        .align  2
10:
        .endm

        .globl _cyg_hal_cache_enable
_cyg_hal_cache_enable:
        GOTO_NONCACHED_SHADOW
        mov     #CYGARC_REG_CCR,r1
        mov.l   @r1,r0
        or      #CYGARC_REG_CCR_CE,r0
        mov.l   r0,@r1
        nop
        rts
         nop

        .globl _cyg_hal_cache_disable
_cyg_hal_cache_disable:
        GOTO_NONCACHED_SHADOW
        mov     #CYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov     #CYGARC_REG_CCR_CE,r2
        not     r2,r2
        and     r2,r0
        mov.l   r0,@r1
        nop
        rts
         nop
        
        .globl _cyg_hal_cache_invalidate_all
_cyg_hal_cache_invalidate_all:
        GOTO_NONCACHED_SHADOW
        mov     #CYGARC_REG_CCR,r1
        mov.l   @r1,r0
        or      #CYGARC_REG_CCR_CF,r0
        mov.l   r0,@r1
        nop                             ! Nothing in the docs suggest we need
        nop                             ! nops here, but without them, the
        nop                             ! CPU crashes.
        rts
         nop

        .globl _cyg_hal_cache_sync
_cyg_hal_cache_sync:
        GOTO_NONCACHED_SHADOW
        mov.l   $CYGARC_REG_CACHE_ADDRESS_FLUSH,r0
        mov.l   $CYGARC_REG_CACHE_ADDRESS_BASE,r1
        mov.l   $CYGARC_REG_CACHE_ADDRESS_TOP,r2
        mov.l   $CYGARC_REG_CACHE_ADDRESS_STEP,r3
1:      cmp/hi  r1,r2
        bf      2f
        mov.l   r0,@r1
        bra     1b        
         add     r3,r1                  ! delay slot!
2:      nop
        rts
         nop

	! r4 = base
	! r5 = size
        .globl _cyg_hal_cache_sync_region
_cyg_hal_cache_sync_region:
        GOTO_NONCACHED_SHADOW
	mov.l   10f,r0
	and	r4,r0		! array index
        mov.l   11f,r1
	add	r0,r1           ! base (aligned, A set)

	! make sure top is aligned to start of _next_ cache line
	mov	r1,r0
	add     r5,r0		! top (non-aligned)
	add	#2*HAL_UCACHE_LINE_SIZE-1,r0
	mov.l	13f,r2
	and	r0,r2

        mov.l   $CYGARC_REG_CACHE_ADDRESS_STEP,r3
        mov.l   12f,r5
1:      cmp/hi  r1,r2
        bf      3f
	mov	r4,r0		! create address tag
	and	r5,r0
	mov.l   r0,@r1          ! store tag in array, causing (sync+)invalidate
		                ! if the tag matches any of the lines
        add     r3,r4		! inc address tag
        bra     1b        
         add     r3,r1          ! inc array index, delay slot!
3:      nop
        rts
         nop

	.align 2
10:	.long	((HAL_UCACHE_SIZE/HAL_UCACHE_WAYS)-1)&~0xf ! mask
11:	.long   CYGARC_REG_CACHE_ADDRESS_BASE|CYGARC_REG_CACHE_ADDRESS_ADDRESS
12:	.long	CYGARC_REG_CACHE_ADDRESS_TAG_Mask
13:	.long   ~(HAL_UCACHE_LINE_SIZE-1)
	
        .globl _cyg_hal_cache_write_mode
_cyg_hal_cache_write_mode:
        GOTO_NONCACHED_SHADOW
        # Mode argument in r4.
        # Read current state and mask out the two caching mode bits
        mov     #CYGARC_REG_CCR,r1
        mov.l   @r1,r3
        mov     #CYGARC_REG_CCR_CB|CYGARC_REG_CCR_WT,r2
	and	r2,r4
        not     r2,r2
        and     r2,r3
        # Or in the new settings and restore to CCR
        or      r4,r3
        mov.l   r3,@r1
        nop
        rts
         nop

        .align  2
$CYGARC_REG_CACHE_ADDRESS_FLUSH:
        .long   CYGARC_REG_CACHE_ADDRESS_FLUSH
$CYGARC_REG_CACHE_ADDRESS_BASE:
        .long   CYGARC_REG_CACHE_ADDRESS_BASE
$CYGARC_REG_CACHE_ADDRESS_TOP:
        .long   CYGARC_REG_CACHE_ADDRESS_TOP
$CYGARC_REG_CACHE_ADDRESS_STEP:
        .long   CYGARC_REG_CACHE_ADDRESS_STEP


$MASK:
        .long   0x1fffffff              ! mask off top 3 bits
$BASE:
        .long   0xa0000000              ! base of non-cachable memory
        
#---------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.

        .data

        .extern _hal_default_isr

        .globl  _hal_interrupt_handlers
_hal_interrupt_handlers:
        .rept   CYG_ISR_TABLE_SIZE
        .long   _hal_default_isr
        .endr

        .globl  _hal_interrupt_data
_hal_interrupt_data:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

        .globl  _hal_interrupt_objects
_hal_interrupt_objects:
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
        .globl  _cyg_hal_ILVL_table
_cyg_hal_ILVL_table:
        # The first entries in the table have static priorities.

        .byte   0xf                     // NMI
        .byte   0xf                     // Reserved
        .byte   0xf                     // LVL0
        .byte   0xe                     // LVL1
        .byte   0xd                     // LVL2
        .byte   0xc                     // LVL3
        .byte   0xb                     // LVL4
        .byte   0xa                     // LVL5
        .byte   0x9                     // LVL6
        .byte   0x8                     // LVL7
        .byte   0x7                     // LVL8
        .byte   0x6                     // LVL9
        .byte   0x5                     // LVL10
        .byte   0x4                     // LVL11
        .byte   0x3                     // LVL12
        .byte   0x2                     // LVL13
        .byte   0x1                     // LVL14
        .byte   0xf                     // Reserved

        # The rest of the table consists of programmable levels, maintained
        # by the HAL_INTERRUPT_SET_LEVEL macro.
        .rept   (CYGNUM_HAL_ISR_MAX-CYGNUM_HAL_INTERRUPT_RESERVED_3E0)
        .byte   0xf
        .endr
#endif // CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

#---------------------------------------------------------------------------
## Temporary interrupt stack
        
        .section ".bss"

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 
        .balign 16
        .global _cyg_interrupt_stack_base
_cyg_interrupt_stack_base:
__interrupt_stack_base:
        .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
        .byte 0
        .endr
        .balign 16
        .global _cyg_interrupt_stack
_cyg_interrupt_stack:
__interrupt_stack:
        
        .long   0,0,0,0
#endif // CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK


#ifdef CYGSEM_HAL_ROM_MONITOR
  // Enough space for stub to handle downloads. If using thread capabilities
  // it will be using the RAM application's stack.
# define STARTUP_STACK_SIZE 1024
#else
# ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
   // Enough space to run constructors.
   // FIXME: 512 is enough for all tests. doc/examples/twothreads
   // calls printf on this stack though, and so requires more space.
#  define STARTUP_STACK_SIZE 1024
# else
#  define STARTUP_STACK_SIZE CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
# endif
#endif

        .balign 16
__startup_stack_base:
        .rept   STARTUP_STACK_SIZE
        .byte 0
        .endr
        .balign 16
__startup_stack:
        .long   0,0,0,0

#---------------------------------------------------------------------------
# end of vectors.S
