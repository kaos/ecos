#ifndef CYGONCE_HAL_VAR_INTR_H
#define CYGONCE_HAL_VAR_INTR_H

//==========================================================================
//
//      imp_intr.h
//
//      RM7000 Interrupt and clock support
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jskov
// Contributors: jskov
// Date:         2000-05-09
// Purpose:      RM7000 Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for variants of the RM7000 MIPS
//               architecture.
//              
// Usage:
//              #include <cyg/hal/var_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/plf_intr.h>

//--------------------------------------------------------------------------
// Interrupt controller information

#ifndef CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED
#define HAL_INTERRUPT_MASK( _vector_ )                                      \
    CYG_MACRO_START                                                         \
    if( (_vector_) <= CYGNUM_HAL_INTERRUPT_COMPARE )                        \
    {                                                                       \
        asm volatile (                                                      \
            "mfc0   $3,$12\n"                                               \
            "la     $2,0x00000400\n"                                        \
            "sllv   $2,$2,%0\n"                                             \
            "nor    $2,$2,$0\n"                                             \
            "and    $3,$3,$2\n"                                             \
            "mtc0   $3,$12\n"                                               \
            "nop; nop; nop\n"                                               \
            :                                                               \
            : "r"(_vector_)                                                 \
            : "$2", "$3"                                                    \
            );                                                              \
    }                                                                       \
    else                                                                    \
    {                                                                       \
        /* int 6:9 are masked in the Interrupt Control register */          \
        asm volatile (                                                      \
            "cfc0   $3,$20\n"                                               \
            "la     $2,0x00000004\n"                                        \
            "sllv   $2,$2,%0\n"                                             \
            "nor    $2,$2,$0\n"                                             \
            "and    $3,$3,$2\n"                                             \
            "ctc0   $3,$20\n"                                               \
            "nop; nop; nop\n"                                               \
            :                                                               \
            : "r"((_vector_) )				                    \
            : "$2", "$3"                                                    \
            );                                                              \
    }                                                                       \
    CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ )                                    \
    CYG_MACRO_START                                                         \
    if( (_vector_) <= CYGNUM_HAL_INTERRUPT_COMPARE )                        \
    {                                                                       \
        asm volatile (                                                      \
            "mfc0   $3,$12\n"                                               \
            "la     $2,0x00000400\n"                                        \
            "sllv   $2,$2,%0\n"                                             \
            "or     $3,$3,$2\n"                                             \
            "mtc0   $3,$12\n"                                               \
            "nop; nop; nop\n"                                               \
            :                                                               \
            : "r"(_vector_)                                                 \
            : "$2", "$3"                                                    \
            );                                                              \
    }                                                                       \
    else								    \
    {                                                                       \
        /* int 6:9 are masked in the Interrupt Control register */          \
        asm volatile (                                                      \
            "cfc0   $3,$20\n"                                               \
            "la     $2,0x00000004\n"                                        \
            "sllv   $2,$2,%0\n"                                             \
            "or     $3,$3,$2\n"                                             \
            "ctc0   $3,$20\n"                                               \
            "nop; nop; nop\n"                                               \
            :                                                               \
            : "r"((_vector_) )                                              \
            : "$2", "$3"                                                    \
            );                                                              \
    }                                                                       \
    CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                           \
    CYG_MACRO_START                                                     \
    /* All 10 interrupts have pending bits in the cause register */     \
    cyg_uint32 _srvector_ = _vector_;                                   \
    asm volatile (                                                      \
        "mfc0   $3,$13\n"                                               \
        "la     $2,0x00000400\n"                                        \
        "sllv   $2,$2,%0\n"                                             \
        "nor    $2,$2,$0\n"                                             \
        "and    $3,$3,$2\n"                                             \
        "mtc0   $3,$13\n"                                               \
        "nop; nop; nop\n"                                               \
        :                                                               \
        : "r"(_srvector_)                                               \
        : "$2", "$3"                                                    \
        );                                                              \
    CYG_MACRO_END

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )

#define CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt vectors.

// Vectors and handling of these are defined in platform HALs since the
// CPU itself does not have a builtin interrupt controller.

//--------------------------------------------------------------------------
// Clock control

// This is handled by the default code

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_VAR_INTR_H
// End of var_intr.h
