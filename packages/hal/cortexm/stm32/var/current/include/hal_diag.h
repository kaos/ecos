#ifndef CYGONCE_HAL_DIAG_H
#define CYGONCE_HAL_DIAG_H
//=============================================================================
//
//      hal_diag.h
//
//      HAL diagnostics
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 eCosCentric Limited.
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
// Author(s):   nickg
// Date:        2008-07-30
// Purpose:     HAL diagnostics
// Description: 
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_if.h>

//-----------------------------------------------------------------------------

#if defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG)

#define HAL_DIAG_INIT()          hal_if_diag_init()
#define HAL_DIAG_WRITE_CHAR(_c_) hal_if_diag_write_char(_c_)
#define HAL_DIAG_READ_CHAR(_c_)  hal_if_diag_read_char(&_c_)

#else

__externC void hal_stm32_diag_init(void);
__externC void hal_stm32_diag_putc(char);
__externC cyg_uint8 hal_stm32_diag_getc(void);

# ifndef HAL_DIAG_INIT
#  define HAL_DIAG_INIT() hal_stm32_diag_init()
# endif

# ifndef HAL_DIAG_WRITE_CHAR
#  define HAL_DIAG_WRITE_CHAR(__c) hal_stm32_diag_putc(__c)
# endif

# ifndef HAL_DIAG_READ_CHAR
#  define HAL_DIAG_READ_CHAR(__c) (__c) = hal_stm32_diag_getc()
# endif

#endif


//-----------------------------------------------------------------------------
// end of hal_diag.h
#endif // CYGONCE_HAL_DIAG_H
