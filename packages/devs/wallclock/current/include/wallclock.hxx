#ifndef CYGONCE_DEVS_WALLCLOCK_HXX
#define CYGONCE_DEVS_WALLCLOCK_HXX

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
// Purpose:       Wall Clock declarations
// Description:   This file defines the interface to the wall clock device
//                that provides real time stamps. This is either an interface
//                to a hardware RTC like the Dallas DS12887, or it is emulated
//                using the kernel clock.
// Usage:         #include <cyg/devs/wallclock.hxx>
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

    // Returns the current timestamp. This may involve reading the
    // hardware, so it may take anything up to a second to complete.
    cyg_uint32 get_current_time();

    // Sets the value of the timestamp relative to now. This may involve
    // writing to the hardware, so it may take anything up to a second to
    // complete.
    void set_current_time( cyg_uint32 time_stamp );

    // A static pointer to the single system defined wall clock device.
    static Cyg_WallClock *wallclock;

};

// -------------------------------------------------------------------------

#endif // ifndef CYGONCE_DEVS_WALLCLOCK_HXX
// EOF wallclock.hxx
