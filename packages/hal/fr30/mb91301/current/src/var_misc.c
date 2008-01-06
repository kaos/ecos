//==========================================================================
//
//      var_misc.c
//
//      HAL implementation miscellaneous functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    larsi
// Contributors:
// Date:         2007-07-09
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/hal_misc.h>
/*------------------------------------------------------------------------*/
// Array which stores the configured priority levels for the configured
// interrupts.

/* this may be useful later when interrupt masking of internal interrupt
   sources is implemented

volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];
*/
/*------------------------------------------------------------------------*/

void hal_variant_init(void)
{
}

//--------------------------------------------------------------------------
// Microsecond delay
// This uses reload timer 2, because timer 0 and 1 can cause DMA transfers
// and may be used by the application.
// Timer is initialized with 32 prescaler. If we need more precise delay
// this has to change.

void hal_delay_us(cyg_int32 n){
#define TIMER_TIME  CYGHWR_HAL_FR30_MB91301_SYSTEM_CLOCK_MHZ * 1000000 / CYGHWR_HAL_FR30_MB91301_CLKP_DIVIDER / 32

    unsigned int calc(unsigned long long n){
        return n * TIMER_TIME / 1000000;
    }

    cyg_uint16  timer_status;

    n = 21;//calc(n);
    // stop eventually running counter and initialize
    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x812);

    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMRLR, 0xFFFF);
    while(n > 0xffff){
        // start counting
        HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x813);
        n = n - 0xffff;
        // look for underflow
        do {
            HAL_READ_UINT16(CYG_HAL_FR30_DLY_TMCSR, timer_status);
        } while (!(timer_status & BIT2));
        // clear underflow bit
        HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x813);
    }
    // clear count enable bit
    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x810);
    // set new remaining count value
    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMRLR, n);
    // start counting
    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x813);
    // look for underflow
    do {
        HAL_READ_UINT16(CYG_HAL_FR30_DLY_TMCSR, timer_status);
    } while (!(timer_status & BIT2));
    // clear underflow and count enable bits
    HAL_WRITE_UINT16(CYG_HAL_FR30_DLY_TMCSR, 0x810);
}

/*------------------------------------------------------------------------*/
/* End of var_misc.c                                                      */
