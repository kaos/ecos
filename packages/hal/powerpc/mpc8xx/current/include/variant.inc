#ifndef CYGONCE_HAL_VARIANT_INC
#define CYGONCE_HAL_VARIANT_INC
##=============================================================================
##
##	variant.inc
##
##	MPC8xx family assembler header file
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
## Author(s): 	jskov
## Contributors:jskov
## Date:	2000-02-04
## Purpose:	MPC8xx family definitions.
## Description:	This file contains various definitions and macros that are
##              useful for writing assembly code for the MPC8xx CPU family.
## Usage:
##		#include <cyg/hal/variant.inc>
##		...
##		
##
######DESCRIPTIONEND####
##
##=============================================================================

#include <pkgconf/hal.h>
	
#include <cyg/hal/arch.inc>

##-----------------------------------------------------------------------------
## MPC8xx defined vectors

        .macro hal_extra_vectors
        # MPC8xx vectors
        exception_vector        software_emu
        exception_vector        instruction_tlb_miss
        exception_vector        data_tlb_miss
        exception_vector        instruction_tlb_error
        exception_vector        data_tlb_error
        exception_vector        reserved_01500
        exception_vector        reserved_01600
        exception_vector        reserved_01700
        exception_vector        reserved_01800
        exception_vector        reserved_01900
        exception_vector        reserved_01A00
        exception_vector        reserved_01B00
        exception_vector        data_breakpoint
        exception_vector        instruction_breakpoint
        exception_vector        peripheral_breakpoint
        exception_vector        NMI_port
        .endm
        
##-----------------------------------------------------------------------------
## MPC8xx CPU initialization
##
## Initialize CPU to a post-reset state, ensuring the ground doesn''t
## shift under us while we try to set things up.

        .macro hal_cpu_init
        # Disable special MPC8xx "development support" which
	# suppresses trace exceptions. The CPU seems to hang, not 
        # executing from offset 0x1e00(?) as expected.
        lwi	r3,0x00000007
        mtspr	ICTRL,r3
        li      r3,0
        mtspr   DER, r3

        # Disable caches
        lwi     r3,CYGARC_REG_DC_CMD_CD
        sync
        mtspr   CYGARC_REG_DC_CST,r3
        lwi     r3,CYGARC_REG_IC_CMD_CD
        isync
        mtspr   CYGARC_REG_IC_CST,r3
        isync

        # Default caching to off when MMU is disabled
        mfspr   r3,CYGARC_REG_MI_CTR
        oris    r3,r3,(CYGARC_REG_MI_CTR_CIDEF)@h
        mtspr   CYGARC_REG_MI_CTR,r3
        mfspr   r3,CYGARC_REG_MD_CTR
        oris    r3,r3,(CYGARC_REG_MD_CTR_CIDEF)@h
        mtspr   CYGARC_REG_MD_CTR,r3
        
        # Set up MSR (disable MMU for now)
        lwi     r3,(CYG_MSR & ~(MSR_IR | MSR_DR))
        sync
        mtmsr   r3
        sync
        .endm
        
##-----------------------------------------------------------------------------
## MPC8xx exception state handling
        .macro	hal_variant_save regs
        .endm

        .macro	hal_variant_load regs
        .endm
        
##-----------------------------------------------------------------------------
## MPC8xx monitor initialization

#ifndef CYGPKG_HAL_PPC_MON_DEFINED

#if     defined(CYG_HAL_STARTUP_ROM) ||                 \
        (       defined(CYG_HAL_STARTUP_RAM) &&         \
                !defined(CYGSEM_HAL_USE_ROM_MONITOR))

        .macro  hal_mon_init
#ifdef CYGSEM_HAL_POWERPC_COPY_VECTORS
        # If we are starting up from ROM and want vectors in RAM
        # or we are starting in RAM and NOT using a ROM monitor,
        # copy exception handler code to 0.
        lwi     r3,rom_vectors          # r3 = rom start
        lwi     r4,0                    # r4 = ram start
        lwi     r5,rom_vectors_end      # r5 = rom end
        cmplw   r3,r5                   # skip if no vectors
        beq     2f

        subi    r3,r3,4
        subi    r4,r4,4
        subi    r5,r5,4
1:
        lwzu    r0,4(r3)                # get word from ROM
        stwu    r0,4(r4)                # store in RAM
        cmplw   r3,r5                   # compare
        blt     1b                      # loop if not yet done
2:
#endif

        # Next initialize the VSR table. This happens whether the
        # vectors were copied to RAM or not.

        # First fill with exception handlers
        lwi     r3,cyg_hal_default_exception_vsr
        lwi     r4,hal_vsr_table
        subi    r4,r4,4
        li      r5,CYGNUM_HAL_VSR_COUNT
1:      stwu    r3,4(r4)
        subi    r5,r5,1
        cmpwi   r5,0
        bne     1b

        # Then fill in the special vectors
        lwi     r3,cyg_hal_default_interrupt_vsr
        lwi     r4,hal_vsr_table
        stw     r3,CYGNUM_HAL_VECTOR_INTERRUPT*4(r4)
        stw     r3,CYGNUM_HAL_VECTOR_DECREMENTER*4(r4)
        .endm

#elif defined(CYG_HAL_STARTUP_RAM) && defined(CYGSEM_HAL_USE_ROM_MONITOR)

        # Initialize the VSR table entries
        # We only take control of the interrupt vectors,
        # the rest are left to the ROM for now...

        .macro  hal_mon_init
        lwi     r3,cyg_hal_default_interrupt_vsr
        lwi     r4,hal_vsr_table
        stw     r3,CYGNUM_HAL_VECTOR_INTERRUPT*4(r4)
        stw     r3,CYGNUM_HAL_VECTOR_DECREMENTER*4(r4)
        .endm


#else

        .macro  hal_mon_init

        .endm

#endif


#define CYGPKG_HAL_PPC_MON_DEFINED

#endif // CYGPKG_HAL_PPC_MON_DEFINED




##-----------------------------------------------------------------------------
## Indicate that the ISR tables are defined in variant.S

#define CYG_HAL_PPC_ISR_TABLES_DEFINED

##-----------------------------------------------------------------------------
## MPC8xx interrupt handling.

#ifndef CYGPKG_HAL_POWERPC_INTC_DEFINED

## First level decoding of MPC8xx SIU interrupt controller.

        # decode the interrupt
	.macro  hal_intc_decode dreg,state
        lwz     \dreg,CYGARC_PPCREG_VECTOR(\state) # retrieve vector number,
        rlwinm. \dreg,\dreg,22,31,31            # isolate bit 21
        beq     0f                              # done if decrementer (vec 0)
        lwi     \dreg,CYGARC_REG_IMM_SIVEC      # if external, get SIU
        lbz     \dreg,0(\dreg)                  # vector.
        srwi    \dreg,\dreg,2
        addi    \dreg,\dreg,1                   # Skip decrementer vector
0:      stw     \dreg,CYGARC_PPCREG_VECTOR(\state) # update vector in state frame.
        slwi    \dreg,\dreg,2                   # convert to byte offset.
        .endm                              


#define CYGPKG_HAL_POWERPC_INTC_DEFINED
#endif // CYGPKG_HAL_POWERPC_INTC_DEFINED

#------------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VARIANT_INC
# end of variant.inc
