//==========================================================================
//
//      var_mk_defs.c
//
//      HAL (variant) "make defs" program
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
// Date:         2001-06-08
// Purpose:      Variant dependent definition generator
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
    // Cache stuff
    DEFINE(HAL_DCACHE_SIZE, HAL_DCACHE_SIZE);
    DEFINE(HAL_ICACHE_SIZE, HAL_ICACHE_SIZE);
    return 0;
}

//--------------------------------------------------------------------------
// EOF var_mk_defs.c
