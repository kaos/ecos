#ifndef CYGONCE_HAL_ARCH_INC
#define CYGONCE_HAL_ARCH_INC
##=============================================================================
##
##	arch.inc
##
##	i386 assembler header file
##
##=============================================================================
######COPYRIGHTBEGIN####
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
######COPYRIGHTEND####
##=============================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s): 	 nickg
## Contributors: nickg, pjo
## Date:	1999-10-15
## Purpose:	Architecture definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the i386 CPU family.
## Usage:
##		#include <cyg/hal/arch.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/i386.inc>

#include <cyg/hal/variant.inc>

##-----------------------------------------------------------------------------
## CPU specific macros. These provide a common assembler interface to
## operations that may have CPU specific implementations on different
## variants of the architecture.		
	
#ifndef CYGPKG_HAL_I386_CPU_INIT_DEFINED
	# Initialize CPU
	.macro	hal_cpu_init
	.endm
#endif /* !CYGPKG_HAL_I386_CPU_INIT_DEFINED */

##-----------------------------------------------------------------------------

#ifndef CYGPKG_HAL_I386_INTC_DEFINED

#ifndef CYGPKG_HAL_I386_INTC_INIT_DEFINED
	# initialize all interrupts to disabled
	.macro	hal_intc_init
	.endm
#endif

	.macro	hal_intc_decode vnum
	.endm

#endif


#------------------------------------------------------------------------------
# FPU macros.
	
#ifndef CYGPKG_HAL_I386_FPU_DEFINED

#ifdef CYGHWR_HAL_I386_FPU

#define CYGPKG_HAL_I386_FPU_DEFINED

	.macro	hal_fpu_init
	# Tell the CPU to use the math hardware.
	movl	%cr0, %eax
	orl	$0x32, %eax
	movl	%eax, %cr0

	finit			# and initialize...

        ## Enable floating point exceptions. Bit mask:
        ##  1 - invalid operation
        ##  2 - denormalized operand
        ##  4 - zero divide
        ##  8 - overflow
        ## 16 - underflow
        ## 32 - precision
	pushl	$0		# space for CW
        fstcw   0(%esp)		# store FPCW to stack
        movl	0(%esp),%eax	# get into EAX
        andb	$(~0x04),%al	# allow only zero divide exceptions
        movl	%eax,0(%esp)	# put back into memory
        fldcw	0(%esp)		# reload
	addl	$4,%esp		# pop value

#ifdef CYGHWR_HAL_I386_FPU_SWITCH_LAZY
	# Tell the CPU to generate an FPU unavailable exception
	# when the FPU is first used.
	movl	%cr0, %eax
	orl	$0x8, %eax
	movl	%eax, %cr0
	# Plant a pointer to the FPU switch VSR into slot 7
	# of the VSR table.
	movl	$__fpu_switch_vsr,%eax
	movl	%eax,(hal_vsr_table+7*4)
	# Now create an FPU context on the stack so that we can take
	# FPU-using interrupts and exceptions before the machine starts
	# up.
	subl	$i386reg_fpucontext_size,%esp
	movl	$0,i386reg_fpucontext_valid(%esp)
	movl	%esp,cyg_hal_fpustate_current
#endif
	.endm

#ifndef CYGHWR_HAL_I386_FPU_SWITCH_LAZY

	# Non-lazy CPU state switching. We simply switch the entire
	# FPU state on every context switch, interrupt or exception.

	# ------------------------------------------------------------
	# Context switch handling

	.macro	hal_fpu_push_ctx
	subl	$i386reg_fpstate_size,%esp	# make space
	fsave	i386reg_fpstate(%esp)		# save FPU state
	movl	$1,i386reg_fpstate_valid(%esp)	# indicate it is valid
	.endm

	.macro	hal_fpu_pop_ctx
	btl	$0,i386reg_fpstate_valid(%esp)	# check ls bit of valid flag
	jc	1f				# if set, restore state
	finit					# otherwise init FPU
	jmp	2f				# and skip restore
1:
	frstor  i386reg_fpstate(%esp)		# restore FPU state
2:
	addl	$i386reg_fpstate_size,%esp	# pop space used
	.endm

	# ------------------------------------------------------------
	# Interrupt and exception handling

	# In this configuration, the interrupt and exception code behaves in
	# exactly the same way as the context switch code.

	.macro	hal_fpu_push_int
	hal_fpu_push_ctx
	.endm

	.macro	hal_fpu_push_int_annex
	.endm

	.macro	hal_fpu_pop_int_annex
	.endm

	.macro	hal_fpu_pop_int
	hal_fpu_pop_ctx
	.endm

	.macro	hal_fpu_push_exc
	hal_fpu_push_ctx
	.endm

	.macro	hal_fpu_push_exc_annex
	.endm

	.macro	hal_fpu_pop_exc_annex
	.endm

	.macro	hal_fpu_pop_exc
	hal_fpu_pop_ctx
	.endm

#else // CYGHWR_HAL_I386_FPU_SWITCH_LAZY

	# Lazy CPU state switching. We defer CPU state switching until the new
	# thread actually uses the FPU. This state switch is handled by
	# __fpu_switch_vsr in vectors.S.

	.extern cyg_hal_fpustate_owner
	.extern cyg_hal_fpustate_current

	# ------------------------------------------------------------
	# Context switch handling

	# On context switch we simply stack a pointer to this
	# threads FPU context save area.

	.macro	hal_fpu_push_ctx
	pushl	cyg_hal_fpustate_current		# push our FPU state ptr
	.endm

	# We do nothing here but set the CR0:TS bit to force
	# an exception when the FPU is next used and pop the
	# FPU save area pointer into the static variable.

	.macro	hal_fpu_pop_ctx
	movl	%cr0, %ecx			# get CR0
	orl	$0x8, %ecx			# set TS bit
	movl	%ecx, %cr0			# restore CR0
	popl	cyg_hal_fpustate_current	# set current FPU state pointer
	.endm

	# ------------------------------------------------------------
	# Interrupt handling

	# On entry to an interrupt we save the current threads FPU context
	# pointer and set the CR0:TS bit to trap any FP operations in the
	# interrupt.

	.macro	hal_fpu_push_int
	pushl	cyg_hal_fpustate_current	# push current threads FPU state ptr
	# ensure that CR0:TS bit is set
	movl	%cr0, %ecx			# get CR0
	orl	$0x8, %ecx			# set TS bit
	movl	%ecx, %cr0			# restore CR0
	.endm

	# The following is called after we transfer to the interrupt
	# stack. We make space here for the FPU state of the interrupt
	# handler to be saved in case we get nested interrupts that use FP.

	.macro	hal_fpu_push_int_annex
	subl	$i386reg_fpucontext_size,%esp
	movl	$0,i386reg_fpucontext_valid(%esp)
	movl	%esp,cyg_hal_fpustate_current
	.endm

	# This is invoked just before any transfer back to the thread stack.
	# We check whether we are the FPU state owner, and if so, abdicate.
	# There is no need to save the state, the next thread will load its
	# own state over the top of it.

	.macro	hal_fpu_pop_int_annex
	cmpl	cyg_hal_fpustate_owner,%esp	# are we FPU owner?
	jne	1f				# if not, then just continue
	movl	$0,cyg_hal_fpustate_owner	# no one owns FPU now
	# ensure that CR0:TS bit is set to force a reload of
	# the previous FPU state
	movl	%cr0, %ecx			# get CR0
	orl	$0x8, %ecx			# set TS bit
	movl	%ecx, %cr0			# restore CR0
1:
	addl	$i386reg_fpucontext_size,%esp	# pop FPU save area
	.endm

	# Final return from interrupt handling. Just pull the current
	# FPU context off the stack.
	.macro	hal_fpu_pop_int
	popl	cyg_hal_fpustate_current	# set current FPU state pointer
	.endm

	# ------------------------------------------------------------
	# Exception handling

	# Whenever we take an exception, we save the current FPU state away
	# into its save area. This way, if we are going to end up in GDB, the
	# whole machine state is saved in memory.

	.macro	hal_fpu_push_exc
	pushl	cyg_hal_fpustate_current	# push our FPU state ptr
	movl	cyg_hal_fpustate_owner,%eax	# EAX = FPU state owner
	jz	1f				# skip if zero
	fsave	i386reg_fpucontext_state(%eax)	# save state
	movl	$1,i386reg_fpucontext_valid(%eax) # set valid
	movl	$0,cyg_hal_fpustate_owner	# zero owner pointer
1:
	.endm

	# The rest of the exception macros behave exactly like the
	# interrupt ones.

	.macro	hal_fpu_push_exc_annex
	hal_fpu_push_int_annex
	.endm

	.macro	hal_fpu_pop_exc_annex
	hal_fpu_pop_int_annex	
	.endm

	.macro	hal_fpu_pop_exc
	hal_fpu_pop_int
	.endm

#endif // CYGHWR_HAL_I386_FPU_SWITCH_LAZY

#else /* !CYGHWR_HAL_I386_FPU */

	# Non-FP macros.

	.macro	hal_fpu_init
	.endm

	.macro	hal_fpu_push_ctx
	.endm

	.macro	hal_fpu_pop_ctx
	.endm

	.macro	hal_fpu_push_int
	.endm

	.macro	hal_fpu_push_int_annex
	.endm

	.macro	hal_fpu_pop_int_annex
	.endm

	.macro	hal_fpu_pop_int
	.endm

	.macro	hal_fpu_push_exc
	.endm

	.macro	hal_fpu_push_exc_annex
	.endm

	.macro	hal_fpu_pop_exc_annex
	.endm

	.macro	hal_fpu_pop_exc
	.endm

#endif

#endif	

#------------------------------------------------------------------------------
# MMU macros.

#ifndef CYGPKG_HAL_I386_MMU_DEFINED

#define CYGPKG_HAL_I386_MMU_DEFINED

	.macro 	hal_mmu_init
	.endm

#endif	

#------------------------------------------------------------------------------
# A20 gate enable

#define K_RDWR			0x60
#define	K_STATUS		0x64
#define	K_CMD			0x64
#define K_OBUF_FUL		0x01
#define K_IBUF_FUL		0x02
#define KC_CMD_WIN		0xD0
#define	KC_CMD_WOUT		0xD1
#define KB_A20			0xDF

	.macro	hal_a20_enable
	// Enable A20 so that addresses at 1MB don't wrap around back to 0.
1:	inb 	$K_STATUS, %al
	testb 	$K_IBUF_FUL, %al
	jnz 	1b

2:	inb 	$K_STATUS, %al
	testb 	$K_OBUF_FUL, %al
	jz 	3f
	inb 	$K_RDWR, %al
	jmp 	2b

3:	movb 	$KC_CMD_WOUT, %al
	outb 	%al, $K_CMD
1:	inb 	$K_STATUS, %al
	testb 	$K_IBUF_FUL, %al
	jnz 	1b

	movb 	$KB_A20, %al
	outb 	%al, $K_RDWR
1:	inb 	$K_STATUS, %al
	testb 	$K_IBUF_FUL, %al
	jnz 	1b
	.endm

#------------------------------------------------------------------------------
# MEMC macros.
# This version simply enables the A20 gate.
	
#ifndef CYGPKG_HAL_I386_MEMC_DEFINED

	.macro	hal_memc_init
	hal_a20_enable
	.endm

#endif	
	
#------------------------------------------------------------------------------
# Cache macros.
	
#ifndef CYGPKG_HAL_I386_CACHE_DEFINED

	.macro	hal_cache_init
	.endm

#endif	

#------------------------------------------------------------------------------
# Diagnostics macros.
	
#ifndef CYGPKG_HAL_I386_DIAG_DEFINED

	.macro	hal_diag_init
	.endm

	.macro	hal_diag_excpt_start
	.endm

	.macro	hal_diag_intr_start
	.endm

	.macro	hal_diag_restore
	.endm
#endif	

#------------------------------------------------------------------------------
# Timer initialization.
	
#ifndef CYGPKG_HAL_I386_TIMER_DEFINED

	.macro	hal_timer_init
	.endm

#endif	

#------------------------------------------------------------------------------
# Monitor initialization.
	
#ifndef CYGPKG_HAL_I386_MON_DEFINED

	.macro	hal_mon_init
	.endm

#endif	

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_ARCH_INC
# end of arch.inc
