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


##=============================================================================
## CPU initialization
	
#define CYGPKG_HAL_I386_CPU_INIT_DEFINED

	.macro 	hal_cpu_init

#ifdef CYG_HAL_STARTUP_FLOPPY
	
	/* This code is loaded from a floppy disk when the PC powers up.  */

	.code16

	.extern _end

	sectorsPerTrack = 18
	bytesPerSector = 512
	bytesPerTrack = sectorsPerTrack * bytesPerSector
	dptLength = 3 /* words. */

	cld				/* always count up. */

	/* Configure a stack that we can use. */
	
	movl 	$_start, %eax
	movw 	%ax, %sp
	shr 	$4, %eax
	andl 	$0xF000, %eax
	movw 	%ax, %ss

	/* Ask the BIOS for info about the amount of RAM available.  We push these
	onto the stack for later use.
	*/
	
	xorl 	%eax, %eax
	movb 	$0x88, %ah		/* Get the amount of extended memory. */
	int 	$0x15
	shl 	$10, %eax
	pushl 	%eax

	xorl	%eax, %eax
	int	$0x12			/* Get the amount of standard memory. */
	shl	$10, %eax
	pushl	%eax


	/* Read the rest of the image to _start.  This code works by reading a track
	at a time...
	*/
	
	movl	$_edata, %eax
	movl	$_start, %ebx		/* Our destination address. */
	subl	%ebx, %eax		/* Number of bytes in the image. */
	movl	$1, %ecx		/* Track/sector to read from (starts at 1). */

0:
	cmpl	$0, %eax
	jle	1f

	pushl	%eax			/* Save the number of bytes we want. */
	pushl	%ebx			/* Save our current address. */
	pushl	%ecx			/* Save our disk location. */

	/* Read in a track with head 0. */
	
	movl	%ebx, %eax		/* Put our address into ES:BX. */
	shr	$4, %eax
	movw	%ax, %es
	andl	$0xF, %ebx
	movw	$0, %dx			/* Disk 0, head 0 */
	movb	$0x02, %ah
	movb	$sectorsPerTrack, %al	/* Read the entire track. */
	movb	$1, %cl
	int	$0x13			/* CX points to our track/sector. */

	/* So go ahead and resume execution at the real starting address.  This
	only serves to move us quickly to the real starting location; and has
	no effect after reading additional tracks.  If we didn't jump after
	reading the first track, then we limit ourselves to reading images of
	30k bytes max before overwriting ourselves at 0x7C00.
	*/
	
	ljmp	$0, $2f
	hlt

	.align 4
2:

	popl	%ecx
	popl	%ebx
	popl	%eax

	/* Add our length to the address. */
	addl	$bytesPerTrack, %ebx
	subl	$bytesPerTrack, %eax

	/* Maybe read in a track with head 1. */
	cmpl	$0, %eax
	jle	1f

	pushl	%eax			/* Save the number of bytes we want. */
	pushl	%ebx			/* Save our current address. */
	pushl	%ecx

	movl	%ebx, %eax		/* Put our address into ES:BX. */
	shr	$4, %eax
	movw	%ax, %es
	andl	$0xF, %ebx
	movw	$0x0100, %dx		/* Disk 0, head 1 */
	movb	$0x02, %ah
	movb	$sectorsPerTrack, %al	/* Read the entire track. */
	movb	$1, %cl
	int	$0x13			/* CX points to our track/sector. */

	popl	%ecx
	popl	%ebx
	popl	%eax

	/* Add our length to the address. */
	addl	$bytesPerTrack, %ebx
	subl	$bytesPerTrack, %eax

	/* Move to the next track. */
	addb	$0x1, %ch

	jmp 0b

	/* Write the 0x55/0xAA signature at the end of the first
	block.  Without this signature the BIOS won't consider this
	block to be bootable.  
	*/
	
	. = _start + 510
	.byte	0x55
	.byte	0xAA


1:

	/* Lets be nice and wait for the diskette drive motor to go off before continuing. */
	
	movw	$0x40, %ax
	movw	%ax, %es
	movl	$0x40, %ebx
2:	es
	movb	(%bx), %al
	cmpb	$0, %al
	jne	2b

	/* Now we're all loaded up in memory. */

	/* Disable interrupt handling. */
	cli

	/* Load GDTR and IDTR. */

	lgdt	%cs:gdt
	lidt	%cs:idt

	/* Switch to protected mode. */
	movl	%cr0,%eax
	orb	$1, %al
	movl	%eax,%cr0
	ljmp	$8, $3f

	hlt

	.align	4, 0xFF
gdt:
	.word	gdtEnd - gdtStart
	.long	gdtStart

	.align	4, 0xFF
idt:
	.word	0x07FF		/* Allow space for 256 interrupt vectors. */
	.long	0

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

	/* Selector 0x18 == shorter code: faults any code access 0xF0000000-0xFFFFFFFF. */
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

	.code32
3:

	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs

	/* Make our new stack point to the same place as the old one. */
	xorl	%ebx, %ebx
	movw	%ss, %bx
	shl	$4, %ebx
	addl	%esp, %ebx
	movw	%ax, %ss
	movl	%ebx, %esp
	movl	$0, %ebp

/* Reset the flags register. */
	pushl	$0
	popfl

/* Save the standard and extended memory lengths into some static variables...
	I'm predicting that someone is going to break our way of passing a pointer
	to this structure...  may as well avoid that problem now.
*/
	.data
	.align	4
	.globl	pc_standard_memory_size
pc_standard_memory_size:
	.long		0
	.globl	pc_extended_memory_size
pc_extended_memory_size:
	.long		0
	.text
	movl	0(%esp), %eax
	movl	4(%esp), %ebx
	movl	%eax, pc_standard_memory_size
	movl	%ebx, pc_extended_memory_size

/* _pc_entry() takes a pointer to our startup array (which only has standard
	and extended memory sizes in it, on the stack).
*/
	movl	%esp, %ebx		/* They are the last parameters on the stack. */
	pushl	%ebx

#endif /* CYG_HAL_STARTUP_FLOPPY */

#ifdef CYG_HAL_STARTUP_RAM

	/* In this case, the sizes of memory are pointed to by a pointer in IRQ
	vector 15.
	*/
	
	/* Save the standard and extended memory lengths into some static variables...
	*/
	.data
	.align	4
	.globl	pc_standard_memory_size
pc_standard_memory_size:
	.long	0
	.globl	pc_extended_memory_size
pc_extended_memory_size:
	.long	0
	.text
	movl	(15 * 8), %ebp
	movl	0(%ebp), %eax
	movl	4(%ebp), %ebx
	movl	%eax, pc_standard_memory_size
	movl	%ebx, pc_extended_memory_size

#endif /* CYG_HAL_STARTUP_RAM */

	.endm /* hal_cpu_init */


##=============================================================================
## Interrupt controller support
	
#define CYGPKG_HAL_I386_INTC_INIT_DEFINED
	
	.macro	hal_intc_init

	# The interrupt controller is configured so that IRQ levels 0-7 trigger
	#  interrupt vector 32-39; levels 8-15 trigger 40-47.
	movb	$0x11, %al
	outb	%al, $0x20
	movb	$0x20, %al
	outb	%al, $0x21
	movb	$0x04, %al
	outb	%al, $0x21
	movb	$0x01, %al
	outb	%al, $0x21
	movb	$0xFF, %al			/* Mask off all interrupts. */
	outb	%al, $0x21

	movb	$0x11, %al
	outb	%al, $0xA0
	movb	$0x28, %al
	outb	%al, $0xA1
	movb	$0x02, %al
	outb	%al, $0xA1
	movb	$0x01, %al
	outb	%al, $0xA1
	movb	 $0xFF, %al			/* Mask off all interrupts. */
	outb	%al, $0xA1

	.endm /* hal_intc_init */

	.macro hal_intc_ack vector
	# Use any registers you like.
	movl	\vector, %edx
	movb	$0x20, %al
	cmpl	$0x20, %edx
	jl	8f
	cmpl	$0x30, %edx
	jl	9f
	outb	%al, $0xA0
9:	outb	%al, $0x20
8:	nop
	.endm
	
##=============================================================================
## FPU support
		
#define CYGPKG_HAL_I386_FPU_DEFINED

	.macro hal_fpu_init
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
	pushl	$0
        fstcw   0(%esp)
        movl	0(%esp),%eax
        andb	$(~0x04),%al
        movl	%eax,0(%esp)
        fldcw	0(%esp)
	addl	$4,%esp

	# Tell the CPU to generate a interrupt 7 when the math device is used.
#if 0
	movl	%cr0, %eax
	orl	$0x3A, %eax
	movl	%eax, %cr0
#endif
	
	.endm /* hal_fpu_init */


	.macro	hal_fpu_save regs
	.endm /* hal_fpu_save */

	.macro	hal_fpu_save_caller regs
	.endm /* hal_fpu_save_caller */

	.macro	hal_fpu_save_callee regs
	.endm /* hal_fpu_save_callee */

	.macro	hal_fpu_load_caller regs
	.endm /* hal_fpu_load_caller */

	.macro	hal_fpu_load_callee regs
	.endm /* hal_fpu_load_callee */

	.macro	hal_fpu_load regs
	.endm /* hal_fpu_load */


##=============================================================================
#endif // ifndef CYGONCE_HAL_PLATFORM_INC	
## end of platform.inc
