##=============================================================================
##
##      romboot.S
##
##      x86 romboot
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
## Purpose:     x86 romboot
## Description: When booting from ROM we need to run a short sequence of real
##		mode code before jumping into 32 bit protected mode. There are
##              problems in GAS with mixing all this code together, and it is
##              easier to just move this bit of code out to a separate file.
##              
##
######DESCRIPTIONEND####
##
##=============================================================================


#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/hal/arch.inc>

#==============================================================================

    .file   "romboot.S"

#------------------------------------------------------------------------------

	.code16

romboot_start:			
	/* Disable interrupt handling. */
	cli

	# Set DS == CS
	movw	%cs,%ax
	movw	%ax,%ds
	# set ES == 0
	movw	$0,%ax
	movw	%ax,%es

	# Call video bios to init display
  	movw	$0xc000,%bx
	movw  	%bx,%ds
  	movw  	0x0000,%ax
  	movw  	$0x0000,%bx
  	movw  	%bx,%ds
	cmpw  	$0xAA55,%ax
  	jne  	1f
	.byte	0x9a		# lcall
	.word	0x0003		# offset
	.word   0xc000		# segment
1:
	
	movw	%cs,%ax
	movw	%ax,%ds
	# set ES == 0
	movw	$0,%ax
	movw	%ax,%es

	# build a GDT descriptor in memory at location zero
	movw	$(gdtEnd - gdtStart),%ax
	movw	%ax,%es:0
	lea	gdtStart,%ax
	addw	$0xFF00,%ax
	movw	%ax,%es:2
	movw	$0x000F,%ax
	movw	%ax,%es:4
	
	# load GDTR
	lgdt	%es:0

	# Switch to protected mode.
	movl	%cr0,%eax
	orb	$1, %al
	movl	%eax,%cr0

	# and do a jump to flush instruction prefetch
	jmp	romboot_pm

	hlt

	.align	4, 0xCC
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

	.align	4, 0xCC
gdtEnd:

	# We arrive here in protected mode
romboot_pm:	

	# Load data selectors
	movw	$0x10, %ax
	movw	%ax, %ds
	movw	%ax, %es
	movw	%ax, %fs
	movw	%ax, %gs
	
	# jump to start of ROM, where the PM code starts	
#	ljmp	$8,$0xF0000
	.byte	0x66,0xea	# opsize + ljmp opcode
	.long	0x000F0000	# destination address
	.word	0x0008		# code selector

	.code16

	.org	0xF0
romboot_reset:		
	jmp	romboot_start

	.org	0x100
	
#------------------------------------------------------------------------------
# end of romboot.S
