/* =================================================================
 *
 *      relocate_arm.c
 *
 *      Relocation types for the ARM processor (Little Endian).
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating System.
 * Copyright (C) 2008, 2009 Free Software Foundation, Inc.
 * 
 * eCos is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 or (at your option)
 * any later version.
 * 
 * eCos is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with eCos; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
 * 
 * As a special exception, if other files instantiate templates or
 * use macros or inline functions from this file, or you compile this
 * file and link it with other works to produce a work based on this
 * file, this file does not by itself cause the resulting work to be
 * covered by the GNU General Public License. However the source code
 * for this file must still be made available in accordance with
 * section (3) of the GNU General Public License.
 * 
 * This exception does not invalidate any other reasons why a work
 * based on this file might be covered by the GNU General Public
 * License.
 *
 * -------------------------------------------
 * ####ECOSGPLCOPYRIGHTEND####
 * =================================================================
 * #####DESCRIPTIONBEGIN####
 * 
 *  Author(s):    Anthony Tonizzo (atonizzo@gmail.com)
 *  Contributors: Sergei Gavrikov (sergei.gavrikov@gmail.com)
 *  Date:         2008-12-01
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

#ifdef CYGPKG_HAL_ARM
void
cyg_ldr_flush_cache(void)
{
    HAL_DCACHE_SYNC();
    HAL_ICACHE_SYNC();
}

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
// Always 16 characters long, with blank padding is necessary, so
//  the printing is pretty. If the name is longer than 16, shorten it.
// We print the relocation symbols only is 
//  CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL is set to 2.
char *relocation_name[] =
{
    "", "R_ARM_PC24      ", "R_ARM_ABS32     ", "", "", "", "", "", "", "",
    "", "", "", "", "", "", "", "", "", "", "", "", "", "", "", "",
    "", "", "R_ARM_CALL      ", "R_ARM_JUMP24    ", "", "", "", "",
    "", "", "", "", "", "", "R_ARM_V4BX      "
};
#endif

// sym_type  Type of relocation to apply,
// mem       Address in memory to modify (relocate).
// sym_value The value of the symbol to use for the relocation.
// The proper relocation to apply (i.e. the proper use of mem and sym_value)
//  depend on the relocation to apply. The number and types of relocations
//  that must be supported by any given architecture is spelled in the ELF/EABI
//  guide for that architecture.
cyg_int32 
cyg_ldr_relocate(cyg_int32 sym_type, cyg_uint32 mem, cyg_int32 sym_value)
{
    cyg_int32 offset;
    volatile cyg_uint32 *mem_addr = (cyg_uint32 *)mem;

    switch(sym_type)
    {
    case R_ARM_ABS32:
        offset = *mem_addr;
        *mem_addr = offset + sym_value;
        break;
    case R_ARM_PC24:
    case R_ARM_CALL:
    case R_ARM_JUMP24:
        offset = (*mem_addr & 0x00FFFFFF) << 2;
        if (offset & 0x02000000)
            offset -= 0x04000000;     // Sign extend.
        *mem_addr &= 0xff000000;      // Mask off the entire offset bits.
        offset = sym_value - mem + offset;  // This is the new offset.
        if ((offset & 0x03) || (offset >= (cyg_int32)0x04000000) ||
                                (offset <= (cyg_int32)0xFC000000))
            return -1;                                     
        *mem_addr |= (offset >> 2) & 0x00FFFFFF;
        break;
    case R_ARM_V4BX:
        // For now only ARMv4T and later cores (with Thumb) are supported.
        break;
    default:
        CYG_ASSERT(0, ("FIXME: Unknown relocation value!!!\r\n"));
        return -1;
    }
    return 0;
}

#endif // CYGPKG_HAL_ARM


