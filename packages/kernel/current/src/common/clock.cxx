//==========================================================================
//
//      common/clock.cxx
//
//      Clock class implementations
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
// Date:        1997-09-15
// Purpose:     Clock class implementation
// Description: This file contains the definitions of the counter,
//              clock and alarm class member functions that are common
//              to all clock implementations.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/kernel/clock.hxx>        // our header

#include <cyg/kernel/sched.hxx>        // scheduler definitions
#include <cyg/kernel/thread.hxx>       // thread definitions
#include <cyg/kernel/intr.hxx>         // interrupt definitions

#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/clock.inl>        // Clock inlines

// -------------------------------------------------------------------------
// Static variables

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK

Cyg_Clock *Cyg_Clock::real_time_clock = NULL;   // System real time clock

#endif

//==========================================================================
// Constructor for counter object

Cyg_Counter::Cyg_Counter(
    cyg_uint32      incr
    )
{
    CYG_REPORT_FUNCTION();

    counter = 0;
    increment = incr;
#if defined(CYGIMP_KERNEL_COUNTERS_SINGLE_LIST)

    alarm_list = NULL;    // Linear list of Alarms

#elif defined(CYGIMP_KERNEL_COUNTERS_MULTI_LIST)

    for(cyg_ucount32 i=0; i < CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE; i++) {
        alarm_list[i] = NULL;
    }

#else
#error "No CYGIMP_KERNEL_COUNTERS_x_LIST config"
#endif

}

// -------------------------------------------------------------------------
// Destructor for Counter object

Cyg_Counter::~Cyg_Counter()
{
    CYG_REPORT_FUNCTION();


}

// -------------------------------------------------------------------------
// 

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Counter::check_this( cyg_assert_class_zeal zeal) const
{
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
    case cyg_quick:
    case cyg_trivial:
    case cyg_none:
    default:
        break;
    };

    return true;
}

#endif

// -------------------------------------------------------------------------
// Counter tick function

void Cyg_Counter::tick( cyg_uint32 ticks )
{
//    CYG_REPORT_FUNCTION();

    CYG_ASSERTCLASS( this, "Bad counter object" );

    // Increment the counter in a loop so we process
    // each tick separately. This is easier than trying
    // to cope with a range of increments.
    
    while( ticks-- )
    {
        Cyg_Scheduler::lock();

        // increment the counter, note that it is
        // allowed to wrap.
        counter += increment;

        // now check for any expired alarms

        Cyg_Alarm **alarm_list_ptr;     // pointer to list

#if defined(CYGIMP_KERNEL_COUNTERS_SINGLE_LIST)

        alarm_list_ptr = &alarm_list;

#elif defined(CYGIMP_KERNEL_COUNTERS_MULTI_LIST)

        // With multiple lists, each one contains only the alarms
        // that will expire at a given tick modulo the list number.
        // So we only have a fraction of the alarms to check here.
        
        alarm_list_ptr = &(alarm_list[
            (counter/increment) % CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE ] );
    
#else
#error "No CYGIMP_KERNEL_COUNTERS_x_LIST config"
#endif

        // Now that we have the list pointer, we can use common code for
        // both list oragnizations.

#ifdef CYGIMP_KERNEL_COUNTERS_SORT_LIST

        // With a sorted alarm list, we can simply pick alarms off the
        // front of the list until we find one that is in the future.
        
        while( *alarm_list_ptr != NULL )
        {
            Cyg_Alarm *alarm = *alarm_list_ptr;

            CYG_ASSERTCLASS(alarm, "Bad alarm in counter list" );
            
            if( alarm->trigger <= counter )
            {
                // remove alarm from list
                *alarm_list_ptr = alarm->next;

                if( alarm->interval != 0 )
                {
                    // The alarm has a retrigger interval.
                    // Reset the trigger time and requeue
                    // the alarm.
                    alarm->trigger += alarm->interval;
                    add_alarm( alarm );
                }
                else alarm->enabled = false;

                CYG_INSTRUMENT_ALARM( CALL, this, alarm );
                
                // call alarm function
                alarm->alarm(alarm, alarm->data);

                // all done, loop
            }
            else break;
        }
#else

        // With an unsorted list, we must scan the whole list for
        // candidates. We move the whole list to a temporary location
        // before doing this so that we are not disturbed by new
        // alarms being added to the list. As we consider and
        // eliminate alarms we put them onto the done_list and at the
        // end we then move it back to where it belongs.
        
        Cyg_Alarm *done_list = NULL;

        Cyg_Alarm *alarm_list = *alarm_list_ptr;
        *alarm_list_ptr = NULL;
        
        while( alarm_list != NULL )
        {
            Cyg_Alarm *alarm = alarm_list;

            CYG_ASSERTCLASS(alarm, "Bad alarm in counter list" );
            
            // remove alarm from list
            alarm_list = alarm->next;

            if( alarm->trigger <= counter )
            {
                if( alarm->interval != 0 )
                {
                    // The alarm has a retrigger interval.
                    // Reset the trigger time and requeue
                    // the alarm.
                    alarm->trigger += alarm->interval;
                    add_alarm( alarm );
                }
                else alarm->enabled = false;

                CYG_INSTRUMENT_ALARM( CALL, this, alarm );
                
                // call alarm function
                alarm->alarm(alarm, alarm->data);

                // all done, loop
            }
            else
            {
                // add unused alarm to done list.
                alarm->next = done_list;
                done_list = alarm;
            }
        }

        // Transfer any alarms that might have been added to the
        // alarm list by alarm callbacks to the done list. This
        // happens very rarely.
        while( *alarm_list_ptr != NULL )
        {
            Cyg_Alarm *alarm = *alarm_list_ptr;            
            *alarm_list_ptr = alarm->next;
            alarm->next = done_list;
            done_list = alarm;
        }
        
        // return done list to real list
        *alarm_list_ptr = done_list;
        
#endif        
        Cyg_Scheduler::unlock();    

    }
    
}

// -------------------------------------------------------------------------
// Add an alarm to this counter

void Cyg_Counter::add_alarm( Cyg_Alarm *alarm )
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERTCLASS( this, "Bad counter object" );
    CYG_ASSERTCLASS( alarm, "Bad alarm passed" );
    
    Cyg_Scheduler::lock();

    // set this now to allow an immediate handler call to manipulate
    // this alarm sensibly.
    alarm->enabled = true;

    // Check here for an alarm that triggers now or in the past and
    // call its alarm function immediately. 
    if( alarm->trigger <= counter )
    {
        CYG_INSTRUMENT_ALARM( CALL, this, alarm );

        // call alarm function. Note that this is being
        // called here before the add_alarm has returned.
        // Note that this function may disable the alarm.
        
        alarm->alarm(alarm, alarm->data);

        // Note that this extra check on alarm->enabled is in case the
        // handler function disables this alarm!
        if( alarm->interval != 0 && alarm->enabled )
        {
            // The alarm has a retrigger interval.
            // Reset the trigger interval and drop
            // through to queue it.
            alarm->trigger += alarm->interval;
            // ensure the next alarm time is in our future, and in phase
            // with the original time requested.
            alarm->synchronize();
        }
        else
        {
            // The alarm is all done with, disable it
            // unlock and return.
            alarm->enabled = false;
            Cyg_Scheduler::unlock();
            return;
        }
    }
    
    CYG_INSTRUMENT_ALARM( ADD, this, alarm );
 
    {
        // Find the pointer to the relevant list _after_ a retrigger
        // alarm has been given its new trigger time.

        Cyg_Alarm **alarm_list_ptr;     // pointer to list

#if defined(CYGIMP_KERNEL_COUNTERS_SINGLE_LIST)

        alarm_list_ptr = &alarm_list;

#elif defined(CYGIMP_KERNEL_COUNTERS_MULTI_LIST)

        // Each alarm must go into the list that covers the tick that is
        // going to happen _after_ the trigger time (or at it if trigger
        // happens to fall on a tick.
    
        alarm_list_ptr = &(alarm_list[
            ((alarm->trigger+increment-1)/increment) %
                                CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE ] );
    
#else
#error "No CYGIMP_KERNEL_COUNTERS_x_LIST config"
#endif

#ifdef CYGIMP_KERNEL_COUNTERS_SORT_LIST
        
        // Now that we have the list pointer, we can use common code for
        // both list oragnizations.

        while( *alarm_list_ptr != NULL )
        {
            Cyg_Alarm *list_alarm = *alarm_list_ptr;
        
            CYG_ASSERTCLASS(list_alarm, "Bad alarm in counter list" );

            // The alarms are in ascending trigger order. When we
            // find an alarm that is later than us, we go in front of
            // it.
        
            if( list_alarm->trigger > alarm->trigger ) break;
            else alarm_list_ptr = &list_alarm->next;
        }
#endif
        // Insert the new alarm at *alarm_list_ptr

        alarm->next = *alarm_list_ptr;
        *alarm_list_ptr = alarm;
    
        Cyg_Scheduler::unlock();            
    }
}

// -------------------------------------------------------------------------
// Remove an alarm from this counter

void Cyg_Counter::rem_alarm( Cyg_Alarm *alarm )
{
    CYG_REPORT_FUNCTION();

    CYG_ASSERTCLASS( this, "Bad counter object" );
    CYG_ASSERTCLASS( alarm, "Bad alarm passed" );
    
    Cyg_Alarm **alarm_list_ptr;     // pointer to list

#if defined(CYGIMP_KERNEL_COUNTERS_SINGLE_LIST)

    alarm_list_ptr = &alarm_list;

#elif defined(CYGIMP_KERNEL_COUNTERS_MULTI_LIST)

    alarm_list_ptr = &(alarm_list[
        ((alarm->trigger+increment-1)/increment) %
                              CYGNUM_KERNEL_COUNTERS_MULTI_LIST_SIZE ] );
    
#else
#error "No CYGIMP_KERNEL_COUNTERS_x_LIST config"
#endif

    // Now that we have the list pointer, we can use common code for
    // both list organizations.

    Cyg_Scheduler::lock();

    CYG_INSTRUMENT_ALARM( REM, this, alarm );
    
    while( *alarm_list_ptr != NULL )
    {
        Cyg_Alarm *list_alarm = *alarm_list_ptr;

        CYG_ASSERTCLASS(list_alarm, "Bad alarm in counter list" );

        if( list_alarm == alarm ) break;
        else alarm_list_ptr = &list_alarm->next;
    }

    // If the alarm was found, remove it from the list.
    if( *alarm_list_ptr != NULL )
    {
        *alarm_list_ptr = alarm->next;
        alarm->enabled = false;
    }

    Cyg_Scheduler::unlock();            
}

//==========================================================================
// Constructor for clock object

Cyg_Clock::Cyg_Clock(
    cyg_resolution      res
    )
{
    CYG_REPORT_FUNCTION();

    resolution = res;
}

// -------------------------------------------------------------------------
// Destructor for Clock objects

Cyg_Clock::~Cyg_Clock()
{
    CYG_REPORT_FUNCTION();

}

// -------------------------------------------------------------------------
// 

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Clock::check_this( cyg_assert_class_zeal zeal) const
{
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
    case cyg_quick:
    case cyg_trivial:
    case cyg_none:
    default:
        break;
    };

    return true;
}

#endif

//==========================================================================
// Constructor for alarm object

Cyg_Alarm::Cyg_Alarm(
        Cyg_Counter     *c,             // Attached to this counter
        cyg_alarm_fn    *a,             // Call-back function
        CYG_ADDRWORD    d               // Call-back data
        )
{
    CYG_REPORT_FUNCTION();

    counter     = c;
    alarm       = a;
    data        = d;
    trigger     = 0;
    interval    = 0;
    enabled     = false;

#if defined(CYGIMP_KERNEL_COUNTERS_SINGLE_LIST) || defined(CYGIMP_KERNEL_COUNTERS_MULTI_LIST)
    next        = NULL;
#endif
    
}

Cyg_Alarm::Cyg_Alarm(){}

// -------------------------------------------------------------------------
// Destructor

Cyg_Alarm::~Cyg_Alarm()
{
    CYG_REPORT_FUNCTION();

    disable();
}

// -------------------------------------------------------------------------
// 

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Alarm::check_this( cyg_assert_class_zeal zeal) const
{
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
        if( trigger != 0 && !enabled ) return false;
    case cyg_quick:
    case cyg_trivial:
    case cyg_none:
    default:
        break;
    };

    return true;
}

#endif

// -------------------------------------------------------------------------
// Initialize Alarm and enable

void Cyg_Alarm::initialize(                
    cyg_tick_count    t,                // Absolute trigger time
    cyg_tick_count    i                 // Relative retrigger interval
    )
{
    CYG_REPORT_FUNCTION();

    // If already enabled, remove from counter
    
    if( enabled ) counter->rem_alarm(this);

    CYG_INSTRUMENT_ALARM( INIT,     this, 0 );
    CYG_INSTRUMENT_ALARM( TRIGGER,
                          ((cyg_uint32 *)&t)[0],
                          ((cyg_uint32 *)&t)[1] );
    CYG_INSTRUMENT_ALARM( INTERVAL,
                          ((cyg_uint32 *)&i)[0],
                          ((cyg_uint32 *)&i)[1] );
 
    trigger = t;
    interval = i;

    counter->add_alarm(this);
}

// -------------------------------------------------------------------------
// Synchronize with a past alarm stream that had been disabled,
// bring past times into synch, and the like.

void
Cyg_Alarm::synchronize( void )
{
    if( interval != 0 ) {
        // This expression sets the trigger to the next whole interval
        // at or after the current time. This means that alarms will
        // continue at the same intervals as if they had never been
        // disabled. The alternative would be to just set trigger to
        // (counter->counter + interval), but this is less satisfying
        // than preserving the original intervals. That behaviour can
        // always be obtained by using initialize() rather than
        // enable(), while the current behaviour would be more
        // difficult to achieve that way.
        cyg_tick_count d;
        d = counter->current_value() + interval - trigger;
        if ( d > interval ) {
            // then trigger was in the past, so resynchronize
            trigger += interval * ((d - 1) / interval );
        }
        // otherwise, we were just set up, so no worries.
    }
}

// -------------------------------------------------------------------------
// Ensure alarm enabled

void Cyg_Alarm::enable()
{
    if( !enabled )
    {
        // ensure the alarm time is in our future:
        synchronize();
        enabled = true;
        counter->add_alarm(this);
    }
}

//==========================================================================
// System clock object

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK

class Cyg_RealTimeClock
    : public Cyg_Clock
{
    Cyg_Interrupt       interrupt;

    static cyg_uint32 isr(cyg_vector vector, CYG_ADDRWORD data);

    static void dsr(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data);

    Cyg_RealTimeClock();

    static Cyg_RealTimeClock rtc;
};

Cyg_Clock::cyg_resolution rtc_resolution = CYGNUM_KERNEL_COUNTERS_RTC_RESOLUTION;

//Cyg_RealTimeClock Cyg_RealTimeClock::rtc __attribute__((init_priority (1)));

Cyg_RealTimeClock Cyg_RealTimeClock::rtc CYG_INIT_PRIORITY( CLOCK );

// -------------------------------------------------------------------------

Cyg_RealTimeClock::Cyg_RealTimeClock()
    : Cyg_Clock(rtc_resolution),
      interrupt(CYGNUM_HAL_INTERRUPT_RTC, 1, (CYG_ADDRWORD)this, isr, dsr)
{
    CYG_REPORT_FUNCTION();

    HAL_CLOCK_INITIALIZE( CYGNUM_KERNEL_COUNTERS_RTC_PERIOD );
    
    interrupt.attach();

    interrupt.unmask_interrupt(CYGNUM_HAL_INTERRUPT_RTC);

    Cyg_Clock::real_time_clock = this;
}

#ifdef HAL_CLOCK_LATENCY
cyg_tick_count total_clock_latency, total_clock_interrupts;
cyg_int32 min_clock_latency = 0x7FFFFFFF;
cyg_int32 max_clock_latency = 0;
bool measure_clock_latency = false;
#endif

// -------------------------------------------------------------------------

cyg_uint32 Cyg_RealTimeClock::isr(cyg_vector vector, CYG_ADDRWORD data)
{
//    CYG_REPORT_FUNCTION();

#ifdef HAL_CLOCK_LATENCY
    if (measure_clock_latency) {
        cyg_int32 delta;
        HAL_CLOCK_LATENCY(&delta);
        // Note: Ignore a latency of 0 when finding min_clock_latency.
        if (delta > 0) {
            // Valid delta measured
            total_clock_latency += delta;
            total_clock_interrupts++;
            if (min_clock_latency > delta) min_clock_latency = delta;
            if (max_clock_latency < delta) max_clock_latency = delta;
        }
    }
#endif

    CYG_INSTRUMENT_CLOCK( ISR, 0, 0);

    HAL_CLOCK_RESET( CYGNUM_HAL_INTERRUPT_RTC, CYGNUM_KERNEL_COUNTERS_RTC_PERIOD );

    Cyg_Interrupt::acknowledge_interrupt(CYGNUM_HAL_INTERRUPT_RTC);
        
    return Cyg_Interrupt::CALL_DSR|Cyg_Interrupt::HANDLED;
}

// -------------------------------------------------------------------------

void Cyg_RealTimeClock::dsr(cyg_vector vector, cyg_ucount32 count, CYG_ADDRWORD data)
{
//    CYG_REPORT_FUNCTION();

    Cyg_RealTimeClock *rtc = (Cyg_RealTimeClock *)data;

    CYG_INSTRUMENT_CLOCK( TICK_START,
                          rtc->current_value_lo(),
                          rtc->current_value_hi());
                          
    rtc->tick( count );

#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE
#if    0 == CYG_SCHED_UNIQUE_PRIORITIES

    // If timeslicing is enabled, call the scheduler to
    // handle it. But not if we have unique priorities.
    
    Cyg_Scheduler::scheduler.timeslice();

#endif
#endif

    CYG_INSTRUMENT_CLOCK( TICK_END,
                          rtc->current_value_lo(),
                          rtc->current_value_hi());
    
}

#endif

// -------------------------------------------------------------------------
// EOF common/clock.cxx
