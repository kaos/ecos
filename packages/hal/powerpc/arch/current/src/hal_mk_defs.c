//==========================================================================
//
//      hal_mk_defs.c
//
//      HAL (architecture) "make defs" program
//
//==========================================================================
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas, jskov
// Date:         2000-02-21
// Purpose:      PowerPC architecture dependent definition generator
// Description:  This file contains code that can be compiled by the target
//               compiler and used to generate machine specific definitions
//               suitable for use in assembly code.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include CYGHWR_MEMORY_LAYOUT_H     

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
# include <cyg/kernel/instrmnt.h>
#endif

/*
 * This program is used to generate definitions needed by
 * assembly language modules.
 *
 * This technique was first used in the OSF Mach kernel code:
 * generate asm statements containing #defines,
 * compile this file to assembler, and then extract the
 * #defines from the assembly-language output.
 */

#define DEFINE(sym, val) \
        asm volatile("\n\t.equ\t" #sym ",%0" : : "i" (val))

int
main(void)
{
    // setjmp/longjmp buffer
    DEFINE(CYGARC_JMPBUF_SP, offsetof(hal_jmp_buf_t, sp));
    DEFINE(CYGARC_JMPBUF_R2, offsetof(hal_jmp_buf_t, r2));
    DEFINE(CYGARC_JMPBUF_R13, offsetof(hal_jmp_buf_t, r13));
    DEFINE(CYGARC_JMPBUF_R14, offsetof(hal_jmp_buf_t, r14));
    DEFINE(CYGARC_JMPBUF_R15, offsetof(hal_jmp_buf_t, r15));
    DEFINE(CYGARC_JMPBUF_R16, offsetof(hal_jmp_buf_t, r16));
    DEFINE(CYGARC_JMPBUF_R17, offsetof(hal_jmp_buf_t, r17));
    DEFINE(CYGARC_JMPBUF_R18, offsetof(hal_jmp_buf_t, r18));
    DEFINE(CYGARC_JMPBUF_R19, offsetof(hal_jmp_buf_t, r19));
    DEFINE(CYGARC_JMPBUF_R20, offsetof(hal_jmp_buf_t, r20));
    DEFINE(CYGARC_JMPBUF_R21, offsetof(hal_jmp_buf_t, r21));
    DEFINE(CYGARC_JMPBUF_R22, offsetof(hal_jmp_buf_t, r22));
    DEFINE(CYGARC_JMPBUF_R23, offsetof(hal_jmp_buf_t, r23));
    DEFINE(CYGARC_JMPBUF_R24, offsetof(hal_jmp_buf_t, r24));
    DEFINE(CYGARC_JMPBUF_R25, offsetof(hal_jmp_buf_t, r25));
    DEFINE(CYGARC_JMPBUF_R26, offsetof(hal_jmp_buf_t, r26));
    DEFINE(CYGARC_JMPBUF_R27, offsetof(hal_jmp_buf_t, r27));
    DEFINE(CYGARC_JMPBUF_R28, offsetof(hal_jmp_buf_t, r28));
    DEFINE(CYGARC_JMPBUF_R29, offsetof(hal_jmp_buf_t, r29));
    DEFINE(CYGARC_JMPBUF_R30, offsetof(hal_jmp_buf_t, r30));
    DEFINE(CYGARC_JMPBUF_R31, offsetof(hal_jmp_buf_t, r31));
    DEFINE(CYGARC_JMPBUF_LR, offsetof(hal_jmp_buf_t, lr));
    DEFINE(CYGARC_JMPBUF_CR, offsetof(hal_jmp_buf_t, cr));

    // Exception/interrupt/context save buffer
#ifdef CYGDBG_HAL_POWERPC_FRAME_WALLS
    DEFINE(CYGARC_PPCREG_WALL_HEAD, offsetof(HAL_SavedRegisters, wall_head));
    DEFINE(CYGARC_PPCREG_WALL_TAIL, offsetof(HAL_SavedRegisters, wall_tail));
#endif
    DEFINE(CYGARC_PPCREG_REGS, offsetof(HAL_SavedRegisters, d[0]));
    DEFINE(CYGARC_PPCREG_CR, offsetof(HAL_SavedRegisters, cr));
    DEFINE(CYGARC_PPCREG_XER, offsetof(HAL_SavedRegisters, xer));
    DEFINE(CYGARC_PPCREG_LR, offsetof(HAL_SavedRegisters, lr));
    DEFINE(CYGARC_PPCREG_CTR, offsetof(HAL_SavedRegisters, ctr));
    DEFINE(CYGARC_PPCREG_MSR, offsetof(HAL_SavedRegisters, msr));
    DEFINE(CYGARC_PPCREG_PC, offsetof(HAL_SavedRegisters, pc));
    DEFINE(CYGARC_PPC_CONTEXT_SIZE, offsetof(HAL_SavedRegisters, context_size));
    // Below only saved on exceptions/interrupts
    DEFINE(CYGARC_PPCREG_VECTOR, offsetof(HAL_SavedRegisters, vector));
    DEFINE(CYGARC_PPC_EXCEPTION_SIZE, sizeof(HAL_SavedRegisters));

    DEFINE(CYGARC_PPC_STACK_FRAME_SIZE, CYGARC_PPC_STACK_FRAME_SIZE);
    DEFINE(CYGARC_PPC_EXCEPTION_DECREMENT, sizeof(HAL_SavedRegisters));

    // Some other exception related definitions
    DEFINE(CYGNUM_HAL_ISR_COUNT, CYGNUM_HAL_ISR_COUNT);
    DEFINE(CYGNUM_HAL_VECTOR_INTERRUPT, CYGNUM_HAL_VECTOR_INTERRUPT);
    DEFINE(CYGNUM_HAL_VECTOR_DECREMENTER, CYGNUM_HAL_VECTOR_DECREMENTER);
    DEFINE(CYGNUM_HAL_VSR_MAX, CYGNUM_HAL_VSR_MAX);
    DEFINE(CYGNUM_HAL_VSR_COUNT, CYGNUM_HAL_VSR_COUNT);

    // Variant definitions - want these to be included instead.
#ifdef CYGARC_VARIANT_DEFS
    CYGARC_VARIANT_DEFS
#endif

    // Memory layout values (since these aren't "asm"-safe)
#ifdef CYGMEM_REGION_rom    
    DEFINE(CYGMEM_REGION_rom, CYGMEM_REGION_rom);
#endif
#ifdef CYGMEM_REGION_ram    
    DEFINE(CYGMEM_REGION_ram, CYGMEM_REGION_ram);
#endif
}

//--------------------------------------------------------------------------
// EOF hal_mk_defs.c
