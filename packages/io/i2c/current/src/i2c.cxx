//==========================================================================
//
//      i2c.cxx
//
//      Generic API for accessing devices attached to an I2C bus
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004, 2005 eCosCentric Limited
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
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2004-10-05
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/infra.h>
#include <pkgconf/io_i2c.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/io/i2c.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_diag.h>

// ----------------------------------------------------------------------------
// Static initialization.
//
// This is somewhat tricky. There is a strong argument for leaving the
// initialization to the platform HAL since with most I2C buses that
// code will be straightforward. However that would mean that the
// cyg_i2c_bus structure(s) and associated code would always get
// linked in, even if the application does not use any i2c devices.
// Instead to get the maximum benefit of linker garbage collection
// initialization is handled by the generic I2C code, using the usual
// dummy C++ object with a prioritized constructor. There is a dummy
// references to this object from the transaction end function. The
// result should be that if the application uses any I2C functionality
// then all required code and data should get included, otherwise it
// will all be elided.
//
// The init priority is configurable, defaulting to CYG_INIT_DRIVERS.
// Arguably it should happen a bit earlier to allow other drivers to
// perform I2C operations, but there is no CYG_INIT_BUS.
//
// All I2C buses are kept in a table, so that the init code can
// iterate through each one.

CYG_HAL_TABLE_BEGIN(cyg_i2c_buses, i2c_buses);
CYG_HAL_TABLE_END(cyg_i2c_buses_end, i2c_buses);

class cyg_i2c_init {
  public:
    cyg_uint8   i2c_dummy;
    cyg_i2c_init();
};

static cyg_i2c_init cyg_i2c_init_object CYGBLD_ATTRIB_INIT_PRI(CYGNUM_I2C_INIT_PRIORITY);

cyg_i2c_init::cyg_i2c_init()
{
    cyg_i2c_bus*    bus;

    cyg_i2c_init_object.i2c_dummy   = 0;
    for (bus = &(cyg_i2c_buses[0]); bus != &cyg_i2c_buses_end; bus++) {
        cyg_drv_mutex_init(&(bus->i2c_lock));
#ifdef CYGDBG_USE_ASSERTS
        bus->i2c_current_device = (const cyg_i2c_device*) 0;
#endif        
        if ((void (*)(cyg_i2c_bus*))0 != bus->i2c_init_fn) {
            (*bus->i2c_init_fn)(bus);
        }
    }
}

// ----------------------------------------------------------------------------
// The main exported routines just operate in terms of the transaction ones.

extern "C" cyg_uint32
cyg_i2c_tx(const cyg_i2c_device* dev, const cyg_uint8* buf, cyg_uint32 count)
{
    cyg_uint32  result;
    cyg_i2c_transaction_begin(dev);
    result = cyg_i2c_transaction_tx(dev, true, buf, count, true);
    cyg_i2c_transaction_end(dev);
    return result;
}

extern "C" cyg_uint32
cyg_i2c_rx(const cyg_i2c_device* dev, cyg_uint8* buf, cyg_uint32 count)
{
    cyg_uint32  result;
    cyg_i2c_transaction_begin(dev);
    result = cyg_i2c_transaction_rx(dev, true, buf, count, true, true);
    cyg_i2c_transaction_end(dev);
    return result;
}

// Transaction begin/end relate to the per-bus locking. There does not
// seem to be any need to interact with the hardware at this point.
extern "C" void
cyg_i2c_transaction_begin(const cyg_i2c_device* dev)
{
    cyg_i2c_bus*    bus;
    
    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");

    while (! cyg_drv_mutex_lock(&(bus->i2c_lock)));
#ifdef CYGDBG_USE_ASSERTS
    bus->i2c_current_device = dev;
#endif
    // All done, return with the bus locked.
}

extern "C" cyg_bool
cyg_i2c_transaction_begin_nb(const cyg_i2c_device* dev)
{
    cyg_bool        result  = false;
    cyg_i2c_bus*    bus;

    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");

    if (cyg_drv_mutex_trylock(&(bus->i2c_lock))) {
#ifdef CYGDBG_USE_ASSERTS
        bus->i2c_current_device = dev;
#endif
        result = true;
    }

    return result;
}

extern "C" void
cyg_i2c_transaction_end(const cyg_i2c_device* dev)
{
    cyg_i2c_bus*    bus;
    
    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");
    CYG_PRECONDITION(bus->i2c_current_device == dev, "I2C transaction end requested without claiming the bus");

    cyg_drv_mutex_unlock(&(bus->i2c_lock));
#ifdef CYGDBG_USE_ASSERTS
    bus->i2c_current_device = (const cyg_i2c_device*)0;
#endif
    // Avoid problems with linker garbage collection
    cyg_i2c_init_object.i2c_dummy   = 0;
}

// The I/O operations just indirect through the per-bus function pointers
extern "C" cyg_uint32
cyg_i2c_transaction_tx(const cyg_i2c_device* dev, cyg_bool start, const cyg_uint8* buf, cyg_uint32 count, cyg_bool stop)
{
    cyg_i2c_bus*    bus;
    cyg_uint32      result;
    
    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");
    CYG_PRECONDITION(bus->i2c_current_device == dev, "I2C transaction end requested without claiming the bus");
    CYG_CHECK_FUNC_PTR(bus->i2c_tx_fn, "I2C bus has not provided a tx function");
    
    result = (*(bus->i2c_tx_fn))(dev, start, buf, count, stop);
    return result;
}

extern "C" cyg_uint32
cyg_i2c_transaction_rx(const cyg_i2c_device* dev, cyg_bool start, cyg_uint8* buf, cyg_uint32 count, cyg_bool nak, cyg_bool stop)
{
    cyg_i2c_bus*    bus;
    cyg_uint32      result;
    
    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");
    CYG_PRECONDITION(bus->i2c_current_device == dev, "I2C transaction end requested without claiming the bus");
    CYG_CHECK_FUNC_PTR(bus->i2c_rx_fn, "I2C bus has not provided an rx function");
    
    result = (*(bus->i2c_rx_fn))(dev, start, buf, count, nak, stop);
    return result;
}

extern "C" void
cyg_i2c_transaction_stop(const cyg_i2c_device* dev)
{
    cyg_i2c_bus*    bus;
    
    CYG_CHECK_DATA_PTR(dev, "valid I2C device pointer required");
    bus = dev->i2c_bus;
    CYG_CHECK_DATA_PTR(bus, "I2C device does not point at a valid bus structure");
    CYG_PRECONDITION(bus->i2c_current_device == dev, "I2C transaction end requested without claiming the bus");
    CYG_CHECK_FUNC_PTR(bus->i2c_stop_fn, "I2C bus has not provided a stop function");
    
    (*(bus->i2c_stop_fn))(dev);
}

// ----------------------------------------------------------------------------
// The bit-banging support
//
// Optional debug support, useful while sorting out the platform-specific
// bitbang function.
#if 1
# define DEBUG(_format_, ...)
#else
# define DEBUG(_format_, ...) diag_printf(_format_, ## __VA_ARGS__)
#endif


// Initialization calls into the h/w specific bit-bang function to set
// up the GPIO pins and to set both SCL and SDA as outputs and high.

extern "C" void
cyg_i2c_bitbang_init(cyg_i2c_bus* mash)
{
    cyg_i2c_bitbang_fn      banger  = (cyg_i2c_bitbang_fn)(mash->i2c_extra);
    CYG_CHECK_FUNC_PTR(banger, "bitbanged i2c bus has not provided a bitbang function");
    
    DEBUG("i2c bitbang init\n");
    (*banger)(mash, CYG_I2C_BITBANG_INIT);
}

// Send a single byte out of the bus and get back the acknowledgement.
// This may be the addressing byte or a data byte.
// Preconditions:
//     SCL     output low. The previous operation has completed, the
//             acknowledgement bit has been received and processed.
//             None of the devices should be driving the bus.
//     SDA     output, indeterminate value.
// Postconditions:
//     SCL     output low, the acknowledgement has been received.
//     SDA     may be left as an input or an output, depending on the
//             last argument
//
// The return value is the acknowledge bit, i.e. 0 for ack, 1 for
// nak.

static cyg_bool
cyg_i2c_bitbang_send_byte(cyg_i2c_bus* mash, const cyg_i2c_device* dev, cyg_uint32 delay, cyg_uint8 data, cyg_bool leave_as_input)
{
    cyg_i2c_bitbang_fn      banger  = (cyg_i2c_bitbang_fn)(mash->i2c_extra);
    cyg_uint8               mask;
    cyg_bool                result;

    DEBUG("i2c bitbang send_byte %02x, leave_as_input %d\n", data, leave_as_input);
    
    for (mask = 0x0080; 0x00 != mask; ) {
        // Transfer the next bit of data
        (*banger)(mash, (0 != (data & mask)) ? CYG_I2C_BITBANG_SDA_HIGH : CYG_I2C_BITBANG_SDA_LOW);
        mask = mask >> 1;
        // We should now be able to just set the clock high and wait
        // for the delay period. However if the device is having
        // difficulty keeping up then it may perform clock stretching,
        // basically keeping the clock low for a period of time.
        // SCL_CLOCKSTRETCH means set the clock high, then wait
        // until it really has gone high.
        (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH_CLOCKSTRETCH);
        // The device is now busy sampling the bit.
        HAL_DELAY_US(delay);
        // Unless this was the last bit, just drop the clock. If it is
        // the last bit switch SDA to an input as well.
        (*banger)(mash, (0 != mask) ? CYG_I2C_BITBANG_SCL_LOW : CYG_I2C_BITBANG_SCL_LOW_SDA_INPUT);
        HAL_DELAY_US(delay);
    }
    // The last bit has been sent, SCL is low, and SDA has been turned
    // into an input. The device should be placing the acknowledge bit
    // onto SDA. Reading the acknowledge bit still needs a clock cycle.
    (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH);
    HAL_DELAY_US(delay);
    result  = (*banger)(mash, CYG_I2C_BITBANG_SDA_READ);
    DEBUG("i2c bitbang send_byte, got ack %d\n", result);
    
    // Drop SCL again, and leave SDA as either an input or an output
    // depending on the calling code's requirements and whether or not
    // the device ack'd.
    (*banger)(mash, CYG_I2C_BITBANG_SCL_LOW);
    HAL_DELAY_US(delay);
    if ((1 == result) || !leave_as_input) {
        (*banger)(mash, CYG_I2C_BITBANG_SDA_OUTPUT);
    }

    return result;
}

// Read a single byte from the bus and generate an ack/nak.
// Preconditions:
//    SCL       output low. The previous operation has completed.
//              The device should have placed the data on the bus.
//    SDA       input
// Postconditions:
//    SCL       output low. The acknowledgement has been sent.
//    SDA       may be left as an input or an output, depending on
//              whether or not this is the last byte in a transfer.
//              For the last byte we want to send a nak.
//
// The result is the byte read.

static cyg_uint8
cyg_i2c_bitbang_read_byte(cyg_i2c_bus* mash, const cyg_i2c_device* dev, cyg_uint32 delay, cyg_bool nak)
{
    cyg_i2c_bitbang_fn  banger  = (cyg_i2c_bitbang_fn)(mash->i2c_extra);
    cyg_uint32          i;
    cyg_uint8           result  = 0;

    DEBUG("i2c bitbang read_byte, nak %d\n", nak);
    
    for (i = 0; i < 8; i++) {
        (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH);
        HAL_DELAY_US(delay);
        result = result << 1;
        if (0 != (*banger)(mash, CYG_I2C_BITBANG_SDA_READ)) {
            result |= 0x01;
        }
        (*banger)(mash, CYG_I2C_BITBANG_SCL_LOW);
        HAL_DELAY_US(delay);
    }
    // We have read the last bit. SDA is still an input, SCL is low.
    // We need to switch SDA to an output and send the ack/nak
    (*banger)(mash, CYG_I2C_BITBANG_SDA_OUTPUT);
    (*banger)(mash, nak ? CYG_I2C_BITBANG_SDA_HIGH : CYG_I2C_BITBANG_SDA_LOW);
    (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH);
    HAL_DELAY_US(delay);
    (*banger)(mash, nak ? CYG_I2C_BITBANG_SCL_LOW : CYG_I2C_BITBANG_SCL_LOW_SDA_INPUT);
    HAL_DELAY_US(delay);

    DEBUG("i2c bitbang read_byte, result %02x\n", result);
    return result;
}

// Generate a start condition.
//
// Preconditions:
//    SCL       output, indeterminate
//    SDA       output, indeterminate
// Postconditions:
//    SCL       output low
//    SDA       output low
//
// At the start of a transaction we know that both SCL and SDA will be
// high, but for a repeated start
static void
cyg_i2c_bitbang_send_start(cyg_i2c_bus* mash, cyg_uint32 delay)
{
    cyg_i2c_bitbang_fn      banger  = (cyg_i2c_bitbang_fn)(mash->i2c_extra);

    DEBUG("i2c bitbang, generating start\n");
    // First get both SCL and SDA back into a known state
    (*banger)(mash, CYG_I2C_BITBANG_SDA_HIGH);
    (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH);
    HAL_DELAY_US(delay);
    // A start condition involves dropping SDA while SCL stays high.
    (*banger)(mash, CYG_I2C_BITBANG_SDA_LOW);
    // Make sure that all devices have seen the start condition and
    // reacted
    HAL_DELAY_US(delay);
    // Drop the clock, so that we can send the address/direction byte
    (*banger)(mash, CYG_I2C_BITBANG_SCL_LOW);
    HAL_DELAY_US(delay);
}

// Generate a stop condition
// Preconditions:
//    SCL       output, low
//    SDA       output, indeterminate
// Postconditions:
//    SCL       output, high
//    SDA       output, high
static void
cyg_i2c_bitbang_send_stop(cyg_i2c_bus* mash, cyg_uint32 delay)
{
    cyg_i2c_bitbang_fn      banger  = (cyg_i2c_bitbang_fn)(mash->i2c_extra);
    
    DEBUG("i2c bitbang, generating stop\n");
    // We need SDA low, then we can generate the stop signal
    (*banger)(mash, CYG_I2C_BITBANG_SDA_LOW);
    (*banger)(mash, CYG_I2C_BITBANG_SCL_HIGH);
    HAL_DELAY_US(delay);
    (*banger)(mash, CYG_I2C_BITBANG_SDA_HIGH);
    // Ensure that the minimum delay between stop and start is observed
    HAL_DELAY_US(delay);
}

// A full transfer to a given device, within in a transaction.
extern "C" cyg_uint32
cyg_i2c_bitbang_tx(const cyg_i2c_device* dev, cyg_bool send_start, const cyg_uint8* buf, cyg_uint32 count, cyg_bool send_stop)
{
    cyg_uint32      result  = 0;
    // The bit-bang code works in terms of us delays rather than ns
    cyg_uint32      delay = (0 == dev->i2c_delay) ? 1 : (dev->i2c_delay + 1999) / 2000;
    
    CYG_CHECK_DATA_PTR(buf, "i2c tx operation requested but no data supplied");
    CYG_PRECONDITION(count > 0, "at least one byte should be sent");

    DEBUG("i2c bitbang tx, dev %08x, addr %02x, delay %d, send_start %d, buf %08x, count %d, send_stop %d\n",
          dev, dev->i2c_address, delay, send_start, buf, count, send_stop);
    
    if (send_start) {
        cyg_i2c_bitbang_send_start(dev->i2c_bus, delay);

        // Now send a single byte, holding the address shifted left and a
        // 0 to indicate a write. A nak indicates that the device has not
        // responded. SDA should stay as an output.
        if (0 != cyg_i2c_bitbang_send_byte(dev->i2c_bus, dev, delay, (dev->i2c_address << 1) | 0x00, false) ) {
            // Get the bus back in a consistent state
            DEBUG("i2 bitbang tx, no device has responded to address %02x\n", dev->i2c_address);
            cyg_i2c_bitbang_send_stop(dev->i2c_bus, delay);
            return 0;
        }
        // The device has ack'd so we can continue sending
    }
    // Send all bytes, unless we receive a nak. SDA should remain an output
    do {
        result += 1;
        if (0 != cyg_i2c_bitbang_send_byte(dev->i2c_bus, dev, delay, buf[result - 1], false)) {
            break;
        }
    } while (result < count);

    // At this point we have SCL low and SDA an indeterminate output
    if (send_stop) {
        cyg_i2c_bitbang_send_stop(dev->i2c_bus, delay);
    }
    DEBUG("i2c bitbang tx, %d bytes sent out of %d\n", result, count);
    
    return result;
}

// A full transfer from a given device. The bus has already been locked
// by higher-level code. On entry both SDA and SCL should be outputs
// and both should be high. The same conditions should hold on exit.
extern "C" cyg_uint32
cyg_i2c_bitbang_rx(const cyg_i2c_device* dev, cyg_bool send_start, cyg_uint8* buf, cyg_uint32 count, cyg_bool send_nak, cyg_bool send_stop)
{
    cyg_uint32      result  = 0;
    // The bit-bang code works in terms of us delays rather than ns
    cyg_uint32      delay = (0 == dev->i2c_delay) ? 1 : (dev->i2c_delay + 1999) / 2000;
    
    DEBUG("i2c bitbang rx, dev %08x, addr %02x, delay %d, send_start %d, buf %08x, count %d, send_nak %d, send_stop %d\n",
          dev, dev->i2c_address, delay, send_start, buf, count, send_nak, send_stop);
    
    CYG_CHECK_DATA_PTR(buf, "i2c rx operation requested but no data supplied");
    CYG_PRECONDITION(count > 0, "at least one byte should be sent");
    CYG_PRECONDITION(send_nak || !send_stop, "a stop can only be generated after the receive has been nak'd");

    if (send_start) {
        cyg_i2c_bitbang_send_start(dev->i2c_bus, delay);

        // Now send a single byte, holding the address shifted left and a
        // 1 to indicate a read. A nak indicates that the device has not
        // responded. SDA should become an output.
        if (0 != cyg_i2c_bitbang_send_byte(dev->i2c_bus, dev, delay, (dev->i2c_address << 1) | 0x01, true) ) {
            // Get the bus back in a consistent state
            DEBUG("i2c bitbang rx, no device has responded to address %02x\n", dev->i2c_address); 
            cyg_i2c_bitbang_send_stop(dev->i2c_bus, delay);
            return 0;
        }
        // The device has ack'd so we can continue sending
    }

    // The device has ack'd. Read the number of bytes specified. The
    // device cannot stop sending, although it may clock-stretch. For
    // the last byte we usually we want to NAK and SDA should become
    // an input, but if the data stream is variable length with e.g.
    // the first byte indicating the length then we want to support
    // partial reads.
    for (result = 0; result < count; result++) {
        buf[result] = cyg_i2c_bitbang_read_byte(dev->i2c_bus, dev, delay, (send_nak && (result == (count - 1))) ? true : false);
    }

    // At this point we have SCL low and SDA an indeterminate output
    if (send_stop) {
        cyg_i2c_bitbang_send_stop(dev->i2c_bus, delay);
    }

    DEBUG("i2c bitbang rx, read %d bytes\n", result);
    return result;
}

extern "C" void
cyg_i2c_bitbang_stop(const cyg_i2c_device* dev)
{
    // The bit-bang code works in terms of us delays rather than ns
    cyg_uint32      delay = (0 == dev->i2c_delay) ? 1 : (dev->i2c_delay + 1999) / 2000;
    cyg_i2c_bitbang_send_stop(dev->i2c_bus, delay);
}
