#ifndef CYGONCE_KERNEL_BITMAP_HXX
#define CYGONCE_KERNEL_BITMAP_HXX

//==========================================================================
//
//      bitmap.hxx
//
//      Bitmap scheduler class declaration(s)
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
// Purpose:     Define bitmap scheduler implementation
// Description: The classes defined here are used as base classes
//              by the common classes that define schedulers and thread
//              things.
// Usage:       Included according to configuration by
//              <cyg/kernel/sched.hxx>
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>

// -------------------------------------------------------------------------
// The macro CYGNUM_KERNEL_SCHED_BITMAP_SIZE contains the number of bits 
// that the scheduler bitmap should contain. It is derived from the number
// of threads that the system is allowed to use during configuration.

#ifndef CYGNUM_KERNEL_SCHED_BITMAP_SIZE
#define CYGNUM_KERNEL_SCHED_BITMAP_SIZE 32
#endif

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

#define CYGIMP_THREAD_PRIORITY  1

#define CYG_THREAD_MIN_PRIORITY (CYGNUM_KERNEL_SCHED_BITMAP_SIZE-1)
#define CYG_THREAD_MAX_PRIORITY 0

// set default scheduling info value for thread constructors.
#define CYG_SCHED_DEFAULT_INFO  CYG_THREAD_MAX_PRIORITY

// -------------------------------------------------------------------------
// This class contains the implementation details of the scheduler, and
// provides a standard API for accessing it.

class Cyg_Scheduler_Implementation
    : public Cyg_Scheduler_Base
{
    friend class Cyg_ThreadQueue_Implementation;
    friend class Cyg_SchedThread_Implementation;
    
    cyg_sched_bitmap    run_queue;

    Cyg_Thread          *thread_table[CYGNUM_KERNEL_SCHED_BITMAP_SIZE];

    
protected:

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
    
};

// -------------------------------------------------------------------------
// Scheduler thread implementation.
// This class provides the implementation of the scheduler specific parts
// of each thread.

class Cyg_SchedThread_Implementation
{
    friend class Cyg_Scheduler_Implementation;
    friend class Cyg_ThreadQueue_Implementation;
    
protected:

    cyg_priority        priority;       // current thread priority

    Cyg_SchedThread_Implementation(CYG_ADDRWORD sched_info);

    void yield();                       // Yield CPU to next thread

    // These are not applicable in a bitmap scheduler; placeholders:
    inline void rotate_queue( cyg_priority pri ) { };
    inline void to_queue_head( void ) { };
};

// -------------------------------------------------------------------------
// Thread queue implementation.
// This class provides the (scheduler specific) implementation of the
// thread queue class.

class Cyg_ThreadQueue_Implementation
{
    cyg_sched_bitmap    wait_queue;

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

};

inline cyg_bool Cyg_ThreadQueue_Implementation::empty()
{
    return wait_queue == 0;
}

// -------------------------------------------------------------------------

#endif // ifndef CYGONCE_KERNEL_BITMAP_HXX
// EOF bitmap.hxx
