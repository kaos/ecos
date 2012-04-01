/*==========================================================================
//
//      hal_dma.c
//
//      Cortex-M Actel A2F DMA channels configuration
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    ccoutand
// Contributors:
// Date:         2011-02-03
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_cortexm.h>
#include <pkgconf/hal_cortexm_a2fxxx.h>
#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#endif

#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // HAL header
#include <cyg/hal/hal_if.h>             // HAL header
#include <cyg/hal/drv_api.h>

#ifdef CYGDBG_HAL_CORTEXM_A2FXXX_DMA_TRACE
# define DMA_TRACE(args...) diag_printf(args)
#else
# define DMA_TRACE(args...)
#endif

//-----------------------------------------------------------------------------
// DMA channel ISR/DSR handling

typedef struct a2fxxx_dma_ch {
    cyg_ISR_t*      isr;
    cyg_DSR_t*      dsr;
    cyg_uint32      isr_ret;
    cyg_addrword_t  data;
} a2fxxx_dma_ch;

typedef struct a2fxxx_dma_info {
    cyg_uint32      init;
    cyg_uint32      base;
    cyg_handle_t    interrupt_handle;
    cyg_interrupt   interrupt_data;
    a2fxxx_dma_ch   ch[CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL];
    cyg_uint32      dma_cr[CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL];
    cyg_uint32      dma_sr;
} a2fxxx_dma_info;

static a2fxxx_dma_info a2fxxx_dma = {
    .base = CYGHWR_HAL_A2FXXX_DMA,
    .init =0,
};


//-----------------------------------------------------------------------------
// DMA ISR handler

static cyg_uint32
a2fxxx_dma_isr (cyg_vector_t vector, cyg_addrword_t data)
{
    a2fxxx_dma_info *dma = (a2fxxx_dma_info *) data;
    cyg_uint32 sr = 0;
    cyg_uint8 i = 0, j = 0;

    HAL_READ_UINT32(dma->base + CYGHWR_HAL_A2FXXX_DMA_BUFFER_STATUS, sr);
    dma->dma_sr |= sr;

    DMA_TRACE("DMA interrupt, sr 0x%x\n", dma->dma_sr);

    while(i < 16){
        if( sr & 0x1 ){
            j = i >> 1;
            if(dma->ch[j].isr != NULL){
              dma->ch[j].isr_ret = dma->ch[j].isr(i, dma->ch[j].data);
            }
        }
        sr = sr >> 1; i++;
    }
    cyg_drv_interrupt_acknowledge (CYGNUM_HAL_INTERRUPT_DMA);
    cyg_drv_interrupt_mask (CYGNUM_HAL_INTERRUPT_DMA);
    return (CYG_ISR_CALL_DSR | CYG_ISR_HANDLED);
}


//-----------------------------------------------------------------------------
// DMA DSR handler

static void
a2fxxx_dma_dsr (cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    a2fxxx_dma_info *dma = (a2fxxx_dma_info *) data;
    cyg_uint8 i = 0, j = 0;
    cyg_uint32 sr;

    cyg_drv_isr_lock ();
    sr = dma->dma_sr;
    dma->dma_sr = 0;
    cyg_drv_isr_unlock ();

    while(i < 16){
        if( sr & 0x1 ){
            j = i >> 1;
            if( (dma->ch[j].dsr != NULL) &&
                           (dma->ch[j].isr_ret & CYG_ISR_CALL_DSR) ){
                dma->ch[j].dsr(i, 0, dma->ch[j].data);
            }
        }
        sr = sr >> 1; i++;
    }

    cyg_drv_interrupt_unmask (CYGNUM_HAL_INTERRUPT_DMA);
}


//-----------------------------------------------------------------------------
// DMA Initialization

void
hal_dma_init( void )
{
    cyg_uint8 i = 0;

    // Avoid multiple initialization
    if(a2fxxx_dma.init)
        return;

    a2fxxx_dma.init = 1;

    // Reset DMA
    CYGHWR_HAL_A2FXXX_PERIPH_RESET( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(PDMA) );

    // Clear channel ISR / DSR
    while( i < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL ) {
        a2fxxx_dma.ch[i].isr    = NULL;
        a2fxxx_dma.ch[i].dsr    = NULL;
        a2fxxx_dma.dma_cr[i]    = 0;
        a2fxxx_dma.ch[i++].data = 0;
    }
    a2fxxx_dma.dma_sr = 0;

    // Register DMA interrupt
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_DMA,
                           CYGNUM_HAL_CORTEXM_A2FXXX_DMA_ISR_PRIORITY,
                           (cyg_addrword_t)&a2fxxx_dma,
                           &a2fxxx_dma_isr,
                           &a2fxxx_dma_dsr,
                           &(a2fxxx_dma.interrupt_handle),
                           &(a2fxxx_dma.interrupt_data));
    cyg_drv_interrupt_attach(a2fxxx_dma.interrupt_handle);
    cyg_drv_interrupt_acknowledge (CYGNUM_HAL_INTERRUPT_DMA);
    cyg_drv_interrupt_unmask (CYGNUM_HAL_INTERRUPT_DMA);

    // Release DMA
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE( CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(PDMA) );

    // HW initialization
    i=0;
    while( i < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL ) {
        HAL_WRITE_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(i++),
                         CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_RESET);
    }
}


//-----------------------------------------------------------------------------
// Register the DMA sub-channel ISR / DSR

__externC cyg_uint32
a2fxxx_dma_ch_attach(cyg_uint8 ch, cyg_ISR_t *isr, cyg_DSR_t *dsr,
                     cyg_addrword_t data)
{
    cyg_uint32 res = 1;

    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                         "DMA : Channel number out of range (Attach).");

#ifdef CYGPKG_KERNEL
    cyg_interrupt_disable();
#endif
    if( a2fxxx_dma.ch[ch].isr != NULL ||
        a2fxxx_dma.ch[ch].dsr != NULL || a2fxxx_dma.ch[ch].data != 0 ) {
        res = 0;
    } else {
        a2fxxx_dma.ch[ch].isr  = isr;
        a2fxxx_dma.ch[ch].dsr  = dsr;
        a2fxxx_dma.ch[ch].data = data;
    }
#ifdef CYGPKG_KERNEL
    cyg_interrupt_enable();
#endif

    return res;
}


//-----------------------------------------------------------------------------
// Update DMA source / destination address increment

__externC void
a2fxxx_dma_update_incr(cyg_uint8 ch, cyg_bool dst, cyg_uint8 incr)
{
    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                          "DMA : Channel number out of range (Update).");

    if(dst==false){
        // Clear bits
        a2fxxx_dma.dma_cr[ch] &= ~CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR_4B;
        a2fxxx_dma.dma_cr[ch] |= CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_SRC_INCR(incr);
    }
    else {
        // Clear bits
        a2fxxx_dma.dma_cr[ch] &= ~CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR_4B;
        a2fxxx_dma.dma_cr[ch] |= CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DST_INCR(incr);
    }
}


//-----------------------------------------------------------------------------
// Setup DMA channel
//
// channel      select the DMA channel ID.
// type         select the transfer type to be performed. For valid
//              values, check CYGHWR_HAL_A2FXXX_DMA_XFER(_x) in var_io.h.
// outbound     set to true for transfer out of memory, false for transfer to
//              memory
// src_incr     select the memory address increment step for the source. Valid
//              values are 0, 1, 2 and 4 byte(s). 0 can be used for DMA
//              transfer from peripheral FIFO for instance.
// dst_incr     select the memory address increment step for the destination.
//              Valid values are 0, 1, 2 and 4 byte(s). 0 can be used for DMA
//              transfer to peripheral FIFO for instance.
// pri          select the DMA channel priority (true = high , false = low)
// wr_adj       indicates the number of FCLK periods which the PDMA must wait
//              after completion of a read or write access to a peripheral before
//              evaluating the out-of-band status signals from that peripheral
//              for another transfer

__externC cyg_uint32
a2fxxx_dma_ch_setup(cyg_uint8 ch, cyg_uint8 type, cyg_bool outbound,
      cyg_uint8 src_incr, cyg_uint8 dst_incr, cyg_bool pri, cyg_uint8 wr_adj)
{
    cyg_uint32 res = 1;
    cyg_uint32 xfer_type = 0, xfer_dir = 0, xfer_incr =
                                    CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_XFER_BYTE;
    a2fxxx_dma.dma_cr[ch] = 0;

    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                               "DMA : Channel number out of range (Setup).");

    DMA_TRACE("DMA setup channel %d, direction: %s, type %x, step %d-%d byte(s)\n", ch ,
             ((outbound==true) ? "outbound" : "inbound"), type, src_incr, dst_incr );

    if( type != CYGHWR_HAL_A2FXXX_DMA_XFER_MEMORY ){
        xfer_type = CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_PERIPH_SEL(type) |
                CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_PERIPH;
    }

    if( outbound == true ){
        xfer_dir = CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_DIR;
    }

    a2fxxx_dma_update_incr(ch, true, dst_incr);
    a2fxxx_dma_update_incr(ch, false, src_incr);

    a2fxxx_dma.dma_cr[ch] |= ( xfer_type | xfer_dir |
          ((pri == true) ? CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_HI_PRI : 0) |
          CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPA |
          CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPB |
          CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_WR_ADJ(wr_adj) |
          xfer_incr);

    HAL_WRITE_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(ch),
                                    a2fxxx_dma.dma_cr[ch]);

    return res;
}


//-----------------------------------------------------------------------------
// Remove DMA channel handler

__externC void
a2fxxx_dma_ch_detach (cyg_uint8 ch)
{
    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                              "DMA : Channel number out of range (Detach).");

#ifdef CYGPKG_KERNEL
    cyg_interrupt_disable();
#endif
    a2fxxx_dma.ch[ch].isr  = NULL;
    a2fxxx_dma.ch[ch].dsr  = NULL;
    a2fxxx_dma.ch[ch].data = 0;
#ifdef CYGPKG_KERNEL
    cyg_interrupt_enable();
#endif
}


//-----------------------------------------------------------------------------
// Start DMA transfer

__externC cyg_uint32
a2fxxx_dma_xfer (cyg_uint8 ch, cyg_bool polled, cyg_uint32 len, cyg_uint8 *src,
                     cyg_uint8 *dst)
{
    cyg_uint32 res = 1;
    cyg_uint32 src_reg, dst_reg, cnt_reg;
    cyg_uint32 sub = 0;
    cyg_haladdress dma_src = (cyg_haladdress) src;
    cyg_haladdress dma_dst = (cyg_haladdress) dst;

    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                           "DMA : Channel number out of range (Transfer).");

    if( polled == true )
        a2fxxx_dma.dma_cr[ch] &= ~CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_INTEN;
    else
        a2fxxx_dma.dma_cr[ch] |= CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_INTEN;

    HAL_WRITE_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(ch),
                                 a2fxxx_dma.dma_cr[ch]);

    HAL_READ_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS(ch), sub);
    sub = CYGHWR_HAL_A2FXXX_DMA_GET_SUB_ID( sub );

    src_reg = ((sub == 0) ? CYGHWR_HAL_A2FXXX_DMA_CHx_BA_SRC(ch) :
                            CYGHWR_HAL_A2FXXX_DMA_CHx_BB_SRC(ch));
    dst_reg = ((sub == 0) ? CYGHWR_HAL_A2FXXX_DMA_CHx_BA_DST(ch) :
                            CYGHWR_HAL_A2FXXX_DMA_CHx_BB_DST(ch));
    cnt_reg = ((sub == 0) ? CYGHWR_HAL_A2FXXX_DMA_CHx_BA_COUNT(ch) :
                            CYGHWR_HAL_A2FXXX_DMA_CHx_BB_COUNT(ch));

    DMA_TRACE("DMA transfer of length %d on channel %d(%s) - SRC: 0x%x / DST: 0x%x\n",
                          len, ch, ((sub==0) ? "A" : "B"), dma_src, dma_dst);
    DMA_TRACE("DMA register address 0x%x / 0x%x\n", src_reg , dst_reg);

    HAL_WRITE_UINT32(a2fxxx_dma.base + src_reg, dma_src);
    HAL_WRITE_UINT32(a2fxxx_dma.base + dst_reg, dma_dst);
    HAL_WRITE_UINT32(a2fxxx_dma.base + cnt_reg, len);

    return res;
}


//-----------------------------------------------------------------------------
// Clear DMA interrupt

void a2fxxx_dma_clear_interrupt (cyg_uint8 ch)
{
    cyg_uint32 reg;

    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                           "DMA : Channel number out of range (Clear IRQ).");

    DMA_TRACE("DMA status register 0x%x\n",
                     (a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(ch)));

    HAL_READ_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(ch), reg);
    reg |= (CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPA |
            CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL_CLR_COMPB);
    HAL_WRITE_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_CTRL(ch), reg);
}


//-----------------------------------------------------------------------------
// Clear DMA interrupt

cyg_uint8 a2fxxx_dma_get_comp_flag (cyg_uint8 ch)
{
    cyg_uint32 reg;

    CYG_ASSERT (ch < CYGHWR_HAL_A2FXXX_DMA_MAX_CHANNEL,
                                  "DMA : Channel number out of range (Get).");

    HAL_READ_UINT32(a2fxxx_dma.base + CYGHWR_HAL_A2FXXX_DMA_CHx_STATUS(ch), reg);

    return (reg&0x3);
}

//-----------------------------------------------------------------------------
// End of hal_dma.c
