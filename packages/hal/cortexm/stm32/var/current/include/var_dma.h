#ifndef CYGONCE_HAL_VAR_DMA_H
#define CYGONCE_HAL_VAR_DMA_H
//=============================================================================
//
//      var_dma.h
//
//      STM32 DMA support
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
// Author(s):   nickg
// Date:        2011-12-07
// Purpose:     STM32 DMA support
// Description: 
// Usage:       #include <cyg/hal/var_dma.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal_cortexm_stm32.h>

#include <cyg/hal/drv_api.h>

//=============================================================================
// DMA stream descriptors

#define CYGHWR_HAL_STM32_DMA_MODE_P2M           0
#define CYGHWR_HAL_STM32_DMA_MODE_M2P           1
#define CYGHWR_HAL_STM32_DMA_MODE_M2M           2


// DMA descriptor. Packs interrupt vector, controller, stream and
// channel IDs together with the mode into a 32 bit descriptor.

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
#define CYGHWR_HAL_STM32_DMA( __ctlr, __stream, __chan, __mode )        \
    (((CYGNUM_HAL_INTERRUPT_DMA##__ctlr##_CH##__stream)<<16) |         \
     ((CYGHWR_HAL_STM32_DMA_MODE_##__mode)<<12) |                       \
     ((__chan)<<8) | ((__stream)<<4) | ((__ctlr)<<0))
#elif defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
#define CYGHWR_HAL_STM32_DMA( __ctlr, __stream, __chan, __mode )        \
    (((CYGNUM_HAL_INTERRUPT_DMA##__ctlr##_STR##__stream)<<16) |         \
     ((CYGHWR_HAL_STM32_DMA_MODE_##__mode)<<12) |                       \
     ((__chan)<<8) | ((__stream)<<4) | ((__ctlr)<<0))
#else
#error "Undefined STM32 family"
#endif

#define CYGHWR_HAL_STM32_DMA_INTERRUPT( __desc )  (((__desc)>>16)&0xFFFF)
#define CYGHWR_HAL_STM32_DMA_MODE( __desc )       (((__desc)>>12)&0xF)
#define CYGHWR_HAL_STM32_DMA_CHANNEL( __desc )    (((__desc)>>8)&0xF)
#define CYGHWR_HAL_STM32_DMA_STREAM( __desc )     (((__desc)>>4)&0xF)
#define CYGHWR_HAL_STM32_DMA_CONTROLLER( __desc ) (((__desc)>>0)&0xF)

//=============================================================================
// API

#ifndef __ASSEMBLER__

typedef struct hal_stm32_dma_stream hal_stm32_dma_stream;

typedef void hal_stm32_dma_callback( hal_stm32_dma_stream *stream, cyg_uint32 count, CYG_ADDRWORD data );

struct hal_stm32_dma_stream
{
    // These fields need to be initialized before calling
    // hal_stm32_dma_init(). This can usually be done statically, when
    // defining an containing data structure.
    
    cyg_uint32                  desc;           // stream descriptor
    hal_stm32_dma_callback      *callback;      // Callback function
    CYG_ADDRWORD                data;           // Client private data

    
    // Runtime data
    
    CYG_ADDRWORD                ctlr;           // Controller base address
    CYG_ADDRWORD                stream;         // Stream/channel index

    cyg_uint32                  ccr;            // Channel control register value
    cyg_bool                    active;         // Channel active
    cyg_uint32                  count;          // Bytes left to transfer
    
    cyg_interrupt               interrupt;      // DMA interrupt object
    cyg_handle_t                handle;         // Interrupt handle
};


__externC void hal_stm32_dma_init( hal_stm32_dma_stream *stream, int pri );

__externC void hal_stm32_dma_delete( hal_stm32_dma_stream *stream );

__externC void hal_stm32_dma_disable( hal_stm32_dma_stream *stream );

__externC void hal_stm32_dma_poll( hal_stm32_dma_stream *stream );

__externC void hal_stm32_dma_configure( hal_stm32_dma_stream *stream, int tfr_size,
                                        cyg_bool no_minc, cyg_bool polled  );

__externC void hal_stm32_dma_configure_circular( hal_stm32_dma_stream *stream,
                                                 cyg_bool enable);

__externC void hal_stm32_dma_start( hal_stm32_dma_stream *stream, void *memory,
                                    CYG_ADDRESS peripheral, cyg_uint32 size );

__externC void hal_stm32_dma_show( hal_stm32_dma_stream *stream );

#endif // __ASSEMBLER__

#endif // CYGONCE_HAL_VAR_DMA_H
//-----------------------------------------------------------------------------
// end of var_dma.h
