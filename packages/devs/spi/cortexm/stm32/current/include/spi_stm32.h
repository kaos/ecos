#ifndef CYGONCE_DEVS_SPI_CORTEXM_STM32_H
#define CYGONCE_DEVS_SPI_CORTEXM_STM32_H
//=============================================================================
//
//      spi_stm32.h
//
//      Header definitions for STM32 SPI driver.
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
// Date:        2008-11-27
// Purpose:     STM32 SPI driver definitions.
// Description: 
// Usage:       #include <cyg/io/spi_stm32.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>
#include <pkgconf/devs_spi_cortexm_stm32.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>

//-----------------------------------------------------------------------------
// STM32 SPI bus configuration and state.

typedef struct cyg_spi_cortexm_stm32_bus_setup_s
{
    cyg_uint32        apb_freq;        // Peripheral bus frequency (fp).
    cyg_haladdress    spi_reg_base;    // Base address of SPI register block.
    cyg_haladdress    dma_reg_base;    // Base address of DMA register block.
    cyg_uint8         dma_tx_channel;  // TX DMA channel for this bus.
    cyg_uint8         dma_rx_channel;  // RX DMA channel for this bus.
    cyg_uint8         cs_gpio_num;     // Number of chip selects for this bus.
    const cyg_uint8*  cs_gpio_list;    // List of GPIOs used as chip selects.
    const cyg_uint8*  spi_gpio_list;   // List of GPIOs used by the SPI interface.
    cyg_vector_t      dma_tx_intr;     // Interrupt used for DMA transmit.
    cyg_vector_t      dma_rx_intr;     // Interrupt used for DMA receive.
    cyg_priority_t    dma_tx_intr_pri; // Interrupt priority for DMA transmit.
    cyg_priority_t    dma_rx_intr_pri; // Interrupt priority for DMA receive.
    cyg_uint32        bbuf_size;       // Size of bounce buffers.
    cyg_uint8*        bbuf_tx;         // Pointer to transmit bounce buffer.
    cyg_uint8*        bbuf_rx;         // Pointer to receive bounce buffer.

} cyg_spi_cortexm_stm32_bus_setup_t;

typedef struct cyg_spi_cortexm_stm32_bus_s
{
    // ---- Upper layer data ----
    cyg_spi_bus       spi_bus;         // Upper layer SPI bus data.

    // ---- Bus configuration constants ----
    const cyg_spi_cortexm_stm32_bus_setup_t* setup;

    // ---- Driver state (private) ----
    cyg_interrupt     tx_intr_data;    // DMA interrupt data (TX).
    cyg_interrupt     rx_intr_data;    // DMA interrupt data (RX).
    cyg_handle_t      tx_intr_handle;  // DMA interrupt handle (TX).
    cyg_handle_t      rx_intr_handle;  // DMA interrupt handle (RX).
    cyg_drv_mutex_t   mutex;           // Transfer mutex.
    cyg_drv_cond_t    condvar;         // Transfer condition variable.
    cyg_bool          tx_dma_done;     // Flags used to signal completion.
    cyg_bool          rx_dma_done;     // Flags used to signal completion.
    cyg_bool          cs_up;           // Chip select asserted flag.

} cyg_spi_cortexm_stm32_bus_t;

//-----------------------------------------------------------------------------
// STM32 SPI device.

typedef struct cyg_spi_cortexm_stm32_device_s
{
    // ---- Upper layer data ----
    cyg_spi_device spi_device;  // Upper layer SPI device data.

    // ---- Device setup (user configurable) ----
    cyg_uint8  dev_num;         // Device number.
    cyg_uint8  bus_16bit;       // Use 16 bit (1) or 8 bit (0) transfers.
    cyg_uint8  cl_pol;          // Clock polarity (0 or 1).
    cyg_uint8  cl_pha;          // Clock phase    (0 or 1).
    cyg_uint32 cl_brate;        // Clock baud rate.
    cyg_uint16 cs_up_udly;      // Minimum delay in us between CS up and transfer start.
    cyg_uint16 cs_dw_udly;      // Minimum delay in us between transfer end and CS down.
    cyg_uint16 tr_bt_udly;      // Minimum delay in us between two transfers.

    // ---- Device state (private) ----
    cyg_uint32 spi_cr1_val;     // SPI configuration register (initialised to 0). 

} cyg_spi_cortexm_stm32_device_t;

//-----------------------------------------------------------------------------
// Exported bus data structures.

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1
externC cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus1;
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2
externC cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus2;
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3
externC cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus3;
#endif

//=============================================================================
#endif // CYGONCE_DEVS_SPI_CORTEXM_STM32_H
