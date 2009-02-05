//=============================================================================
//
//      m25pxx.c
//
//      SPI flash driver for Numonyx M25Pxx devices and compatibles.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
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
// Author(s):   Chris Holgate
// Date:        2008-12-22
// Purpose:     Numonyx M25Pxx SPI flash driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/io/spi.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>

#include <pkgconf/devs_flash_spi_m25pxx.h>

#include <string.h>

//-----------------------------------------------------------------------------
// Enable polled SPI operation for non-kernel builds.

#ifdef CYGPKG_KERNEL
#define M25PXX_POLLED false

#else
#define M25PXX_POLLED true
#endif

//-----------------------------------------------------------------------------
// Implement delay functions for kernel and non-kernel builds.  The kernel
// build assumes that the API calls are made in the thread context.

#ifdef CYGPKG_KERNEL
#define M25PXX_DELAY_MS(_msdelay_) cyg_thread_delay (\
  1 + ((1000 * _msdelay_ * CYGNUM_HAL_RTC_DENOMINATOR) / (CYGNUM_HAL_RTC_NUMERATOR / 1000)))

#else
#define M25PXX_DELAY_MS(_msdelay_) CYGACC_CALL_IF_DELAY_US (_msdelay_ * 1000)
#endif

//-----------------------------------------------------------------------------
// Maintenance and debug macros.

#define TODO_M25P(_msg_) CYG_ASSERT(false, "TODO (M25P) : " _msg_)
#define FAIL_M25P(_msg_) CYG_ASSERT(false, "FAIL (M25P) : " _msg_)
#define ASSERT_M25P(_test_, _msg_) CYG_ASSERT(_test_, "FAIL (M25P) : " _msg_)
#define TRACE_M25P(_msg_, _args_...) if (dev->pf) dev->pf ("M25PXX : " _msg_, ##_args_)

//=============================================================================
// Define M25Pxx SPI protocol.
//=============================================================================

typedef enum m25pxx_cmd { 
  M25PXX_CMD_WREN  = 0x06,  // Write enable.
  M25PXX_CMD_WDRI  = 0x04,  // Write disable.
  M25PXX_CMD_RDID  = 0x9F,  // Read identification.
  M25PXX_CMD_RDSR  = 0x05,  // Read status register.
  M25PXX_CMD_WRSR  = 0x01,  // Write status register.
  M25PXX_CMD_READ  = 0x03,  // Read data.
  M25PXX_CMD_FREAD = 0x0B,  // Read data (fast transaction).
  M25PXX_CMD_PP    = 0x02,  // Page program.
  M25PXX_CMD_SE    = 0xD8,  // Sector erase.
  M25PXX_CMD_BE    = 0xC7,  // Bulk erase.
  M25PXX_CMD_RES   = 0xAB,  // Read electronic signature.
} m25pxx_cmd;

// Status register bitfields.
#define M25PXX_STATUS_WIP  0x01  /* Write in progress. */
#define M25PXX_STATUS_WEL  0x02  /* Write enable latch. */
#define M25PXX_STATUS_BP0  0x04  /* Block protect 0. */
#define M25PXX_STATUS_BP1  0x08  /* Block protect 1. */
#define M25PXX_STATUS_BP2  0x10  /* Block protect 2. */
#define M25PXX_STATUS_SRWD 0x80  /* Status register write protect. */

// Page size of 256 bytes appears to be common for all devices.
#define M25PXX_PAGE_SIZE 256

//=============================================================================
// Array containing a list of supported devices.  This allows the device
// parameters to be dynamically detected on initialisation.
//=============================================================================

typedef struct m25pxx_params {
  cyg_uint16 sector_size;   // Number of pages in a sector.
  cyg_uint16 sector_count;  // Number of sectors on device.  
  cyg_uint32 jedec_id;      // 3 byte JEDEC identifier for this device.
} m25pxx_params;

static const m25pxx_params m25pxx_supported_devices [] = {
  { // Support for Numonyx 128 MBit devices.
    sector_size  : 1024,
    sector_count : 64,
    jedec_id     : 0x00202018
  },
  { // Support for Numonyx 64 MBit devices.
    sector_size  : 256,
    sector_count : 128,
    jedec_id     : 0x00202017
  },
  { // Support for Numonyx 16 MBit devices.
    sector_size  : 256,
    sector_count : 64,
    jedec_id     : 0x00202016
  },
  { // Support for Numonyx 16 MBit devices.
    sector_size  : 256,
    sector_count : 32,
    jedec_id     : 0x00202015
  },
  { // Support for Numonyx 8 MBit devices.
    sector_size  : 256,
    sector_count : 16,
    jedec_id     : 0x00202014
  },
  { // Support for Numonyx 4 MBit devices.
    sector_size  : 256,
    sector_count : 8,
    jedec_id     : 0x00202013
  },
  { // Support for Numonyx 2 MBit devices.
    sector_size  : 256,
    sector_count : 4,
    jedec_id     : 0x00202012
  },
  { // Support for Numonyx 1 MBit devices.
    sector_size  : 128,
    sector_count : 4,
    jedec_id     : 0x00202011
  },
  { // Support for Numonyx 512 KBit devices.
    sector_size  : 128,
    sector_count : 2,
    jedec_id     : 0x00202010
  },
  { // Null terminating entry.
    sector_size  : 0,
    sector_count : 0,
    jedec_id     : 0
  }
};

//=============================================================================
// Utility functions for address calculations.
//=============================================================================

//-----------------------------------------------------------------------------
// Strips out any device address offset to give address within device.

static cyg_bool m25pxx_to_local_addr
  (struct cyg_flash_dev* dev, cyg_flashaddr_t* addr)
{
  cyg_bool retval = false;

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

static inline cyg_uint32 m25pxx_spi_rdid
  (struct cyg_flash_dev *dev)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [4] = { M25PXX_CMD_RDID, 0, 0, 0 };
  cyg_uint8 rx_buf [4];
  cyg_uint32 retval = 0;

  // Carry out SPI transfer.
  cyg_spi_transfer (spi_device, true, 4, tx_buf, rx_buf);

  // Convert 3-byte ID to 32-bit integer.
  retval |= ((cyg_uint32) rx_buf[1]) << 16;   
  retval |= ((cyg_uint32) rx_buf[2]) << 8;   
  retval |= ((cyg_uint32) rx_buf[3]);   

  return retval;
}

//-----------------------------------------------------------------------------
// Send write enable command.

static inline void m25pxx_spi_wren
  (struct cyg_flash_dev *dev)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [1] = { M25PXX_CMD_WREN };
  cyg_spi_transfer (spi_device, M25PXX_POLLED, 1, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Send sector erase command.  The address parameter is a device local address 
// within the sector to be erased.

static inline void m25pxx_spi_se
  (struct cyg_flash_dev *dev, cyg_flashaddr_t addr)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [4] = { M25PXX_CMD_SE,
    (cyg_uint8) (addr >> 16), (cyg_uint8) (addr >> 8), (cyg_uint8) (addr) };
  cyg_spi_transfer (spi_device, M25PXX_POLLED, 4, tx_buf, NULL);
}

//-----------------------------------------------------------------------------
// Read and return the 8-bit device status register.

static inline cyg_uint8 m25pxx_spi_rdsr
  (struct cyg_flash_dev *dev)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [2] = { M25PXX_CMD_RDSR, 0 };
  cyg_uint8 rx_buf [2];

  // Carry out SPI transfer and return the status byte.
  cyg_spi_transfer (spi_device, M25PXX_POLLED, 2, tx_buf, rx_buf);
  return rx_buf [1];
}

//-----------------------------------------------------------------------------
// Program a single page.

static inline void m25pxx_spi_pp
  (struct cyg_flash_dev *dev, cyg_flashaddr_t addr, cyg_uint8* wbuf, cyg_uint32 wbuf_len)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [4] = { M25PXX_CMD_PP,
    (cyg_uint8) (addr >> 16), (cyg_uint8) (addr >> 8), (cyg_uint8) (addr) };

  // Implement the program operation as a multistage SPI transaction.
  cyg_spi_transaction_begin (spi_device);
  cyg_spi_transaction_transfer (spi_device, M25PXX_POLLED, 4, tx_buf, NULL, false);
  cyg_spi_transaction_transfer (spi_device, M25PXX_POLLED, wbuf_len, wbuf, NULL, false);
  cyg_spi_transaction_end (spi_device);
}

//-----------------------------------------------------------------------------
// Implement fast reads to the specified buffer.

static inline void m25pxx_spi_fread
  (struct cyg_flash_dev *dev, cyg_flashaddr_t addr, cyg_uint8* rbuf, cyg_uint32 rbuf_len)
{
  cyg_spi_device* spi_device = (cyg_spi_device*) dev->priv;
  const cyg_uint8 tx_buf [5] = { M25PXX_CMD_FREAD,
    (cyg_uint8) (addr >> 16), (cyg_uint8) (addr >> 8), (cyg_uint8) (addr), 0 };

  // Implement the read operation as a multistage SPI transaction.
  cyg_spi_transaction_begin (spi_device);
  cyg_spi_transaction_transfer (spi_device, M25PXX_POLLED, 5, tx_buf, NULL, false);
  cyg_spi_transaction_transfer (spi_device, M25PXX_POLLED, rbuf_len, NULL, rbuf, false);
  cyg_spi_transaction_end (spi_device);
}

//=============================================================================
// Standard Flash device API.  All the following functions assume that a valid
// SPI device handle is passed in the 'priv' reference of the flash device
// data structure.
//=============================================================================

//-----------------------------------------------------------------------------
// Initialise the SPI flash, reading back the flash parameters.

static int m25pxx_init 
  (struct cyg_flash_dev *dev)
{
  m25pxx_params* dev_params = (m25pxx_params*) m25pxx_supported_devices;
  cyg_uint32 device_id;
  int retval = FLASH_ERR_INVALID;

  // Find the device in the supported devices list.
  device_id = m25pxx_spi_rdid (dev);
  while ((dev_params->jedec_id != 0) && (dev_params->jedec_id != device_id)) {
    dev_params ++;
  }

  // Found supported device - update device parameters.  M25PXX devices have a 
  // uniform sector distribution, so only 1 block info record is required.
  if (dev_params->jedec_id != 0) {
    ASSERT_M25P (dev->num_block_infos == 1, "Only 1 block info record required.");
    ASSERT_M25P (dev->block_info != NULL, "Null pointer to block info record.");
    if ((dev->num_block_infos == 1) && (dev->block_info != NULL)) { 
      TRACE_M25P ("Init device with JEDEC ID 0x%06X.\n", device_id);
      dev->end = dev->start + (M25PXX_PAGE_SIZE * (cyg_flashaddr_t) dev_params->sector_size * 
        (cyg_flashaddr_t) dev_params->sector_count) - 1;

      // Strictly speaking the block info fields are 'read only'.  However, we
      // have a legitimate reason for updating the contents here and can cast
      // away the const.
      ((cyg_flash_block_info_t*) dev->block_info)->block_size = 
        M25PXX_PAGE_SIZE * (size_t) dev_params->sector_size; 
      ((cyg_flash_block_info_t*) dev->block_info)->blocks = 
        (cyg_uint32) dev_params->sector_count;
      retval = FLASH_ERR_OK;
    }
  }
  return retval;
}

//-----------------------------------------------------------------------------
// Erase a single sector of the flash.

static int m25pxx_erase_block 
  (struct cyg_flash_dev *dev, cyg_flashaddr_t block_base)
{
  cyg_flashaddr_t local_base = block_base;
  int retval = FLASH_ERR_INVALID;
  cyg_uint8 dev_status;

  // Fix up the block address and send the sector erase command.
  if (m25pxx_to_local_addr (dev, &local_base)) {
    m25pxx_spi_wren (dev);
    m25pxx_spi_se (dev, local_base);

    // Spin waiting for the erase to complete.  This can take between 1 and 3
    // seconds, so we use a polling interval of 1/2 sec.
    do {
      M25PXX_DELAY_MS (500);
      dev_status = m25pxx_spi_rdsr (dev);
    } while (dev_status & M25PXX_STATUS_WIP);

    retval = FLASH_ERR_OK;    
  }
  return retval;
}

//-----------------------------------------------------------------------------
// Program an arbitrary number of pages into flash and verify written data.

static int m25pxx_program 
  (struct cyg_flash_dev *dev, cyg_flashaddr_t base, const void* data, size_t len)
{
  cyg_flashaddr_t local_base = base;
  int retval = FLASH_ERR_OK;
  cyg_uint8* tx_ptr = (cyg_uint8*) data;
  cyg_uint32 tx_bytes_left = (cyg_uint32) len;
  cyg_uint32 tx_bytes;
  cyg_uint8  dev_status;

  // Fix up the block address.
  if (!m25pxx_to_local_addr (dev, &local_base)) {
    retval = FLASH_ERR_INVALID;
    goto out;
  }

  // The start of the transaction may not be page aligned, so we need to work
  // out how many bytes to transmit before we hit the first page boundary.
  tx_bytes = M25PXX_PAGE_SIZE - (((cyg_uint32) local_base) & (M25PXX_PAGE_SIZE - 1));
  if (tx_bytes > tx_bytes_left) tx_bytes = tx_bytes_left;

  // Perform page program operations.
  while (tx_bytes_left) {
    m25pxx_spi_wren (dev);
    m25pxx_spi_pp (dev, local_base, tx_ptr, tx_bytes);

    // Spin waiting for write to complete.  This can take up to 5ms, so
    // we use a polling interval of 1ms - which may get rounded up to the
    // RTC tick granularity.
    do {
      M25PXX_DELAY_MS (1);
      dev_status = m25pxx_spi_rdsr (dev);
    } while (dev_status & M25PXX_STATUS_WIP);
    
    // Update counters and data pointers for the next page.
    tx_bytes_left -= tx_bytes;
    tx_ptr += tx_bytes;
    local_base += tx_bytes; 
    tx_bytes = (tx_bytes_left > M25PXX_PAGE_SIZE) ? M25PXX_PAGE_SIZE : tx_bytes_left;
  }

out:
  return retval;
}

//-----------------------------------------------------------------------------
// Read back an arbitrary amount of data from flash.

static int m25pxx_read 
  (struct cyg_flash_dev *dev, const cyg_flashaddr_t base, void* data, size_t len)
{
  cyg_flashaddr_t local_base = base;
  int retval = FLASH_ERR_INVALID;
  cyg_uint8* rx_ptr = (cyg_uint8*) data;
  cyg_uint32 rx_bytes_left = (cyg_uint32) len;
  cyg_uint32 rx_bytes;

  // Determine the maximum transfer size to use.
  cyg_uint32 rx_block_size = (CYGNUM_DEVS_FLASH_SPI_M25PXX_READ_BLOCK_SIZE == 0) ? 
    0xFFFFFFFF : CYGNUM_DEVS_FLASH_SPI_M25PXX_READ_BLOCK_SIZE;

  // Fix up the block address and fill the read buffer.
  if (m25pxx_to_local_addr (dev, &local_base)) {
    while (rx_bytes_left) {
      rx_bytes = (rx_bytes_left < rx_block_size) ? rx_bytes_left : rx_block_size;
      m25pxx_spi_fread (dev, local_base, rx_ptr, rx_bytes);

      // Update counters and data pointers for next read block.
      rx_bytes_left -= rx_bytes;
      rx_ptr += rx_bytes;
      local_base += rx_bytes; 
    }
    retval = FLASH_ERR_OK;
  }
  return retval;
}

//=============================================================================
// Fill in the driver data structures.
//=============================================================================

CYG_FLASH_FUNS (
  cyg_devs_flash_spi_m25pxx_funs, // Exported name of function pointers.
  m25pxx_init,                    // Flash initialisation.
  cyg_flash_devfn_query_nop,      // Query operations not supported.
  m25pxx_erase_block,             // Sector erase.
  m25pxx_program,                 // Program multiple pages.
  m25pxx_read,                    // Read arbitrary amount of data.
  cyg_flash_devfn_lock_nop,       // Locking not supported (no per-sector locks).
  cyg_flash_devfn_unlock_nop
);

//=============================================================================
