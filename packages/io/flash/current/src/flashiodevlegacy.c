//==========================================================================
//
//      flashiodev.c
//
//      Old flash device interface to I/O subsystem
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2009, 2010 Free Software Foundation, Inc.
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
// Author(s):    jlarmour
// Contributors: woehler, Andrew Lunn
// Date:         2002-01-16
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#define _FLASH_PRIVATE_
#include <pkgconf/io_flash.h>

#include <errno.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/devtab.h>
#include <cyg/io/config_keys.h>
#include <cyg/io/flash.h>
#include <string.h> // memcpy
#include <cyg/hal/hal_if.h>

#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

struct flashiodev_priv_t{
  cyg_flashaddr_t start;
  cyg_flashaddr_t end;
  cyg_bool        use_fis;
  cyg_bool        use_absolute;
  cyg_bool        use_offset;
  char *          fis_name;
  cyg_uint32      block_size;
  cyg_bool        init;
};

static bool
flashiodev_init( struct cyg_devtab_entry *tab )
{
  int stat = cyg_flash_init(NULL);

  return (stat == CYG_FLASH_ERR_OK);
} // flashiodev_init()

static Cyg_ErrNo
flashiodev_lookup(struct cyg_devtab_entry **tab,
                  struct cyg_devtab_entry  *sub_tab,
                  const char *name) 
{
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)(*tab)->priv;
  cyg_flash_info_t info;
  cyg_uint32 i;
  int stat;

  if (dev->init)
    return ENOERR;

  if (dev->use_fis) {
    CYG_ADDRESS	flash_base;
    unsigned long	size;
    
    if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_FLASH_BASE, 
                                    dev->fis_name,
                                    &flash_base))
      return ENOERR; 
 // Strange, yes, but needed since we have to do a lookup in order to
 // set the name using cyg_io_config_set. If we fail here you cannot
 // do a set. Since dev->init will still be false and attempts to
 // actually use the device will fail, so it is safe.
    
    if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_SIZE, 
                                    dev->fis_name,
                                    &size))
      return ENOERR; // Ditto.
    dev->start = flash_base;
    dev->end = flash_base + size;
  }
  if (dev->use_offset) {
    // dev->start contains the offset to the beginning of the block
    // dev->end is the length of the block
    cyg_flash_info_t info;
    
    cyg_flash_get_info(0, &info);
    dev->start = dev->start + info.start;
    dev->end = dev->start + dev->end;
  }
  if (dev->use_absolute) {
    // dev->start is the absolute address of the start
    // dev->end is the length;
    dev->end = dev->start + dev->end;
  }
  
  stat = cyg_flash_get_info_addr(dev->start, &info);
  if (stat != CYG_FLASH_ERR_OK) {
    return ENODEV;
    }
  dev->block_size = 0;
  for (i=0; i < info.num_block_infos; i++){
    dev->block_size = MAX(dev->block_size, info.block_info[i].block_size);
  }

  dev->init = 1;
  return ENOERR;
} // flashiodev_lookup()

static Cyg_ErrNo
flashiodev_bread( cyg_io_handle_t handle, void *buf, cyg_uint32 *len,
                  cyg_uint32 pos)
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;

  cyg_flashaddr_t startpos = dev->start + pos;
  Cyg_ErrNo err;
  
  if (!dev->init) {
    return -EINVAL;
  }
  
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
  cyg_flashaddr_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;
#endif
  
  err = cyg_flash_read( startpos,(void *)buf, *len, NULL );
  
  if ( err != CYG_FLASH_ERR_OK )
    err = -EIO; // just something sane
  return err;
} // flashiodev_bread()

static Cyg_ErrNo
flashiodev_bwrite( cyg_io_handle_t handle, const void *buf, cyg_uint32 *len,
                   cyg_uint32 pos )
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;
  
  Cyg_ErrNo err;
  cyg_flashaddr_t startpos = dev->start + pos;
  
  if (!dev->init) {
    return -EINVAL;
  }
  
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
  cyg_flashaddr_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;
#endif
  err = cyg_flash_program( startpos, (void *)buf, *len, NULL );
  
  if ( err )
    err = -EIO; // just something sane
  return err;
} // flashiodev_bwrite()

static Cyg_ErrNo
flashiodev_get_config( cyg_io_handle_t handle,
                       cyg_uint32 key,
                       void* buf,
                       cyg_uint32* len)
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;

  if (!dev->init) {
    return -EINVAL;
  }
  
  switch (key) {
  case CYG_IO_GET_CONFIG_FLASH_ERASE:
    {
      if (*len != sizeof( cyg_io_flash_getconfig_erase_t ) )
        return -EINVAL;
      {
        cyg_io_flash_getconfig_erase_t *e = (cyg_io_flash_getconfig_erase_t *)buf;
        cyg_flashaddr_t startpos = dev->start + e->offset;
        
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
        cyg_flashaddr_t endpos = startpos + e->len - 1;
        if ( startpos < dev->start )
          return -EINVAL;
        if ( endpos > dev->end )
          return -EINVAL;
#endif
        e->flasherr = cyg_flash_erase( startpos, e->len, e->err_address );
      }
      return ENOERR;
    }
  case CYG_IO_GET_CONFIG_FLASH_DEVSIZE:
    {
      if ( *len != sizeof( cyg_io_flash_getconfig_devsize_t ) )
        return -EINVAL;
      {
        cyg_io_flash_getconfig_devsize_t *d =
          (cyg_io_flash_getconfig_devsize_t *)buf;

        d->dev_size = dev->end - dev->start;
      }
      return ENOERR;
    }
    
  case CYG_IO_GET_CONFIG_FLASH_BLOCKSIZE:
    {
      cyg_io_flash_getconfig_blocksize_t *b =
        (cyg_io_flash_getconfig_blocksize_t *)buf;
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      cyg_flashaddr_t startpos = dev->start + b->offset;
      
      if ( startpos < dev->start )
        return -EINVAL;
      if ( startpos > dev->end )
        return -EINVAL;
#endif  
      if ( *len != sizeof( cyg_io_flash_getconfig_blocksize_t ) )
        return -EINVAL;
      
      // offset unused for now
      b->block_size = dev->block_size;
      return ENOERR;
    }
    
  default:
    return -EINVAL;
  }
} // flashiodev_get_config()

static Cyg_ErrNo
flashiodev_set_config( cyg_io_handle_t handle,
                       cyg_uint32 key,
                       const void* buf,
                       cyg_uint32* len)
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;
  
  switch (key) {
  case CYG_IO_SET_CONFIG_FLASH_FIS_NAME:
    {
      cyg_flashaddr_t flash_base;
      unsigned long size;
      cyg_flash_info_t info;
      cyg_uint32 i;
      int stat;
      
      if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_FLASH_BASE, 
                                      (char *)buf, &flash_base))
        return -ENOENT;
      
      if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_SIZE, 
                                      (char *)buf, &size))
        return -ENOENT;
      
      dev->start = flash_base;
      dev->end = flash_base + size;

      stat = cyg_flash_get_info_addr(dev->start, &info);

      if (stat != CYG_FLASH_ERR_OK) {
        return -ENOENT;
      }
      dev->block_size = 0;
      for (i=0; i < info.num_block_infos; i++){
        dev->block_size = MAX(dev->block_size, info.block_info[i].block_size);
      }
      dev->init = 1;
     
      return ENOERR;
    }
  default:
    return -EINVAL;
  }
} // flashiodev_set_config()

// get_config/set_config should be added later to provide the other flash
// operations possible, like erase etc.

#define CYG_FLASHIODEV_DEV(start, end, fis, static, offset, name) \
   { start, end, fis, static, offset, name, 0, 0 }

#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_FIS_1
static struct flashiodev_priv_t priv1 = CYG_FLASHIODEV_DEV(
  0,       // start
  0,       // end
  1,       // use_fis
  0,       // use_static
  0,       // use_offset
  CYGDAT_IO_FLASH_BLOCK_FIS_NAME_1);
#endif

#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_STATIC_ABSOLUTE_1 
static struct flashiodev_priv_t priv1 = CYG_FLASHIODEV_DEV(
  CYGNUM_IO_FLASH_BLOCK_ABSOLUTE_START_1,       // start
  CYGNUM_IO_FLASH_BLOCK_ABSOLUTE_LENGTH_1,      // end
  0,       // use_fis
  1,       // use_static
  0,       // use_offset
  "");     // fis_name
#endif

#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_STATIC_1
static struct flashiodev_priv_t priv1 = CYG_FLASHIODEV_DEV(
  CYGNUM_IO_FLASH_BLOCK_OFFSET_1,       // start
  CYGNUM_IO_FLASH_BLOCK_LENGTH_1,       // end 
  0,       // use_fis
  0,       // use_static
  1,       // use_offset
  "");     // fis_name
#endif

BLOCK_DEVIO_TABLE( cyg_io_flashdev1_ops,
                   &flashiodev_bwrite,
                   &flashiodev_bread,
                   0, // no select
                   &flashiodev_get_config,
                   &flashiodev_set_config
                   ); 


BLOCK_DEVTAB_ENTRY( cyg_io_flashdev1,
                    CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_1,
                    0,
                    &cyg_io_flashdev1_ops,
                    &flashiodev_init,
                    &flashiodev_lookup,
                    &priv1 );

#ifdef CYGSEM_IO_FLASH_BLOCK_DEVICE_2 
#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_FIS_2
static struct flashiodev_priv_t priv2 = CYG_FLASHIODEV_DEV(
  0,       // start
  0,       // end
  1,       // use_fis
  0,       // use_static
  0,       // use_offset
  CYGDAT_IO_FLASH_BLOCK_FIS_NAME_2);
#endif

#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_STATIC_ABSOLUTE_2 
static struct flashiodev_priv_t priv2 = CYG_FLASHIODEV_DEV(
  CYGNUM_IO_FLASH_BLOCK_ABSOLUTE_START_2,       // start
  CYGNUM_IO_FLASH_BLOCK_ABSOLUTE_LENGTH_2,      // end
  0,       // use_fis
  1,       // use_static
  0,       // use_offset
  "");     // fis_name
#endif

#ifdef CYGNUM_IO_FLASH_BLOCK_CFG_STATIC_2
static struct flashiodev_priv_t priv2 = CYG_FLASHIODEV_DEV(
  CYGNUM_IO_FLASH_BLOCK_OFFSET_2,       // start
  CYGNUM_IO_FLASH_BLOCK_LENGTH_2,       // end 
  0,       // use_fis
  0,       // use_static
  1,       // use_offset
  "");     // fis_name
#endif

BLOCK_DEVTAB_ENTRY( cyg_io_flashdev2,
                    CYGDAT_IO_FLASH_BLOCK_DEVICE_NAME_2,
                    0,
                    &cyg_io_flashdev1_ops,
                    &flashiodev_init,
                    &flashiodev_lookup,
                    &priv2 );
#endif

// EOF flashiodev.c
