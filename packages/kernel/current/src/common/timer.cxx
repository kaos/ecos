//==========================================================================
//
//	common/timer.cxx
//
//	Timer class implementations
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
// Date:	1998-06-11
// Purpose:	Clock class implementation
// Description:	This file implements the Timer class which is derived from
//              the Alarm class to support uITRON type functionality
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/kernel/clock.hxx>
#include <cyg/kernel/timer.hxx>

#include <cyg/kernel/clock.inl>        // Clock inlines

// -------------------------------------------------------------------------

Cyg_Timer::Cyg_Timer()
{
}

Cyg_Timer::~Cyg_Timer()
{
    CYG_REPORT_FUNCTION();

    disable();
    counter = NULL;
}

// -------------------------------------------------------------------------

void
Cyg_Timer::initialize(
        Cyg_Counter    *c,
        cyg_alarm_fn   a,
        CYG_ADDRWORD   d,
        cyg_tick_count t,               // absolute time
        cyg_tick_count i,               // 0 => one shot, else repeating
        cyg_uint32     action           // (DISABLE | ENABLE)
        )
{
    CYG_REPORT_FUNCTION();

    counter  = c;
    alarm    = a;
    data     = d;
    trigger  = t;
    interval = i;
    enabled  = false;

    CYG_ASSERT(0 == (action & ~ENABLE), "unknown action");

    if(action & ENABLE)
        enable();
}
    
void
Cyg_Timer::activate(cyg_uint32 action)  // (DISABLE | ENABLE) [|RESET]
{
    if(!(action & ENABLE))
        disable();

    if((action & RESET))
    {
        cyg_tick_count t;
        t = counter->current_value();
        trigger = t + interval;
    }

    if((action & ENABLE)) {
        enable();
    }
}

// -------------------------------------------------------------------------
// EOF common/timer.cxx
