//==========================================================================
//
//	sync/mutex.cxx
//
//	Mutex and condition variable implementation
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
// Date:	1997-10-21
// Purpose:	Mutex implementation
// Description:	This file contains the implementations of the mutex
//              and condition variable classes.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/mutex.hxx>        // our header

#include <cyg/kernel/thread.inl>       // thread inlines
#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/clock.inl>        // clock inlines


// -------------------------------------------------------------------------
// Constructor

Cyg_Mutex::Cyg_Mutex()
{
    CYG_REPORT_FUNCTION();
        
    locked      = false;
    owner       = NULL;
}

// -------------------------------------------------------------------------
// Destructor

Cyg_Mutex::~Cyg_Mutex()
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERT( owner == NULL, "Deleting mutex with owner");
    CYG_ASSERT( queue.empty(), "Deleting mutex with waiting threads");
}

// -------------------------------------------------------------------------

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Mutex::check_this( cyg_assert_class_zeal zeal)
{
//    CYG_REPORT_FUNCTION();
        
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
    case cyg_quick:
    case cyg_trivial:
        if(  locked && owner == NULL ) return false;
        if( !locked && owner != NULL ) return false;        
    case cyg_none:
    default:
        break;
    };

    return true;
}

#endif

// -------------------------------------------------------------------------
// Lock and/or wait

cyg_bool Cyg_Mutex::lock()
{
    CYG_REPORT_FUNCTION();

    cyg_bool result = true;
    Cyg_Thread *self = Cyg_Thread::self();
    
    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    CYG_INSTRUMENT_MUTEX(LOCK, this, 0);

    // Loop while the mutex is locked, sleeping each time around
    // the loop. This copes with the possibility of a higher priority
    // thread grabbing the mutex between the wakeup in unlock() and
    // this thread actually starting.
    
    while( locked )
    {
        CYG_ASSERT( self != owner, "Locking mutex I already own");
        
        self->set_sleep_reason( Cyg_Thread::WAIT );
        
        self->sleep();
        
        queue.enqueue( self );

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE

        owner->inherit_priority(self);
    
#endif

        CYG_INSTRUMENT_MUTEX(WAIT, this, 0);

        CYG_ASSERT( Cyg_Scheduler::get_sched_lock() == 1, "Called with non-zero scheduler lock");
        
        // Unlock scheduler and allow other threads
        // to run
        Cyg_Scheduler::unlock();
        Cyg_Scheduler::lock();

        CYG_ASSERTCLASS( this, "Bad this pointer");

        switch( self->get_wake_reason() )
        {
        case Cyg_Thread::DESTRUCT:
        case Cyg_Thread::BREAK:
            result = false;
            break;
            
        case Cyg_Thread::EXIT:            
            self->exit();
            break;

        default:
            break;
        }

    }

    if( result )
    {
        locked      = true;
        owner       = self;

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE

        self->count_mutex();

#endif

        CYG_INSTRUMENT_MUTEX(LOCKED, this, 0);
    }
    
    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();

    CYG_ASSERTCLASS( this, "Bad this pointer");    

    return result;
}

// -------------------------------------------------------------------------
// Try to lock and return success

cyg_bool Cyg_Mutex::trylock()
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_bool result = true;
    
    // Prevent preemption
    Cyg_Scheduler::lock();

    // If the mutex is not locked, grab it
    // for ourself. Otherwise return failure.
    if( !locked )
    {
        Cyg_Thread *self = Cyg_Thread::self();
        
        locked  = true;
        owner   = self;

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE

        self->count_mutex();

#endif
    }
    else result = false;

    CYG_INSTRUMENT_MUTEX(TRY, this, result);
    
    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
    
    return result;    
}

// -------------------------------------------------------------------------
// unlock

void Cyg_Mutex::unlock()
{
    CYG_REPORT_FUNCTION();
        
    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_INSTRUMENT_MUTEX(UNLOCK, this, 0);

    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERT( locked, "Unlock mutex that is not locked");
    CYG_ASSERT( owner == Cyg_Thread::self(), "Unlock mutex I do not own");
        
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INHERITANCE

    owner->uncount_mutex();
    owner->disinherit_priority();
    
#endif
    
    locked      = false;
    owner       = NULL;
    
    if( !queue.empty() ) {

        // The queue is non-empty, so grab the next
        // thread from it and wake it up.

        Cyg_Thread *thread = queue.dequeue();

        CYG_ASSERTCLASS( thread, "Bad thread pointer");

        thread->set_wake_reason( Cyg_Thread::DONE );
        
        thread->wake();

        CYG_INSTRUMENT_MUTEX(WAKE, this, thread);
        
    }

    CYG_ASSERTCLASS( this, "Bad this pointer");    
    
    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();

}

//==========================================================================
// Condition variables

Cyg_Condition_Variable::Cyg_Condition_Variable(
    Cyg_Mutex   &mx                // linked mutex
    )
{
    CYG_REPORT_FUNCTION();
        
    mutex       = &mx;

    CYG_ASSERTCLASS( mutex, "Invalid mutex argument");
}

// -------------------------------------------------------------------------
// Destructor

Cyg_Condition_Variable::~Cyg_Condition_Variable()
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERT( queue.empty(), "Deleting condvar with waiting threads");
}

// -------------------------------------------------------------------------

#ifdef CYGDBG_USE_ASSERTS

bool Cyg_Condition_Variable::check_this( cyg_assert_class_zeal zeal)
{
    CYG_REPORT_FUNCTION();
        
    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;
    
    switch( zeal )
    {
    case cyg_system_test:
    case cyg_extreme:
    case cyg_thorough:
        if( !mutex->check_this(zeal) ) return false;
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
// Wait for condition to be true    

void Cyg_Condition_Variable::wait()
{
    CYG_REPORT_FUNCTION();
        
    Cyg_Thread *self = Cyg_Thread::self();

    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");
    CYG_ASSERTCLASS( self, "Bad self thread");

    CYG_INSTRUMENT_CONDVAR(WAIT, this, 0);
    
    mutex->unlock();

    self->set_sleep_reason( Cyg_Thread::WAIT );
        
    self->sleep();
        
    queue.enqueue( self );

    CYG_ASSERT( Cyg_Scheduler::get_sched_lock() == 1, "Called with non-zero scheduler lock");
    
    // Unlock the scheduler and switch threads
    Cyg_Scheduler::unlock();

    CYG_INSTRUMENT_CONDVAR(WOKE, this, self->get_wake_reason());
    
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");

    switch( self->get_wake_reason() )
    {
    case Cyg_Thread::EXIT:            
        self->exit();
        break;

    default:
        break;
    }


    // When we awake, we must re-acquire the mutex.  Note that while
    // it is essential to release the mutex and queue on the CV
    // atomically relative to other threads, to avoid races, it is not
    // necessary for us to re-acquire the mutex in the same atomic
    // action. Hence we can do it after unlocking the scheduler.

    mutex->lock();

    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");
    CYG_ASSERT( mutex->owner == self, "Not mutex owner");
}

// -------------------------------------------------------------------------
// Wake one thread

void Cyg_Condition_Variable::signal()
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");

    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_INSTRUMENT_CONDVAR(SIGNAL, this, 0);
    
    if( !queue.empty() )
    {
        // The queue is non-empty, so grab the next
        // thread from it and wake it up.

        Cyg_Thread *thread = queue.dequeue();

        CYG_ASSERTCLASS( thread, "Bad thread pointer");
        
        thread->set_wake_reason( Cyg_Thread::DONE );
        
        thread->wake();

        CYG_INSTRUMENT_CONDVAR(WAKE, this, thread);
        
    }
    
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();

}

// -------------------------------------------------------------------------
// Set cond true, wake all threads

void Cyg_Condition_Variable::broadcast()
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");

    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_INSTRUMENT_CONDVAR(BROADCAST, this, 0);
    
    // Grab all the threads from the queue and let them
    // go.
    
    while( !queue.empty() )
    {
        Cyg_Thread *thread = queue.dequeue();

        CYG_ASSERTCLASS( thread, "Bad thread pointer");
        
        thread->set_wake_reason( Cyg_Thread::DONE );
        
        thread->wake();

        CYG_INSTRUMENT_CONDVAR(WAKE, this, thread);        
    }
    
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");
    
    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();    
}

// -------------------------------------------------------------------------
// Optional timed wait on a CV

#if defined(CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT) && defined(CYGFUN_KERNEL_THREADS_TIMER)

cyg_bool Cyg_Condition_Variable::wait( cyg_tick_count timeout )
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");

    cyg_bool result = true;
    
    Cyg_Thread *self = Cyg_Thread::self();

    CYG_ASSERTCLASS( self, "Bad self thread");
    
    // Prevent preemption
    Cyg_Scheduler::lock();

    CYG_INSTRUMENT_CONDVAR(TIMED_WAIT, this, 0 );
    
    mutex->unlock();

    // The ordering of sleep() and set_timer() here are
    // important. If the timeout is in the past, the thread
    // will be woken up immediately and will not sleep.
    
    self->sleep();
        
    // Set the timer and sleep reason
    self->set_timer( timeout, Cyg_Thread::TIMEOUT );

    // Only enqueue if the timeout has not already fired.
    if( self->get_wake_reason() == Cyg_Thread::NONE )
        queue.enqueue( self );

    CYG_ASSERT( Cyg_Scheduler::get_sched_lock() == 1, "Called with non-zero scheduler lock");
    
    // Unlock the scheduler and switch threads
    Cyg_Scheduler::unlock();

    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");
    
    self->clear_timer();

    CYG_INSTRUMENT_CONDVAR(WOKE, this, self->get_wake_reason());
    
    switch( self->get_wake_reason() )
    {
    case Cyg_Thread::TIMEOUT:            
    case Cyg_Thread::DESTRUCT:          // which, the cv or the mutex?
    case Cyg_Thread::BREAK:
        result = false;
        break;
            
    case Cyg_Thread::EXIT:            
        self->exit();
        break;

    default:
        break;
    }

    
    // When we awake, we must re-acquire the mutex.  Note that while
    // it is essential to release the mutex and queue on the CV
    // atomically relative to other threads, to avoid races, it is not
    // necessary for us to re-acquire the mutex in the same atomic
    // action. Hence we can do it after unlocking the scheduler.

    mutex->lock();

    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERTCLASS( mutex, "Corrupt mutex");
    
    return result;
}

#endif


// -------------------------------------------------------------------------
// EOF sync/mutex.cxx
