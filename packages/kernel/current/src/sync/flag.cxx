//==========================================================================
//
//      flag.cxx
//
//      Flag class implementation
//
//==========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-02-11
// Purpose:     Flag implementation
// Description: This file contains the implementations of the flag class
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>         // base kernel types
#include <cyg/infra/cyg_trac.h>        // tracing macros
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/instrmnt.h>       // instrumentation

#include <cyg/kernel/thread.inl>       // thread inlines

#include <cyg/kernel/flag.hxx>         // our own header

#include <cyg/kernel/sched.inl>        // scheduler inlines
#include <cyg/kernel/clock.inl>        // clock inlines

// -------------------------------------------------------------------------
// Constructor

Cyg_Flag::Cyg_Flag( Cyg_FlagValue init = 0 )
{
    CYG_REPORT_FUNCTION();
    value = init;
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Destructor

Cyg_Flag::~Cyg_Flag()
{
    CYG_REPORT_FUNCTION();
#if 0
    CYG_ASSERT( queue.empty(), "Deleting flag with threads waiting");
#endif
    // Prevent preemption
    Cyg_Scheduler::lock();

    while ( ! queue.empty() ) {
        Cyg_Thread *thread = queue.dequeue();
        thread->set_wake_reason( Cyg_Thread::DESTRUCT );
        thread->wake();
    }

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();    
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// debugging/assert function

#ifdef CYGDBG_USE_ASSERTS
cyg_bool 
Cyg_Flag::check_this(cyg_assert_class_zeal zeal) const
{
    CYG_REPORT_FUNCTION();
        
    if ( Cyg_Thread::DESTRUCT == Cyg_Thread::self()->get_wake_reason() )
        // then the whole thing is invalid, and we know it.
        // so return OK, since this check should NOT make an error.
        return true;

    // check that we have a non-NULL pointer first
    if( this == NULL ) {
        CYG_REPORT_RETVAL( false );
        return false;
    }

    // there ain't a lot to check here.
    CYG_REPORT_RETVAL( true );
    return true;
}
#endif

// -------------------------------------------------------------------------
// now the members themselves:
    
// clear some bits in the value (all of them by default) by ANDing with the
// argument.  This cannot make a wait condition become true, so there's not
// much to it.
void
Cyg_Flag::maskbits( Cyg_FlagValue arg = 0 )
{
    CYG_REPORT_FUNCTION();
  
    // Prevent preemption
    Cyg_Scheduler::lock();

    value &= arg;
    // no need to wake anyone up; no waiter can become valid in
    // consequence of this operation.

    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();
    CYG_REPORT_RETURN();
}


// -------------------------------------------------------------------------
// set some bits in the value (all of them by default) and wake up any
// affected waiting threads; we do the decision making here so as to get
// atomicity wrt the other threads waking up - the value might have changed
// by the time they get to run.

void
Cyg_Flag::setbits( Cyg_FlagValue arg = ~0 )
{
    CYG_REPORT_FUNCTION();
    CYG_ASSERTCLASS( this, "Bad this pointer");

    // Prevent preemption
    Cyg_Scheduler::lock();
    
    // OR in the argument to get a new flag value.
    value |= arg;

    // anyone waiting?
    if ( !(queue.empty()) ) {
        FlagWaitInfo   *p;
        Cyg_Thread     *thread;
        Cyg_ThreadQueue holding;

        do {
            thread = queue.dequeue();
            p = (FlagWaitInfo *)(thread->get_wait_info());

            CYG_ASSERT( (p->allmask == 0) != (p->anymask == 0),
                        "Both masks set" );
            CYG_ASSERT( 0 == p->value_out, "Thread already awoken?" );

            if ( ((p->allmask != 0) && (p->allmask & value) == p->allmask) ||
                 ((p->anymask & value) != 0 ) ) {
                // success!  awaken the thread
                thread->set_wake_reason( Cyg_Thread::DONE );
                thread->wake();
                // return the successful value to it
                p->value_out = value;
                // do we clear the value; is this the end?
                if ( p->do_clear ) {
                    // we can break here but need to preserve ordering
                    value = 0;
                    // so let it cycle the whole queue regardless
                }
            }
            else {
                // preserve the entry on the holding queue
                holding.enqueue( thread );
            }
        } while ( !(queue.empty()) );
            
        // Now re-queue the unaffected threads back into the flag queue
        while ( !(holding.empty()) ) {
            queue.enqueue( holding.dequeue() );
        }
    }
    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();
    CYG_REPORT_RETURN();
}

// -------------------------------------------------------------------------
// Wait for a match on our pattern, according to the flags given.
// Return the matching value.
Cyg_FlagValue
Cyg_Flag::wait( Cyg_FlagValue pattern, WaitMode mode )
{
    CYG_REPORT_FUNCTION();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERT( Cyg_Flag::MASK >= mode, "Bad mode" );

    Cyg_FlagValue result;

    // Prevent preemption so that we compare atomically
    Cyg_Scheduler::lock();

    // try the current value
    result = poll( pattern, mode );

    if ( 0 != result ) {
        Cyg_Scheduler::unlock();
        CYG_REPORT_RETVAL( result );
        return result;                  // all done
    }

    // we have to wait until we are awoken
    Cyg_Thread *self = Cyg_Thread::self();

    FlagWaitInfo saveme;
    saveme.allmask = (Cyg_Flag::OR & mode) ? 0 : pattern;
    saveme.anymask = (Cyg_Flag::OR & mode) ? pattern : 0;
    saveme.do_clear = (0 != (Cyg_Flag::CLR & mode));

    self->set_wait_info( (CYG_ADDRWORD)&saveme );

    result = true; // just being used as an early-out flag now
    // this loop allows us to deal correctly with spurious wakeups
    while ( result && (0 == saveme.value_out) ) {
        self->set_sleep_reason( Cyg_Thread::WAIT );
        self->sleep();
        // keep track of myself on the queue of waiting threads
        queue.enqueue( self );

        CYG_ASSERT( 1 == Cyg_Scheduler::get_sched_lock(),
                    "Called with non-zero scheduler lock");
        
        // Unlock scheduler and allow other threads to run
        Cyg_Scheduler::unlock();
        Cyg_Scheduler::lock();

        CYG_ASSERT( ((CYG_ADDRWORD)&saveme) == 
                    Cyg_Thread::self()->get_wait_info(),
                    "Wait info lost" );

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

    CYG_ASSERT( (false == result) ^ (0 != saveme.value_out),
                "Break out but also good result!" );

    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();
    CYG_REPORT_RETVAL( saveme.value_out );
    return saveme.value_out;
}

// -------------------------------------------------------------------------
// Wait for a match on our pattern, with a timeout.
// Return the matching value, or zero if timed out.
// (zero cannot match any pattern).
#ifdef CYGFUN_KERNEL_THREADS_TIMER
Cyg_FlagValue
Cyg_Flag::wait( Cyg_FlagValue pattern, WaitMode mode,
                cyg_tick_count abs_timeout )
{
    CYG_REPORT_FUNCTION();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERT( Cyg_Flag::MASK >= mode, "Bad mode" );

    Cyg_FlagValue result;

    // Prevent preemption so that we compare atomically
    Cyg_Scheduler::lock();

    // try the current value
    result = poll( pattern, mode );

    if ( 0 != result ) {
        Cyg_Scheduler::unlock();
        CYG_REPORT_RETVAL( result );
        return result;                  // all done
    }

    // we have to wait until we are awoken
    Cyg_Thread *self = Cyg_Thread::self();

    FlagWaitInfo saveme;
    saveme.allmask = (Cyg_Flag::OR & mode) ? 0 : pattern;
    saveme.anymask = (Cyg_Flag::OR & mode) ? pattern : 0;
    saveme.do_clear = (0 != (Cyg_Flag::CLR & mode));

    self->set_wait_info( (CYG_ADDRWORD)&saveme );

    // Set the timer _once_ outside the loop.
    self->set_timer( abs_timeout, Cyg_Thread::TIMEOUT );

    // If the timeout was in the past, it will have changed the value
    // of wake_reason, so avoid going into the loop.
    if( self->get_wake_reason() != Cyg_Thread::NONE )
        result = false;
    else result = true;

    // Result is just being used as an early-out flag now. This loop
    // allows us to deal correctly with spurious wakeups.

    while ( result && (0 == saveme.value_out) ) {
        self->set_sleep_reason( Cyg_Thread::TIMEOUT );
        self->sleep();
        // keep track of myself on the queue of waiting threads
        queue.enqueue( self );

        CYG_ASSERT( 1 == Cyg_Scheduler::get_sched_lock(),
                    "Called with non-zero scheduler lock");
        
        // Unlock scheduler and allow other threads to run
        Cyg_Scheduler::unlock();
        Cyg_Scheduler::lock();

        CYG_ASSERT( ((CYG_ADDRWORD)&saveme) == 
                    Cyg_Thread::self()->get_wait_info(),
                    "Wait info lost" );

        switch( self->get_wake_reason() )
        {
        case Cyg_Thread::TIMEOUT:
            result = false;
            break;
            
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

    CYG_ASSERT( (false == result) ^ (0 != saveme.value_out),
                "Break out but also good result!" );

    // clear the timer; if it actually fired, no worries.
    self->clear_timer();

    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();
    // in this version, value_out might be zero meaning timed out.
    CYG_REPORT_RETVAL( saveme.value_out );
    return saveme.value_out;
}
#endif // CYGFUN_KERNEL_THREADS_TIMER

// -------------------------------------------------------------------------
// Test for a match on our pattern, according to the flags given.
// Return the matching value if success, else zero.
Cyg_FlagValue
Cyg_Flag::poll( Cyg_FlagValue pattern, WaitMode mode )
{
    CYG_REPORT_FUNCTION();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_ASSERT( Cyg_Flag::MASK >= mode, "Bad mode" );

    // Prevent preemption so that we compare atomically
    Cyg_Scheduler::lock();

    Cyg_FlagValue result = 0;

    if ( Cyg_Flag::OR & mode ) {
        if ( 0 != (value & pattern) )
            result = value;
    }
    else { // Cyg_Flag::AND - all must be set
        if ( (pattern != 0) && (pattern == (value & pattern)) )
            result = value;
    }

    // result != 0 <=> test passed
    if ( result && (Cyg_Flag::CLR & mode) )
        value = 0;

    Cyg_Scheduler::unlock();

    CYG_REPORT_RETVAL( result );
    return result;
}


// -------------------------------------------------------------------------
// EOF flag.cxx
