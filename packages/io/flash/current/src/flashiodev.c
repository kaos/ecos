//==========================================================================
//
//      flashiodev.c
//
//      Flash device interface to I/O subsystem
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2004 Andrew Lunn
// Copyright (C) 2004, 2007 eCosCentric Ltd.
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: woehler, Andrew Lunn
// Date:         2004-12-03
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

// INCLUDES

#include <pkgconf/io_flash.h>
#include <errno.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/io/devtab.h>
#include <cyg/io/config_keys.h>
#include <cyg/io/flash.h>
#include <string.h> // memcpy
#include <cyg/hal/hal_if.h>
#include <cyg/hal/drv_api.h> // mutexes

// MACROS

#define MIN(x,y) ((x)<(y) ? (x) : (y))
#define MAX(x,y) ((x)>(y) ? (x) : (y))

// TYPES

struct flashiodev_priv_t{
  cyg_devtab_entry_t handle;
  cyg_flashaddr_t start;
  cyg_flashaddr_t end;
  cyg_bool        valid;
};
// FUNCTION PROTOTYPES

static bool flashiodev_init( struct cyg_devtab_entry *tab );
static Cyg_ErrNo
flashiodev_lookup(struct cyg_devtab_entry **tab,
                  struct cyg_devtab_entry  *sub_tab,
                  const char *name) ;
static Cyg_ErrNo
flashiodev_bread( cyg_io_handle_t handle, void *buf, cyg_uint32 *len,
                  cyg_uint32 pos);
static Cyg_ErrNo
flashiodev_bwrite( cyg_io_handle_t handle, const void *buf, cyg_uint32 *len,
                   cyg_uint32 pos );
static Cyg_ErrNo
flashiodev_get_config( cyg_io_handle_t handle,
                       cyg_uint32 key,
                       void* buf,
                       cyg_uint32* len);
static Cyg_ErrNo
flashiodev_set_config( cyg_io_handle_t handle,
                       cyg_uint32 key,
                       const void* buf,
                       cyg_uint32* len);

// STATICS/GLOBALS

static struct flashiodev_priv_t flashiodev_table[CYGNUM_IO_FLASH_BLOCK_DEVICES];
static cyg_drv_mutex_t flashiodev_table_lock;

BLOCK_DEVIO_TABLE( cyg_io_flashdev_ops,
                   &flashiodev_bwrite,
                   &flashiodev_bread,
                   NULL, // no select
                   &flashiodev_get_config,
                   &flashiodev_set_config
                   ); 


BLOCK_DEVTAB_ENTRY( cyg_io_flashdev,
                    "/dev/flash/",
                    NULL,
                    &cyg_io_flashdev_ops,
                    &flashiodev_init,
                    &flashiodev_lookup,
                    NULL );

// FUNCTIONS

static int dummy_printf( const char *fmt, ... ) {return 0;}

static bool
flashiodev_init( struct cyg_devtab_entry *tab )
{
  int stat = cyg_flash_init( &dummy_printf );
  cyg_ucount32 i;

  if (stat == CYG_FLASH_ERR_OK)
  {
      for (i=0; i<CYGNUM_IO_FLASH_BLOCK_DEVICES; i++)
      {
          CYG_ASSERT( tab->handlers == &cyg_io_flashdev_ops, "Unexpected handlers - not flashdev_ops" );
          flashiodev_table[i].handle.handlers = &cyg_io_flashdev_ops;
          flashiodev_table[i].handle.status = CYG_DEVTAB_STATUS_BLOCK;
          flashiodev_table[i].handle.priv = &flashiodev_table[i]; // link back
      } // for
      cyg_drv_mutex_init( &flashiodev_table_lock );
  } // if

  return (stat == CYG_FLASH_ERR_OK);
} // flashiodev_init()

#ifdef CYGFUN_IO_FLASH_BLOCK_FROM_DEVOFFSET
static cyg_uint32
parsenum( const char **str )
{
    cyg_uint32 num = 0;
    cyg_uint8 base = 10;

    // trim leading 0s
    while (**str == '0')
        (*str)++;
    // crudely detect hex by assuming that something with a leading 0x
    // can just match the 'x' with a leading 0 being trimmed.
    if ( (**str == 'x') || (**str == 'X') )
    {
        (*str)++;
        base = 16;
    }

    while (**str) {
        switch (**str)
        {
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            num = (num * base) + (**str-'0');
            break;
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
            if ( base != 16 )
                return num;
            num = (num * base) + (**str-'a' + 10);
            break;
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
            if ( base != 16 )
                return num;
            num = (num << 4) + (**str-'A' + 10);
            break;
        default:
            return num;
        } // switch
        (*str)++;
    } // while

    return num;
} // parsenum()
#endif // ifdef CYGFUN_IO_FLASH_BLOCK_FROM_DEVOFFSET

static Cyg_ErrNo
flashiodev_lookup(struct cyg_devtab_entry **tab,
                  struct cyg_devtab_entry  *sub_tab,
                  const char *name) 
{
    // We enter with **tab being the entry for /dev/flash only
    // We will leave with **tab instead pointing to a newly configured flash device instance
    // We could allocate the space for recording these dynamically, but that's overkill,
    // so we instead choose the eCos ethos of "let the user decide at configure time".
    // After all there's a good chance there'll be only one or two needed at one time.
    
    // There are two styles of path:
    //    /dev/flash/fis/<fispartitionname>
    // and
    //    /dev/flash/<deviceno>/<offset>[,<length>]

    cyg_flashaddr_t start=0;
    cyg_flashaddr_t end=0;
    cyg_bool valid = false;

#ifdef CYGFUN_IO_FLASH_BLOCK_FROM_FIS
    // First, support FIS
    if ((name[0] == 'f') && (name[1] == 'i') &&
        (name[2] == 's') && (name[3] == '/'))
    {
        CYG_ADDRESS	flash_base;
        unsigned long	size;
    
        if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_FLASH_BASE, 
                                        (char *)&name[4],
                                        &flash_base))
            return -ENOENT; 
    
        if(!CYGACC_CALL_IF_FLASH_FIS_OP(CYGNUM_CALL_IF_FLASH_FIS_GET_SIZE, 
                                        (char *)&name[4],
                                        &size))
            return -ENODEV; // If the previous call worked, then this failing would be very wrong.
        start = flash_base;
        end = flash_base + size - 1;
        valid = true;
    } else
#endif // ifdef CYGFUN_IO_FLASH_BLOCK_FROM_FIS
#ifdef CYGFUN_IO_FLASH_BLOCK_FROM_DEVOFFSET
    // Next, support device numbers with offsets encoded in path name
    // Note that for now we assume < 10 flash devices. I think this is reasonable
    // to avoid unnecessary code. It can be changed later if needed.
    if ( (name[0] >= '0') && (name[0] <= '9') )
    {
        cyg_uint32 flashdevno = name[0] - '0';
        int res;
        cyg_flash_info_t info;
        cyg_uint32 offset;
        const char *tempstr;

        if (name[1] != '/')
            return -ENOTSUP;
        res = cyg_flash_get_info( flashdevno, &info );
        if (CYG_FLASH_ERR_OK != res)
            return -ENOENT;

        // Now modify with offset and optional length
        tempstr = &name[2];
        offset = parsenum( &tempstr );

        start = info.start + offset;
        end = info.end;
        
        if (*tempstr == ',') // optional length
        {
            cyg_uint32 length;

            tempstr++;
            length = parsenum( &tempstr );

            // Check we don't exceed end of device.
            // Note the flash end address is the last byte of addressible flash *not* the base+size
            if ( (start + length - 1) > end )
                return -EINVAL;
            end = start + length - 1;
        }
        valid = true;
    } // else if
#endif

    if (valid)
    {
        // Find a slot and use it
        cyg_ucount32 i;

        cyg_drv_mutex_lock( &flashiodev_table_lock );
        for (i=0; i<CYGNUM_IO_FLASH_BLOCK_DEVICES; i++)
        {
            if ( !flashiodev_table[i].valid ) {
                flashiodev_table[i].start = start;
                flashiodev_table[i].end = end;
                flashiodev_table[i].valid = true;
                cyg_drv_mutex_unlock( &flashiodev_table_lock );

                *tab = &flashiodev_table[i].handle;
                // Theoretically we could set up the devtab entry more fully, e.g.
                // the name, but I don't see the need!
                return ENOERR;
            } // if
        } // for
        cyg_drv_mutex_unlock( &flashiodev_table_lock );

        // If we got here we must have reached the end of the table without finding a space
        return -ENOMEM;
    }
    // Wasn't valid, so....
    return -ENOENT;
} // flashiodev_lookup()

static Cyg_ErrNo
flashiodev_bread( cyg_io_handle_t handle, void *buf, cyg_uint32 *len,
                  cyg_uint32 pos)
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;

  cyg_flashaddr_t startpos = dev->start + pos;
  Cyg_ErrNo err = ENOERR;
  int flasherr;
  
  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flashiodev_priv_t*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );
  
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
  cyg_flashaddr_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;
#endif
  
  flasherr = cyg_flash_read( startpos,(void *)buf, *len, NULL );
  
  if ( flasherr != CYG_FLASH_ERR_OK )
    err = -EIO; // just something sane
  return err;
} // flashiodev_bread()

static Cyg_ErrNo
flashiodev_bwrite( cyg_io_handle_t handle, const void *buf, cyg_uint32 *len,
                   cyg_uint32 pos )
{
  struct cyg_devtab_entry *tab = (struct cyg_devtab_entry *)handle;
  struct flashiodev_priv_t *dev = (struct flashiodev_priv_t *)tab->priv;
  
  Cyg_ErrNo err = ENOERR;
  int flasherr;
  cyg_flashaddr_t startpos = dev->start + pos;
  
  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flashiodev_priv_t*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );
  
  // Unlike some other cases we _do_ do bounds checking on this all the time, because
  // the consequences of writing over the wrong bit of flash are so nasty.
  cyg_flashaddr_t endpos = startpos + *len - 1;
  if ( startpos < dev->start )
    return -EINVAL;
  if ( endpos > dev->end )
    return -EINVAL;

  flasherr = cyg_flash_program( startpos, (void *)buf, *len, NULL );
  
  if ( flasherr != CYG_FLASH_ERR_OK )
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

  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flashiodev_priv_t*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );
  
  switch (key) {
  case CYG_IO_GET_CONFIG_FLASH_ERASE:
    {
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if (*len != sizeof( cyg_io_flash_getconfig_erase_t ) )
        return -EINVAL;
#endif
      {
        cyg_io_flash_getconfig_erase_t *e = (cyg_io_flash_getconfig_erase_t *)buf;
        cyg_flashaddr_t startpos = dev->start + e->offset;
        
        // Unlike some other cases we _do_ do bounds checking on this all the time, because
        // the consequences of erasing the wrong bit of flash are so nasty.
        cyg_flashaddr_t endpos = startpos + e->len - 1;
        if ( startpos < dev->start )
          return -EINVAL;
        if ( endpos > dev->end )
          return -EINVAL;

        e->flasherr = cyg_flash_erase( startpos, e->len, &e->err_address );
      }
      return ENOERR;
    }

#ifdef CYGHWR_IO_FLASH_BLOCK_LOCKING    
  case CYG_IO_GET_CONFIG_FLASH_LOCK:
    {
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if (*len != sizeof( cyg_io_flash_getconfig_lock_t ) )
        return -EINVAL;
#endif
      {
        cyg_io_flash_getconfig_lock_t *d = (cyg_io_flash_getconfig_lock_t *)buf;
        cyg_flashaddr_t startpos = dev->start + d->offset;

#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time        
        cyg_flashaddr_t endpos = startpos + d->len - 1;
        if ( startpos < dev->start )
          return -EINVAL;
        if ( endpos > dev->end )
          return -EINVAL;
#endif
        
        d->flasherr = cyg_flash_lock( startpos, d->len, &d->err_address );
      }
      return ENOERR;
    }

  case CYG_IO_GET_CONFIG_FLASH_UNLOCK:
    {
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if (*len != sizeof( cyg_io_flash_getconfig_unlock_t ) )
        return -EINVAL;
#endif
      {
        cyg_io_flash_getconfig_unlock_t *d = (cyg_io_flash_getconfig_unlock_t *)buf;
        cyg_flashaddr_t startpos = dev->start + d->offset;

#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time        
        cyg_flashaddr_t endpos = startpos + d->len - 1;
        if ( startpos < dev->start )
          return -EINVAL;
        if ( endpos > dev->end )
          return -EINVAL;
#endif        
        d->flasherr = cyg_flash_unlock( startpos, d->len, &d->err_address );
      }
      return ENOERR;
    }
#endif
    
  case CYG_IO_GET_CONFIG_FLASH_DEVSIZE:
    {
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if ( *len != sizeof( cyg_io_flash_getconfig_devsize_t ) )
        return -EINVAL;
#endif
      {
        cyg_io_flash_getconfig_devsize_t *d =
          (cyg_io_flash_getconfig_devsize_t *)buf;

        d->dev_size = dev->end - dev->start + 1;
      }
      return ENOERR;
    }

  case CYG_IO_GET_CONFIG_FLASH_DEVADDR:
    {
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if ( *len != sizeof( cyg_io_flash_getconfig_devaddr_t ) )
        return -EINVAL;
#endif
      {
        cyg_io_flash_getconfig_devaddr_t *d =
          (cyg_io_flash_getconfig_devaddr_t *)buf;

        d->dev_addr = dev->start;
      }
      return ENOERR;
    }

  case CYG_IO_GET_CONFIG_FLASH_BLOCKSIZE:
    {
      cyg_io_flash_getconfig_blocksize_t *b =
        (cyg_io_flash_getconfig_blocksize_t *)buf;
      cyg_flashaddr_t pos = dev->start + b->offset;
      
#ifdef CYGPKG_INFRA_DEBUG // don't bother checking this all the time
      if ( pos < dev->start )
        return -EINVAL;
      if ( pos > dev->end )
        return -EINVAL;
      if ( *len != sizeof( cyg_io_flash_getconfig_blocksize_t ) )
        return -EINVAL;
#endif  

      b->block_size = cyg_flash_block_size( pos );
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
  
  CYG_ASSERT( dev->handle.handlers == &cyg_io_flashdev_ops, "bad flash operation link" );
  CYG_ASSERT( ((struct flashiodev_priv_t*)(dev->handle.priv))->valid, "operation on not valid flash device instance" );
  
  switch (key) {
  case CYG_IO_SET_CONFIG_CLOSE:
    {
        Cyg_ErrNo err = ENOERR;
        cyg_drv_mutex_lock( &flashiodev_table_lock );
        if (!dev->valid)
        {
            err = -EBADF;
        } else {
            dev->valid = false;
        }
        cyg_drv_mutex_unlock( &flashiodev_table_lock );
        return err;
    }
  default:
    return -EINVAL;
  }
} // flashiodev_set_config()

// EOF flashiodev.c
