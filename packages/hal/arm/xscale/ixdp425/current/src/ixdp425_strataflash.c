//==========================================================================
//
//      ixdp425_strataflash.c
//
//      Support for the Strata flash device on Intel IXDP425
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2005, 2006 Free Software Foundation, Inc.                  
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
// Author(s):   jlarmour based on original by bartv
// Date:        2006-01-17
//
//####DESCRIPTIONEND####
//=============================================================================

// There is a single Intel StrataFLASH device on the IXDP425, supported via the
// generic Strata flash package. By default this is the 28F128J3 which is a
// 16MByte device, arranged as 128*128K blocks with no boot blocks, 16-bits
// wide. There is a 32-byte program buffer and locking is supported.
//
// But CFI is used to determine Flash geometry as the part is fitted in a socket
// to allow replacement with other StrataFLASH parts.

#include <pkgconf/system.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/io/strata_dev.h>
#include <cyg/hal/plf_io.h>

static const CYG_FLASH_FUNS(hal_ixdp425_flash_strata_funs,
                            &cyg_strata_init_cfi_16,
                            &cyg_flash_devfn_query_nop,
                            &cyg_strata_erase_16,
                            &cyg_strata_bufprogram_16,
                            (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
                            &cyg_strata_lock_j3_16,
                            &cyg_strata_unlock_j3_16);

static cyg_strata_dev hal_ixdp425_flash_priv;
#ifdef notusingcfianymore
 = {
    .manufacturer_code  = 0x0089,
    .device_code        = 0x0018,
    .bufsize            = 16,       // 32 bytes -> 16 words
    .block_info = {
        { 0x00020000, 128 }
    }
};
#endif

// The Flash driver initially claims to use the cached mapping of the Flash
// device, but through CYGARC_UNCACHED_ADDRESS, will actually use the *uncached*
// mapping when it comes to driver operations. That way things like
// RedBoot's FIS directory still correctly use the cached flash addresses.

CYG_FLASH_DRIVER(hal_ixdp425_flash,
                 &hal_ixdp425_flash_strata_funs,
                 0,
                 IXDP_FLASH_BASE,
                 (IXDP_FLASH_BASE+IXDP_FLASH_SIZE-1),
                 1,
                 hal_ixdp425_flash_priv.block_info,
                 &hal_ixdp425_flash_priv
);

// EOF ixdp425_strataflash.c
