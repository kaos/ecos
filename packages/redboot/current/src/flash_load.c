//==========================================================================
//
//      flash_load.c
//
//      RedBoot file/image loader into flash
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric LTD
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
// Author(s):    Oliver Munz
// Contributors: om, asl
// Date:         2006-02-21
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <flash_load.h>

#include <cyg/io/flash.h>
#include <cyg/infra/cyg_ass.h>

static int flash_block_size;

static cyg_uint8 * current_flash_page;

/* Allocation of the flash-sector size RAM-buffer is done */
static bool init_done = false; 

/* We have initialized the current page ready for writing */
static bool flash_page_init = false; 

static cyg_uint8 *flash_buffer;

// If the io flash code outputs when erasing/writing it will upset the
// download over the communications channel. So we install a dummy
// print function.
static int dummy_printf(const char *fmt, ...){ 
  return 0;
}

// Calculate the address of the first byte in a flash block
static cyg_uint8 * flash_block_begin(cyg_uint32 addr) 
{
  return (cyg_uint8 *)
    ((addr / flash_block_size) * flash_block_size);
}

// Initialize the loading process
void flash_load_start(void)
{
  flash_init(dummy_printf);
  
  init_done = true;
  flash_page_init = false;
}

// Write a byte into flash. We maintain a copy in RAM of the FLASH
// page we are currently "writing" into. This copy is loaded with the
// current contents of the FLASH page when the first byte is "written"
// to the page. The "writes" are then made into the RAM copy. We only
// write to FLASH when there is a "write" outside of the current page,
// or the flash_load_finish function is called.
void flash_load_write(cyg_uint8 *flash_addr, cyg_uint8 value)
{
  
  cyg_uint32 retcode = FLASH_ERR_OK;
  void * err_addr;
  cyg_uint32 addr = (cyg_uint32)flash_addr;
  cyg_uint32 offset;
  
  if (!flash_page_init) { 
    /* First Byte for the current flash block. Read the current contents */
    current_flash_page = flash_block_begin(addr);
    flash_read(flash_buffer, current_flash_page, flash_block_size, &err_addr); 
    flash_page_init = true;
  } 
  if (flash_block_begin(addr) != current_flash_page) { 
    /* We have moved into the next flash page. Write the current
       page so we can move on */
    retcode = flash_erase(current_flash_page, flash_block_size, &err_addr);
    if (retcode != FLASH_ERR_OK){ /* Flash ERROR */
      diag_printf("Error erase at %p: %s\n", err_addr, flash_errmsg(retcode));
      return;
    }
    
    retcode = flash_program(current_flash_page, flash_buffer, 
                            flash_block_size, &err_addr); 
    if (retcode != FLASH_ERR_OK){ 
      diag_printf("Error writing at %p: %s\n", 
                  err_addr, flash_errmsg(retcode));
      return;
    }
    current_flash_page = flash_block_begin(addr);
    flash_read(flash_buffer, current_flash_page, flash_block_size, &err_addr);
  }
  
  offset = flash_addr - current_flash_page;
  CYG_ASSERT(offset < flash_block_size, "offset not inside flash block");
  
  flash_buffer[offset] = value;
}

// Program the current page into flash.
void flash_load_finish(void)
{
  cyg_uint32 retcode = FLASH_ERR_OK;
  void * err_addr;
  
  if (init_done && flash_page_init) {
    flash_page_init = false;
    
    retcode = flash_erase(current_flash_page, flash_block_size, &err_addr);
    if (retcode != FLASH_ERR_OK){
      diag_printf("Error erase at %p: %s\n", err_addr, flash_errmsg(retcode));
    } else {
      retcode = flash_program(current_flash_page, flash_buffer, 
                              flash_block_size, &err_addr);
      if (retcode != FLASH_ERR_OK){ 
        diag_printf("Error writing at %p: %s\n", 
                    err_addr, flash_errmsg(retcode));
      }
    }
  }
  flash_init(diag_printf);
}

// This is called during redboot start up. We allocate a buffer the
// size of the flash page.
void
flash_load_init(void)
{
  int flash_blocks;

  flash_get_block_info(&flash_block_size, &flash_blocks);
  workspace_end -= flash_block_size;
  
  flash_buffer = workspace_end;
}

// Register this initialization function in the table
RedBoot_init(flash_load_init, RedBoot_INIT_LAST);


    
