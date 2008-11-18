//==========================================================================
//
//      flash.h
//
//      Flash programming - external interfaces
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// copyright (C) 2004 Andrew Lunn
// Copyright (C) 2004, 2005 eCosCentric Ltd.
// Copyright (C) 2003 Gary Thomas
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
// Author(s):    gthomas
// Contributors: gthomas, Andrew Lunn, bartv
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef _IO_FLASH_H_
#define _IO_FLASH_H_

#include <pkgconf/system.h>
#include <pkgconf/io_flash.h>
#include <stddef.h>
#include <cyg/infra/cyg_type.h>
#ifdef CYGPKG_KERNEL
#include <cyg/kernel/kapi.h>
#endif

// Currently a 32-bit quantity. In future this may be 64-bits on some
// platforms, e.g. to support very large nand flashes which can only
// be accessed indirectly.
typedef CYG_ADDRESS cyg_flashaddr_t;

typedef struct cyg_flash_block_info 
{
  size_t                    block_size;
  cyg_uint32                blocks;
} cyg_flash_block_info_t;

// Information about what one device driver drives
typedef struct {
  cyg_flashaddr_t               start;              // First address
  cyg_flashaddr_t               end;                // Last address
  cyg_uint32                    num_block_infos;    // Number of entries
  const cyg_flash_block_info_t* block_info;         // Info about block sizes
} cyg_flash_info_t;

typedef int cyg_flash_printf(const char *fmt, ...);
__externC int cyg_flash_init( cyg_flash_printf *pf );
__externC int cyg_flash_get_info(cyg_uint32 devno, 
                                 cyg_flash_info_t * info);
__externC int cyg_flash_get_info_addr(const cyg_flashaddr_t flash_base, 
                                      cyg_flash_info_t * info);
__externC int cyg_flash_verify_addr(const cyg_flashaddr_t address);
__externC size_t cyg_flash_block_size(const cyg_flashaddr_t flash_base);
__externC int cyg_flash_read(const cyg_flashaddr_t flash_base, 
                             void *ram_base, 
                             size_t len, 
                             cyg_flashaddr_t *err_address);
__externC int cyg_flash_erase(cyg_flashaddr_t flash_base, 
                              size_t len, 
                              cyg_flashaddr_t *err_address);
__externC int cyg_flash_program(cyg_flashaddr_t flash_base, 
                                const void *ram_base, 
                                size_t len, 
                                cyg_flashaddr_t *err_address);
__externC int cyg_flash_lock(const cyg_flashaddr_t flash_base, 
                             size_t len, 
                             cyg_flashaddr_t *err_address);
__externC int cyg_flash_unlock(const cyg_flashaddr_t flash_base, 
                               size_t len, 
                               cyg_flashaddr_t *err_address);
__externC const char *cyg_flash_errmsg(const int err);
#ifdef CYGPKG_KERNEL
__externC int cyg_flash_mutex_lock(const cyg_flashaddr_t from, 
                                   size_t len);
__externC int cyg_flash_mutex_unlock(const cyg_flashaddr_t from, 
                                     size_t len);
#endif

#define CYG_FLASH_ERR_OK              0x00  // No error - operation complete
#define CYG_FLASH_ERR_INVALID         0x01  // Invalid FLASH address
#define CYG_FLASH_ERR_ERASE           0x02  // Error trying to erase
#define CYG_FLASH_ERR_LOCK            0x03  // Error trying to lock/unlock
#define CYG_FLASH_ERR_PROGRAM         0x04  // Error trying to program
#define CYG_FLASH_ERR_PROTOCOL        0x05  // Generic error
#define CYG_FLASH_ERR_PROTECT         0x06  // Device/region is write-protected
#define CYG_FLASH_ERR_NOT_INIT        0x07  // FLASH info not yet initialized
#define CYG_FLASH_ERR_HWR             0x08  // Hardware (configuration?) problem
#define CYG_FLASH_ERR_ERASE_SUSPEND   0x09  // Device is in erase suspend mode
#define CYG_FLASH_ERR_PROGRAM_SUSPEND 0x0a  // Device is in program suspend mode
#define CYG_FLASH_ERR_DRV_VERIFY      0x0b  // Driver failed to verify data
#define CYG_FLASH_ERR_DRV_TIMEOUT     0x0c  // Driver timed out 
#define CYG_FLASH_ERR_DRV_WRONG_PART  0x0d  // Driver does not support device
#define CYG_FLASH_ERR_LOW_VOLTAGE     0x0e  // Not enough juice to complete job

#ifdef CYGSEM_IO_FLASH_LEGACY_API
typedef int _printf(const char *fmt, ...);

externC int flash_init(_printf *pf);
externC int flash_erase(void *base, int len, void **err_address);
externC int flash_program(void *flash_base, void *ram_base, int len, 
                          void **err_address);
externC int flash_read(void *flash_base, void *ram_base, int len, 
                       void **err_address);
externC void flash_dev_query(void *data);
#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING
externC int flash_lock(void *base, int len, void **err_address);
externC int flash_unlock(void *base, int len, void **err_address);
#endif
externC int flash_verify_addr(void *base);
externC int flash_get_limits(void *base, void **start, void **end);
externC int flash_get_block_info(int *block_size, int *blocks);
externC bool flash_code_overlaps(void *start, void *end);
externC char *flash_errmsg(int err);
#endif // CYGSEM_IO_FLASH_LEGACY_API

#if defined(CYGSEM_IO_FLASH_LEGACY_API) || defined(CYGHWR_IO_FLASH_DEVICE_LEGACY)
#define FLASH_ERR_OK              CYG_FLASH_ERR_OK              
#define FLASH_ERR_INVALID         CYG_FLASH_ERR_INVALID         
#define FLASH_ERR_ERASE           CYG_FLASH_ERR_ERASE           
#define FLASH_ERR_LOCK            CYG_FLASH_ERR_LOCK            
#define FLASH_ERR_PROGRAM         CYG_FLASH_ERR_PROGRAM         
#define FLASH_ERR_PROTOCOL        CYG_FLASH_ERR_PROTOCOL        
#define FLASH_ERR_PROTECT         CYG_FLASH_ERR_PROTECT         
#define FLASH_ERR_NOT_INIT        CYG_FLASH_ERR_NOT_INIT        
#define FLASH_ERR_HWR             CYG_FLASH_ERR_HWR             
#define FLASH_ERR_ERASE_SUSPEND   CYG_FLASH_ERR_ERASE_SUSPEND   
#define FLASH_ERR_PROGRAM_SUSPEND CYG_FLASH_ERR_PROGRAM_SUSPEND 
#define FLASH_ERR_DRV_VERIFY      CYG_FLASH_ERR_DRV_VERIFY      
#define FLASH_ERR_DRV_TIMEOUT     CYG_FLASH_ERR_DRV_TIMEOUT     
#define FLASH_ERR_DRV_WRONG_PART  CYG_FLASH_ERR_DRV_WRONG_PART  
#define FLASH_ERR_LOW_VOLTAGE     CYG_FLASH_ERR_LOW_VOLTAGE     
#endif 

#if defined(CYGPKG_IO_FLASH_BLOCK_DEVICE) || \
      defined(CYGPKG_IO_FLASH_BLOCK_DEVICE_LEGACY)
typedef struct {
    CYG_ADDRESS offset;
    size_t len;
    int flasherr;
    cyg_flashaddr_t err_address;
} cyg_io_flash_getconfig_erase_t;

typedef cyg_io_flash_getconfig_erase_t cyg_io_flash_getconfig_lock_t;
typedef cyg_io_flash_getconfig_erase_t cyg_io_flash_getconfig_unlock_t;

typedef struct {
    size_t dev_size;
} cyg_io_flash_getconfig_devsize_t;

typedef struct {
    cyg_flashaddr_t dev_addr;
} cyg_io_flash_getconfig_devaddr_t;

typedef struct {
    CYG_ADDRESS offset;
    size_t block_size;
} cyg_io_flash_getconfig_blocksize_t;
#endif

#endif  // _IO_FLASH_H_
