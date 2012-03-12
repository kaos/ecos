//==========================================================================
//
//      i2c_a2fxxx.c
//
//      I2C driver for Smartfusion Cortex M3 microcontroller
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
// Description:   I2C driver for Smartfusion Cortex M3 microcontroller
//                The RX part of the driver has not been tested.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_a2fxxx.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

#ifdef CYGDBG_DEVS_I2C_CORTEXM_A2FXXX_TRACE
# define I2C_TRACE(args...) diag_printf(args)
#else
# define I2C_TRACE(args...)
#endif

#define I2C_DAT(_base_)       (_base_ + CYGHWR_HAL_A2FXXX_I2C_DATA)
#define I2C_ADR(_base_)       (_base_ + CYGHWR_HAL_A2FXXX_I2C_ADDR)
#define I2C_SR(_base_)        (_base_ + CYGHWR_HAL_A2FXXX_I2C_STATUS)
#define I2C_CR(_base_)        (_base_ + CYGHWR_HAL_A2FXXX_I2C_CTRL)
#define I2C_FREQ(_base_)      (_base_ + CYGHWR_HAL_A2FXXX_I2C_FREQ)
#define I2C_SMBUS(_base_)     (_base_ + CYGHWR_HAL_A2FXXX_I2C_SMBUS)
#define I2C_GLITCH(_base_)    (_base_ + CYGHWR_HAL_A2FXXX_I2C_GLITCH)

// Bit-band definition
#define I2C_CR_CR0(_base_)    (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 0*sizeof(cyg_uint32) ) )
#define I2C_CR_CR1(_base_)    (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 1*sizeof(cyg_uint32) ) )
#define I2C_CR_AA(_base_)     (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 2*sizeof(cyg_uint32) ) )
#define I2C_CR_SI(_base_)     (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 3*sizeof(cyg_uint32) ) )
#define I2C_CR_STO(_base_)    (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 4*sizeof(cyg_uint32) ) )
#define I2C_CR_STA(_base_)    (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 5*sizeof(cyg_uint32) ) )
#define I2C_CR_ENS1(_base_)   (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 6*sizeof(cyg_uint32) ) )
#define I2C_CR_CR2(_base_)    (_base_ + ( CYGHWR_HAL_A2FXXX_I2C_CTRL << 5 ) +  \
                               ( 7*sizeof(cyg_uint32) ) )

#define I2C_MTX_STATE(x)      CYGHWR_HAL_A2FXXX_I2C_STATUS_MTX_##x
#define I2C_MRX_STATE(x)      CYGHWR_HAL_A2FXXX_I2C_STATUS_MRX_##x

// Divider coefficient
static const unsigned int i2c_div[] = {
    256,
    224,
    192,
    160,
    960,
    120,
    60,
    8,
    0
};

static cyg_uint32
a2fxxx_i2c_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) data;
    cyg_uint32      result = CYG_ISR_HANDLED;
    cyg_uint32      sr,
                    dr;
    cyg_uint8       tx_data = *extra->i2c_data.i2c_tx_data;
    cyg_uint32      cr = 0;

    // Read the current status of the I2C
    HAL_READ_UINT32(I2C_SR(extra->i2c_base), sr);
    HAL_READ_UINT32(I2C_CR(extra->i2c_base), cr);

    // What to do next depends on the current transfer mode.
    if (A2FXXX_I2C_XFER_MODE_TX == extra->i2c_mode) {
        I2C_TRACE("I2C: TX IRQ handling -> 0x%x\n", sr);
        switch (sr) {
        // Start/Repeated start, write the slave address and clear the
        // START and SI bits
        case I2C_MTX_STATE(START):
        case I2C_MTX_STATE(REPEAT_START):
            HAL_WRITE_UINT32(I2C_DAT(extra->i2c_base), extra->slave_addr);
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_STA;
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            break;
        // ACK is received for the address or data, keep sending
        // START and SI bits
        case I2C_MTX_STATE(ADDR_ACK):
        case I2C_MTX_STATE(DATA_ACK):
            if (0 == extra->i2c_count) {
                if (extra->send_stop) {
                    cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_STO;
                    cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
                } else {
                    // For repeated start, we must disable interrupt
                    extra->i2c_isr_mask = true;
                }
                // No more bytes to send.
                result |= CYG_ISR_CALL_DSR;
            } else {
                // Send byte
                HAL_WRITE_UINT32(I2C_DAT(extra->i2c_base),
                                 (cyg_uint32)tx_data);
                extra->i2c_data.i2c_tx_data += 1;
                extra->i2c_count -= 1;
                cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            }
            break;
        case I2C_MTX_STATE(ARBLOST):
        case I2C_MTX_STATE(DATA_NACK):
            // Lost the bus, abort the transfer. count has already been
            // decremented. Assume the byte did not actually arrive.
            extra->i2c_count += 1;
        case I2C_MTX_STATE(ADDR_NACK):
            // For all error type, clear interrupt and send stop bit
            cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_STO;
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            extra->i2c_got_nack = 1;
            result |= CYG_ISR_CALL_DSR;
            break;
        default:
            HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);
            return result;
        }
    } else if (A2FXXX_I2C_XFER_MODE_RX == extra->i2c_mode) {
        I2C_TRACE("I2C: RX IRQ handling -> 0x%x\n", sr);
        cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_AA;
        switch (sr) {
        // Start/Repeated start, write the slave address and clear the
        // START and SI bits
        case I2C_MRX_STATE(START):
        case I2C_MRX_STATE(REPEAT_START):
            HAL_WRITE_UINT32(I2C_DAT(extra->i2c_base), extra->slave_addr);
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_STA;
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            break;
        // ACK is received for the address
        case I2C_MRX_STATE(ADDR_ACK):
            if (extra->i2c_count > 1) {
                cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_AA;
            }
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            break;
        // Receive data
        case I2C_MRX_STATE(DATA_ACK):
        case I2C_MRX_STATE(DATA_NACK):
            cr &= ~CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
            if (extra->i2c_count > 2) {
                cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_AA;
            }
            if (1 == extra->i2c_count) {
                if (extra->send_stop) {
                    cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_STO;
                } else {
                    // For repeated start, we must disable interrupt
                    cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_SI;
                    extra->i2c_isr_mask = true;
                }
                // Received the last byte.
                result |= CYG_ISR_CALL_DSR;
            }
            HAL_READ_UINT32(I2C_DAT(extra->i2c_base), dr);
            *(extra->i2c_data.i2c_rx_data) = (cyg_uint8)dr;
            extra->i2c_data.i2c_rx_data += 1;
            extra->i2c_count -= 1;
            break;
        case I2C_MRX_STATE(ARBLOST):
        case I2C_MRX_STATE(ADDR_NACK):
            // Lost the bus? Maybe a spurious stop
            cr |= CYGHWR_HAL_A2FXXX_I2C_CTRL_STO;
            result |= CYG_ISR_CALL_DSR;
            break;
        default:
            HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);
            return result;
        }
    } else {
        // Invalid state? Some kind of spurious interrupt?
        // Just ignore it.
        I2C_TRACE("I2C spurious interrupt\n");
    }

    HAL_WRITE_UINT32(I2C_CR(extra->i2c_base), cr);

    HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);

    if (extra->i2c_isr_mask)
        HAL_INTERRUPT_MASK(extra->i2c_isr_id);

    return result;
}


static void
a2fxxx_i2c_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) data;
    extra->i2c_completed = 1;
    cyg_drv_cond_signal(&(extra->i2c_wait));
}


// A transfer has been started. Wait for completion
static inline void
a2fxxx_i2c_doit(a2fxxx_i2c_extra * extra)
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
a2fxxx_i2c_stopit(a2fxxx_i2c_extra * extra)
{
    extra->i2c_lost_arb = 0;
    extra->i2c_owner = 0;
    extra->i2c_mode = A2FXXX_I2C_XFER_MODE_INVALID;
}


void
a2fxxx_i2c_stop(const cyg_i2c_device * dev)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) dev->i2c_bus->i2c_extra;
    a2fxxx_i2c_stopit(extra);
}


cyg_uint32
a2fxxx_i2c_tx(const cyg_i2c_device * dev, cyg_bool send_start,
              const cyg_uint8 *tx_data, cyg_uint32 count, cyg_bool send_stop)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) dev->i2c_bus->i2c_extra;
    extra->send_stop = send_stop;
    extra->send_start = send_start;
    extra->i2c_count = count;
    extra->slave_addr = ((dev->i2c_address << 1) | 0x00);
    cyg_uint32      si = 0;

    if (!extra->i2c_lost_arb) {
        extra->i2c_completed = 0;
        extra->i2c_mode = A2FXXX_I2C_XFER_MODE_TX;

        if (send_start || !extra->i2c_got_nack) {
            I2C_TRACE
                ("I2C: TX to %2x, data %2x, count: %4d, START flag: %s, STOP flag: %s\n",
                 dev->i2c_address, *tx_data, count,
                 (send_start == true) ? "true" : "false",
                 (send_stop == true) ? "true" : "false");
            extra->i2c_data.i2c_tx_data = tx_data;
            HAL_READ_UINT32(I2C_CR_SI(extra->i2c_base_bb), si);
            // Set start bit
            HAL_WRITE_UINT32(I2C_CR_STA(extra->i2c_base_bb),
                             CYGHWR_HAL_A2FXXX_BITSET);
            // For repeated start, we need to clear the interrupt
            if (si != 0) {
                HAL_WRITE_UINT32(I2C_CR_SI(extra->i2c_base_bb),
                                 CYGHWR_HAL_A2FXXX_BITCLEAR);
                HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);
            }
            // For repeated start, we need to un-mask the interrupt
            if (extra->i2c_isr_mask) {
                extra->i2c_isr_mask = false;
                HAL_INTERRUPT_UNMASK(extra->i2c_isr_id);
            }
            a2fxxx_i2c_doit(extra);
        }

    }
    if (send_stop) {
        I2C_TRACE("I2C: TX send stop\n");
        a2fxxx_i2c_stopit(extra);
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
a2fxxx_i2c_rx(const cyg_i2c_device * dev, cyg_bool send_start,
              cyg_uint8 *rx_data, cyg_uint32 count, cyg_bool send_nack,
              cyg_bool send_stop)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) dev->i2c_bus->i2c_extra;
    extra->i2c_send_nack = send_nack;
    extra->i2c_count = count;
    extra->send_stop = send_stop;
    extra->send_start = send_start;
    extra->slave_addr = ((dev->i2c_address << 1) | 0x01);
    cyg_uint32      si = 0;

    if (!extra->i2c_lost_arb) {
        extra->i2c_completed = 0;
        extra->i2c_data.i2c_rx_data = rx_data;
        extra->i2c_mode = A2FXXX_I2C_XFER_MODE_RX;
        I2C_TRACE
            ("I2C: RX to %2x, count: %4d, START flag: %s, STOP flag: %s\n",
             dev->i2c_address, count, (send_start == true) ? "true" : "false",
             (send_stop == true) ? "true" : "false");
        HAL_READ_UINT32(I2C_CR_SI(extra->i2c_base_bb), si);
        // Set start bit
        HAL_WRITE_UINT32(I2C_CR_STA(extra->i2c_base_bb),
                         CYGHWR_HAL_A2FXXX_BITSET);
        // For repeated start, we need to clear the interrupt
        if (si != 0) {
            HAL_WRITE_UINT32(I2C_CR_SI(extra->i2c_base_bb),
                             CYGHWR_HAL_A2FXXX_BITCLEAR);
            HAL_INTERRUPT_ACKNOWLEDGE(extra->i2c_isr_id);
        }
        // For repeated start, we need to un-mask the interrupt
        if (extra->i2c_isr_mask) {
            extra->i2c_isr_mask = false;
            HAL_INTERRUPT_UNMASK(extra->i2c_isr_id);
        }
        a2fxxx_i2c_doit(extra);
    }

    if (send_stop) {
        I2C_TRACE("I2C: RX send stop\n");
        a2fxxx_i2c_stopit(extra);
    }

    return count - extra->i2c_count;
}


// ----------------------------------------------------------------------------
// The functions needed for all I2C devices.

void
a2fxxx_i2c_init(struct cyg_i2c_bus *bus)
{
    a2fxxx_i2c_extra *extra = (a2fxxx_i2c_extra *) bus->i2c_extra;
    cyg_uint8       div = 0;
#ifdef CYGDBG_DEVS_I2C_CORTEXM_A2FXXX_TRACE
    cyg_uint32      i2c_freq =
        ((hal_a2fxxx_i2c_clock(extra->i2c_base) /
          CYGNUM_HAL_CORTEXM_A2FXXX_I2C_CLK_DIV));
#endif

    while ((i2c_div[div] != CYGNUM_HAL_CORTEXM_A2FXXX_I2C_CLK_DIV) &&
           i2c_div[div] != 0) {
        div++;
    }

    I2C_TRACE("\nI2C INIT, divider: %d(%d), frequency: %d\n", i2c_div[div],
              div, i2c_freq);

    cyg_drv_mutex_init(&extra->i2c_lock);
    cyg_drv_cond_init(&extra->i2c_wait, &extra->i2c_lock);
    cyg_drv_interrupt_create(extra->i2c_isr_id,
                             extra->i2c_isr_pri,
                             (cyg_addrword_t)extra,
                             &a2fxxx_i2c_isr,
                             &a2fxxx_i2c_dsr,
                             &(extra->i2c_interrupt_handle),
                             &(extra->i2c_interrupt_data));
    cyg_drv_interrupt_attach(extra->i2c_interrupt_handle);

    // Enable I2C peripheral
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE(extra->i2c_periph);

    // Set I2C bus speed
    HAL_WRITE_UINT32(I2C_CR_ENS1(extra->i2c_base_bb),
                     CYGHWR_HAL_A2FXXX_BITSET);

    HAL_WRITE_UINT32(I2C_CR_CR0(extra->i2c_base_bb),
                     ((div & 0x1) ? CYGHWR_HAL_A2FXXX_BITSET :
                      CYGHWR_HAL_A2FXXX_BITCLEAR));
    HAL_WRITE_UINT32(I2C_CR_CR1(extra->i2c_base_bb),
                     ((div & 0x2) ? CYGHWR_HAL_A2FXXX_BITSET :
                      CYGHWR_HAL_A2FXXX_BITCLEAR));
    HAL_WRITE_UINT32(I2C_CR_CR2(extra->i2c_base_bb),
                     ((div & 0x4) ? CYGHWR_HAL_A2FXXX_BITSET :
                      CYGHWR_HAL_A2FXXX_BITCLEAR));

    // Interrupts can now be safely unmasked
    HAL_INTERRUPT_UNMASK(extra->i2c_isr_id);
}

//---------------------------------------------------------------------------
// EOF i2c_a2fxxx.c
