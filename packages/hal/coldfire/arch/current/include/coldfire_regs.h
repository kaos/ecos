#ifndef CYGONCE_HAL_CF_REGS_H
#define CYGONCE_HAL_CF_REGS_H

//==========================================================================
//
//      coldfire_regs.h
//
//      ColdFire CPU definitions
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
// Author(s):    Enrico Piria
// Contributors:
// Date:         2005-25-06
// Purpose:      Provide ColdFire register definitions.
// Usage:        #include <cyg/hal/coldfire_regs.h>
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/var_regs.h>


// Macro to embed movec instructions in C code
#define CYGARC_MOVEC(_value_, _reg_) \
    asm volatile("movec %0,%1" : : "d" (_value_), "i" (_reg_))


// ---------------------------------------------------------------------------
// End of coldfire_regs.h
#endif // ifdef CYGONCE_HAL_CF_REGS_H
