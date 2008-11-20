//==========================================================================
//
//      synth.c
//
//      Flash programming
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
// Author(s):    andrew.lunn@ascom.ch
// Contributors: jlarmour
// Date:         2001-10-30
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_flash_synth_v2.h>

#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>
#include <string.h>

#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>
#include <cyg/flash/synth.h>

#ifndef MIN
#define MIN(x,y) ((x)<(y) ? (x) : (y))
#endif

static int
synth_flash_init(struct cyg_flash_dev *dev)
{
    struct cyg_flash_synth_priv *priv = (struct cyg_flash_synth_priv*)dev->priv;
    cyg_flashaddr_t base;
    int flags=CYG_HAL_SYS_MAP_SHARED;
    
    priv->flashfd = 
        cyg_hal_sys_open(priv->filename,
                         CYG_HAL_SYS_O_RDWR, 
                         CYG_HAL_SYS_S_IRWXU|CYG_HAL_SYS_S_IRWXG|
                         CYG_HAL_SYS_S_IRWXO);
    if (priv->flashfd == -ENOENT) {
        long w, bytesleft;
        char buf[128];
        
        priv->flashfd = cyg_hal_sys_open(
            priv->filename, 
            CYG_HAL_SYS_O_RDWR|CYG_HAL_SYS_O_CREAT, 
            CYG_HAL_SYS_S_IRWXU|CYG_HAL_SYS_S_IRWXG|CYG_HAL_SYS_S_IRWXO);
        CYG_ASSERT( priv->flashfd >= 0, 
                    "Opening of the file for the synth flash failed!");
        // fill with 0xff
        memset( buf, 0xff, sizeof(buf) );
        bytesleft = priv->block_size * priv->blocks +
            priv->boot_block_size * priv->boot_blocks;

        while (bytesleft > 0) {
            int bytesneeded;
            bytesneeded = bytesleft < sizeof(buf) ?  
                bytesleft : sizeof(buf);
            
            w = cyg_hal_sys_write( priv->flashfd, buf,
                                   bytesneeded );
            CYG_ASSERT(w == bytesneeded, 
                       "initialization of flash file failed");
            bytesleft -= bytesneeded;
        } // while
    }
    CYG_ASSERT( priv->flashfd >= 0, 
                "Opening of the file for the synth flash failed!");
    if ( priv->flashfd <= 0 ) {
        return CYG_FLASH_ERR_HWR;
    }

    if (dev->start != 0) {
        flags |= CYG_HAL_SYS_MAP_FIXED;
    }
    base = (cyg_flashaddr_t)cyg_hal_sys_mmap( 
        (void *)dev->start,
        priv->blocks * priv->block_size + 
        priv->boot_block_size * priv->boot_blocks,
        CYG_HAL_SYS_PROT_READ, 
        flags,
        priv->flashfd, 
        0l);
    CYG_ASSERT( (int) base > 0, "mmap of flash file failed!" );
    if (base <= 0) {
        return CYG_FLASH_ERR_HWR;
    }
    dev->start = base;
    dev->end = base + (priv->blocks * priv->block_size) +
        (priv->boot_blocks * priv->boot_block_size) - 1;
    if (priv->boot_blocks) {
        if (priv->boot_block_bottom) {
            priv->block_info[0].block_size = priv->boot_block_size;
            priv->block_info[0].blocks = priv->boot_blocks;
            priv->block_info[1].block_size = priv->block_size;
            priv->block_info[1].blocks = priv->blocks;
        } else {
            priv->block_info[0].block_size = priv->block_size;
            priv->block_info[0].blocks = priv->blocks;
            priv->block_info[1].block_size = priv->boot_block_size;
            priv->block_info[1].blocks = priv->boot_blocks;
        }
        dev->num_block_infos = 2;
    } else {
        priv->block_info[0].block_size = priv->block_size;
        priv->block_info[0].blocks = priv->blocks;
        dev->num_block_infos = 1;
    }
    dev->block_info = &priv->block_info[0];

    return CYG_FLASH_ERR_OK;
}

/* This helps speed up the erase. */
static char empty[4096];
static cyg_bool empty_inited = false;

// Return the size of the block which is at the given address.
// __inline__ so that we know it will be in RAM, not ROM.
static __inline__ size_t 
flash_block_size(struct cyg_flash_dev *dev, const cyg_flashaddr_t addr)
{
  int i;
  size_t offset;
  
  
  CYG_ASSERT((addr >= dev->start) && (addr <= dev->end), "Not inside device");
  
  offset = addr - dev->start;
  for (i=0; i < dev->num_block_infos; i++) {
    if (offset < (dev->block_info[i].blocks * dev->block_info[i].block_size))
      return dev->block_info[i].block_size;
    offset = offset - 
      (dev->block_info[i].blocks * dev->block_info[i].block_size);
  }
  CYG_FAIL("Programming error");
  return 0;
}

static int 
synth_flash_erase_block(struct cyg_flash_dev *dev, 
                        cyg_flashaddr_t block_base)
{
    const struct cyg_flash_synth_priv *priv = dev->priv;
    int offset = (int)block_base;
    size_t remaining;
    int write_size;

    offset -= dev->start;
    
    cyg_hal_sys_lseek(priv->flashfd, offset,
                      CYG_HAL_SYS_SEEK_SET);
    
    if (!empty_inited) {
        memset(empty, 0xff, sizeof(empty));
        empty_inited = true;
    }

    remaining = flash_block_size(dev, block_base);

    while (remaining) {
      write_size = MIN(remaining, sizeof(empty));
      cyg_hal_sys_write(priv->flashfd, empty, write_size);
      remaining -= write_size;
    }
    return CYG_FLASH_ERR_OK;
}

static int
synth_flash_program (struct cyg_flash_dev *dev, 
                     cyg_flashaddr_t base, 
                     const void* data, size_t len)
{
    const struct cyg_flash_synth_priv *priv = dev->priv;
    int offset = base;
    offset -= dev->start;
    
    cyg_hal_sys_lseek(priv->flashfd, offset, CYG_HAL_SYS_SEEK_SET);
    cyg_hal_sys_write(priv->flashfd, data, len);
  
    return CYG_FLASH_ERR_OK;
}

#define QUERY "Linux Synthetic Flash" 

static size_t
synth_flash_query(struct cyg_flash_dev *dev, void * data, 
                  size_t len)
{
    memcpy(data,QUERY,sizeof(QUERY));
    return sizeof(QUERY);
}

const CYG_FLASH_FUNS(cyg_flash_synth_funs,
                     synth_flash_init,
                     synth_flash_query,
                     synth_flash_erase_block,
                     synth_flash_program,
                     NULL,                 // read
                     cyg_flash_devfn_lock_nop,
                     cyg_flash_devfn_unlock_nop);

static struct cyg_flash_synth_priv synth_flash_priv = {
    .block_size         = CYGNUM_FLASH_SYNTH_V2_BLOCKSIZE,
    .blocks             = CYGNUM_FLASH_SYNTH_V2_NUMBLOCKS,
    .boot_block_size    = CYGNUM_FLASH_SYNTH_V2_BOOT_BLOCKSIZE,
    .boot_blocks        = CYGNUM_FLASH_SYNTH_V2_NUMBOOT_BLOCKS,
    .boot_block_bottom  = CYGNUM_FLASH_SYNTH_V2_BOOT_BLOCK_BOTTOM,
    .filename           = CYGDAT_FLASH_SYNTH_V2_FILENAME,
    .flashfd            = -1
};

CYG_FLASH_DRIVER(cyg_flash_synth_flashdev,
                 &cyg_flash_synth_funs,
                 0,                             // flags
                 CYGMEM_FLASH_SYNTH_V2_BASE,    // Start, if 0 will be updated by init
                 0,                             // end, filled in by init
                 0,                             // number of block_info's, filled in by init
                 synth_flash_priv.block_info,
                 &synth_flash_priv);

// EOF synth.c
