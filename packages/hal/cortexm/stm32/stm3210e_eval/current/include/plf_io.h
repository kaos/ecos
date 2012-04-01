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
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Purpose:     STM3210E EVAL platform specific registers
// Description: 
// Usage:       #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm_stm32_stm3210e_eval.h>

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

#define HAL_AM29XXXXX_UNCACHED_ADDRESS(__addr) (__addr)

//=============================================================================
// I2C busses and devices

# define HAL_I2C_EXPORTED_DEVICES                                       \
    __externC cyg_i2c_bus                  hal_stm32_i2c_bus1;          \
    __externC cyg_i2c_device               hal_stm32_i2c_temperature;


//=============================================================================
// I2C bus pin configurations

#if defined(CYGHWR_HAL_STM32_I2C1_REMAP)
#define CYGHWR_HAL_STM32_I2C1_SCL               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  8, NA, OPENDRAIN, NA, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_I2C1_SDA               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  9, NA, OPENDRAIN, NA, AT_LEAST(50) )
#else
#define CYGHWR_HAL_STM32_I2C1_SCL               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  6, NA, OPENDRAIN, NA, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_I2C1_SDA               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  7, NA, OPENDRAIN, NA, AT_LEAST(50) )
#endif

#define CYGHWR_HAL_STM32_I2C1_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 6, 0, M2P )
#define CYGHWR_HAL_STM32_I2C1_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 7, 0, P2M )


#define CYGHWR_HAL_STM32_I2C2_SCL               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 10, NA, OPENDRAIN, NA, AT_LEAST(50) )
#define CYGHWR_HAL_STM32_I2C2_SDA               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 11, NA, OPENDRAIN, NA, AT_LEAST(50) )

#define CYGHWR_HAL_STM32_I2C2_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 4, 0, M2P )
#define CYGHWR_HAL_STM32_I2C2_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 5, 0, P2M )

//=============================================================================
// SPI bus pin configurations

// NOTE: The SPEED_SPI manifest is declared by the device driver
// (e.g. "devs/spi/cortexm/stm32/<vsn>/src/spi_stm32.c") and is not
// currently defined in a header.

#ifndef CYGHWR_HAL_STM32_SPI1_REMAP
#define CYGHWR_HAL_STM32_SPI1_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  5, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  A,  6, NA, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI1_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( A,  7, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_REMAP_CONFIG      0
#else
#define CYGHWR_HAL_STM32_SPI1_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  3, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  B,  4, NA, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI1_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  5, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI1_REMAP_CONFIG      CYGHWR_HAL_STM32_AFIO_MAPR_SPI1_RMP
#endif

#define CYGHWR_HAL_STM32_SPI1_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 3, 0, M2P )
#define CYGHWR_HAL_STM32_SPI1_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 2, 0, P2M )


#define CYGHWR_HAL_STM32_SPI2_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 13, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI2_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  B, 14, NA, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI2_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B, 15, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI2_REMAP_CONFIG      0

#define CYGHWR_HAL_STM32_SPI2_DMA_TX            CYGHWR_HAL_STM32_DMA( 1, 5, 0, M2P )
#define CYGHWR_HAL_STM32_SPI2_DMA_RX            CYGHWR_HAL_STM32_DMA( 1, 4, 0, P2M )



#ifndef CYGHWR_HAL_STM32_SPI3_REMAP
#define CYGHWR_HAL_STM32_SPI3_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  3, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  B,  4, NA, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI3_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( B,  5, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_REMAP_CONFIG      0
#else
#define CYGHWR_HAL_STM32_SPI3_SCK               CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C, 10, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_MISO              CYGHWR_HAL_STM32_PIN_ALTFN_IN(  C, 11, NA, NA,       PULLUP )
#define CYGHWR_HAL_STM32_SPI3_MOSI              CYGHWR_HAL_STM32_PIN_ALTFN_OUT( C, 12, NA, PUSHPULL, NONE, SPEED_SPI )
#define CYGHWR_HAL_STM32_SPI3_REMAP_CONFIG      CYGHWR_HAL_STM32_AFIO_MAPR_SPI3_RMP
#endif

#define CYGHWR_HAL_STM32_SPI3_DMA_TX            CYGHWR_HAL_STM32_DMA( 2, 2, 0, M2P )
#define CYGHWR_HAL_STM32_SPI3_DMA_RX            CYGHWR_HAL_STM32_DMA( 2, 1, 0, P2M )


//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H
