//==========================================================================
//
//      powerpc_pati_flash.c
//
//      Flash programming for INTEL device on PowerPC MPC555 MPL PATI
//      board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Author(s):    Steven Clugston
// Original:     jskov
// Contributors: 
// Date:         2008-06-18
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/plf_misc.h>

//--------------------------------------------------------------------------
// Device properties

// One 28F320C3T part used on the board
/// 32Mbit top boot block
#define CYGNUM_FLASH_INTERLEAVE	(1)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_WIDTH      (16)
#define CYGNUM_FLASH_BASE 	(CYGMEM_REGION_eflash)


//--------------------------------------------------------------------------
// Platform specific extras

// The programming voltage is switched on/off by setting a register bit in
// the on board EPLD.
#define CYGHWR_FLASH_WRITE_ENABLE()                      \
{cyg_uint32 epld_misc;                                   \
HAL_READ_UINT32(CYGPLF_REG_PLD_MISC_CONFIG, epld_misc);  \
epld_misc |= CYGPLF_REG_PLD_MISC_EXT_VPP;                \
HAL_WRITE_UINT32(CYGPLF_REG_PLD_MISC_CONFIG, epld_misc);}\

#define CYGHWR_FLASH_WRITE_DISABLE()                     \
{cyg_uint32 epld_misc;                                   \
HAL_READ_UINT32(CYGPLF_REG_PLD_MISC_CONFIG, epld_misc);  \
epld_misc &= ~(CYGPLF_REG_PLD_MISC_EXT_VPP);             \
HAL_WRITE_UINT32(CYGPLF_REG_PLD_MISC_CONFIG, epld_misc);}\

// Force a reset of the flash to keep driver happy
# define CYGHWR_FLASH_28FXXX_PLF_INIT()                  \
HAL_WRITE_UINT16(CYGNUM_FLASH_BASE,0xFF);                \
HAL_DELAY_US(10);                                        \

//--------------------------------------------------------------------------
// Now include the driver code.
#include "cyg/io/flash_28fxxx.inl"

// ------------------------------------------------------------------------
// EOF powerpc_pati_flash.c
