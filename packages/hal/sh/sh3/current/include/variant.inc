#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##      variant.inc
##
##      SH3 variant assembler header file
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
## Date:        2000-10-30
## Purpose:     SH3 variant definitions and init code
## Description: This file contains various definitions and macros that are
##              useful for writing assembly code for the SH3 CPU family.
## Usage:
##              #include <cyg/hal/variant.inc>
##              ...
##              
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/sh_regs.h>
        
#===========================================================================
# SR initialization value
# zero all bits except:
# MD = Processor operation mode field (privileged mode)
# I0-3 = Mask out all interrupts but NMI.
#
# When saving or restoring the state of an exception or interrupt, the bit
# CYGARC_REG_SR_RB is also set, switching the register bank. When this bit
# is set, interrupts must be disabled.
#
# Note: We could also use the BL bit to prevent interrupts, but that would
#       also prevent the use of breakpoints.

#define CYG_SR (CYGARC_REG_SR_MD|CYGARC_REG_SR_IMASK)
#define CYG_SR_BANK1 (CYGARC_REG_SR_MD|CYGARC_REG_SR_IMASK|CYGARC_REG_SR_RB)

##-----------------------------------------------------------------------------
## Hardware init macros
#ifndef CYGPKG_HAL_SH_POST_RESET_INIT
	.macro  hal_post_reset_init
        # Initialize CPU
        mov.l   $nCYG_SR,r1             ! Put CPU in a well-known state
        ldc     r1,sr
        mov     #0,r0
        mov     #CYGARC_REG_CCR,r1      ! Disable cache
        mov.l   r0,@r1
        mov     #CYGARC_REG_MMUCR,r1    ! Disable MMU
        mov.l   r0,@r1
        mov     #CYGARC_REG_BBRA,r1     ! Disable UBC Channel A
        mov.w   r0,@r1
        mov     #CYGARC_REG_BBRB,r1     ! Disable UBC Channel B
        mov.w   r0,@r1
        mov     #CYGARC_REG_BRCR,r1     ! Reset UBC common register
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_TSTR,r1     ! Disable timers
        mov.b   r0,@r1
        mov.l   $CYGARC_REG_IPRA,r1     ! Disable interrupt request sources
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRB,r1
        mov.w   r0,@r1
#if (CYGARC_SH_MOD_INTC >= 2)
        mov.l   $CYGARC_REG_IPRC,r1
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRD,r1
        mov.w   r0,@r1
        mov.l   $CYGARC_REG_IPRE,r1
        mov.w   r0,@r1
#if (CYGARC_SH_MOD_INTC >= 3)
        mov.l   $CYGARC_REG_IPRF,r1
        mov.w   r0,@r1
#endif
	mov.w   $nCYG_ICR1_INIT,r0
        mov.l   $CYGARC_REG_ICR1,r1     ! Set interrupt controller to IRQ mode
        mov.w   r0,@r1
#endif
	mov.w	$nCYG_WTCSR,r0          ! Clear watchdog
	mov	#CYGARC_REG_WTCSR,r1
	mov.w	r0,@r1

        # Initialize VBR if necessary
#if     !defined(CYG_HAL_STARTUP_RAM) ||                \
        (       defined(CYG_HAL_STARTUP_RAM) &&         \
                !defined(CYGSEM_HAL_USE_ROM_MONITOR))
        mov.l   $_reset,r1             ! Set VBR
        ldc     r1,vbr
#endif
	bra	1f
	 nop
	
$nCYG_WTCSR:
	.word	0xa500		! clear all CSR bits

	.align	2
$nCYG_SR:
        .long   CYG_SR
$CYGARC_REG_TSTR:
        .long   CYGARC_REG_TSTR
$CYGARC_REG_IPRA:
        .long   CYGARC_REG_IPRA
$CYGARC_REG_IPRB:
        .long   CYGARC_REG_IPRB
#if (CYGARC_SH_MOD_INTC >= 2)
$CYGARC_REG_IPRC:
        .long   CYGARC_REG_IPRC
$CYGARC_REG_IPRD:
        .long   CYGARC_REG_IPRD
$CYGARC_REG_IPRE:
        .long   CYGARC_REG_IPRE
$CYGARC_REG_ICR1:
        .long   CYGARC_REG_ICR1
$nCYG_ICR1_INIT:
	.word	CYGARC_REG_ICR1_INIT
	.align	2
#endif
#if (CYGARC_SH_MOD_INTC >= 3)
$CYGARC_REG_IPRF:
        .long   CYGARC_REG_IPRF
#endif
	SYM_PTR_REF(_reset)

1:	
	.endm
#define CYGPKG_HAL_SH_POST_RESET_INIT
#endif

##-----------------------------------------------------------------------------
## Interrupt decode macros
	.macro	hal_intc_decode tmp,inum
#if (CYGARC_SH_MOD_INTC >= 2)
        mov.l   $nCYGARC_REG_INTEVT2,\tmp
#else
        mov     #CYGARC_REG_INTEVT,\tmp
#endif
        mov.l   @\tmp,\inum
        mov     #-5,\tmp                ! divide cause by 0x20
        shld    \tmp,\inum
        add     #-14,\inum              ! adjust so NMI becomes 0

	.endm
	
#ifdef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN
	.macro	hal_intc_translate inum,vnum
	mov     #0,\vnum		! Just vector zero is supported
	.endm
#else			
	.macro	hal_intc_translate inum,vnum
	mov	\inum,\vnum		! Vector == interrupt number
        shll2   \vnum                   ! get from vector number to ISR index
	.endm
#endif

#------------------------------------------------------------------------------
#endif // CYGONCE_HAL_VARIANT_INC
# end of variant.inc
