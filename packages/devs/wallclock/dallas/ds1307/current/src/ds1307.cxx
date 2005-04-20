//==========================================================================
//
//      devs/wallclock/ds1307.inl
//
//      Wallclock implementation for Dallas 1307
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas
// Copyright (C) 2004 eCosCentric Ltd
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  
// Date:          2003-09-19
// Purpose:       Wallclock driver for Dallas 1307
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>                // Platform specific configury
#include <pkgconf/wallclock.h>          // Wallclock device config
#include <pkgconf/devices_wallclock_dallas_ds1307.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // interrupt enable/disable
#include <cyg/infra/cyg_type.h>         // Common type definitions and support
#include <string.h>                     // memcpy()

#include <cyg/io/wallclock.hxx>         // The WallClock API
#include <cyg/io/wallclock/wallclock.inl> // Helpers

#include <cyg/infra/diag.h>

#if 1
# define DEBUG(_format_, ...)
#else
# define DEBUG(_format_, ...) diag_printf(_format_, ## __VA_ARGS__)
#endif

// Registers.
// FIXME: there is no need to include the control register here, it
// controls a square wave output which is independent from the wallclock.
// However fixing it would require changing any platforms that use the
// old DS_GET()/DS_PUT() functionality.
#define DS_SECONDS         0x00
#define DS_MINUTES         0x01
#define DS_HOURS           0x02
#define DS_DOW             0x03
#define DS_DOM             0x04
#define DS_MONTH           0x05
#define DS_YEAR            0x06
#define DS_CONTROL         0x07
#define DS_REGS_SIZE       0x08   // Size of register space

#define DS_SECONDS_CH      0x80   // Clock Halt
#define DS_HOURS_24        0x40   // 24 hour clock mode

// The DS1307 chip is accessed via I2C (2-wire protocol). This can be
// implemented in one of two ways. If the platform supports the generic
// I2C API then it should also export a cyg_i2c_device structure
// cyg_i2c_wallclock_ds1307, and this can be manipulated via the
// usual cyg_i2c_tx() and cyg_i2c_rx() functions. Alternatively (and
// primarily for older ports predating the generic I2C package)
// the platform HAL can provide the following two macros/functions:
//
// void DS_GET(cyg_uint8 *regs)
//    Reads the entire set of registers (8 bytes) into *regs
// void DS_PUT(cyg_uint8 *regs)
//    Updated the entire set of registers (8 bytes) from *regs
//
// Using this method, the data in the registers is guaranteed to be
// stable (if the access function manipulates the registers in an
// single operation)
//
// If the platform HAL implements the CDL interface
// CYGINT_DEVICES_WALLCLOCK_DALLAS_DS1307_I2C then the I2C API will be used.

#ifdef CYGINT_DEVICES_WALLCLOCK_DALLAS_DS1307_I2C
# if defined(DS_GET) || defined(DS_PUT)
#  error The macros DS_GET and DS_PUT should not be defined if the generic I2C API is used
# endif

#include <cyg/io/i2c.h>

static void
DS_GET(cyg_uint8* regs)
{
    cyg_uint8   tx_data[1];
    cyg_bool    ok = true;

    tx_data[0]  = 0x00; // Initial register to read
    cyg_i2c_transaction_begin(&cyg_i2c_wallclock_ds1307);
    if (1 != cyg_i2c_transaction_tx(&cyg_i2c_wallclock_ds1307, true, tx_data, 1, false)) {
        // The device has not responded to the address byte.
        ok = false;
    } else {
        // Now fetch the data
        cyg_i2c_transaction_rx(&cyg_i2c_wallclock_ds1307, true, regs, 8, true, true);

        // Verify that there are reasonable default settings. The
        // register values can be used as array indices so bogus
        // values can lead to bus errors or similar problems.
        
        // Years: 00 - 99, with 70-99 interpreted as 1970 onwards.
        if ((regs[DS_YEAR] & 0x0F) > 0x09) {
            ok = false;
        }
        // Month: 1 - 12
        if ((regs[DS_MONTH] == 0x00) ||
            ((regs[DS_MONTH] > 0x09) && (regs[DS_MONTH] < 0x10)) ||
            (regs[DS_MONTH] > 0x12)) {
            ok = false;
        }
        // Day: 1 - 31. This check does not allow for 28-30 day months.
        if ((regs[DS_DOM] == 0x00) ||
            ((regs[DS_DOM] & 0x0F) > 0x09) ||
            (regs[DS_DOM] > 0x31)) {
            ok = false;
        }
        // Hours: 0 - 23. Always run in 24-hour mode
        if ((0 != (regs[DS_HOURS] & DS_HOURS_24)) ||
            ((regs[DS_HOURS] & 0x0F) > 0x09) ||
            ((regs[DS_HOURS] & 0x3F) > 0x023)) {
            ok = false;
        }
        // Ignore the DOW field. The wallclock code does not need it, and
        // it is hard to calculate.
        // Minutes: 0 - 59
        if (((regs[DS_MINUTES] & 0x0F) > 0x09) ||
            (regs[DS_MINUTES] > 0x59)) {
            ok = false;
        }
        // Seconds: 0 - 59
        if (((regs[DS_SECONDS] & 0x0F) > 0x09) ||
            (regs[DS_SECONDS] > 0x59)) {
            ok = false;
        }
    }
    cyg_i2c_transaction_end(&cyg_i2c_wallclock_ds1307);
    if (! ok) {
        // Any problems, return Jan 1 1970 but do not update the hardware.
        // Leave it to the user or other code to set the clock to a sensible
        // value.
        regs[DS_SECONDS]  = 0x00;
        regs[DS_MINUTES]  = 0x00;
        regs[DS_HOURS]    = 0x00;
        regs[DS_DOW]      = 0x00;
        regs[DS_DOM]      = 0x01;                                                         
        regs[DS_MONTH]    = 0x01;                                                        
        regs[DS_YEAR]     = 0x70;
        regs[DS_CONTROL]  = 0x00;
    }
}

static void
DS_PUT(cyg_uint8* regs)
{
    cyg_uint8 tx_data[9];
    tx_data[0] = 0;
    memcpy(&(tx_data[1]), regs, 8);
    cyg_i2c_tx(&cyg_i2c_wallclock_ds1307, tx_data, 9);
}

#else
// Platform details. The platform HAL or some other package should
// provide this header, containing the required macros
# include CYGDAT_DEVS_WALLCLOCK_DALLAS_1307_INL
#endif

//----------------------------------------------------------------------------
// Accessor functions

static inline void
init_ds_hwclock(void)
{
    cyg_uint8 regs[DS_REGS_SIZE];

    // Fetch the current state
    DS_GET(regs);
    
    // If the clock is not currently running or is not in 24-hours mode,
    // update it. Otherwise skip the update because the clock may have
    // ticked between DS_GET() and DS_PUT() and we could be losing the
    // occasional second.
    if ((0 != (regs[DS_HOURS] & DS_HOURS_24)) ||
        (0 != (regs[DS_SECONDS] & DS_SECONDS_CH))) {
        regs[DS_SECONDS] &= ~DS_SECONDS_CH;
        regs[DS_HOURS]   &= ~DS_HOURS_24;
        DS_PUT(regs);
    }
}

static inline void
set_ds_hwclock(cyg_uint32 year, cyg_uint32 month, cyg_uint32 mday,
               cyg_uint32 hour, cyg_uint32 minute, cyg_uint32 second)
{
    cyg_uint8 regs[DS_REGS_SIZE];

    // Set up the registers
    regs[DS_CONTROL]    = 0x00;
    regs[DS_YEAR]       = TO_BCD((cyg_uint8)(year % 100));
    regs[DS_MONTH]      = TO_BCD((cyg_uint8)month);
    regs[DS_DOM]        = TO_BCD((cyg_uint8)mday);
    regs[DS_DOW]        = TO_BCD(0x01);     // Not accurate, but not used by this driver either
    regs[DS_HOURS]      = TO_BCD((cyg_uint8)hour);
    regs[DS_MINUTES]    = TO_BCD((cyg_uint8)minute);
    // This also starts the clock
    regs[DS_SECONDS]    = TO_BCD((cyg_uint8)second);

    // Send the register set to the hardware
    DS_PUT(regs);

    // These debugs will cause the test to eventually fail due to
    // the printouts causing timer interrupts to be lost...
    DEBUG("DS1307 set -------------\n");
    DEBUG("regs %02x %02x %02x %02x %02x %02x %02x %02x\n",
          regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7]);
    DEBUG("year %02d\n", year);
    DEBUG("month %02d\n", month);
    DEBUG("mday %02d\n", mday);
    DEBUG("hour %02d\n", hour);
    DEBUG("minute %02d\n", minute);
    DEBUG("second %02d\n", second);
}

static inline void
get_ds_hwclock(cyg_uint32* year, cyg_uint32* month, cyg_uint32* mday,
               cyg_uint32* hour, cyg_uint32* minute, cyg_uint32* second)
{
    cyg_uint8 regs[DS_REGS_SIZE];

    // Fetch the current state
    DS_GET(regs);

    *year = (cyg_uint32)TO_DEC(regs[DS_YEAR]);
    // The year field only has the 2 least significant digits :-(
    if (*year >= 70) {
        *year += 1900;
    } else {
        *year += 2000;
    }
    *month = (cyg_uint32)TO_DEC(regs[DS_MONTH]);
    *mday = (cyg_uint32)TO_DEC(regs[DS_DOM]);
    *hour = (cyg_uint32)TO_DEC(regs[DS_HOURS] & 0x3F);
    *minute = (cyg_uint32)TO_DEC(regs[DS_MINUTES]);
    *second = (cyg_uint32)TO_DEC(regs[DS_SECONDS] & 0x7F);

    // These debugs will cause the test to eventually fail due to
    // the printouts causing timer interrupts to be lost...
    DEBUG("DS1307 get -------------\n");
    DEBUG("regs %02x %02x %02x %02x %02x %02x %02x %02x\n",
          regs[0], regs[1], regs[2], regs[3], regs[4], regs[5], regs[6], regs[7]);
    DEBUG("year %02d\n", *year);
    DEBUG("month %02d\n", *month);
    DEBUG("mday %02d\n", *mday);
    DEBUG("hour %02d\n", *hour);
    DEBUG("minute %02d\n", *minute);
    DEBUG("second %02d\n", *second);
}

//-----------------------------------------------------------------------------
// Functions required for the hardware-driver API.

// Returns the number of seconds elapsed since 1970-01-01 00:00:00.
cyg_uint32 
Cyg_WallClock::get_hw_seconds(void)
{
    cyg_uint32 year, month, mday, hour, minute, second;

    get_ds_hwclock(&year, &month, &mday, &hour, &minute, &second);
    cyg_uint32 now = _simple_mktime(year, month, mday, hour, minute, second);
    return now;
}

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE

// Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
void
Cyg_WallClock::set_hw_seconds( cyg_uint32 secs )
{
    cyg_uint32 year, month, mday, hour, minute, second;

    _simple_mkdate(secs, &year, &month, &mday, &hour, &minute, &second);
    set_ds_hwclock(year, month, mday, hour, minute, second);
}

#endif

void
Cyg_WallClock::init_hw_seconds(void)
{
#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    init_ds_hwclock();
#else
    // This is our base: 1970-01-01 00:00:00
    // Set the HW clock - if for nothing else, just to be sure it's in a
    // legal range. Any arbitrary base could be used.
    // After this the hardware clock is only read.
    set_ds_hwclock(1970,1,1,0,0,0);
#endif
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/ds1307.inl
