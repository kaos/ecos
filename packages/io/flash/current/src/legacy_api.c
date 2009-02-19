//==========================================================================
//
//      legacy_api.c
//
//      Legacy API implementation on top of the new API
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004, 2009 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Andrew Lunn
// Contributors: Andrew Lunn
// Date:         2004-07-02
// Purpose:      
// Description:  Implement an interface to the legacy device drivers
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_flash.h>

#include <cyg/io/flash.h>

int
flash_init(_printf *pf)
{
  cyg_flash_set_global_printf(pf);
  return cyg_flash_init();
}

int
flash_verify_addr(void *target)
{
  return cyg_flash_verify_addr((cyg_flashaddr_t) target);
}

int
flash_get_limits(void *target, void **start, void **end)
{
  int err;
  cyg_flash_info_t info;
  
  err = cyg_flash_get_info(0, &info);
  if (err != CYG_FLASH_ERR_OK) {
    return err;
  }

  *start = (void *)info.start;
  *end = (void *)info.end;

  return CYG_FLASH_ERR_OK;
}


int
flash_get_block_info(int *block_size, int *blocks)
{
  size_t biggest_size=0;
  cyg_uint32 i;
  cyg_flash_info_t info;
  int err;
  
  err = cyg_flash_get_info(0, &info);
  if (err != CYG_FLASH_ERR_OK) {
    return err;
  }

  // Find the biggest size of blocks
  for (i=0; i < info.num_block_infos; i++) {
    if (info.block_info[i].block_size > biggest_size) {
      biggest_size = info.block_info[i].block_size;
    }
  }
  
  // Calculate the number of biggest size blocks
  *block_size = biggest_size;
  *blocks = 0;
  for (i=0; i < info.num_block_infos; i++) {
    *blocks += (info.block_info[i].block_size *
                info.block_info[i].blocks) /
      biggest_size;
  }
  return CYG_FLASH_ERR_OK;
}

int
flash_erase(void *addr, int len, void **err_addr)
{
  return cyg_flash_erase((cyg_flashaddr_t)addr, len, 
                         (cyg_flashaddr_t *)err_addr);
}

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING

int
flash_lock(void *addr, int len, void **err_addr)
{
  return cyg_flash_lock((cyg_flashaddr_t)addr, len, 
                        (cyg_flashaddr_t *)err_addr);
}

int
flash_unlock(void *addr, int len, void **err_addr)
{
  return cyg_flash_unlock((cyg_flashaddr_t)addr, len, 
                          (cyg_flashaddr_t *)err_addr);
}

#endif

int flash_program(void *flash_base, void *ram_base, int len, 
                  void **err_address)
{
  return cyg_flash_program((cyg_flashaddr_t)flash_base, ram_base, 
                           len, (cyg_flashaddr_t *)err_address);
}

int flash_read(void *flash_base, void *ram_base, int len, void **err_address)
{
  return cyg_flash_read((cyg_flashaddr_t)flash_base, ram_base, 
                        len, (cyg_flashaddr_t *)err_address);
}

char *flash_errmsg(int err)
{
  return (char *)cyg_flash_errmsg(err);
}

// EOF legacy_api.c
