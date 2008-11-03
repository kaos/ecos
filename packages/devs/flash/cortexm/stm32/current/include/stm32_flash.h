#ifndef CYGONCE_DEVS_FLASH_STM32_H
#define CYGONCE_DEVS_FLASH_STM32_H
//==========================================================================
//
//      stm32_flash.h
//
//      STM32 internal flash driver definitions
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-09-22
//
//####DESCRIPTIONEND####
//
//========================================================================*/

// The driver-specific data, pointed at by the priv field in a
// a cyg_flash_dev structure.

typedef struct cyg_stm32_dev
{
    cyg_flash_block_info_t      block_info[1];
    
} cyg_stm32_flash_dev;

//========================================================================*/
// Exported function pointers.

__externC const struct cyg_flash_dev_funs cyg_stm32_flash_funs;


//========================================================================*/
#endif // CYGONCE_DEVS_FLASH_STM32_H
// End
