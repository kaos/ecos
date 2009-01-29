/*==========================================================================
//
//      phycore_misc.c
//
//      HAL misc board support code for Phytec phyCORE-LPC2292/94
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler 
// Contributors: Uwe Kindler
// Date:         2007-11-20
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/
#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_lpc2xxx_phycore229x.h>
#include <cyg/hal/hal_io.h>             // IO macros

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>
#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#ifdef CYGPKG_REDBOOT
#include <redboot.h>
#endif
 
extern void cyg_hal_plf_serial_init(void);

void cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;
	
    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init();
}

//--------------------------------------------------------------------------
// hal_plf_hardware_init
//
void hal_plf_hardware_init(void)
{
#if defined(CYG_HAL_STARTUP_ROM) && defined(CYGPKG_DEVS_ETH_ARM_PHYCORE229X)
    cyg_uint32 regval; 
    
    //
    // Configures the LAN IRQ
    // The interrupt is being used as active high edge triggered.
    // IMPORTANT: We execute this step only for ROM startup. If this is done
    // for RAM startup then wrong values are stored in EXTMODE and EXTPOLAR
    // register because of a bug in LPC229x hardware.
    //
    HAL_INTERRUPT_CONFIGURE(CYGHWR_HAL_ARM_PHYCORE229X_ETH_EINT + // the configured external interrupt
                            CYGNUM_HAL_INTERRUPT_EINT0,           // the first external interrupt
                            0,                                    // level = 0 - edge triggered
                            1);                                   // up = 1 - rising edge
    
    //
    // Set pin function of P0.16 to EINT0 or P0.14 to EINT1 for ethernet interrupt
    //
#if CYGHWR_HAL_ARM_PHYCORE229X_ETH_EINT == 0
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_PIN_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_PINSEL1, regval);
    regval |= 0x01;
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_PIN_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_PINSEL1, regval);
#elif CYGHWR_HAL_ARM_PHYCORE229X_ETH_EINT == 1
    HAL_READ_UINT32(CYGARC_HAL_LPC2XXX_REG_PIN_BASE + 
                    CYGARC_HAL_LPC2XXX_REG_PINSEL0, regval);
    regval |= (0x10 << 28);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_PIN_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_PINSEL0, regval);
#else
#error "Invalid CYGHWR_HAL_ARM_PHYCORE229X_ETH_EINT value"
#endif
#endif // #if defined(CYG_HAL_STARTUP_ROM) && defined(CYGPKG_DEVS_ETH_ARM_PHYCORE229X)
}

//--------------------------------------------------------------------------
// hal_lpc2xxx_set_leds
//
void hal_lpc2xxx_set_leds (int mask)
{
    //
    // implement function for setting diagnostic leds
    //
}

//--------------------------------------------------------------------------
// EOF phycore_misc.c







