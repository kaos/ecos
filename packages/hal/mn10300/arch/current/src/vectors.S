##=============================================================================
##
##	vectors.S
##
##	MN10300 exception vectors
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
# by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	nickg
## Contributors:	nickg
## Date:	1997-10-16
## Purpose:	MN10300 exception vectors
## Description:	This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##		that save and restore state for both exceptions and
##		interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL

#include <pkgconf/kernel.h>
	
#endif		

#include <cyg/hal/arch.inc>	
		
	
##-----------------------------------------------------------------------------
	
	.file	"vectors.S"

##-----------------------------------------------------------------------------
		
	.section ".vectors","ax"

	.globl	reset_vector
	
reset_vector:	
	# Reset vector
	jmp	_start

	.p2align 3

	# NMI vector
	jmp	__nmi

	.p2align 3
	
	# Trap vector
	jmp	__trap

##-----------------------------------------------------------------------------
## NMI vector

	.globl __nmi
__nmi:
	add	-4,sp				# space for SP
	hal_cpu_save_all			# push all registers
	mov	(_hal_vsr_table+28),a0 
	jmp	(a0)

##-----------------------------------------------------------------------------
## Trap vector

	.globl __trap
__trap:
	add	-8,sp				# space for SP and PSW
	hal_cpu_save_all			# push all registers
	mov	(_hal_vsr_table+32),a0
	jmp	(a0)
		
##-----------------------------------------------------------------------------
## Macro to define a hardware VSR	

	.macro	hardware_vector name
	.globl	__hardware_vector_\name
__hardware_vector_\name:
	add	-4,sp				# space for SP
	hal_cpu_save_all
	mov	\name,d0			# d0 = vector id
	mov	d0,(0,sp)			# save in dummy stack loc
	mov	_hal_vsr_table,a0		# a0 = vsr table
	add	\name<<2,a0			# a0 = &required vsr
	mov	(a0),a0				# a0 = required vsr
	jmp	(a0)				# go there
	.endm	

##-----------------------------------------------------------------------------
## Hardware Vectors. These are pointed to by the registers in
## mn10300_interrupt_vectors and must be in the range 0x4000XXXX.

	hardware_vector	0
	hardware_vector	1
	hardware_vector	2
	hardware_vector	3
	hardware_vector	4
	hardware_vector	5
	hardware_vector	6
		
##-----------------------------------------------------------------------------
## Startup code
			
	.text

	.globl	_start
_start:

	# set up stack

	mov	__interrupt_stack,a0
	sub	8,a0
	mov	a0,sp

	# Initialize hardware
	hal_cpu_init
	hal_mmu_init
	hal_memc_init
	hal_diag_init
	hal_intc_init
	hal_cache_init
	hal_timer_init
	hal_mon_init

#ifdef CYG_HAL_STARTUP_ROM
	
	# Copy data from ROM to RAM

	mov	__rom_data_start,a0
	mov	__ram_data_end,a1
	mov	__ram_data_start,a2
	
	cmp	a2,a1
	beq	8f
1:	
	movbu	(a0),d0
	movbu	d0,(a2)
	inc	a0
	inc	a2
	cmp	a2,a1
	bne	1b	
8:	
	
#endif
	
	# Clear BSS
	mov	__bss_start,a0
	mov	__bss_end,a1

	cmp	a0,a1
	beq	8f
	clr	d0
1:	
	movbu	d0,(a0)
	inc	a0
	cmp	a0,a1
	bne	1b	
8:	

	# Call variant and platform HAL
	# initialization routines.

	.extern _hal_variant_init
	call    _hal_variant_init,[],0

	.extern _hal_platform_init
	call	_hal_platform_init,[],0
	
	# Call constructors

	.extern _cyg_hal_invoke_constructors
	call    _cyg_hal_invoke_constructors,[],0

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        .extern _initialize_stub
        call    _initialize_stub,[],0
#endif

       	# Call cyg_start
	clr	d0
	mov	d0,(4,sp)
	
	.extern _cyg_start
	call    _cyg_start,[],0

9:
	bra	9b		# Loop if we return

##-----------------------------------------------------------------------------
## The following macros are defined depending on whether the HAL is configured
## to support the kernel or not. 

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	.extern	cyg_scheduler_sched_lock
	
	# Increment the scheduler lock
			
	.macro increment_sched_lock reg=d0
	mov	(cyg_scheduler_sched_lock),\reg
	inc	\reg
	mov	\reg,(cyg_scheduler_sched_lock)
	.endm

#else
	
	.macro increment_sched_lock reg=d0
	.endm

#endif		

##-----------------------------------------------------------------------------
## Default interrupt VSR

	.text
	.globl	__default_interrupt_vsr
__default_interrupt_vsr:

	# We come here with all the registers pushed
	# onto the stack.

	hal_diag_intr_start
	
	increment_sched_lock
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	# Increment interrupt nesting counter
	mov	__interrupt_stack,a0		# A0 = interrupt stack top
	mov	sp,a2				# A2 = SP
	cmp	__interrupt_stack_base,a2	# compare with base of stack
	blt	1f				# if lt switch to int stack
	cmp	a0,a2				# compare sp with stack top
	ble	8f				# if le already on istack
1:
	mov	a0,sp				# switch to new SP
8:
	movm	[a2],(sp)			# save old SP

#else
	mov	sp,a2				# A2 = saved thread state
#endif

	# Here A2 -> saved thread state on the threads own
	# stack. We will be executing either on the same stack
	# or on the interrupt stack, depending on config options.
		
	hal_intc_decode d3,a3,d0

	# Here D3 contains the table byte offset of the vector to
	# call.

#if defined(CYGPKG_KERNEL_INSTRUMENT) && defined(CYGDBG_KERNEL_INSTRUMENT_INTR)

	# Call cyg_instrument to record that this interrupt is being raised.

	.extern _cyg_instrument		
	add	-16,sp				# make space for return link + args	
	mov	0x0301,d0			# d0 = type = INTR,RAISE
	mov	(0,a2),d1			# d1 = arg1 = vector
	mov	d3,(12,sp)			# (12,sp) = arg2 = table offset
	calls	_cyg_instrument			# call instrumentation
	add	16,sp				# pop space
	
#endif		

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        .extern _cyg_hal_gdb_isr
        add     -16,sp                          # make space for return link + args
        mov     (0,a2),d0                       # d0 = vector
        mov     (56,a2),d1                      # d1 = pc
        calls   _cyg_hal_gdb_isr                # call GDB isr function
        cmp     0x00,d0                         # Call ISR proper?
        beq     2f                              # (d0 is 0 when skipping
                                                #  to avoid DSR call)
        add     16,sp                           # pop space
#endif
	
#ifdef CYGSEM_HAL_COMMON_INTERRUPTS_ALLOW_NESTING

	# To allow nested interrupts, we set the IE bit. We do
	# not touch the IPL bits, so only higher priority interrupts
	# will be nested on top of us. Also, new interrupts will not
	# be delivered until the ISR calls 
	# Cyg_Interrupt::acknowledge_interrupt(). At some future point
	# we may want to do the ack stuff here to allow immediate nesting.
	
	or	0x0800,psw
	
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)

	# If we have Ctrl-C support enabled, save a pointer to the
	# saved CPU state here so we can plant a breakpoint there if
	# this is a ^C.
	
	.extern _hal_saved_interrupt_state
	mov	a2,(_hal_saved_interrupt_state)
	
#endif
		
	mov	_hal_interrupt_handlers,a0	# a0 = isr table
	mov	(d3,a0),a0			# a0 = isr
	
	mov	_hal_interrupt_data,a1		# a1 = data table
	mov	(d3,a1),d1			# d1 = isr data

	mov	(0,a2),d0			# d0 = vector. (d3 is...
						# ...adjusted for table reads)

	add	-16,sp				# make space for return link

	calls	(a0)				# call isr

	# on return d0 bit 1 will indicate whether a DSR is
	# to be posted. Pass this together with a pointer to
	# the interrupt object we have just used to the
	# interrupt tidy up routine.
	
	# D3 is defined to be saved across procedure calls, and
	# should still contain the vector byte index. Similarly,
	# A2 should still point to the saved machine state.

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
        # If interrupt was caused by GDB, the ISR call above
        # is skipped by jumping here.
2:
#endif

#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK

	# If we are returning from the last nested interrupt, move back
	# to the thread stack. interrupt_end() must be called on the
	# thread stack since it potentially causes a context switch.
	
	add	16,sp				# pop call frame from int stack
	movm	(sp),[a3]			# pop old sp
	mov	a3,sp				# put in SP

	add	-16,sp				# make call frame for call

#endif		

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	
	# We only need to call _interrupt_end() when there is a kernel
	# present to do any tidying up.
		
	# Using the vector offset in D3, get the interrupt object pointer
	# into D1.
	mov	_hal_interrupt_objects,a0	# a0 = object table
	mov	(d3,a0),d1			# d1 = object

	# Even when this is not the last nested interrupt, we must call
	# _interrupt_end() to post the DSR and decrement the scheduler
	# lock.
		
	mov	a2,(12,sp)			# arg3 = saved state.
	
	calls	_interrupt_end			# call interrupt end fn		
#endif

	add	16,sp				# pop return link

#	show_interrupts

	hal_diag_restore
	
	hal_cpu_load_all
	add	4,sp
	rti					# and return

##-----------------------------------------------------------------------------
## Execute pending DSRs on the interrupt stack with interrupts enabled.
## Note: this can only be called from code running on a thread stack
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_USE_INTERRUPT_STACK
	.extern _cyg_interrupt_call_pending_DSRs
	.global _hal_interrupt_stack_call_pending_DSRs

_hal_interrupt_stack_call_pending_DSRs:		
	movm	[d2,a2],(sp)			# save some work regs
	mov	__interrupt_stack,a0		# interrupt stack pointer
	mov	sp,a2				# save current SP
	mov	a0,sp				# set SP to interrupt stack
	hal_cpu_get_psw d2			# save PSW
	hal_cpu_int_enable			# enable interrupts

	# Call kernel code to invoke DSRs.
	calls	_cyg_interrupt_call_pending_DSRs	

	# On return the old SP in a2 and the old PSW in d2 will 
	# have been preserved by the calling conventions.
	
	hal_cpu_int_merge d2			# Restore previous PSW
	mov	a2,sp				# restore old SP
	movm	(sp),[d2,a2]			# Retrieve old work regs
	ret	[],0				# and return
		
#endif		
		
##-----------------------------------------------------------------------------
## Default NMI VSR

	.globl	_cyg_hal_exception_handler
	
	.text
	.globl	__default_nmi_vsr
__default_nmi_vsr:

	# We come here with all the registers saved
	# on the stack.

	hal_diag_excpt_start
	
	# Decode the cause of the NMI and cancel all the bits in all
	# the registers. We need to clear any bits set in the ISR and
	# then clear any bits set in the NMICR. Note that we can only 
	# access the ISR if the DCR:DE bit is set. 
	
	movhu	(NMICR),d0			# D0 = NMI Control register

	movhu	(DCR),d1			# D1 = Debug Control Register
	mov	d1,d2				# D2 = copy of DCR
	movhu	(ISR),d3			# D3 = Interrupt Status Register

	or	DCR_DE,d1			# Set DE bit	
	movhu	d1,(DCR)			

	movhu	d3,(ISR)			# clear ISR bits

	movhu	d2,(DCR)			# restore DCR to original value

	movhu	d0,(NMICR)			# clear NMI bits
	and	0x7,d0				# LS 3 bits only

	mov	hal_lsbit_table,a0		
	movbu	(d0,a0),d1			# D1 = NMI code
						## 0 = NMI
						## 1 = Watchdog
						## 2 = System Error

	add	9,d1				# offset into exception number space
	
	hal_cpu_clear_nmid

	mov	sp,a1				# a1 = saved state
	mov	a1,d0				# d0 = arg1 = saved state

	add	-16,sp				# return link + args
	mov	d1,(16,sp)			# save initial vector in spare save slot
	mov	d3,d1				# d1 = arg2 = ISR value

	calls	_cyg_hal_exception_handler	# call C code

	add	16,sp				# pop args

	hal_cpu_load_all

	add	4,sp
	rti

	.globl hal_lsbit_table
hal_lsbit_table:
	.byte	0, 0, 1, 0
	.byte	2, 0, 1, 0
	.byte	3, 0, 1, 0
	.byte	2, 0, 1, 0
	
##-----------------------------------------------------------------------------
## Default TRAP VSR

	.text
	.globl	__default_trap_vsr
__default_trap_vsr:

	# We come here with all the registers saved
	# on the stack.

	add	-8,sp				# return link + arg
	mov	3,d1				# 3 == TRAP trap
	mov	d1,(0,sp)			# save in spare save slot
	calls	_cyg_hal_exception_handler	# call C code
	add	8,sp				# pop args
	hal_cpu_load_all
	add	4,sp
	rets

##-----------------------------------------------------------------------------
## VSR table. The VSRs pointed to by this table are called from the stubs
## connected to the hardware.

#ifndef CYG_HAL_MN10300_VSR_TABLE_DEFINED
	
	.data

	.globl	_hal_vsr_table
_hal_vsr_table:	
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_interrupt_vsr
	.long	__default_nmi_vsr
	.long	__default_trap_vsr

	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
	.long	__default_nmi_vsr
#endif
	
##-----------------------------------------------------------------------------
## Interrupt tables
	
	.data

	.extern _hal_default_isr
	
	.globl	_hal_interrupt_handlers
_hal_interrupt_handlers:
	.rept	CYG_ISR_TABLE_SIZE
	.long	_hal_default_isr
	.endr
	
	.globl	_hal_interrupt_data
_hal_interrupt_data:
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
	.globl	_hal_interrupt_objects
_hal_interrupt_objects:
	.rept	CYG_ISR_TABLE_SIZE
	.long	0
	.endr
	
			
##-----------------------------------------------------------------------------
## Temporary interrupt stack
	
	.section ".bss"

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
	
	.long	0,0,0,0,0,0,0,0	

##-----------------------------------------------------------------------------

	.data
	hal_diag_data

##-----------------------------------------------------------------------------
## end of vectors.S

