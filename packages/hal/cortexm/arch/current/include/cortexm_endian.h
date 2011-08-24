#ifndef CYGONCE_CORTEXM_ENDIAN_H
#define CYGONCE_CORTEXM_ENDIAN_H
//==========================================================================
//
//      cortexm_endian.h
//
//      Cortex-M architecture endian conversion macros/functions.
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
// Author(s):    Ilija Kocho <ilijak@siva.com.mk>
//               Sergei Gavrikov
// Date:         2011-08-20
// Description:  Endian conversion macros/functions optimized for Cortex-M
// Usage:        #include <cyg/hal/cortexm_endian.h>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/cortexm_regs.h>

//===========================================================================
// Endian operations optimized for Cortex-M architecture.

static __inline__ cyg_uint32 cyg_hal_swap32(cyg_uint32 original)
{
    cyg_uint32 swapped;
    CYGARC_REV(swapped, original);
    return swapped;
}

static __inline__ cyg_uint16 cyg_hal_swap16(cyg_uint16 original)
{
    cyg_uint16 swapped;
    CYGARC_REV16(swapped, original);
    return swapped;
}

#define CYG_SWAP32(__val) cyg_hal_swap32(__val)
#define CYG_SWAP16(__val) cyg_hal_swap16(__val)

//==========================================================================
#endif //CYGONCE_CORTEXM_ENDIAN_H
