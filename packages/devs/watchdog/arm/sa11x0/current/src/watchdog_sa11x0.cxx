//==========================================================================
//
//      watchdog/sa11x0.cxx
//
//      Watchdog implementation for StrongARM SA11x0s
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
// Author(s):    hmt, jskov (based on the MN10300 watchdog code by nickg)
// Contributors: jskov, nickg
// Date:         2001-02-27
// Purpose:      Watchdog class implementation
// Description:  Contains an implementation of the Watchdog class for use
//               with the SA11X0 hardware watchdog timer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>             // system configuration file
#include <pkgconf/watchdog.h>           // configuration for this package
#include <pkgconf/kernel.h>             // kernel config

#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access
#include <cyg/hal/hal_intr.h>           // Interrupts
#include <cyg/hal/hal_sa11x0.h>         // IO registers per se

#include <cyg/io/watchdog.hxx>          // watchdog API

// -------------------------------------------------------------------------
// SA11x0 watchdog works by enabling the watchdog (duh!) which means that
// when OS Timer Match Register #3 "OSMR3" compares equal to the 3.6864MHz
// clock in OSCR, then the system resets.
//
// To stay ahead of this, we must repeatedly set OSMR3 to OSCR + K where K
// is the watchdog timeout.  This REQUIRES that the OSCR be freerunning.
//
// OSCR runs at 3.6864MHz, so one second is 3686400 ticks.
// 
// The match register is 32 bits, and wraps as an int32 does (the
// comparison is exact, so you don't need to take special care.)  So we can
// literally do the addition in the obvious way.

#define WATCHDOG_TIMER_TICKS            3686400
#define WATCHDOG_RESOLUTION             (1000000000)

// -------------------------------------------------------------------------
// Constructor

void
Cyg_Watchdog::init_hw(void)
{
    CYG_REPORT_FUNCTION();

    // HW doesn't need init
    
    resolution          = WATCHDOG_RESOLUTION;
        
    CYG_REPORT_RETURN();
}



// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start(void)
{
    int old;
    CYG_REPORT_FUNCTION();
    
    HAL_DISABLE_INTERRUPTS( old );

    // Init the watchdog timer.
    *SA11X0_OSMR3 = *SA11X0_OSCR + WATCHDOG_TIMER_TICKS;
    *SA11X0_OSSR = SA11X0_OSSR_TIMER3; // Ack any pending intr
    *SA11X0_OIER |= SA11X0_OIER_TIMER3; // Enable interrupt is necessary

    CYG_ASSERT( *SA11X0_OSCR < *SA11X0_OSMR3 || 
                *SA11X0_OSMR3 <= WATCHDOG_TIMER_TICKS, "Watchdog wierdness" );

    // Enable the watchdog.
    *SA11X0_OWER = SA11X0_OWER_ENABLE;

    HAL_RESTORE_INTERRUPTS( old );

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void
Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();

    *SA11X0_OSMR3 = *SA11X0_OSCR + WATCHDOG_TIMER_TICKS;
    
    CYG_ASSERT( *SA11X0_OSCR < *SA11X0_OSMR3 || 
                *SA11X0_OSMR3 <= WATCHDOG_TIMER_TICKS, "Watchdog wierdness" );

    CYG_REPORT_RETURN();
}


// -------------------------------------------------------------------------
// EOF watchdog_sa11x0.cxx
