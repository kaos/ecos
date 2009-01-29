//==========================================================================
//
//      arm_olpcx2294_flash.c
//
//      Flash programming for Intel FlashFile devices on Olimex LPC-X2294
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
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

// The Olimex LPC-E2294 eval. board, and Olimex LPC-H2294 header board have
// Intel 28F320C3 flash memory part, Olimex LPC-L2294-1M has Intel 28F160C3
// flash memory part.

#define CYGNUM_FLASH_INTERLEAVE (1)
#define CYGNUM_FLASH_SERIES     (1)
#define CYGNUM_FLASH_WIDTH      (16)
#define CYGNUM_FLASH_BASE       (0x80000000u)

//--------------------------------------------------------------------------
// Now include the driver code.
#include "cyg/io/flash_28fxxx.inl"

// indent: --indent-level4 -br -nut; vim: expandtab tabstop=4 shiftwidth=4
// ------------------------------------------------------------------------
// EOF arm_olpcx2294_flash.c
