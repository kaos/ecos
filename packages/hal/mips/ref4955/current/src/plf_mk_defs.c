//==========================================================================
//
//      plf_mk_defs.c
//
//      HAL (platform) "make defs" program
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
// Date:         2000-05-11
// Purpose:      Platform dependent definition generator
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
    // Some other exception related definitions
    DEFINE(CYGNUM_HAL_ISR_COUNT, CYGNUM_HAL_ISR_COUNT);
    DEFINE(CYGNUM_HAL_VSR_COUNT, CYGNUM_HAL_VSR_COUNT);
    DEFINE(CYGNUM_HAL_VECTOR_INTERRUPT, CYGNUM_HAL_VECTOR_INTERRUPT);
    DEFINE(CYGNUM_HAL_VECTOR_BREAKPOINT, CYGNUM_HAL_VECTOR_BREAKPOINT);

    // Interrupt details
    DEFINE(CYGNUM_HAL_INTERRUPT_INTC_V320USC_base, CYGNUM_HAL_INTERRUPT_INTC_V320USC_base);
    DEFINE(CYGNUM_HAL_INTERRUPT_INTC_PCI_base, CYGNUM_HAL_INTERRUPT_INTC_PCI_base);
    DEFINE(CYGNUM_HAL_INTERRUPT_INTC_IO_base, CYGNUM_HAL_INTERRUPT_INTC_IO_base);
    DEFINE(CYGARC_REG_INT_STAT, CYGARC_REG_INT_STAT);
    DEFINE(CYGARC_REG_PCI_STAT, CYGARC_REG_PCI_STAT);
    DEFINE(CYGARC_REG_IO_STAT, CYGARC_REG_IO_STAT);
    DEFINE(CYGARC_REG_PCI_MASK, CYGARC_REG_PCI_MASK);
    DEFINE(CYGARC_REG_IO_MASK, CYGARC_REG_IO_MASK);
    DEFINE(CYGARC_REG_INT_CFG0, CYGARC_REG_INT_CFG0);
    DEFINE(CYGARC_REG_INT_CFG3, CYGARC_REG_INT_CFG3);
#if defined(CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN)
    DEFINE(CYGNUM_HAL_INTERRUPT_CHAINING, CYGNUM_HAL_INTERRUPT_CHAINING);
#endif

    return 0;
}

//--------------------------------------------------------------------------
// EOF plf_mk_defs.c
