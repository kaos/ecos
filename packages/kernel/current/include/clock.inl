#ifndef CYGONCE_KERNEL_CLOCK_INL
#define CYGONCE_KERNEL_CLOCK_INL

//==========================================================================
//
//	clock.inl
//
//	Clock and Alarm class inlines
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
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1997-09-30
// Purpose:	Define Clock and Alarm class inlines
// Description:	Define inline functions for counter, clock and alarm
//              classes.
// Usage:       #include <cyg/kernel/clock.hxx>
//              #include <cyg/kernel/clock.inl>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/clock.hxx>

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// Counter class inlines

// Return current value of counter
inline cyg_tick_count Cyg_Counter::current_value()
{
    return counter;
}

inline cyg_uint32 Cyg_Counter::current_value_lo()
{
    return counter&0xFFFFFFFF;
}

inline cyg_uint32 Cyg_Counter::current_value_hi()
{
    return (counter>>32)&0xFFFFFFFF;
}

// Set the counter's current value
inline void Cyg_Counter::set_value( cyg_tick_count new_value)
{
    counter = new_value;
}
        
// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// Clock class inlines

// Return current resolution
inline Cyg_Clock::cyg_resolution Cyg_Clock::get_resolution()
{
    return resolution;
}

// Set new resolution
inline void Cyg_Clock::set_resolution(                
        Cyg_Clock::cyg_resolution new_resolution
        )
{
    resolution = new_resolution;
}

// -------------------------------------------------------------------------
// -------------------------------------------------------------------------
// Alarm inlines

// Ensure alarm disabled
inline void Cyg_Alarm::disable()
{
    if( enabled ) counter->rem_alarm(this);
}


// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_CLOCK_INL
// EOF clock.inl
