//==========================================================================
//
//      devs_flash_atmel_dataflash_flash_dev_funs.c 
//
//      DataFlash flash device funs 
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
// Author(s):    Savin Zlobec <savin@elatec.si> 
// Date:         2004-09-06
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/devs_flash_atmel_dataflash.h>
#include <pkgconf/io_flash.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/io/spi.h>
#include <cyg/io/dataflash.h>

// -------------------------------------------------------------------------- 

#define RETURN_ON_ERROR(_op_) \
    if (CYG_DATAFLASH_ERR_OK != (err = _op_)) return df_flash_hwr_map_error(dev, err)

#define GOTO_ON_ERROR(_op_) \
    if (CYG_DATAFLASH_ERR_OK != (err = _op_)) goto on_error
    
static int 
df_flash_hwr_map_error(struct cyg_flash_dev *dev, int err)
{
    switch (err)
    {
        case CYG_DATAFLASH_ERR_OK:         return CYG_FLASH_ERR_OK; 
        case CYG_DATAFLASH_ERR_INVALID:    return CYG_FLASH_ERR_INVALID;  
        case CYG_DATAFLASH_ERR_WRONG_PART: return CYG_FLASH_ERR_DRV_WRONG_PART; 
        case CYG_DATAFLASH_ERR_TIMEOUT:    return CYG_FLASH_ERR_DRV_TIMEOUT;
        case CYG_DATAFLASH_ERR_COMPARE:    return CYG_FLASH_ERR_DRV_VERIFY; 
        default:                           return CYG_FLASH_ERR_INVALID;
    }
}

// -------------------------------------------------------------------------- 

static int 
df_flash_init(struct cyg_flash_dev *dev)
{
    cyg_dataflash_flash_dev_priv_t   *priv;
    cyg_dataflash_device_t           *df_dev;
    int sector_cnt;
    
    priv   = (cyg_dataflash_flash_dev_priv_t *) dev->priv; 
    df_dev = &priv->dev;
   
#ifdef CYGPKG_REDBOOT    
    if (cyg_dataflash_init(true, df_dev))
#else
    if (cyg_dataflash_init(false, df_dev))
#endif        
        return CYG_DATAFLASH_ERR_WRONG_PART; 
  
    cyg_dataflash_set_blocking_operation(df_dev, true);

    sector_cnt = cyg_dataflash_get_sector_count(df_dev);

    if (priv->end_sector < 0)
       priv->end_sector = sector_cnt - 1; 
    
    if (priv->start_sector >= sector_cnt ||
        priv->end_sector   >= sector_cnt ||
        priv->end_sector   <  priv->start_sector)
        return CYG_DATAFLASH_ERR_INVALID;        
    
    priv->start_page = cyg_dataflash_get_block_size(df_dev) *
        cyg_dataflash_get_sector_start(df_dev, priv->start_sector);

    priv->end_page = cyg_dataflash_get_block_size(df_dev) *
        (cyg_dataflash_get_sector_start(df_dev, priv->end_sector) +
         cyg_dataflash_get_sector_size(df_dev, priv->end_sector));
    
    dev->end   = (cyg_flashaddr_t)(dev->start +
        ((priv->end_page - priv->start_page) *
         cyg_dataflash_get_page_size(df_dev)) - 1);
    priv->block_info[0].block_size = cyg_dataflash_get_page_size(df_dev);
    priv->block_info[0].blocks     = priv->end_page - priv->start_page;

    return CYG_DATAFLASH_ERR_OK;
}

static int 
df_flash_erase_block(struct cyg_flash_dev   *dev, 
                     cyg_flashaddr_t  base)
{
    cyg_dataflash_flash_dev_priv_t   *priv;
    cyg_uint32 page;
    int        err;
    
    priv   = (cyg_dataflash_flash_dev_priv_t *) dev->priv;
    
    page = priv->start_page + ((base - dev->start) / 
           cyg_dataflash_get_page_size(&priv->dev)); 
   
    RETURN_ON_ERROR( cyg_dataflash_aquire(&priv->dev)                       );
    GOTO_ON_ERROR(   cyg_dataflash_erase(&priv->dev, page)                  ); 
    RETURN_ON_ERROR( cyg_dataflash_release(&priv->dev)                      );
    
    return CYG_DATAFLASH_ERR_OK;
    
on_error:
    cyg_dataflash_release(&priv->dev);
    return df_flash_hwr_map_error(dev, err);
}

static int
df_flash_program(struct cyg_flash_dev *dev, 
                 cyg_flashaddr_t       base, 
                 const void           *data, 
                 size_t                len)
{
    cyg_dataflash_flash_dev_priv_t   *priv;
    cyg_uint32 page, pos;
    int        err;
 
    priv   = (cyg_dataflash_flash_dev_priv_t *) dev->priv;
    
    page = priv->start_page + ((base - dev->start) / 
           cyg_dataflash_get_page_size(&priv->dev)); 
    pos  = (base - dev->start) % cyg_dataflash_get_page_size(&priv->dev);
    
    RETURN_ON_ERROR( cyg_dataflash_aquire(&priv->dev)                       );
    GOTO_ON_ERROR(   cyg_dataflash_mem_to_buf(&priv->dev, 1, page)          );
    GOTO_ON_ERROR(   cyg_dataflash_write_buf(&priv->dev, 1, data, len, pos) );
    GOTO_ON_ERROR(   cyg_dataflash_program_buf(&priv->dev, 1, page, true)   );
    RETURN_ON_ERROR( cyg_dataflash_release(&priv->dev)                      );

    return CYG_DATAFLASH_ERR_OK;

on_error:
    cyg_dataflash_release(&priv->dev);
    return df_flash_hwr_map_error(dev, err);
}

static int 
df_flash_read(struct cyg_flash_dev   *dev, 
              const  cyg_flashaddr_t  base, 
              void                   *data, 
              size_t                  len)
{
    cyg_dataflash_flash_dev_priv_t   *priv;
    cyg_uint32 page, pos;
    int        err;
 
    priv   = (cyg_dataflash_flash_dev_priv_t *) dev->priv;
    
    page = priv->start_page + ((base - dev->start) / 
           cyg_dataflash_get_page_size(&priv->dev)); 
    pos  = (base - dev->start) % cyg_dataflash_get_page_size(&priv->dev);
 
    RETURN_ON_ERROR( cyg_dataflash_aquire(&priv->dev)                       );
    GOTO_ON_ERROR(   cyg_dataflash_mem_to_buf(&priv->dev, 1, page)          );
    GOTO_ON_ERROR(   cyg_dataflash_read_buf(&priv->dev, 1, data, len, pos)  );
    RETURN_ON_ERROR( cyg_dataflash_release(&priv->dev)                      );
    
    return CYG_DATAFLASH_ERR_OK;

on_error:
    cyg_dataflash_release(&priv->dev);
    return df_flash_hwr_map_error(dev, err);    
}

// -------------------------------------------------------------------------- 

CYG_FLASH_FUNS(cyg_dataflash_flash_dev_funs,
               df_flash_init,
               cyg_flash_devfn_query_nop,
               df_flash_erase_block,
               df_flash_program,
               df_flash_read,
               cyg_flash_devfn_lock_nop,
               cyg_flash_devfn_unlock_nop
);

//----------------------------------------------------------------------------
// End of devs_flash_atmel_dataflash_flash_dev_funs.c
