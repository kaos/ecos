/*==========================================================================
//
//      lpc_misc.c
//
//      HAL misc board support code for Olimex P2106 
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jani 
// Contributors: 
// Date:         2004-09-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/
#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>             // IO macros

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/var_io.h>

extern void cyg_hal_plf_serial_init(void);
// Low bit of mask determines the on/off status of the LED. 0 means ON
// and 1 is OFF!
void hal_lpc2xxx_set_leds(int mask)
{
    HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_IO_BASE + 
                     CYGARC_HAL_LPC2XXX_REG_IOSET, (1<<7));
    if (mask & 1)	
        HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_IO_BASE + 
                         CYGARC_HAL_LPC2XXX_REG_IOCLR, (1<<7));
}

void
cyg_hal_plf_comms_init(void)
{
    static int initialized = 0;
	
    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init();
}
//--------------------------------------------------------------------------
// EOF p2106_misc.c
