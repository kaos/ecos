//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
// Author(s):   Steven Clugston
// Original:    jskov
// Date:        2008-05-08
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/plf_misc.h>
#include <cyg/hal/hal_io.h>             // IO macros

void
hal_plf_stub_init(void)
{

}

/*------------------------------------------------------------------------*/
/* Reset support                                                          */
#define CHIPRST       CYGPLF_REG_PLD_RESET
void cyg_hal_plf_reset(void)
{
    // Write to EPLD Reset Register asserts HRESET#
    // which resets the processor,
    // the EPLD register is not reset, only a power on reset 
    // will do this.
    HAL_WRITE_UINT32(CHIPRST, 0xFF);
    // Just in case.
    while (1) ;
}

//-----------------------------------------------------------------------------
// End of plf_stub.c
