//==========================================================================
//
//      devs/wallclock/emulate.cxx
//
//      Wallclock emulation implementation
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          1999-03-05
// Purpose:       Wallclock emulation
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/wallclock.h>  // Wallclock device config

#ifdef CYGIMP_WALLCLOCK_EMULATE

#include <pkgconf/kernel.h>             // Kernel config

#include <cyg/infra/cyg_type.h> // Common type definitions and support
#include <cyg/hal/hal_diag.h>   // HAL diagnostic output

#include <cyg/devs/wallclock.hxx>

#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/clock.hxx>

#include <cyg/kernel/sched.inl>


/*---------------------------------------------------------------------------*/
// Local static variables

static Cyg_WallClock wallclock_instance CYG_INIT_PRIORITY( CLOCK );

static cyg_tick_count epoch_ticks;

static cyg_uint32 epoch_time_stamp;

Cyg_WallClock *Cyg_WallClock::wallclock;

/*---------------------------------------------------------------------------*/
// Constructor

Cyg_WallClock::Cyg_WallClock()
{
    // install instance pointer
    wallclock = &wallclock_instance;
}

/*---------------------------------------------------------------------------*/
// Returns the current timestamp. This may involve reading the
// hardware, so it may take anything up to a second to complete.

cyg_uint32 Cyg_WallClock::get_current_time()
{
    Cyg_Clock::cyg_resolution res = Cyg_Clock::real_time_clock->get_resolution();
        
    Cyg_Scheduler::lock();

    cyg_tick_count diff = Cyg_Clock::real_time_clock->current_value()
                          - epoch_ticks;

    diff = ( diff * res.dividend ) / ( res.divisor * 1000000000LL ) ;
    
    Cyg_Scheduler::unlock();

    return epoch_time_stamp + diff;
}

/*---------------------------------------------------------------------------*/
// Sets the value of the timestamp relative to now. This may involve
// writing to the hardware, so it may take anything up to a second to
// complete.
void Cyg_WallClock::set_current_time( cyg_uint32 time_stamp )
{
    Cyg_Scheduler::lock();

    epoch_time_stamp    = time_stamp;
    epoch_ticks         = Cyg_Clock::real_time_clock->current_value();

    Cyg_Scheduler::unlock();
}

/*---------------------------------------------------------------------------*/

#endif
/*---------------------------------------------------------------------------*/
/* End of devs/wallclock/emulate.cxx */
