#ifndef CYGONCE_HAL_PLATFORM_INC
#define CYGONCE_HAL_PLATFORM_INC
##=============================================================================
##
##      platform.inc
##
##      PC platform support
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
## Contributors:jskov, pjo, nickg
## Date:        1999-01-07
## Purpose:     PC platform support
## Description: This file contains any PC specific assembler macros needed to
##		run eCos on a standard i386 PC.
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <cyg/hal/i386.inc>

#include <cyg/hal/pcmb.inc>	

##=============================================================================
## CPU initialization

#ifndef CYGPKG_HAL_I386_CPU_INIT_DEFINED
	
#define CYGPKG_HAL_I386_CPU_INIT_DEFINED

##=============================================================================
## ROM startup

#ifdef CYG_HAL_STARTUP_ROM

	.macro	hal_cpu_init

hal_cpu_init_start:

	# Disable interrupt handling.
	cli

	# Make a GDT pointer in location 0 and load new GDTR
	movw	$(gdtEnd - gdtStart),%ax
	movw	%ax,0
	leal	gdtStart,%eax
	movl	%eax,2
	lgdt	0

	# Make an IDT pointer in location 0 and load new LDTR
	movw	$0x77F,%ax
	movw	%ax,0
	leal	idtStart,%eax
	movl	%eax,2	
	lidt	0

	# Jump long to reload CS
	jmpl	$8,$3f

	.align	4, 0xFF	
gdtStart:
	/* Selector 0x00 == invalid. */
	.word	0x0000
	.word	0x0000
	.byte	0x00
	.byte	0x00
	.byte	0x00
	.byte	0x00

	/* Selector 0x08 == code. */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9B
	.byte	0xCF
	.byte	0x00

	/* Selector 0x10 == data. */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x93
	.byte	0xCF
	.byte	0x00

	/* Selector 0x18 == shorter code: faults any code 
         * access 0xF0000000-0xFFFFFFFF.
         */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x9B
	.byte	0xC7
	.byte	0x00

	/* Selector 0x20 == data; faults any access 0xF0000000-0xFFFFFFFF. */
	.word	0xFFFF
	.word	0x0000
	.byte	0x00
	.byte	0x93
	.byte	0xC7
	.byte	0x00

	.align	4, 0xFF
gdtEnd:

3:
	# Set up data selectors
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	movw	%ax, %ss
	
	# Set up SP
	movl	$__interrupt_stack,%esp
	
	/* Reset the flags register. */
	push	$0
	popf

hal_cpu_init_end:
        nop

	.endm	
#endif	
	
	
##=============================================================================
## RAM startup	
	
#ifdef CYG_HAL_STARTUP_RAM

	.macro	hal_cpu_init
	.endm
	
#endif /* CYG_HAL_STARTUP_RAM */

#endif // CYGPKG_HAL_I386_CPU_INIT_DEFINED

##=============================================================================
## IDT support
		
	.macro set_IDT_entry idx,addr
	pushl	$(idtStart+8*\idx) 
	pushl	$\addr
	.extern cyg_hal_pc_set_idt_entry
	call	cyg_hal_pc_set_idt_entry
	addl	$8,%esp
	.endm

  

	.macro hal_idt_init
	.text	
hal_idt_init:
	set_IDT_entry  0, hal_pc_exception_0
	set_IDT_entry  1, hal_pc_exception_1
	set_IDT_entry  2, hal_pc_exception_2
	set_IDT_entry  3, hal_pc_exception_3
	set_IDT_entry  4, hal_pc_exception_4
	set_IDT_entry  5, hal_pc_exception_5
	set_IDT_entry  6, hal_pc_exception_6
	set_IDT_entry  7, hal_pc_exception_7
	set_IDT_entry  8, hal_pc_exception_8
	set_IDT_entry  9, hal_pc_exception_9
	set_IDT_entry  10, hal_pc_exception_10
	set_IDT_entry  11, hal_pc_exception_11
	set_IDT_entry  12, hal_pc_exception_12
	set_IDT_entry  13, hal_pc_exception_13
	set_IDT_entry  14, hal_pc_exception_14
	set_IDT_entry  15, hal_pc_exception_15
	set_IDT_entry  16, hal_pc_exception_16
	set_IDT_entry  17, hal_pc_exception_17
	set_IDT_entry  18, hal_pc_exception_18
	set_IDT_entry  19, hal_pc_exception_19
	set_IDT_entry  20, hal_pc_exception_20
	set_IDT_entry  21, hal_pc_exception_21
	set_IDT_entry  22, hal_pc_exception_22
	set_IDT_entry  23, hal_pc_exception_23
	set_IDT_entry  24, hal_pc_exception_24
	set_IDT_entry  25, hal_pc_exception_25
	set_IDT_entry  26, hal_pc_exception_26
	set_IDT_entry  27, hal_pc_exception_27
	set_IDT_entry  28, hal_pc_exception_28
	set_IDT_entry  29, hal_pc_exception_29
	set_IDT_entry  30, hal_pc_exception_30
	set_IDT_entry  31, hal_pc_exception_31
	set_IDT_entry  32, hal_pc_irq_32
	set_IDT_entry  33, hal_pc_irq_33
	set_IDT_entry  34, hal_pc_irq_34
	set_IDT_entry  35, hal_pc_irq_35
	set_IDT_entry  36, hal_pc_irq_36
	set_IDT_entry  37, hal_pc_irq_37
	set_IDT_entry  38, hal_pc_irq_38
	set_IDT_entry  39, hal_pc_irq_39
	set_IDT_entry  40, hal_pc_irq_40
	set_IDT_entry  41, hal_pc_irq_41
	set_IDT_entry  42, hal_pc_irq_42
	set_IDT_entry  43, hal_pc_irq_43
	set_IDT_entry  44, hal_pc_irq_44
	set_IDT_entry  45, hal_pc_irq_45
	set_IDT_entry  46, hal_pc_irq_46
	set_IDT_entry  47, hal_pc_irq_47
	.endm
	
##=============================================================================
#endif // ifndef CYGONCE_HAL_PLATFORM_INC	
## end of platform.inc
