##==========================================================================
##
##      variant.S
##
##      SH4 variant assembly code
##
##==========================================================================
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
##==========================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):    jskov
## Contributors: jskov
## Date:         2000-10-31
## Purpose:      SH4 misc assembly code
######DESCRIPTIONEND####
##
##==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_sh.h>

#include <cyg/hal/sh_regs.h>
#include <cyg/hal/sh4_offsets.inc>

#include <cyg/hal/arch.inc>
#include <cyg/hal/hal_intr.h>
	
#---------------------------------------------------------------------------
# Cache operations
# These need to be written in assembly to ensure they do not rely on data
# in cachable space (i.e., code must use registers exclusively, not the stack).

# This macro must be used at the top of each cache function. It ensures
# that the code gets executed from a shadow region where caching is disabled
# (0xA0000000).
        .macro GOTO_NONCACHED_SHADOW
        mova    10f,r0
        mov.l   $MASK,r1
        and     r1,r0
        mov.l   $BASE,r1
        or      r1,r0
        jmp     @r0
         nop
        .align  2
10:
        .endm

FUNC_START(cyg_hal_dcache_enable)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov     #CYGARC_REG_CCR_OCE,r2
        or      r2,r0
        mov.l   r0,@r1
        nop
        rts
         nop

FUNC_START(cyg_hal_dcache_disable)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov     #CYGARC_REG_CCR_OCE,r2
        not     r2,r2
        and     r2,r0
        mov.l   r0,@r1
        nop
        rts
         nop
        
FUNC_START(cyg_hal_dcache_invalidate_all)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov     #CYGARC_REG_CCR_OCI,r2
        or      r2,r0
        mov.l   r0,@r1
        nop                             ! Nothing in the docs suggest we need
        nop                             ! nops here, but without them, the
        nop                             ! CPU crashes.
        rts
         nop

FUNC_START(cyg_hal_dcache_sync)
        GOTO_NONCACHED_SHADOW
        mov.l   $CYGARC_REG_DCACHE_ADDRESS_FLUSH,r0
        mov.l   $CYGARC_REG_DCACHE_ADDRESS_BASE,r1
        mov.l   $CYGARC_REG_DCACHE_ADDRESS_TOP,r2
        mov.l   $CYGARC_REG_DCACHE_ADDRESS_STEP,r3
1:      cmp/hi  r1,r2
        bf      2f
        mov.l   r0,@r1
        bra     1b        
         add     r3,r1                  ! delay slot!
2:      nop
        rts
         nop

        .align  2
$CYGARC_REG_DCACHE_ADDRESS_FLUSH:
        .long   CYGARC_REG_DCACHE_ADDRESS_FLUSH
$CYGARC_REG_DCACHE_ADDRESS_BASE:
        .long   CYGARC_REG_DCACHE_ADDRESS_BASE
$CYGARC_REG_DCACHE_ADDRESS_TOP:
        .long   CYGARC_REG_DCACHE_ADDRESS_TOP
$CYGARC_REG_DCACHE_ADDRESS_STEP:
        .long   CYGARC_REG_DCACHE_ADDRESS_STEP


	! r4 = base
	! r5 = size
FUNC_START(cyg_hal_dcache_sync_region)
        GOTO_NONCACHED_SHADOW
1:	ocbp	@r4                     ! operand cache block purge
	add	#CYGARC_SH_MOD_DCAC_ADDRESS_STEP,r4
	add     #-CYGARC_SH_MOD_DCAC_ADDRESS_STEP,r5
        cmp/pl  r5
	bt	1b
        rts
         nop

FUNC_START(cyg_hal_dcache_write_mode)
        GOTO_NONCACHED_SHADOW
        # Mode argument in r4.
        # Read current state and mask out the two caching mode bits
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r3
        mov     #CYGARC_REG_CCR_CB|CYGARC_REG_CCR_WT,r2
	and	r2,r4
        not     r2,r2
        and     r2,r3
        # Or in the new settings and restore to CCR
        or      r4,r3
        mov.l   r3,@r1
        nop
        rts
         nop


FUNC_START(cyg_hal_icache_enable)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov.l   $nCYGARC_REG_CCR_ICE,r2
        or      r2,r0
        mov.l   r0,@r1
        nop
        rts
         nop

FUNC_START(cyg_hal_icache_disable)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov.l   $nCYGARC_REG_CCR_ICE,r2
        not     r2,r2
        and     r2,r0
        mov.l   r0,@r1
        nop
        rts
         nop
        
FUNC_START(cyg_hal_icache_invalidate_all)
        GOTO_NONCACHED_SHADOW
        mov.l   $nCYGARC_REG_CCR,r1
        mov.l   @r1,r0
        mov.l   $nCYGARC_REG_CCR_ICI,r2
        or      r2,r0
        mov.l   r0,@r1
        nop                             ! Nothing in the docs suggest we need
        nop                             ! nops here, but without them, the
        nop                             ! CPU crashes.
        rts
         nop

	.align	2
$MASK:
        .long   0x1fffffff              ! mask off top 3 bits
$BASE:
        .long   0xa0000000              ! base of non-cachable memory
$nCYGARC_REG_CCR:
	.long	CYGARC_REG_CCR        
$nCYGARC_REG_CCR_ICE:
	.long	CYGARC_REG_CCR_ICE
$nCYGARC_REG_CCR_ICI:
	.long	CYGARC_REG_CCR_ICI


        .data

SYM_DEF(cyg_hal_ILVL_table)
        # The first entries in the table have static priorities.

        .byte   0xf                     // NMI
        .byte   0xf                     // Reserved
        .byte   0xf                     // LVL0
        .byte   0xe                     // LVL1
        .byte   0xd                     // LVL2
        .byte   0xc                     // LVL3
        .byte   0xb                     // LVL4
        .byte   0xa                     // LVL5
        .byte   0x9                     // LVL6
        .byte   0x8                     // LVL7
        .byte   0x7                     // LVL8
        .byte   0x6                     // LVL9
        .byte   0x5                     // LVL10
        .byte   0x4                     // LVL11
        .byte   0x3                     // LVL12
        .byte   0x2                     // LVL13
        .byte   0x1                     // LVL14
        .byte   0xf                     // Reserved

        # The rest of the table consists of programmable levels, maintained
        # by the HAL_INTERRUPT_SET_LEVEL macro. 
        # These default to the highest level so that a spurious
        # interrupt cause the IPL to be suddenly lowered to allow all
        # interrupts. This should give a better chance at tracking down
        # the problem.
        .rept   (CYGNUM_HAL_ISR_MAX-CYGNUM_HAL_INTERRUPT_RESERVED_3E0)
        .byte   0xf
        .endr

        # All interrupts are masked initally. Set to 1 to enable.
SYM_DEF(cyg_hal_IMASK_table)
        .rept   (CYGNUM_HAL_ISR_MAX)
        .byte   0x0
        .endr
