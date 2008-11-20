//=============================================================================
//
//      platform.c
//
//      M5272C3 platform code
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2006,2008 Free Software Foundation, Inc.
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
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_m5272c3.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_if.h>

// ----------------------------------------------------------------------------
// Platform initialization. This is called early on from
// hal_m68k_startup(), once the C environment has been set up. We are
// running on the startup stack and interrupts are disabled. It is
// assumed that the memory map is already sane - either the ROM
// monitor or the assembler startup code should have taken care of
// that.
//
// For ROM/ROMFFE startup the assumption is that we have just come
// out of reset, either because of a hard reset or because a soft
// reset activated the watchdog. Hence we can assume all peripherals
// are at their default settings.

void
hal_m68k_m5272c3_init(void)
{
    // Start with the hardware. If this code is reached from a hard
    // reset, RedBoot or dBUG then most of the hardware should be in a
    // reasonable state. For a soft reset assume that the watchdog has
    // been used so again everything should be in a reasonable state.
    // Hence we only need to worry about registers where the default
    // settings are inappropriate.
    
    // After a power-up the ethernet phy needs a bit of time to negotiate
    // the link. Without this delay packets appear to go out but are
    // just lost, which is a problem for bootp.
#if defined(CYGPKG_DEVS_ETH_MCFxxxx) && (defined(CYG_HAL_STARTUP_ROM) || defined(CYG_HAL_STARTUP_ROMFFE))
    HAL_DELAY_US(10000);
#endif    
}
