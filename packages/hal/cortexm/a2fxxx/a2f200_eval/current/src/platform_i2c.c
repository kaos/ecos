//==========================================================================
//
//      platform_i2c.c
//
//      Optional I2C support for Cortex-M3 Actel Smartfusion
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
// Date:         2011-04-08
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
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_endian.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGPKG_DEVS_I2C_CORTEXM_A2FXXX

#include <cyg/io/i2c.h>
#include <cyg/io/i2c_a2fxxx.h>

//=============================================================================
// Setup I2C bus 0
//
static void
a2fxxx_i2c0_init( struct cyg_i2c_bus *bus )
{
    cyg_uint32 scl_io = CYGHWR_HAL_A2FXXX_I2C0_SCL;
    cyg_uint32 sda_io = CYGHWR_HAL_A2FXXX_I2C0_SDA;

    //
    // We only need to setup the pins here and
    // leave the I2C driver to take care of the rest.
    //
    CYGHWR_HAL_A2FXXX_GPIO_SET( scl_io );
    CYGHWR_HAL_A2FXXX_GPIO_SET( sda_io );

    a2fxxx_i2c_init( bus );
}


//-----------------------------------------------------------------------------
// I2C bus 0
//
CYG_A2FXXX_I2C_BUS(hal_a2fxxx_i2c0_bus,
                 &a2fxxx_i2c0_init,
                 CYGHWR_HAL_A2FXXX_I2C0,       // Base address
                 CYGHWR_HAL_A2FXXX_I2C0_BB,    // Bit-band base address
                 CYGHWR_HAL_A2FXXX_SC_CLR_SOFTRST_CR_I2C0,
                 CYGNUM_HAL_INTERRUPT_I2C0_0,
                 CYGNUM_DEVS_I2C_CORTEXM_A2FXXX_I2C0_ISR_PRIORITY);


//-----------------------------------------------------------------------------
// OLED
//
CYG_I2C_DEVICE(i2c_a2fxxx_oled,
               &hal_a2fxxx_i2c0_bus,
               0x3c,
               0,
               CYG_I2C_DEFAULT_DELAY);


#define DELAY 1

// Wrapper to OLED driver

externC cyg_uint32
a2fxxx_oled_write_first( cyg_uint8 byte )
{
    cyg_uint32      result;

    cyg_i2c_transaction_begin( &i2c_a2fxxx_oled );

    result = cyg_i2c_transaction_tx( &i2c_a2fxxx_oled,
                                     true, ( cyg_uint8 * )&byte, 1, false );

#ifdef CYGPKG_KERNEL
    cyg_thread_delay( DELAY );
#endif

    return result;
}

externC cyg_uint32
a2fxxx_oled_write_byte( cyg_uint8 byte )
{
    cyg_uint32      result;

    result = cyg_i2c_transaction_tx( &i2c_a2fxxx_oled,
                                     false, ( cyg_uint8 * )&byte, 1, false );

#ifdef CYGPKG_KERNEL
    cyg_thread_delay( DELAY );
#endif

    return result;
}


externC cyg_uint32
a2fxxx_oled_write_array( const cyg_uint8 *array, cyg_uint32 count )
{
    cyg_uint32      result;

    result = cyg_i2c_transaction_tx( &i2c_a2fxxx_oled,
                                     false, array, count, false );

    return result;
}


externC cyg_uint32
a2fxxx_oled_write_final( cyg_uint8 byte )
{
    cyg_uint32      result;

    result = cyg_i2c_transaction_tx( &i2c_a2fxxx_oled,
                                     false, ( cyg_uint8 * )&byte, 1, true );
#ifdef CYGPKG_KERNEL
    cyg_thread_delay( DELAY );
#endif

    cyg_i2c_transaction_end( &i2c_a2fxxx_oled );

    return result;
}

#endif // #ifdef CYGPKG_DEVS_I2C_CORTEXM_A2FXXX

//-----------------------------------------------------------------------------
// EOF platform_i2c.c
