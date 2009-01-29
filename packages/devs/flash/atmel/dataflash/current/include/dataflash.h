#ifndef CYGONCE_DATAFLASH_H
#define CYGONCE_DATAFLASH_H
//==========================================================================
//
//      dataflash.h
//
//      DataFlash series flash driver defines
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):    Savin Zlobec <savin@elatec.si> 
// Date:         2004-08-27
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/devs_flash_atmel_dataflash.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>

//----------------------------------------------------------------------------

enum {
    CYG_DATAFLASH_ERR_OK         = 0, // No error
    CYG_DATAFLASH_ERR_INVALID    = 1, // Invalid address error
    CYG_DATAFLASH_ERR_WRONG_PART = 2, // Unsupported device error
    CYG_DATAFLASH_ERR_TIMEOUT    = 3, // Operation timeout error
    CYG_DATAFLASH_ERR_COMPARE    = 4  // Buffer - main memory compare error
};

enum {
    CYG_DATAFLASH_STATE_IDLE = 0,
    CYG_DATAFLASH_STATE_BUSY = 1
};

//----------------------------------------------------------------------------

typedef struct cyg_dataflash_dev_info_s
{
    cyg_uint8  device_id;        // Device ID 
    cyg_uint16 page_size;        // Page size in bytes
    cyg_uint16 page_count;       // Total number of pages
    cyg_uint8  baddr_bits;       // Bits used for byte address in addressing seq
    cyg_uint16 block_size;       // Block size in pages
    cyg_uint16 sector_sizes[64]; // Sector sizes in blocks
    cyg_uint16 sector_count;     // Total number of sectors 
} cyg_dataflash_dev_info_t;

typedef struct cyg_dataflash_device_s
{
    const cyg_dataflash_dev_info_t *info;     // DataFlash device info
    cyg_spi_device                 *spi_dev;  // SPI device
    cyg_drv_mutex_t                 lock;     // Access lock 
    cyg_bool                        polled;   // Polled mode flag
    cyg_bool                        blocking; // Blocking mode flag 
    int                             state;    // Current state (IDLE, BUSY, ...)
    cyg_uint8                       busy_buf; // Current busy buffer number
} cyg_dataflash_device_t;

//----------------------------------------------------------------------------
#ifdef CYGPKG_IO_FLASH
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

typedef struct cyg_dataflash_flash_dev_priv_s
{
    cyg_int16               start_sector;   // Start sector of flash driver space
    cyg_int16               end_sector;     // End sector of flash driver space
    cyg_dataflash_device_t  dev;            // DataFlash device
    cyg_uint32              start_page;     // Start page of flash driver space
    cyg_uint32              end_page;       // End page of flash driver space
    cyg_flash_block_info_t  block_info[1];
} cyg_dataflash_flash_dev_priv_t;

externC struct cyg_flash_dev_funs cyg_dataflash_flash_dev_funs;

#define CYG_DATAFLASH_FLASH_DRIVER(name, _sdev, _addr, _start, _end)            \
    static cyg_dataflash_flash_dev_priv_t cyg_dataflash_priv_ ## name = {       \
        .start_sector = _start,                                                 \
        .end_sector   = _end,                                                   \
        .dev.spi_dev  = _sdev,                                                  \
    };                                                                          \
    CYG_FLASH_DRIVER(name,                                                      \
                     &cyg_dataflash_flash_dev_funs,                             \
                     0,                                                         \
                     _addr,                                                     \
                     0,                                                         \
                     1,                                                         \
                     cyg_dataflash_priv_ ## name.block_info,                    \
                     & cyg_dataflash_priv_ ## name                              \
        )
    
#endif
//----------------------------------------------------------------------------

static inline cyg_uint8
cyg_dataflash_get_device_id(cyg_dataflash_device_t *dev)
{
    return dev->info->device_id;
}

static inline cyg_uint16
cyg_dataflash_get_page_size(cyg_dataflash_device_t *dev)
{
    return dev->info->page_size;
}

static inline cyg_uint16
cyg_dataflash_get_page_count(cyg_dataflash_device_t *dev)
{
    return dev->info->page_count;
}

static inline cyg_uint16
cyg_dataflash_get_block_size(cyg_dataflash_device_t *dev)
{
    return dev->info->block_size;
}

static inline cyg_uint16
cyg_dataflash_get_sector_size(cyg_dataflash_device_t *dev, cyg_uint8 sector_num)
{
    return dev->info->sector_sizes[sector_num];
}

static inline cyg_uint16
cyg_dataflash_get_sector_count(cyg_dataflash_device_t *dev)
{
    return dev->info->sector_count;
}

static inline void
cyg_dataflash_set_polled_operation(cyg_dataflash_device_t *dev, cyg_bool polled)
{
    dev->polled = polled;
}

static inline void
cyg_dataflash_set_blocking_operation(cyg_dataflash_device_t *dev, cyg_bool block)
{
    dev->blocking = block;
} 

//----------------------------------------------------------------------------

externC int cyg_dataflash_init(cyg_bool                polled,
                               cyg_dataflash_device_t *dev);

externC int cyg_dataflash_aquire(cyg_dataflash_device_t *dev);

externC int cyg_dataflash_release(cyg_dataflash_device_t *dev);

externC cyg_uint16 cyg_dataflash_get_sector_start(cyg_dataflash_device_t *dev,
                                                  cyg_uint16 sector_num);

externC int cyg_dataflash_wait_ready(cyg_dataflash_device_t *dev);

externC int cyg_dataflash_read_buf(cyg_dataflash_device_t *dev,
                                   cyg_uint8               buf_num,
                                   cyg_uint8              *buf, 
                                   cyg_uint32              len, 
                                   cyg_uint32              pos); 
                       
externC int cyg_dataflash_write_buf(cyg_dataflash_device_t *dev,
                                    cyg_uint8               buf_num,
                                    const cyg_uint8        *buf,
                                    cyg_uint32              len,
                                    cyg_uint32              pos);

externC int cyg_dataflash_mem_to_buf(cyg_dataflash_device_t *dev,
                                     cyg_uint8               buf_num,
                                     cyg_uint32              page_num);

externC int cyg_dataflash_program_buf(cyg_dataflash_device_t *dev,
                                      cyg_uint8               buf_num,
                                      cyg_uint32              page_num,
                                      cyg_bool                erase);

externC int cyg_dataflash_compare_buf(cyg_dataflash_device_t *dev,
                                      cyg_uint8               buf_num,
                                      cyg_uint32              page_num);

externC int cyg_dataflash_erase(cyg_dataflash_device_t *dev,
                                cyg_uint32              page_num);

externC int cyg_dataflash_erase_block(cyg_dataflash_device_t *dev,
                                      cyg_uint32              block_num);

externC int cyg_dataflash_auto_rewrite(cyg_dataflash_device_t *dev,
                                       cyg_uint8               buf_num,
                                       cyg_uint32              page_num);

externC int cyg_dataflash_read(cyg_dataflash_device_t *dev, 
                               cyg_uint8              *buf, 
                               cyg_uint32              len, 
                               cyg_uint32              pos);

externC int cyg_dataflash_program(cyg_dataflash_device_t *dev, 
                                  const cyg_uint8        *buf, 
                                  cyg_uint32             *len, 
                                  cyg_uint32              pos,
                                  cyg_bool                erase,
                                  cyg_bool                verify);

//----------------------------------------------------------------------------

#endif // CYGONCE_DATAFLASH_H

//----------------------------------------------------------------------------
// End of dataflash.h
