//==========================================================================
//
//      sched/bitmap.cxx
//
//      Bitmap scheduler class implementation
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
// Date:        1997-09-16
// Purpose:     Bitmap scheduler class implementation
// Description: This file contains the implementations of
//              Cyg_Scheduler_Implementation and Cyg_SchedThread_Implementation.
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>             // base kernel types
#include <cyg/infra/cyg_trac.h>           // tracing macros
#include <cyg/infra/cyg_ass.h>            // assertion macros

#include <cyg/kernel/sched.hxx>            // our header

#include <cyg/hal/hal_arch.h>           // Architecture specific definitions

#include <cyg/kernel/thread.inl>           // thread inlines
#include <cyg/kernel/sched.inl>            // scheduler inlines

#ifdef CYGSEM_KERNEL_SCHED_BITMAP

//==========================================================================
// Cyg_Scheduler_Implementation class members

// -------------------------------------------------------------------------
// Constructor.

Cyg_Scheduler_Implementation::Cyg_Scheduler_Implementation()
{
    CYG_REPORT_FUNCTION();
        
    // At present we cannot init run_queue here because the absence of
    // ordering of static constructors means that we could do this
    // after the static idle thread has been created. (Guess how I
    // found this out!)    
//    run_queue = 0;

}

// -------------------------------------------------------------------------
// Choose the best thread to run next

Cyg_Thread *Cyg_Scheduler_Implementation::schedule()
{
    CYG_REPORT_FUNCTION();
        
    // The run queue may _never_ be empty, there is always
    // an idle thread at the lowest priority.
    
    CYG_ASSERT(run_queue != 0, "Run queue empty");

    cyg_uint32 index;

    HAL_LSBIT_INDEX(index, run_queue);
    
    return thread_table[index];
}

// -------------------------------------------------------------------------

void Cyg_Scheduler_Implementation::add_thread(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERT((CYG_THREAD_MIN_PRIORITY >= thread->priority) 
               && (CYG_THREAD_MAX_PRIORITY <= thread->priority),
               "Priority out of range!");

    CYG_ASSERT( thread_table[thread->priority] == NULL ||
                thread_table[thread->priority] == thread,
                "Duplicate thread priorities" );

    CYG_ASSERT( (run_queue & (1<<thread->priority)) == 0,
                "Run queue bit already set" );

    // If the thread is on some other queue, remove it
    // here.
    if( thread->queue != NULL )
    {
        thread->queue->remove(thread);
        thread->queue = NULL;
    }
    
    run_queue |= 1<<thread->priority;

    // If the new thread is higher priority than the
    // current thread, request a reschedule.

    if( thread->priority < Cyg_Scheduler::get_current_thread()->priority )
        need_reschedule = true;
}

// -------------------------------------------------------------------------

void Cyg_Scheduler_Implementation::rem_thread(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    CYG_ASSERT( thread_table[thread->priority] == thread,
                "Invalid thread priority" );
    
    CYG_ASSERT( (run_queue & (1<<thread->priority)) != 0,
                "Run queue bit not set" );

    run_queue &= ~(1<<thread->priority);

    if( thread == Cyg_Scheduler::get_current_thread() )
        need_reschedule = true;
}

// -------------------------------------------------------------------------
// register thread with scheduler

void Cyg_Scheduler_Implementation::register_thread(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    thread_table[thread->priority] = thread;
}

// -------------------------------------------------------------------------

// deregister thread
void Cyg_Scheduler_Implementation::deregister_thread(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    thread_table[thread->priority] = NULL;
}
    
// -------------------------------------------------------------------------
// Test the given priority for uniqueness

cyg_bool Cyg_Scheduler_Implementation::unique( cyg_priority priority)
{
    CYG_REPORT_FUNCTION();
        
    return thread_table[priority] == NULL;
}


//==========================================================================
// Cyg_Cyg_SchedThread_Implementation class members

Cyg_SchedThread_Implementation::Cyg_SchedThread_Implementation
(
    CYG_ADDRWORD sched_info
)
{
    CYG_REPORT_FUNCTION();

#if 1
    // Assign this thread's priority to the supplied sched_info
    // or the next highest priority available.
    
    priority = cyg_priority(sched_info);

    while( !Cyg_Scheduler::scheduler.unique(priority) )
        priority++;
    
#else    
    // Assign initial priorities to threads in descending order of
    // creation.

    static cyg_priority init_priority = 0;
    
    priority = init_priority++;
#endif
    
}

// -------------------------------------------------------------------------

void Cyg_SchedThread_Implementation::yield()
{
    CYG_REPORT_FUNCTION();
        
    // We cannot yield in this scheduler
}

//==========================================================================
// Cyg_ThreadQueue_Implementation class members

Cyg_ThreadQueue_Implementation::Cyg_ThreadQueue_Implementation()
{
    CYG_REPORT_FUNCTION();
        
    wait_queue = 0;                       // empty queue

    CYG_REPORT_RETURN();
}


void Cyg_ThreadQueue_Implementation::enqueue(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    wait_queue |= 1<<thread->priority;
    thread->queue = CYG_CLASSFROMBASE(Cyg_ThreadQueue,
                                      Cyg_ThreadQueue_Implementation,
                                      this);
}

// -------------------------------------------------------------------------

Cyg_Thread *Cyg_ThreadQueue_Implementation::dequeue()
{
    CYG_REPORT_FUNCTION();
        
    // Isolate ls bit in run_queue.
    cyg_sched_bitmap next_thread = wait_queue & -wait_queue;

    if( next_thread == 0 ) return NULL;

    wait_queue &= ~next_thread;

    cyg_uint32 index;

    HAL_LSBIT_INDEX(index, next_thread);
    
    Cyg_Thread *thread = Cyg_Scheduler::scheduler.thread_table[index];

    thread->queue = NULL;

    return thread;
}

// -------------------------------------------------------------------------

Cyg_Thread *Cyg_ThreadQueue_Implementation::highpri()
{
    CYG_REPORT_FUNCTION();
        
    // Isolate ls bit in run_queue.
    cyg_sched_bitmap next_thread = wait_queue & -wait_queue;

    if( next_thread == 0 ) return NULL;

    cyg_uint32 index;

    HAL_LSBIT_INDEX(index, next_thread);
    
    return Cyg_Scheduler::scheduler.thread_table[index];
}

// -------------------------------------------------------------------------

void Cyg_ThreadQueue_Implementation::remove(Cyg_Thread *thread)
{
    CYG_REPORT_FUNCTION();
        
    wait_queue &= ~(1<<thread->priority);
    thread->queue = NULL;
}

#endif

// -------------------------------------------------------------------------
// EOF sched/bitmap.cxx
