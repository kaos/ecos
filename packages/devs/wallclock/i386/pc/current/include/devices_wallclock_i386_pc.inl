//==========================================================================
//
//      devs/wallclock/i386/pc/include/devs_wallclock_i386_pc.inl
//
//      PC RTC IO definitions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
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
// Author(s):   rajt
// Contributors:rajt
// Date:        2001-07-19
// Purpose:     PC RTC definitions for using DS12887
//####DESCRIPTIONEND####
//==========================================================================

#include CYGDAT_DEVS_WALLCLOCK_i386_PC_CFG

#define DS_ADDR CYGDAT_DEVS_WALLCLOCK_I386_PC_RTC_ADDRESS_PORT
#define DS_DATA    CYGDAT_DEVS_WALLCLOCK_I386_PC_RTC_DATA_PORT

#ifndef DS_LINEAR
#define DS_READ(offset, data)                               \
CYG_MACRO_START                                             \
    register CYG_BYTE btval;                                \
    DS_READ_UINT8( DS_ADDR, (btval));                       \
    DS_WRITE_UINT8(DS_ADDR, ((offset)&0x7F)|(btval&0x80));  \
    DS_READ_UINT8( DS_DATA, (data));                        \
CYG_MACRO_END
#define DS_WRITE(offset, data)                              \
CYG_MACRO_START                                             \
    register CYG_BYTE btval;                                \
    DS_READ_UINT8( DS_ADDR, (btval));                       \
    DS_WRITE_UINT8(DS_ADDR, ((offset)&0x7F)|(btval&0x80));  \
    DS_WRITE_UINT8(DS_DATA, (data));                        \
CYG_MACRO_END
#endif


// EOF devs_wallclock_i386_pc.inl
