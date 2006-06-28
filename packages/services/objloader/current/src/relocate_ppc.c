/* =================================================================
 *
 *      relocate_ppc.c
 *
 *      Relocation types for the PowerPC processor.
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####
 * -------------------------------------------
 * This file is part of eCos, the Embedded Configurable Operating
 * System.
 * Copyright (C) 2005 eCosCentric Ltd.
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
 *  Date:         2005-05-13
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <pkgconf/io_fileio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

#ifdef CYGPKG_HAL_POWERPC
void
cyg_ldr_flush_cache(void)
{
    HAL_DCACHE_SYNC();
    HAL_ICACHE_SYNC();
}

// in:
// 
// sym_type  Type of relocation to apply,
// mem       Address in memory to modify (relocate).
// sym_value 
cyg_int32 
cyg_ldr_relocate(cyg_int32 sym_type, cyg_uint32 mem, cyg_int32 sym_value)
{
    cyg_int32  rel_offset, i;

    // PPC uses rela, so we have to add the addend.
    switch(sym_type)
    {
    case R_PPC_ADDR16_HA:
        HAL_WRITE_UINT16(mem, _ha_(sym_value));
        return 0;
    case R_PPC_ADDR16_HI:
        HAL_WRITE_UINT16(mem, _hi_(sym_value));
        return 0;
    case R_PPC_ADDR16_LO:
        HAL_WRITE_UINT16(mem, _lo_(sym_value));
        return 0;
    case R_PPC_REL24:
        // Now it is time to seek the destination address of the call.
        // We need to do something in case the user jumps more than 16MB.
        rel_offset = (sym_value - mem) & 0x03FFFFFC; 
        HAL_READ_UINT32(mem, i);
        i &= 0xFC000003;
        HAL_WRITE_UINT32(mem, rel_offset | i);
        return 0;
    case R_PPC_REL32:
        HAL_WRITE_UINT32(mem, (sym_value - mem));
        return 0;
    case R_PPC_ADDR32:
        HAL_WRITE_UINT32(mem, sym_value);
        return 0;
    default:
        ELFDEBUG("FIXME: Unknown relocation value!!!\n");
        return -1;
    }
}
#endif // CYGPKG_HAL_POWERPC


