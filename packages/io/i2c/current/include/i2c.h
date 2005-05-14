#ifndef CYGONCE_IO_I2C_H
#define CYGONCE_IO_I2C_H

//=============================================================================
//
//      i2c.h
//
//      Generic API for accessing devices on an I2C bus
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2004 eCosCentric Limited
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
//=============================================================================
//####DESCRIPTIONBEGIN####
//
// Author(s): 	bartv
// Date:	    2004-10-05
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/infra.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_tables.h>
#include <cyg/hal/hal_io.h>

// The information needed for interacting with a specific I2C device.
// The bus provides the tx and rx functions. The address is (usually)
// a 7-bit number sent at the start of each operation. The flags is
// not currently used, but provides room for future expansion such as
// 10-bit addresses. The delay should be a clock period in
// nanoseconds. For the default 100KHz clock that gives a value of
// 10000

typedef struct cyg_i2c_device {
    struct cyg_i2c_bus* i2c_bus;
    cyg_uint16          i2c_address;
    cyg_uint16          i2c_flags;
    cyg_uint32          i2c_delay;
} cyg_i2c_device;

#define CYG_I2C_DEFAULT_DELAY   10000

// A utility macro for defining an I2C device
#define CYG_I2C_DEVICE(_name_, _bus_, _address_, _flags_, _delay_)  \
    cyg_i2c_device _name_ = {                                       \
        .i2c_bus        = _bus_,                                    \
        .i2c_address    = _address_,                                \
        .i2c_flags      = _flags_,                                  \
        .i2c_delay      = _delay_                                   \
    }

// The information needed for interacting over a particular I2C bus.
// Most hardware will only have one bus, but multiple buses are
// supported. Thread synchronization happens on a per-bus level.
typedef struct cyg_i2c_bus {
    cyg_drv_mutex_t         i2c_lock;
#ifdef CYGDBG_USE_ASSERTS
    const cyg_i2c_device*   i2c_current_device;
#endif
    // The hardware-specific functions that do the real work
    void                    (*i2c_init_fn)(struct cyg_i2c_bus*);
    cyg_uint32              (*i2c_tx_fn)(const cyg_i2c_device*, cyg_bool, const cyg_uint8*, cyg_uint32, cyg_bool);
    cyg_uint32              (*i2c_rx_fn)(const cyg_i2c_device*, cyg_bool, cyg_uint8*, cyg_uint32, cyg_bool, cyg_bool);
    void                    (*i2c_stop_fn)(const cyg_i2c_device*);
    // A spare field for use by the driver
    void*                   i2c_extra;
} CYG_HAL_TABLE_TYPE cyg_i2c_bus;

#define CYG_I2C_BUS(_name_, _init_fn_, _tx_fn_, _rx_fn_, _stop_fn_, _extra_)    \
    cyg_i2c_bus _name_  CYG_HAL_TABLE_ENTRY( i2c_buses ) = {                    \
        .i2c_init_fn    = _init_fn_,                                            \
        .i2c_tx_fn      = _tx_fn_,                                              \
        .i2c_rx_fn      = _rx_fn_,                                              \
        .i2c_stop_fn    = _stop_fn_,                                            \
        .i2c_extra      = _extra_                                               \
    }

// To support static initialization all buses are held in a table.
// Of course usually this table will only contain a single entry
extern cyg_i2c_bus cyg_i2c_buses[], cyg_i2c_buses_end;

// The main exported interface. There are high-level operations for
// simple transmits and receives, and transaction-oriented routines
// for more complicated operations including those involving repeated
// starts.
externC cyg_uint32  cyg_i2c_tx(const cyg_i2c_device*, const cyg_uint8*, cyg_uint32);
externC cyg_uint32  cyg_i2c_rx(const cyg_i2c_device*, cyg_uint8*, cyg_uint32);
externC void        cyg_i2c_transaction_begin(const cyg_i2c_device*);
externC cyg_bool    cyg_i2c_transaction_begin_nb(const cyg_i2c_device*);
externC cyg_uint32  cyg_i2c_transaction_tx(const cyg_i2c_device*, cyg_bool, const cyg_uint8*, cyg_uint32, cyg_bool);
externC cyg_uint32  cyg_i2c_transaction_rx(const cyg_i2c_device*, cyg_bool, cyg_uint8*, cyg_uint32, cyg_bool, cyg_bool);
externC void        cyg_i2c_transaction_stop(const cyg_i2c_device*);
externC void        cyg_i2c_transaction_end(const cyg_i2c_device*);

// Bit-bang support. This merely requires the platform HAL to provide
// a function for the actual bit-bang operation. The rest is handled
// by the generic code.
typedef enum cyg_i2c_bitbang_op {
    CYG_I2C_BITBANG_INIT                    = 0,
    CYG_I2C_BITBANG_SCL_HIGH                = 1,
    CYG_I2C_BITBANG_SCL_LOW                 = 2,
    CYG_I2C_BITBANG_SCL_HIGH_CLOCKSTRETCH   = 3,
    CYG_I2C_BITBANG_SCL_LOW_SDA_INPUT       = 4,
    CYG_I2C_BITBANG_SDA_OUTPUT              = 5,
    CYG_I2C_BITBANG_SDA_HIGH                = 6,
    CYG_I2C_BITBANG_SDA_LOW                 = 7,
    CYG_I2C_BITBANG_SDA_READ                = 8
} cyg_i2c_bitbang_op;

typedef cyg_bool (*cyg_i2c_bitbang_fn)(cyg_i2c_bus*, cyg_i2c_bitbang_op);

// A bitbang bus can be instantiated by just providing the bitbang function.
// That is held in the extra field.
#define CYG_I2C_BITBANG_BUS(_name_, _bitbang_fn_)                   \
    CYG_I2C_BUS(_name_,                                             \
                &cyg_i2c_bitbang_init,                              \
                &cyg_i2c_bitbang_tx,                                \
                &cyg_i2c_bitbang_rx,                                \
                &cyg_i2c_bitbang_stop,                              \
                (void*) _bitbang_fn_)

// The generic bit-bang functions. These are not called directly,
// but must be exported for use by the above macro.
externC void        cyg_i2c_bitbang_init(cyg_i2c_bus*);
externC cyg_uint32  cyg_i2c_bitbang_tx(const cyg_i2c_device*, cyg_bool, const cyg_uint8*, cyg_uint32, cyg_bool);
externC cyg_uint32  cyg_i2c_bitbang_rx(const cyg_i2c_device*, cyg_bool, cyg_uint8*, cyg_uint32, cyg_bool, cyg_bool);
externC void        cyg_i2c_bitbang_stop(const cyg_i2c_device*);

// Allow the HAL to export the buses and named devices. There are
// circular dependencies between this header and the HAL ones so
// it is not easy for the HAL headers to define cyg_i2c_device
// externs directly. Instead the HAL can define this macro which
// provides the externs.
#ifdef HAL_I2C_EXPORTED_DEVICES
  HAL_I2C_EXPORTED_DEVICES
#endif

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_IO_I2C_H
// End of i2c.h
