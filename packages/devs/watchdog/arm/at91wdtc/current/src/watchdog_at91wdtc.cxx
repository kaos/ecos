//==========================================================================
//
//      devs/watchdog/arm/at91/watchdog_at91wdtc.cxx
//
//      Watchdog implementation for ARM AT91 CPUs using the WDTC
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Nick Garnett <nickg@calivar.com>
// copyright (C) 2006 Andrew Lunn <andrew.lunn@ascom.ch>
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
// Author(s):    tkoeller
// Contributors: tkoeller, nickg, asl
// Date:         2006-02-18
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the ATMEL AT91 watchdog timer controller.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/infra.h>
#include <pkgconf/kernel.h>
#include <pkgconf/watchdog.h>
#include <pkgconf/devs_watchdog_arm_at91wdtc.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/io/watchdog.hxx>

#include <cyg/infra/diag.h>

#if !defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT)
#include <cyg/hal/hal_platform_ints.h>
#include <cyg/kernel/intr.hxx>
#endif

//==========================================================================

#if defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT)

#define MRVAL	(AT91_WDTC_WDMR_RSTEN | AT91_WDTC_WDMR_DBGHLT)

void
Cyg_Watchdog::init_hw(void)
{
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();
  resolution = CYGNUM_DEVS_WATCHDOG_ARM_AT91WDTC_DESIRED_TIMEOUT_MS * 1000000;
  CYG_REPORT_RETURN();
}

#else /* defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT) */

//==========================================================================

#define MRVAL	(AT91_WDTC_WDMR_FIEN | AT91_WDTC_WDMR_DBGHLT)
#define INT_PRIO    7

//==========================================================================

static Cyg_Watchdog *wd;

//==========================================================================

static cyg_uint32
isr(cyg_vector vector, CYG_ADDRWORD data)
{
  cyg_uint32 sr;
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARG2XV(vector, data);
  
  // Read the status register to clear the interrupt
  HAL_READ_UINT32(AT91_WDTC + AT91_WDTC_WDSR, sr);
  
  wd->trigger();
  Cyg_Interrupt::acknowledge_interrupt(CYGNUM_HAL_INTERRUPT_WDTC);
  CYG_REPORT_RETVAL(Cyg_Interrupt::HANDLED);
  return Cyg_Interrupt::HANDLED;
}

//==========================================================================

static CYGBLD_ATTRIB_INIT_PRI(CYG_INIT_DRIVERS)
  Cyg_Interrupt wdint(
                      CYGNUM_HAL_INTERRUPT_WDTC,
                      INT_PRIO,
                      0,
                      isr,
                      NULL
                      );

//==========================================================================

void
Cyg_Watchdog::init_hw(void)
{
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();
  
  wd = this;

  wdint.attach();
  wdint.acknowledge_interrupt(CYGNUM_HAL_INTERRUPT_WDTC);
  wdint.unmask_interrupt(CYGNUM_HAL_INTERRUPT_WDTC);
  resolution = CYGNUM_DEVS_WATCHDOG_ARM_AT91WDTC_DESIRED_TIMEOUT_MS * 1000000;
  CYG_REPORT_RETURN();
}

#endif	/* defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT) */

//==========================================================================
/*
 * Reset watchdog timer. This needs to be called regularly to prevent
 * the watchdog from firing.
 */

void
Cyg_Watchdog::reset(void)
{
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();

  /* Write magic code to reset the watchdog. */
  HAL_WRITE_UINT32(AT91_WDTC + AT91_WDTC_WDCR, 
                   AT91_WDTC_WDCR_RELOAD | AT91_WDTC_WDCR_KEY);
  
  CYG_REPORT_RETURN();
}

//==========================================================================
/*
 * Start watchdog to generate a hardware reset
 * or interrupt when expiring.
 */

void
Cyg_Watchdog::start(void)
{
  cyg_uint32 val, val1;

  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();

  val = (MRVAL | CYGNUM_DEVS_WATCHDOG_ARM_AT91WDTC_TIMEOUT_VALUE |
         (CYGNUM_DEVS_WATCHDOG_ARM_AT91WDTC_TIMEOUT_VALUE << 
          AT91_WDTC_WDMR_WDD_SHIFT));

  HAL_WRITE_UINT32(AT91_WDTC + AT91_WDTC_WDMR, val);
  HAL_READ_UINT32(AT91_WDTC + AT91_WDTC_WDMR, val1);

  // If this assert goes if it probably means something else has
  // already programmed the watchdog. The mode register is only
  // writeable once and once it is set it can only be reset by a
  // processor reset.
  CYG_ASSERT(val == val1, "Unable to configure watchdog");

  CYG_REPORT_RETURN();
}

//==========================================================================
// End of watchdog_at91.cxx
