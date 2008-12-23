//==========================================================================
//
//      flash_erase_block.c
//
//      Flash programming
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andrew.lunn@ascom.ch
// Contributors: andrew.lunn
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include "synth.h"

#include <cyg/hal/hal_io.h>
#include <pkgconf/devs_flash_synth.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <string.h> // memset

#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif

int flash_erase_block(volatile flash_t *block, unsigned int block_size)
{
    unsigned int offset = (unsigned int) block;
    size_t remaining;
    int write_size;
    
    // This helps speed up the erasing
    static cyg_uint8 empty[4096];
    static cyg_bool empty_inited;
    
    offset -= (unsigned int) cyg_dev_flash_synth_base;

    cyg_hal_sys_lseek(cyg_dev_flash_synth_flashfd, offset,
                      CYG_HAL_SYS_SEEK_SET);
  
    if (!empty_inited) {
        memset(empty, 0xff, sizeof(empty));
        empty_inited = true;
    }
    
    remaining = flash_info.block_size;

    while (remaining) {
      write_size = MIN(remaining, sizeof(empty));
      cyg_hal_sys_write(cyg_dev_flash_synth_flashfd, empty, write_size);
      remaining -= write_size;
    }

    return FLASH_ERR_OK;
}
