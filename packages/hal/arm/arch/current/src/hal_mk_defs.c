/*==========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-02-20
// Purpose:      ARM architecture dependent definition generator
// Description:  This file contains code that can be compiled by the target
//               compiler and used to generate machine specific definitions
//               suitable for use in assembly code.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
# include <cyg/kernel/instrmnt.h>
#endif
#include <cyg/hal/hal_if.h>

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
    DEFINE(armreg_r0, offsetof(HAL_SavedRegisters, d[HAL_THREAD_CONTEXT_R0]));
    DEFINE(armreg_r4, offsetof(HAL_SavedRegisters, d[HAL_THREAD_CONTEXT_R4]));
    DEFINE(armreg_sp, offsetof(HAL_SavedRegisters, sp));
    DEFINE(armreg_fp, offsetof(HAL_SavedRegisters, fp));
    DEFINE(armreg_ip, offsetof(HAL_SavedRegisters, ip));
    DEFINE(armreg_lr, offsetof(HAL_SavedRegisters, lr));
    DEFINE(armreg_pc, offsetof(HAL_SavedRegisters, pc));
    DEFINE(armreg_cpsr, offsetof(HAL_SavedRegisters, cpsr));
    DEFINE(armreg_vector, offsetof(HAL_SavedRegisters, vector));
    DEFINE(ARMREG_SIZE, sizeof(HAL_SavedRegisters));
    DEFINE(CYGNUM_HAL_ISR_COUNT, CYGNUM_HAL_ISR_COUNT);
    DEFINE(CYGNUM_HAL_VSR_COUNT, CYGNUM_HAL_VSR_COUNT);
    DEFINE(CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION,
           CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION);
    DEFINE(CYGNUM_HAL_EXCEPTION_INTERRUPT, CYGNUM_HAL_EXCEPTION_INTERRUPT);
    DEFINE(CYGNUM_HAL_EXCEPTION_CODE_ACCESS,
           CYGNUM_HAL_EXCEPTION_CODE_ACCESS);
    DEFINE(CYGNUM_HAL_EXCEPTION_DATA_ACCESS,
           CYGNUM_HAL_EXCEPTION_DATA_ACCESS);
    DEFINE(CYGNUM_HAL_VECTOR_IRQ, CYGNUM_HAL_VECTOR_IRQ);
#ifdef CYGPKG_KERNEL
    DEFINE(RAISE_INTR, CYG_INSTRUMENT_CLASS_INTR|CYG_INSTRUMENT_EVENT_INTR_RAISE);
#endif
    DEFINE(CPSR_IRQ_DISABLE, CPSR_IRQ_DISABLE);
    DEFINE(CPSR_FIQ_DISABLE, CPSR_FIQ_DISABLE);
    DEFINE(CPSR_THUMB_ENABLE, CPSR_THUMB_ENABLE);
    DEFINE(CPSR_IRQ_MODE, CPSR_IRQ_MODE);
    DEFINE(CPSR_FIQ_MODE, CPSR_FIQ_MODE);
    DEFINE(CPSR_SUPERVISOR_MODE, CPSR_SUPERVISOR_MODE);
    DEFINE(CPSR_UNDEF_MODE, CPSR_UNDEF_MODE);
    DEFINE(CPSR_MODE_BITS, CPSR_MODE_BITS);
    DEFINE(CPSR_INITIAL, CPSR_INITIAL);
    DEFINE(CPSR_THREAD_INITIAL, CPSR_THREAD_INITIAL);
#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT)
    DEFINE(CYGNUM_CALL_IF_TABLE_SIZE, CYGNUM_CALL_IF_TABLE_SIZE);
#endif
    DEFINE(CYGNUM_HAL_INTERRUPT_NONE, CYGNUM_HAL_INTERRUPT_NONE);

    return 0;
}


/*------------------------------------------------------------------------*/
// EOF hal_mk_defs.c
