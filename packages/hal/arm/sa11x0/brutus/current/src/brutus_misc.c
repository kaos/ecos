//==========================================================================
//
//      brutus_misc.c
//
//      HAL misc board support code for StrongARM SA1110/Brutus
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
// Contributors: hmt
//               Travis C. Furrer <furrer@mit.edu>
// Date:         2000-05-21
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt names
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_sa11x0.h>         // Hardware definitions
#include <cyg/hal/brutus.h>             // Platform specifics

#include <cyg/infra/diag.h>             // diag_printf

// All the MM table layout is here:
#include <cyg/hal/hal_mm.h>

void
hal_mmu_init(void)
{
    unsigned long ttb_base = SA11X0_RAM_BANK0_BASE + 0x4000;
    unsigned long i;

    /*
     * Set the TTB register
     */
    asm volatile ("mcr  p15,0,%0,c2,c0,0" : : "r"(ttb_base) /*:*/);

    /*
     * Set the Domain Access Control Register
     */
    i = ARM_ACCESS_DACR_DEFAULT;
    asm volatile ("mcr  p15,0,%0,c3,c0,0" : : "r"(i) /*:*/);

    /*
     * First clear all TT entries - ie Set them to Faulting
     */
    memset((void *)ttb_base, 0, ARM_FIRST_LEVEL_PAGE_TABLE_SIZE);

    /*               Actual  Virtual  Size   Attributes                                                    Function  */
    /*		     Base     Base     MB      cached?           buffered?        access permissions                 */
    /*             xxx00000  xxx00000                                                                                */
    X_ARM_MMU_SECTION(0x000,  0x400,     1,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* Boot flash ROMspace */
    X_ARM_MMU_SECTION(0x080,  0x080,     4,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* Application flash ROM */
    X_ARM_MMU_SECTION(0x100,  0x100,     1,  ARM_CACHEABLE,   ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Static RAM - 512K */
    X_ARM_MMU_SECTION(0x180,  0x180,     1,  ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* Static select 3 */
    X_ARM_MMU_SECTION(0x200,  0x200,   256,  ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PCMCIA Socket 0 */
    X_ARM_MMU_SECTION(0x300,  0x300,   256,  ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* PCMCIA Sockets 1 */
    X_ARM_MMU_SECTION(0x800,  0x800, 0x400,  ARM_UNCACHEABLE, ARM_UNBUFFERABLE, ARM_ACCESS_PERM_RW_RW); /* StrongARM(R) Registers */
    X_ARM_MMU_SECTION(0xC00,  0x000,     4,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* DRAM Bank 0 */
    X_ARM_MMU_SECTION(0xC80,  0x004,     4,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* DRAM Bank 1 */
    X_ARM_MMU_SECTION(0xD00,  0x008,     4,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* DRAM Bank 2 */
    X_ARM_MMU_SECTION(0xD80,  0x00C,     4,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* DRAM Bank 3 */
    X_ARM_MMU_SECTION(0xC00,  0xC00,   256,  ARM_UNCACHEABLE, ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* DRAM Banks - raw access */
    X_ARM_MMU_SECTION(0xE00,  0xE00,   128,  ARM_CACHEABLE,   ARM_BUFFERABLE,   ARM_ACCESS_PERM_RW_RW); /* Zeros (Cache Clean) Bank */

}

//
// Platform specific initialization
//

void
plf_hardware_init(void)
{
}

#include CYGHWR_MEMORY_LAYOUT_H
typedef void code_fun(void);
void brutus_program_new_stack(void *func)
{
    register CYG_ADDRESS stack_ptr asm("sp");
    register CYG_ADDRESS old_stack asm("r4");
    register code_fun *new_func asm("r0");
    old_stack = stack_ptr;
    stack_ptr = CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE - sizeof(CYG_ADDRESS);
    new_func = (code_fun*)func;
    new_func();
    stack_ptr = old_stack;
    return;
}

// ------------------------------------------------------------------------
// EOF brutus_misc.c
