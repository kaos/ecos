#ifndef CYGONCE_IO_WALLCLOCK_HXX
#define CYGONCE_IO_WALLCLOCK_HXX

//==========================================================================
//
//      wallclock.hxx
//
//      Wallclock interface declaration
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
// Purpose:       Wall Clock declarations
// Description:   This file defines the interface to the wall clock device
//                that provides real time stamps. Actual implementations of
//                this driver can be found in devs/wallclock.
// Usage:         #include <cyg/io/wallclock.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/cyg_type.h>

// -------------------------------------------------------------------------
// Wallclock class

class Cyg_WallClock
{

public:

    Cyg_WallClock();

    // Returns the number of seconds elapsed since 1970-01-01 00:00:00.
    // This may involve reading the hardware, so it may take anything
    // up to a second to complete.
    cyg_uint32 get_current_time();

    // Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
    // This may involve writing to the hardware, so it may take
    // anything up to a second to complete.
    void set_current_time( cyg_uint32 time_stamp );

    // A static pointer to the single system defined wall clock device.
    static Cyg_WallClock *wallclock;

private:

    // Functions implemented by the hardware-specific part of the
    // driver.

    // Note: While the API specifies elapsed time since 1970-01-01
    // 00:00:00 this is only an actual requirement if the driver is
    // used in set-get mode - in init-get mode, the upper layer of the
    // driver only needs to know the number of elapsed seconds between
    // the init_hw_seconds and get_hw_seconds calls; the hardware
    // epoch base is factored out, so the driver can use any base
    // desired (see the emulated device).

    // Called to initialize the hardware clock to a known sane state, or
    // simply to make sure it can be accessed in set-get mode.
    void init_hw_seconds( void );

    // Returns the number of seconds elapsed since 1970-01-01 00:00:00.
    // (or any desired epoch when in init-get mode)
    cyg_uint32 get_hw_seconds( void );

#ifdef CYGSEM_WALLCLOCK_SET_GET_MODE
    // Sets the clock. Argument is seconds elapsed since 1970-01-01 00:00:00.
    void set_hw_seconds( cyg_uint32 secs );
#endif
};

#endif // ifndef CYGONCE_DEVS_WALLCLOCK_HXX
// EOF wallclock.hxx
