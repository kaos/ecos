/* =================================================================
 *
 *      relocate_i386.c
 *
 *      Relocation types for the i386 processor.
 *
 * ================================================================= 
 * ####ECOSGPLCOPYRIGHTBEGIN####                                     
 * -------------------------------------------                       
 * This file is part of eCos, the Embedded Configurable Operating System.
 * Copyright (C) 2005, 2008 Free Software Foundation, Inc.                 
 *
 * eCos is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 or (at your option) any later
 * version.                                                          
 *
 * eCos is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.                                                 
 *
 * You should have received a copy of the GNU General Public License 
 * along with eCos; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.     
 *
 * As a special exception, if other files instantiate templates or use
 * macros or inline functions from this file, or you compile this file
 * and link it with other works to produce a work based on this file,
 * this file does not by itself cause the resulting work to be covered by
 * the GNU General Public License. However the source code for this file
 * must still be made available in accordance with section (3) of the GNU
 * General Public License v2.                                        
 *
 * This exception does not invalidate any other reasons why a work based
 * on this file might be covered by the GNU General Public License.  
 * -------------------------------------------                       
 * ####ECOSGPLCOPYRIGHTEND####                                       
 * =================================================================
 * #####DESCRIPTIONBEGIN####
 * 
 *  Author(s):    Anthony Tonizzo (atonizzo@gmail.com), andrew.lunn@ascom.ch
 *  Date:         2005-07-07
 *  Purpose:      
 *  Description:  
 *               
 * ####DESCRIPTIONEND####
 * 
 * =================================================================
 */

#include <cyg/infra/diag.h>     // For diagnostic printing.
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>

#include <pkgconf/objloader.h>
#include <cyg/objloader/elf.h>
#include <cyg/objloader/objelf.h>

#if CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL > 1
// Always 16 characters long, with blank padding is necessary, so
//  the printing is pretty. If the name is longer than 16, shorten it.
// We print the relocation symbols only is 
//  CYGPKG_SERVICES_OBJLOADER_DEBUG_LEVEL is set to 2.
char *relocation_name[] =
{
    "", "R_386_32        ", "R_386_PC32      "
};
#endif

#if defined(CYGPKG_HAL_I386) || defined(CYGPKG_HAL_SYNTH_I386)
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
  cyg_int32  i;
  
  // PPC uses rela, so we have to add the addend.
  switch(sym_type)
    {
      case R_386_32:
        HAL_READ_UINT32(mem , i);
        HAL_WRITE_UINT32(mem, i + sym_value );
      return 0;
      case R_386_PC32:
        HAL_READ_UINT32(mem , i);
        HAL_WRITE_UINT32(mem,  i + sym_value - mem);
        return 0;
      default:
        diag_printf("FIXME: Unknown relocation value!!!\n");
        return -1;
    }
}
#endif // CYGPKG_HAL_I386 || CYGPKG_HAL_SYNTH_I386


