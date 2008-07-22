#ifndef CYGONCE_HAL_ARM_LPC2XXX_VAR_LPC2XXX_MISC_H
#define CYGONCE_HAL_ARM_LPC2XXX_VAR_LPC2XXX_MISC_H
//=============================================================================
//
//      lpc2xxx_misc.h
//
//      HAL misc variant support code for Philips LPC2xxx header file
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric Limited 
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
//#####DESCRIPTIONBEGIN####
//
// Author(s):    andyj 
// Contributors: jani
// Date:         2006-02-04
// Purpose:      LPC2XXX specific miscellaneous support header file
// Description: 
// Usage:        #include <cyg/hal/lpc2xxx_misc.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//-----------------------------------------------------------------------------
// Macros to derive the baudrate divider values for the internal UARTs
//-----------------------------------------------------------------------------
#define CYG_HAL_ARM_LPC2XXX_BAUD_GENERATOR(baud) \
            (CYGNUM_HAL_ARM_LPC2XXX_PCLK/((baud)*16))
            
//-----------------------------------------------------------------------------
// LPX2xxx varaint specific initialisatio of CAN channels
// This function configures the pin functions for CAN use
//-----------------------------------------------------------------------------            
#ifdef CYGPKG_DEVS_CAN_LPC2XXX
externC void hal_lpc_can_init(cyg_uint8 can_chan_no);            
#define HAL_LPC2XXX_INIT_CAN(_can_chan_no_) hal_lpc_can_init(_can_chan_no_)
#define CYGNUM_HAL_ARM_LPC2XXX_CAN_CLK CYGNUM_HAL_ARM_LPC2XXX_PCLK
#endif // CYGPKG_DEVS_CAN_LPC2XXX

//-----------------------------------------------------------------------------
// LPX2xxx watchdog support
//-----------------------------------------------------------------------------
externC void hal_lpc_watchdog_reset(void);

#define HAL_PLATFORM_RESET() hal_lpc_watchdog_reset()
#define HAL_PLATFORM_RESET_ENTRY 0

//-----------------------------------------------------------------------------
// end of lpc2xxx_misc.h
#endif // CYGONCE_HAL_ARM_LPC2XXX_VAR_LPC2XXX_MISC_H
