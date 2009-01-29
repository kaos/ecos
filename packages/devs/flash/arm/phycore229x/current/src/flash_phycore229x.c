//==========================================================================
//
//      flash_phycore229x.c
//
//      Flash programming for AMD device on Phytec phyCORE-LPC229x board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
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
// Author(s):    Uwe Kindler
// Contributors: jskov, nickg,jlarmour, block
// Date:         2007-11-21
// Purpose:      
// Description:  
//              
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal_arm_lpc2xxx_phycore229x.h>
#include <pkgconf/devs_flash_phycore229x.h>

// 
// There are pairs of
// 
#define CYGNUM_FLASH_INTERLEAVE (2)
#define CYGNUM_FLASH_WIDTH      (16)
#define CYGNUM_FLASH_SERIES     (CYGHWR_HAL_ARM_PHYCORE229X_FLASH_CNT >> 1)
#define CYGNUM_FLASH_BASE       (0x80000000u)

#include "cyg/io/flash_am29xxxxx.inl"

// ------------------------------------------------------------------------
// EOF phycore_flash.c
