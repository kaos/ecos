//==========================================================================
//
//      watchdog/emulate.cxx
//
//      Watchdog implementation emulation
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1998-07-29
// Purpose:     Watchdog class implementation
// Description: Contains an implementation of the Watchdog class for use
//              when there is no hardware watchdog timer. Instead it is
//              emulated using an Alarm object.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/watchdog.h>           // watchdog configuration file

#ifdef CYGIMP_WATCHDOG_EMULATE

#include <pkgconf/kernel.h>             // Kernel config

#include <cyg/kernel/ktypes.h>          // base kernel types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/kernel/instrmnt.h>        // instrumentation

#include <cyg/kernel/clock.hxx>         // clock and alarm
#include <cyg/kernel/sched.hxx>         // scheduler

#include <cyg/devs/watchdog.hxx>        // my header

#include <cyg/kernel/sched.inl>         // scheduler inlines

// -------------------------------------------------------------------------
// Forward definitions

static cyg_alarm_fn    watchdog_alarm;

// -------------------------------------------------------------------------
// Statics

// A static pointer to the single system defined watchdog device.
Cyg_Watchdog Cyg_Watchdog::watchdog;

static Cyg_Alarm alarm( Cyg_Clock::real_time_clock, watchdog_alarm, 0 );

// One second's worth of ticks.
static cyg_tick_count one_sec;

// -------------------------------------------------------------------------
// Constructor

Cyg_Watchdog::Cyg_Watchdog()
{
    CYG_REPORT_FUNCTION();
    
    action_list         = 0;

    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();

    one_sec             = ( res.divisor * 1000000000LL ) / res.dividend ;
    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Start the watchdog running.

void Cyg_Watchdog::start()
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();

    // Set alarm to a one second single-shot trigger
    alarm.initialize( Cyg_Clock::real_time_clock->current_value() + one_sec, 0 );

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Reset watchdog timer. This needs to be called regularly to prevent
// the watchdog firing.

void Cyg_Watchdog::reset()
{    
    CYG_REPORT_FUNCTION();
    
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();

    Cyg_Scheduler::lock();
    
    // Set alarm to a one second single-shot trigger
    alarm.initialize( Cyg_Clock::real_time_clock->current_value() + one_sec, 0 );    

    Cyg_Scheduler::unlock();    

    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Trigger the watchdog as if the timer had expired.

void Cyg_Watchdog::trigger()
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Scheduler::lock();
    
    // Disable alarm just in case
    alarm.disable();

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

void Cyg_Watchdog::install_action( Cyg_Watchdog_Action *action )
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

void Cyg_Watchdog::uninstall_action( Cyg_Watchdog_Action *action )
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
// Alarm function

void watchdog_alarm( Cyg_Alarm *a, CYG_ADDRWORD data)
{
    CYG_REPORT_FUNCTION();
    
    Cyg_Watchdog::watchdog.trigger();
}

#endif // CYGIMP_WATCHDOG_EMULATE
// -------------------------------------------------------------------------
// EOF watchdog/emulate.cxx
