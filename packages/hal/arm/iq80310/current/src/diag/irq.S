//=============================================================================
//
//      irq.S - Cyclone Diagnostics
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Scott Coulter, Jeff Frazier, Eric Breeden
// Contributors:
// Date:        2001-01-25
// Purpose:     
// Description: 
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

/*
 * Low-lebvel interrupt support for IQ80310 diags
 */
	.extern iq80310_irq_handler
	.extern iq80310_fiq_handler

	.text
	.globl __diag_IRQ
  __diag_IRQ:
	ldr	sp, =__irq_stack    /* point stack pointer at IRQ stack */
	sub	lr, lr, #4 	   /* adjust lr (return to last address) */
	stmfd	sp!, {r12, lr}     /* push r12 and link reg onto stack */
	mrs	r12, spsr	   /* store spsr in r12 */
	stmfd	sp!, {r12}	   /* push spsr onto stack */
	stmfd	sp!, {r0-r11}  	   /* push all registers onto stack */
	bl	iq80310_irq_handler
	ldmfd	sp!, {r0-r12} 	   /* restore r0 thru r12 */
	msr	spsr, r12	   /* restore SPSR */
	ldmfd	sp!, {r12,pc}^	   /* restore r12 and PC, return */

	.globl __diag_FIQ
  __diag_FIQ:
	/* Cyclone FIQ handler */
	/* save registers onto stack */
	ldr     sp, =__fiq_stack	/* point stack pointer at FIQ stack */
	sub	lr, lr, #4	/* adjust link register (return to last address) */
	stmfd	sp!, {r12, lr}	/* push r12 and link reg onto stack */
	mrs	r12, spsr	/* store spsr in r12 */
	stmfd	sp!, {r12}	/* push spsr onto stack */
	stmfd	sp!, {r0-r7}	/* push r0 thru r7 (r8 - r14 are banked) */
	bl      iq80310_fiq_handler
	/* restore registers and return */
	ldmfd	sp!, {r0-r7}	/* restore r0 thru r7 (r8 - r14 are banked) */
	ldmfd	sp!, {r12}
	msr	spsr, r12	/* restore SPSR */
	ldmfd	sp!, {r12,pc}^	/* restore r12 and PC, return to inst before exception occurred */

	.globl __ignore_abort
__ignore_abort:
	subs pc,lr,#4

	.globl _cspr_enable_fiq_int
_cspr_enable_fiq_int:
	mrs	r0, cpsr
	bic	r0, r0, #0x40
	msr	cpsr, r0
	mov     pc, lr

	.globl _cspr_enable_irq_int
_cspr_enable_irq_int:
	mrs	r0, cpsr
	bic	r0, r0, #0x80
	msr	cpsr, r0
	mov     pc, lr

	.bss
	.rept 1024
	.word 0
	.endr
  __irq_stack:
	.rept 1024
	.word 0
	.endr
  __fiq_stack:


