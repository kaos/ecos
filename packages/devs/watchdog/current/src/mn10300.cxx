//==========================================================================
//
//      watchdog/mn10300.cxx
//
//      Watchdog implementation for MN10300
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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

#if defined(CYG_HAL_MN10300_MN103002) && !defined(CYGIMP_WATCHDOG_EMULATE)

#include <cyg/kernel/ktypes.h>          // base kernel types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/hal/hal_io.h>             // IO register access

#include <cyg/kernel/sched.hxx>         // scheduler
#include <cyg/kernel/intr.hxx>          // interrupts

#include <cyg/kernel/sched.inl>         // scheduler inlines

#include <cyg/devs/watchdog.hxx>        // my header

// -------------------------------------------------------------------------
// MN10300 watchdog timer registers

#define WATCHDOG_BASE           0x34004000
#define WATCHDOG_COUNTER        (WATCHDOG_BASE)
#define WATCHDOG_CONTROL        (WATCHDOG_BASE+2)
#define WATCHDOG_RESET          (WATCHDOG_BASE+4)

#define WATCHDOG_WDCK0          0x07
#define WATCHDOG_WDCK0_DEFAULT  0x04    // 1016.801ms cycle
#define WATCHDOG_WDRST          0x40
#define WATCHDOG_WDCNE          0x80

// -------------------------------------------------------------------------
// Forward definitions

static cyg_ISR    watchdog_isr;
static cyg_DSR    watchdog_dsr;

// -------------------------------------------------------------------------
// Statics

// A static pointer to the single system defined watchdog device.
Cyg_Watchdog Cyg_Watchdog::watchdog;

// Interrupt object
static Cyg_Interrupt interrupt(
    CYGNUM_HAL_INTERRUPT_WATCHDOG,
    0,
    0,
    watchdog_isr,
    watchdog_dsr
    );

// -------------------------------------------------------------------------
// Constructor

Cyg_Watchdog::Cyg_Watchdog()
{
    CYG_REPORT_FUNCTION();
    
    action_list         = 0;

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Start the watchdog running.

void
Cyg_Watchdog::start()
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
// Trigger the watchdog as if the timer had expired.

void
Cyg_Watchdog::trigger()
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Scheduler::lock();
    
    // Disable interrupt just in case
    interrupt.detach();

    Cyg_Watchdog_Action *act = action_list;

    while( 0 != act )
    {
        act->action( act->data );

        act = act->next;
    }

    Cyg_Scheduler::unlock();

    CYG_REPORT_RETURN();
}
    
// -------------------------------------------------------------------------
// Register an action routine that will be called when the timer
// triggers.

void
Cyg_Watchdog::install_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Scheduler::lock();
    
    action->next = action_list;
    action_list = action;

    Cyg_Scheduler::unlock();

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Deregister a previously registered action routine.

void
Cyg_Watchdog::uninstall_action( Cyg_Watchdog_Action *action )
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Scheduler::lock();

    Cyg_Watchdog_Action **act_ptr = &action_list;    

    while( 0 != *act_ptr )
    {
        Cyg_Watchdog_Action *a = *act_ptr;

        if( a == action )
        {
            *act_ptr = a->next;
            break;
        }
        act_ptr = &a->next;
    }
    
    Cyg_Scheduler::unlock();

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// ISR and DSR

cyg_uint32
watchdog_isr( cyg_vector vector, CYG_ADDRWORD data)
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Watchdog::watchdog.trigger();

    CYG_REPORT_RETURN();

    // FIXME: why is this 0??? why not Cyg_Interrupt::HANDLED? - Jifl
    return 0;
}

void
watchdog_dsr(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data)
{
}

#endif // defined(CYG_HAL_MN10300_MN103002) &&
       // !defined(CYGIMP_WATCHDOG_EMULATE)
// -------------------------------------------------------------------------
// EOF watchdog/mn10300.cxx
