//==========================================================================
//
//      devs_flash_atmel_dataflash.c 
//
//      Atmel DataFlash series flash driver
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
// Date:         2004-08-27
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/devs_flash_atmel_dataflash.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>
#include <cyg/io/dataflash.h>

// -------------------------------------------------------------------------- 
// DataFlash command opcodes

// Read commands opcodes
#define DF_CONT_ARRAY_READ_CMD              0x68
#define DF_MMEM_PAGE_READ_CMD               0x52
#define DF_BUF1_READ_CMD                    0x54
#define DF_BUF2_READ_CMD                    0x56
#define DF_STATUS_READ_CMD                  0x57

// Program and erase commands opcodes
#define DF_BUF1_WRITE_CMD                   0x84
#define DF_BUF2_WRITE_CMD                   0x87
#define DF_BUF1_PROG_W_ERASE_CMD            0x83
#define DF_BUF2_PROG_W_ERASE_CMD            0x86
#define DF_BUF1_PROG_WO_ERASE_CMD           0x88
#define DF_BUF2_PROG_WO_ERASE_CMD           0x89
#define DF_PAGE_ERASE_CMD                   0x81
#define DF_BLOCK_ERASE_CMD                  0x50
#define DF_PROG_THROUGH_BUF1_CMD            0x82  
#define DF_PROG_THROUGH_BUF2_CMD            0x85

// Additional commands opcodes
#define DF_TRANSFER_TO_BUF1_CMD             0x53
#define DF_TRANSFER_TO_BUF2_CMD             0x55
#define DF_BUF1_COMPARE_CMD                 0x60
#define DF_BUF2_COMPARE_CMD                 0x61
#define DF_AUTO_REWRITE_THROUGH_BUF1_CMD    0x58
#define DF_AUTO_REWRITE_THROUGH_BUF2_CMD    0x59

//----------------------------------------------------------------------------

#define DATA_BUF_NONE 0x00 // Data buffer number for no buffer
#define DATA_BUF_ALL  0xFF // Data buffer number for all buffers

//----------------------------------------------------------------------------

typedef struct df_status_s
{
    cyg_uint8 reserved:2;
    cyg_uint8 device_id:4;
    cyg_uint8 compare_err:1;
    cyg_uint8 ready:1;
} df_status_t;

//----------------------------------------------------------------------------

static const cyg_dataflash_dev_info_t df_dev_info[] =
{
    {   // AT45DB011B
        device_id:    0x03,
        page_size:    264,
        page_count:   512,
        baddr_bits:   9,  
        block_size:   8,
        sector_sizes: { 1, 31, 32 },          
        sector_count: 3
    },
    {   // AT45DB021B
        device_id:    0x05,
        page_size:    264,
        page_count:   1024, 
        baddr_bits:   9,
        block_size:   8,
        sector_sizes: { 1, 31, 32, 64 },
        sector_count: 4
    },
    {   // AT45DB041B
        device_id:    0x07,
        page_size:    264,
        page_count:   2048, 
        baddr_bits:   9,
        block_size:   8,
        sector_sizes: { 1, 31, 32, 64, 64, 64 },
        sector_count: 6
    },
    {   // AT45DB081B
        device_id:    0x09,
        page_size:    264,
        page_count:   4096, 
        baddr_bits:   9,
        block_size:   8,
        sector_sizes: { 1, 31, 32, 64, 64, 64, 64, 64, 64, 64 },
        sector_count: 10 
    },
    {   // AT45DB161B
        device_id:    0x0B,
        page_size:    528,
        page_count:   4096, 
        baddr_bits:   10,
        block_size:   8,
        sector_sizes: {  1, 31, 32, 32, 32, 32, 32, 32, 
                        32, 32, 32, 32, 32, 32, 32, 32, 32 },
        sector_count: 17 
    },
    {   // AT45DB321B
        device_id:    0x0D,
        page_size:    528,
        page_count:   8192,
        baddr_bits:   10,
        block_size:   8,
        sector_sizes: {  1, 63, 64, 64, 64, 64, 64, 64, 
                        64, 64, 64, 64, 64, 64, 64, 64, 64 },
        sector_count: 17 
    },
    {   // AT45DB642
        device_id:    0x0F,
        page_size:    1056,
        page_count:   8192, 
        baddr_bits:   11,
        block_size:   8,
        sector_sizes: {  1, 31, 32, 32, 32, 32, 32, 32,
                        32, 32, 32, 32, 32, 32, 32, 32,
                        32, 32, 32, 32, 32, 32, 32, 32,
                        32, 32, 32, 32, 32, 32, 32, 32, 32 },
        sector_count: 35
    },
    { 0 }
};

//----------------------------------------------------------------------------

static void
df_compose_addr(cyg_dataflash_device_t *dev,
                cyg_uint8              *cmd_buf, 
                cyg_uint16              page_addr, 
                cyg_uint16              byte_addr)
{
    cyg_uint32 baddr_bits = dev->info->baddr_bits;
    cyg_uint32 addr; 

    addr = (page_addr << baddr_bits) | (byte_addr & ((1<<baddr_bits) - 1));
    
    cmd_buf[1] = (addr >> 16) & 0xFF;
    cmd_buf[2] = (addr >>  8) & 0xFF;
    cmd_buf[3] = addr         & 0xFF;
}
        
static df_status_t
df_read_status(cyg_dataflash_device_t *dev)
{
    cyg_spi_device  *spi_dev    = dev->spi_dev;
    const cyg_uint8  cmd_buf[2] = { DF_STATUS_READ_CMD, 0 };
    cyg_uint8        rx_buf[2];
    df_status_t     *status;
   
    cyg_spi_transaction_transfer(spi_dev, true, 2, cmd_buf, rx_buf, true);

    status = (df_status_t *) &rx_buf[1];

    return *status;    
}

static void
df_set_busy_buf(cyg_dataflash_device_t *dev, cyg_uint8 buf_num)
{
    dev->state    = CYG_DATAFLASH_STATE_BUSY;
    dev->busy_buf = buf_num;
}

static void
df_set_busy(cyg_dataflash_device_t *dev)
{
    df_set_busy_buf(dev, DATA_BUF_ALL);
}

static int
df_wait_ready_buf(cyg_dataflash_device_t *dev, cyg_uint8 buf_num)
{
    df_status_t status;
  
    if (CYG_DATAFLASH_STATE_IDLE == dev->state)
        return CYG_DATAFLASH_ERR_OK;
    
    if (DATA_BUF_ALL  == buf_num       || 
        DATA_BUF_ALL  == dev->busy_buf || 
        dev->busy_buf == buf_num)
    {
        // REMIND: this loop should have an timeout 
        // in case of device malfunction

        do 
        {
            status = df_read_status(dev);
        } while (0 == status.ready); 

        dev->state    = CYG_DATAFLASH_STATE_IDLE;
        dev->busy_buf = DATA_BUF_NONE;
    }
    
    return CYG_DATAFLASH_ERR_OK;  
   
}

static int
df_wait_ready(cyg_dataflash_device_t *dev)
{
    return df_wait_ready_buf(dev, DATA_BUF_ALL);
}

static void 
df_detect_device(cyg_dataflash_device_t *dev)
{
    const cyg_dataflash_dev_info_t *dev_info;
    cyg_spi_device                 *spi_dev = dev->spi_dev;
    df_status_t                     status;
    
    cyg_spi_transaction_begin(spi_dev);
    status = df_read_status(dev);
    cyg_spi_transaction_end(spi_dev);
   
    dev_info = df_dev_info;

    while (dev_info->device_id != 0)
    {
        if (status.device_id == dev_info->device_id)
        {
            dev->info = dev_info;
            return;
        }
        dev_info++;
    }
    dev->info = NULL;
}

//----------------------------------------------------------------------------
// cyg_dataflash_init()

int
cyg_dataflash_init(cyg_bool                polled,
                   cyg_dataflash_device_t *dev)
{
    dev->polled   = polled;
    dev->blocking = false;
    dev->state    = CYG_DATAFLASH_STATE_IDLE;
    dev->busy_buf = DATA_BUF_NONE;

    cyg_drv_mutex_init(&dev->lock);

    df_detect_device(dev);

    if (NULL == dev->info)
        return CYG_DATAFLASH_ERR_WRONG_PART;
    else
        return CYG_DATAFLASH_ERR_OK;
}

//----------------------------------------------------------------------------
// cyg_dataflash_aquire()

int
cyg_dataflash_aquire(cyg_dataflash_device_t *dev)
{
    while (!cyg_drv_mutex_lock(&dev->lock));
    return CYG_DATAFLASH_ERR_OK; 
}

//----------------------------------------------------------------------------
// cyg_dataflash_release()

int
cyg_dataflash_release(cyg_dataflash_device_t *dev)
{
    int err;

    err = df_wait_ready(dev); 
        
    cyg_drv_mutex_unlock(&dev->lock);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_get_sector_start()

cyg_uint16
cyg_dataflash_get_sector_start(cyg_dataflash_device_t *dev, 
                               cyg_uint16              sector_num)
{
    cyg_uint16 res, i;
    
    if (sector_num >= dev->info->sector_count)
        return 0;

    res = 0;
    for (i = 0; i < sector_num; i++)
        res += dev->info->sector_sizes[i];

    return res;
}

//----------------------------------------------------------------------------
// cyg_dataflash_wait_ready()

int 
cyg_dataflash_wait_ready(cyg_dataflash_device_t *dev)
{
    return df_wait_ready(dev);
}

//----------------------------------------------------------------------------
// cyg_dataflash_read_buf()

int
cyg_dataflash_read_buf(cyg_dataflash_device_t *dev,
                       cyg_uint8               buf_num,
                       cyg_uint8              *buf, 
                       cyg_uint32              len, 
                       cyg_uint32              pos) 
                       
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[5];

    // Check if the position is inside the page 
    if (pos >= dev->info->page_size)
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    if (1 == buf_num)      cmd_buf[0] = DF_BUF1_READ_CMD;
    else if (2 == buf_num) cmd_buf[0] = DF_BUF2_READ_CMD;
    else 
        return CYG_DATAFLASH_ERR_INVALID;

    df_compose_addr(dev, cmd_buf, 0, pos);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the target buffer to become ready
    err = df_wait_ready_buf(dev, buf_num);

    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send command and read data

        cyg_spi_transaction_transfer(spi_dev, true, 5, cmd_buf, NULL, false);
        cyg_spi_transaction_transfer(spi_dev, dev->polled, len, buf, buf, true);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_write_buf()

int
cyg_dataflash_write_buf(cyg_dataflash_device_t *dev,
                        cyg_uint8               buf_num,
                        const cyg_uint8        *buf,
                        cyg_uint32              len,
                        cyg_uint32              pos)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];
 
    // Check if the position is inside the page 
    if (pos >= dev->info->page_size)
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    if (1 == buf_num)      cmd_buf[0] = DF_BUF1_WRITE_CMD;
    else if (2 == buf_num) cmd_buf[0] = DF_BUF2_WRITE_CMD;
    else 
        return CYG_DATAFLASH_ERR_INVALID;

    df_compose_addr(dev, cmd_buf, 0, pos);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the target buffer to become ready
    err = df_wait_ready_buf(dev, buf_num);
    
    if (CYG_DATAFLASH_ERR_OK == err)
    { 
        // Send command and data

        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, false);
        cyg_spi_transaction_transfer(spi_dev, dev->polled, 
                                     len, buf, NULL, true);
    }

    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_mem_to_buf()

int
cyg_dataflash_mem_to_buf(cyg_dataflash_device_t *dev,
                         cyg_uint8               buf_num,
                         cyg_uint32              page_num)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];
    
    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count)
        return CYG_DATAFLASH_ERR_INVALID;
    
    // Compose DataFlash command

    if (1 == buf_num)      cmd_buf[0] = DF_TRANSFER_TO_BUF1_CMD;
    else if (2 == buf_num) cmd_buf[0] = DF_TRANSFER_TO_BUF2_CMD;
    else 
        return CYG_DATAFLASH_ERR_INVALID;

    df_compose_addr(dev, cmd_buf, page_num, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);

    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Mark the target buffer as busy
        df_set_busy_buf(dev, buf_num);

        // Wait if in blocking mode     
        if (dev->blocking)
            err = df_wait_ready(dev);
    }

    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_program_buf()

int
cyg_dataflash_program_buf(cyg_dataflash_device_t *dev,
                          cyg_uint8               buf_num,
                          cyg_uint32              page_num,
                          cyg_bool                erase)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];
    
    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count)
        return CYG_DATAFLASH_ERR_INVALID;
    
    // Compose DataFlash command

    if (erase)
    {
        if (1 == buf_num)      cmd_buf[0] = DF_BUF1_PROG_W_ERASE_CMD;
        else if (2 == buf_num) cmd_buf[0] = DF_BUF2_PROG_W_ERASE_CMD; 
        else 
            return CYG_DATAFLASH_ERR_INVALID;
    }
    else
    {
        if (1 == buf_num)      cmd_buf[0] = DF_BUF1_PROG_WO_ERASE_CMD;
        else if (2 == buf_num) cmd_buf[0] = DF_BUF2_PROG_WO_ERASE_CMD; 
        else 
            return CYG_DATAFLASH_ERR_INVALID;
    }
    
    df_compose_addr(dev, cmd_buf, page_num, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);
    
    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Mark the target buffer as busy    
        df_set_busy_buf(dev, buf_num);

        // Wait if in blocking mode
        if (dev->blocking)
            err = df_wait_ready(dev);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_compare_buf()

int
cyg_dataflash_compare_buf(cyg_dataflash_device_t *dev,
                          cyg_uint8               buf_num,
                          cyg_uint32              page_num)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];
    df_status_t     status;

    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count)
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    if (1 == buf_num)      cmd_buf[0] = DF_BUF1_COMPARE_CMD;
    else if (2 == buf_num) cmd_buf[0] = DF_BUF2_COMPARE_CMD; 
    else 
        return CYG_DATAFLASH_ERR_INVALID;

    df_compose_addr(dev, cmd_buf, page_num, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);
   
    if (CYG_DATAFLASH_ERR_OK == err)
    { 
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Wait for the device to become ready
        df_set_busy(dev);
        err = df_wait_ready(dev);
    
        // Read the result of memory compare

        if (CYG_DATAFLASH_ERR_OK == err)
        {
            status = df_read_status(dev);
            if (status.compare_err)
                err = CYG_DATAFLASH_ERR_COMPARE;
        }
    }

    cyg_spi_transaction_end(spi_dev);

    return err; 
}

//----------------------------------------------------------------------------
// cyg_dataflash_erase()

int
cyg_dataflash_erase(cyg_dataflash_device_t *dev,
                    cyg_uint32              page_num)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];

    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count)
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    cmd_buf[0] = DF_PAGE_ERASE_CMD;
    df_compose_addr(dev, cmd_buf, page_num, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);
    
    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Set device state to busy
        df_set_busy_buf(dev, DATA_BUF_NONE);
        
        // Wait if in blocking mode
        if (dev->blocking)
            err = df_wait_ready(dev);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_erase_block()

int
cyg_dataflash_erase_block(cyg_dataflash_device_t *dev,
                          cyg_uint32              block_num)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];

    // Check if the block number is inside the flash
    if (block_num >= (dev->info->page_count >> 3))
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    cmd_buf[0] = DF_BLOCK_ERASE_CMD;
    df_compose_addr(dev, cmd_buf, block_num << 3, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);

    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Set device state to busy
        df_set_busy_buf(dev, DATA_BUF_NONE);
 
        // Wait if in blocking mode
        if (dev->blocking)
            err = df_wait_ready(dev);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_auto_rewrite()

int
cyg_dataflash_auto_rewrite(cyg_dataflash_device_t *dev,
                           cyg_uint8               buf_num,
                           cyg_uint32              page_num)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];

    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count)
        return CYG_DATAFLASH_ERR_INVALID;

    // Compose DataFlash command

    if (1 == buf_num)      cmd_buf[0] = DF_AUTO_REWRITE_THROUGH_BUF1_CMD;
    else if (2 == buf_num) cmd_buf[0] = DF_AUTO_REWRITE_THROUGH_BUF2_CMD; 
    else 
        return CYG_DATAFLASH_ERR_INVALID;

    df_compose_addr(dev, cmd_buf, page_num, 0);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);

    if (CYG_DATAFLASH_ERR_OK == err)
    {
        // Send the command 
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);

        // Mark the target buffer as busy
        df_set_busy_buf(dev, buf_num);

        // Wait if in blocking mode
        if (dev->blocking)
            err = df_wait_ready(dev);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_read()

int
cyg_dataflash_read(cyg_dataflash_device_t *dev, 
                   cyg_uint8              *buf, 
                   cyg_uint32              len, 
                   cyg_uint32              pos)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[8];
    cyg_uint32      page_num, page_pos;

    // Calculate page number and position from given absolute position

    page_num = pos / dev->info->page_size;
    page_pos = pos % dev->info->page_size;

    // Check if the page number is inside the flash
    if (page_num >= dev->info->page_count) 
        return CYG_DATAFLASH_ERR_INVALID;
    
    // Compose DataFlash command

    cmd_buf[0] = DF_CONT_ARRAY_READ_CMD;
    df_compose_addr(dev, cmd_buf, page_num, page_pos);

    cyg_spi_transaction_begin(spi_dev);

    // Wait for the device to become ready
    err = df_wait_ready(dev);

    if (CYG_DATAFLASH_ERR_OK == err)
    {    
        // Send the command and read data from DataFlash main memory
        
        cyg_spi_transaction_transfer(spi_dev, true, 8, cmd_buf, NULL, false);
        cyg_spi_transaction_transfer(spi_dev, dev->polled, len, buf, buf, true);
    }
    
    cyg_spi_transaction_end(spi_dev);

    return err;
}

//----------------------------------------------------------------------------
// cyg_dataflash_program()

int
cyg_dataflash_program(cyg_dataflash_device_t *dev, 
                      const cyg_uint8        *buf, 
                      cyg_uint32             *len, 
                      cyg_uint32              pos,
                      cyg_bool                erase,
                      cyg_bool                verify)
{
    cyg_spi_device *spi_dev = dev->spi_dev;
    int             err     = CYG_DATAFLASH_ERR_OK;
    cyg_uint8       cmd_buf[4];
    cyg_uint32      count, page_num, page_pos;
   
    // Calculate page number and position from given absolute position

    page_num = pos / dev->info->page_size;
    page_pos = pos % dev->info->page_size;
    count    = *len;

    cyg_spi_transaction_begin(spi_dev);

    // Wait for device to become ready
    err = df_wait_ready(dev);
    if (CYG_DATAFLASH_ERR_OK != err)
        goto out;
   
    // Loop until count bytes written

    while (count > 0)
    { 
        df_status_t status;
        cyg_uint32  size;

        // Check if the current page number is inside the flash 
        if (page_num >= dev->info->page_count)
        {
            err = CYG_DATAFLASH_ERR_INVALID;
            goto out;
        }
       
        // Calculate the number of bytes to write to the current page
        if ((page_pos + count) > dev->info->page_size)
            size = dev->info->page_size - page_pos;
        else
            size = count;
        
        // Compose DataFlash command address
        df_compose_addr(dev, cmd_buf, page_num, page_pos);
       
        // If we are not rewritting the whole page, then first 
        // read the old data from main memory to the target buffer

        if (page_pos > 0 || size < dev->info->page_size)
        {
            cmd_buf[0] = DF_TRANSFER_TO_BUF1_CMD;
            cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);
            df_set_busy(dev);
            err = df_wait_ready(dev);
            if (CYG_DATAFLASH_ERR_OK != err)
                goto out;
        }
       
        // Write data to the target buffer

        cmd_buf[0] = DF_BUF1_WRITE_CMD;
        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, false);
        cyg_spi_transaction_transfer(spi_dev, dev->polled, 
                                     size, buf, NULL, true);
        df_set_busy(dev);
        err = df_wait_ready(dev);
        if (CYG_DATAFLASH_ERR_OK != err)
            goto out;
       
        // Program data from the target buffer to main memory 
        // and perform an erase before if requested
        
        if (erase)
            cmd_buf[0] = DF_BUF1_PROG_W_ERASE_CMD;
        else
            cmd_buf[0] = DF_BUF1_PROG_WO_ERASE_CMD;

        cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);
        df_set_busy(dev);
        err = df_wait_ready(dev);
        if (CYG_DATAFLASH_ERR_OK != err)
            goto out;
        
        // Compare the target buffer contents with the freshly 
        // written main memory page (if requested)
        
        if (verify)
        { 
            cmd_buf[0] = DF_BUF1_COMPARE_CMD;
            cyg_spi_transaction_transfer(spi_dev, true, 4, cmd_buf, NULL, true);
            df_set_busy(dev);
            err = df_wait_ready(dev);
            if (CYG_DATAFLASH_ERR_OK != err)
                goto out;
            
            status = df_read_status(dev);

            if (status.compare_err)
            {
                err = CYG_DATAFLASH_ERR_COMPARE;
                goto out;
            }
        }

        // Adjust running values

        page_pos  = 0;
        page_num += 1;
        count    -= size;
        buf      += size;
    }

out:
    cyg_spi_transaction_end(spi_dev);

    *len -= count;
    
    return err; 
}

//----------------------------------------------------------------------------
// End of devs_flash_atmel_dataflash.c
