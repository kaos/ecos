#ifndef CYGONCE_DEVS_FLASH_STM32_H
#define CYGONCE_DEVS_FLASH_STM32_H
//==========================================================================
//
//      stm32_flash.h
//
//      STM32 internal flash driver definitions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2011, 2012 Free Software Foundation, Inc.                        
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2008-09-22
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal_cortexm_stm32.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/flash.h>
#include <cyg/io/flash_dev.h>

#if defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_F1)
# define STM32_FLASH_MAXBLOCKINFOS 1
#elif defined(CYGHWR_HAL_CORTEXM_STM32_FAMILY_HIPERFORMANCE)
# define STM32_FLASH_MAXBLOCKINFOS 3
#endif

// The driver-specific data, pointed at by the priv field in a
// a cyg_flash_dev structure.

typedef struct cyg_stm32_dev
{
    cyg_flash_block_info_t      block_info[STM32_FLASH_MAXBLOCKINFOS];
} cyg_stm32_flash_dev;

// The instantiation of that data.
__externC const cyg_stm32_flash_dev hal_stm32_flash_priv;

//========================================================================*/
// Exported function pointers.

__externC const struct cyg_flash_dev_funs cyg_stm32_flash_funs;


//========================================================================*/
#endif // CYGONCE_DEVS_FLASH_STM32_H
// End of stm32_flash.h
