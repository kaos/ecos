#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##      variant.inc
##
##      PC variant support
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
## Purpose:     PC variant support
## Description: This file contains any i386 variant specific assembler macros 
##		needed to run eCos on a standard i386. There is not much variation
##		between IA-32 processors, so it is unlikely that there will ever
##		be much here.
##
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/i386.inc>
#include <cyg/hal/platform.inc>

##=============================================================================

#define CYGPKG_HAL_I386_MMU_DEFINED

#define K_RDWR			0x60
#define	K_STATUS		0x64
#define	K_CMD			0x64
#define K_OBUF_FUL		0x01
#define K_IBUF_FUL		0x02
#define KC_CMD_WIN		0xD0
#define	KC_CMD_WOUT		0xD1
#define KB_A20			0xDF

	.macro 	hal_mmu_init

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


##=============================================================================

	.macro 	hal_pc_irq_instrument vector
	# \vector (which is a stack reference)
	.endm

	.macro 	hal_pc_irq_uninstrument vector
	.endm


	.macro 	hal_pc_idle_instrument
#	movw 	$0x24C, %dx
#	movb 	$1, %al
#	outb 	%al, %dx
	.endm

	.macro 	hal_pc_unidle_instrument
#	movw 	$0x24C, %dx
#	movb 	$0, %al
#	outb 	%al, %dx
	.endm

##=============================================================================
#endif // ifndef CYGONCE_HAL_VARIANT_INC
## end of variant.inc
