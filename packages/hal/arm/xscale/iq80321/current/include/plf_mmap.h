#ifndef CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
#define CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
/*=============================================================================
//
//      plf_mmap.h
//
//      Platform specific memory map support
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         2001-01-04
// Purpose:      Intel SA11x0 series platform-specific memory mapping macros
// Description:  Macros to convert a cached, virtual address to
//               1) an uncached adddress for the same memory (if available)
//               2) the equivalent physical address for giving to external
//               hardware eg. DMA engines.
//               
//               NB: this mapping is expressed here statically, independent
//               of the actual mapping installed in the MMAP table.  So if
//               someone changes that, or its initialisation is changed,
//               then this module must change.  This is intended to be
//               efficient at a cost of generality.  It is also intended to
//               be called with constants (such as &my_struct) as input
//               args, so that all the work can be done at compile time,
//               with optimization on.
//
// Usage:        #include <cyg/hal/hal_cache.h>
//		 (which includes this file itself)
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/hal_misc.h>


//---------------------------------------------------------------------------
#endif // CYGONCE_HAL_SA11X0_PLATFORM_PLF_MMAP_H
// EOF plf_mmap.h
