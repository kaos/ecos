#ifndef CYGONCE_HAL_XPIC_H
#define CYGONCE_HAL_XPIC_H

//=============================================================================
//
//      hal_xpic.h
//
//      HAL eXternal Programmable Interrupt Controller support
//
//=============================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   nickg, gthomas, hmt
// Contributors:        nickg, gthomas, hmt
// Date:        1999-01-28
// Purpose:     Define Interrupt support
// Description: The macros defined here provide the HAL APIs for handling
//              an external interrupt controller, and which interrupt is
//              used for what.
//              
// Usage:
//              #include <cyg/hal/hal_intr.h> // which includes this file
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/hal_io.h>

//-----------------------------------------------------------------------------
// Interrupt controller access

// in erc32 simulator:
//  4 = UART A
//  5 = UART B
//  7 = UART error
// 12 = GPT (general purpose timer)
// 13 = RTC (realtime clock)
// 15 = watchdog

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC            CYGNUM_HAL_INTERRUPT_13


/* These must be accessed word-wide to work! */
#define SPARC_MEC_INTCON              (0x01f80000)

#define SPARC_MEC_INTCON_PENDING (SPARC_MEC_INTCON + 0x48)
#define SPARC_MEC_INTCON_MASK    (SPARC_MEC_INTCON + 0x4c)
#define SPARC_MEC_INTCON_CLEAR   (SPARC_MEC_INTCON + 0x50)
#define SPARC_MEC_INTCON_FORCE   (SPARC_MEC_INTCON + 0x54)


#define HAL_INTERRUPT_MASK( _vector_ ) CYG_MACRO_START                      \
    cyg_uint32 _traps_, _mask_;                                             \
    HAL_DISABLE_TRAPS( _traps_ );                                           \
    HAL_READ_UINT32( SPARC_MEC_INTCON_MASK, _mask_ );                       \
    _mask_ |= ( 1 << (_vector_) );                                          \
    HAL_WRITE_UINT32(SPARC_MEC_INTCON_MASK,  _mask_ );                      \
    HAL_RESTORE_INTERRUPTS( _traps_ );                                      \
CYG_MACRO_END

#define HAL_INTERRUPT_UNMASK( _vector_ ) CYG_MACRO_START                    \
    cyg_uint32 _traps_, _mask_;                                             \
    HAL_DISABLE_TRAPS( _traps_ );                                           \
    HAL_READ_UINT32( SPARC_MEC_INTCON_MASK, _mask_ );                       \
    _mask_ &=~ ( 1 << (_vector_) );                                         \
    HAL_WRITE_UINT32( SPARC_MEC_INTCON_MASK, _mask_ );                      \
    HAL_RESTORE_INTERRUPTS( _traps_ );                                      \
CYG_MACRO_END

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ ) CYG_MACRO_START               \
    cyg_uint32 _traps_;                                                     \
    HAL_DISABLE_TRAPS( _traps_ );                                           \
    HAL_WRITE_UINT32( SPARC_MEC_INTCON_CLEAR, ( 1 << (_vector_) ) );        \
    HAL_RESTORE_INTERRUPTS( _traps_ );                                      \
CYG_MACRO_END

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ ) /* nothing */

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ ) /* nothing */

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_XPIC_H
// End of hal_xpic.h
