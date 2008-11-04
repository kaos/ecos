//==========================================================================
//
//      platform_i2c.c
//
//      Optional I2C support for LPC2468
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler <uwe_kindler@web.de>
// Contributors: 
// Date:         2008-09-11
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


//=============================================================================
//                               INCLUDES
//=============================================================================
#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_endian.h>


#ifdef CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP
#include <cyg/io/i2c.h>
#include <cyg/io/i2c_lpc2xxx.h>
//=============================================================================
// Setup I2C bus 0
//=============================================================================
static void ea2468_i2c0_init(struct cyg_i2c_bus* bus)
{  
    //
    // The LPC24xx variant sets up the I2C0 clock and powers up the I2C0
    // on-chip peripheral. We only need to setup the pins here and 
    // leave the I2C driver to take care of the rest.
    //
    hal_set_pin_function(0, 27, 1); // SDA0
    hal_set_pin_function(0, 28, 1); // SCL0
    cyg_lpc2xxx_i2c_init(bus);
}


//-----------------------------------------------------------------------------
// I2C bus 0
// 
CYG_LPC2XXX_I2C_BUS(hal_ea2468_i2c0_bus,
                    &ea2468_i2c0_init,
                    CYGARC_HAL_LPC24XX_REG_I2C0_BASE,
                    CYGNUM_HAL_INTERRUPT_I2C,
                    CYGNUM_HAL_ARM_LPC24XX_I2C0_INT_PRIO,
                    CYGNUM_HAL_ARM_LPC24XX_I2C0_CLK,
                    CYGNUM_HAL_ARM_LPC24XX_I2C0_BUS_FREQ);
                   

//-----------------------------------------------------------------------------
// 256-Kb I2C CMOS Serial EEPROM
//      
CYG_I2C_DEVICE(i2c_cat24c256_eeprom, 
               &hal_ea2468_i2c0_bus, 
               0x50, 
               0, 
               CYG_I2C_DEFAULT_DELAY);
                 
               
//===========================================================================
// Check if device is busy - Acknowledge polling can be used to determine
// if the CAT24C256 is busy writing or is ready to accept commands.
// Polling is implemented by interrogating the device with a Selective Read
// command (see READ OPERATIONS). The CAT24C256 will not acknowledge the
// Slave address, as long as internal Write is in progress.
//===========================================================================
static bool cat24c256_is_busy(void)
{
    cyg_uint16 addr = 0;
    cyg_uint32 result;

    cyg_i2c_transaction_begin(&i2c_cat24c256_eeprom);
    result = cyg_i2c_transaction_tx(&i2c_cat24c256_eeprom,
                                    true, (cyg_uint8*)&addr, 2, true);
    cyg_i2c_transaction_end(&i2c_cat24c256_eeprom);

    return (result != 2);
}
 
               
//===========================================================================
// Write up to 64 bytes of data - one single write cycle
//===========================================================================
static cyg_uint32 cat24c256_do_write_cyle(cyg_uint16 addr,
                                          cyg_uint8 *pdata,
                                          cyg_uint8  size)
{
    cyg_uint32 result;

    cyg_i2c_transaction_begin(&i2c_cat24c256_eeprom);
    addr = CYG_CPU_TO_BE16(addr);
    result = cyg_i2c_transaction_tx(&i2c_cat24c256_eeprom,
                                    true, (cyg_uint8*)&addr, 2, false);
    if (result)
    {
        result = cyg_i2c_transaction_tx(&i2c_cat24c256_eeprom,
                                        false, pdata, size, true);
    }
    cyg_i2c_transaction_end(&i2c_cat24c256_eeprom);

    return result;
}


//===========================================================================
// Write data to eeprom (page write)
// If the call is blocking (blocking == true) then the function will poll
// the device if it is busy writing. If it is non blocking then the function
// immediately return 0 if the device is busy
//===========================================================================
cyg_uint32 hal_lpc_eeprom_write(cyg_uint16 addr,
                                cyg_uint8 *pdata,
                                cyg_uint16 size,
                                cyg_bool   blocking)
{
    cyg_uint32 result;
    cyg_uint16 wsize;
    cyg_uint16 datasize = size;
    
    CYG_ASSERT(addr < 0x8000, "Address out of range");
    CYG_ASSERT(pdata, "Invalid data pointer");
    CYG_ASSERT((size <= 0x8000) && (size > 0), "Invalid data size");
    CYG_ASSERT((addr + size) < 0x8000,
               "Attempt to write data beyond end of memory.");

   
    do
    { 
        if (cat24c256_is_busy())
        {
            if (blocking)
            {
                while (cat24c256_is_busy()) {};
            }
            else
            {
                return 0;
            }
        } // if (cat24c256_is_busy())
        wsize = (datasize > 64) ? 64 : datasize;
        result = cat24c256_do_write_cyle(addr, pdata, wsize);
        datasize  -= result;
        addr      += wsize;
        pdata     += wsize;
    } while (datasize && (result == wsize));

    return size - datasize;
}


//===========================================================================
// Read data from eeprom (selective, sequential read)
// If the call is blocking (blocking == true) then the function will poll
// the device if it is busy writing. If it is non blocking then the function
// immediately return 0 if the device is busy
//===========================================================================
cyg_uint32 hal_lpc_eeprom_read(cyg_uint16 addr,
                               cyg_uint8 *pdata,
                               cyg_uint16 size,
                               cyg_bool   blocking)
{
    cyg_uint32 result;

    CYG_ASSERT(addr < 0x8000, "Address out of range");
    CYG_ASSERT(pdata, "Invalid data pointer");
    CYG_ASSERT((size <= 64) && (size > 0), "Invalid data size");
    CYG_ASSERT((addr + size) < 0x8000,
               "Attempt to read data beyond end of memory.");

    if (cat24c256_is_busy())
    {
        if (blocking)
        {
            while (cat24c256_is_busy());
        }
        else
        {
            return 0;
        }
    }

    cyg_i2c_transaction_begin(&i2c_cat24c256_eeprom);
    addr = CYG_CPU_TO_BE16(addr);
    result = cyg_i2c_transaction_tx(&i2c_cat24c256_eeprom,
                                    true, (cyg_uint8*)&addr, 2, false);
    if (result)
    {
        result = cyg_i2c_transaction_rx(&i2c_cat24c256_eeprom,
                                        true, pdata, size, true, true);
    }
    cyg_i2c_transaction_end(&i2c_cat24c256_eeprom);

    return result;
}
#endif // #ifdef CYGHWR_HAL_ARM_LPC24XX_I2C0_SUPP               

//-----------------------------------------------------------------------------
// EOF platform_i2c.c

