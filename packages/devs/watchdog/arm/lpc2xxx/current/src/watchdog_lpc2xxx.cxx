//==========================================================================
//
//      devs/watchdog/arm/lpc2xxx/watchdog_lpc2xxx.cxx
//
//      Watchdog implementation for ARM LPC2XXX CPU
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
// Contributors: tkoeller, nickg
// Date:         2004-10-05
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the Philips LPC2XXX watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/infra.h>
#include <pkgconf/kernel.h>
#include <pkgconf/watchdog.h>
#include <pkgconf/devs_watchdog_arm_lpc2xxx.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>

#include <cyg/io/watchdog.hxx>

#include <cyg/hal/hal_var_ints.h>
#if !defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT)
#include <cyg/kernel/intr.hxx>
#endif

//==========================================================================

#define TICKS (CYG_HAL_ARM_LPC2XXX_PCLK()/4000 * \
               CYGNUM_DEVS_WATCHDOG_ARM_LPC2XXX_DESIRED_TIMEOUT_MS)

#define RESOLUTION CYGNUM_DEVS_WATCHDOG_ARM_LPC2XXX_DESIRED_TIMEOUT_MS*1000000

//==========================================================================

#if defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT)

#define MODVAL	(CYGARC_HAL_LPC2XXX_REG_WDMOD_WDEN | \
                 CYGARC_HAL_LPC2XXX_REG_WDMOD_WDRESET)

void
Cyg_Watchdog::init_hw(void)
{
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();
  resolution = RESOLUTION;
  CYG_REPORT_RETURN();
}

#else /* defined(CYGSEM_WATCHDOG_RESETS_ON_TIMEOUT) */

//==========================================================================

#define MODVAL	(CYGARC_HAL_LPC2XXX_REG_WDMOD_WDEN | \
                 CYGARC_HAL_LPC2XXX_REG_WDMOD_WDINT)
#define INT_PRIO    7

//==========================================================================

static Cyg_Watchdog *wd;

//==========================================================================

static cyg_uint32
isr(cyg_vector vector, CYG_ADDRWORD data)
{
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARG2XV(vector, data);

  wd->trigger();
  Cyg_Interrupt::acknowledge_interrupt(CYGNUM_HAL_INTERRUPT_WD);
  CYG_REPORT_RETVAL(Cyg_Interrupt::HANDLED);
  return Cyg_Interrupt::HANDLED;
}

//==========================================================================

static Cyg_Interrupt wdint(
    CYGNUM_HAL_INTERRUPT_WD,
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
  resolution = RESOLUTION;
  wdint.configure_interrupt(CYGNUM_HAL_INTERRUPT_WD, false, true);
  wdint.attach();
  wdint.acknowledge_interrupt(CYGNUM_HAL_INTERRUPT_WD);
  wdint.unmask_interrupt(CYGNUM_HAL_INTERRUPT_WD);
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

  /* Feed magic values to reset the watchdog. */
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC1);
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC2);
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
  CYG_REPORT_FUNCTION();
  CYG_REPORT_FUNCARGVOID();
  
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDTC, TICKS);
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDMOD, MODVAL);
  /* Feed magic values to reset the watchdog. */
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC1);
  HAL_WRITE_UINT32(CYGARC_HAL_LPC2XXX_REG_WD_BASE + 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED, 
                   CYGARC_HAL_LPC2XXX_REG_WDFEED_MAGIC2);
  CYG_REPORT_RETURN();
}

//==========================================================================
// End of watchdog_lpc2xxx.cxx
