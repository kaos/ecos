#ifndef CYGONCE_HAL_PLF_IO_H
#define CYGONCE_HAL_PLF_IO_H
//=============================================================================
//
//      plf_io.h
//
//      Embedded Artists LPC2468 OEM board specific registers
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler
// Contributors: 
// Date:         2008-07-06
// Purpose:      EA LPC2468 oem board specific registers
// Description:
// Usage:        #include <cyg/hal/plf_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================
// On-chip device base addresses


// ----------------------------------------------------------------------------
// exported I2C devices on OEM board
//
#define HAL_I2C_EXPORTED_DEVICES                      \
    extern cyg_i2c_device i2c_cat24c256_eeprom;       \
    extern cyg_i2c_bus    hal_ea2468_i2c0_bus;
    

//----------------------------------------------------------------------
// The platform needs this initialization during the
// hal_hardware_init() function in the varient HAL.
#ifndef __ASSEMBLER__
extern void hal_plf_hardware_init(void);
#define HAL_PLF_HARDWARE_INIT() \
    hal_plf_hardware_init()

//-----------------------------------------------------------------------------
// LPX24xx variant specific initialisation of CAN channels
// This function configures the pin functions for CAN use
//-----------------------------------------------------------------------------            
#ifdef CYGPKG_DEVS_CAN_LPC2XXX
externC void hal_lpc_can_init(cyg_uint8 can_chan_no);            
#define HAL_LPC2XXX_INIT_CAN(_can_chan_no_) hal_lpc_can_init(_can_chan_no_)
#endif // CYGPKG_DEVS_CAN_LPC2XXX 


// ----------------------------------------------------------------------------
// I2C support. The LPC2468 OEM board supports up to 3 I2C busses. Only the
// I2C bus 0 is used on the board. The other I2C busses are available via
// the expansion connector. If only the first bus is used, the I2C driver
// will be optimized for a single I2C bus
#ifdef CYGPKG_DEVS_I2C_ARM_LPC2XXX
# define HAL_LPC2XXX_I2C_SINGLETON_BASE     CYGARC_HAL_LPC24XX_REG_I2C0_BASE
# define HAL_LPC2XXX_I2C_SINGLETON_ISRVEC   CYGNUM_HAL_INTERRUPT_I2C
# define HAL_LPC2XXX_I2C_SINGLETON_CLK      CYGNUM_HAL_ARM_LPC24XX_I2C0_CLK
# define HAL_LPC2XXX_I2C_SINGLETON_ISRPRI   CYGNUM_HAL_ARM_LPC24XX_I2C0_INT_PRIO
# define HAL_LPC2XXX_I2C_SINGLETON_BUS_FREQ CYGNUM_HAL_ARM_LPC24XX_I2C0_BUS_FREQ


//-----------------------------------------------------------------------------
// Write data to eeprom (page write)
// If the call is blocking (blocking == true) then the function will poll
// the device if it is busy writing. If it is non blocking then the function
// immediately return 0 if the device is busy
// Function returns number of bytes written to device
//-----------------------------------------------------------------------------
externC cyg_uint32 hal_lpc_eeprom_write(cyg_uint16 addr,
                                        cyg_uint8 *pdata,
                                        cyg_uint16 size,
                                        cyg_bool   blocking);


//-----------------------------------------------------------------------------
// Read data from eeprom (selcetive, sequencial read)
// If the call is blocking (blocking == true) then the function will poll
// the device if it is busy writing. If it is non blocking then the function
// immediately return 0 if the device is busy
// Function returns number of bytes read from device
//-----------------------------------------------------------------------------
externC cyg_uint32 hal_lpc_eeprom_read(cyg_uint16 addr,
                                       cyg_uint8 *pdata,
                                       cyg_uint16 size,
                                       cyg_bool   blocking);
#endif // CYGPKG_DEVS_I2C_ARM_LPC2XXX  
#endif  //__ASSEMBLER__ 

//-----------------------------------------------------------------------------
// end of plf_io.h
#endif // CYGONCE_HAL_PLF_IO_H

