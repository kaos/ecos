//==========================================================================
//
//      i2c_lm3s.c
//
//      I2C driver for Stellaris Cortex M3 microcontroller
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
// Author(s):     ccoutand
// Contributors:  
// Date:          2011-01-18
// Original:      Uwe Kindler, Bart Veer
//                I2C driver for motorola coldfire processor
// Description:   I2C driver for Stellaris Cortex M3 microcontroller
//                The RX part of the driver has not been tested.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_lm3s.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#ifdef CYGPKG_DEVS_I2C_CORTEXM_LM3S_TRACE
# define I2C_TRACE(args...) diag_printf(args)
#else
# define I2C_TRACE(args...)
#endif

#define I2C_DAT(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MDR)
#define I2C_ADR(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MSA)
#define I2C_SR(_extra_)      (_extra_ + CYGHWR_HAL_LM3S_I2C_MCS)
#define I2C_CR(_extra_)      (_extra_ + CYGHWR_HAL_LM3S_I2C_MCS)
#define I2C_MCR(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MCR)
#define I2C_MTPR(_extra_)    (_extra_ + CYGHWR_HAL_LM3S_I2C_MTPR)
#define I2C_MCR(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MCR)
#define I2C_IMR(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MIMR)
#define I2C_ICR(_extra_)     (_extra_ + CYGHWR_HAL_LM3S_I2C_MICR)

#define WAIT_BUS_READY( __sr__, __extra__ )                     \
{                                                               \
    if (!__extra__->i2c_owner) {                                \
        __extra__->i2c_got_nack = 0;                            \
        __extra__->i2c_owner = 1;                               \
        HAL_READ_UINT32(I2C_SR(__extra__->i2c_base), __sr__);   \
        while (__sr__ & CYGHWR_HAL_LM3S_I2C_MCS_BUSBSY) {       \
          HAL_READ_UINT32(I2C_SR(__extra__->i2c_base),__sr__);  \
        }                                                       \
    }                                                           \
}


static cyg_uint32
lm3s_i2c_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) data;
    cyg_uint32      sr,
                    dr;
    cyg_uint32      result = CYG_ISR_HANDLED;
    cyg_uint32      reg = CYGHWR_HAL_LM3S_I2C_MCS_RUN;
    cyg_uint8       tx_data = *extra->i2c_data.i2c_tx_data;

    // Read the current status, then clear the interrupt
    HAL_READ_UINT32(I2C_SR(extra->i2c_base), sr);
    HAL_WRITE_UINT32(I2C_ICR(extra->i2c_base), 1);

    // What to do next depends on the current transfer mode.
    if (LM3S_I2C_XFER_MODE_TX == extra->i2c_mode) {
        I2C_TRACE("I2C: TX IRQ handling\n");
        if (sr & CYGHWR_HAL_LM3S_I2C_MCS_ERR) {
            // Lost the bus, abort the transfer. count has already been
            // decremented. Assume the byte did not actually arrive.
            extra->i2c_count += 1;
            // Arbitration  lost, stop
            if (sr & CYGHWR_HAL_LM3S_I2C_MCS_ARBLST) {
                reg = CYGHWR_HAL_LM3S_I2C_MCS_STOP;
                HAL_WRITE_UINT32(I2C_CR(extra->i2c_base), reg);
            }
            // This byte has been sent but the device cannot accept
            // any more. The nack must be remembered. Otherwise if
            // we got a nack for the last byte in a tx then the
            // calling code will think the entire tx succeeded,
            // and there will be problems if the next call is
            // another tx without a repeated start.
            if ((sr & CYGHWR_HAL_LM3S_I2C_MCS_ADRACK) |
                (sr & CYGHWR_HAL_LM3S_I2C_MCS_DATACK)) {
                extra->i2c_got_nack = 1;
            }
            result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
            I2C_TRACE("I2C TX, bus arbitration error\n");
        } else if (0 == extra->i2c_count) {
            // No more bytes to send.
            result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else {
            // Send byte
            HAL_WRITE_UINT32(I2C_DAT(extra->i2c_base), (cyg_uint32)tx_data);
            extra->i2c_data.i2c_tx_data += 1;
            extra->i2c_count -= 1;
            // Last byte
            if ((0 == extra->i2c_count) && extra->send_stop) {
                reg |= CYGHWR_HAL_LM3S_I2C_MCS_STOP;
            }
            HAL_WRITE_UINT32(I2C_CR(extra->i2c_base), reg);
        }
    } else if (LM3S_I2C_XFER_MODE_RX == extra->i2c_mode) {
        I2C_TRACE("I2C: RX IRQ handling\n");
        if (sr & CYGHWR_HAL_LM3S_I2C_MCS_ERR) {
            // Lost the bus? Maybe a spurious stop
            result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else {
            if (2 == extra->i2c_count) {
                // Received one, one more to go,
                // and that one should be nacked.
                if (!extra->i2c_send_nack) {
                    reg |= CYGHWR_HAL_LM3S_I2C_MCS_ACK;
                }
                if (extra->send_stop) {
                    reg |= CYGHWR_HAL_LM3S_I2C_MCS_STOP;
                }
                HAL_WRITE_UINT32(I2C_CR(extra->i2c_base), reg);
            } else if (1 == extra->i2c_count) {
                // Received the last byte.
                result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
            }
            HAL_READ_UINT32(I2C_DAT(extra->i2c_base), dr);
            *(extra->i2c_data.i2c_rx_data) = (cyg_uint8)dr;
            extra->i2c_data.i2c_rx_data += 1;
            extra->i2c_count -= 1;
        }
    } else {
        // Invalid state? Some kind of spurious interrupt?
        // Just ignore it.
        I2C_TRACE("I2C spurious interrupt\n");
    }

    HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);

    return result;
}


static void
lm3s_i2c_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) data;
    extra->i2c_completed = 1;
    cyg_drv_cond_signal(&(extra->i2c_wait));
}


// A transfer has been started. Wait for completion
static inline void
lm3s_i2c_doit(lm3s_i2c_extra * extra)
{
    cyg_drv_mutex_lock(&(extra->i2c_lock));
    cyg_drv_dsr_lock();
    while (!extra->i2c_completed) {
        cyg_drv_cond_wait(&(extra->i2c_wait));
    }
    cyg_drv_dsr_unlock();
    cyg_drv_mutex_unlock(&(extra->i2c_lock));
}


static inline void
lm3s_i2c_stopit(lm3s_i2c_extra * extra)
{
    extra->i2c_lost_arb = 0;
    extra->i2c_owner = 0;
    extra->i2c_mode = LM3S_I2C_XFER_MODE_INVALID;
}


void
lm3s_i2c_stop(const cyg_i2c_device * dev)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) dev->i2c_bus->i2c_extra;
    lm3s_i2c_stopit(extra);
}


static cyg_bool
lm3s_i2c_handle_xfer(lm3s_i2c_extra * extra, int address)
{
    cyg_uint32      sr;
    cyg_uint8       data = *extra->i2c_data.i2c_tx_data;
    cyg_uint32      reg = CYGHWR_HAL_LM3S_I2C_MCS_RUN;

    // Nothing to send or receive
    if (extra->i2c_count == 0)
        return 0;

    // Take the bus ownership
    WAIT_BUS_READY(sr, extra);

    // This can be a start or repeated start
    if (extra->send_start) {
        reg |= CYGHWR_HAL_LM3S_I2C_MCS_START;
        HAL_WRITE_UINT32(I2C_ADR(extra->i2c_base), address);
    }
    // TX transfer
    if (extra->i2c_mode == LM3S_I2C_XFER_MODE_TX) {
        extra->i2c_data.i2c_tx_data += 1;
        extra->i2c_count -= 1;
        HAL_WRITE_UINT32(I2C_DAT(extra->i2c_base), (cyg_uint32)data);

        // Single byte transfer
        if (extra->send_stop && (extra->i2c_count == 0)) {
            reg |= CYGHWR_HAL_LM3S_I2C_MCS_STOP;
        }
    } else {
        // Single byte transfer, set the STOP bit
        if (extra->send_stop && (extra->i2c_count == 1)) {
            reg |= CYGHWR_HAL_LM3S_I2C_MCS_STOP;
        }
        // Do not ACK last byte per user request
        if (!extra->i2c_send_nack || !(extra->i2c_count == 1)) {
            reg |= CYGHWR_HAL_LM3S_I2C_MCS_ACK;
        }
    }

    HAL_WRITE_UINT32(I2C_CR(extra->i2c_base), reg);

    return 1;
}


cyg_uint32
lm3s_i2c_tx(const cyg_i2c_device * dev, cyg_bool send_start,
            const cyg_uint8 *tx_data, cyg_uint32 count, cyg_bool send_stop)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) dev->i2c_bus->i2c_extra;
    extra->send_stop = send_stop;
    extra->send_start = send_start;
    extra->i2c_count = count;

    if (!extra->i2c_lost_arb) {
        extra->i2c_completed = 0;
        extra->i2c_mode = LM3S_I2C_XFER_MODE_TX;

        if (send_start || !extra->i2c_got_nack) {
            I2C_TRACE
                ("I2C: TX to %2x, data %2x, count: %4d, START flag: %s\n",
                 dev->i2c_address, *tx_data, count,
                 (send_start == true) ? "true" : "false");
            extra->i2c_data.i2c_tx_data = tx_data;
            if (!lm3s_i2c_handle_xfer(extra, (dev->i2c_address << 1) | 0x00)) {
                return 0;
            }
            lm3s_i2c_doit(extra);
        }

    }
    if (send_stop) {
        I2C_TRACE("I2C: TX send stop\n");
        lm3s_i2c_stopit(extra);
    }

    I2C_TRACE("I2C: TX count %d\n", extra->i2c_count);

    // tx() should return the number of bytes actually transmitted.
    // ISR() increments extra->count after a failure, which leads to
    // an edge condition when send_start and there is no acknowledgment
    // of the address byte.
    if (extra->i2c_count > count) {
        return 0;
    }

    return count - extra->i2c_count;
}


cyg_uint32
lm3s_i2c_rx(const cyg_i2c_device * dev, cyg_bool send_start,
            cyg_uint8 *rx_data, cyg_uint32 count, cyg_bool send_nack,
            cyg_bool send_stop)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) dev->i2c_bus->i2c_extra;
    extra->i2c_count = count;
    extra->i2c_send_nack = send_nack;
    extra->send_stop = send_stop;
    extra->send_start = send_start;

    if (!extra->i2c_lost_arb) {
        extra->i2c_completed = 0;
        extra->i2c_data.i2c_rx_data = rx_data;
        extra->i2c_mode = LM3S_I2C_XFER_MODE_RX;
        I2C_TRACE("I2C: RX to %2x, count: %4d, START flag: %s\n",
                  dev->i2c_address, count,
                  (send_start == true) ? "true" : "false");
        if (!lm3s_i2c_handle_xfer(extra, (dev->i2c_address << 1) | 0x01)) {
            return 0;
        }
        lm3s_i2c_doit(extra);
    }

    if (send_stop) {
        I2C_TRACE("I2C: RX send stop\n");
        lm3s_i2c_stopit(extra);
    }

    return count - extra->i2c_count;
}


// ----------------------------------------------------------------------------
// The functions needed for all I2C devices.

void
lm3s_i2c_init(struct cyg_i2c_bus *bus)
{
    lm3s_i2c_extra *extra = (lm3s_i2c_extra *) bus->i2c_extra;
    cyg_uint32      tpr =
        ((hal_lm3s_i2c_clock() /
          (20 * CYGNUM_HAL_CORTEXM_LM3S_I2C_CLK_SPEED)) - 1);

    I2C_TRACE("I2C INIT, TPR register: %d\n", tpr);

    cyg_drv_mutex_init(&extra->i2c_lock);
    cyg_drv_cond_init(&extra->i2c_wait, &extra->i2c_lock);
    cyg_drv_interrupt_create(extra->i2c_isr_id,
                             extra->i2c_isr_pri,
                             (cyg_addrword_t)extra,
                             &lm3s_i2c_isr,
                             &lm3s_i2c_dsr,
                             &(extra->i2c_interrupt_handle),
                             &(extra->i2c_interrupt_data));
    cyg_drv_interrupt_attach(extra->i2c_interrupt_handle);

    // Enable I2C peripheral, it is left to the variant/platform HAL to
    // configure the SDA and SCL IOs.
    CYGHWR_HAL_LM3S_PERIPH_SET(extra->i2c_periph, 1);

    // Enable Master mode
    HAL_WRITE_UINT32(I2C_MCR(extra->i2c_base), CYGHWR_HAL_LM3S_I2C_MCR_MFE);

    // Set I2C bus speed
    HAL_WRITE_UINT32(I2C_MTPR(extra->i2c_base), tpr);

    // Enable Interrupt
    HAL_WRITE_UINT32(I2C_IMR(extra->i2c_base), 1);

    // Interrupts can now be safely unmasked
    HAL_INTERRUPT_UNMASK(extra->i2c_isr_id);
}

//---------------------------------------------------------------------------
// EOF i2c_lm3s.c
