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
// Purpose:      SH architecture dependent definition generator
// Description:  This file contains code that can be compiled by the target
//               compiler and used to generate machine specific definitions
//               suitable for use in assembly code.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_cache.h>          // HAL header
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
    DEFINE(CYGARC_JMPBUF_PR, offsetof(hal_jmp_buf_t, pr));
    DEFINE(CYGARC_JMPBUF_R8, offsetof(hal_jmp_buf_t, r8));
    DEFINE(CYGARC_JMPBUF_R9, offsetof(hal_jmp_buf_t, r9));
    DEFINE(CYGARC_JMPBUF_R10, offsetof(hal_jmp_buf_t, r10));
    DEFINE(CYGARC_JMPBUF_R11, offsetof(hal_jmp_buf_t, r11));
    DEFINE(CYGARC_JMPBUF_R12, offsetof(hal_jmp_buf_t, r12));
    DEFINE(CYGARC_JMPBUF_R13, offsetof(hal_jmp_buf_t, r13));
    DEFINE(CYGARC_JMPBUF_R14, offsetof(hal_jmp_buf_t, r14));

    // Exception/interrupt/context save buffer
    DEFINE(CYGARC_SHREG_REGS, offsetof(HAL_SavedRegisters, r[0]));
    DEFINE(CYGARC_SHREG_MACH, offsetof(HAL_SavedRegisters, mach));
    DEFINE(CYGARC_SHREG_MACL, offsetof(HAL_SavedRegisters, macl));
    DEFINE(CYGARC_SHREG_PR, offsetof(HAL_SavedRegisters, pr));
    DEFINE(CYGARC_SHREG_SR, offsetof(HAL_SavedRegisters, sr));
    DEFINE(CYGARC_SHREG_PC, offsetof(HAL_SavedRegisters, pc));
    DEFINE(CYGARC_SH_CONTEXT_SIZE, offsetof(HAL_SavedRegisters, context_size));

    // Below only saved on exceptions/interrupts
    DEFINE(CYGARC_SHREG_VBR, offsetof(HAL_SavedRegisters, vbr));
    DEFINE(CYGARC_SHREG_GBR, offsetof(HAL_SavedRegisters, gbr));
    DEFINE(CYGARC_SHREG_EVENT, offsetof(HAL_SavedRegisters, event));
    DEFINE(CYGARC_SH_EXCEPTION_SIZE, sizeof(HAL_SavedRegisters));

    DEFINE(CYGARC_SH_EXCEPTION_DECREMENT, sizeof(HAL_SavedRegisters));

    // Some other exception related definitions
    DEFINE(CYGNUM_HAL_ISR_COUNT, CYGNUM_HAL_ISR_COUNT);
    DEFINE(CYGNUM_HAL_VECTOR_INTERRUPT, CYGNUM_HAL_VECTOR_INTERRUPT);
    DEFINE(CYGNUM_HAL_VSR_MAX, CYGNUM_HAL_VSR_MAX);
    DEFINE(CYGNUM_HAL_VSR_COUNT, CYGNUM_HAL_VSR_COUNT);
    DEFINE(CYGNUM_HAL_VSR_EXCEPTION_COUNT, CYGNUM_HAL_VSR_EXCEPTION_COUNT);

    // Caching details
    DEFINE(CYGARC_REG_CACHE_ADDRESS_FLUSH, CYGARC_REG_CACHE_ADDRESS_FLUSH);
    DEFINE(CYGARC_REG_CACHE_ADDRESS_BASE,CYGARC_REG_CACHE_ADDRESS_BASE);
    DEFINE(CYGARC_REG_CACHE_ADDRESS_TOP,CYGARC_REG_CACHE_ADDRESS_TOP);
    DEFINE(CYGARC_REG_CACHE_ADDRESS_STEP,CYGARC_REG_CACHE_ADDRESS_STEP);
    DEFINE(HAL_UCACHE_SIZE, HAL_UCACHE_SIZE);
    DEFINE(HAL_UCACHE_WAYS, HAL_UCACHE_WAYS);
    DEFINE(HAL_UCACHE_LINE_SIZE, HAL_UCACHE_LINE_SIZE);


    // Variant definitions - want these to be included instead.
}

//--------------------------------------------------------------------------
// EOF hal_mk_defs.c
