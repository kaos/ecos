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
// Author(s):   gthomas
// Contributors:jskov
//              Travis C. Furrer <furrer@mit.edu>
// Date:        2000-05-08
// Purpose:     Platform HAL stub support for Intel SA11x0 boards.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/hal_sa11x0.h>         // registers
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // Interrupt macros
#include <cyg/hal/arm_stub.h>           // architecture stub support

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//----------------------------------------------------------------------------
// Define some platform specific communication details. This is mostly
// handled by hal_if now, but we need to make sure the comms tables are
// properly initialized.

externC void cyg_hal_plf_comms_init(void);

#define HAL_STUB_PLATFORM_INIT_SERIAL()       cyg_hal_plf_comms_init()
#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))
#define HAL_STUB_PLATFORM_INTERRUPTIBLE       0

//----------------------------------------------------------------------------
// Stub initializer.
#define HAL_STUB_PLATFORM_INIT()        CYG_EMPTY_STATEMENT

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//----------------------------------------------------------------------------
// Reset.
#define HAL_STUB_PLATFORM_RESET()                                          \
    CYG_MACRO_START                                                        \
    cyg_uint32 ctrl;                                                       \
                                                                           \
    /* By disabling interupts we will just hang in the loop below      */  \
    /* if for some reason the software reset fails.                    */  \
    HAL_DISABLE_INTERRUPTS(ctrl);                                          \
                                                                           \
    /* Software reset. */                                                  \
    *SA11X0_RESET_SOFTWARE_RESET = SA11X0_INVOKE_SOFTWARE_RESET;           \
                                                                           \
    for(;;); /* hang here forever if reset fails */                        \
    CYG_MACRO_END

// Fallback (never really used)
#define HAL_STUB_PLATFORM_RESET_ENTRY 0x00000000

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
