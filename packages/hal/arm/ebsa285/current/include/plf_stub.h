#ifndef CYGONCE_HAL_PLF_STUB_H
#define CYGONCE_HAL_PLF_STUB_H

//=============================================================================
//
//      plf_stub.h
//
//      Platform header for GDB stub support.
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
// Author(s):   jskov
// Contributors:jskov, gthomas
// Date:        1999-04-21
// Purpose:     Platform HAL stub support for Intel EBSA285 boards.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_ebsa285.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/arm_stub.h>           // architecture stub support

//----------------------------------------------------------------------------
// Define some platform specific communication details. This is mostly
// handled by hal_if now, but we need to make sure the comms tables are
// properly initialized.

externC void cyg_hal_plf_comms_init(void);

#define HAL_STUB_PLATFORM_INIT_SERIAL()       cyg_hal_plf_comms_init()

#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))
#define HAL_STUB_PLATFORM_INTERRUPTIBLE       0
#define HAL_STUB_PLATFORM_INIT_BREAK_IRQ()    CYG_EMPTY_STATEMENT

//----------------------------------------------------------------------------
// Stub initializer.
#define HAL_STUB_PLATFORM_INIT()              CYG_EMPTY_STATEMENT

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//----------------------------------------------------------------------------
// Reset.
#include <cyg/hal/hal_ebsa285.h>        // registers
#include <cyg/hal/hal_io.h>             // IO macros

#define HAL_STUB_PLATFORM_RESET()                                          \
    CYG_MACRO_START                                                        \
    cyg_uint32 ctrl;                                                       \
                                                                           \
    /* If watchdog is already enabled, writing to timer4 has no effect. */ \
    /* But by disabling interupts and just hanging in the loop below    */ \
    /* the timer might run out eventually (not guaranteed).             */ \
    HAL_DISABLE_INTERRUPTS(ctrl);                                          \
                                                                           \
    /* Set timer4 (must be done before enabling watchdog) */               \
    HAL_WRITE_UINT32(SA110_TIMER4_LOAD, 2);                                \
    HAL_WRITE_UINT32(SA110_TIMER4_CONTROL, SA110_TIMER_CONTROL_ENABLE);    \
                                                                           \
    /* Enable watchdog */                                                  \
    HAL_READ_UINT32(SA110_CONTROL, ctrl);                                  \
    ctrl |= SA110_CONTROL_WATCHDOG;                                        \
    HAL_WRITE_UINT32(SA110_CONTROL, ctrl);                                 \
                                                                           \
    for(;;); /* wait for it */                                             \
    CYG_MACRO_END

#define HAL_STUB_PLATFORM_RESET_ENTRY 0x41000000

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
