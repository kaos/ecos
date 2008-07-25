#ifndef CYGONCE_HAL_VAR_ARCH_H
#define CYGONCE_HAL_VAR_ARCH_H
//=============================================================================
//
//      var_arch.h
//
//      LPC24XX variant architecture overrides
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003 Jonathan Larmour <jifl@eCosCentric.com>
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler 
// Contributors: jlarmour,Daniel Neri
// Date:         2008-07-06
// Purpose:      LPC24XX variant architecture overrides
// Description: 
// Usage:        #include <cyg/hal/hal_arch.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>
#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Idle thread code.
// This macro is called in the idle thread loop, and gives the HAL the
// chance to insert code. Typical idle thread behaviour might be to halt the
// processor. These implementations halt the system core clock.

#ifdef CYGHWR_HAL_ARM_LPC24XX_IDLE_PWRSAVE
#ifndef HAL_IDLE_THREAD_ACTION

#define HAL_IDLE_THREAD_ACTION(_count_)                       \
CYG_MACRO_START                                               \
HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE +            \
                 CYGARC_HAL_LPC24XX_REG_PCON,                 \
                 CYGARC_HAL_LPC24XX_REG_PCON_IDL);            \
CYG_MACRO_END

#endif		// HAL_IDLE_THREAD_ACTION
#endif		// CYGHWR_HAL_ARM_LPC24XX_IDLE_MODE

//-----------------------------------------------------------------------------
// end of var_arch.h
#endif // CYGONCE_HAL_VAR_ARCH_H
