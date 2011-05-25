#ifndef CYGONCE_DEVS_FLASH_SPI_AT25DFXXX_H
#define CYGONCE_DEVS_FLASH_SPI_AT25DFXXX_H

//=============================================================================
//
//      at25dfxxx.h
//
//      SPI flash driver for Atmel AT95DFxxx devices and compatibles.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009, 2011 Free Software Foundation, Inc.
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   ccoutand, updated for Atmel AT95DFxxx flash
// Original(s): Chris Holgate
// Purpose:     Atmel AT95DFxxx SPI flash driver implementation
//
//####DESCRIPTIONEND####
//
//=============================================================================

// Required data structures.
#include <cyg/io/flash_dev.h>

// Exported handle on the driver function table.
externC struct cyg_flash_dev_funs cyg_devs_flash_spi_at25dfxxx_funs;

//-----------------------------------------------------------------------------
// Macro used to generate a flash device object with the default AT25DFXXX
// settings.  Even though the block info data structure is declared here, the
// details are not filled in until the device type is inferred during
// initialization.  This also applies to the 'end' field which is calculated
// using the _start_ address and the inferred size of the device.
// _name_   is the root name of the instantiated data structures.
// _start_  is the base address of the device - for SPI based devices this can
//          have an arbitrary value, since the device is not memory mapped.
// _spidev_ is a pointer to a SPI device object of type cyg_spi_device.  This
//          is not typechecked during compilation so be careful!

#define CYG_DEVS_FLASH_SPI_AT25DFXXX_DRIVER(_name_, _start_, _spidev_)  \
struct cyg_flash_block_info _name_ ##_block_info;                       \
CYG_FLASH_DRIVER(_name_, &cyg_devs_flash_spi_at25dfxxx_funs, 0,         \
  _start_, _start_, 1, & _name_ ##_block_info, (void*) _spidev_)

//-----------------------------------------------------------------------------
#endif // CYGONCE_DEVS_FLASH_SPI_AT25DFXXX_H
// EOF atd25dfxxx.h
