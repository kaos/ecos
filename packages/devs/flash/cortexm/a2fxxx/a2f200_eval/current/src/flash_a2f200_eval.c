//==========================================================================
//
//      flash_a2f200_eval.c
//
//      Flash programming for Atmel device on Actel SmartFusion Board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2011 Free Software Foundation, Inc.
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
// Author(s):    ccoutand
// Date:         2011-05-05
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

// ------------------------------------------------------------------------
// There is an AT25DF161 DataFlash on the SPI bus

#include <pkgconf/system.h>
#include <pkgconf/devs_flash_cortexm_a2f200_eval.h>

#if defined(CYGPKG_IO_SPI) && defined(CYGPKG_DEVS_FLASH_SPI_AT25DFXXX)

#include <cyg/io/spi.h>
#include <cyg/io/at25dfxxx.h>
#include <cyg/io/spi_a2fxxx.h>

// Declare Device on SPI bus 1, Use GPIO 19 for chip select, Motorola protocol (mode 0)
CYG_DEVS_SPI_CORTEXM_A2FXXX_DEVICE (
    at25dfxxx_spi_device, 1, 19, true, A2FXXX_SPI_MOTOROLA, 0, 0, 5000000, 1, 1, 1
);

//-----------------------------------------------------------------------------
// Instantiate the AT25DFxxx device driver.

CYG_DEVS_FLASH_SPI_AT25DFXXX_DRIVER (
    at25dfxxx_flash_device, CYGNUM_DEVS_FLASH_SPI_AT25XXX_DEV0_MAP_ADDR, &at25dfxxx_spi_device
);

#endif

// ------------------------------------------------------------------------
// EOF flash_a2f200_eval.c
