##=============================================================================
##
##	vectors.S
##
##	H8/300 exception vectors
##
##=============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
## Author(s): 	Yoshinori Sato
## Contributors: Yoshinori Sato
## Date:	2002-02-14
## Purpose:	H8/300 exception vectors
## Description:	This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##		that save and restore state for both exceptions and
##		interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#endif
	
#ifdef CYGPKG_KERNEL

#include <pkgconf/kernel.h>
	
#endif		

#include <cyg/hal/arch.inc>	
#include <cyg/hal/basetype.h>	
#include <cyg/hal/variant.inc>
			
	
##-----------------------------------------------------------------------------
	
#ifdef CYGPKG_HAL_H8300_H8300H
	.h8300h
	.macro	shll2 reg
	shll.l	\reg
	shll.l	\reg
	.endm
	.macro	shlr2 reg
	shlr.l	\reg
	shlr.l	\reg
	.endm
#endif
#ifdef CYGPKG_HAL_H8300_H8S
	.h8300s
	.macro	shll2 reg
	shll.l	#2,\reg
	.endm
	.macro	shlr2 reg
	shlr.l	#2,\reg
	.endm
#endif
	
#ifdef CYGPKG_CYGMON
##-----------------------------------------------------------------------------
## Macros for Stack handling when running Cygmon
        		
.macro hal_cygmon_switch_app_stack
	#
	; Switch to interrupt stack to handle exception
	#

	; First, save some scratch registers
	mov.l	er0,@er0_save
	mov.l	er1,@er1_save

	; Copy the exception frame
	mov.l	#__cygmon_interrupt_stack,er0
	mov.l	@sp,er1
	mov.l	er1,@-er0

	; Save the pre-exception sp in the register image	 
	mov.l	sp,@-er0

	; Actually switch the stack
	mov.l	er0,sp

	; Now, restore the scratch registers
	mov.l	@er0_save,er0
	mov.l	@er1_save,er1
.endm

.macro hal_cygmon_restore_app_stack
	// For cygmon we are switching stacks immediately on exception.
	// We must wait until the very end before restoring the original stack.

	; Save some scratch registers
	mov.l	er0,@er0_save
	mov.l	er1,@er1_save

	; We need to restore the application stack pointer, but we also
	; need to restore the exception frame.
	mov.l	@sp+,er0
	mov.l	@sp+,er1
	mov.l	er0,@er1
	mov.l	er1,sp				// Restore the frame-adjusted SP

	; Restore the scratch registers
	mov.l	@er0_save,er0
	mov.l	@er1_save,er1
.endm

#endif // CYGPKG_CYGMON

##-----------------------------------------------------------------------------
		
#if (defined(CYG_HAL_STARTUP_ROM) || \
     defined(CYGPKG_HAL_H8300_H8300H_SIM) || \
     defined(CYGPKG_HAL_H8300_H8S_SIM))
    
	.section .vectors,"a"
	
	.globl	reset_vector

reset_vector:
	; Reset vector
	.long	CYG_LABEL_DEFN(_start)
	.long	CYG_LABEL_DEFN(_start)
	.space	3*4
	.long	__interrupt5
	.long	__interrupt6
	.long	__interrupt7
	.long	__interrupt8
	.long	__interrupt9
	.long	__interrupt10
	.long	__interrupt11
	.long	__interrupt12
	.long	__interrupt13
	.long	__interrupt14
	.long	__interrupt15
	.long	__interrupt16
	.long	__interrupt17
	.long	__interrupt18
	.long	__interrupt19
	.long	__interrupt20
	.long	__interrupt21
	.long	__interrupt22
	.long	__interrupt23
	.long	__interrupt24
	.long	__interrupt25
	.long	__interrupt26
	.long	__interrupt27
	.long	__interrupt28
	.long	__interrupt29
	.long	__interrupt30
	.long	__interrupt31
	.long	__interrupt32
	.long	__interrupt33
	.long	__interrupt34
	.long	__interrupt35
	.long	__interrupt36
	.long	__interrupt37
	.long	__interrupt38
	.long	__interrupt39
	.long	__interrupt40
	.long	__interrupt41
	.long	__interrupt42
	.long	__interrupt43
	.long	__interrupt44
	.long	__interrupt45
	.long	__interrupt46
	.long	__interrupt47
	.long	__interrupt48
	.long	__interrupt49
	.long	__interrupt50
	.long	__interrupt51
	.long	__interrupt52
	.long	__interrupt53
	.long	__interrupt54
	.long	__interrupt55
	.long	__interrupt56
	.long	__interrupt57
	.long	__interrupt58
	.long	__interrupt59
	.long	__interrupt60
	.long	__interrupt61
	.long	__interrupt62
	.long	__interrupt63

#ifdef CYGPKG_HAL_H8300_H8S
	.long	__interrupt64
	.long	__interrupt65
	.long	__interrupt66
	.long	__interrupt67
	.long	__interrupt68
	.long	__interrupt69
	.long	__interrupt70
	.long	__interrupt71
	.long	__interrupt72
	.long	__interrupt73
	.long	__interrupt74
	.long	__interrupt75
	.long	__interrupt76
	.long	__interrupt77
	.long	__interrupt78
	.long	__interrupt79
	.long	__interrupt80
	.long	__interrupt81
	.long	__interrupt82
	.long	__interrupt83
	.long	__interrupt84
	.long	__interrupt85
	.long	__interrupt86
	.long	__interrupt87
	.long	__interrupt88
	.long	__interrupt89
	.long	__interrupt90
	.long	__interrupt91
	.long	__interrupt92
	.long	__interrupt93
	.long	__interrupt94
	.long	__interrupt95
	.long	__interrupt96
	.long	__interrupt97
	.long	__interrupt98
	.long	__interrupt99
	.long	__interrupt100
	.long	__interrupt101
	.long	__interrupt102
	.long	__interrupt103
	.long	__interrupt104
	.long	__interrupt105
	.long	__interrupt106
	.long	__interrupt107
	.long	__interrupt108
	.long	__interrupt109
	.long	__interrupt110
	.long	__interrupt111
	.long	__interrupt112
	.long	__interrupt113
	.long	__interrupt114
	.long	__interrupt115
	.long	__interrupt116
	.long	__interrupt117
	.long	__interrupt118
	.long	__interrupt119
	.long	__interrupt120
	.long	__interrupt121
	.long	__interrupt122
	.long	__interrupt123
	.long	__interrupt124
	.long	__interrupt125
	.long	__interrupt126
	.long	__interrupt127
#endif
#endif

#define INTERRUPT_ENTRY(no) \
__interrupt##no:	    \
	jsr	@interrupt_entry

	.section .int_hook_table,"ax"
	.space	5*4	
__interrupt_table:
INTERRUPT_ENTRY(5)
INTERRUPT_ENTRY(6)
INTERRUPT_ENTRY(7)
INTERRUPT_ENTRY(8)
INTERRUPT_ENTRY(9)
INTERRUPT_ENTRY(10)
INTERRUPT_ENTRY(11)
INTERRUPT_ENTRY(12)
INTERRUPT_ENTRY(13)
INTERRUPT_ENTRY(14)
INTERRUPT_ENTRY(15)
INTERRUPT_ENTRY(16)
INTERRUPT_ENTRY(17)
INTERRUPT_ENTRY(18)
INTERRUPT_ENTRY(19)
INTERRUPT_ENTRY(20)
INTERRUPT_ENTRY(21)
INTERRUPT_ENTRY(22)
INTERRUPT_ENTRY(23)
INTERRUPT_ENTRY(24)
INTERRUPT_ENTRY(25)
INTERRUPT_ENTRY(26)
INTERRUPT_ENTRY(27)
INTERRUPT_ENTRY(28)
INTERRUPT_ENTRY(29)
INTERRUPT_ENTRY(30)
INTERRUPT_ENTRY(31)
INTERRUPT_ENTRY(32)
INTERRUPT_ENTRY(33)
INTERRUPT_ENTRY(34)
INTERRUPT_ENTRY(35)
INTERRUPT_ENTRY(36)
INTERRUPT_ENTRY(37)
INTERRUPT_ENTRY(38)
INTERRUPT_ENTRY(39)
INTERRUPT_ENTRY(40)
INTERRUPT_ENTRY(41)
INTERRUPT_ENTRY(42)
INTERRUPT_ENTRY(43)
INTERRUPT_ENTRY(44)
INTERRUPT_ENTRY(45)
INTERRUPT_ENTRY(46)
INTERRUPT_ENTRY(47)
INTERRUPT_ENTRY(48)
INTERRUPT_ENTRY(49)
INTERRUPT_ENTRY(50)
INTERRUPT_ENTRY(51)
INTERRUPT_ENTRY(52)
INTERRUPT_ENTRY(53)
INTERRUPT_ENTRY(54)
INTERRUPT_ENTRY(55)
INTERRUPT_ENTRY(56)
INTERRUPT_ENTRY(57)
INTERRUPT_ENTRY(58)
INTERRUPT_ENTRY(59)
INTERRUPT_ENTRY(60)
INTERRUPT_ENTRY(61)
INTERRUPT_ENTRY(62)
INTERRUPT_ENTRY(63)
#ifdef CYGPKG_HAL_H8300_H8S
INTERRUPT_ENTRY(64)
INTERRUPT_ENTRY(65)
INTERRUPT_ENTRY(66)
INTERRUPT_ENTRY(67)
INTERRUPT_ENTRY(68)
INTERRUPT_ENTRY(69)
INTERRUPT_ENTRY(70)
INTERRUPT_ENTRY(71)
INTERRUPT_ENTRY(72)
INTERRUPT_ENTRY(73)
INTERRUPT_ENTRY(74)
INTERRUPT_ENTRY(75)
INTERRUPT_ENTRY(76)
INTERRUPT_ENTRY(77)
INTERRUPT_ENTRY(78)
INTERRUPT_ENTRY(79)
INTERRUPT_ENTRY(80)
INTERRUPT_ENTRY(81)
INTERRUPT_ENTRY(82)
INTERRUPT_ENTRY(83)
INTERRUPT_ENTRY(84)
INTERRUPT_ENTRY(85)
INTERRUPT_ENTRY(86)
INTERRUPT_ENTRY(87)
INTERRUPT_ENTRY(88)
INTERRUPT_ENTRY(89)
INTERRUPT_ENTRY(90)
INTERRUPT_ENTRY(91)
INTERRUPT_ENTRY(92)
INTERRUPT_ENTRY(93)
INTERRUPT_ENTRY(94)
INTERRUPT_ENTRY(95)
INTERRUPT_ENTRY(96)
INTERRUPT_ENTRY(97)
INTERRUPT_ENTRY(98)
INTERRUPT_ENTRY(99)
INTERRUPT_ENTRY(100)
INTERRUPT_ENTRY(101)
INTERRUPT_ENTRY(102)
INTERRUPT_ENTRY(103)
INTERRUPT_ENTRY(104)
INTERRUPT_ENTRY(105)
INTERRUPT_ENTRY(106)
INTERRUPT_ENTRY(107)
INTERRUPT_ENTRY(108)
INTERRUPT_ENTRY(109)
INTERRUPT_ENTRY(110)
INTERRUPT_ENTRY(111)
INTERRUPT_ENTRY(112)
INTERRUPT_ENTRY(113)
INTERRUPT_ENTRY(114)
INTERRUPT_ENTRY(115)
INTERRUPT_ENTRY(116)
INTERRUPT_ENTRY(117)
INTERRUPT_ENTRY(118)
INTERRUPT_ENTRY(119)
INTERRUPT_ENTRY(120)
INTERRUPT_ENTRY(121)
INTERRUPT_ENTRY(122)
INTERRUPT_ENTRY(123)
INTERRUPT_ENTRY(124)
INTERRUPT_ENTRY(125)
INTERRUPT_ENTRY(126)
INTERRUPT_ENTRY(127)
#endif
			
##-----------------------------------------------------------------------------
## Startup code
			
	.text

	.globl	CYG_LABEL_DEFN(_start)
CYG_LABEL_DEFN(_start):
	; set up stack

	mov.l	#__interrupt_stack,sp
	
	; Initialize hardware
	hal_cpu_init
	hal_mmu_init
	hal_memc_init
	hal_diag_init
	hal_intc_init
	hal_cache_init
	hal_timer_init
	hal_mon_init

#ifdef CYG_HAL_STARTUP_ROM
	
	; Copy data from ROM to RAM

	mov.l	#CYG_LABEL_DEFN(__rom_data_start),er5
	mov.l	#CYG_LABEL_DEFN(__ram_data_start),er6
	mov.l	#CYG_LABEL_DEFN(__ram_data_end),er4
	sub.l	er6,er4
1:	
	eepmov.w
	dec.w	#1,e4
	cmp.w	#-1,e4
	bhi	1b

#endif

#if defined(CYGSEM_HAL_H8300_VECTOR_HOOK)
	; Setup Interrupt Vector (virtual)
	mov.l	#CYG_LABEL_DEFN(__rom_int_hook_table),er5
	mov.l	#CYG_LABEL_DEFN(__int_hook_start),er6
	mov.l	#CYG_LABEL_DEFN(__int_hook_end),er4
	sub.l	er6,er4
1:	
	eepmov.w
	dec.w	#1,e4
	cmp.w	#-1,e4
	bhi	1b
#endif
	
	; Clear BSS
	mov.l	#CYG_LABEL_DEFN(__bss_start),er5
	mov.l	er5,er6
	adds	#1,er6
	mov.l	#CYG_LABEL_DEFN(__bss_end),er4
	sub.l	#CYG_LABEL_DEFN(__bss_start),er4

	mov.b	#0,r0l
	mov.b	r0l,@er5
1:	
	eepmov.w
	dec.w	#1,e4
	cmp.w	#-1,e4
	bhi	1b

	; Call variant and platform HAL
	; initialization routines.

	.extern CYG_LABEL_DEFN(hal_variant_init)
	jsr	@CYG_LABEL_DEFN(hal_variant_init)

	.extern CYG_LABEL_DEFN(hal_platform_init)
	jsr	@CYG_LABEL_DEFN(hal_platform_init)
	
	; Call constructors

	.extern CYG_LABEL_DEFN(cyg_hal_invoke_constructors)
	jsr	@CYG_LABEL_DEFN(cyg_hal_invoke_constructors)

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern CYG_LABEL_DEFN(initialize_stub)
        jsr	@CYG_LABEL_DEFN(initialize_stub)
#endif

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) \
    || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
	.extern CYG_LABEL_DEFN(hal_ctrlc_isr_init)
	jsr	@CYG_LABEL_DEFN(hal_ctrlc_isr_init)
#endif


       	; Call cyg_start
	sub.l	er0,er0
		
	.extern CYG_LABEL_DEFN(cyg_start)
	jsr	@CYG_LABEL_DEFN(cyg_start)

9:
	bra	9b		; Loop if we return

interrupt_entry:
	mov.l	er0,@-sp
	mov.l	@(4,sp),er0
	sub.l	#__interrupt_table,er0
	shlr.l	er0
	shlr.l	er0
	add.w	#5-1,r0
	mov.l	er0,@CYG_LABEL_DEFN(_intvector)
	mov.l	@sp+,er0
	adds	#4,sp
#ifdef CYGPKG_CYGMON
	hal_cygmon_switch_app_stack
#endif
	hal_cpu_save_all
	h8300_var_interrupt_entry		; load isr address
	jmp	@er0

##-----------------------------------------------------------------------------
## The following macros are defined depending on whether the HAL is configured
## to support the kernel or not. 

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	.extern	CYG_LABEL_DEFN(cyg_scheduler_sched_lock)
	
	; Increment the scheduler lock
			
	.macro increment_sched_lock reg=er0
	mov.l	@CYG_LABEL_DEFN(cyg_scheduler_sched_lock),\reg
	inc.l	#1,\reg
	mov.l	\reg,@CYG_LABEL_DEFN(cyg_scheduler_sched_lock)
	.endm

#else
	
	.macro increment_sched_lock reg=er0
	.endm

#endif		

##-----------------------------------------------------------------------------
## Default interrupt VSR

	.text
	.globl	CYG_LABEL_DEFN(__default_interrupt_vsr)
CYG_LABEL_DEFN(__default_interrupt_vsr):

	; We come here with all the registers pushed
	; onto the stack.

	hal_diag_intr_start
	
	increment_sched_lock

#if defined(CYGPKG_CYGMON)
        // For Cygmon, we saved this back when we originally switched stacks.
	mov.l	sp,er1				; A2 = saved thread state
#elif defined(CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK)
	; Increment interrupt nesting counter
	mov.l	sp,er1
	mov.l	#__interrupt_stack,er0		; A0 = interrupt stack top
	cmp.l	#__interrupt_stack_base,sp	; compare with base of stack
	blt	1f				; if lt switch to int stack
	cmp.l	er0,sp				; compare sp with stack top
	ble	8f				; if le already on istack
1:
	mov.l	er0,sp				; switch to new SP
8:
	mov.l	er1,@-sp			; save old SP
	
#else
	mov	sp,er1				; A2 = saved thread state
#endif
	mov.l	er1,er4

	; Here D3 contains the table byte offset of the vector to
	; call.

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

	; Call cyg_instrument to record that this interrupt is being raised.

	.extern CYG_LABEL_DEFN(cyg_instrument)
	mov.l	#0x0301,er0			; type = INTR,RAISE
	mov.l	@CYG_LABEL_DEFN(_intvector),er2		; arg2 = table offset
	jsr	CYG_LABEL_DEFN(cyg_instrument)	; call instrumentation
	
#endif		

#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

	; To allow nested interrupts, we set the IE bit. We do
	; not touch the IPL bits, so only higher priority interrupts
	; will be nested on top of us. Also, new interrupts will not
	; be delivered until the ISR calls 
	; Cyg_Interrupt::acknowledge_interrupt(). At some future point
	; we may want to do the ack stuff here to allow immediate nesting.
	
	mov.l	@CYG_LABEL_DEFN(_intvector),er0
	mov.b	@(CYG_LABEL_DEFN(cyg_hal_level_table),er0),r0l
	hal_cpu_set_int_level
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) || \
    defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)

	; If we have Ctrl-C support enabled, save a pointer to the
	; saved CPU state here so we can plant a breakpoint there if
	; this is a ^C.
	
	.extern CYG_LABEL_DEFN(hal_saved_interrupt_state)
	mov.l	er4,@CYG_LABEL_DEFN(hal_saved_interrupt_state)
	
#endif
	mov.l	@CYG_LABEL_DEFN(_intvector),er0
	shll2	er0
	mov.l	@(CYG_LABEL_DEFN(hal_interrupt_handlers),er0),er2
	
	mov.l	@(CYG_LABEL_DEFN(hal_interrupt_data),er0),er1

	shlr2	er0

	jsr	@er2

	; on return d0 bit 1 will indicate whether a DSR is
	; to be posted. Pass this together with a pointer to
	; the interrupt object we have just used to the
	; interrupt tidy up routine.
	
	; D3 is defined to be saved across procedure calls, and
	; should still contain the vector byte index. Similarly,
	; A2 should still point to the saved machine state.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        ; If interrupt was caused by GDB, the ISR call above
        ; is skipped by jumping here.
2:
#endif

#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK) && !defined(CYGPKG_CYGMON)

	; If we are returning from the last nested interrupt, move back
	; to the thread stack. interrupt_end() must be called on the
	; thread stack since it potentially causes a context switch.
	
	mov.l	@sp+,sp				; pop old sp

#endif		

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	; We only need to call _interrupt_end() when there is a kernel
	; present to do any tidying up.
		
	; Using the vector offset in D3, get the interrupt object pointer
	; into D1.
	mov.l	@CYG_LABEL_DEFN(_intvector),er1
	shll2	er1
	mov.l	@(CYG_LABEL_DEFN(hal_interrupt_objects),er1),er1

	; Even when this is not the last nested interrupt, we must call
	; _interrupt_end() to post the DSR and decrement the scheduler
	; lock.
		
	mov.l	er3,er2				; arg3 = saved state.
	
	jsr	@CYG_LABEL_DEFN(interrupt_end)	; call interrupt end fn		
#endif


#	show_interrupts

	hal_diag_restore

	hal_cpu_load_all
#ifdef CYGPKG_CYGMON
	hal_cygmon_restore_app_stack
#endif
        mov.l   er0,@-sp
        mov.l   @(4:16,sp),er0
        mov.l   er0,@__lastpc
        mov.l   @sp+,er0
        
	rte					; and return

	.section	.bss
	.global	CYG_LABEL_DEFN(_intvector)
CYG_LABEL_DEFN(_intvector):
	.long	0
        .global __lastpc        
__lastpc:
        .long   0
	.text	
##-----------------------------------------------------------------------------
## Execute pending DSRs on the interrupt stack with interrupts enabled.
## Note: this can only be called from code running on a thread stack
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	.extern CYG_LABEL_DEFN(cyg_interrupt_call_pending_DSRs)
	.global CYG_LABEL_DEFN(hal_interrupt_stack_call_pending_DSRs)

CYG_LABEL_DEFN(hal_interrupt_stack_call_pending_DSRs):
	h8300_var_call_pending_DSR
#endif		
		
#ifdef CYGPKG_CYGMON
        .section	.bss
er0_save:
	.long	0
er1_save:
	.long	0
#endif

##-----------------------------------------------------------------------------
## Default TRAP VSR

	.text
	.globl	CYG_LABEL_DEFN(__default_trap_vsr)
CYG_LABEL_DEFN(__default_trap_vsr):

#ifdef CYG_HAL_DIAG_EXCPT_END
	hal_diag_excpt_start
#endif
	mov.l	@(32,sp),er0
	subs	#2,er0
	mov.l	er0,@(32,sp)
	mov.l	sp,er0
	mov.l	@CYG_LABEL_DEFN(_intvector),er1	

	jsr	@CYG_LABEL_DEFN(cyg_hal_exception_handler)

#ifdef CYG_HAL_DIAG_EXCPT_END
	hal_diag_excpt_end
#endif
	hal_cpu_load_all
#ifdef CYGPKG_CYGMON
	hal_cygmon_restore_app_stack
#endif
	rte

##-----------------------------------------------------------------------------
## VSR table. The VSRs pointed to by this table are called from the stubs
## connected to the hardware.

#ifndef CYG_HAL_H8300_VSR_TABLE_DEFINED
	
	.data

	.globl	CYG_LABEL_DEFN(hal_vsr_table)
CYG_LABEL_DEFN(hal_vsr_table):	
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	0
	.long	0
	.long	0
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)
	.long	CYG_LABEL_DEFN(__default_trap_vsr)

	.rept	CYG_ISR_TABLE_SIZE-12
	.long	CYG_LABEL_DEFN(__default_interrupt_vsr)
	.endr
#endif
	
##-----------------------------------------------------------------------------
## Interrupt tables
	
	.data

	.extern CYG_LABEL_DEFN(hal_default_isr)
	
	.globl	CYG_LABEL_DEFN(hal_interrupt_handlers)
CYG_LABEL_DEFN(hal_interrupt_handlers):
	.rept	CYG_ISR_TABLE_SIZE
	.long	CYG_LABEL_DEFN(hal_default_isr)
	.endr
	
	.globl	CYG_LABEL_DEFN(hal_interrupt_data)
CYG_LABEL_DEFN(hal_interrupt_data):
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
	.globl	CYG_LABEL_DEFN(hal_interrupt_objects)
CYG_LABEL_DEFN(hal_interrupt_objects):
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
			
##-----------------------------------------------------------------------------
## Temporary interrupt stack
	
	.section	.bss

	.balign 2
	.global _cyg_interrupt_stack_base
_cyg_interrupt_stack_base:
__interrupt_stack_base:
	.rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
	.byte 0
	.endr
	.balign 2
	.global _cyg_interrupt_stack
_cyg_interrupt_stack:
__interrupt_stack:
	
#ifdef CYGPKG_CYGMON
	.balign 2
	.global __cygmon_interrupt_stack_base
__cygmon_interrupt_stack_base:
	.rept CYGNUM_HAL_COMMON_INTERRUPTS_STACK_SIZE
	.byte 0
	.endr
	.balign 2
	.global __cygmon_interrupt_stack
__cygmon_interrupt_stack:
#endif

	.long	0,0,0,0,0,0,0,0	

##-----------------------------------------------------------------------------

	.data
	hal_diag_data


##-----------------------------------------------------------------------------
## end of vectors.S

