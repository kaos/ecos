//==========================================================================
//
//      devs/i2c/m68k/mcf52xx/current/src/i2c_mcf52xx.c
//
//      I2C driver for Motorola coldfire processors
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2005, 2006 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Uwe Kindler, Bart Veer
// Contributors:  
// Date:          2005-10-23
// Description:   I2C driver for motorola coldfire processor
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_i2c_mcf52xx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_mcf52xx.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>

// Optimize for the case of a single bus device, while still allowing
// multiple devices.
#ifndef CYGHWR_DEVS_I2C_MCF52xx_MULTIPLE_BUSES
# define    I2C_BASE(_extra_)       (cyg_uint8*)HAL_MCF52xx_I2C_SINGLETON_BASE
# define    I2C_ISRVEC(_extra_)     HAL_MCF52xx_I2C_SINGLETON_ISRVEC
# define    I2C_ISRPRI(_extra_)     HAL_MCF52xx_I2C_SINGLETON_ISRPRI
# define    I2C_FDR(_extra_)        HAL_MCF52xx_I2C_SINGLETON_FDR
#else
# define    I2C_BASE(_extra_)       ((_extra_)->i2c_base)
# define    I2C_ISRVEC(_extra_)     ((_extra_)->i2c_isrvec)
# define    I2C_ISRPRI(_extra_)     ((_extra_)->i2c_isrpri)
# define    I2C_FDR(_extra_)        ((_extra_)->i2c_fdr)
#endif

// If building for a singleton but the macros are no defined, assume
// the I2C support is conditional on a disabled platform HAL
// configuration option. This handles the common case of an I2C bus
// accessed only via an expansion connector.
#if defined(CYGHWR_DEVS_I2C_MCF52xx_MULTIPLE_BUSES) || defined(HAL_MCF52xx_I2C_SINGLETON_BASE)

// ----------------------------------------------------------------------------
// Interrupt handling and polling
//
// The MCF52xx I2C bus device does not have a fifo or any kind of DMA
// capability, so can generate interrupts at a very high rate: ~10K
// interrupts per second if the bus is running at the standard 100KHz,
// or 50K for a high-speed 400KHz bus. To keep the cpu load down to
// something vaguely reasonable as much work as possible has to be
// done in the ISR, with the DSR used only for completion.
static cyg_uint32
mcf52xx_i2c_isr(cyg_vector_t vec, cyg_addrword_t data)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)data;
    cyg_uint8               sr, dr;
    cyg_uint8*              base    = I2C_BASE(extra);
    cyg_uint32              result  = CYG_ISR_HANDLED;

    // Read the current status, then clear the interrupt and
    // arbitration-lost flags. No later code will look at the
    // SR register again.
    HAL_READ_UINT8( base + HAL_MCF52xx_I2C_SR_OFF, sr);
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_SR_OFF, 0x00);

    // What to do next depends on the current transfer mode.
    if (CYG_MCF52xx_I2C_XFER_MODE_TX == extra->i2c_mode) {
        // We are in a transmit, or sending the address byte just
        // before a transmit.
        if (sr & HAL_MCF52xx_I2C_SR_IAL) {
            // Lost the bus, abort the transfer. count has already been
            // decremented. Assume the byte did not actually arrive.
            extra->i2c_count    += 1;
            result              = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else if (sr & HAL_MCF52xx_I2C_SR_RXAK) {
            // This byte has been sent but the device cannot accept
            // any more. The nack must be remembered. Otherwise if
            // we got a nack for the last byte in a tx then the
            // calling code will think the entire tx succeeded,
            // and there will be problems if the next call is
            // another tx without a repeated start.
            extra->i2c_got_nack = 1;
            result              = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else if (0 == extra->i2c_count) {
            // No more bytes to send.
            result          = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else {
            HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_DR_OFF, *(extra->i2c_data.i2c_tx_data));
            extra->i2c_data.i2c_tx_data += 1;
            extra->i2c_count            -= 1;
        }
    } else if (CYG_MCF52xx_I2C_XFER_MODE_RX == extra->i2c_mode) {
        if (sr & HAL_MCF52xx_I2C_SR_IAL) {
            // Lost the bus? Maybe a spurious stop
            result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else {
            if (extra->i2c_send_nack && (2 == extra->i2c_count)) {
                // Received one, one more to go, and that one should be nacked.
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA |
                                HAL_MCF52xx_I2C_CR_TXAK);
            } else if (1 == extra->i2c_count) {
                // Received the last byte. The docs say to send a stop,
                // but there may be another transaction_rx() call. We
                // cannot just read DR again, that would trigger another
                // read. So instead switch to transmit mode for now,
                // which should cause the h/w to wait until a byte is
                // written to DR.
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA |
                                HAL_MCF52xx_I2C_CR_MTX);
                result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
            }

            HAL_READ_UINT8(base + HAL_MCF52xx_I2C_DR_OFF, dr);
            *(extra->i2c_data.i2c_rx_data)  = dr;
            extra->i2c_data.i2c_rx_data     += 1;
            extra->i2c_count                -= 1;
        }
    } else if (CYG_MCF52xx_I2C_XFER_MODE_STARTRX == extra->i2c_mode) {
        // Start followed by RX. The address byte has been sent, we
        // need to switch to receiving.
        if (sr & HAL_MCF52xx_I2C_SR_IAL) {
            // Looks like no device acknowledged the address.
            result = CYG_ISR_HANDLED | CYG_ISR_CALL_DSR;
        } else {
            extra->i2c_mode = CYG_MCF52xx_I2C_XFER_MODE_RX;
            if (extra->i2c_send_nack && (1 == extra->i2c_count)) {
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA |
                                HAL_MCF52xx_I2C_CR_TXAK);
            } else {
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA);
            }
            // This dummy read causes the next rx to start
            HAL_READ_UINT8(base + HAL_MCF52xx_I2C_DR_OFF, dr);
        }
    } else {
        // Invalid state? Some kind of spurious interrupt? Just ignore
        // it.
        CYG_FAIL("I2C spurious interrupt");
    }

    // NOTE: this will acknowledge the interrupt even in polled mode.
    // Probably harmless. Using I2C_ISRVEC rather than the vec arg
    // means a constant number for the singleton case, which may
    // allow the HAL to optimize the acknowledge away completely.
    HAL_INTERRUPT_ACKNOWLEDGE(I2C_ISRVEC(extra));
    return result;
}

static void
mcf52xx_i2c_dsr(cyg_vector_t vec, cyg_ucount32 count, cyg_addrword_t data)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)data;
    extra->i2c_completed    = 1;
    cyg_drv_cond_signal(&(extra->i2c_wait));
}

// A transfer has been started. Wait for completion, allowing for both
// polled and interrupt-driven mode.
static inline void
mcf52xx_i2c_doit(cyg_mcf52xx_i2c_extra* extra)
{
    cyg_uint8*  base    = I2C_BASE(extra);
    int         ints_state;
    int         sr;
    
    HAL_QUERY_INTERRUPTS(ints_state);
    if (((ints_state >> 8) & 0x07) > CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL) {
        // Interrupts are currently disabled. We'll have to poll.
        for ( ; ; ) {
            HAL_READ_UINT8(base + HAL_MCF52xx_I2C_SR_OFF, sr);
            if (sr & HAL_MCF52xx_I2C_SR_IIF) {
                if (CYG_ISR_CALL_DSR & mcf52xx_i2c_isr(I2C_ISRVEC(extra), (cyg_addrword_t)extra)) {
                    break;
                }
            }
        }
    } else {
        cyg_drv_mutex_lock(&(extra->i2c_lock));
        cyg_drv_dsr_lock();
        while (! extra->i2c_completed) {
            cyg_drv_cond_wait(&(extra->i2c_wait));
        }
        cyg_drv_dsr_unlock();
        cyg_drv_mutex_unlock(&(extra->i2c_lock));
    }
}

static cyg_bool
mcf52xx_i2c_send_start(cyg_mcf52xx_i2c_extra* extra, int address)
{
    cyg_uint8*  base    = I2C_BASE(extra);
    cyg_uint8   sr;
    
    // This may be a repeated start or the beginning of a transaction.
    // If the former then we still own the bus.
    if (!extra->i2c_owner) {
        // The bus is currently in slave mode. See if another master
        // currently owns the bus and if so fail immediately. It is up
        // to higher level code to decide when to retry. Alternatively
        // if the bus has somehow got stuck in busy mode it is again
        // up to higher level code to sort things out.
        HAL_READ_UINT8(I2C_BASE(extra) + HAL_MCF52xx_I2C_SR_OFF, sr);
        if (sr & HAL_MCF52xx_I2C_SR_IBB) {
            return 0;
        }

        // Now we can put the bus into master mode
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                        HAL_MCF52xx_I2C_CR_IEN   |
                        HAL_MCF52xx_I2C_CR_IIEN  |
                        HAL_MCF52xx_I2C_CR_MSTA  |  // This implicitly generates the start
                        HAL_MCF52xx_I2C_CR_MTX);    // The address byte needs to be transmitted.
        extra->i2c_owner    = 1;
    } else {
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                        HAL_MCF52xx_I2C_CR_IEN   |
                        HAL_MCF52xx_I2C_CR_IIEN  |
                        HAL_MCF52xx_I2C_CR_MSTA  |  // Already set so no start generated by this
                        HAL_MCF52xx_I2C_CR_MTX   |
                        HAL_MCF52xx_I2C_CR_RSTA);    // Repeated start
    }

    // Any previous nack is no longer relevant. If the device cannot accept
    // more data it will nack the address.
    extra->i2c_got_nack = 0;
    // Now send the address. The rest of the transfer is handled by the
    // interrupt/polling code.
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_DR_OFF, address);
    return 1;
}

static inline void
mcf52xx_i2c_stopit(cyg_mcf52xx_i2c_extra* extra)
{
    // If we still own the bus this releases it (by clearing MSTA) and
    // generating a stop. If we have lost arbitration then this write
    // has no effect (other than disabling interrupts). Either way the
    // bus should end up in a consistent state.
    HAL_WRITE_UINT8(I2C_BASE(extra) + HAL_MCF52xx_I2C_CR_OFF, HAL_MCF52xx_I2C_CR_IEN);
    extra->i2c_lost_arb = 0;
    extra->i2c_owner    = 0;
    extra->i2c_mode     = CYG_MCF52xx_I2C_XFER_MODE_INVALID;
}

// ----------------------------------------------------------------------------
// The functions needed for all I2C devices.

void
cyg_mcf52xx_i2c_init(struct cyg_i2c_bus* bus)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)bus->i2c_extra;
    cyg_uint8               reg;
    cyg_uint8*              base    = I2C_BASE(extra);

    cyg_drv_mutex_init(&extra->i2c_lock);
    cyg_drv_cond_init(&extra->i2c_wait, &extra->i2c_lock);
    cyg_drv_interrupt_create(I2C_ISRVEC(extra),
                             I2C_ISRPRI(extra),
                             (cyg_addrword_t) extra,
                             &mcf52xx_i2c_isr,
                             &mcf52xx_i2c_dsr,
                             &(extra->i2c_interrupt_handle),
                             &(extra->i2c_interrupt_data));
    cyg_drv_interrupt_attach(extra->i2c_interrupt_handle);

    // Before unmasking the interrupt sort out the hardware.
    //
    // The bus frequency is set by the platform HAL or user, since
    // it depends on what mixture of devices are present on the bus.
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_FDR_OFF, I2C_FDR(extra));
    // The device will operate in slave mode when idle. If there is
    // another bus master then the coldfire might accidentally accept
    // requests intended for another device. Address 0 is installed
    // as the slave address. This is the General Call address, used
    // for broadcasting. It might be better to use another address
    // like an Hs-mode one, but conflicts are still possible.
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_ADR_OFF, 0x0);
    // Enable the I2C device but do not start any transfers and
    // leave interrupts disabled.
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF, HAL_MCF52xx_I2C_CR_IEN);
    
    // As per the documentation, if IBB is set then issue a stop. It
    // is not really clear this is the right thing to do in
    // multimaster setups, if another master happens to start a
    // transfer at this exact time. Presumably it solves more problems
    // than it might cause.
    HAL_READ_UINT8(base + HAL_MCF52xx_I2C_SR_OFF, reg);
    if (reg & HAL_MCF52xx_I2C_SR_IBB) {
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF, 0x0000);
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF, 0x00A0);
        HAL_READ_UINT8( base + HAL_MCF52xx_I2C_DR_OFF, reg);
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_SR_OFF, 0x0000);
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF, 0x0000);

        // Don't forget to reenable the device.
        HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF, HAL_MCF52xx_I2C_CR_IEN);
    }
    
    // Clear any pending conditions including interrupts.
    HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_SR_OFF, 0);

    // Interrupts can now be safely unmasked
    HAL_INTERRUPT_UNMASK(I2C_ISRVEC(extra));
}

cyg_uint32
cyg_mcf52xx_i2c_tx(const cyg_i2c_device* dev, cyg_bool send_start, const cyg_uint8* tx_data, cyg_uint32 count, cyg_bool send_stop)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)dev->i2c_bus->i2c_extra;

    extra->i2c_count        = count;
    if (! extra->i2c_lost_arb) {
        extra->i2c_completed    = 0;
        extra->i2c_mode         = CYG_MCF52xx_I2C_XFER_MODE_TX;

        if (send_start) {
            extra->i2c_data.i2c_tx_data = tx_data;
            if (! mcf52xx_i2c_send_start(extra, (dev->i2c_address << 1) | 0x00)) {
                diag_printf("send_start failed\n");
                return 0;
            }
            mcf52xx_i2c_doit(extra);
        } else if ( !extra->i2c_got_nack) {
            // We are in the middle of a transaction and not
            // generating a repeated start, so the device must already
            // be set up for writes.
            extra->i2c_data.i2c_tx_data = &(tx_data[1]);
            extra->i2c_count            = count - 1;
            HAL_WRITE_UINT8(I2C_BASE(extra) + HAL_MCF52xx_I2C_DR_OFF, *tx_data);
            mcf52xx_i2c_doit(extra);
        }
    }
    if (send_stop) {
        mcf52xx_i2c_stopit(extra);
    }

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
cyg_mcf52xx_i2c_rx(const cyg_i2c_device* dev, cyg_bool send_start, cyg_uint8* rx_data, cyg_uint32 count, cyg_bool send_nack, cyg_bool send_stop)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)dev->i2c_bus->i2c_extra;
    cyg_uint8*              base    = I2C_BASE(extra);
    cyg_uint8               discard;
    
    extra->i2c_count        = count;
    extra->i2c_send_nack    = send_nack;
    
    if (! extra->i2c_lost_arb) {
        extra->i2c_completed        = 0;
        extra->i2c_data.i2c_rx_data = rx_data;
        if (send_start) {
            extra->i2c_mode     = CYG_MCF52xx_I2C_XFER_MODE_STARTRX;
            if (! mcf52xx_i2c_send_start(extra, (dev->i2c_address << 1) | 0x01) ) {
                return 0;
            }
        } else {
            // In the middle of a transaction. The previous transfer
            // will have left the device in tx mode.
            extra->i2c_mode     = CYG_MCF52xx_I2C_XFER_MODE_RX;
            if (send_nack && (1 == count)) {
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA |
                                HAL_MCF52xx_I2C_CR_TXAK);
            } else {
                HAL_WRITE_UINT8(base + HAL_MCF52xx_I2C_CR_OFF,
                                HAL_MCF52xx_I2C_CR_IEN  |
                                HAL_MCF52xx_I2C_CR_IIEN |
                                HAL_MCF52xx_I2C_CR_MSTA);
            }
            // So reading the data register here should get the device
            // reading the next byte.
            HAL_READ_UINT8(base + HAL_MCF52xx_I2C_DR_OFF, discard);
        }
        mcf52xx_i2c_doit(extra);
    }
    if (send_stop) {
        mcf52xx_i2c_stopit(extra);
    }
    return count - extra->i2c_count;
}

void
cyg_mcf52xx_i2c_stop(const cyg_i2c_device* dev)
{
    cyg_mcf52xx_i2c_extra*  extra   = (cyg_mcf52xx_i2c_extra*)dev->i2c_bus->i2c_extra;
    mcf52xx_i2c_stopit(extra);
}

#endif  //  defined(CYGHWR_DEVS_I2C_MCF52xx_MULTIPLE_BUSES) || defined(HAL_MCF52xx_I2C_SINGLETON_BASE)
//---------------------------------------------------------------------------
// EOF i2c_mcf52xx.c
