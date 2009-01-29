//==========================================================================
//
//      eb55_flash.c
//
//      Flash programming for Atmel device on Atmel EB55 board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003, 2005 Free Software Foundation, Inc.
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
// Author(s):    jskov, tdrury
// Contributors: jskov, nickg,jlarmour
// Date:         2002-07-08
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================


// There's a single AT49BV1604A on the EB55 board.

#define CYGHWR_DEVS_FLASH_ATMEL_AT49BV1604A
#define CYGNUM_FLASH_INTERLEAVE	(1)
#define CYGNUM_FLASH_WIDTH      (16)
#define CYGNUM_FLASH_SERIES	(1)
#define CYGNUM_FLASH_BASE 	(0x01000000u)

#include "cyg/io/flash_at49xxxx.inl"

// ------------------------------------------------------------------------
// There is also an AT45DB321B DataFlash on the SPI bus

#include <pkgconf/system.h>

#if defined(CYGPKG_IO_SPI) && defined(CYGPKG_DEVS_FLASH_ATMEL_DATAFLASH)

#include <cyg/io/spi.h>
#include <cyg/io/spi_at91.h>
#include <cyg/io/dataflash.h>

__externC cyg_spi_at91_device_t spi_dataflash_dev0;

CYG_DATAFLASH_FLASH_DRIVER( cyg_eb55_dataflash,
                            cyg_spi_dataflash_dev0,
                            0x08000000,
                            0,
                            16 );

#endif

// ------------------------------------------------------------------------
// EOF eb55_flash.c
