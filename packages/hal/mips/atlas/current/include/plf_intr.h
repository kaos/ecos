#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H

//==========================================================================
//
//      plf_intr.h
//
//      Atlas Interrupt and clock support
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jskov,
//               gthomas, jlarmour, dmoseley
// Date:         2000-06-06
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for the Atlas board.
//              
// Usage:
//              #include <cyg/hal/plf_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/plf_io.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

// These are decoded via the IP bits of the cause
// register when an external interrupt is delivered.

#define CYGNUM_HAL_INTERRUPT_SER                 0
#define CYGNUM_HAL_INTERRUPT_TIM0                1
#define CYGNUM_HAL_INTERRUPT_2                   2
#define CYGNUM_HAL_INTERRUPT_3                   3
#define CYGNUM_HAL_INTERRUPT_RTC                 4
#define CYGNUM_HAL_INTERRUPT_COREHI              5
#define CYGNUM_HAL_INTERRUPT_CORELO              6
#define CYGNUM_HAL_INTERRUPT_7                   7
#define CYGNUM_HAL_INTERRUPT_PCIA                8
#define CYGNUM_HAL_INTERRUPT_PCIB                9
#define CYGNUM_HAL_INTERRUPT_PCIC               10
#define CYGNUM_HAL_INTERRUPT_PCID               11
#define CYGNUM_HAL_INTERRUPT_ENUM               12
#define CYGNUM_HAL_INTERRUPT_DEG                13
#define CYGNUM_HAL_INTERRUPT_ATXFAIL            14
#define CYGNUM_HAL_INTERRUPT_INTA               15
#define CYGNUM_HAL_INTERRUPT_INTB               16
#define CYGNUM_HAL_INTERRUPT_INTC               17
#define CYGNUM_HAL_INTERRUPT_INTD               18
#define CYGNUM_HAL_INTERRUPT_SERR               19
#define CYGNUM_HAL_INTERRUPT_HW1                20
#define CYGNUM_HAL_INTERRUPT_HW2                21
#define CYGNUM_HAL_INTERRUPT_HW3                22
#define CYGNUM_HAL_INTERRUPT_HW4                23
#define CYGNUM_HAL_INTERRUPT_HW5                24

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     24
#define CYGNUM_HAL_ISR_COUNT                   25

#define CYGNUM_HAL_INTERRUPT_DEBUG_UART        CYGNUM_HAL_INTERRUPT_SER

#define CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt controller access.

#ifndef CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

// Array which stores the configured priority levels for the configured
// interrupts.
externC volatile CYG_BYTE hal_interrupt_level[CYGNUM_HAL_ISR_COUNT];

#define HAL_INTERRUPT_MASK( _vector_ )                       \
    HAL_WRITE_UINT32( HAL_ATLAS_INTRSTEN, (1<<(_vector_)) );

#define HAL_INTERRUPT_UNMASK( _vector_ )                     \
    HAL_WRITE_UINT32( HAL_ATLAS_INTSETEN, (1<<(_vector_)) );

#define HAL_INTERRUPT_ACKNOWLEDGE( _vector_ )                \
{                                                            \
}

#define HAL_INTERRUPT_CONFIGURE( _vector_, _level_, _up_ )   \
{                                                            \
}

#define HAL_INTERRUPT_SET_LEVEL( _vector_, _level_ )         \
{                                                            \
}

#define CYGHWR_HAL_INTERRUPT_CONTROLLER_ACCESS_DEFINED

#endif


//--------------------------------------------------------------------------
// Control-C support.

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)

# define CYGHWR_HAL_GDB_PORT_VECTOR CYGNUM_HAL_INTERRUPT_SER

externC cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data);

# define HAL_CTRLC_ISR hal_ctrlc_isr

#endif


//----------------------------------------------------------------------------
// Reset.
#ifndef CYGHWR_HAL_RESET_DEFINED
extern void hal_atlas_reset( void );
#define CYGHWR_HAL_RESET_DEFINED
#define HAL_PLATFORM_RESET()             hal_atlas_reset()

#define HAL_PLATFORM_RESET_ENTRY 0xbfc00000

#endif // CYGHWR_HAL_RESET_DEFINED

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
