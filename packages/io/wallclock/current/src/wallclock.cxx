//==========================================================================
//
//      devs/wallclock/wallclock.cxx
//
//      Wallclock base
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
// Author(s):     jskov
// Contributors:  jskov
// Date:          2000-03-28
// Purpose:       Wallclock base driver
// Description:   This provides a generic wallclock base driver which
//                relies on sub-drivers to provide the interface functions
//                to the hardware device (see wallclock.hxx for details).
//
//                The driver can be configured to run in one of two modes:
//                  init-get mode:
//                    In this mode, the hardware driver only needs to
//                    implement a function to read the hardware clock
//                    and a (possibly empty) init function which makes
//                    sure the hardware clock is properly initialized.
//                    While the driver in this mode has a smaller memory
//                    foot-print, it does not support battery-backed up
//                    clocks.
//
//                  set-get mode:
//                    Requires both set and get functions for the hardware
//                    device. While bigger, it allows support of 
//                    battery-backed up clocks.
//
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/wallclock.h>          // Wallclock device config

#include <cyg/infra/cyg_type.h>         // Common type definitions and support

#include <cyg/hal/drv_api.h>	        // Driver API

#include <cyg/io/wallclock.hxx>         // The WallClock API


//-----------------------------------------------------------------------------
// Local static variables

static Cyg_WallClock wallclock_instance CYGBLD_ATTRIB_INIT_AFTER( CYG_INIT_CLOCK );

#ifndef CYGSEM_WALLCLOCK_SET_GET_MODE
static cyg_uint32 epoch_ticks;
static cyg_uint32 epoch_time_stamp;
#endif

Cyg_WallClock *Cyg_WallClock::wallclock;

//-----------------------------------------------------------------------------
// Constructor

Cyg_WallClock::Cyg_WallClock()
{
    // install instance pointer
    wallclock = &wallclock_instance;

    // Always allow low-level driver to initialize clock, even though it
    // may not be necessary for set-get mode.
    init_hw_seconds();
}

//-----------------------------------------------------------------------------
// Returns the number of seconds elapsed since 1970-01-01 00:00:00.
// This may involve reading the hardware, so it may take anything up to
// a second to complete.

cyg_uint32 Cyg_WallClock::get_current_time()
{
    cyg_uint32 res;

    cyg_drv_dsr_lock();

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    res = get_hw_seconds();
#else
    res = epoch_time_stamp + get_hw_seconds() - epoch_ticks;
#endif

    cyg_drv_dsr_unlock();

    return res;
}

//-----------------------------------------------------------------------------
// Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
// This may involve reading or writing to the hardware, so it may take
// anything up to a second to complete.
void Cyg_WallClock::set_current_time( cyg_uint32 time_stamp )
{
    cyg_drv_dsr_lock();

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    set_hw_seconds(time_stamp);
#else
    epoch_time_stamp    = time_stamp;
    epoch_ticks         = get_hw_seconds();
#endif

    cyg_drv_dsr_unlock();
}

//-----------------------------------------------------------------------------
// End of devs/wallclock/wallclock.cxx
