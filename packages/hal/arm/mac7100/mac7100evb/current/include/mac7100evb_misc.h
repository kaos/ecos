#ifndef CYGONCE_HAL_MAC7100EVB_MISC_H
#define CYGONCE_HAL_MAC7100EVB_MISC_H
//=============================================================================
//
//      mac7100evb_misc.h
//
//      MAC7100EVB board specific functions
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors: 
// Date:        2006-05-07
// Purpose:     MAC7100EVB board specific registers
// Description:
// Usage:       #include <cyg/hal/mac7100evb_misc.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

void hal_mac7100evb_set_leds(int);
void hal_mac7100evb_led_on(int);
void hal_mac7100evb_led_off(int);
int  hal_mac7100evb_get_leds(void);

#define HAL_MAC7100_SET_LEDS(x) hal_mac7100evb_set_leds(x)
#define HAL_MAC7100_LED_ON(x)   hal_mac7100evb_led_on(x)
#define HAL_MAC7100_LED_OFF(x)  hal_mac7100evb_led_off(x)
#define HAL_MAC7100_GET_LEDS(x) hal_mac7100evb_get_leds(x)

//-----------------------------------------------------------------------------
// end of mac7100evb_misc.h
#endif // CYGONCE_HAL_MAC7100EVB_MISC_H
