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
// Work out the bus clock frequencies.

#define APB1_FREQ ((CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL) / \
  (CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK1_DIV)) 
#define APB2_FREQ ((CYGARC_HAL_CORTEXM_STM32_INPUT_CLOCK * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PLL_MUL) / \
  (CYGHWR_HAL_CORTEXM_STM32_CLOCK_HCLK_DIV * CYGHWR_HAL_CORTEXM_STM32_CLOCK_PCLK2_DIV)) 

//-----------------------------------------------------------------------------
// API function call forward references.

static void stm32_transaction_begin    (cyg_spi_device*);
static void stm32_transaction_transfer (cyg_spi_device*, cyg_bool, cyg_uint32, const cyg_uint8*, cyg_uint8*, cyg_bool);
static void stm32_transaction_tick     (cyg_spi_device*, cyg_bool, cyg_uint32);
static void stm32_transaction_end      (cyg_spi_device*);
static int  stm32_get_config           (cyg_spi_device*, cyg_uint32, void*, cyg_uint32*);
static int  stm32_set_config           (cyg_spi_device*, cyg_uint32, const void*, cyg_uint32*);

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
// Instantiate the bus state data structures.

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1
static const cyg_uint8 bus1_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS1_CS_GPIOS };
static const cyg_uint8 bus1_spi_gpio_list[] = { 0x05, 0x06, 0x07 };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE > 0)
static cyg_uint8 bus1_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus1_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS1_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus1_setup = {
  .apb_freq                         = APB2_FREQ,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI1,
  .dma_reg_base                     = CYGHWR_HAL_STM32_DMA1,
  .dma_tx_channel                   = 3,
  .dma_rx_channel                   = 2,
  .cs_gpio_num                      = sizeof (bus1_cs_gpio_list),
  .cs_gpio_list                     = bus1_cs_gpio_list,
  .spi_gpio_list                    = bus1_spi_gpio_list,
  .dma_tx_intr                      = CYGNUM_HAL_INTERRUPT_DMA1_CH3,
  .dma_rx_intr                      = CYGNUM_HAL_INTERRUPT_DMA1_CH2,
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
  .cs_up                            = false
};
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2
static const cyg_uint8 bus2_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS2_CS_GPIOS };
static const cyg_uint8 bus2_spi_gpio_list[] = { 0x1D, 0x1E, 0x1F };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE > 0)
static cyg_uint8 bus2_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus2_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS2_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus2_setup = {
  .apb_freq                         = APB1_FREQ,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI2,
  .dma_reg_base                     = CYGHWR_HAL_STM32_DMA1,
  .dma_tx_channel                   = 5,
  .dma_rx_channel                   = 4,
  .cs_gpio_num                      = sizeof (bus2_cs_gpio_list),
  .cs_gpio_list                     = bus2_cs_gpio_list,
  .spi_gpio_list                    = bus2_spi_gpio_list,
  .dma_tx_intr                      = CYGNUM_HAL_INTERRUPT_DMA1_CH5,
  .dma_rx_intr                      = CYGNUM_HAL_INTERRUPT_DMA1_CH4,
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
  .cs_up                            = false
};
#endif

#ifdef CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3
static const cyg_uint8 bus3_cs_gpio_list[] = { CYGHWR_DEVS_SPI_CORTEXM_STM32_BUS3_CS_GPIOS };
static const cyg_uint8 bus3_spi_gpio_list[] = { 0x13, 0x14, 0x15 };

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE > 0)
static cyg_uint8 bus3_tx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE] 
  __attribute__((aligned (2), section (".sram"))) = { 0 };
static cyg_uint8 bus3_rx_bbuf [CYGNUM_DEVS_SPI_CORTEXM_STM32_BUS3_BBUF_SIZE] 
  __attribute__((alugned (2), section (".sram"))) = { 0 };
#endif

static const cyg_spi_cortexm_stm32_bus_setup_t bus3_setup = {
  .apb_freq                         = APB1_FREQ,
  .spi_reg_base                     = CYGHWR_HAL_STM32_SPI3,
  .dma_reg_base                     = CYGHWR_HAL_STM32_DMA2,
  .dma_tx_channel                   = 2,
  .dma_rx_channel                   = 1
  .cs_gpio_num                      = sizeof (bus3_cs_gpio_list),
  .cs_gpio_list                     = bus3_cs_gpio_list,
  .spi_gpio_list                    = bus3_spi_gpio_list,
  .dma_tx_intr                      = CYGNUM_HAL_INTERRUPT_DMA2_CH2,
  .dma_rx_intr                      = CYGNUM_HAL_INTERRUPT_DMA2_CH1,
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
  .cs_up                            = false
};
#endif

//-----------------------------------------------------------------------------
// Useful GPIO macros for 'dynamic' pin setup.

static const cyg_uint32 stm32_gpio_port_offsets[] = {
  CYGHWR_HAL_STM32_GPIOA - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOB - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOC - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOD - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOE - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOF - CYGHWR_HAL_STM32_GPIOA,
  CYGHWR_HAL_STM32_GPIOG - CYGHWR_HAL_STM32_GPIOA
};

#define STM32_GPIO_PINSPEC(__port, __bit, __mode, __cnf )          \
  (stm32_gpio_port_offsets[__port] | (__bit<<16) |                 \
  (CYGHWR_HAL_STM32_GPIO_MODE_##__mode) |                          \
  (CYGHWR_HAL_STM32_GPIO_CNF_##__cnf))

#if (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 2)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_2MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 10)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_10MHZ

#elif (CYGNUM_DEVS_SPI_CORTEXM_STM32_PIN_TOGGLE_RATE == 50)
#define CYGHWR_HAL_STM32_GPIO_MODE_OUT_SPI CYGHWR_HAL_STM32_GPIO_MODE_OUT_50MHZ

#else
#error "Invalid SPI bus toggle rate."
#endif

//-----------------------------------------------------------------------------
// Configure a GPIO pin as a SPI chip select line.

static inline void stm32_spi_gpio_cs_setup
  (cyg_uint32 gpio_num)
{
  cyg_uint32 port, pin, pinspec;

  // Check that the pin number is in range (16 pins per port).
  pin = gpio_num & 0xF;
  port = gpio_num >> 4;
  CYG_ASSERT (port < 7, "STM32 SPI : Invalid GPIO number in chip select list."); 

  // Generate the pin setup specification and configure it.
  pinspec = STM32_GPIO_PINSPEC (port, pin, OUT_SPI, OUT_PUSHPULL);
  CYGHWR_HAL_STM32_GPIO_SET (pinspec);
  CYGHWR_HAL_STM32_GPIO_OUT (pinspec, 1);
}

//-----------------------------------------------------------------------------
// Drive a GPIO pin as a SPI chip select line.

static inline void stm32_spi_chip_select
  (cyg_uint32 gpio_num, cyg_bool assert)
{
  cyg_uint32 port, pin, pinspec;

  // Check that the pin number is in range (16 pins per port).
  pin = gpio_num & 0xF;
  port = gpio_num >> 4;
  CYG_ASSERT (port < 7, "STM32 SPI : Invalid GPIO number in chip select list."); 

  // Generate the pin setup specification and drive it.
  pinspec = STM32_GPIO_PINSPEC (port, pin, OUT_SPI, OUT_PUSHPULL);
  CYGHWR_HAL_STM32_GPIO_OUT (pinspec, assert ? 0 : 1);
}

//-----------------------------------------------------------------------------
// Implement DMA ISRs.  These disable the DMA channel, mask the interrupt 
// condition and schedule their respective DSRs.

static cyg_uint32 stm32_tx_ISR 
  (cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) data;
  cyg_uint32 chan = stm32_bus->setup->dma_tx_channel;
  cyg_haladdress reg_addr;

  // Disable the DMA channel.
  cyg_drv_isr_lock ();
  reg_addr = stm32_bus->setup->dma_reg_base + CYGHWR_HAL_STM32_DMA_CCR (chan);
  HAL_WRITE_UINT32 (reg_addr, 0);

  // Clear down the interrupts.
  reg_addr = stm32_bus->setup->dma_reg_base + CYGHWR_HAL_STM32_DMA_IFCR;
  HAL_WRITE_UINT32 (reg_addr, CYGHWR_HAL_STM32_DMA_IFCR_MASK (chan));

  cyg_drv_interrupt_acknowledge (vector);
  cyg_drv_interrupt_mask (vector);
  cyg_drv_isr_unlock ();

  return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

static cyg_uint32 stm32_rx_ISR 
  (cyg_vector_t vector, cyg_addrword_t data)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) data;
  cyg_uint32 chan = stm32_bus->setup->dma_rx_channel;
  cyg_haladdress reg_addr;

  // Disable the DMA channel.
  cyg_drv_isr_lock ();
  reg_addr = stm32_bus->setup->dma_reg_base + CYGHWR_HAL_STM32_DMA_CCR (chan);
  HAL_WRITE_UINT32 (reg_addr, 0);

  // Clear down the interrupts.
  reg_addr = stm32_bus->setup->dma_reg_base + CYGHWR_HAL_STM32_DMA_IFCR;
  HAL_WRITE_UINT32 (reg_addr, CYGHWR_HAL_STM32_DMA_IFCR_MASK (chan));

  cyg_drv_interrupt_acknowledge (vector);
  cyg_drv_interrupt_mask (vector);
  cyg_drv_isr_unlock ();

  return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}

//-----------------------------------------------------------------------------
// Implement DMA DSRs.  These clear down the interrupt conditions and assert 
// their respective 'transaction complete' flags.

static void stm32_tx_DSR 
  (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
  cyg_spi_cortexm_stm32_bus_t* stm32_bus = (cyg_spi_cortexm_stm32_bus_t*) data;

  cyg_drv_dsr_lock ();
  stm32_bus->tx_dma_done = true;
  cyg_drv_cond_signal (&stm32_bus->condvar);
  cyg_drv_dsr_unlock ();
}

static void stm32_rx_DSR 
  (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
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
  cyg_uint32 pin, pinspec, reg_data;

  // Set up the GPIOs for use as chip select lines.
  for (i = 0; i < stm32_bus->setup->cs_gpio_num; i ++) {
    stm32_spi_gpio_cs_setup (stm32_bus->setup->cs_gpio_list[i]);
  }

  // Configure the SPI clock output pin.
  pin = stm32_bus->setup->spi_gpio_list[0];
  pinspec = STM32_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), OUT_SPI, ALT_PUSHPULL);
  CYGHWR_HAL_STM32_GPIO_SET (pinspec);

  // Configure the SPI MISO input.
  pin = stm32_bus->setup->spi_gpio_list[1];
  pinspec = STM32_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), IN, PULLUP);
  CYGHWR_HAL_STM32_GPIO_SET (pinspec);

  // Configure the SPI MOSI output.  
  pin = stm32_bus->setup->spi_gpio_list[2];
  pinspec = STM32_GPIO_PINSPEC ((pin >> 4), (pin & 0xF), OUT_SPI, ALT_PUSHPULL);
  CYGHWR_HAL_STM32_GPIO_SET (pinspec);

  // Set up SPI default configuration.
  reg_addr = stm32_bus->setup->spi_reg_base + CYGHWR_HAL_STM32_SPI_CR2;
  reg_data = CYGHWR_HAL_STM32_SPI_CR2_TXDMAEN | CYGHWR_HAL_STM32_SPI_CR2_RXDMAEN;
  HAL_WRITE_UINT32 (reg_addr, reg_data);

  // Ensure that the DMA clocks are enabled.
  reg_addr = CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_AHBENR;
  HAL_READ_UINT32 (reg_addr, reg_data);
  if (stm32_bus->setup->dma_reg_base == CYGHWR_HAL_STM32_DMA1)
    reg_data |= CYGHWR_HAL_STM32_RCC_AHBENR_DMA1;
  else
    reg_data |= CYGHWR_HAL_STM32_RCC_AHBENR_DMA2;
  HAL_WRITE_UINT32 (reg_addr, reg_data);

  // Initialise the synchronisation primitivies.
  cyg_drv_mutex_init (&stm32_bus->mutex);
  cyg_drv_cond_init (&stm32_bus->condvar, &stm32_bus->mutex);

  // Hook up the ISRs and DSRs.
  cyg_drv_interrupt_create (stm32_bus->setup->dma_tx_intr, stm32_bus->setup->dma_tx_intr_pri, 
    (cyg_addrword_t) stm32_bus, stm32_tx_ISR, stm32_tx_DSR, &stm32_bus->tx_intr_handle, 
    &stm32_bus->tx_intr_data);
  cyg_drv_interrupt_attach (stm32_bus->tx_intr_handle);

  cyg_drv_interrupt_create (stm32_bus->setup->dma_rx_intr, stm32_bus->setup->dma_rx_intr_pri, 
    (cyg_addrword_t) stm32_bus, stm32_rx_ISR, stm32_rx_DSR, &stm32_bus->rx_intr_handle, 
    &stm32_bus->rx_intr_data);
  cyg_drv_interrupt_attach (stm32_bus->rx_intr_handle);

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

  cyg_uint32 chan, reg_data;
  cyg_haladdress dma_reg_base, spi_reg_base, dma_addr, reg_addr;

  // Extract address and channel information.
  dma_reg_base = stm32_bus->setup->dma_reg_base;
  spi_reg_base = stm32_bus->setup->spi_reg_base;
  chan = is_tx ? stm32_bus->setup->dma_tx_channel : stm32_bus->setup->dma_rx_channel;

  // Default options for the DMA channel.
  reg_data = CYGHWR_HAL_STM32_DMA_CCR_EN;

  // Do not enable interrupts in polled mode.
  if (!polled)
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_TCIE | CYGHWR_HAL_STM32_DMA_CCR_TEIE;

  // Set DMA channel direction and priority level.  The receive channel has higher
  // priority so that the inbound buffer is always cleared first.
  if (is_tx) 
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_DIR | CYGHWR_HAL_STM32_DMA_CCR_PLMEDIUM;
  else 
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_PLHIGH;
 
  // Set the correct transfer size.
  if (stm32_device->bus_16bit)
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_PSIZE16 | CYGHWR_HAL_STM32_DMA_CCR_MSIZE16;
  else 
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_PSIZE8 | CYGHWR_HAL_STM32_DMA_CCR_MSIZE8;

  // Do not use memory address incrementing for dummy data.
  if (data_buf != NULL) {
    reg_data |= CYGHWR_HAL_STM32_DMA_CCR_MINC;
    dma_addr = (cyg_haladdress) data_buf;
  }
  else 
    dma_addr = (cyg_haladdress) (is_tx ? &dma_tx_null : &dma_rx_null);

  // Program up the DMA memory address.
  reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CMAR (chan);
  HAL_WRITE_UINT32 (reg_addr, dma_addr);
  
  // Program up the peripheral memory address.
  dma_addr = spi_reg_base + CYGHWR_HAL_STM32_SPI_DR;
  reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CPAR (chan);
  HAL_WRITE_UINT32 (reg_addr, dma_addr);

  // Program up the data buffer size.
  reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CNDTR (chan);
  HAL_WRITE_UINT32 (reg_addr, count);

  // Enable the DMA via the configuration register.
  reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CCR (chan);
  HAL_WRITE_UINT32 (reg_addr, reg_data);
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
  cyg_uint32 chan, reg_data;

  cyg_haladdress dma_reg_base = stm32_bus->setup->dma_reg_base;
  cyg_haladdress spi_reg_base = stm32_bus->setup->spi_reg_base;

  // Ensure the chip select is asserted, inserting inter-transaction guard 
  // time if required.  Note that when ticking the device we do not touch the CS.
  if (!stm32_bus->cs_up && !tick_only) {
    CYGACC_CALL_IF_DELAY_US (stm32_device->tr_bt_udly);
    stm32_spi_chip_select (stm32_bus->setup->cs_gpio_list[stm32_device->dev_num], true);        
    stm32_bus->cs_up = true;
    CYGACC_CALL_IF_DELAY_US (stm32_device->cs_up_udly);
  }

  // Set up the DMA channels.
  dma_channel_setup (device, (cyg_uint8*) tx_data, count, true, polled);
  dma_channel_setup (device, rx_data, count, false, polled);

  // Run the DMA (polling for completion).
  if (polled) {
    cyg_bool transfer_done = false;

    // Enable the SPI controller.
    reg_addr = spi_reg_base + CYGHWR_HAL_STM32_SPI_CR1;
    HAL_WRITE_UINT32 (reg_addr, stm32_device->spi_cr1_val | CYGHWR_HAL_STM32_SPI_CR1_SPE);

    // Spin waiting on both DMA status flags.  Trap bus errors and assert in
    // debug builds or return garbage in production builds.
    reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_ISR;
    do {
      HAL_READ_UINT32 (reg_addr, reg_data);
      if ((reg_data & CYGHWR_HAL_STM32_DMA_ISR_TEIF (stm32_bus->setup->dma_tx_channel)) ||
          (reg_data & CYGHWR_HAL_STM32_DMA_ISR_TEIF (stm32_bus->setup->dma_rx_channel))) {
        CYG_ASSERT (false, "STM32 SPI : DMA bus fault - enable bounce buffers.");
        transfer_done = true;
      }
      if ((reg_data & CYGHWR_HAL_STM32_DMA_ISR_TCIF (stm32_bus->setup->dma_tx_channel)) &&
          (reg_data & CYGHWR_HAL_STM32_DMA_ISR_TCIF (stm32_bus->setup->dma_rx_channel))) {
        transfer_done = true;
      }
    } while (!transfer_done);

    // Disable the DMA channels on completion and clear the status flags.
    chan = stm32_bus->setup->dma_tx_channel;
    reg_data = CYGHWR_HAL_STM32_DMA_IFCR_MASK (chan);
    reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CCR (chan);
    HAL_WRITE_UINT32 (reg_addr, 0);

    chan = stm32_bus->setup->dma_rx_channel;
    reg_data |= CYGHWR_HAL_STM32_DMA_IFCR_MASK (chan);
    reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_CCR (chan);
    HAL_WRITE_UINT32 (reg_addr, 0);

    reg_addr = dma_reg_base + CYGHWR_HAL_STM32_DMA_IFCR;
    HAL_WRITE_UINT32 (reg_addr, reg_data);
  }

  // Run the DMA (interrupt driven).
  else {
    cyg_drv_mutex_lock (&stm32_bus->mutex);
    cyg_drv_dsr_lock ();
    stm32_bus->tx_dma_done = false;
    stm32_bus->rx_dma_done = false;

    cyg_drv_interrupt_unmask (stm32_bus->setup->dma_tx_intr);
    cyg_drv_interrupt_unmask (stm32_bus->setup->dma_rx_intr);

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
// Initialise SPI interfaces on startup.

void cyg_spi_cortexm_stm32_init 
  (void)
{
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
  cyg_uint32 reg_data, divided_clk, br;

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

    // Calculate the maximum viable bus speed.
    divided_clk = stm32_bus->setup->apb_freq / 2;
    for (br = 0; (br < 7) && (divided_clk > stm32_device->cl_brate); br++)
      divided_clk >>= 1;
    CYG_ASSERT (divided_clk <= stm32_device->cl_brate, 
      "STM32 SPI : Cannot run bus slowly enough for peripheral.");
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
// Note that no dynamic configuration options are currently defined.

static int stm32_get_config 
  (cyg_spi_device* dev, cyg_uint32 key, void* buf, cyg_uint32* len)
{
    return -1;
}

static int stm32_set_config 
  (cyg_spi_device* dev, cyg_uint32 key, const void* buf, cyg_uint32* len)
{
    return -1;
}

//=============================================================================
