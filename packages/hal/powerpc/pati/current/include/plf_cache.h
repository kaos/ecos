#ifndef CYGONCE_PLF_CACHE_H
#define CYGONCE_PLF_CACHE_H

//=============================================================================
//
//      plf_cache.h
//
//      Platform HAL cache details
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Steven Clugston
// Original:    Bob Koninckx
// Date:        2008-05-08
// Purpose:     Platform cache control API
// Description: The macros defined here provide the platform specific
//              cache control operations / behavior.
// Usage:       Is included via the architecture cache header:
//              #include <cyg/hal/hal_cache.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

//---------------------------------------------------------------------------
// Initial cache enabling
// MPC555 does not have any cache, disable caches completely
#undef CYGPRI_INIT_CACHES
#undef CYGPRI_ENABLE_CACHES

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_PLF_CACHE_H
// End of plf_cache.h
