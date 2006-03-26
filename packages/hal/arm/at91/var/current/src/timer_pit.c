/*==========================================================================
//
//      timer_pit.c
//
//      HAL timer code using the Periodic Interval Timer
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
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
// Author(s):    asl, Oliver Munz
// Contributors: asl, Oliver Munz
// Date:         2006-02-12
// Purpose:      Clock support using the PIT
// Description:  
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_platform_ints.h>
// -------------------------------------------------------------------------
// Use system clock
void
hal_clock_initialize(cyg_uint32 period)
{
  cyg_uint32 sr;
  
  CYG_ASSERT(CYGNUM_HAL_INTERRUPT_RTC == CYGNUM_HAL_INTERRUPT_PITC,
             "Invalid timer interrupt");
  
  /* Set Period Interval timer and enable interrupt */
  HAL_WRITE_UINT32((AT91_PITC + AT91_PITC_PIMR), 
                   period |  
                   AT91_PITC_PIMR_PITEN |
                   AT91_PITC_PIMR_PITIEN);
  
  // Read the status register to clear any pending interrupt
  HAL_READ_UINT32(AT91_PITC + AT91_PITC_PISR, sr);
}

// This routine is called during a clock interrupt.
void
hal_clock_reset(cyg_uint32 vector, cyg_uint32 period)
{
  cyg_uint32 reg;
  cyg_uint32 pimr;
  
  CYG_ASSERT(period < 0xffffff, "Invalid HAL clock configuration");
  
  // Check that the PIT has the right period.
  HAL_READ_UINT32((AT91_PITC + AT91_PITC_PIMR), pimr);
  if ((pimr & 0xffffff) != (period - 1)){
    HAL_WRITE_UINT32((AT91_PITC + AT91_PITC_PIMR), 
                     (period - 1) |  
                     AT91_PITC_PIMR_PITEN |
                     AT91_PITC_PIMR_PITIEN);
  }

  /* Read the value register so that we clear the interrupt */
  HAL_READ_UINT32(AT91_PITC + AT91_PITC_PIVR, reg);
}

// Read the current value of the clock, returning the number of hardware
// "ticks" that have occurred (i.e. how far away the current value is from
// the start)
void
hal_clock_read(cyg_uint32 *pvalue)
{
  cyg_uint32 ir;
  cyg_uint32 pimr;
  
  // Check that the PIT is running. If not start it.
  HAL_READ_UINT32((AT91_PITC + AT91_PITC_PIMR),pimr);
  if (!(pimr & AT91_PITC_PIMR_PITEN)) {
    HAL_WRITE_UINT32((AT91_PITC + AT91_PITC_PIMR), 
                     0xffffff | AT91_PITC_PIMR_PITEN);
  }
  
  HAL_READ_UINT32(AT91_PITC + AT91_PITC_PIIR, ir);
  *pvalue = ir & 0xfffff;
}

// -------------------------------------------------------------------------
//
// Delay for some number of micro-seconds
// PIT is clocked at MCLK / 16
//
void hal_delay_us(cyg_int32 usecs)
{
  cyg_int64 ticks;
  cyg_uint32 val1, val2;
  cyg_uint32 piv;
  
  // Calculate how many PIT ticks the required number of microseconds
  // equate to. We do this calculation in 64 bit arithmetic to avoid
  // overflow.
  ticks = (((cyg_uint64)usecs) * 
           ((cyg_uint64)CYGNUM_HAL_ARM_AT91_CLOCK_SPEED))/16/1000000LL;
  
  // Calculate the wrap around period. 
  HAL_READ_UINT32(AT91_PITC + AT91_PITC_PIMR, piv);
  piv = (piv & 0xffffff) - 1; 
  
  hal_clock_read(&val1);
  while (ticks > 0) {
    hal_clock_read(&val2);
    if (val2 < val1)
      ticks -= ((piv + val2) - val1); //overflow occurred
    else 
      ticks -= (val2 - val1);
    val1 = val2;
  }
}

// timer_pit.c
