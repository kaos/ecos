//==========================================================================
//
//	instrmnt/meminst.cxx
//
//	Memory buffer instrumentation functions
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
// Date:	1997-10-27
// Purpose:	Instrumentation functions
// Description:	The functions in this file are implementations of the
//              standard instrumentation functions that place records
//              into a memory buffer.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/intr.hxx>         // interrupt control
#include <cyg/kernel/sched.hxx>        // scheduler defines

#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/clock.inl>        // clock inlines

#ifdef CYGPKG_KERNEL_INSTRUMENT

// -------------------------------------------------------------------------
// Instrumentation record.

struct Instrument_Record
{
    CYG_WORD16  type;                   // record type
    CYG_WORD16  thread;                 // current thread id
    CYG_WORD    timestamp;              // 32 bit timestamp
    CYG_WORD    arg1;                   // first arg
    CYG_WORD    arg2;                   // second arg
};

// -------------------------------------------------------------------------
// Buffer base and end. This buffer must be a whole number of 

#ifdef CYGVAR_KERNEL_INSTRUMENT_EXTERNAL_BUFFER

externC Instrument_Record       instrument_buffer[];
externC cyg_uint32              instrument_buffer_size;

#else

extern "C"
{
    
Instrument_Record       instrument_buffer[CYGNUM_KERNEL_INSTRUMENT_BUFFER_SIZE];
   
cyg_uint32              instrument_buffer_size = CYGNUM_KERNEL_INSTRUMENT_BUFFER_SIZE;
    
};

#endif

extern "C"
{

#define instrument_buffer_start instrument_buffer[0]
#define instrument_buffer_end   instrument_buffer[instrument_buffer_size]

extern "C"
{
Instrument_Record       *instrument_buffer_pointer = &instrument_buffer_start;
};

#ifdef CYGDBG_KERNEL_INSTRUMENT_FLAGS

// This array contains a 32 bit word for each event class. The
// bits in the word correspond to events. By setting or clearing
// the appropriate bit, the selected instrumentation event may
// be enabled or disabled dynamically.
    
cyg_uint32 instrument_flags[(CYG_INSTRUMENT_CLASS_MAX>>8)+1];
    
#endif
    
};

// -------------------------------------------------------------------------

void cyg_instrument( cyg_uint32 type, CYG_ADDRWORD arg1, CYG_ADDRWORD arg2 )
{

    cyg_uint32 old_ints;

#ifdef CYGDBG_KERNEL_INSTRUMENT_FLAGS    
    
    cyg_ucount8 cl = (type>>8)&0xff;
    cyg_ucount8 event = type&0xff;

    if( instrument_flags[cl] & (1<<event) )
#endif        
    {
        HAL_DISABLE_INTERRUPTS(old_ints);

        Instrument_Record *p = instrument_buffer_pointer;
        Cyg_Thread *t = Cyg_Scheduler::get_current_thread();
        p->type             = type;
        p->thread           = (t==0)?0xFFFF:t->get_unique_id();
#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK        
//        p->timestamp        = Cyg_Clock::real_time_clock->current_value_lo();
        HAL_CLOCK_READ( &p->timestamp );
#else
        p->timestamp        = 0;
#endif
        p->arg1             = arg1;
        p->arg2             = arg2;
    
        p++;
        if( p == &instrument_buffer_end )
            instrument_buffer_pointer = &instrument_buffer_start;
        else instrument_buffer_pointer = p;

        HAL_RESTORE_INTERRUPTS(old_ints);
    }
    
    return;
}

// -------------------------------------------------------------------------
// Functions to enable and disable selected instrumentation events
// when the flags are enabled.

#ifdef CYGDBG_KERNEL_INSTRUMENT_FLAGS

externC void cyg_instrument_enable( cyg_uint32 cl, cyg_uint32 event)
{
    if( 0 != event )
        instrument_flags[cl>>8] |= 1<<event;
    else
        instrument_flags[cl>>8] = ~0;
}

externC void cyg_instrument_disable( cyg_uint32 cl, cyg_uint32 event)
{
    if( 0 != event )
        instrument_flags[cl>>8] &= ~(1<<event);
    else
        instrument_flags[cl>>8] = 0;

}

#endif

// -------------------------------------------------------------------------

#endif // CYGPKG_KERNEL_INSTRUMENT

// EOF instrmnt/null.cxx
