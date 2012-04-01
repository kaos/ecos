#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Platform specific registers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011 Free Software Foundation, Inc.
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
// Author(s):   nickg
// Date:        2008-07-30
// Purpose:     STM32X0G EVAL platform specific registers
// Description: 
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm_stm32_stm32x0g_eval.h>

//=============================================================================
// Memory access checks.
//
// Accesses to areas not backed by real devices or memory can cause
// the CPU to hang. These macros allow the GDB stubs to avoid making
// accidental accesses to these areas.

__externC int cyg_hal_stub_permit_data_access( CYG_ADDRESS addr, cyg_uint32 count );

#define CYG_HAL_STUB_PERMIT_DATA_READ(_addr_, _count_) cyg_hal_stub_permit_data_access( _addr_, _count_ )

#define CYG_HAL_STUB_PERMIT_DATA_WRITE(_addr_, _count_ ) cyg_hal_stub_permit_data_access( _addr_, _count_ )

//=============================================================================
// Board LEDs

#define CYGHWR_HAL_STM32X0G_LED1   CYGHWR_HAL_STM32_PIN_OUT( G, 6, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32X0G_LED2   CYGHWR_HAL_STM32_PIN_OUT( G, 8, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32X0G_LED3   CYGHWR_HAL_STM32_PIN_OUT( I, 9, PUSHPULL, NONE, LOW )
#define CYGHWR_HAL_STM32X0G_LED4   CYGHWR_HAL_STM32_PIN_OUT( C, 7, PUSHPULL, NONE, LOW )

// A convenience function to set LEDs. Lowest 4 bits of 'c' correspond to the 4 LEDs.
__externC void hal_stm32x0_led(char c);

//=============================================================================
// Custom Ethernet pin mappings

#define CYGHWR_HAL_STM32_ETH_MII_TX_CRS         CYGHWR_HAL_STM32_PIN_ALTFN_IN(  H,  2, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_COL            CYGHWR_HAL_STM32_PIN_ALTFN_IN(  H,  3, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_RXD2           CYGHWR_HAL_STM32_PIN_ALTFN_IN(  H,  6, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_RXD3           CYGHWR_HAL_STM32_PIN_ALTFN_IN(  H,  7, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_TXD3           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  8, 11, PUSHPULL,  NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_RX_ER          CYGHWR_HAL_STM32_PIN_ALTFN_IN(  I, 10, 11, OPENDRAIN, NONE )
#define CYGHWR_HAL_STM32_ETH_MII_TX_EN          CYGHWR_HAL_STM32_PIN_ALTFN_OUT( G, 11, 11, PUSHPULL,  NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD0           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( G, 13, 11, PUSHPULL,  NONE, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_ETH_MII_TXD1           CYGHWR_HAL_STM32_PIN_ALTFN_OUT( G, 14, 11, PUSHPULL,  NONE, AT_LEAST(50) )
// NOTE: CYGHWR_HAL_STM32_ETH_MII_PPS_OUT not defined

#define CYGHWR_HAL_STM32_ETH_CONFIGURE_MCO()                            \
  CYG_MACRO_START                                                       \
  cyg_uint32 cfgr;                                                      \
  HAL_READ_UINT32( CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CFGR, cfgr ); \
  cfgr &= ~CYGHWR_HAL_STM32_RCC_CFGR_MCO1_MASK;                          \
  cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_MCO1_HSE;                           \
  cfgr &= ~CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_MASK;                       \
  cfgr |= CYGHWR_HAL_STM32_RCC_CFGR_MCO1PRE_1;                          \
  HAL_WRITE_UINT32( CYGHWR_HAL_STM32_RCC + CYGHWR_HAL_STM32_RCC_CFGR, cfgr ); \
  CYG_MACRO_END
  
//=============================================================================
// GPIO pin and DMA definitions for each SPI bus

// NOTE: The SPEED_SPI manifest is declared by the device driver
// (e.g. "devs/spi/cortexm/stm32/<vsn>/src/spi_stm32.c") and is not
// currently defined in a header.

#ifndef CYGHWR_HAL_STM32_SPI1_REMAP
#define CYGHWR_HAL_STM32_SPI1_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  5, 5, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  A,  6, 5, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI1_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  7, 5, PUSHPULL, NONE, SPEED_SPI )
#else
#define CYGHWR_HAL_STM32_SPI1_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  3, 5, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  B,  4, 5, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI1_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  5, 5, PUSHPULL, NONE, SPEED_SPI )
#endif
#define CYGHWR_HAL_STM32_SPI1_REMAP_CONFIG      0

#define CYGHWR_HAL_STM32_SPI1_DMA_TX            CYGHWR_HAL_STM32_DMA( 2, 3, 3, M2P )
#define CYGHWR_HAL_STM32_SPI1_DMA_RX            CYGHWR_HAL_STM32_DMA( 2, 0, 3, P2M )



#define CYGHWR_HAL_STM32_SPI2_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( I, 1, 5, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI2_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  I, 2, 5, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI2_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( I, 3, 5, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI2_REMAP_CONFIG      0

#define CYGHWR_HAL_STM32_SPI2_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 4, 0, M2P )
#define CYGHWR_HAL_STM32_SPI2_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 3, 0, P2M )


    
#ifndef CYGHWR_HAL_STM32_SPI3_REMAP
#define CYGHWR_HAL_STM32_SPI3_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  3, 6, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  B,  4, 6, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI3_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  5, 6, PUSHPULL, NONE, SPEED_SPI )
#else
#define CYGHWR_HAL_STM32_SPI3_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C, 10, 6, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  C, 11, 6, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI3_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C, 12, 6, PUSHPULL, NONE, SPEED_SPI )
#endif
#define CYGHWR_HAL_STM32_SPI3_REMAP_CONFIG      0

#define CYGHWR_HAL_STM32_SPI3_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 2, 0, M2P )
#define CYGHWR_HAL_STM32_SPI3_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 0, 0, P2M )


//=============================================================================
// GPIO pin and DMA definitions for each I2C bus

#define CYGHWR_HAL_STM32_I2C1_SCL               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  6, 4, OPENDRAIN, PULLUP, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_I2C1_SDA               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  9, 4, OPENDRAIN, PULLUP, AT_LEAST(50) )

#define CYGHWR_HAL_STM32_I2C1_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 7, 1, M2P )
#define CYGHWR_HAL_STM32_I2C1_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 5, 1, P2M )

#define CYGHWR_HAL_STM32_I2C2_SCL               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 10, 4, OPENDRAIN, PULLUP, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_I2C2_SDA               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 11, 4, OPENDRAIN, PULLUP, AT_LEAST(50) )

#define CYGHWR_HAL_STM32_I2C2_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 7, 7, M2P )
#define CYGHWR_HAL_STM32_I2C2_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 3, 7, P2M )

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
