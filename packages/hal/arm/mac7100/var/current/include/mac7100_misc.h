#ifndef CYGONCE_HAL_MAC7100_MISC_H
#define CYGONCE_HAL_MAC7100_MISC_H
//=============================================================================
//
//      mac7100_misc.h
//
//      Variant specific functions
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) eCosCentric Ltd
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk>
// Contributors:
// Date:        2006-02-03
// Purpose:     MAC7100 specific hal functions
// Description: 
// Usage:       #include <cyg/hal/mac7100_misc.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

// *********************************************************************
//
// ESCI Module
//
// *********************************************************************

externC void hal_mac7100_esci_pins(cyg_uint32 i_esci);

#endif // CYGONCE_HAL_MAC7100_MISC_H
// end of mac7100_misc.h
