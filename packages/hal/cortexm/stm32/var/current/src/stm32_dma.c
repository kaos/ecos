/*=============================================================================
//
//      stm32_dma.c
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
// Author(s):    nickg
// Contributors: 
// Date:         2009-10-11
// Purpose:      STM32 DMA support
// Description:  This file provides the implementation for the STM32's
//               on-chip DMA controllers.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal_cortexm_stm32.h>

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/var_dma.h>

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_ass.h>

//=============================================================================

#if 0
#define dma_diag( __fmt, ... ) diag_printf("DMA: %20s[%3d]: " __fmt, __FUNCTION__, __LINE__, ## __VA_ARGS__ );
#define dma_dump_buf(__addr, __size ) diag_dump_buf( __addr, __size )
#else
#define dma_diag( __fmt, ... )
#define dma_dump_buf(__addr, __size )
#endif

//=============================================================================

static const struct
{
    CYG_ADDRWORD        base;
    cyg_uint32          clock;
} hal_stm32_dma_controller[] =
{
    { 0, 0 },
    { CYGHWR_HAL_STM32_DMA1, CYGHWR_HAL_STM32_DMA1_CLOCK },
    { CYGHWR_HAL_STM32_DMA2, CYGHWR_HAL_STM32_DMA2_CLOCK },
};

//-----------------------------------------------------------------------------

static cyg_uint32 hal_stm32_dma_isr( cyg_vector_t vector, CYG_ADDRWORD data )
{
    hal_stm32_dma_stream *stream = (hal_stm32_dma_stream *)data;
    cyg_uint32 ret = CYG_ISR_HANDLED;    
    cyg_uint32 isr;

    HAL_READ_UINT32( stream->ctlr+CYGHWR_HAL_STM32_DMA_ISR_REG(stream->stream), isr );

    dma_diag("ctlr %08x stream %d chan %d isr %08x\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc), isr );

    if( isr & CYGHWR_HAL_STM32_DMA_ISR_TCIF(stream->stream) )
    {
        // Clear all stream interrupt bits
        HAL_WRITE_UINT32( stream->ctlr+CYGHWR_HAL_STM32_DMA_IFCR_REG(stream->stream),
                          CYGHWR_HAL_STM32_DMA_IFCR_MASK(stream->stream) );
        
        if( (stream->ccr & CYGHWR_HAL_STM32_DMA_CCR_CIRC) == 0)
        {
            // Disable the stream
            HAL_WRITE_UINT32( stream->ctlr+CYGHWR_HAL_STM32_DMA_CCR(stream->stream), 0 );
        }

        // Update the count
        HAL_READ_UINT32( stream->ctlr+CYGHWR_HAL_STM32_DMA_CNDTR(stream->stream), stream->count );

        ret = CYG_ISR_CALL_DSR;
    }

    return ret;
}

//-----------------------------------------------------------------------------

static void hal_stm32_dma_dsr( cyg_vector_t vector, cyg_ucount32 count, CYG_ADDRWORD data )
{
    hal_stm32_dma_stream *stream = (hal_stm32_dma_stream *)data;

    dma_diag("ctlr %08x stream %d chan %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc) );
    
    stream->callback( stream, stream->count, stream->data );

    stream->active = false;    
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_init( hal_stm32_dma_stream *stream, int pri )
{
    stream->ctlr = hal_stm32_dma_controller[CYGHWR_HAL_STM32_DMA_CONTROLLER(stream->desc)].base;
    stream->stream = CYGHWR_HAL_STM32_DMA_STREAM(stream->desc);

    dma_diag("ctlr %08x stream %d chan %d pri %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc), pri );
             
    cyg_drv_interrupt_create( CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc),
                              pri,
                              (CYG_ADDRWORD)stream,
                              hal_stm32_dma_isr,
                              hal_stm32_dma_dsr,
                              &stream->handle,
                              &stream->interrupt );
    cyg_drv_interrupt_attach( stream->handle );
    cyg_drv_interrupt_unmask( CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc) );

    // Enable DMA controller clock
    CYGHWR_HAL_STM32_CLOCK_ENABLE( hal_stm32_dma_controller[CYGHWR_HAL_STM32_DMA_CONTROLLER(stream->desc)].clock );


    // Clear CCR, disable channel and put into known state
    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CCR(stream->stream), 0 );

    
    // Initialize a private copy of the CCR, we don't write this to
    // the hardware until we are ready to start the transfer.

    stream->ccr = CYGHWR_HAL_STM32_DMA_CCR_EN;

#if defined (CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
    // Select channel number in F2/F4 variants. The F1 variants simply
    // have the various device DMA request lines wire-ORed together.
    stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_CHSEL(CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc));
#endif
    
    // Set stream direction
    if( CYGHWR_HAL_STM32_DMA_MODE(stream->desc) == CYGHWR_HAL_STM32_DMA_MODE_M2P )
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_MEM2P;

    // Set memory increment mode
    stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_MINC;
           
    // Transfer end interrupt enable
    stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_TCIE;

    // Use top 2 bits of priority to define DMA stream priority
    stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_PL((pri>>6)&3);
    
    dma_diag("ccr %08x\n", stream->ccr);
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_delete( hal_stm32_dma_stream *stream )
{
    dma_diag("ctlr %08x stream %d chan %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc) );

    // Clear CCR, disables stream
    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CCR(stream->stream), 0 );

    // Mask, detach and delete interrupt object
    cyg_drv_interrupt_mask( CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc) );
    cyg_drv_interrupt_detach( stream->handle );
    cyg_drv_interrupt_delete( stream->handle );
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_disable( hal_stm32_dma_stream *stream )
{
    dma_diag("ctlr %08x stream %d chan %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc) );

    // Clear CCR, disables stream
    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CCR(stream->stream), 0 );
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_poll( hal_stm32_dma_stream *stream )
{
//    dma_diag("ctlr %08x stream %d chan %d\n", stream->ctlr, stream->stream,
//             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc) );
    
    if( stream->active )
        if( hal_stm32_dma_isr( CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc), (CYG_ADDRWORD)stream ) & CYG_ISR_CALL_DSR )
            hal_stm32_dma_dsr( CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc), 1, (CYG_ADDRWORD)stream );
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_configure( hal_stm32_dma_stream *stream, int tfr_size, cyg_bool no_minc, cyg_bool polled  )
{
    dma_diag("ctlr %08x stream %d chan %d tfr_size %d no_minc %d polled %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc), tfr_size, no_minc, polled );
    
    if( tfr_size == 8 )
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_PSIZE8 | CYGHWR_HAL_STM32_DMA_CCR_MSIZE8;
    else if( tfr_size == 16 )
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_PSIZE16 | CYGHWR_HAL_STM32_DMA_CCR_MSIZE16;
    else 
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_PSIZE32 | CYGHWR_HAL_STM32_DMA_CCR_MSIZE32;

    // Clear MINC bit if not wanted
    if( no_minc )
        stream->ccr &= ~CYGHWR_HAL_STM32_DMA_CCR_MINC;
    else
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_MINC;

    // Clear interrupt enables if polled mode, otherwise enable them
    if( polled )
        stream->ccr &= ~(CYGHWR_HAL_STM32_DMA_CCR_TCIE | CYGHWR_HAL_STM32_DMA_CCR_TEIE);
    else
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_TCIE | CYGHWR_HAL_STM32_DMA_CCR_TEIE;
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_configure_circular( hal_stm32_dma_stream *stream, cyg_bool enable)
{
    if (enable) {
        stream->ccr |= CYGHWR_HAL_STM32_DMA_CCR_CIRC;
    } else {
        stream->ccr &= ~CYGHWR_HAL_STM32_DMA_CCR_CIRC;
    }
}

//-----------------------------------------------------------------------------

void hal_stm32_dma_start( hal_stm32_dma_stream *stream, void *memory, CYG_ADDRESS peripheral, cyg_uint32 size )
{
    dma_diag("ctlr %08x stream %d chan %d mem %08x peri %08x size %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc), memory, peripheral, size );

    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CMAR(stream->stream), (cyg_uint32)memory );

    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CPAR(stream->stream), peripheral );

    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CNDTR(stream->stream), size );

    HAL_WRITE_UINT32(stream->ctlr+CYGHWR_HAL_STM32_DMA_CCR(stream->stream), stream->ccr );

    stream->active = true;
}

//=============================================================================

void hal_stm32_dma_show( hal_stm32_dma_stream *stream )
{
    cyg_uint32 dma = stream->ctlr;
    cyg_uint32 chan = stream->stream;
    cyg_uint32 reg;
    
    dma_diag("ctlr %08x stream %d chan %d\n", stream->ctlr, stream->stream,
             CYGHWR_HAL_STM32_DMA_CHANNEL(stream->desc) );
    dma_diag("vector %d stream->ccr %08x\n", CYGHWR_HAL_STM32_DMA_INTERRUPT(stream->desc), stream->ccr );

    HAL_READ_UINT32( dma+CYGHWR_HAL_STM32_DMA_ISR_REG(chan), reg );
    dma_diag("DMA   ISR:   %08x\n", reg );
    
    HAL_READ_UINT32( dma+CYGHWR_HAL_STM32_DMA_CCR(chan), reg );   dma_diag("DMA %d CCR:   %08x\n", chan, reg );
    HAL_READ_UINT32( dma+CYGHWR_HAL_STM32_DMA_CNDTR(chan), reg ); dma_diag("DMA %d CNDTR: %08x\n", chan, reg );
    HAL_READ_UINT32( dma+CYGHWR_HAL_STM32_DMA_CPAR(chan), reg );  dma_diag("DMA %d CPAR:  %08x\n", chan, reg );
    HAL_READ_UINT32( dma+CYGHWR_HAL_STM32_DMA_CMAR(chan), reg );  dma_diag("DMA %d CMAR:  %08x\n", chan, reg );
    
}


//=============================================================================
/* EOF stm32_dma.c */
