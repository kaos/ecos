//==========================================================================
//
//      ide_disk.c
//
//      IDE polled mode disk driver 
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2004, 2006 Free Software Foundation, Inc.
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
// Author(s):    iz
// Contributors: 
// Date:         2004-10-16
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/devs_disk_ide.h>
#include <pkgconf/io_disk.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>             // delays
#include <cyg/hal/drv_api.h>
#include <cyg/io/io.h>
#include <cyg/io/devtab.h>
#include <cyg/io/disk.h>

#include "ide_disk.h"

// ----------------------------------------------------------------------------

#ifdef CYGDBG_IO_DISK_DEBUG
# define DEBUG 1
#endif

#ifdef DEBUG
# define D(fmt,args...) diag_printf(fmt, ## args)
#else
# define D(fmt,args...)
#endif

// ----------------------------------------------------------------------------

// No h/w controller structure is needed in this driver, but the address of the
// second argument is taken anyway.
DISK_CONTROLLER(ide_disk_controller, ide_disk_controller);

#ifdef CYGVAR_DEVS_DISK_IDE_DISK0
IDE_DISK_INSTANCE(0, 0, 0, true, CYGDAT_IO_DISK_IDE_DISK0_NAME);
#endif

#ifdef CYGVAR_DEVS_DISK_IDE_DISK1
IDE_DISK_INSTANCE(1, 0, 1, true, CYGDAT_IO_DISK_IDE_DISK1_NAME);
#endif

#ifdef CYGVAR_DEVS_DISK_IDE_DISK2
IDE_DISK_INSTANCE(2, 1, 0, true, CYGDAT_IO_DISK_IDE_DISK2_NAME);
#endif

#ifdef CYGVAR_DEVS_DISK_IDE_DISK3
IDE_DISK_INSTANCE(3, 1, 1, true, CYGDAT_IO_DISK_IDE_DISK3_NAME);
#endif

#define MIN(a,b) (((a) < (b)) ? (a) : (b))

// ----------------------------------------------------------------------------

static void
id_strcpy(char *dest, cyg_uint16 *src, cyg_uint16 size)
{
    int i;

    for (i=0; i<size; i+=2) {
        *dest++ = (char)(*src >> 8);
        *dest++ = (char)(*src & 0x00FF);
        src++;
    }
    *dest = '\0';
}

// ----------------------------------------------------------------------------

static inline void
__wait_for_ready(int ctlr)
{
    cyg_uint8 status;
    do {
         HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
    } while (status & (IDE_STAT_BSY | IDE_STAT_DRQ));
}

// ----------------------------------------------------------------------------
// Wait while the device is busy with the last command

static inline int
__wait_busy(int ctlr)
{
    cyg_uint8 status;
    cyg_ucount32 tries;
    
    for (tries=0; tries < 1000000; tries++) {
         CYGACC_CALL_IF_DELAY_US(10);
         HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
         if ((status & IDE_STAT_BSY) == 0)
              return 1;
    }
    return 0;   
}

// ----------------------------------------------------------------------------

static inline int
__wait_for_drq(int ctlr)
{
    cyg_uint8 status;
    cyg_ucount32 tries;

    for (tries=0; tries<1000000; tries++) {
        CYGACC_CALL_IF_DELAY_US(10);
        HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
        if (!(status & IDE_STAT_BSY)) {
            if (status & IDE_STAT_DRQ)
                return 1;
            else
                return 0;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------
// Return true if any devices attached to controller

static int
ide_presence_detect(int ctlr)
{
    cyg_uint8 sel, val;
    int i;

    for (i = 0; i < HAL_IDE_NUM_CONTROLLERS; i++) {
        sel = (i << 4) | 0xA0;
        CYGACC_CALL_IF_DELAY_US(50000U);
        HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE, sel);
        CYGACC_CALL_IF_DELAY_US(50000U);
        HAL_IDE_READ_UINT8(ctlr, IDE_REG_DEVICE, val);
        if (val == sel) {
#ifndef CYGSEM_DEVS_DISK_IDE_VMWARE
            if (i)
                HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE, 0);
#endif
            return 1;
        }
    }
    return 0;
}

// ----------------------------------------------------------------------------

static int
ide_reset(int ctlr)
{
    cyg_uint8 status;
    int delay;
//
// VMware note:
// VMware virtual IDE device handler obviously expects that
// the reset and setup functions were already done
// by it's bios and complains if one uses reset here...
//
#ifndef CYGSEM_DEVS_DISK_IDE_VMWARE
    HAL_IDE_WRITE_CONTROL(ctlr, 6);    // polled mode, reset asserted
    CYGACC_CALL_IF_DELAY_US(5000U);
    HAL_IDE_WRITE_CONTROL(ctlr, 2);   // polled mode, reset cleared
    CYGACC_CALL_IF_DELAY_US(50000U);
#endif

    // wait 30 seconds max for not busy and drive ready
    for (delay=0; delay<300; ++delay) {
        CYGACC_CALL_IF_DELAY_US(100000U);
        HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, status);
            if (!(status & IDE_STAT_BSY)) {
                if (status & IDE_STAT_DRDY) {
                    return 1;
                }
            }
    }
    return 0;
}

// ----------------------------------------------------------------------------

static int
ide_ident(int ctlr, int dev, cyg_uint16 *buf)
{
    int i;

    if (!__wait_busy(ctlr))
         return 0;

    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE, dev << 4);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND, 0xEC);
    CYGACC_CALL_IF_DELAY_US(50000U);

    if (!__wait_for_drq(ctlr))
         return 0;

    for (i=0; i<(CYGDAT_DEVS_DISK_IDE_SECTOR_SIZE/sizeof(cyg_uint16)); 
         i++, buf++) {
      HAL_IDE_READ_UINT16(ctlr, IDE_REG_DATA, *buf);
    }
    return 1;
}

// ----------------------------------------------------------------------------
// Requests the disk to use an 8-bit data path. This is probably ignored by
// most modern drives, but is supported by compact flash devices.

#ifdef CYGDAT_DEVS_DISK_IDE_8_BIT_DATA_PATH
static int
ide_8bit_mode(int ctlr, int dev, cyg_bool on)
{
    cyg_uint8 stat;

    if (!__wait_busy(ctlr))
         return 0;
    
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE, dev << 4);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_FEATURES, 0x01);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND,  0xEF);

    if (!__wait_busy(ctlr))
        return 0;

    HAL_IDE_READ_UINT8(ctlr, IDE_REG_STATUS, stat);
    return (stat & 1) ? 0 : 1;
}
#endif

// ----------------------------------------------------------------------------
// Reads a group of contiguous sectors from the drive.
// It can read up to 256 sectors.

static int
ide_read_sector(int ctlr, int dev, cyg_uint32 start, 
                cyg_uint8 *buf, cyg_uint32 len)
{
    int i, nword;
    cyg_uint8 lenb;
    cyg_uint16 w;

    if (len==0 || !__wait_busy(ctlr))
       return 0;

    len = MIN(len, 256);
    lenb = (len == 256) ? 0 : ((cyg_uint8) len);

    nword = len * CYGDAT_DEVS_DISK_IDE_SECTOR_SIZE / sizeof(cyg_uint16);

    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COUNT, lenb);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBALOW, start & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAMID, (start >>  8) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAHI,  (start >> 16) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE,
          ((start >> 24) & 0xf) | (dev << 4) | 0x40);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND, 0x20);

    if (!__wait_for_drq(ctlr))
        return 0;

    if ((int) buf & 1) {
        // Unaligned buffer, so split each word manually
        for (i=0; i<nword; i++) {
            HAL_IDE_READ_UINT16(ctlr, IDE_REG_DATA, w);
            *buf++ = w & 0xff;
            *buf++ = (w>>8) & 0xff;
        }
    }
    else {
        cyg_uint16* wbuf = (cyg_uint16*) buf;
        for (i=0; i<nword; i++, wbuf++)
            HAL_IDE_READ_UINT16(ctlr, IDE_REG_DATA, *wbuf);
    }
    return (int) len;
}

// ----------------------------------------------------------------------------
// Writes a group of contiguous sectors to the drive.
// It can write up to 256 sectors.

static int
ide_write_sector(int ctlr, int dev, cyg_uint32 start, 
                 cyg_uint8 *buf, cyg_uint32 len)
{
    int i, nword;
    cyg_uint8 lenb;
    cyg_uint16 w;

    if (len==0 || !__wait_busy(ctlr))
       return 0;

    len = MIN(len, 256);
    lenb = (len == 256) ? 0 : ((cyg_uint8) len);

    nword = len * CYGDAT_DEVS_DISK_IDE_SECTOR_SIZE / sizeof(cyg_uint16);

    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COUNT, lenb);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBALOW, start & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAMID, (start >>  8) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_LBAHI,  (start >> 16) & 0xff);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_DEVICE,
                            ((start >> 24) & 0xf) | (dev << 4) | 0x40);
    HAL_IDE_WRITE_UINT8(ctlr, IDE_REG_COMMAND, 0x30);

    if (!__wait_for_drq(ctlr))
        return 0;

    if ((int) buf & 1) {
        // Unaligned buffer, so assemble each word manually
        for (i=0; i<nword; ++i) {
            w  = *buf++;
            w |= (cyg_uint16) (*buf++) << 8;
            HAL_IDE_WRITE_UINT16(ctlr, IDE_REG_DATA, w);
        }
    }
    else {
        cyg_uint16* wbuf = (cyg_uint16*) buf;
        for (i=0; i<nword; ++i)
            HAL_IDE_WRITE_UINT16(ctlr, IDE_REG_DATA, *wbuf++);
    }
    return (int) len;
}

// ----------------------------------------------------------------------------

static cyg_bool 
ide_disk_init(struct cyg_devtab_entry *tab)
{
    disk_channel *chan = (disk_channel *) tab->priv;
    ide_disk_info_t *info = (ide_disk_info_t *) chan->dev_priv;
    cyg_uint32 id_buf[CYGDAT_DEVS_DISK_IDE_SECTOR_SIZE/sizeof(cyg_uint32)];
    static int num_controllers;
    static int ide_present[4], ide_reset_done[4];
    cyg_disk_identify_t ident;
    ide_identify_data_t *ide_idData=(ide_identify_data_t*)id_buf;
    
    if (chan->init) 
        return true;

    D("IDE(%d:%d) hw init\n", info->port, info->chan);
    
    if (!num_controllers) num_controllers=HAL_IDE_INIT();
    if (info->chan>=num_controllers) {
        D("No IDE controller for channel %d:%d\n", info->port, info->chan);
        return false;
    }

#if CYGDAT_DEVS_DISK_IDE_STARTUP_DELAY
    CYGACC_CALL_IF_DELAY_US(CYGDAT_DEVS_DISK_IDE_STARTUP_DELAY*1000U);
#endif

    if (!ide_present[info->port]) {
        ide_present[info->port]=ide_presence_detect(info->port);
        if (!ide_present[info->port]) {
            diag_printf("No devices on IDE controller #%d\n",info->port);
            return false;
        }
    }
    if (!ide_reset_done[info->port]) {
        ide_reset_done[info->port]=ide_reset(info->port);
        if (!ide_reset_done[info->port]) {
            D("Controller #%d reset failure\n",info->port);
            return false;
        }
    }

#ifdef CYGDAT_DEVS_DISK_IDE_8_BIT_DATA_PATH
    if (!ide_8bit_mode(info->port, info->chan, true)) {
        D("IDE disk %d:%d failed to enter 8-bit mode\n",
          info->port, info->chan);
    }
#endif
    
    D("IDE %d:%d identify drive\n", info->port, info->chan);
    
    if (!ide_ident(info->port, info->chan, (cyg_uint16 *)id_buf)) {
        diag_printf("IDE %d:%d ident DRQ error\n", info->port, info->chan);
        return false;
    }

    id_strcpy(ident.serial, ide_idData->serial, 20);
    id_strcpy(ident.firmware_rev, ide_idData->firmware_rev, 8);
    id_strcpy(ident.model_num, ide_idData->model_num, 40);
    
    ident.cylinders_num  = ide_idData->num_cylinders;
    ident.heads_num = ide_idData->num_heads;
    ident.sectors_num = ide_idData->num_sectors;
    ident.lba_sectors_num = ide_idData->lba_total_sectors[1] << 16 | 
                            ide_idData->lba_total_sectors[0];
    ident.phys_block_size = 1;

    // TODO: Should this be CYGDAT_DEVS_DISK_IDE_SECTOR_SIZE?
    ident.max_transfer = 512;
    
    D("\tSerial : %s\n", ident.serial);
    D("\tFirmware rev. : %s\n", ident.firmware_rev);
    D("\tModel : %s\n", ident.model_num);
    D("\tC/H/S : %d/%d/%d\n", ident.cylinders_num, 
                              ident.heads_num, ident.sectors_num);

    if (!(chan->callbacks->disk_init)(tab))
        return false;

    if (ENOERR != (chan->callbacks->disk_connected)(tab, &ident))
        return false;

    return true;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo 
ide_disk_lookup(struct cyg_devtab_entry **tab, 
                struct cyg_devtab_entry  *sub_tab,
                const char *name)
{
    disk_channel *chan = (disk_channel *) (*tab)->priv;
    return (chan->callbacks->disk_lookup)(tab, sub_tab, name);
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo 
ide_disk_read(disk_channel *chan, 
              void         *buf,
              cyg_uint32    len, 
              cyg_uint32    block_num)
{
    ide_disk_info_t *info = (ide_disk_info_t *)chan->dev_priv;

    D("IDE %d:%d read block %d\n", info->port, info->chan, block_num);

    if (!ide_read_sector(info->port, info->chan, block_num, 
                         (cyg_uint8 *)buf, len)) {
        diag_printf("IDE %d:%d read DRQ error\n", info->port, info->chan);
        return -EIO; 
    }

    return ENOERR;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo 
ide_disk_write(disk_channel *chan, 
               const void   *buf,
               cyg_uint32    len, 
               cyg_uint32    block_num)
{
    ide_disk_info_t *info = (ide_disk_info_t *)chan->dev_priv;

    D("IDE %d:%d write block %d\n", info->port, info->chan, block_num);

    if (!ide_write_sector(info->port, info->chan, block_num, 
                         (cyg_uint8 *)buf, len)) {
        diag_printf("IDE %d:%d read DRQ error\n", info->port, info->chan);
        return -EIO; 
    }
        
    return ENOERR;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo
ide_disk_get_config(disk_channel *chan, 
                    cyg_uint32    key,
                    const void   *xbuf, 
                    cyg_uint32   *len)
{
    return -EINVAL;
}

// ----------------------------------------------------------------------------

static Cyg_ErrNo
ide_disk_set_config(disk_channel *chan, 
                    cyg_uint32    key,
                    const void   *xbuf, 
                    cyg_uint32   *len)
{
    return -EINVAL;
}

//EOF ide_disk.c
