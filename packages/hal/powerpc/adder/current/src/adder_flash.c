//==========================================================================
//
//      adder_flash.c
//
//      Support for the external AMD flash device on Adder boards
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004, 2006, 2008 Free Software Foundation, Inc.            
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
// Author(s):   bartv
// Date:        2004-11-25
//
//####DESCRIPTIONEND####
//=============================================================================

// There is a single AMD AM29LV320D device. DevId 22F6, 4MB, 63*64K,
// 8*8K.

#include <pkgconf/system.h>
#ifdef CYGPKG_IO_FLASH
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/io/am29xxxxx_dev.h>

static const CYG_FLASH_FUNS(hal_adder_flash_amd_funs,
                            &cyg_am29xxxxx_init_cfi_16,
                            &cyg_flash_devfn_query_nop,
                            &cyg_am29xxxxx_erase_16,
                            &cyg_am29xxxxx_program_16,
                            (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                            &cyg_flash_devfn_lock_nop,
                            &cyg_flash_devfn_unlock_nop);

static cyg_am29xxxxx_dev hal_adder_flash_priv = {
    .devid      = 0x22F6,
    .block_info = {
        { 0, 0 },
        { 0, 0 },
        { 0, 0 },
        { 0, 0 }
    }
};

CYG_FLASH_DRIVER(hal_adder_flash,
                 &hal_adder_flash_amd_funs,
                 0,
                 0xFE000000,
                 0xFE3FFFFF,
                 4,
                 hal_adder_flash_priv.block_info,
                 &hal_adder_flash_priv
);
#endif  // CYGPKG_IO_FLASH

// EOF adder_flash.c
