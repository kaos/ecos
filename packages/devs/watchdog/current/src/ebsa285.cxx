//==========================================================================
//
//      watchdog/ebsa285.cxx
//
//      Watchdog implementation for Intel EBSA-285 StronARM board
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
// Author(s):    jskov (based on the MN10300 watchdog code by nickg)
// Contributors: jskov, nickg
// Date:         1999-08-26
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the EBSA285/21285 hardware watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package

#if defined(CYGPKG_HAL_ARM_EBSA285) && !defined(CYGIMP_WATCHDOG_EMULATE)

#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access

#include <cyg/devs/watchdog.hxx>        // my header

// -------------------------------------------------------------------------
// 21285 watchdog works by letting timer4 run; if it ever underflows,
// it resets the system. 
// Timer 4 is set to run at fclk_in/16 = 50MHz/16 = 3.125MHz
// The timer register is 24 bits, so it can easily hold a value that
// gives a 1s timeout.
#define WATCHDOG_TIMER_TICKS            3125000
#define WATCHDOG_RESOLUTION             (1000000000)

// -------------------------------------------------------------------------
// Statics

// A static pointer to the single system defined watchdog device.
Cyg_Watchdog Cyg_Watchdog::watchdog;

// -------------------------------------------------------------------------
// Constructor


Cyg_Watchdog::Cyg_Watchdog()
{
    CYG_REPORT_FUNCTION();
    
    action_list         = 0;

    resolution          = WATCHDOG_RESOLUTION;
        
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Return reset resolution

cyg_uint64 Cyg_Watchdog::get_resolution()
{
    return resolution;
}


// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start()
{
    CYG_REPORT_FUNCTION();

    // Init the watchdog timer.
    HAL_WRITE_UINT32(SA110_TIMER4_LOAD, WATCHDOG_TIMER_TICKS);
    HAL_WRITE_UINT32(SA110_TIMER4_CLEAR, 0);
    HAL_WRITE_UINT32(SA110_TIMER4_CONTROL, 
                     SA110_TIMER_CONTROL_ENABLE|SA110_TIMER_CONTROL_SCALE_16);
    // Enable the watchdog.
    cyg_uint32 ctrl;
    HAL_READ_UINT32(SA110_CONTROL, ctrl);
    ctrl |= SA110_CONTROL_WATCHDOG;
    HAL_WRITE_UINT32(SA110_CONTROL, ctrl);

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void
Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();

    HAL_WRITE_UINT32(SA110_TIMER4_LOAD, WATCHDOG_TIMER_TICKS);
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Trigger the watchdog as if the timer had expired.

void
Cyg_Watchdog::trigger()
{
    CYG_REPORT_FUNCTION();
    
    // Init the watchdog timer.
    HAL_WRITE_UINT32(SA110_TIMER4_LOAD, 1);
    HAL_WRITE_UINT32(SA110_TIMER4_CONTROL, SA110_TIMER_CONTROL_ENABLE);
    // Enable the watchdog.
    cyg_uint32 ctrl;
    HAL_READ_UINT32(SA110_CONTROL, ctrl);
    ctrl |= SA110_CONTROL_WATCHDOG;
    HAL_WRITE_UINT32(SA110_CONTROL, ctrl);

    CYG_REPORT_RETURN();
}
    
// -------------------------------------------------------------------------
// Register an action routine that will be called when the timer
// triggers.

void
Cyg_Watchdog::install_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    // Can't be done. If the time expires the board resets. End of story.
    CYG_FAIL("It's not possible to register actions for the EBSA watchdog");

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Deregister a previously registered action routine.

void
Cyg_Watchdog::uninstall_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    // Can't be done. If the time expires the board resets. End of story.
    CYG_FAIL("It's not possible to deregister actions for the EBSA watchdog");

    CYG_REPORT_RETURN();
}

#endif // defined(CYGPKG_HAL_ARM_EBSA285) &&
       // !defined(CYGIMP_WATCHDOG_EMULATE)
// -------------------------------------------------------------------------
// EOF watchdog/ebsa285.cxx
