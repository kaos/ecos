#ifndef CYGONCE_HAL_HAL_IO_H
#define CYGONCE_HAL_HAL_IO_H

//=============================================================================
//
//      hal_io.h
//
//      HAL device IO register support.
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
// Date:        1998-02-17
// Purpose:     Define IO register support
// Description: The macros defined here provide the HAL APIs for handling
//              device IO control registers.
//              
// Usage:
//              #include <cyg/hal/hal_io.h>
//              ...
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/plf_io.h>


//-----------------------------------------------------------------------------
// IO Register address.
// This type is for recording the address of an IO register.

typedef volatile CYG_ADDRWORD HAL_IO_REGISTER;

//-----------------------------------------------------------------------------
// BYTE Register access.
// Individual and vectorized access to 8 bit registers.

#define HAL_READ_UINT8( _register_, _value_ )   \
CYG_MACRO_START                                 \
{                                               \
    asm volatile ( "xor %%eax,%%eax ;"          \
                   "inb %%dx, %%al"             \
                   : "=eax" (_value_)           \
                   :  "edx"(_register_)         \
        );                                      \
}                                               \
CYG_MACRO_END

#define HAL_WRITE_UINT8( _register_, _value_ )          \
CYG_MACRO_START                                         \
{                                                       \
    asm volatile ( "outb %%al,%%dx"                     \
                   :                                    \
                   : "eax" (_value_), "edx"(_register_) \
        );                                              \
}                                                       \
CYG_MACRO_END

#define HAL_READ_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )     \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END

#define HAL_WRITE_UINT8_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END


//-----------------------------------------------------------------------------
// 16 bit access.
// Individual and vectorized access to 16 bit registers.
    
#define HAL_READ_UINT16( _register_, _value_ )  \
CYG_MACRO_START                                 \
{                                               \
    asm volatile ( "xor %%eax,%%eax ;"          \
                   "inw %%dx, %%ax"             \
                   : "=eax" (_value_)           \
                   :  "edx"(_register_)         \
        );                                      \
}                                               \
CYG_MACRO_END

#define HAL_WRITE_UINT16( _register_, _value_ )         \
CYG_MACRO_START                                         \
{                                                       \
    asm volatile ( "outw %%ax,%%dx"                     \
                   :                                    \
                   : "eax" (_value_), "edx"(_register_) \
        );                                              \
}                                                       \
CYG_MACRO_END


#define HAL_READ_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END

#define HAL_WRITE_UINT16_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END

//-----------------------------------------------------------------------------
// 32 bit access.
// Individual and vectorized access to 32 bit registers.
    
#define HAL_READ_UINT32( _register_, _value_ )  \
CYG_MACRO_START                                 \
{                                               \
    asm volatile ( "inl %%dx, %%eax"            \
                   : "=eax" (_value_)           \
                   :  "edx"(_register_)         \
        );                                      \
}                                               \
CYG_MACRO_END

#define HAL_WRITE_UINT32( _register_, _value_ )         \
CYG_MACRO_START                                         \
{                                                       \
    asm volatile ( "outl %%eax,%%dx"                    \
                   :                                    \
                   : "eax" (_value_), "edx"(_register_) \
        );                                              \
}                                                       \
CYG_MACRO_END

#define HAL_READ_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )    \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END

#define HAL_WRITE_UINT32_VECTOR( _register_, _buf_, _count_, _step_ )   \
    CYG_MACRO_START                                                     \
    ! Not supported MACRO !						\
    CYG_MACRO_END

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_HAL_IO_H
// End of hal_io.h
