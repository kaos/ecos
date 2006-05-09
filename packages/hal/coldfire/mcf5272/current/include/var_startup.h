#ifndef CYGONCE_VAR_STARTUP_H
#define CYGONCE_VAR_STARTUP_H

//=============================================================================
//
//      var_startup.h
//
//      ColdFire MCF5272 CPU variant startup header
//
//=============================================================================
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
// Purpose:      ColdFire MCF5272 CPU variant startup header.
// Description:
// Usage:        Included via "hal_startup.h". Do not use directly.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/cyg_type.h>

// Include the platform-specific startup header.
#include <cyg/hal/plf_startup.h>

// Variant specific hardware initialization routine
externC void var_reset(void) __attribute__ ((section (".boot")));

// Variant specific data initialization routine
externC void var_init_data(void) __attribute__ ((section (".boot")));

// ---------------------------------------------------------------------------
// End of var_startup.h
#endif // CYGONCE_VAR_STARTUP_H
