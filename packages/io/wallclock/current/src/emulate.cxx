//==========================================================================
//
//      io/wallclock/emulate.cxx
//
//      Wallclock emulation implementation
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          1999-03-05
// Purpose:       Wallclock emulation
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/wallclock.h>          // Wallclock device config
#include <pkgconf/kernel.h>             // Kernel config

#include <cyg/infra/cyg_type.h>         // Common type definitions and support
#include <cyg/kernel/ktypes.h>

#include <cyg/io/wallclock.hxx>         // The WallClock API

#include <cyg/kernel/clock.hxx>         // Access to kernel clock
#include <cyg/kernel/clock.inl>


// Remember the tick count at the time of initialization.
static cyg_tick_count epoch_ticks;

//-----------------------------------------------------------------------------
// Called to initialize the hardware clock with a known valid
// time. Which of course isn't required when doing emulation using the
// kernel clock. But we set the private epoch base to "now".
void
Cyg_WallClock::init_hw_seconds( void )
{
    // Remember the current clock tick count.
    epoch_ticks = Cyg_Clock::real_time_clock->current_value();
}

//-----------------------------------------------------------------------------
// Returns the number of seconds elapsed since the init call.
cyg_uint32 
Cyg_WallClock::get_hw_seconds( void )
{
    Cyg_Clock::cyg_resolution res = 
        Cyg_Clock::real_time_clock->get_resolution();
    cyg_tick_count secs = Cyg_Clock::real_time_clock->current_value()
        - epoch_ticks;

    secs = ( secs * res.dividend ) / ( res.divisor * 1000000000LL ) ;

    return secs;
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/emulate.cxx
