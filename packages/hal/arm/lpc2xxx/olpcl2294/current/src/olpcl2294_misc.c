//==========================================================================
//
//      olpcl2294_misc.c
//
//      HAL misc board support code for Olimex LPC-L2294-1MB development
//      board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2008 eCosCentric Limited
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:       HAL board support
// Description:   Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>     // IO macros
#include <cyg/hal/hal_arch.h>   // Register state info

#include <cyg/infra/cyg_type.h> // base types
#include <cyg/hal/var_io.h>

#include <cyg/hal/hal_var_ints.h>
#include <cyg/hal/drv_api.h>

// Newlib provides support for building the run-time elements of C++
// within the toolchain. Modern newlib stuff looks for a _impure_ptr
// entry.
void           *_impure_ptr;

extern void cyg_hal_plf_serial_init (void);

//--------------------------------------------------------------------------
// hal_lpc2xxx_set_leds --
//
// There is one diagnostic LED marked 'STAT' on the board
//
void
hal_lpc2xxx_set_leds (int mask)
{
    HAL_WRITE_UINT32 (CYGARC_HAL_LPC2XXX_REG_IO_BASE +
                      CYGARC_HAL_LPC2XXX_REG_IO1SET, (1 << 23));
    if (mask & 1)
        HAL_WRITE_UINT32 (CYGARC_HAL_LPC2XXX_REG_IO_BASE +
                          CYGARC_HAL_LPC2XXX_REG_IO1CLR, (1 << 23));
}

//--------------------------------------------------------------------------
// cyg_hal_plf_comms_init  --
//
void
cyg_hal_plf_comms_init (void)
{
    static int initialized = 0;

    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init ();
}

#ifdef HAL_PLF_HARDWARE_INIT
//--------------------------------------------------------------------------
// hal_plf_hardware_init --
//
void
hal_plf_hardware_init (void)
{
    // configure IRQ level from CL CS8900A
    cyg_drv_interrupt_configure (CYGNUM_HAL_INTERRUPT_EINT2, 0, 1);
}
#endif // HAL_PLF_HARDWARE_INIT

// indent: --indent-level4 -br -nut; vim: expandtab tabstop=4 shiftwidth=4
//--------------------------------------------------------------------------
// EOF olpcl2294_misc.c
