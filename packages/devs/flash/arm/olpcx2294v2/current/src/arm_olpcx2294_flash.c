//==========================================================================
//
//      arm_olpcx2294_flash.c
//
//      Flash programming for Intel FlashFile devices on Olimex LPC-X2294
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-11-28
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>

#if !defined(CYGPKG_HAL_ARM_LPC2XXX_OLPCE2294) && \
    !defined(CYGPKG_HAL_ARM_LPC2XXX_OLPCH2294) && \
    !defined(CYGPKG_HAL_ARM_LPC2XXX_OLPCL2294)
# error Unsupported target
#endif

#ifdef CYGPKG_DEVS_FLASH_STRATA_V2

#include <cyg/io/flash.h>
#include <cyg/io/strata_dev.h>

// Olimex LPC-E2294 development board and Olimex LPC-H2294 header board both
// have 28F320C3-B flash memory part, Olimex LPC-L2294-1M development board has
// 28F160C3-B flash memory part and Olimex LPC-L2294-8M development board has
// 28F320C3-B flash memory part. All boards have 16-bit access to it's flash
// devices.
static const CYG_FLASH_FUNS(hal_olpcx2294_flash_strata_funs,
    &cyg_strata_init_check_devid_16,
    &cyg_flash_devfn_query_nop,
    &cyg_strata_erase_16,
    &cyg_strata_program_16,
    (int (*)(struct cyg_flash_dev*, const cyg_flashaddr_t, void*, size_t))0,
    &cyg_strata_lock_k3_16,
    &cyg_strata_unlock_k3_16);

static const cyg_strata_dev hal_olpcx2294_flash_priv = {
    .manufacturer_code = CYG_FLASH_STRATA_MANUFACTURER_INTEL,
#if defined(CYGHWR_OLIMEX_BOARD_OLPCL2294_1M)
    .device_code = 0x88c3, /* 16-Mbit x 16-B, 28F160C3-B */
#else
    .device_code = 0x88c5, /* 32-Mbit x 16-B, 28F320C3-B */
#endif
    .bufsize    = 1,
    .block_info = {
#if defined(CYGHWR_OLIMEX_BOARD_OLPCL2294_1M)
        { 0x00002000,  8 },/* boot bottom 8 x 8K blocks */
        { 0x00010000, 31 } /* 31 x 64K blocks */
#else
        { 0x00002000,  8 },/* boot bottom 8 x 8K blocks */
        { 0x00010000, 63 } /* 63 x 64K blocks */
#endif
    }
};

#if defined(CYGHWR_OLIMEX_BOARD_OLPCL2294_1M)
CYG_FLASH_DRIVER(hal_olpcx2294_flash,
                 &hal_olpcx2294_flash_strata_funs,
                 0,
                 0x80000000,
                 0x801fffff,
                 2,
                 hal_olpcx2294_flash_priv.block_info,
                 &hal_olpcx2294_flash_priv
);
#else
CYG_FLASH_DRIVER(hal_olpcx2294_flash,
                 &hal_olpcx2294_flash_strata_funs,
                 0,
                 0x80000000,
                 0x803fffff,
                 2,
                 hal_olpcx2294_flash_priv.block_info,
                 &hal_olpcx2294_flash_priv
);
#endif

#endif//CYGPKG_DEVS_FLASH_STRATA_V2
// ------------------------------------------------------------------------
// EOF arm_olpcx2294_flash.c
