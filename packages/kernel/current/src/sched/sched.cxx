//==========================================================================
//
//      sched/sched.cxx
//
//      Scheduler class implementations
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
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-09-15
// Purpose:     Scheduler class implementation
// Description: This file contains the definitions of the scheduler class
//              member functions that are common to all scheduler
//              implementations.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/sched.hxx>        // our header

#include <cyg/kernel/thread.hxx>       // thread classes
#include <cyg/kernel/intr.hxx>         // Interrupt interface

#include <cyg/hal/hal_arch.h>          // Architecture specific definitions

#include <cyg/kernel/thread.inl>       // thread inlines

//-------------------------------------------------------------------------
// Some local tracing control - a default.
#ifdef CYGDBG_USE_TRACING
# if !defined( CYGDBG_INFRA_DEBUG_TRACE_ASSERT_SIMPLE ) && \
     !defined( CYGDBG_INFRA_DEBUG_TRACE_ASSERT_FANCY  )
   // ie. not a tracing implementation that takes a long time to output

#  ifndef CYGDBG_KERNEL_TRACE_UNLOCK_INNER
#   define CYGDBG_KERNEL_TRACE_UNLOCK_INNER
#  endif // control not already defined

# endif  // trace implementation not ..._SIMPLE && not ..._FANCY
#endif   // CYGDBG_USE_TRACING

// -------------------------------------------------------------------------
// Static Cyg_Scheduler class members

// We start with sched_lock at 1 so that any kernel code we
// call during initialization will not try to reschedule.

volatile cyg_ucount32 Cyg_Scheduler_Base::sched_lock = 1;

Cyg_Thread            *Cyg_Scheduler_Base::current_thread = NULL;

cyg_bool              Cyg_Scheduler_Base::need_reschedule = false;

Cyg_Scheduler         Cyg_Scheduler::scheduler CYG_INIT_PRIORITY( SCHEDULER );

cyg_ucount32          Cyg_Scheduler_Base::thread_switches = 0;

// -------------------------------------------------------------------------
// Scheduler unlock function.
// This is only called when the lock is to be decremented to zero and there
// is the potential for real work to be done. Other cases are handled in
// Cyg_Scheduler::unlock() which is an inline.

void Cyg_Scheduler::unlock_inner()
{
#ifdef CYGDBG_KERNEL_TRACE_UNLOCK_INNER
    CYG_REPORT_FUNCTION();
#endif    

    do {

        CYG_PRECONDITION( sched_lock == 1 , "sched_lock not 1" );
        
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS
        
        // Call any pending DSRs. Do this here to ensure that any
        // threads that get awakened are properly scheduled.

        if( Cyg_Interrupt::DSRs_pending() )
            Cyg_Interrupt::call_pending_DSRs();
#endif
            
        Cyg_Thread *current = current_thread;

        CYG_ASSERTCLASS( current, "Bad current thread" );

        // If the current thread is going to sleep, or someone
        // wants a reschedule, choose another thread to run

        if( current->state != Cyg_Thread::RUNNING || need_reschedule ) {

            CYG_INSTRUMENT_SCHED(RESCHEDULE,0,0);
            
            // Get the next thread to run from scheduler
            Cyg_Thread *next = scheduler.schedule();

            CYG_CHECK_DATA_PTR( next, "Invalid next thread pointer");
            CYG_ASSERTCLASS( next, "Bad next thread" );

            if( current != next )
            {

                CYG_INSTRUMENT_THREAD(SWITCH,current,next);

                // Count this thread switch
                thread_switches++;
                
                // Switch contexts
                HAL_THREAD_SWITCH_CONTEXT( &current->stack_ptr,
                                           &next->stack_ptr );

                // Worry here about possible compiler
                // optimizations across the above call that may try to
                // propogate common subexpresions.  We would end up
                // with the expression from one thread in its
                // successor. This is only a worry if we do not save
                // and restore the complete register set. We need a
                // way of marking functions that return into a
                // different context. A temporary fix would be to
                // disable CSE (-fdisable-cse) in the compiler.
                
                // We return here only when the current thread is
                // rescheduled.  There is a bit of housekeeping to do
                // here before we are allowed to go on our way.

                CYG_CHECK_DATA_PTR( current, "Invalid current thread pointer");
                CYG_ASSERTCLASS( current, "Bad current thread" );

                current_thread = current;   // restore current thread pointer

            }

#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE
            // Reset the timeslice counter so that this thread gets a full
            // quantum. 
            reset_timeslice_count();
#endif

            need_reschedule = false;        // finished rescheduling
        }
          
        HAL_REORDER_BARRIER(); // Make sure everything above has happened
                               // by this point
        sched_lock = 0;       // Clear the lock
        HAL_REORDER_BARRIER();
                
#ifdef CYGIMP_KERNEL_INTERRUPTS_DSRS

        // Now check whether any DSRs got posted during the thread
        // switch and if so, go around again. Making this test after
        // the lock has been zeroed avoids a race condition in which
        // a DSR could have been posted during a reschedule, but would
        // not be run until the _next_ time we release the sched lock.

        if( Cyg_Interrupt::DSRs_pending() ) {
            sched_lock = 1;     // reclaim the lock
            continue;           // go back to head of loop
        }

#endif
        // Otherwise the lock is zero, we can return.

        CYG_POSTCONDITION( sched_lock == 0, "sched_lock not zero" );
        
#ifdef CYGDBG_KERNEL_TRACE_UNLOCK_INNER
        CYG_REPORT_RETURN();
#endif
        return;

    } while( 1 );

    CYG_FAIL( "Should not be executed" );
}

// -------------------------------------------------------------------------
// Start the scheduler. This is called after the initial threads have been
// created to start scheduling.

void Cyg_Scheduler::start()
{
    CYG_REPORT_FUNCTION();
        
    // Get the first thread to run from scheduler
    register Cyg_Thread *next = scheduler.schedule();

    CYG_ASSERTCLASS( next, "Bad initial thread" );

    need_reschedule = false;    // finished rescheduling
    current_thread = next;      // restore current thread pointer

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK
    // Reference the real time clock. This ensures that at least one
    // reference to the kernel_clock.o object exists, without which
    // the object will not be included while linking.
    CYG_REFERENCE_OBJECT( Cyg_Clock::real_time_clock );
#endif

    // Let the interrupts go
    Cyg_Interrupt::enable_interrupts();
    
    HAL_THREAD_LOAD_CONTEXT( &next->stack_ptr );    
    
}

// -------------------------------------------------------------------------
// Consistency checker

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Scheduler::check_this( cyg_assert_class_zeal zeal) const
{
    CYG_REPORT_FUNCTION();
        
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
        if( !current_thread->check_this(zeal) ) return false;
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
// SchedThread members

// -------------------------------------------------------------------------
// Constructor

Cyg_SchedThread::Cyg_SchedThread(Cyg_Thread *thread, CYG_ADDRWORD sched_info)
: Cyg_SchedThread_Implementation(sched_info)
{
    CYG_REPORT_FUNCTION();
        
    queue = NULL;
    
    if( Cyg_Scheduler::current_thread == NULL )
        Cyg_Scheduler::current_thread = thread;

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE

    mutex_count = 0;
    priority_inherited = false;
    
#endif
    
}

// -------------------------------------------------------------------------
// Priority inheritance support.

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE

// -------------------------------------------------------------------------
// Inherit the priority of the provided thread if it
// has a higher priority than ours.

void Cyg_SchedThread::inherit_priority( Cyg_Thread *thread)
{
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE

    // A simple implementation of priority inheritance.  If the other
    // thread is of higher priority, reset our priority to his. The
    // first time we do this, save our original priority.

    Cyg_Thread *self = CYG_CLASSFROMBASE(Cyg_Thread,
                                         Cyg_SchedThread,
                                         this);

    CYG_ASSERT( mutex_count > 0, "Non-positive mutex count");
    CYG_ASSERT( self != thread, "Trying to inherit from self!");
    
    // Compare with *current* priority in case thread has already
    // inherited - for relay case below.
    if( thread->get_current_priority() < priority )
    {
        cyg_priority mypri = priority;
        cyg_bool already_inherited = priority_inherited;

        // If this is first inheritance, copy the old pri
        // and set inherited flag. We clear it before setting the
        // pri since set_priority() is inheritance aware.
        // This is called with the sched locked, so no race conditions.

        priority_inherited = false;     // so that set_prio DTRT

        self->set_priority( thread->get_current_priority() );            

        if( !already_inherited )
            original_priority = mypri;

        priority_inherited = true;      // regardless, because it is now

    }

#endif
}

// -------------------------------------------------------------------------
// Inherit the priority of the ex-owner thread or from the queue if it
// has a higher priority than ours.

void Cyg_SchedThread::relay_priority( Cyg_Thread *ex_owner, Cyg_ThreadQueue *pqueue)
{
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE

    // A simple implementation of priority inheritance.
    // At its simplest, this member does nothing.

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE_RELAY

    // If there is anyone else waiting, then the *new* owner inherits from
    // the current one, since that is a maxima of the others waiting.
    // (It's worth not doing if there's nobody waiting to prevent
    // unneccessary priority skew.)  This could be viewed as a discovered
    // priority ceiling.

    if ( !pqueue->empty() )
        inherit_priority( ex_owner );

#endif
#endif
}

// -------------------------------------------------------------------------
// Lose a priority inheritance

void Cyg_SchedThread::disinherit_priority()
{
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE

    // A simple implementation of priority inheritance.  The
    // simplification in this algorithm is that we do not reduce our
    // priority until we have freed all mutexes claimed. Hence we can
    // continue to run at an artificially high priority even when we
    // should not.  However, since nested mutexes are rare, the thread
    // we have inherited from is likely to be locking the same mutexes
    // we are, and mutex claim periods should be very short, the
    // performance difference between this and a more complex algorithm
    // should be negligible. The most important advantage of this
    // algorithm is that it is fast and deterministic.
    
    // The simplest algorithm also does not cause a 2nd owner (who waited)
    // of a mutex to inherit from 3rd, 4th &c threads that are queueing up
    // when it is awoken.  That limitation is avoided when
    // CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE_SIMPLE_RELAY is also
    // enabled, see above, which passes the raised priority from one thread
    // to the next along with the mutex, like a relay baton.

    Cyg_Thread *self = CYG_CLASSFROMBASE(Cyg_Thread,
                                         Cyg_SchedThread,
                                         this);

    CYG_ASSERT( mutex_count >= 0, "Non-positive mutex count");
    
    if( mutex_count == 0 && priority_inherited )
    {
        priority_inherited = false;

        // Only make an effort if the priority must change
        if( priority < original_priority )
            self->set_priority( original_priority );
        
    }
    
#endif    
}

#endif // CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE of any kind

// -------------------------------------------------------------------------
// EOF sched/sched.cxx
