#ifndef CYGONCE_KERNEL_TIMER_HXX
#define CYGONCE_KERNEL_TIMER_HXX

//==========================================================================
//
//	timer.hxx
//
//	Timer handler class declaration(s)
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	dsm
// Contributors:	dsm
// Date:	1998-06-10
// Purpose:	Define Timer class interfaces
// Description:	This file defines the Timer class which is derived from
//              the Alarm class to support uITRON type functionality
// Usage:       #include <cyg/kernel/timer.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/kernel/clock.hxx>         // Cyg_Alarm

// -------------------------------------------------------------------------
// Timer handler class

class Cyg_Timer 
    : public Cyg_Alarm
{
public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Timer();

    ~Cyg_Timer();

    enum {
        DISABLE = 0,
        ENABLE  = 1,
        RESET   = 2,
    };
    
    void initialize(
        Cyg_Counter    *counter,
        cyg_alarm_fn   alarm_fn,
        CYG_ADDRWORD   data,
        cyg_tick_count trigger,         // absolute time
        cyg_tick_count interval,        // 0 => one shot, else repeating
        cyg_uint32     action           // (DISABLE | ENABLE)
        );
    
    void activate(cyg_uint32 action);   // (DISABLE | ENABLE) [|RESET]

    cyg_tick_count get_trigger();
    cyg_bool       is_enabled();
    cyg_bool       is_initialized();
    CYG_ADDRWORD   get_data();
};

// -------------------------------------------------------------------------
// Timer inlines

inline cyg_tick_count
Cyg_Timer::get_trigger()
{
    return trigger;
}

inline cyg_bool
Cyg_Timer::is_initialized()
{
    return NULL != counter;
}

inline cyg_bool
Cyg_Timer::is_enabled()
{
    return enabled;
}

inline CYG_ADDRWORD
Cyg_Timer::get_data()
{
    return data;
}

#endif // ifndef CYGONCE_KERNEL_TIMER_HXX
// EOF timer.hxx
