//==========================================================================
//
//      var_startup.c
//
//      Functions for the processor variant startup
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
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
// Author(s):     Enrico Piria
// Contributors:
// Date:          2005-25-06
// Purpose:       Functions needed for MCF5272 startup.
// Description:   This module contains code that sets up the variant specific
//                hardware and data. All the code must be contained in the
//                section called ".boot", in order for the ROMRAM startup
//                to work properly.
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_startup.h>


// Variant specific initialization routine.
// This routine must be called with interrupts disabled.
void var_reset(void)
{
    // Nothing to do
}

// Variant specific data initialization routine
void var_init_data(void)
{
    int i;

    // Initialize priority and mask tables
    for(i = 0; i < CYGNUM_HAL_ISR_COUNT; i++)
    {
        cyg_hal_IMASK_table[i] = 0;
        cyg_hal_ILVL_table[i] = 0;
    }
}
