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
#include CYGHWR_MEMORY_LAYOUT_H
        
#include <cyg/hal/arch.inc>
#include <cyg/hal/variant.inc>
#include <cyg/hal/sh_regs.h>
#include <cyg/hal/sh_offsets.inc>

#include <cyg/hal/hal_intr.h>


#===========================================================================

        .file   "vectors.S"

#undef __DEBUG
        
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
FUNC_START(_reset)
        mov.l   $_reset_platform,r0
        jmp     @r0
         nop

# Note: this is the unmapped, shadow address of the start of code
# Sadly, it is too far to just branch to.                        
        .align   2
$_reset_platform:
#ifdef CYG_HAL_STARTUP_RAM
        .long   CYG_LABEL_DEFN(_reset_platform)
#elif defined(CYG_HAL_STARTUP_ROMRAM)
        // Uncached "shadow" address but adjusted for VMA/LMA differences
        .long   __reset_platform+0x20000000-CYGMEM_REGION_ram+CYGMEM_REGION_rom
#else
        // Uncached "shadow" address
        .long   CYG_LABEL_DEFN(_reset_platform)+0x20000000
#endif
                
#---------------------------------------------------------------------------
# Exception entry
        
        .org    0x100
__exception:

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov     #1,r7
#endif

        mov.l   $nCYGARC_REG_EXCEVT,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $hal_vsr_table2,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop
        
        .align   2
        SYM_PTR_REFn(hal_vsr_table,2)
        
#---------------------------------------------------------------------------
# This code handles the common part of all exception handlers.
# It saves the machine state onto the stack  and then calls
# a "C" routine to do the rest of the work. This work may result
# in thread switches, and changes to the saved state. When we return
# here the saved state is restored and execution is continued.

FUNC_START(cyg_hal_default_exception_vsr)
        
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
        mov.l   $nCYGARC_REG_EXCEVT,r0
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
        mov.l   $nCYGARC_REG_EXCEVT,r0
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

        mov.l   $cyg_hal_exception_handler,r0
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

        SYM_PTR_REF(cyg_hal_exception_handler)

#---------------------------------------------------------------------------
# TLB miss entry

        .org    0x400
__tlb_miss:
#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)
        mov      #2,r7
#endif

        mov.l   $nCYGARC_REG_EXCEVT,r1
        mov.l   @r1,r1
        shlr2   r1
        shlr    r1                      ! divide cause by 0x08
        mov.l   $hal_vsr_table3,r0
        mov.l   @(r0,r1),r1
        jmp     @r1
         nop

        .align   2
        SYM_PTR_REFn(hal_vsr_table,3)
$nCYGARC_REG_EXCEVT:
        .long   CYGARC_REG_EXCEVT

#---------------------------------------------------------------------------
# Interrupt entry

        .org    0x600
__interrupt:        

#ifdef CYGHWR_HAL_SH_IRQ_HANDLE_SPURIOUS_INTERRUPTS
        # A spurious interrupt with INTEVT=0 may be caused by
        # clearing of BL. Those interrupts need to be ignored.
        mov.l   $nCYGARC_REG_INTEVT,r0
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

        mov.l    $hal_vsr_table_int,r1
        mov.l    @r1,r1
        jmp      @r1
         nop

        .align   2
$hal_vsr_table_int:
        .long    CYG_LABEL_DEFN(hal_vsr_table)+CYGNUM_HAL_VECTOR_INTERRUPT*4

#---------------------------------------------------------------------------
# Common interrupt handling code.

FUNC_START(cyg_hal_default_interrupt_vsr)

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

        mov.l   $cyg_interrupt_stack_base,r0
        mov.l   $cyg_interrupt_stack,r1
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
        mov.l   $cyg_instrument,r0
        mov     #0,r6                   ! arg2 = 0
        jsr      @r0                    ! call instrument function
         nop
        bra     1f
         nop

        .align  2
$n0301: 
        .long   0x0301
        SYM_PTR_REF(cyg_instrument)
1:
#endif

        # Decode the interrupt vector, and find ISR index
        hal_intc_decode r0,r4

        mov     #CYGARC_SHREG_EVENT,r0
        mov.l   r4,@(r0,r8)             ! store decoded vector number back
                                        ! to saved state.
        hal_intc_translate r4,r9

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        # If we are supporting Ctrl-C interrupts from GDB, we must squirrel
        # away a pointer to the save interrupt state here so that we can
        # plant a breakpoint at some later time.
        
        mov.l   $hal_saved_interrupt_state,r1
        mov.l   r8,@r1
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
        mov.l   $hal_interrupt_handlers,r1 ! get interrupt handler
        mov.l   @(r0,r1),r1

        mov.l   $hal_interrupt_data,r5 ! get interrupt data
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

        mov.l   $hal_interrupt_objects,r0  ! get interrupt object table
        mov.l   @(r0,r9),r5             ! arg2 = interrupt object
        mov.l   $interrupt_end,r0
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
FUNC_START(hal_interrupt_stack_call_pending_DSRs)
        # Change to interrupt stack
        mov     r15,r2
        mov.l   $cyg_interrupt_stack,r15

        mov.l   r2,@-r15                ! save old stack pointer
        sts.l   pr,@-r15                ! save pr on stack
        stc     sr,r3
        mov.l   r3,@-r15                ! save sr on stack

        # enable interrupts
        hal_cpu_int_enable r0,r1

        # Call into kernel which will execute DSRs
        mov.l   $cyg_interrupt_call_pending_DSRs,r0
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
        SYM_PTR_REF(hal_saved_interrupt_state)
#endif

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 
        SYM_PTR_REF(cyg_interrupt_stack_base)
        SYM_PTR_REF(cyg_interrupt_stack)
#endif
$nCYG_SR2:
        .long   CYG_SR
$nCYG_SR_BANK1:
        .long   CYG_SR_BANK1
$nCYGARC_REG_INTEVT:
        .long   CYGARC_REG_INTEVT
#ifdef CYGARC_REG_INTEVT2
$nCYGARC_REG_INTEVT2:
        .long   CYGARC_REG_INTEVT2
#endif
        SYM_PTR_REF(hal_interrupt_handlers)
        SYM_PTR_REF(hal_interrupt_data)
        SYM_PTR_REF(hal_interrupt_objects)

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
        SYM_PTR_REF(cyg_hal_ILVL_table)
$unmasked_SR:
        .long   (CYG_SR & ~CYGARC_REG_SR_IMASK)
#endif // CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING
        
        
#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
        SYM_PTR_REF(cyg_interrupt_call_pending_DSRs)
        SYM_PTR_REF(interrupt_end)
        SYM_PTR_REF(cyg_scheduler_sched_lock)
#endif

#---------------------------------------------------------------------------
# Platform initialization (reset)
FUNC_START(_reset_platform)
        hal_post_reset_init     

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
        mov.l   $_rom_data_start,r3    ! r3 = rom start
        mov.l   $_ram_data_start,r4    ! r4 = ram start
        mov.l   $_ram_data_end,r5      ! r5 = ram end
        
        cmp/eq  r4,r5                   ! skip if no data
        bt      2f
1:      mov.l   @r3+,r0                 ! get word from ROM
        mov.l   r0,@r4                  ! store in RAM
        add     #4,r4
        cmp/eq  r4,r5                   ! compare
        bf      1b                      ! loop if not yet done
2:


        # Jump to the proper VMA base of the code.
        mov.l   $complete_setup,r0
        jmp     @r0
         nop
        .align 2

        SYM_PTR_REF(_rom_data_start)
        SYM_PTR_REF(_ram_data_start)
        SYM_PTR_REF(_ram_data_end)
        SYM_PTR_REF(complete_setup)


#elif defined(CYG_HAL_STARTUP_ROMRAM)

        # Copy everything to the proper VMA base and jump to it.
        mov.l   $_vector_code_lma,r0
        mov.l   $_reset,r1
        mov.l   $end,r2
1:      mov.l   @r0+,r3                 ! get word from ROM
        mov.l   r3,@r1                  ! store in RAM
        add     #4,r1
        cmp/eq  r1,r2                   ! compare
        bf      1b                      ! loop if not yet done
        mov.l   $complete_setup,r0
        jmp     @r0
         nop
        .align  2

        SYM_PTR_REF(_vector_code_lma)
        SYM_PTR_REF(_reset)
        SYM_PTR_REF(end)
        SYM_PTR_REF(complete_setup)

#else

        # Jump to remaining setup code. Relative branch is OK since VMA=LMA.
        bra   CYG_LABEL_DEFN(complete_setup)
         nop
                
#endif

#-----------------------------------------------------------------------------
# Complete target initialization and setup.
# After this point we can use absolute addressing modes and access all the
# memory in the system.

FUNC_START(complete_setup)
                
        # Set up monitor related stuff (vectors primarily)
        hal_mon_init

        # set up stack
        mov.l    $startup_stack,r15

        # clear BSS
        mov.l   $_bss_start,r3         ! r3 = start
        mov.l   $_bss_end,r4           ! r4 = end
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
        mov.l    $cyg_var_enable_caches,r1
        jsr      @r1
         nop

        # Variant HALs may need to do something special before we continue
        mov.l    $hal_variant_init,r1
        jsr      @r1
         nop

        # Platform initialization
        mov.l    $hal_platform_init,r1
        jsr      @r1
         nop
        
        # call c++ constructors
        mov.l    $cyg_hal_invoke_constructors,r1
        jsr      @r1
         nop

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        mov.l    $initialize_stub,r1
        jsr      @r1
         nop
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        mov.l    $hal_ctrlc_isr_init,r1
        jsr      @r1
         nop
#endif

        mov.l    $cyg_start,r1
        jsr      @r1
         nop
9:      
        bra      9b                     ! if we return, loop
         nop

        .align  2
        SYM_PTR_REF(_bss_start)
        SYM_PTR_REF(_bss_end)
        SYM_PTR_REF(startup_stack)
        SYM_PTR_REF(cyg_hal_invoke_constructors)
        SYM_PTR_REF(cyg_var_enable_caches)
        SYM_PTR_REF(hal_variant_init)
        SYM_PTR_REF(hal_platform_init)
        SYM_PTR_REF(cyg_start)

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        SYM_PTR_REF(initialize_stub)
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
        SYM_PTR_REF(hal_ctrlc_isr_init)
#endif

#---------------------------------------------------------------------------
# Interrupt vector tables.
# These tables contain the isr, data and object pointers used to deliver
# interrupts to user code.

        .data

        .extern CYG_LABEL_DEFN(hal_default_isr)
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
#define CYG_ISR_TABLE_SIZE    1
#else
#define CYG_ISR_TABLE_SIZE    CYGNUM_HAL_ISR_COUNT
#endif

SYM_DEF(hal_interrupt_handlers)
        .rept   CYG_ISR_TABLE_SIZE
        .long   CYG_LABEL_DEFN(hal_default_isr)
        .endr

SYM_DEF(hal_interrupt_data)
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

SYM_DEF(hal_interrupt_objects)
        .rept   CYG_ISR_TABLE_SIZE
        .long   0
        .endr

#---------------------------------------------------------------------------
## Temporary interrupt stack
        
        .section ".bss"

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK 
        .balign 16
SYM_DEF(cyg_interrupt_stack_base)
        .rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
        .byte 0
        .endr
        .balign 16
        .global _cyg_interrupt_stack
SYM_DEF(cyg_interrupt_stack)
        
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
SYM_DEF(startup_stack_base)
        .rept   STARTUP_STACK_SIZE
        .byte 0
        .endr
        .balign 16
SYM_DEF(startup_stack)
        .long   0,0,0,0

#---------------------------------------------------------------------------
# end of vectors.S
