#ifndef CYGONCE_KERNEL_MLQUEUE_HXX
#define CYGONCE_KERNEL_MLQUEUE_HXX

//==========================================================================
//
//      mlqueue.hxx
//
//      Multi-Level Queue scheduler class declarations
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
// Date:        1997-09-10
// Purpose:     Define multilevel queue scheduler implementation
// Description: The classes defined here are used as base classes
//              by the common classes that define schedulers and thread
//              things. The MLQ scheduler in various configurations
//              provides standard FIFO, round-robin and single priority
//              schedulers.
// Usage:       Included according to configuration by
//              <cyg/kernel/sched.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>

// -------------------------------------------------------------------------
// The macro CYGNUM_KERNEL_SCHED_PRIORITIES contains the number of priorities
// supported by the scheduler.

#ifndef CYGNUM_KERNEL_SCHED_PRIORITIES
#define CYGNUM_KERNEL_SCHED_PRIORITIES 32       // define a default
#endif

// set bitmap size
#define CYGNUM_KERNEL_SCHED_BITMAP_SIZE CYGNUM_KERNEL_SCHED_PRIORITIES

// -------------------------------------------------------------------------
// The macro CYGNUM_KERNEL_SCHED_BITMAP_SIZE contains the number of bits that the
// scheduler bitmap should contain. It is derived from the number of prioirity
// levels defined by the configuration.

#if CYGNUM_KERNEL_SCHED_BITMAP_SIZE <= 8
typedef cyg_ucount8 cyg_sched_bitmap;
#elif CYGNUM_KERNEL_SCHED_BITMAP_SIZE <= 16
typedef cyg_ucount16 cyg_sched_bitmap;
#elif CYGNUM_KERNEL_SCHED_BITMAP_SIZE <= 32
typedef cyg_ucount32 cyg_sched_bitmap;
#else
#error Bitmaps greater than 32 bits not currently allowed
#endif

// -------------------------------------------------------------------------
// Customize the scheduler

#define CYGIMP_THREAD_PRIORITY  1       // Threads have changable priorities
#define CYG_SCHED_UNIQUE_PRIORITIES 0   // Threads don't have unique priorities

#define CYG_THREAD_MIN_PRIORITY (CYGNUM_KERNEL_SCHED_PRIORITIES-1)
#define CYG_THREAD_MAX_PRIORITY 0

// set default scheduling info value for thread constructors.
#define CYG_SCHED_DEFAULT_INFO  CYG_THREAD_MAX_PRIORITY

// -------------------------------------------------------------------------
// Thread queue implementation.
// This class provides the (scheduler specific) implementation of the
// thread queue class.

class Cyg_ThreadQueue_Implementation
{
    friend class Cyg_Scheduler_Implementation;
    friend class Cyg_SchedThread_Implementation;
    
    Cyg_Thread *queue;

protected:

    // API used by Cyg_ThreadQueue

    Cyg_ThreadQueue_Implementation();   // Constructor
    
                                        // Add thread to queue
    void                enqueue(Cyg_Thread *thread);

                                        // return first thread on queue
    Cyg_Thread          *highpri();

                                        // remove first thread on queue    
    Cyg_Thread          *dequeue();

                                        // remove specified thread from queue    
    void                remove(Cyg_Thread *thread);

                                        // test if queue is empty
    cyg_bool            empty();

    void                rotate();       // Rotate the queue

    void                to_head(Cyg_Thread *thread);

};

inline cyg_bool Cyg_ThreadQueue_Implementation::empty()
{
    return queue == NULL;
}

// -------------------------------------------------------------------------
// This class contains the implementation details of the scheduler, and
// provides a standard API for accessing it.

class Cyg_Scheduler_Implementation
    : public Cyg_Scheduler_Base
{
    friend class Cyg_ThreadQueue_Implementation;
    friend class Cyg_SchedThread_Implementation;
    friend class Cyg_HardwareThread;
    
    // Mask of which run queues have ready threads
    cyg_sched_bitmap    queue_map;

    // Each run queue is a double linked circular list of threads.
    // These pointers point to the head element of each list.
    Cyg_ThreadQueue_Implementation     run_queue[CYGNUM_KERNEL_SCHED_PRIORITIES];

protected:
    
#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE

    // Timeslice counter. This is decremented on each
    // clock tick, and a timeslice is performed each
    // time it zeroes.
    
    static cyg_ucount32                 timeslice_count;

    static void reset_timeslice_count();
    
#endif
    
    Cyg_Scheduler_Implementation();     // Constructor
    
    // The following functions provide the scheduler implementation
    // interface to the Cyg_Scheduler class. These are protected
    // so that only the scheduler can call them.
    
    // choose a new thread
    Cyg_Thread  *schedule();

    // make thread schedulable
    void        add_thread(Cyg_Thread *thread);

    // make thread un-schedulable
    void        rem_thread(Cyg_Thread *thread);

    // register thread with scheduler
    void        register_thread(Cyg_Thread *thread);

    // deregister thread
    void        deregister_thread(Cyg_Thread *thread);
    
    // Test the given priority for uniqueness
    cyg_bool    unique( cyg_priority priority);

#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE

    // If timeslicing is enbled, define a scheduler
    // entry point to do timeslicing. This will be
    // called from the RTC DSR.
public:    
    void timeslice();
    
#endif

};

// -------------------------------------------------------------------------
// Cyg_Scheduler_Implementation inlines

#ifdef CYGSEM_KERNEL_SCHED_TIMESLICE

inline void Cyg_Scheduler_Implementation::reset_timeslice_count()
{
    timeslice_count = CYGNUM_KERNEL_SCHED_TIMESLICE_TICKS;
}

#endif

// -------------------------------------------------------------------------
// Scheduler thread implementation.
// This class provides the implementation of the scheduler specific parts
// of each thread.

class Cyg_SchedThread_Implementation
{
    friend class Cyg_Scheduler_Implementation;
    friend class Cyg_ThreadQueue_Implementation;

    Cyg_Thread *next;                   // next thread in queue
    Cyg_Thread *prev;                   // previous thread in queue
        
    void insert( Cyg_Thread *thread );  // Insert thread in front of this

    void remove();                      // remove this from queue
    
protected:

    cyg_priority        priority;       // current thread priority

    Cyg_SchedThread_Implementation(CYG_ADDRWORD sched_info);

    void yield();                       // Yield CPU to next thread

    static void rotate_queue( cyg_priority pri );
                                        // Rotate that run queue

    void to_queue_head( void );         // Move this thread to the head
                                        // of its queue (not necessarily
                                        // a scheduler queue)
};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MLQUEUE_HXX
// EOF mlqueue.hxx
