/*==========================================================================
//
//      at91sam7s_misc.c
//
//      HAL misc board support code for Atmel AT91sam7s
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// Copyright (C) 2006 eCosCentric Ltd
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
// Author(s):    gthomas
// Contributors: gthomas, jskov, nickg, tkoeller, Oliver Munz, Andrew Lunn
// Date:         2001-07-12
// Purpose:      HAL board support
// Description:  Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // necessary?
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_if.h>             // calling interface
#include <cyg/hal/hal_misc.h>           // helper functions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif

extern void hal_at91_led(int val);

void
hal_at91_set_leds (int val)
{
    hal_at91_led(val);
}

// -------------------------------------------------------------------------
// Hardware init

void hal_plf_hardware_init (void) 
{
  /* Enable the Serial devices to driver the serial port pins */
  HAL_ARM_AT91_PIO_CFG(AT91_USART_RXD0);
  HAL_ARM_AT91_PIO_CFG(AT91_USART_TXD0);
  HAL_ARM_AT91_PIO_CFG(AT91_DBG_DTXD);
  HAL_ARM_AT91_PIO_CFG(AT91_DBG_DRXD);

#if !defined(CYGHWR_HAL_ARM_AT91SAM7S_at91sam7s32)
  /* Enable the Serial devices to driver the serial port pins */
  HAL_ARM_AT91_PIO_CFG(AT91_USART_RXD1);
  HAL_ARM_AT91_PIO_CFG(AT91_USART_TXD1);
#endif

  /* Setup the Reset controller. Allow user resets */
  HAL_WRITE_UINT32(AT91_RST+AT91_RST_RMR, 
                   AT91_RST_RMR_URSTEN | 
                   10 << 8 | 
                   AT91_RST_RMR_KEY);

#ifdef CYGBLD_HAL_ARM_AT91_SERIAL_UART
  /* Enable peripheral clocks for USART 0 and 1 if they are to be used */
  HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER, 
                   AT91_PMC_PCER_US0 |
                   AT91_PMC_PCER_US1);
#endif

#ifdef CYGBLD_HAL_ARM_AT91_TIMER_TC
  /* Enable peripheral clocks for TC 0 and 1 if they are to be used */
  HAL_WRITE_UINT32(AT91_PMC+AT91_PMC_PCER, 
                   AT91_PMC_PCER_TC0 |
                   AT91_PMC_PCER_TC2); 
#endif

#ifndef CYGPKG_IO_WATCHDOG
  /* Disable the watchdog. The eCos philosophy is that the watchdog is
     disabled unless the watchdog driver is used to enable it.
     Whoever if we disable it here we cannot re-enable it in the
     watchdog driver, hence the conditional compilation. */
  HAL_WRITE_UINT32(AT91_WDTC + AT91_WDTC_WDMR, AT91_WDTC_WDMR_DIS); 
#endif
}

// Calculate the baud value to be programmed into the serial port baud
// rate generators. This function will determine what the clock speed
// is that is driving the generator so it can be used in situations
// when the application dynamically changes the clock speed. 
cyg_uint32 
hal_at91_us_baud(cyg_uint32 baud_rate)
{
  cyg_uint32 val, pll;
  cyg_uint32 main_clock = 0;
  cyg_uint32 baud_value = 0;

  HAL_READ_UINT32((AT91_PMC+AT91_PMC_MCKR), val);
  switch (val & 0x03) {
    /* Slow clock */
    case AT91_PMC_MCKR_SLOW_CLK:
      main_clock = CYGNUM_HAL_ARM_AT91_SLOW_CLOCK;
      break;
      
      /* Main clock */
    case AT91_PMC_MCKR_MAIN_CLK:
      main_clock = CYGNUM_HAL_ARM_AT91_CLOCK_OSC_MAIN;
      break;
      /* PLL */
    case AT91_PMC_MCKR_PLL_CLK:
      HAL_READ_UINT32((AT91_PMC+AT91_PMC_PLLR), pll);
      main_clock = CYGNUM_HAL_ARM_AT91_CLOCK_OSC_MAIN * 
        (((pll & 0x7FF0000) >> 16) + 1) / (pll & 0xFF);
      break;
  }
  
  // Process prescale
  val = (val & 0x1C) >> 2;
  main_clock = main_clock >> val;

  /* Define the baud rate divisor register, (round) */
  baud_value = (main_clock/(8*baud_rate)+1)/2;
  
  return baud_value;
}

//--------------------------------------------------------------------------
// EOF at91sam7s_misc.c
