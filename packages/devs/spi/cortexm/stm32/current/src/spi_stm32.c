//=============================================================================
//
//      spi_stm32.c
//
//      SPI driver implementation for STM32
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011, 2012 Free Software Foundation, Inc.
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
// Author(s):   Chris Holgate, nickg
// Date:        2008-11-27
// Purpose:     STM32 SPI driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>

#include <cyg/io/spi.h>
#include <cyg/io/spi_stm32.h>

#include <pkgconf/devs_spi_cortexm_stm32.h>

#include <string.h>

//-----------------------------------------------------------------------------
// Diagnostics

#if 0
#define spi_diag( __fmt, ... ) diag_printf("SPI: %30s[%3d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define spi_dump_buf( __addr, __size ) diag_dump_buf( __addr, __size )
#else
#define spi_diag( __fmt, ... )
#define spi_dump_buf( __addr, __size )
#endif

//-----------------------------------------------------------------------------
// Bus frequencies

__externC cyg_uint32 hal_stm32_pclk1;
__externC cyg_uint32 hal_stm32_pclk2;

//-----------------------------------------------------------------------------
// API function call forward references.

static void stm32_transaction_begin    (cyg_spi_device*);
static void stm32_transaction_transfer (cyg_spi_device*, cyg_bool, cyg_uint32, const cyg_uint8*, cyg_uint8*, cyg_bool);
static void stm32_transaction_tick     (cyg_spi_device*, cyg_bool, cyg_uint32);
static void stm32_transaction_end      (cyg_spi_device*);
static int  stm32_get_config           (cyg_spi_device*, cyg_uint32, void*, cyg_uint32*);
static int  stm32_set_config           (cyg_spi_device*, cyg_uint32, const void*, cyg_uint32*);

//-----------------------------------------------------------------------------
// DMA callbacks

static void stm32_dma_tx_callback( hal_stm32_dma_stream *stream, cyg_uint32 count, CYG_ADDRWORD data );
static void stm32_dma_rx_callback( hal_stm32_dma_stream *stream, cyg_uint32 count, CYG_ADDRWORD data );

//-----------------------------------------------------------------------------
// Null data source and sink must be placed in the on-chip SRAM.  This is
// either done explicitly (bounce buffers instantiated) or implicitly (no
// bounce buffers implies that the data area is already on SRAM).

#if (defined (CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1) && (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE > 0)) || \
  (defined (CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2) && (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE > 0)) || \
  (defined (CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3) && (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE > 0))
static cyg_uint16 dma_tx_null __attribute__((section (".sram"))) = 0xFFFF;
static cyg_uint16 dma_rx_null __attribute__((section (".sram"))) = 0xFFFF;

#else
static cyg_uint16 dma_tx_null = 0xFFFF;
static cyg_uint16 dma_rx_null = 0xFFFF;
#endif

//-----------------------------------------------------------------------------
// Useful GPIO macros for 'dynamic' pin setup.

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 2)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPEED_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 10)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPEED_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 50)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPEED_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ

#else
#error "Invalid SPI bus toggle rate."
#endif

#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 2)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SPEED_SPI CYGHWR_HAL_STM32_GPIO_OSPEED_2MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 25)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SPEED_SPI CYGHWR_HAL_STM32_GPIO_OSPEED_25MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 50)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SPEED_SPI CYGHWR_HAL_STM32_GPIO_OSPEED_50MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 80)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SPEED_SPI CYGHWR_HAL_STM32_GPIO_OSPEED_HIGH

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 100)
#define CYGHWR_HAL_STM32_GPIO_OSPEED_SPEED_SPI CYGHWR_HAL_STM32_GPIO_OSPEED_HIGH

#else
#error "Invalid SPI bus toggle rate."
#endif

#else

#error "Unknown STM32 family"

#endif

#define SPI_CS( __port, __bit )         CYGHWR_HAL_STM32_PIN_OUT( __port, __bit, PUSHPULL, NONE, SPEED_SPI )

//-----------------------------------------------------------------------------

// Instantiate the bus state data structures.

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1
static const cyg_uint32 bus1_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1_CS_GPIOS };
static const cyg_uint32 bus1_spi_gpio_list[] = { CYGHWR_HAL_STM32_SPI1_SCK,
                                                 CYGHWR_HAL_STM32_SPI1_MISO,
                                                 CYGHWR_HAL_STM32_SPI1_MOSI };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE > 0)
static cyg_uint8 bus1_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus1_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus1_setup = {
  .apb_freq                         = &hal_stm32_pclk2,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI1,
  .spi_enable                       = CYGHWR_HAL_STM32_SPI1_CLOCK,
  .cs_gpio_num                      = sizeof (bus1_cs_gpio_list)/sizeof(cyg_uint32),
  .cs_gpio_list                     = bus1_cs_gpio_list,
  .spi_gpio_list                    = bus1_spi_gpio_list,
  .spi_gpio_remap                   = CYGHWR_HAL_STM32_SPI1_REMAP_CONFIG,
  .dma_tx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_TXINTR_PRI,
  .dma_rx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_RXINTR_PRI,

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE > 0)
  .bbuf_size                        = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE,
  .bbuf_tx                          = bus1_tx_bbuf,
  .bbuf_rx                          = bus1_rx_bbuf,
#else
  .bbuf_size                        = 0,
#endif
};

cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus1 = {
  .spi_bus.spi_transaction_begin    = stm32_transaction_begin,
  .spi_bus.spi_transaction_transfer = stm32_transaction_transfer,
  .spi_bus.spi_transaction_tick     = stm32_transaction_tick,
  .spi_bus.spi_transaction_end      = stm32_transaction_end,
  .spi_bus.spi_get_config           = stm32_get_config,
  .spi_bus.spi_set_config           = stm32_set_config,
  .setup                            = &bus1_setup,
  .cs_up                            = false,

  .dma_tx_stream.desc                 = CYGHWR_HAL_STM32_SPI1_DMA_TX,
  .dma_tx_stream.callback             = stm32_dma_tx_callback,
  .dma_tx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus1,

  .dma_rx_stream.desc                 = CYGHWR_HAL_STM32_SPI1_DMA_RX,
  .dma_rx_stream.callback             = stm32_dma_rx_callback,
  .dma_rx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus1,
  
  
  
};
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2
static const cyg_uint32 bus2_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2_CS_GPIOS };
static const cyg_uint32 bus2_spi_gpio_list[] = { CYGHWR_HAL_STM32_SPI2_SCK,
                                                 CYGHWR_HAL_STM32_SPI2_MISO,
                                                 CYGHWR_HAL_STM32_SPI2_MOSI };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE > 0)
static cyg_uint8 bus2_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus2_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus2_setup = {
  .apb_freq                         = &hal_stm32_pclk1,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI2,
  .spi_enable                       = CYGHWR_HAL_STM32_SPI2_CLOCK,
  .cs_gpio_num                      = sizeof (bus2_cs_gpio_list)/sizeof(cyg_uint32),
  .cs_gpio_list                     = bus2_cs_gpio_list,
  .spi_gpio_list                    = bus2_spi_gpio_list,
  .spi_gpio_remap                   = CYGHWR_HAL_STM32_SPI2_REMAP_CONFIG,  
  .dma_tx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_TXINTR_PRI,
  .dma_rx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_RXINTR_PRI,

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE > 0)
  .bbuf_size                        = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE,
  .bbuf_tx                          = bus2_tx_bbuf,
  .bbuf_rx                          = bus2_rx_bbuf,
#else
  .bbuf_size                        = 0,
#endif
};

cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus2 = {
  .spi_bus.spi_transaction_begin    = stm32_transaction_begin,
  .spi_bus.spi_transaction_transfer = stm32_transaction_transfer,
  .spi_bus.spi_transaction_tick     = stm32_transaction_tick,
  .spi_bus.spi_transaction_end      = stm32_transaction_end,
  .spi_bus.spi_get_config           = stm32_get_config,
  .spi_bus.spi_set_config           = stm32_set_config,
  .setup                            = &bus2_setup,
  .cs_up                            = false,

  .dma_tx_stream.desc                 = CYGHWR_HAL_STM32_SPI2_DMA_TX,
  .dma_tx_stream.callback             = stm32_dma_tx_callback,
  .dma_tx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus2,

  .dma_rx_stream.desc                 = CYGHWR_HAL_STM32_SPI2_DMA_RX,
  .dma_rx_stream.callback             = stm32_dma_rx_callback,
  .dma_rx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus2,
  
  
};
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3
static const cyg_uint32 bus3_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3_CS_GPIOS };
static const cyg_uint32 bus3_spi_gpio_list[] = { CYGHWR_HAL_STM32_SPI3_SCK,
                                                 CYGHWR_HAL_STM32_SPI3_MISO,
                                                 CYGHWR_HAL_STM32_SPI3_MOSI };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE > 0)
static cyg_uint8 bus3_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus3_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus3_setup = {
  .apb_freq                         = &hal_stm32_pclk1,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI3,
  .spi_enable                       = CYGHWR_HAL_STM32_SPI3_CLOCK,
  .cs_gpio_num                      = sizeof (bus3_cs_gpio_list)/sizeof(cyg_uint32),
  .cs_gpio_list                     = bus3_cs_gpio_list,
  .spi_gpio_list                    = bus3_spi_gpio_list,
  .spi_gpio_remap                   = CYGHWR_HAL_STM32_SPI3_REMAP_CONFIG,  
  .dma_tx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_TXINTR_PRI,
  .dma_rx_intr_pri                  = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_RXINTR_PRI,

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE > 0)
  .bbuf_size                        = CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE,
  .bbuf_tx                          = bus3_tx_bbuf,
  .bbuf_rx                          = bus3_rx_bbuf,
#else
  .bbuf_size                        = 0,
#endif
};

cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus3 = {
  .spi_bus.spi_transaction_begin    = stm32_transaction_begin,
  .spi_bus.spi_transaction_transfer = stm32_transaction_transfer,
  .spi_bus.spi_transaction_tick     = stm32_transaction_tick,
  .spi_bus.spi_transaction_end      = stm32_transaction_end,
  .spi_bus.spi_get_config           = stm32_get_config,
  .spi_bus.spi_set_config           = stm32_set_config,
  .setup                            = &bus3_setup,
  .cs_up                            = false,

  .dma_tx_stream.desc                 = CYGHWR_HAL_STM32_SPI3_DMA_TX,
  .dma_tx_stream.callback             = stm32_dma_tx_callback,
  .dma_tx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus3,

  .dma_rx_stream.desc                 = CYGHWR_HAL_STM32_SPI3_DMA_RX,
  .dma_rx_stream.callback             = stm32_dma_rx_callback,
  .dma_rx_stream.data                 = (CYG_ADDRWORD)&cyg_spi_stm32_bus3,


};
#endif

//-----------------------------------------------------------------------------
// Configure a GPIO pin as a SPI chip select line.

static inline void stm32_spi_gpio_cs_setup
  (cyg_uint32 gpio_num)
{
  CYGHWR_HAL_STM32_GPIO_SET (gpio_num);
  CYGHWR_HAL_STM32_GPIO_OUT (gpio_num, 1);
}

//-----------------------------------------------------------------------------
// Drive a GPIO pin as a SPI chip select line.

static inline void stm32_spi_chip_select
  (cyg_uint32 gpio_num, cyg_bool assert)
{
  CYGHWR_HAL_STM32_GPIO_OUT( gpio_num, assert ? 0 : 1);
}

//-----------------------------------------------------------------------------
// DMA Callbacks. The DMA driver has disabled the DMA channel and
// masked the interrupt condition, here we need to wake up the client
// thread.

static void stm32_dma_tx_callback( hal_stm32_dma_stream *stream, cyg_uint32 count, CYG_ADDRWORD data )
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) data;

  cyg_drv_dsr_lock ();
  stm32_bus->tx_dma_done = true;
  cyg_drv_cond_signal (&stm32_bus->condvar);
  cyg_drv_dsr_unlock ();
}

static void stm32_dma_rx_callback( hal_stm32_dma_stream *stream, cyg_uint32 count, CYG_ADDRWORD data )
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) data;
 
  cyg_drv_dsr_lock ();
  stm32_bus->rx_dma_done = true;
  cyg_drv_cond_signal (&stm32_bus->condvar);
  cyg_drv_dsr_unlock ();
}

//-----------------------------------------------------------------------------
// Set up a new SPI bus on initialisation.

static void stm32_spi_bus_setup 
  (cyg_spi_cortexm_stm32_bus_t* stm32_bus)
{
  int i;
  cyg_haladdress reg_addr;
  cyg_uint32 reg_data;  

  spi_diag("bus %p\n", stm32_bus );  
  // Set up the GPIOs for use as chip select lines.
  for (i = 0; i < stm32_bus->setup->cs_gpio_num; i ++) {
    stm32_spi_gpio_cs_setup (stm32_bus->setup->cs_gpio_list[i]);
  }

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
  // Remap GPIO pins if required
  if( stm32_bus->setup->spi_gpio_remap )
  {
      CYG_ADDRESS afio = CYGHWR_HAL_STM32_AFIO;
      cyg_uint32 mapr;
      spi_diag("remap %08x\n", stm32_bus->setup->spi_gpio_remap );
      CYGHWR_HAL_STM32_CLOCK_ENABLE( CYGHWR_HAL_STM32_AFIO_CLOCK );
      HAL_READ_UINT32( afio+CYGHWR_HAL_STM32_AFIO_MAPR, mapr );
      mapr |= stm32_bus->setup->spi_gpio_remap;
      HAL_WRITE_UINT32( afio+CYGHWR_HAL_STM32_AFIO_MAPR, mapr );
  }
#endif
  
  CYGHWR_HAL_STM32_GPIO_SET( stm32_bus->setup->spi_gpio_list[0] );
  CYGHWR_HAL_STM32_GPIO_SET( stm32_bus->setup->spi_gpio_list[1] );
  CYGHWR_HAL_STM32_GPIO_SET( stm32_bus->setup->spi_gpio_list[2] );

  CYGHWR_HAL_STM32_CLOCK_ENABLE( stm32_bus->setup->spi_enable );
  //CYGHWR_HAL_STM32_CLOCK_ENABLE( stm32_bus->setup->dma_enable );

  // Set up SPI default configuration.
  reg_addr = stm32_bus->setup->spi_reg_base + CYGHWR_HAL_STM32_SPI_CR2;
  reg_data = CYGHWR_HAL_STM32_SPI_CR2_TXDMAEN | CYGHWR_HAL_STM32_SPI_CR2_RXDMAEN;
  HAL_WRITE_UINT32 (reg_addr, reg_data);

  // Initialise the synchronisation primitivies.
  cyg_drv_mutex_init (&stm32_bus->mutex);
  cyg_drv_cond_init (&stm32_bus->condvar, &stm32_bus->mutex);

  // Initialize DMA streams
  hal_stm32_dma_init( &stm32_bus->dma_tx_stream, stm32_bus->setup->dma_tx_intr_pri );
  hal_stm32_dma_init( &stm32_bus->dma_rx_stream, stm32_bus->setup->dma_rx_intr_pri );

  // Call upper layer bus init.
  CYG_SPI_BUS_COMMON_INIT(&stm32_bus->spi_bus);
}

//-----------------------------------------------------------------------------
// Set up a DMA channel.

static void dma_channel_setup
  (cyg_spi_device* device, cyg_uint8* data_buf, cyg_uint32 count, cyg_bool is_tx, cyg_bool polled)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;
    hal_stm32_dma_stream *stream = is_tx ? &stm32_bus->dma_tx_stream : &stm32_bus->dma_rx_stream;

    hal_stm32_dma_configure( stream,
                             stm32_device->bus_16bit ? 16 : 8,
                             (data_buf==NULL),
                             polled );

    if( data_buf == NULL )
        data_buf = (cyg_uint8 *)(is_tx ? &dma_tx_null : &dma_rx_null);
    
    hal_stm32_dma_start( stream,
                         data_buf,
                         stm32_bus->setup->spi_reg_base+CYGHWR_HAL_STM32_SPI_DR,
                         count );
 
    hal_stm32_dma_show( stream );
}

//-----------------------------------------------------------------------------
// Initiate a DMA transfer over the SPI interface.

static void spi_transaction_dma 
  (cyg_spi_device* device, cyg_bool tick_only, cyg_bool polled, cyg_uint32 count, 
  const cyg_uint8* tx_data, cyg_uint8* rx_data, cyg_bool drop_cs)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  cyg_haladdress reg_addr;

  cyg_haladdress spi_reg_base = stm32_bus->setup->spi_reg_base;

  spi_diag("device %p\n", device );    
  // Ensure the chip select is asserted, inserting inter-transaction guard 
  // time if required.  Note that when ticking the device we do not touch the CS.
  if (!stm32_bus->cs_up && !tick_only) {
    CYGACC_CALL_IF_DELAY_US (stm32_device->tr_bt_udly);
    stm32_spi_chip_select (stm32_bus->setup->cs_gpio_list[stm32_device->dev_num], true);        
    stm32_bus->cs_up = true;
    CYGACC_CALL_IF_DELAY_US (stm32_device->cs_up_udly);
  }

  // Clear done flags prior to configuring DMA:
  stm32_bus->tx_dma_done = false;
  stm32_bus->rx_dma_done = false;

  // Set up the DMA channels.
  dma_channel_setup (device, (cyg_uint8*) tx_data, count, true, polled);
  dma_channel_setup (device, rx_data, count, false, polled);

  // Run the DMA (polling for completion).
  if (polled) {
    // Enable the SPI controller.
    reg_addr = spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
    HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val | CYGHWR_HAL_STM32_SPI_CR1_SPE);

    while( !(stm32_bus->tx_dma_done && stm32_bus->rx_dma_done) )
    {
        hal_stm32_dma_poll( &stm32_bus->dma_tx_stream );
        hal_stm32_dma_poll( &stm32_bus->dma_rx_stream );
    }
  }
  // Run the DMA (interrupt driven).
  else {
    cyg_drv_mutex_lock (&stm32_bus->mutex);
    cyg_drv_dsr_lock ();

    // Enable the SPI controller.
    reg_addr = spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
    HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val | CYGHWR_HAL_STM32_SPI_CR1_SPE);

    // Sit back and wait for the ISR/DSRs to signal completion.
    do {
      cyg_drv_cond_wait (&stm32_bus->condvar);
    } while (!(stm32_bus->tx_dma_done && stm32_bus->rx_dma_done));

    cyg_drv_dsr_unlock ();
    cyg_drv_mutex_unlock (&stm32_bus->mutex);
  }

  
  // Disable the SPI controller.
  reg_addr = spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
  HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val);
        
  // Deassert the chip select.
  if (drop_cs && !tick_only) {
    CYGACC_CALL_IF_DELAY_US (stm32_device->cs_dw_udly);
    stm32_spi_chip_select (stm32_bus->setup->cs_gpio_list[stm32_device->dev_num], false);        
    stm32_bus->cs_up = false;
  }     
}

//-----------------------------------------------------------------------------
// Calculate BR bits for SPI_CR1.
static int calculate_br_bits
  (cyg_spi_cortexm_stm32_bus_t* bus, cyg_uint32 *target_clockrate, cyg_uint32 *br)
{
  cyg_uint32 divided_clk;

  // Calculate the maximum viable bus speed.
  divided_clk = *bus->setup->apb_freq / 2;
  for (*br = 0; (*br < 7) && (divided_clk > *target_clockrate); (*br)++)
    divided_clk >>= 1;

  if ( divided_clk <= *target_clockrate )
    return 0;

  return -1;
}

//-----------------------------------------------------------------------------
// Initialise SPI interfaces on startup.

static void CYGBLD_ATTRIB_C_INIT_PRI(CYG_INIT_BUS_SPI)
stm32_spi_init(void)
{
  spi_diag("\n");      
#if defined(CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3) && \
    defined(CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3_DISABLE_DEBUG_PORT)
  // Disable debug port, freeing up SPI bus 3 pins.
  cyg_uint32 reg_val;
  HAL_READ_UINT32 (CYGHWR_HAL_STM32_AFIO + CYGHWR_HAL_STM32_AFIO_MAPR, reg_val);
  reg_val &= ~((cyg_uint32) CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_MASK);
  reg_val |= CYGHWR_HAL_STM32_AFIO_MAPR_SWJ_SWDPDIS;
  HAL_WRITE_UINT32 (CYGHWR_HAL_STM32_AFIO + CYGHWR_HAL_STM32_AFIO_MAPR, reg_val);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1
  stm32_spi_bus_setup (&cyg_spi_stm32_bus1);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2
  stm32_spi_bus_setup (&cyg_spi_stm32_bus2);
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3
  stm32_spi_bus_setup (&cyg_spi_stm32_bus3);
#endif
}

//-----------------------------------------------------------------------------
// Start a SPI transaction.

static void stm32_transaction_begin    
  (cyg_spi_device* device)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  cyg_haladdress reg_addr;
  cyg_uint32 reg_data, br;

  spi_diag("\n");
  // On the first transaction, generate the values to be programmed into the
  // SPI configuration registers for this device and cache them.  This avoids
  // having to recalculate the prescaler for every transaction.
  if (!stm32_device->spi_cr1_val) {
    reg_data = CYGHWR_HAL_STM32_SPI_CR1_MSTR | 
      CYGHWR_HAL_STM32_SPI_CR1_SSI | CYGHWR_HAL_STM32_SPI_CR1_SSM;
    if (stm32_device->cl_pol)
      reg_data |= CYGHWR_HAL_STM32_SPI_CR1_CPOL;
    if (stm32_device->cl_pha)
      reg_data |= CYGHWR_HAL_STM32_SPI_CR1_CPHA;
    if (stm32_device->bus_16bit)
      reg_data |= CYGHWR_HAL_STM32_SPI_CR1_DFF;

    // Get divider bits
    if ( 0 != calculate_br_bits(stm32_bus, (cyg_uint32 *)&(stm32_device->cl_brate), &br) )
      CYG_ASSERT (false, "STM32 SPI : Cannot run bus slowly enough for peripheral.");

    reg_data |= CYGHWR_HAL_STM32_SPI_CR1_BR (br);

    // Cache the configuration register settings.
    stm32_device->spi_cr1_val = reg_data;
  }

  // Set up the SPI controller.
  reg_addr = stm32_bus->setup->spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
  HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val);
}

//-----------------------------------------------------------------------------
// Run a transaction transfer.

static void stm32_transaction_transfer 
  (cyg_spi_device* device, cyg_bool polled, cyg_uint32 count, 
  const cyg_uint8* tx_data, cyg_uint8* rx_data, cyg_bool drop_cs)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  // Check for unsupported transactions.
  CYG_ASSERT (count > 0, "STM32 SPI : Null transfer requested.");

  spi_diag("count %d\n", count);
  if( tx_data ) spi_dump_buf(tx_data, count );
  
  // We check that the buffers are half-word aligned and that count is a 
  // multiple of two in order to carry out the 16-bit transfer.
  if (stm32_device->bus_16bit) {
    CYG_ASSERT (!(count & 1) && !((cyg_uint32) tx_data & 1) && !((cyg_uint32) rx_data & 1),   
      "STM32 SPI : Misaligned data in 16-bit transfer.");
  }

  // Perform transfer via the bounce buffers.  
  if (stm32_bus->setup->bbuf_size != 0) {
    cyg_uint8* tx_local = NULL;
    cyg_uint8* rx_local = NULL;

    // If the requested transfer is too large for the bounce buffer we assert 
    // in debug builds and truncate in production builds.
    if (count > stm32_bus->setup->bbuf_size) {
      CYG_ASSERT (false, "STM32 SPI : Transfer exceeds bounce buffer size.");
      count = stm32_bus->setup->bbuf_size;
    }
    if (tx_data != NULL) {
      tx_local = stm32_bus->setup->bbuf_tx;        
      memcpy (tx_local, tx_data, count);
    }
    if (rx_data != NULL) {
      rx_local = stm32_bus->setup->bbuf_rx;        
    }
    spi_transaction_dma (device, false, polled, count, tx_local, rx_local, drop_cs);
    if (rx_data != NULL) {
      memcpy (rx_data, rx_local, count);
    }
  }

  // Perform conventional transfer.
  else {
    spi_transaction_dma (device, false, polled, count, tx_data, rx_data, drop_cs);
  }

  spi_diag("done\n");
  if( rx_data ) spi_dump_buf(rx_data, count );
  
}

//-----------------------------------------------------------------------------
// Carry out a bus tick operation - this just pushes the required number of
// zeros onto the bus, leaving the chip select in its current state.

static void stm32_transaction_tick 
  (cyg_spi_device* device, cyg_bool polled, cyg_uint32 count)
{
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  // Check for unsupported transactions.
  CYG_ASSERT (count > 0, "STM32 SPI : Null transfer requested.");

  spi_diag("count %d\n", count);

  // We check that count is a multiple of two in order to carry out the 16-bit transfer.
  if (stm32_device->bus_16bit) {
    CYG_ASSERT (!(count & 1),   
      "STM32 SPI : Misaligned data in 16-bit transfer.");
  }

  // Perform null transfer.
  spi_transaction_dma (device, true, polled, count, NULL, NULL, false);
}

//-----------------------------------------------------------------------------
// Terminate a SPI transaction, disabling the SPI controller.

static void stm32_transaction_end 
  (cyg_spi_device* device)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  cyg_haladdress reg_addr;

  spi_diag("\n");  
  // Ensure that the chip select is deasserted.
  if (stm32_bus->cs_up) {
    CYGACC_CALL_IF_DELAY_US (stm32_device->cs_dw_udly);
    stm32_spi_chip_select (stm32_bus->setup->cs_gpio_list[stm32_device->dev_num], false);   
    stm32_bus->cs_up = false;
  }     

  // Ensure the SPI controller is disabled.
  reg_addr = stm32_bus->setup->spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
  HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val);
}

//-----------------------------------------------------------------------------
static int stm32_get_config 
  (cyg_spi_device* device, cyg_uint32 key, void* buf, cyg_uint32* len)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;
  cyg_uint32* data_p = buf;

  switch (key)
  {
  case CYG_IO_GET_CONFIG_SPI_CLOCKRATE :
    // Sanity check
    if (NULL == len) {
        CYG_ASSERT (false, "STM32 SPI : Null pointer as len argument for stm32_get_config().");
        return -1;
    }
    if (sizeof(cyg_uint32) != *len) {
        CYG_ASSERT (false, "STM32 SPI : Invalid length with stm32_get_config().");
        return -1;
    }
    if (NULL == buf) {
        CYG_ASSERT (false, "STM32 SPI : Null poiter as buf argument for stm32_get_config().");
        return -1;
    }

    *data_p = *stm32_bus->setup->apb_freq >> ((( stm32_device->spi_cr1_val >> 3 ) & 7) + 1 ) ;
    return 0;

  default :
    break;
  }

  return -1;
}

//-----------------------------------------------------------------------------
static int stm32_set_config
  (cyg_spi_device* device, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) device->spi_bus;
  cyg_spi_cortexm_stm32_device_t* stm32_device = (cyg_spi_cortexm_stm32_device_t*) device;

  cyg_uint32 br;

  switch (key)
  {
  case CYG_IO_SET_CONFIG_SPI_CLOCKRATE :
    // Sanity check
    if (NULL == len) {
      CYG_ASSERT (false, "STM32 SPI : Null pointer as len argument for stm32_set_config().");
      return -1;
    }
    if (sizeof(cyg_uint32) != *len) {
      CYG_ASSERT (false, "STM32 SPI : Invalid length with stm32_set_config().");
      return -1;
    }
    if (NULL == buf) {
      CYG_ASSERT (false, "STM32 SPI : Null pointer as buf argument for stm32_set_config().");
      return -1;
    }

    // Get divider bits
    if ( 0 != calculate_br_bits(stm32_bus, (cyg_uint32 *)buf, &br) ) {
      CYG_ASSERT (false, "STM32 SPI : Cannot run bus as slowly as requested.");
      return -1;
    }

    // Update the cache of the configuration register settings.
    stm32_device->spi_cr1_val &= ~CYGHWR_HAL_STM32_SPI_CR1_BR(7);
    stm32_device->spi_cr1_val |= CYGHWR_HAL_STM32_SPI_CR1_BR(br);

    return 0;

  default :
    break;
  }

  return -1;
}

//=============================================================================
