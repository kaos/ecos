#ifndef CYGONCE_KERNEL_MEMPOLT2_INL
#define CYGONCE_KERNEL_MEMPOLT2_INL

//==========================================================================
//
//	mempolt2.inl
//
//	Mempolt2 (Memory pool template) class declarations
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
// Author(s): 	hmt
// Contributors:	hmt
// Date:	1998-02-10
// Purpose:	Define Mempolt2 class interface

// Description: The class defined here provides the APIs for thread-safe,
//              kernel-savvy memory managers; make a class with the
//              underlying allocator as the template parameter.
// Usage:	#include <cyg/kernel/mempolt2.hxx>
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/thread.inl>  // implementation eg. Cyg_Thread::self();
#include <cyg/kernel/sched.inl>   // implementation eg. Cyg_Scheduler::lock();

// -------------------------------------------------------------------------
// Constructor; we _require_ these arguments and just pass them through to
// the implementation memory pool in use.
template <class T>
Cyg_Mempolt2<T>::Cyg_Mempolt2(
    cyg_uint8 *base,
    cyg_int32 size,
    CYG_ADDRWORD arg_thru)              // Constructor
    : pool( base, size, arg_thru )
{
}


template <class T>
Cyg_Mempolt2<T>::~Cyg_Mempolt2()  // destructor
{
    // Prevent preemption
    Cyg_Scheduler::lock();
            
    while ( ! queue.empty() ) {
        Cyg_Thread *thread = queue.dequeue();
        thread->set_wake_reason( Cyg_Thread::DESTRUCT );
        thread->wake();
    }

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();    
}
        
// -------------------------------------------------------------------------
// get some memory; wait if none available
template <class T>
inline cyg_uint8 *
Cyg_Mempolt2<T>::alloc( cyg_int32 size )
{
    CYG_REPORT_FUNCTION();
        
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_uint8 *ret;
    ret = pool.alloc( size );
    if ( ret ) {
        Cyg_Scheduler::unlock();
        CYG_ASSERTCLASS( this, "Bad this pointer");
        CYG_REPORT_RETVAL( ret );
        return ret;
    }

    Cyg_Thread *self = Cyg_Thread::self();

    Mempolt2WaitInfo waitinfo( size );

    self->set_wait_info( (CYG_ADDRWORD)&waitinfo );
    self->set_sleep_reason( Cyg_Thread::WAIT );
    self->sleep();
    queue.enqueue( self );

    CYG_ASSERT( 1 == Cyg_Scheduler::get_sched_lock(),
                "Called with non-zero scheduler lock");
        
    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();

    cyg_bool result = true; // just used as a flag here
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

    if ( ! result )
        ret = NULL;
    else
        ret = waitinfo.addr;

    CYG_ASSERT( (!result) || (NULL != ret), "Good result but no alloc!" );
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_REPORT_RETVAL( ret );
    return ret;
}

#ifdef CYGFUN_KERNEL_THREADS_TIMER
// -------------------------------------------------------------------------
// get some memory with a timeout
template <class T>
inline cyg_uint8 *
Cyg_Mempolt2<T>::alloc( cyg_int32 size, cyg_tick_count abs_timeout )
{
    CYG_REPORT_FUNCTION();
    
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_uint8 *ret;
    ret = pool.alloc( size );
    if ( ret ) {
        Cyg_Scheduler::unlock();
        CYG_ASSERTCLASS( this, "Bad this pointer");
        CYG_REPORT_RETVAL( ret );
        return ret;
    }

    Cyg_Thread *self = Cyg_Thread::self();

    Mempolt2WaitInfo waitinfo( size );

    self->set_timer( abs_timeout, Cyg_Thread::TIMEOUT );

    // If the timeout is in the past, the wake reason will have been set to
    // something other than NONE already. If so, skip the wait and go
    // straight to unlock.
    
    if( Cyg_Thread::NONE == self->get_wake_reason() ) {
        self->set_wait_info( (CYG_ADDRWORD)&waitinfo );
        self->sleep();
        queue.enqueue( self );
    }

    CYG_ASSERT( 1 == Cyg_Scheduler::get_sched_lock(),
                "Called with non-zero scheduler lock");
        
    // Unlock scheduler and allow other threads to run
    Cyg_Scheduler::unlock();

    // clear the timer; if it actually fired, no worries.
    self->clear_timer();

    cyg_bool result = true; // just used as a flag here
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

    if ( ! result )
        ret = NULL;
    else
        ret = waitinfo.addr;

    CYG_ASSERT( (!result) || (NULL != ret), "Good result but no alloc!" );
    CYG_ASSERTCLASS( this, "Bad this pointer");
    CYG_REPORT_RETVAL( ret );
    return ret;
}
#endif 

// -------------------------------------------------------------------------
// get some memory, return NULL if none available
template <class T>
inline cyg_uint8 *
Cyg_Mempolt2<T>::try_alloc( cyg_int32 size )
{
    CYG_REPORT_FUNCTION();
        
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_uint8 *ret = pool.alloc( size );

    CYG_ASSERTCLASS( this, "Bad this pointer");

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
    return ret;
}
    
    
// -------------------------------------------------------------------------
// free the memory back to the pool
template <class T>
cyg_bool
Cyg_Mempolt2<T>::free( cyg_uint8 *p, cyg_int32 size )
{
    CYG_REPORT_FUNCTION();
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_int32 ret = pool.free( p, size );

    // anyone waiting?
    if ( !(queue.empty()) ) {
        Mempolt2WaitInfo *p;
        Cyg_Thread     *thread;

#ifdef CYGIMP_MEM_T_ONEFREE_TO_ONEALLOC
        thread = queue.dequeue();
        p = (Mempolt2WaitInfo *)(thread->get_wait_info());
        CYG_ASSERT( NULL == p->addr, "Thread already awoken?" );

        cyg_uint8 *mem;
        mem = pool.alloc( p->size );
        CYG_ASSERT( NULL != mem, "That should have succeeded" );
        thread->set_wake_reason( Cyg_Thread::DONE );
        thread->wake();
        // return the successful value to it
        p->addr = mem;
#else
        Cyg_ThreadQueue holding;
        do {
            thread = queue.dequeue();
            p = (Mempolt2WaitInfo *)(thread->get_wait_info());
            CYG_ASSERT( NULL == p->addr, "Thread already awoken?" );

            cyg_uint8 *mem;
            if ( NULL != (mem = pool.alloc( p->size )) ) {
                // success!  awaken the thread
                thread->set_wake_reason( Cyg_Thread::DONE );
                thread->wake();
                // return the successful value to it
                p->addr = mem;
            }
            else {
                // preserve the entry on the holding queue
                holding.enqueue( thread );
            }
        } while ( !(queue.empty()) );
            
        // Now re-queue the unaffected threads back into the pool queue
        // (no pun intended)
        while ( !(holding.empty()) ) {
            queue.enqueue( holding.dequeue() );
        }
#endif // CYGIMP_MEM_T_ONEFREE_TO_ONEALLOC
    }
    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
    CYG_REPORT_RETVAL( ret );
    return ret;
}

// -------------------------------------------------------------------------
// if applicable: return -1 if not fixed size
template <class T>
inline cyg_int32
Cyg_Mempolt2<T>::get_blocksize()
{
    // there should not be any atomicity issues here
    return pool.get_blocksize();
}

// -------------------------------------------------------------------------
// these two are obvious and generic, but need atomicity protection (maybe)
template <class T>
inline cyg_int32
Cyg_Mempolt2<T>::get_totalmem()
{
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_int32 ret = pool.get_totalmem();

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
    return ret;
}

template <class T>
inline cyg_int32
Cyg_Mempolt2<T>::get_freemem()
{
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    cyg_int32 ret = pool.get_freemem();

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
    return ret;
}

// -------------------------------------------------------------------------
// get information about the construction parameters for external
// freeing after the destruction of the holding object
template <class T>
inline void
Cyg_Mempolt2<T>::get_arena(
    cyg_uint8 * &base, cyg_int32 &size, CYG_ADDRWORD &arg_thru )
{
    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    pool.get_arena( base, size, arg_thru );

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();
}

// -------------------------------------------------------------------------
// Return the size of the memory allocation (previously returned 
// by alloc() or try_alloc() ) at ptr. Returns -1 if not found
template <class T>
cyg_int32
Cyg_Mempolt2<T>::get_allocation_size( cyg_uint8 *ptr )
{
    cyg_int32 ret;

    // Prevent preemption
    Cyg_Scheduler::lock();
    CYG_ASSERTCLASS( this, "Bad this pointer");
    
    ret = pool.get_allocation_size( ptr );

    // Unlock the scheduler and maybe switch threads
    Cyg_Scheduler::unlock();

    return ret;
}

// -------------------------------------------------------------------------
// debugging/assert function

#ifdef CYGDBG_USE_ASSERTS

template <class T>
inline cyg_bool
Cyg_Mempolt2<T>::check_this(cyg_assert_class_zeal zeal)
{
    CYG_REPORT_FUNCTION();
        
    if ( Cyg_Thread::DESTRUCT == Cyg_Thread::self()->get_wake_reason() )
        // then the whole thing is invalid, and we know it.
        // so return OK, since this check should NOT make an error.
        return true;

    // check that we have a non-NULL pointer first
    if( this == NULL ) return false;

    return true;
}
#endif

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MEMPOLT2_INL
// EOF mempolt2.inl
