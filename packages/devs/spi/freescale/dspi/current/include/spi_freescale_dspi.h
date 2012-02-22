#ifndef CYGONCE_DEVS_SPI_FREESCALE_DSPI_H
#define CYGONCE_DEVS_SPI_FREESCALE_DSPI_H
//=============================================================================
//
//      spi_freescale_dspi.h
//
//      Header definitions for Freescale DSPI driver.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):   ilijak
// Date:        2011-11-03
// Purpose:     Freescale DSPI driver definitions.
// Description:
// Usage:       #include <cyg/io/spi_freescale_dspi.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/io_spi.h>
#include <pkgconf/devs_spi_freescale_dspi.h>
#include <pkgconf/hal_freescale_edma.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/spi.h>
#include <cyg/hal/freescale_edma.h>
#include <cyg/io/spi_freescale_dspi_io.h>

//-----------------------------------------------------------------------------
// Macro for defining a SPI device and attaching it to the appropriate bus.
//
// _name_     is the name of the SPI device.  This will be used to reference a
//              data structure of type cyg_spi_device which can be passed to the
//              SPI driver API without needing a cast.
// _bus_      is the bus number to which this device is attached (0, 1 or 2).
// _csnum_    is the chip select line used for this device, numbered from 0.
// _fmsz_     is set device SPI frame size (bits)
// _clpol_    is the SPI bus clock polarity used by the device.  This must be
//               set to 1 if clock inactive state is high, 0 if clock inactive
//               state is low.
// _clpha_    is the SPI bus clock phase used by the device.
// _brate_    is the SPI bus clock baud rate used by the device, measured in Hz.
// _csup_dly_ is the minimum delay between chip select assert and transfer
//              start, measured in microseconds.
// _csdw_dly_ is the minimum delay between transfer end and chip select deassert,
//              measured in delay units.
// _trbt_dly_ is the minimum delay between consecutive transfers.
// _dlu_      is delay unit in ns
// _dbr_      is enabling double baud rate feature


#define CYGNUM_DSPI_DELAY_UNIT(__val) (__val/10)

#define CYG_SPI_BUS_NAME(_b_) cyg_spi_dspi_bus ## _b_

#define CYG_DEVS_SPI_FREESCALE_DSPI_DEVICE( \
  _name_, _bus_, _csnum_, _fmsz_, _clpol_, _clpha_, _brate_, _csup_dly_, _csdw_dly_, _trbt_dly_, _dlu_, _dbr_ \
) \
 cyg_spi_freescale_dspi_device_t _name_ ##_fs_dspi CYG_SPI_DEVICE_ON_BUS(_bus_) = { \
{ .spi_bus    = (cyg_spi_bus*) &CYG_SPI_BUS_NAME(_bus_) }, \
  .dev_num    = _csnum_, \
  .clocking.bus_16bit  = _fmsz_ > 8 ? 1 : 0, \
  .clocking.frame_size = _fmsz_, \
  .clocking.cl_pol     = _clpol_, \
  .clocking.cl_pha     = _clpha_, \
  .clocking.cl_brate   = _brate_, \
  .clocking.cs_up_udly = _csup_dly_, \
  .clocking.cs_dw_udly = _csdw_dly_, \
  .clocking.tr_bt_udly = _trbt_dly_, \
  .clocking.dl_unit    = CYGNUM_DSPI_DELAY_UNIT(_dlu_), \
  .clocking.cl_dbr     = _dbr_, \
  .clocking.dspi_ctar  = 0 \
}; \
extern cyg_spi_device _name_ __attribute__((alias ( #_name_ "_fs_dspi" )))

enum { SPI_DMA_CHAN_TX_I, SPI_DMA_CHAN_RX_I };

//-----------------------------------------------------------------------------
// Freescale DSPI bus configuration and state.

typedef struct cyg_spi_freescale_dspi_bus_setup_s
{
    cyghwr_devs_freescale_dspi_t*   dspi_p;    // Base address of SPI register block.
    cyghwr_hal_freescale_dma_set_t* dma_set_p;       // DMA configuration block.
    cyg_vector_t                    intr_num;        // DSPI interrupt vector
    cyg_priority_t                  intr_prio;       // Interrupt priority
    cyg_uint32                      mcr_opt ;        // Module Configuratyon Register options
    const cyg_uint32*               spi_pin_list_p;  // List of GPIOs used by the SPI interface.
    const cyg_uint32*               cs_pin_list_p;   // List of GPIOs used as chip selects.
    cyg_uint8                       cs_pin_num;      // Number of chip selects for this bus.
} cyg_spi_freescale_dspi_bus_setup_t;

#define SPI_DMA_CHAN_I(__dma_set,__rt) (__dma_set->chan_p[SPI_DMA_CHAN_ ## __rt ## _I].dma_chan_i)

typedef struct cyg_spi_freescale_dspi_bus_s
{
    // ---- Upper layer data ----
    cyg_spi_bus       spi_bus;         // Upper layer SPI bus data.

    // ---- Bus configuration constants ----
    const cyg_spi_freescale_dspi_bus_setup_t* setup_p;
    // ---- Driver state (private) ----
    // DMA transfer control descriptors
    const cyghwr_hal_freescale_edma_tcd_t* tx_dma_tcd_ini_p; // TCD init.
    const cyghwr_hal_freescale_edma_tcd_t* rx_dma_tcd_ini_p; //     data
    volatile cyghwr_hal_freescale_edma_tcd_t* rx_dma_tcd_p; // DMA TCD (RX)
    volatile cyghwr_hal_freescale_edma_tcd_t* tx_dma_tcd_p; // DMA TCD (TX)
    volatile cyg_uint32*   pushque_p;             // Tx command queue
    cyg_uint16             pushque_n;             // Tx command buffer size
    cyg_uint8              txfifo_n;              // TxFIFO size
    cyg_uint8              rxfifo_n;              // RxFIFO size
    cyg_interrupt          intr_data;       // Interrupt state
    cyg_handle_t           intr_handle;     // Interrupt handle
    cyg_drv_mutex_t        transfer_mutex;  // Transfer mutex.
    cyg_drv_cond_t         transfer_done;   // Transfer condition variable.
    cyg_uint32             clock_freq;      // Clock provided by hal
} cyg_spi_freescale_dspi_bus_t;

//-----------------------------------------------------------------------------
// Freescale DSPI device.

typedef struct cyg_freescale_dspi_clocking_s {
    cyg_uint32 dspi_ctar;  // DSPI Clock and Transfer Attributes Rregister shadow.
    cyg_uint32 cl_brate;   // Clock baud rate.
    cyg_uint8  bus_16bit;  // Use 16 bit (1) or 8 bit (0) transfers.
    cyg_uint8  cl_pol;     // Clock polarity (0 or 1).
    cyg_uint8  cl_pha;     // Clock phase    (0 or 1).
    cyg_uint8  cl_dbr;     // Use double baud-rate feature if needed
    cyg_uint8  cs_up_udly; // Minimum delay in us/ns between CS up and transfer start.
    cyg_uint8  cs_dw_udly; // Minimum delay in us/ns between transfer end and CS down.
    cyg_uint8  tr_bt_udly; // Minimum delay in us/ns between two transfers.
    cyg_uint8  dl_unit;    // Delay unit (1/10 * ns)
    cyg_uint8  lsb_first;  // LSbit shifted first.
    cyg_uint8  frame_size; // Device SPI frame size (bits).
} cyg_freescale_dspi_clocking_t;;

typedef struct cyg_spi_freescale_dspi_device_s
{
    // ---- Upper layer data ----
    cyg_spi_device spi_device;  // Upper layer SPI device data.

    // ---- Device setup (user configurable) ----
    cyg_freescale_dspi_clocking_t clocking;
    cyg_uint8  dev_num;   // Device SPI select.
    cyg_uint8  chip_sel;
    // ---- Device state (private) ----
} cyg_spi_freescale_dspi_device_t;

//-----------------------------------------------------------------------------
// Exported bus data structures.

#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI0
externC cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus0;
#endif

#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI1
externC cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus1;
#endif

#ifdef CYGHWR_DEVS_SPI_FREESCALE_DSPI2
externC cyg_spi_freescale_dspi_bus_t cyg_spi_dspi_bus2;
#endif

__externC void cyghwr_devs_freescale_dspi_diag(cyg_spi_freescale_dspi_bus_t* spi_bus_p);
__externC void cyghwr_devs_freescale_dspi_diag_array(char* name_p,
                                                     volatile cyg_uint32* fifo_p,
                                                     cyg_uint32 fifo_n);

//=============================================================================
#endif // CYGONCE_DEVS_SPI_FREESCALE_DSPI_H
