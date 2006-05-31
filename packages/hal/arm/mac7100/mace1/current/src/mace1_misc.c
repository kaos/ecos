//==========================================================================
//
//      mace1_misc.c
//
//      HAL misc board support code for MACE1 board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2006 eCosCentric Ltd
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
// Author(s):    Ilija Koco <ilijak@siva.com.mk>
// Contributors: 
// Date:         2006-04-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/hal/hal_io.h>             // low level i/o
#include <cyg/hal/var_io.h>             // common registers
#include <cyg/hal/plf_io.h>             // platform registers
#include <cyg/hal/mace1_misc.h>         // mace1 misc functions

void
hal_mace1_set_leds(int val)
{
    HAL_WRITE_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), 
                     ((val<<8)&0xff00)); // turn all LEDs off
}

void
hal_mace1_led_on(int val)
{
    cyg_uint16 leds;

    // read old LED state
    HAL_READ_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), leds); 
    leds|=1<<(8+val);
    // Write new state
    HAL_WRITE_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), leds); 
}

void
hal_mace1_led_off(int val)
{
    cyg_uint16 leds;

    // read old LED state
    HAL_READ_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), leds); 
    leds&=~(1<<(8+val));
    // Write new state
    HAL_WRITE_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), leds); 
}

int
hal_mace1_get_leds(void)
{
    cyg_uint16 leds = 0;

    // read old LED state
    HAL_READ_UINT16(MAC7100_PIM_PORTDATA(MAC7100_PORT_A_OFFSET), leds); 
    return (leds>>8)&0xFF;
}

//--------------------------------------------------------------------------
// EOF mace1_misc.c
