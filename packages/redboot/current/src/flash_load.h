//==========================================================================
//
//      flash_load.h
//
//      Interfaces to byte writing into FLASH during load
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2006 eCosCentric Ltd
// Copyright (C) 2006 Andrew Lunn <andrew.lunn@ascom.ch>
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
// Author(s):    Oliver Munz
// Contributors: om, asl
// Date:         2005-02-21
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#ifndef FLASH_LOAD_H
#define FLASH_LOAd_H

// Prepare a write to flash.
void flash_load_start(void);

// Finish a write to flash
void flash_load_finish(void);

// Write a single byte. This will be buffered until either a full page
// is available or flash_write_finish is called.
void flash_load_write(cyg_uint8 *addr, cyg_uint8 value);

#endif // FLASH_LOAD_H

