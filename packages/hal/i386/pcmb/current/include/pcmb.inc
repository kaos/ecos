#ifndef CYGONCE_HAL_PCMB_INC
#define CYGONCE_HAL_PCMB_INC
##=============================================================================
##
##      pcmb.inc
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


##=============================================================================
## CPU initialization

#ifndef CYGPKG_HAL_I386_CPU_INIT_DEFINED
	
#ifdef CYG_HAL_STARTUP_FLOPPY

#define CYGPKG_HAL_I386_CPU_INIT_DEFINED

	.macro 	hal_cpu_init

	/* This code is loaded from a floppy disk when the PC powers up.  */

	.code16

	.extern _end

	sectorsPerTrack = 18
	bytesPerSector = 512
	esPerSector = 32	/* = 512/16 */

	cld				/* always count up. */

	/* Configure a stack that we can use. */
	
	movl 	$_start, %eax
	movw 	%ax, %sp
	shr 	$4, %eax
	andl 	$0xF000, %eax
	movw 	%ax, %ss

	/* Ask the BIOS for info about the amount of RAM available.  We push
         * these onto the stack for later use.
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

	/* reset floppy */
	movb $0,%ah
	movb $0,%dl
	int $0x13
	jc _error1
	
	/* Read the rest of the image to _start.  This code works by reading
	   only one sector at a time to avoid "buffer cross 64k boundary" fatal
	   problem... This is slow but should work in almost all situations.
	   _start should be aligned on a 512 bytes boundary to be sure.
	*/
	
	/* destination pointer es:bx */
	/* With correct alignement, bx should be 0 and es should be a multiple
         * of 32. If not it may cause the "buffer cross 64k boundary" problem
         * (cf above)
	 */  
	movl	$_start,%eax
	movw	%ax,%bx
	andw	$0xF,%bx
	shrl	$4,%eax
	movw	%ax, %es

	/* initials head/track/sector */
	movw	$0,%dx
	movw	$1,%cx

	movl	$_edata,%edi
	addl	$(bytesPerSector-1),%edi
	shrl	$4,%edi
	
	jmp	_loadsector

_nextsector:
	movw	%es,%ax
	cmpw	%di,%ax
	jge	_endload
	addw	$esPerSector,%ax
	movw	%ax,%es
	incb	%cl
        cmpb	$sectorsPerTrack, %cl 
        jbe	_loadsector	/* next head ?*/
	movb	$1, %cl
        incb	%dh    
        cmpb	$1, %dh
        je	_loadsector	/* next track ? */
        movb	$0, %dh
        incb	%ch    

_loadsector:

	pushw	%es
	pushw	%di
	movw	$0x0201, %ax
	clc
	int	$0x13
	popw	%di
	popw	%es
	jc _error2

	movw	$(0x0E*256+'.'), %ax	/* print a dot */
	int	$0x10

	/* So go ahead and resume execution at the real starting address.  This
	only serves to move us quickly to the real starting location; and has
	no effect after reading additional tracks.  If we didn't jump after
	reading the first track, then we limit ourselves to reading images of
	30k bytes max before overwriting ourselves at 0x7C00.
	*/

	ljmp $0,$_nextsector	

_error1:
	movw	$(0x0E*256+'1'), %ax	/* print a ! */
	int	$0x10	
	jmp	_error

_error2:
	mov	%ah,%al
	pushw	%ax
	shrw	$4,%ax
	andw	$15,%ax
	addw	$0x0E41,%ax
	int	$0x10
	popw	%ax
	andw	$15,%ax
	addw	$0x0E41,%ax
	int	$0x10
	
	movw	$(0x0E*256+'2'), %ax	/* print a ! */
	int	$0x10
	jmp	_error
		
_error:		/* halt on error */
	movw	$(0x0E*256+'!'), %ax	/* print a ! */
	int	$0x10

	cli
	hlt
	jmp _start


        /* Write the 0x55/0xAA signature at the end of the first
	block.  Without this signature the BIOS won't consider this
	block to be bootable.  
	*/
	
	. = _start + 510
	.byte	0x55
	.byte	0xAA

_endload:
1:

	/* Lets be nice and wait for the diskette drive motor to go off
         * before continuing. */
	
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
	.extern idtStart
	.word	0x07FF		# space for 256 entries
	.long	idtStart

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

hal_cpu_init_end:
        nop

	.endm /* hal_cpu_init */
	
#endif /* CYG_HAL_STARTUP_FLOPPY */

#endif // CYGPKG_HAL_I386_CPU_INIT_DEFINED

##=============================================================================
## Interrupt controller support
	
#define CYGPKG_HAL_I386_INTC_INIT_DEFINED

#ifndef CYG_HAL_STARTUP_RAM
			
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
	movb	$0xFB, %al			/* Mask off all interrupts except 2. */
	outb	%al, $0x21

	movb	$0x11, %al
	outb	%al, $0xA0
	movb	$0x28, %al
	outb	%al, $0xA1
	movb	$0x02, %al
	outb	%al, $0xA1
	movb	$0x01, %al
	outb	%al, $0xA1
	movb	$0xFF, %al			/* Mask off all interrupts. */
	outb	%al, $0xA1

	.endm /* hal_intc_init */

#else

	# No need to do any initialization in RAM startup
	.macro	hal_intc_init
	.endm	

#endif	
	
	.macro hal_intc_ack vector
	# Use any registers you like.
	movl	\vector, %edx
	movb	$0x20, %al
	cmpl	$0x20, %edx
	jl	8f
	cmpl	$0x28, %edx
	jl	9f
	outb	%al, $0xA0
9:	outb	%al, $0x20
8:	nop
	.endm
	
##=============================================================================
#endif // ifndef CYGONCE_HAL_PCMB_INC	
## end of pcmb.inc
