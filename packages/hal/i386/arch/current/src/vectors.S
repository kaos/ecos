##=============================================================================
##
##      vectors.S
##
##      x86 exception vectors
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
## Author(s):   jskov
## Contributors:jskov
## Date:        1999-01-07
## Purpose:     x86 exception vectors
## Description: This file defines the code placed into the exception
##              vectors. It also contains the first level default VSRs
##              that save and restore state for both exceptions and
##              interrupts.
##
######DESCRIPTIONEND####
##
##=============================================================================


#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif /* CYGPKG_KERNEL */

#include <cyg/hal/arch.inc>

#==============================================================================

    .file   "vectors.S"

#==============================================================================	
# The Linux synthetic target defines its own startup.

#ifdef CYGPKG_HAL_I386_LINUX

	.extern	_linux_entry
	.text
	.globl	_start

_start:
	jmp	_linux_entry

#else  ## ifdef CYGPKG_HAL_I386_LINUX
#==============================================================================
# Real startup code. We jump here from the various reset vectors to set up the
# world.

	.text
	.globl	_start

_start: 
	hal_cpu_init
	hal_diag_init
	hal_mmu_init
	hal_fpu_init
	hal_memc_init
	hal_intc_init
	hal_cache_init
	hal_timer_init

# loading the stack pointer seems appropriate now.
	movl	$_start, %esp

# Save the sizes of memory on the stack.  These were initialized by hal_cpu_init.
	movl	pc_standard_memory_size, %eax
	movl	pc_extended_memory_size, %ebx
	pushl	%ebx
	pushl	%eax
	pushl	%esp		# pointer to that, um, 'struct' that we just saved.
	
	hal_mon_init

	.extern hal_zero_bss
	call	hal_zero_bss

	.extern hal_variant_init
	call	hal_variant_init

	.extern hal_platform_init
	call	hal_platform_init

	.extern cyg_hal_invoke_constructors
	call	cyg_hal_invoke_constructors

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
	.extern initialize_stub
	call	initialize_stub
#endif /* CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS */

# Unused IRQ vector 15 points to our 'struct'...
	popl	%eax
	movl	%eax, (15 * 8)
	
	.extern cyg_start
	call	cyg_start

# Hmm.  Not expecting to return from cyg_start.
1:	hlt
	jmp	1b

#==============================================================================
# Add a bit of debugger support.
#if 0	
	.align	4
	.globl	_breakinst
_breakinst:
	int	$3
	ret
#endif
	
#==============================================================================
#
#	hal_pc_generic_interrupt
#
#- Pushes the interrupt vector onto the stack, then
#  jumps to hal_pc_irq.
#

	.extern	__interrupt_stack

	.align	4, 0xCC
	.globl	hal_pc_generic_interrupt
	.globl	hal_pc_generic_interrupt_start
	.globl	hal_pc_generic_interrupt_vector
	.globl	hal_pc_generic_interrupt_end
hal_pc_generic_interrupt_start:
hal_pc_generic_interrupt:
	pushl	$0xbad4c0de
hal_pc_generic_interrupt_vector = . - 4
	jmp	*hal_pc_irq_p
hal_pc_generic_interrupt_end:

	.align	4, 0xCC
hal_pc_irq_p:
	.long	hal_pc_irq

#
#	hal_pc_irq
#
#- Actually saves the CPU state, executes ISRs, etc.
#- The stack has eflags, cs, eip, and vector; all 32 bits.
#
	.align	4, 0xCC
	.globl	hal_pc_irq
hal_pc_irq:
	pusha

# Maybe instrument for an interrupt.  Parameter==vector number.
	hal_pc_irq_instrument 0x20(%esp)

# Maybe instrument for idle detection.
	hal_pc_unidle_instrument
	
# Show how far nested we are.
	.extern hal_pc_in_interrupt
	incl	hal_pc_in_interrupt


#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
	.extern cyg_scheduler_sched_lock
	incl	cyg_scheduler_sched_lock
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */


# Call cyg_hal_interrupt_handlers[vector](vector, cyg_hal_interrupt_data[vector])
	movl	0x20(%esp), %ecx
	movl	$cyg_hal_interrupt_handlers, %ebx
	movl	(%ebx, %ecx, 4), %edx
	movl	$cyg_hal_interrupt_data, %ebx
	movl	(%ebx, %ecx, 4), %eax
	pushl	%esp		# Save the pointer to registers.  Secret parameter!
	pushl	%eax
	pushl	%ecx
	call	*%edx		# eax has the return value, needed for interrupt_end()
	popl	%ebx
	popl	%ebx
	popl	%ebx

# Fix our nesting figure.
	decl	hal_pc_in_interrupt

# Following code messes up eax, but not esi.
	movl	%eax, %esi
	
# Send an EOI, maybe.
	hal_intc_ack 0x20(%esp)

# Reenable interrupts, if necessary.
	movl	0x2C(%esp), %eax
#	Turn off the single step flag.  GDB uses that to single step through
#	the code; we'll use the tasks' version of this flag in the popf later.
	andl	$~(0x100), %eax
	pushl	%eax
	popfl

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
# Call interrupt_end(r, cyg_hal_interrupt_objects[vector], regs)
# - r is the return value from cyg_hal_interrupt_handlers;
# - regs just points to the stack; we have already saved our context there;
	movl	$cyg_hal_interrupt_objects, %ebx
	movl	0x20(%esp), %ecx	
	movl	(%ebx, %ecx, 4), %edx
	# regs is left from the push of ebp above;
	pushl	%edx
	pushl	%eax
	call	interrupt_end
	addl	$8, %esp
#endif /* CYGFUN_HAL_COMMON_KERNEL_SUPPORT */

# Maybe uninstrument the irq handler.
	hal_pc_irq_uninstrument 0x20(%esp)
	
	popa			# restore all our registers.
	addl	$4, %esp	# dump the vector number
	iret			# and return to the program.

#==============================================================================	
# hal_idle_thread_action(cyg_uint32 loop_count)
	
	.align	4
	.globl	hal_idle_thread_action
hal_idle_thread_action:
	hal_pc_idle_instrument
	hlt
	ret
	
#==============================================================================
# Port IO functions
		
# pc_outb(port, value)
	.align	4
	.globl	pc_outb
pc_outb:
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outb	%al, %dx
	ret


# x = pc_inb(port)
	.align	4
	.globl	pc_inb
pc_inb:
	movl	4(%esp), %edx
	inb	%dx, %al
	cbtw
	cwtl
	ret

# pc_outw(port, value)
	.align	4
	.globl	pc_outw
pc_outw:
	movl	4(%esp), %edx
	movl	8(%esp), %eax
	outw	%ax, %dx
	ret

# x = pc_inw(port)
	.align	4
	.globl	pc_inw
pc_inw:
	movl	4(%esp), %edx
	inw	%dx, %ax
	cwtl
	ret

# x = CPU_swap_u16(x)
	.align	4
	.global CPU_swap_u16
CPU_swap_u16:
	xorl	%eax, %eax
	movb	4(%esp), %ah
	movb	5(%esp), %al
	ret

# x = CPU_swap_u32(x)
	.align	4
	.global CPU_swap_u32
CPU_swap_u32:
	xorl	%eax, %eax
	movb	4(%esp), %ah
	movb	5(%esp), %al
	sall	$16, %eax
	movb	6(%esp), %ah
	movb	7(%esp), %al
	ret
	
#endif // ifndef CYGPKG_HAL_I386_LINUX

#------------------------------------------------------------------------------
# end of vectors.S
