/*==========================================================================
//
//      at91sam7sek_misc.c
//
//      HAL misc board support code for Atmel AT91sam7s EK board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 Andrew Lunn <andrew.lunn@ascom.ch>
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
// Author(s):    andrew lunn
// Contributors: Oliver Munz, Andrew Lunn
// Date:         2006-06-20
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/hal/hal_io.h>             // IO macros

// The development board has four LEDs
void 
hal_at91_led (int val)
{
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PA0, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PA1, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PA2, AT91_PIN_OUT);
  HAL_ARM_AT91_GPIO_CFG_DIRECTION(AT91_GPIO_PA3, AT91_PIN_OUT);
  
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PA0, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PA1, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PA2, AT91_PIN_PULLUP_DISABLE);
  HAL_ARM_AT91_GPIO_CFG_PULLUP(AT91_GPIO_PA3, AT91_PIN_PULLUP_DISABLE);

  // Set the bits. The logic is inverted 
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PA1, !(val & 1));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PA2, !(val & 2));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PA3, !(val & 4));
  HAL_ARM_AT91_GPIO_PUT(AT91_GPIO_PA4, !(val & 8));
}

//--------------------------------------------------------------------------
// EOF at91sam7sek_misc.c
