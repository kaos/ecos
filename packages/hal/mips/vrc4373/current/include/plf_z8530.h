#ifndef CYGONCE_HAL_PLF_Z8530_H
#define CYGONCE_HAL_PLF_Z8530_H

//=============================================================================
//
//      plf_z8530.h
//
//      Platform header for Z8530 support.
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:nickg
// Date:        1999-05-21
// Purpose:     Platform HAL Z8530 support.
// Usage:       #include <cyg/hal/plf_z8530.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

/*---------------------------------------------------------------------------*/
/* Zilog Z8530 access macros.                                                */

#if defined(CYGPKG_HAL_MIPS_LSBFIRST)
# define DUART_BASE      0xc1000000
#elif defined(CYGPKG_HAL_MIPS_MSBFIRST)
# define DUART_BASE      0xc1000003
#else
#error MIPS endianness not defined by configuration
#endif

// Address offsets for DUART channels
#define DUART_A         4
#define DUART_B         0

/* Require Delay between Zilog chip (Z8530 and Z8536) register access */

//#define ZDEL    66              /* 66 Instructions for 66Mhz */
#define ZDEL    200              /* 200 Instructions for 133Mhz */

#define DELAY_ZACC\
	{ register int N = ZDEL; while (--N > 0); }


#define HAL_DUART_READ_CR( _chan_, _reg_, _val_)        \
CYG_MACRO_START                                         \
    if( (_reg_) != 0 )                                  \
    {                                                   \
        DELAY_ZACC;                                     \
        HAL_WRITE_UINT8( DUART_BASE+(_chan_), _reg_);   \
    }                                                   \
    DELAY_ZACC;                                         \
    HAL_READ_UINT8( DUART_BASE+(_chan_), _val_ );       \
CYG_MACRO_END

#define HAL_DUART_WRITE_CR( _chan_, _reg_, _val_ )      \
CYG_MACRO_START                                         \
    if( (_reg_) != 0 )                                  \
    {                                                   \
        DELAY_ZACC;                                     \
        HAL_WRITE_UINT8( DUART_BASE+(_chan_), _reg_);   \
    }                                                   \
    DELAY_ZACC;                                         \
    HAL_WRITE_UINT8( DUART_BASE+(_chan_), _val_ );      \
CYG_MACRO_END

#define HAL_DUART_WRITE_TR( _chan_, _val_ )             \
CYG_MACRO_START                                         \
    DELAY_ZACC;                                         \
    HAL_WRITE_UINT8( DUART_BASE+(_chan_)+8, _val_ );    \
CYG_MACRO_END

#define HAL_DUART_READ_RR( _chan_, _val_ )              \
CYG_MACRO_START                                         \
    DELAY_ZACC;                                         \
    HAL_READ_UINT8( DUART_BASE+(_chan_)+8, _val_ );     \
CYG_MACRO_END

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_Z8530_H
// End of plf_z8530.h
