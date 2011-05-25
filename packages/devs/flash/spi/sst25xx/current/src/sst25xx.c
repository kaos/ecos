//=============================================================================
//
//      sst25xx.c
//
//      SPI flash driver for Silicon Storage Technology SST25xx devices
//      and compatibles.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   ccoutand, updated for Silicon Storage Technology SST25xx
//              flash
// Original(s): Chris Holgate
// Date:        2011-04-25
// Purpose:     Silicon Storage Technology SST25xx SPI flash driver
//              implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/io/spi.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/devs_flash_spi_sst25xx.h>

#include <string.h>

//-----------------------------------------------------------------------------
// Enable polled SPI operation for non-kernel builds.

#ifdef CYGPKG_KERNEL
# define SST25XX_POLLED false
#else
# define SST25XX_POLLED true
#endif

//-----------------------------------------------------------------------------
// Implement delay functions for kernel and non-kernel builds.  The kernel
// build assumes that the API calls are made in the thread context.

#ifdef CYGPKG_KERNEL
#define SST25XX_DELAY_MS(_msdelay_) cyg_thread_delay \
    (1 + ((1000 * _msdelay_ * CYGNUM_HAL_RTC_DENOMINATOR) / (CYGNUM_HAL_RTC_NUMERATOR / 1000)))

#else
#define SST25XX_DELAY_MS(_msdelay_) CYGACC_CALL_IF_DELAY_US (_msdelay_ * 1000)
#endif

// Byte / word programming takes 10 us max
#define SST25XX_WAIT_CHIP_READY( dev )                  \
{                                                       \
    cyg_uint8 dev_status;                               \
    do {                                                \
        HAL_DELAY_US (10);                              \
        dev_status = sst25xx_spi_rdsr ( dev );          \
    } while ( dev_status & SST25XX_STATUS_BSY );        \
}

//-----------------------------------------------------------------------------
// Maintenance and debug macros.

#define ASSERT_SST25XX(_test_, _msg_) CYG_ASSERT(_test_, "FAIL (SST25XX) : " _msg_)
#define TRACE_SST25XX(_msg_, _args_...) if (dev->pf) dev->pf ("SST25XX : " _msg_, ##_args_)

//=============================================================================
// Define SST25VFxxx SPI protocol.
//=============================================================================

typedef enum sst25xx_cmd {
    SST25XX_CMD_WREN   = 0x06,     // Write enable.
    SST25XX_CMD_WDRI   = 0x04,     // Write disable.
    SST25XX_CMD_RDJID  = 0x9F,     // Read JEDEC identification.
    SST25XX_CMD_RDID   = 0x90,     // Read Manufacturer / Device identification.
    SST25XX_CMD_RDSR   = 0x05,     // Read status register.
    SST25XX_CMD_WRSR   = 0x01,     // Write status register.
    SST25XX_CMD_READ   = 0x03,     // Read data
    SST25XX_CMD_FREAD  = 0x0B,     // Read data (fast).
    SST25XX_CMD_BP     = 0x02,     // Byte program.
    SST25XX_CMD_AAI    = 0xAD,     // Word program (with address increment).
    SST25XX_CMD_SE_4K  = 0x20,     // 4K sector erase.
    SST25XX_CMD_SE_32K = 0x52,     // 32K sector erase.
    SST25XX_CMD_SE_64K = 0xD8,     // 64K sector erase.
    SST25XX_CMD_BE     = 0xC7,     // Chip erase.
} sst25xx_cmd;

// Status register bitfields.
#define SST25XX_STATUS_BSY  0x01   /* Operation in progress. */
#define SST25XX_STATUS_WEL  0x02   /* Write enable latch. */
#define SST25XX_STATUS_BP0  0x04   /* Block Write Protect 0. */
#define SST25XX_STATUS_BP1  0x08   /* Block Write Protect 1. */
#define SST25XX_STATUS_BP2  0x10   /* Block Write Protect 2. */
#define SST25XX_STATUS_BP3  0x20   /* Block Write Protect 3. */
#define SST25XX_STATUS_AAI  0x40   /* Auto Address Increment Programming Status */
#define SST25XX_STATUS_SPRL 0x80   /* Sector Protect Register Bit Lock. */

#define SST25XX_LOCK_BITS   ( SST25XX_STATUS_BP0 | SST25XX_STATUS_BP1 | \
                              SST25XX_STATUS_BP2 | SST25XX_STATUS_BP3 )

#define SST25XX_MEMORY_ID   0x25

// A few helper constants
#ifndef SZ_1K
# define SZ_1K           0x00000400
# define SZ_2K           0x00000800
# define SZ_4K           0x00001000
# define SZ_8K           0x00002000
# define SZ_16K          0x00004000
# define SZ_32K          0x00008000
# define SZ_64K          0x00010000
#endif

// Select read OPCODE
#if defined(CYGPKG_DEVS_FLASH_SPI_SST25XX_READ_MODE_SLOW)
# define SST25XX_CMD_READ_LEN 4
# define SST25XX_CMD_READ_OPCODE SST25XX_CMD_READ
#elif defined(CYGPKG_DEVS_FLASH_SPI_SST25XX_READ_MODE_FAST)
# define SST25XX_CMD_READ_LEN 5
# define SST25XX_CMD_READ_OPCODE SST25XX_CMD_FREAD
#endif

// Select sector size
#if CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE == SZ_64K
# define SST25XX_CMD_SE SST25XX_CMD_SE_64K
#elif CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE == SZ_32K
# define SST25XX_CMD_SE SST25XX_CMD_SE_32K
#elif CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE == SZ_4K
# define SST25XX_CMD_SE SST25XX_CMD_SE_4K
#endif

//=============================================================================
// Array containing a list of supported devices.  This allows the device
// parameters to be dynamically detected on initialization.
//=============================================================================

typedef struct sst25xx_params {
    cyg_uint16 sector_count[4];       // Number of sectors on device.
    cyg_uint32 sector_size[4];        // Supported sector size(s) in Byte
    cyg_uint32 jedec_id;              // 3 byte JEDEC identifier for this device.
} sst25xx_params;


static const sst25xx_params sst25xx_supported_devices [] = {
    // Some Silicon Storage Technology parts.
    // Note1: Some part do not support the Read Jedec ID Opcode, instead,
    // the Read ID Opcode is used which is backward compatible with all
    // parts
    // Note2: Not all parts can support sector size of 4, 32 or 64 KBytes
    { // Support for SST 64 MBit devices (SST25VF064C).
        sector_count        : {2048, 256, 128, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf254B
    },
    { // Support for SST 32 MBit devices (SST25VF032B).
        sector_count        : {1024, 128, 64, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf254a
    },
    { // Support for SST 16 MBit devices (SST25VF016B).
        sector_count        : {512, 64, 32, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf2541
    },
    { // Support for SST 8 MBit devices (SST25VF080B).
        sector_count        : {256, 32, 16, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf258E
    },
    { // Support for SST 4 MBit devices (SST25VF040B).
        sector_count        : {128, 16, 8, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf258D
    },
    { // Support for SST 2 MBit devices (SST25VF020B).
        sector_count        : {64, 8, 4, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf258C
    },
    { // Support for SST 2 MBit devices (SST25LF020A).
        sector_count        : {64, 8, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        // Constructed Jedec ID from ID (0xBF43)
        jedec_id            : 0x00bf2543
    },
    { // Support for SST 1 MBit devices (SST25VF010A).
        sector_count        : {32, 4, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        // Constructed Jedec ID from ID (0xBF49)
        jedec_id            : 0x00bf2549
    },
    { // Support for SST 512 KBit devices (SST25VF512A).
        sector_count        : {16, 2, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        // Constructed Jedec ID from ID (0xBF48)
        jedec_id            : 0x00bf2548
    },
    { // Support for SST 512 KBit devices (SST25WF512).
        sector_count        : {16, 2, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        jedec_id            : 0x00bf2501
    },
    { // Support for SST 1 MBit devices (SST25WF010).
        sector_count        : {32, 4, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        jedec_id            : 0x00bf2502
    },
    { // Support for SST 2 MBit devices (SST25WF020).
        sector_count        : {64, 8, 4},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf2503
    },
    { // Support for SST 4 MBit devices (SST25WF040).
        sector_count        : {128, 16, 8, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf2504
    },
    { // Support for SST 8 MBit devices (SST25WF080).
        sector_count        : {256, 32, 16, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        jedec_id            : 0x00bf2505
    },
    { // Null terminating entry.
        sector_count        : {0},
        sector_size         : {0},
        jedec_id            : 0
    }
};

//=============================================================================
// Utility functions for address calculations.
//=============================================================================

//-----------------------------------------------------------------------------
// Strips out any device address offset to give address within device.

static cyg_bool
sst25xx_to_local_addr(struct cyg_flash_dev *dev, cyg_flashaddr_t * addr)
{
    cyg_bool        retval = false;

    // Range check address before modifying it.
    if ((*addr >= dev->start) && (*addr <= dev->end)) {
        *addr -= dev->start;
        retval = true;
    }
    return retval;
}

//=============================================================================
// Wrapper functions for various SPI transactions.
//=============================================================================

//-----------------------------------------------------------------------------
// Read back the 3-byte JEDEC ID, returning it as a 32-bit integer.
// This function is called during flash initialization, which can often be
// called from the startup/idle thread.  This means that we should always use
// SPI polled mode in order to prevent the thread from attempting to sleep.

#if defined(CYGPKG_DEVS_FLASH_SPI_SST25XX_SUPPORT_JEDECID)

static inline cyg_uint32
sst25xx_spi_rdid(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { SST25XX_CMD_RDJID, 0x0, 0x0, 0x0 };
    cyg_uint8       rx_buf[4];
    cyg_uint32      retval = 0;

    // Carry out SPI transfer.
    cyg_spi_transfer(spi_device, true, 4, tx_buf, rx_buf);

    // Convert 3-byte ID to 32-bit integer.
    retval |= ((cyg_uint32)rx_buf[1]) << 16;
    retval |= ((cyg_uint32)rx_buf[2]) << 8;
    retval |= ((cyg_uint32)rx_buf[3]);

    return retval;
}

#else

static inline cyg_uint32
sst25xx_spi_rdid(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[6] = { SST25XX_CMD_RDID, 0x0, 0x0, 0x0, 0x0, 0x0 };
    cyg_uint8       rx_buf[6];
    cyg_uint32      retval = 0;

    // Carry out SPI transfer.
    cyg_spi_transfer(spi_device, true, 6, tx_buf, rx_buf);

    // Convert 2-byte ID to 32-bit Jedec ID.
    retval |= ((cyg_uint32)rx_buf[4]) << 16;
    retval |= (SST25XX_MEMORY_ID << 8);
    retval |= ((cyg_uint32)rx_buf[5]);

    return retval;
}

#endif

//-----------------------------------------------------------------------------
// Send write enable command.

static inline void
sst25xx_spi_wren(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[1] = { SST25XX_CMD_WREN };
    cyg_spi_transfer(spi_device, SST25XX_POLLED, 1, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send write disable command.

static inline void
sst25xx_spi_wrdis(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[1] = { SST25XX_CMD_WDRI };
    cyg_spi_transfer(spi_device, SST25XX_POLLED, 1, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send sector erase command.  The address parameter is a device local address
// within the sector to be erased.

static inline void
sst25xx_spi_se(struct cyg_flash_dev *dev, cyg_flashaddr_t addr)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    cyg_uint8       tx_buf[4] = { SST25XX_CMD_SE,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };
    cyg_spi_transfer(spi_device, SST25XX_POLLED, 4, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Read and return the 8-bit device status register.

static inline cyg_uint8
sst25xx_spi_rdsr(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[2] = { SST25XX_CMD_RDSR, 0 };
    cyg_uint8       rx_buf[2];

    // Carry out SPI transfer and return the status byte.
    cyg_spi_transfer(spi_device, SST25XX_POLLED, 2, tx_buf, rx_buf);

    return rx_buf[1];
}

//-----------------------------------------------------------------------------
// Program a single byte.

static inline void
sst25xx_spi_pbyte(struct cyg_flash_dev *dev, cyg_flashaddr_t addr,
                  cyg_uint8 *wbuf)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { SST25XX_CMD_BP,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };

    // Implement the program operation as a multistage SPI transaction.
    cyg_spi_transaction_begin(spi_device);
    cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, 4, tx_buf, NULL,
                                 false);
    cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, 1, wbuf, NULL,
                                 false);
    cyg_spi_transaction_end(spi_device);
}

//-----------------------------------------------------------------------------
// Program word with automatic address increment

static inline void
sst25xx_spi_pword(struct cyg_flash_dev *dev, cyg_flashaddr_t addr,
                  cyg_uint8 *wbuf, cyg_bool first)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { SST25XX_CMD_AAI,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };

    // Implement the program operation as a multistage SPI transaction.
    cyg_spi_transaction_begin(spi_device);
    // Only convey the address with the first transaction. In the following
    // transfers, address will be incremented internally by the SPI flash
    if (first == true) {
        cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, 4, tx_buf,
                                     NULL, false);
    } else {
        cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, 1, tx_buf,
                                     NULL, false);
    }
    cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, 2, wbuf, NULL,
                                 false);
    cyg_spi_transaction_end(spi_device);
}

//-----------------------------------------------------------------------------
// Implement reads to the specified buffer.

static inline void
sst25xx_spi_read(struct cyg_flash_dev *dev, cyg_flashaddr_t addr,
                 cyg_uint8 *rbuf, cyg_uint32 rbuf_len)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[5] = { SST25XX_CMD_READ_OPCODE,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr), 0
    };

    // Implement the read operation as a multistage SPI transaction.
    cyg_spi_transaction_begin(spi_device);
    cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED,
                                 SST25XX_CMD_READ_LEN, tx_buf, NULL, false);
    cyg_spi_transaction_transfer(spi_device, SST25XX_POLLED, rbuf_len, NULL,
                                 rbuf, false);
    cyg_spi_transaction_end(spi_device);
}


//=============================================================================
// Standard Flash device API.  All the following functions assume that a valid
// SPI device handle is passed in the 'priv' reference of the flash device
// data structure.
//=============================================================================

//-----------------------------------------------------------------------------
// Initialize the SPI flash, reading back the flash parameters.

static int
sst25xx_init(struct cyg_flash_dev *dev)
{
    sst25xx_params *dev_params = (sst25xx_params *) sst25xx_supported_devices;
    cyg_uint32      device_id;
    cyg_uint8       i = 0;
    int             retval = FLASH_ERR_INVALID;

    // Find the device in the supported devices list.
    device_id = sst25xx_spi_rdid(dev);

    while ((dev_params->jedec_id != 0) && (dev_params->jedec_id != device_id)) {
        dev_params++;
    }

    // Found supported device - update device parameters.  SST25XX devices have
    // a uniform sector distribution, so only 1 block info record is required.
    if (dev_params->jedec_id != 0) {

        // Find out is the wanted sector size is supported by the device
        while ((dev_params->sector_size[i] !=
                CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE)
               && (dev_params->sector_size[i] != 0))
            i++;

        if (dev_params->sector_size[i] !=
            CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE) {
            TRACE_SST25XX("Init device with JEDEC ID 0x%06X\n",
                          dev_params->jedec_id);
            TRACE_SST25XX("SPI Flash Error, not supporting %dK sector size\n",
                          CYGPKG_DEVS_FLASH_SPI_SST25XX_BLOCK_SIZE);
            return retval;
        }

        ASSERT_SST25XX(dev->num_block_infos == 1,
                       "Only 1 block info record required.");
        ASSERT_SST25XX(dev->block_info != NULL,
                       "Null pointer to block info record.");

        if ((dev->num_block_infos == 1) && (dev->block_info != NULL)) {
            TRACE_SST25XX("Init device with JEDEC ID 0x%06X.\n", device_id);
            dev->end =
                dev->start +
                ((cyg_flashaddr_t) dev_params->sector_size[i] *
                 (cyg_flashaddr_t) (dev_params->sector_count[i])) - 1;

            // Strictly speaking the block info fields are 'read only'.
            // However, we have a legitimate reason for updating the contents
            // here and can cast away the const.
            ((cyg_flash_block_info_t *) dev->block_info)->block_size =
                (size_t)(dev_params->sector_size[i]);
            ((cyg_flash_block_info_t *) dev->block_info)->blocks =
                (cyg_uint32)(dev_params->sector_count[i]);

            retval = FLASH_ERR_OK;
        }

    }
    return retval;
}

//-----------------------------------------------------------------------------
// Erase a single sector of the flash.

static int
sst25xx_erase_block(struct cyg_flash_dev *dev, cyg_flashaddr_t block_base)
{
    cyg_flashaddr_t local_base = block_base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8       dev_status;

    // Fix up the block address and send the sector erase command.
    if (sst25xx_to_local_addr(dev, &local_base)) {
        sst25xx_spi_wren(dev);
        sst25xx_spi_se(dev, local_base);

        // Spin waiting for the erase to complete. Erasing takes around
        // 25 ms
        do {
            SST25XX_DELAY_MS(10);
            dev_status = sst25xx_spi_rdsr(dev);
        } while (dev_status & SST25XX_STATUS_BSY);

        retval = FLASH_ERR_OK;
    }
    return retval;
}

//-----------------------------------------------------------------------------
// Program an arbitrary number of pages into flash and verify written data.

static int
sst25xx_program(struct cyg_flash_dev *dev, cyg_flashaddr_t base,
                const void *data, size_t len)
{
    cyg_uint8      *tx_ptr = (cyg_uint8 *)data;
    cyg_flashaddr_t local_base = base;
    cyg_flashaddr_t end;
    size_t          tx_len = len;
    cyg_bool        start_wxfer = true;
    int             retval = FLASH_ERR_OK;

    // Fix up the block address.
    if (!sst25xx_to_local_addr(dev, &local_base)) {
        retval = FLASH_ERR_INVALID;
        goto out;
    }
    end = local_base + len - 1;

    // Start with a byte programming if the destination is not
    // 16 bits aligned
    if (local_base & (cyg_flashaddr_t) 0x1) {
        sst25xx_spi_wren(dev);
        sst25xx_spi_pbyte(dev, local_base, tx_ptr);
        SST25XX_WAIT_CHIP_READY(dev);
        sst25xx_spi_wrdis(dev);
        tx_ptr += 1;
        tx_len -= 1;
        local_base = local_base + 1;
    }
    // Write word(s)
    if (tx_len > 1) {
        sst25xx_spi_wren(dev);
        while (tx_len > 1) {
            sst25xx_spi_pword(dev, local_base, tx_ptr, start_wxfer);
            tx_ptr += 2;
            tx_len -= 2;
            start_wxfer = false;
            SST25XX_WAIT_CHIP_READY(dev);
        }
        sst25xx_spi_wrdis(dev);
    }
    // Write last byte
    if (tx_len) {
        sst25xx_spi_wren(dev);
        sst25xx_spi_pbyte(dev, end, tx_ptr);
        SST25XX_WAIT_CHIP_READY(dev);
        sst25xx_spi_wrdis(dev);
    }

out:
    return retval;
}

//-----------------------------------------------------------------------------
// Read back an arbitrary amount of data from flash.

static int
sst25xx_read(struct cyg_flash_dev *dev, const cyg_flashaddr_t base,
             void *data, size_t len)
{
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8      *rx_ptr = (cyg_uint8 *)data;
    cyg_uint32      rx_bytes_left = (cyg_uint32)len;
    cyg_uint32      rx_bytes;

    // Determine the maximum transfer size to use.
    cyg_uint32      rx_block_size =
        (CYGNUM_DEVS_FLASH_SPI_SST25XX_READ_BLOCK_SIZE ==
         0) ? 0xFFFFFFFF : CYGNUM_DEVS_FLASH_SPI_SST25XX_READ_BLOCK_SIZE;

    // Fix up the block address and fill the read buffer.
    if (sst25xx_to_local_addr(dev, &local_base)) {
        while (rx_bytes_left) {
            rx_bytes =
                (rx_bytes_left <
                 rx_block_size) ? rx_bytes_left : rx_block_size;
            sst25xx_spi_read(dev, local_base, rx_ptr, rx_bytes);

            // Update counters and data pointers for next read block.
            rx_bytes_left -= rx_bytes;
            rx_ptr += rx_bytes;
            local_base += rx_bytes;
        }
        retval = FLASH_ERR_OK;
    }
    return retval;
}

//-----------------------------------------------------------------------------
// Lock device

static int
sst25xx_lock(struct cyg_flash_dev *dev, cyg_flashaddr_t base)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    const cyg_uint8 tx_buf[2] = { SST25XX_CMD_WRSR, SST25XX_LOCK_BITS };

    if (sst25xx_to_local_addr(dev, &local_base)) {
        sst25xx_spi_wren(dev);
        cyg_spi_transfer(spi_device, SST25XX_POLLED, 2, tx_buf, NULL);
        retval = FLASH_ERR_OK;
    }
    return retval;
}

//-----------------------------------------------------------------------------
// Unlock device

static int
sst25xx_unlock(struct cyg_flash_dev *dev, cyg_flashaddr_t base)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    const cyg_uint8 tx_buf[2] = { SST25XX_CMD_WRSR, 0x0 };

    if (sst25xx_to_local_addr(dev, &local_base)) {
        sst25xx_spi_wren(dev);
        cyg_spi_transfer(spi_device, SST25XX_POLLED, 2, tx_buf, NULL);
        retval = FLASH_ERR_OK;
    }
    return retval;
}

//=============================================================================
// Fill in the driver data structures.
//=============================================================================

CYG_FLASH_FUNS (
    cyg_devs_flash_spi_sst25xx_funs, // Exported name of function pointers.
    sst25xx_init,                    // Flash initialization.
    cyg_flash_devfn_query_nop,       // Query operations not supported.
    sst25xx_erase_block,             // Sector erase.
    sst25xx_program,                 // Program multiple pages.
    sst25xx_read,                    // Read arbitrary amount of data.
    sst25xx_lock,                    // Locking (lock the whole device).
    sst25xx_unlock
);

//-----------------------------------------------------------------------------
// EOF sst25xx.c
