//=============================================================================
//
//      xsengine_misc.c
//
//      Miscellaneous platform support for xsengine
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2005 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Knud Woehler <knud.woehler@microplex.de>
// Date:         2003-01-06
//
//####DESCRIPTIONEND####

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
#include <cyg/hal/hal_pxa2x0.h>
#include <cyg/hal/xsengine.h>           // Platform specifics
#include <cyg/infra/diag.h>             // diag_printf
#include <cyg/hal/hal_mm.h>

#include <string.h> // memset

void
hal_mmu_init(void)
{
    // Set up the translation tables at offset 0x4000
    unsigned long ttb_base = PXA2X0_RAM_BANK0_BASE + 0x4000;
    unsigned long i;

    // Set the TTB register
    asm volatile ("mcr  p15,0,%0,c2,c0,0" : : "r"(ttb_base) /*:*/);

    // Set the Domain Access Control Register
    i = ARM_ACCESS_DACR_DEFAULT;
    asm volatile ("mcr  p15,0,%0,c3,c0,0" : : "r"(i) /*:*/);

    // First clear all TT entries - ie Set them to Faulting
    memset((void *)ttb_base, 0, ARM_FIRST_LEVEL_PAGE_TABLE_SIZE);

    /*               Actual  Virtual  Size   Attributes                                                    Function  */
    /*	              Base     Base     MB      cached?           buffered?        access permissions                */
    /*             xxx00000  xxx00000                                                                                */
#define _CACHED   ARM_CACHEABLE
#define _UNCACHED ARM_UNCACHEABLE
#define _BUF      ARM_BUFFERABLE
#define _NOBUF    ARM_UNBUFFERABLE
#define _RWRW     ARM_ACCESS_PERM_RW_RW
    X_ARM_MMU_SECTION(0x000,  0x500,    32,  _UNCACHED, _NOBUF, _RWRW); /* Boot flash ROMspace */
    X_ARM_MMU_SECTION(0x040,  0x600,    32,  _UNCACHED, _NOBUF, _RWRW); /* LAN chip */
    X_ARM_MMU_SECTION(0x0C0,  0x700,    32,  _UNCACHED, _NOBUF, _RWRW); /* FPGA chip */
    X_ARM_MMU_SECTION(0xA00,  0x000,    64,  _CACHED,     _BUF, _RWRW); /* SDRAM Bank 0 */
    X_ARM_MMU_SECTION(0xA00,  0xC00,    64,  _UNCACHED,   _BUF, _RWRW); /* SDRAM Bank 0 */
    X_ARM_MMU_SECTION(0xE00,  0xE00,   128,  _CACHED,     _BUF, _RWRW); /* Zeros (Cache Clean) Bank */
    X_ARM_MMU_SECTION(0x400,  0x400,    64,  _UNCACHED, _NOBUF, _RWRW); /* Peripheral Registers */
    X_ARM_MMU_SECTION(0x440,  0x440,    64,  _UNCACHED, _NOBUF, _RWRW); /* LCD Registers */
    X_ARM_MMU_SECTION(0x480,  0x480,    64,  _UNCACHED, _NOBUF, _RWRW); /* Memory Ctl Registers */
}

//
// Platform specific initialization
//

void
plf_hardware_init(void)
{
    // RAM startup only - rewrite relevent bits depending on config
#ifndef CYG_HAL_STARTUP_ROM
    HAL_DCACHE_SYNC();            // Force data out
    HAL_DCACHE_INVALIDATE_ALL();  // Flush TLBs: make new mmu state effective
#endif // ! CYG_HAL_STARTUP_ROM - RAM start only

    hal_if_init();
}

// ------------------------------------------------------------------------
// EOF xsengine_misc.c
