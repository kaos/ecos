#ifndef CYGONCE_HAL_DIAG_H
#define CYGONCE_HAL_DIAG_H

/*=============================================================================
//
//      hal_diag.h
//
//      MCFxxxx HAL Support for Kernel Diagnostic Routines
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2006,2008 Free Software Foundation, Inc.
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
// Author(s):    bartv
// Date:         2003-06-04
//
//####DESCRIPTIONEND####
//===========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k_mcfxxxx.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_if.h>

// On most ColdFire platforms the mcfxxxx variant HAL provides the HAL
// diagnostics support via a UART. The
// CYGINT_HAL_M68K_MCFxxxx_DIAGNOSTICS_USE_DEFAULT interface
// enables this.
#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG) && !defined(CYGSEM_HAL_VIRTUAL_VECTOR_CLAIM_COMMS)
#   include <cyg/hal/hal_if.h>
#   define HAL_DIAG_INIT()          hal_if_diag_init()
#   define HAL_DIAG_WRITE_CHAR(_c_) hal_if_diag_write_char(_c_)
#   define HAL_DIAG_READ_CHAR(_c_)  hal_if_diag_read_char(&_c_)

#elif !defined(CYGINT_HAL_M68K_MCFxxxx_DIAGNOSTICS_USE_DEFAULT)
// Allow the platform HAL to provide the diagnostics channel.
#  include <cyg/hal/plf_diag.h>
#else

externC void        hal_mcfxxxx_diag_uart_init(cyg_uint8*, cyg_uint32);
externC void        hal_mcfxxxx_diag_uart_putc(void*, char c);
externC cyg_uint8   hal_mcfxxxx_diag_uart_getc(void*);

# if defined(CYGHWR_HAL_M68K_MCFxxxx_DIAGNOSTICS_PORT_uart0)
#  define _HAL_MCFxxxx_DIAG_UART_BASE_    HAL_MCFxxxx_UART0_BASE
#  define _HAL_MCFxxxx_DIAG_UART_ISRVEC_  CYGNUM_HAL_ISR_UART0
# elif defined(CYGHWR_HAL_M68K_MCFxxxx_DIAGNOSTICS_PORT_uart1)
#  define _HAL_MCFxxxx_DIAG_UART_BASE_    HAL_MCFxxxx_UART1_BASE
#  define _HAL_MCFxxxx_DIAG_UART_ISRVEC_  CYGNUM_HAL_ISR_UART1
# elif defined(CYGHWR_HAL_M68K_MCFxxxx_DIAGNOSTICS_PORT_uart2)
#  define _HAL_MCFxxxx_DIAG_UART_BASE_    HAL_MCFxxxx_UART2_BASE
#  define _HAL_MCFxxxx_DIAG_UART_ISRVEC_  CYGNUM_HAL_ISR_UART2
# else
#  error Diagnostics channel UART base not defined.
# endif

// Even if the standard diagnostics facilities are enabled, allow the
// platform HAL to override things.
# ifndef HAL_DIAG_INIT
#  define HAL_DIAG_INIT()                                                           \
     CYG_MACRO_START                                                                \
     hal_mcfxxxx_diag_uart_init((cyg_uint8*) (_HAL_MCFxxxx_DIAG_UART_BASE_),        \
                             CYGNUM_HAL_M68K_MCFxxxx_DIAGNOSTICS_BAUD);             \
     CYG_MACRO_END
# endif

# ifndef HAL_DIAG_WRITE_CHAR
#  define HAL_DIAG_WRITE_CHAR(_c_)                                                  \
    CYG_MACRO_START                                                                 \
    hal_mcfxxxx_diag_uart_putc((void*)_HAL_MCFxxxx_DIAG_UART_BASE_, _c_);           \
    CYG_MACRO_END
# endif

# ifndef HAL_DIAG_READ_CHAR
#  define HAL_DIAG_READ_CHAR(_c_)                                                   \
    CYG_MACRO_START                                                                 \
    (_c_) = hal_mcfxxxx_diag_uart_getc((void*)(_HAL_MCFxxxx_DIAG_UART_BASE_));      \
    CYG_MACRO_END
# endif

#endif      // VV/MCFxxxx_DIAGNOSTICS

#endif /* CYGONCE_HAL_DIAG_H */
