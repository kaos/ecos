//==========================================================================
//
//      devs/watchdog/mn10300/mn10300/watchdog_mn10300.cxx
//
//      Watchdog implementation for MN10300
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
// Author(s):    nickg
// Contributors: nickg
// Date:         1999-02-18
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the MN10300 hardware watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package
#include <pkgconf/kernel.h>             // Kernel config

#include <cyg/kernel/ktypes.h>          // base kernel types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access

#include <cyg/kernel/intr.hxx>          // interrupts

#include <cyg/io/watchdog.hxx>          // watchdog API

// -------------------------------------------------------------------------
// MN10300 watchdog timer registers

#if defined(CYGPKG_HAL_MN10300_AM31)

#define WATCHDOG_BASE           0x34004000
#define WATCHDOG_COUNTER        (WATCHDOG_BASE)
#define WATCHDOG_CONTROL        (WATCHDOG_BASE+2)
#define WATCHDOG_RESET          (WATCHDOG_BASE+4)

#define WATCHDOG_WDCK0          0x07
#define WATCHDOG_WDCK0_DEFAULT  0x04    // 1016.801ms cycle
#define WATCHDOG_WDRST          0x40
#define WATCHDOG_WDCNE          0x80

#define WATCHDOG_RESOLUTION     1016801000      // cycle time in nanoseconds

#elif defined(CYGPKG_HAL_MN10300_AM33)

#define WATCHDOG_BASE           0xC0001000
#define WATCHDOG_COUNTER        (WATCHDOG_BASE)
#define WATCHDOG_CONTROL        (WATCHDOG_BASE+2)
#define WATCHDOG_RESET          (WATCHDOG_BASE+4)

#define WATCHDOG_WDCK0          0x07
#define WATCHDOG_WDCK0_DEFAULT  0x04    // 621.387ms cycle
#define WATCHDOG_WDRST          0x40
#define WATCHDOG_WDCNE          0x80

#define WATCHDOG_RESOLUTION     621387000       // cycle time in nanoseconds

#else

#error Unsupported MN10300 variant

#endif

// -------------------------------------------------------------------------
// Forward definitions

static cyg_ISR    watchdog_isr;

// -------------------------------------------------------------------------
// Statics

// Interrupt object
static Cyg_Interrupt interrupt(
    CYGNUM_HAL_INTERRUPT_WATCHDOG,
    0,
    0,
    watchdog_isr,
    NULL                                // no DSR
    );

// -------------------------------------------------------------------------
// Constructor

void
Cyg_Watchdog::init_hw(void)
{
    CYG_REPORT_FUNCTION();

    // HW doesn't need init.
    
    resolution          = WATCHDOG_RESOLUTION;
        
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start(void)
{
    CYG_REPORT_FUNCTION();

    interrupt.attach();

    HAL_WRITE_UINT8( WATCHDOG_COUNTER, 0 );

    // Set overflow cycle 
    // Enable and reset counter
    HAL_WRITE_UINT8( WATCHDOG_CONTROL,
                     WATCHDOG_WDCK0_DEFAULT|WATCHDOG_WDCNE|WATCHDOG_WDRST);

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void
Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();

    cyg_uint8 ctrl;
    
    HAL_READ_UINT8( WATCHDOG_CONTROL, ctrl );

    ctrl |= WATCHDOG_WDRST;

    HAL_WRITE_UINT8( WATCHDOG_CONTROL, ctrl );
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// ISR

cyg_uint32
watchdog_isr( cyg_vector vector, CYG_ADDRWORD data)
{
    CYG_REPORT_FUNCTION();
    
    // Disable interrupt just in case
    interrupt.detach();

    // Turn watchdog off to prevent it re-triggering.
    HAL_WRITE_UINT8( WATCHDOG_CONTROL, 0 );

    Cyg_Watchdog::watchdog.trigger();

    CYG_REPORT_RETURN();

    return Cyg_Interrupt::HANDLED;
}

// -------------------------------------------------------------------------
// EOF watchdog_mn10300.cxx
