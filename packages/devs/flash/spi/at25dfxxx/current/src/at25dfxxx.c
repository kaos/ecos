//=============================================================================
//
//      at25dfxxx.c
//
//      SPI flash driver for Atmel AT25DFxxx devices and compatibles.
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
// Author(s):   ccoutand, updated for Atmel AT95DFxxx flash
// Original(s): Chris Holgate
// Date:        2011-04-25
// Purpose:     Atmel AT95DFxxx SPI flash driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/io/spi.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/devs_flash_spi_at25dfxxx.h>

#include <string.h>

//-----------------------------------------------------------------------------
// Enable polled SPI operation for non-kernel builds.

#ifdef CYGPKG_KERNEL
# define AT25DFXXX_POLLED false
#else
# define AT25DFXXX_POLLED true
#endif

//-----------------------------------------------------------------------------
// Implement delay functions for kernel and non-kernel builds.  The kernel
// build assumes that the API calls are made in the thread context.

#ifdef CYGPKG_KERNEL
# define AT25DFXXX_DELAY_MS(_msdelay_) cyg_thread_delay \
  (1 + ((1000 * _msdelay_ * CYGNUM_HAL_RTC_DENOMINATOR) / (CYGNUM_HAL_RTC_NUMERATOR / 1000)))

#else
# define AT25DFXXX_DELAY_MS(_msdelay_) CYGACC_CALL_IF_DELAY_US (_msdelay_ * 1000)
#endif

//-----------------------------------------------------------------------------
// Maintenance and debug macros.

#define ASSERT_AT25DFXXX(_test_, _msg_) CYG_ASSERT(_test_, "FAIL (AT25DFXXX) : " _msg_)
#define TRACE_AT25DFXXX(_msg_, _args_...) if (dev->pf) dev->pf ("AT25DFXXX : " _msg_, ##_args_)

//=============================================================================
// Define AT25DFxxx SPI protocol.
//=============================================================================

typedef enum at25dfxxx_cmd {
    AT25DFXXX_CMD_WREN   = 0x06,       // Write enable.
    AT25DFXXX_CMD_WDRI   = 0x04,       // Write disable.
    AT25DFXXX_CMD_RDID   = 0x9F,       // Read identification.
    AT25DFXXX_CMD_RDSR1  = 0x05,       // Read status register.
    AT25DFXXX_CMD_RDSR2  = 0x31,       // Read status register.
    AT25DFXXX_CMD_WRSR   = 0x01,       // Write status register.
    AT25DFXXX_CMD_SREAD  = 0x03,       // Read data (slow transaction).
    AT25DFXXX_CMD_READ   = 0x0B,       // Read data.
    AT25DFXXX_CMD_FREAD  = 0x1B,       // Read data (fast transaction).
    AT25DFXXX_CMD_PP     = 0x02,       // Page program.
    AT25DFXXX_CMD_SE_4K  = 0x20,       // 4K sector erase.
    AT25DFXXX_CMD_SE_32K = 0x52,       // 32K sector erase.
    AT25DFXXX_CMD_SE_64K = 0xD8,       // 64K sector erase.
    AT25DFXXX_CMD_BE     = 0xC7,       // Chip erase.
    AT25DFXXX_CMD_LOCK   = 0x36,       // Protect sector
    AT25DFXXX_CMD_UNLOCK = 0x39,       // Unprotect sector
} at25dfxxx_cmd;

// Status register bitfields.
#define AT25DFXXX_STATUS_BSY  0x01     /* Operation in progress. */
#define AT25DFXXX_STATUS_WEL  0x02     /* Write enable latch. */
#define AT25DFXXX_STATUS_BP0  0x04     /* Global protect 0. */
#define AT25DFXXX_STATUS_BP1  0x08     /* Global protect 1. */
#define AT25DFXXX_STATUS_BP2  0x10     /* Global protect 2. */
#define AT25DFXXX_STATUS_SPRL 0x80     /* Sector Protect Register Lock. */

// Page size of 256 bytes appears to be common for all devices.
#define AT25DFXXX_PAGE_SIZE   256

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
#if defined(CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_READ_MODE_SLOW)
# define AT25DFXXX_CMD_READ_LEN 4
# define AT25DFXXX_CMD_READ_OPCODE AT25DFXXX_CMD_SREAD
#elif defined(CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_READ_MODE_FAST)
# define AT25DFXXX_CMD_READ_LEN 5
# define AT25DFXXX_CMD_READ_OPCODE AT25DFXXX_CMD_READ
#elif defined(CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_READ_MODE_RAPIDS)
# define AT25DFXXX_CMD_READ_LEN 6
# define AT25DFXXX_CMD_READ_OPCODE AT25DFXXX_CMD_FREAD
#endif

// Select sector size
#if CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE == SZ_64K
# define AT25DFXXX_CMD_SE AT25DFXXX_CMD_SE_64K
#elif CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE == SZ_32K
# define AT25DFXXX_CMD_SE AT25DFXXX_CMD_SE_32K
#elif CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE == SZ_4K
# define AT25DFXXX_CMD_SE AT25DFXXX_CMD_SE_4K
#endif

//=============================================================================
// Array containing a list of supported devices.  This allows the device
// parameters to be dynamically detected on initialization.
//=============================================================================

typedef struct at25dfxxx_params {
    // Atmel AT25DFxxx SPI flash can be used with 4K, 32K or 64K sector size.
    // sector_size, sector_count are defined as array, allowing
    // to describe the geometry of the device for all supported sector size.
    cyg_uint32      sector_size[4];       // Supported sector size(s) in Byte
    cyg_uint16      sector_count[4];      // Number of sectors on device.
    cyg_bool        support_rapids;       // Atmel RapidS protocol support
    cyg_uint32      jedec_id;             // 3 byte JEDEC identifier
} at25dfxxx_params;


static const at25dfxxx_params at25dfxxx_supported_devices [] = {
    // Some Atmel AT25DFxxx part (Atmel JEDEC code: 0x1f)
    // Consider removing the revision number from the Manufacturer /
    // Device ID for larger chip coverage.

    { // Support for Atmel 64 MBit devices (AT25DF641).
        sector_count        : {2048, 256, 128, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : true,
        jedec_id            : 0x001f4800
    },
    { // Support for Atmel 32 MBit devices (AT25DF321A).
        sector_count        : {1024, 128, 64, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : true,
        jedec_id            : 0x001f4701
    },
    { // Support for Atmel 16 MBit devices (AT25DF161).
        sector_count        : {512, 64, 32, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : true,
        jedec_id            : 0x001f4602
    },
    { // Support for Atmel 8 MBit devices (AT25DF081A).
        sector_count        : {256, 32, 16, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : true,
        jedec_id            : 0x001f4501
    },
    { // Support for Atmel 4 MBit devices (AT25DF041A).
        sector_count        : {128, 16, 8, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : false,
        jedec_id            : 0x001f4400
    },
    { // Support for Atmel 2 MBit devices (AT25DF021).
        sector_count        : {64, 8, 4, 0},
        sector_size         : {SZ_4K, SZ_32K, SZ_64K, 0},
        support_rapids      : false,
        jedec_id            : 0x001f4300
    },
    { // Support for Atmel 512 KBit devices (AT25F512B).
        sector_count        : {8, 1, 0},
        sector_size         : {SZ_4K, SZ_32K, 0},
        support_rapids      : false,
        jedec_id            : 0x001f6500
    },
    { // Null terminating entry.
        sector_count        : {0},
        sector_size         : {0},
        support_rapids      : false,
        jedec_id            : 0
    }
};
//=============================================================================
// Utility functions for address calculations.
//=============================================================================

//-----------------------------------------------------------------------------
// Strips out any device address offset to give address within device.

static cyg_bool
at25dfxxx_to_local_addr(struct cyg_flash_dev *dev, cyg_flashaddr_t * addr)
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
// This function is called during flash initialisation, which can often be
// called from the startup/idle thread.  This means that we should always use
// SPI polled mode in order to prevent the thread from attempting to sleep.

static inline cyg_uint32
at25dfxxx_spi_rdid(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { AT25DFXXX_CMD_RDID, 0x0, 0x0, 0x0 };
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

//-----------------------------------------------------------------------------
// Send write enable command.

static inline void
at25dfxxx_spi_wren(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[1] = { AT25DFXXX_CMD_WREN };
    cyg_spi_transfer(spi_device, AT25DFXXX_POLLED, 1, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send sector erase command.  The address parameter is a device local address
// within the sector to be erased.

static inline void
at25dfxxx_spi_se(struct cyg_flash_dev *dev, cyg_flashaddr_t addr)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    cyg_uint8       tx_buf[4] = { AT25DFXXX_CMD_SE,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };
    cyg_spi_transfer(spi_device, AT25DFXXX_POLLED, 4, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send sector erase command.  The address parameter is a device local address
// within the sector to be erased.

static inline void
at25dfxxx_spi_lock(struct cyg_flash_dev *dev, cyg_flashaddr_t addr)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { AT25DFXXX_CMD_LOCK,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };
    cyg_spi_transfer(spi_device, AT25DFXXX_POLLED, 4, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send sector erase command.  The address parameter is a device local address
// within the sector to be erased.

static inline void
at25dfxxx_spi_unlock(struct cyg_flash_dev *dev, cyg_flashaddr_t addr)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { AT25DFXXX_CMD_UNLOCK,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };
    cyg_spi_transfer(spi_device, AT25DFXXX_POLLED, 4, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Read and return the 8-bit device status register.

static inline cyg_uint8
at25dfxxx_spi_rdsr(struct cyg_flash_dev *dev)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[2] = { AT25DFXXX_CMD_RDSR1, 0 };
    cyg_uint8       rx_buf[2];

    // Carry out SPI transfer and return the status byte.
    cyg_spi_transfer(spi_device, AT25DFXXX_POLLED, 2, tx_buf, rx_buf);

    return rx_buf[1];
}

//-----------------------------------------------------------------------------
// Program a single page.

static inline void
at25dfxxx_spi_pp(struct cyg_flash_dev *dev, cyg_flashaddr_t addr,
                 cyg_uint8 *wbuf, cyg_uint32 wbuf_len)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[4] = { AT25DFXXX_CMD_PP,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr)
    };

    // Implement the program operation as a multistage SPI transaction.
    cyg_spi_transaction_begin(spi_device);
    cyg_spi_transaction_transfer(spi_device, AT25DFXXX_POLLED, 4, tx_buf,
                                 NULL, false);
    cyg_spi_transaction_transfer(spi_device, AT25DFXXX_POLLED, wbuf_len, wbuf,
                                 NULL, false);
    cyg_spi_transaction_end(spi_device);
}

//-----------------------------------------------------------------------------
// Implement reads to the specified buffer.

static inline void
at25dfxxx_spi_read(struct cyg_flash_dev *dev, cyg_flashaddr_t addr,
                   cyg_uint8 *rbuf, cyg_uint32 rbuf_len)
{
    cyg_spi_device *spi_device = (cyg_spi_device *) dev->priv;
    const cyg_uint8 tx_buf[6] = { AT25DFXXX_CMD_READ_OPCODE,
        (cyg_uint8)(addr >> 16), (cyg_uint8)(addr >> 8), (cyg_uint8)(addr), 0,
            0
    };

    // Implement the read operation as a multistage SPI transaction.
    cyg_spi_transaction_begin(spi_device);
    cyg_spi_transaction_transfer(spi_device, AT25DFXXX_POLLED,
                                 AT25DFXXX_CMD_READ_LEN, tx_buf, NULL, false);
    cyg_spi_transaction_transfer(spi_device, AT25DFXXX_POLLED, rbuf_len, NULL,
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
at25dfxxx_init(struct cyg_flash_dev *dev)
{
    at25dfxxx_params *dev_params =
        (at25dfxxx_params *) at25dfxxx_supported_devices;
    cyg_uint32      device_id;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8       i = 0;

    // Find the device in the supported devices list.
    device_id = at25dfxxx_spi_rdid(dev);
    while ((dev_params->jedec_id != 0) && (dev_params->jedec_id != device_id)) {
        dev_params++;
    }

    // Found supported device - update device parameters.  AT25DFXXX devices
    // have a uniform sector distribution, so only 1 block info record is
    // required.
    if (dev_params->jedec_id != 0) {

        while ((dev_params->sector_size[i] !=
                CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE)
               && (dev_params->sector_size[i] != 0))
            i++;

        // Verify that the device supports the wanted geometry
        if (dev_params->sector_size[i] !=
            CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE) {
            TRACE_AT25DFXXX("Init device with JEDEC ID 0x%06X\n",
                            dev_params->jedec_id);
            TRACE_AT25DFXXX
                ("SPI Flash Error, not supporting %dK sector size\n",
                 CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_BLOCK_SIZE);
            return retval;
        }

#if defined(CYGPKG_DEVS_FLASH_SPI_AT25DFXXX_READ_MODE_RAPIDS)
        if (dev_params->support_rapids != true) {
            TRACE_AT25DFXXX("Init device with JEDEC ID 0x%06X\n",
                            dev_params->jedec_id);
            TRACE_AT25DFXXX
                ("SPI Flash Error, not supporting RapidS Opcode\n");
            return retval;
        }
#endif

        ASSERT_AT25DFXXX(dev->num_block_infos == 1,
                         "Only 1 block info record required.");
        ASSERT_AT25DFXXX(dev->block_info != NULL,
                         "Null pointer to block info record.");

        if ((dev->num_block_infos == 1) && (dev->block_info != NULL)) {
            TRACE_AT25DFXXX("Init device with JEDEC ID 0x%06X.\n", device_id);

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
at25dfxxx_erase_block(struct cyg_flash_dev *dev, cyg_flashaddr_t block_base)
{
    cyg_flashaddr_t local_base = block_base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8       dev_status;

    // Fix up the block address and send the sector erase command.
    if (at25dfxxx_to_local_addr(dev, &local_base)) {
        at25dfxxx_spi_wren(dev);
        at25dfxxx_spi_se(dev, local_base);

        // Spin waiting for the erase to complete.
        do {
            AT25DFXXX_DELAY_MS(1);
            dev_status = at25dfxxx_spi_rdsr(dev);
        } while (dev_status & AT25DFXXX_STATUS_BSY);

        retval = FLASH_ERR_OK;
    }
    return retval;
}

//-----------------------------------------------------------------------------
// Program an arbitrary number of pages into flash and verify written data.

static int
at25dfxxx_program(struct cyg_flash_dev *dev, cyg_flashaddr_t base,
                  const void *data, size_t len)
{
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_OK;
    cyg_uint8      *tx_ptr = (cyg_uint8 *)data;
    cyg_uint32      tx_bytes_left = (cyg_uint32)len;
    cyg_uint32      tx_bytes;
    cyg_uint8       dev_status;

    // Fix up the block address.
    if (!at25dfxxx_to_local_addr(dev, &local_base)) {
        retval = FLASH_ERR_INVALID;
        goto out;
    }
    // The start of the transaction may not be page aligned, so we need to work
    // out how many bytes to transmit before we hit the first page boundary.
    tx_bytes =
        AT25DFXXX_PAGE_SIZE -
        (((cyg_uint32)local_base) & (AT25DFXXX_PAGE_SIZE - 1));
    if (tx_bytes > tx_bytes_left)
        tx_bytes = tx_bytes_left;

    // Perform page program operations.
    while (tx_bytes_left) {
        at25dfxxx_spi_wren(dev);
        at25dfxxx_spi_pp(dev, local_base, tx_ptr, tx_bytes);

        // Spin waiting for write to complete.  This can take up to 5ms, so
        // we use a polling interval of 1ms - which may get rounded up to the
        // RTC tick granularity.
        do {
            AT25DFXXX_DELAY_MS(1);
            dev_status = at25dfxxx_spi_rdsr(dev);
        } while (dev_status & AT25DFXXX_STATUS_BSY);

        // Update counters and data pointers for the next page.
        tx_bytes_left -= tx_bytes;
        tx_ptr += tx_bytes;
        local_base += tx_bytes;
        tx_bytes =
            (tx_bytes_left >
             AT25DFXXX_PAGE_SIZE) ? AT25DFXXX_PAGE_SIZE : tx_bytes_left;
    }

out:
    return retval;
}

//-----------------------------------------------------------------------------
// Read back an arbitrary amount of data from flash.

static int
at25dfxxx_read(struct cyg_flash_dev *dev, const cyg_flashaddr_t base,
               void *data, size_t len)
{
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8      *rx_ptr = (cyg_uint8 *)data;
    cyg_uint32      rx_bytes_left = (cyg_uint32)len;
    cyg_uint32      rx_bytes;

    // Determine the maximum transfer size to use.
    cyg_uint32      rx_block_size =
        (CYGNUM_DEVS_FLASH_SPI_AT25DFXXX_READ_BLOCK_SIZE ==
         0) ? 0xFFFFFFFF : CYGNUM_DEVS_FLASH_SPI_AT25DFXXX_READ_BLOCK_SIZE;

    // Fix up the block address and fill the read buffer.
    if (at25dfxxx_to_local_addr(dev, &local_base)) {
        while (rx_bytes_left) {
            rx_bytes =
                (rx_bytes_left <
                 rx_block_size) ? rx_bytes_left : rx_block_size;
            at25dfxxx_spi_read(dev, local_base, rx_ptr, rx_bytes);

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
// Lock sector

static int
at25dfxxx_lock(struct cyg_flash_dev *dev, cyg_flashaddr_t base)
{
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8       dev_status;

    // Fix up the block address and send the sector erase command.
    if (at25dfxxx_to_local_addr(dev, &local_base)) {
        at25dfxxx_spi_wren(dev);
        at25dfxxx_spi_lock(dev, local_base);

        // Spin waiting for the lock operation to complete.
        do {
            AT25DFXXX_DELAY_MS(1);
            dev_status = at25dfxxx_spi_rdsr(dev);
        } while (dev_status & AT25DFXXX_STATUS_BSY);
        retval = FLASH_ERR_OK;
    }
    return retval;
}

//-----------------------------------------------------------------------------
// Unlock sector

static int
at25dfxxx_unlock(struct cyg_flash_dev *dev, cyg_flashaddr_t base)
{
    cyg_flashaddr_t local_base = base;
    int             retval = FLASH_ERR_INVALID;
    cyg_uint8       dev_status;

    // Fix up the block address and send the sector erase command.
    if (at25dfxxx_to_local_addr(dev, &local_base)) {
        at25dfxxx_spi_wren(dev);
        at25dfxxx_spi_unlock(dev, local_base);

        // Spin waiting for the unlock operation to complete.
        do {
            AT25DFXXX_DELAY_MS(1);
            dev_status = at25dfxxx_spi_rdsr(dev);
        } while (dev_status & AT25DFXXX_STATUS_BSY);
        retval = FLASH_ERR_OK;
    }
    return retval;
}

//=============================================================================
// Fill in the driver data structures.
//=============================================================================

CYG_FLASH_FUNS(
        cyg_devs_flash_spi_at25dfxxx_funs, // Exported name of function pointers
        at25dfxxx_init,                    // Flash initialization
        cyg_flash_devfn_query_nop,         // Query operations not supported
        at25dfxxx_erase_block,             // Sector erase
        at25dfxxx_program,                 // Program multiple pages
        at25dfxxx_read,                    // Read arbitrary amount of data
        at25dfxxx_lock,                    // Locking
        at25dfxxx_unlock);

//-----------------------------------------------------------------------------
// EOF at25dfxxx.c
