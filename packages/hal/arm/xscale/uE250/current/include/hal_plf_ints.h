#ifndef CYGONCE_HAL_PLF_INTS_H
#define CYGONCE_HAL_PLF_INTS_H
//==========================================================================
//
//      hal_plf_ints.h
//
//      HAL Platform Interrupt support
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2003 Gary Thomas <gary@mind.be>
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
// Author(s):    msalter
// Contributors: msalter
// Date:         2001-12-03
// Purpose:      Define Interrupt support
// Description:  The interrupt details for a specific platform is defined here.
// Usage:
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// start with variant ints
#include <pkgconf/hal.h>
#include CYGBLD_HAL_VAR_INTS_H
#include <cyg/hal/plf_io.h>

//----------------------------------------------------------------------------
// Reset.
#undef  HAL_PLATFORM_RESET
#define HAL_PLATFORM_RESET()                                               \
    CYG_MACRO_START                                                        \
    cyg_uint32 ctrl;                                                       \
                                                                           \
    /* By disabling interupts we will just hang in the loop below      */  \
    /* if for some reason the software reset fails.                    */  \
    HAL_DISABLE_INTERRUPTS(ctrl);                                          \
                                                                           \
    PCICTL_MISC = PCI_SYSTEM_RESET;                                              \
                                                                           \
    for(;;); /* hang here forever if reset fails */                        \
    CYG_MACRO_END

// Fallback (never really used)
#define HAL_PLATFORM_RESET_ENTRY 0x00000000


#endif // CYGONCE_HAL_PLF_INTS_H
