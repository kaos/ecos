#ifndef CYGONCE_HAL_HAL_ENDIAN_H
#define CYGONCE_HAL_HAL_ENDIAN_H

//=============================================================================
//
//      hal_endian.h
//
//      Endian conversion macros
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.1 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://www.redhat.com/                                                   
//                                                                          
// Software distributed under the License is distributed on an "AS IS"      
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
// License for the specific language governing rights and limitations under 
// the License.                                                             
//                                                                          
// The Original Code is eCos - Embedded Configurable Operating System,      
// released September 30, 1998.                                             
//                                                                          
// The Initial Developer of the Original Code is Red Hat.                   
// Portions created by Red Hat are                                          
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        2001-10-04
// Purpose:     Endian conversion macros
// Usage:       #include <cyg/hal/hal_endian.h>
//                           
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>                // CYGBLD_HAL_ENDIAN_H
#include <cyg/infra/cyg_type.h>         // endian setting

// Allow HALs to override the generic implementation of swap macros
#ifdef CYGBLD_HAL_ENDIAN_H
# include CYGBLD_HAL_ENDIAN_H
#endif

#ifndef CYG_SWAP16
# define CYG_SWAP16(_x_)                                        \
    ({ cyg_uint16 _x = (_x_); ((_x << 8) | (_x >> 8)); })
#endif

#ifndef CYG_SWAP32
# define CYG_SWAP32(_x_)                        \
    ({ cyg_uint32 _x = (_x_);                   \
       ((_x << 24) |                            \
       ((0x0000FF00UL & _x) <<  8) |            \
       ((0x00FF0000UL & _x) >>  8) |            \
       (_x  >> 24)); })
#endif


#if (CYG_BYTEORDER == CYG_LSBFIRST)
# define CYG_CPU_TO_BE16(_x_) CYG_SWAP16((_x_))
# define CYG_CPU_TO_BE32(_x_) CYG_SWAP32((_x_))
# define CYG_BE16_TO_CPU(_x_) CYG_SWAP16((_x_))
# define CYG_BE32_TO_CPU(_x_) CYG_SWAP32((_x_))

# define CYG_CPU_TO_LE16(_x_) (_x_)
# define CYG_CPU_TO_LE32(_x_) (_x_)
# define CYG_LE16_TO_CPU(_x_) (_x_)
# define CYG_LE32_TO_CPU(_x_) (_x_)

#elif (CYG_BYTEORDER == CYG_MSBFIRST)

# define CYG_CPU_TO_BE16(_x_) (_x_)
# define CYG_CPU_TO_BE32(_x_) (_x_)
# define CYG_BE16_TO_CPU(_x_) (_x_)
# define CYG_BE32_TO_CPU(_x_) (_x_)

# define CYG_CPU_TO_LE16(_x_) CYG_SWAP16((_x_))
# define CYG_CPU_TO_LE32(_x_) CYG_SWAP32((_x_))
# define CYG_LE16_TO_CPU(_x_) CYG_SWAP16((_x_))
# define CYG_LE32_TO_CPU(_x_) CYG_SWAP32((_x_))

#else

# error "Endian mode not selected"

#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_HAL_ENDIAN_H
// End of hal_endian.h
