//==========================================================================
//
//      plf_mk_defs.c
//
//      "make defs" program for M5272C3 platform
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2002 Gary Thomas
// Copyright (C) 2006 eCosCentric Ltd.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Enrico Piria
// Contributors: gthomas, jskov
// Date:         2005-25-06
// Purpose:      Definition generator for M5272C3 board.
// Description:  This file contains code that can be compiled by the target
//               compiler and used to generate machine specific definitions
//               suitable for use in assembly code.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include CYGHWR_MEMORY_LAYOUT_H     

// This program is used to generate definitions needed by
// assembly language modules.
//
// This technique was first used in the OSF Mach kernel code:
// generate asm statements containing #defines,
// compile this file to assembler, and then extract the
// #defines from the assembly-language output.

#define DEFINE(sym, val) \
        asm volatile("\n\t.equ\t" #sym ",%0" : : "i" (val))

int
main(void)
{
    // Memory layout values
    DEFINE(CYGMEM_REGION_sdram, CYGMEM_REGION_sdram);
    DEFINE(CYGMEM_REGION_sdram_SIZE, CYGMEM_REGION_sdram_SIZE);
    DEFINE(CYGMEM_REGION_devs, CYGMEM_REGION_devs);
    DEFINE(CYGMEM_REGION_devs_SIZE, CYGMEM_REGION_devs_SIZE);
    DEFINE(CYGMEM_REGION_sram, CYGMEM_REGION_sram);
    DEFINE(CYGMEM_REGION_sram_SIZE, CYGMEM_REGION_sram_SIZE);
    DEFINE(CYGMEM_REGION_flash, CYGMEM_REGION_flash);
    DEFINE(CYGMEM_REGION_flash_SIZE, CYGMEM_REGION_flash_SIZE);
    
    return 0;
}

// -------------------------------------------------------------------------
// EOF hal_mk_defs.c
