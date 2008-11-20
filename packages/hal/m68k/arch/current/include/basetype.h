#ifndef CYGONCE_HAL_BASETYPE_H
#define CYGONCE_HAL_BASETYPE_H

//=============================================================================
//
//      basetype.h
//
//      Standard types for this architecture.
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2008 Free Software Foundation, Inc.
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
//####DESCRIPTIONBEGIN####
//
// Author(s): 	bartv
// Date:	2003-06-04
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/hal_m68k.h>
#ifdef CYGINT_HAL_M68K_VARIANT_TYPES
# include <cyg/hal/var_basetype.h>
#endif

//-----------------------------------------------------------------------------
// Characterize the architecture: big-endian
#ifndef CYG_BYTEORDER
# define CYG_BYTEORDER   CYG_MSBFIRST
#endif

//-----------------------------------------------------------------------------
// 68k does not usually use labels with underscores. 
#define CYG_LABEL_NAME(_name_) _name_

// ----------------------------------------------------------------------------
// The 68k architecture uses the default definitions of the base types,
// so we do not need to define any here.

//-----------------------------------------------------------------------------
// Override the alignment definitions from cyg_type.h.
// Most 68k variants use a 4 byte alignment.
// A few variants (68000) may override this setting to use a 2 byte alignment.
#ifndef CYGARC_ALIGNMENT
# define CYGARC_ALIGNMENT 4
#endif
// the corresponding power of two alignment
#ifndef CYGARC_P2ALIGNMENT
# define CYGARC_P2ALIGNMENT 2
#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_BASETYPE_H
