#ifndef CYGONCE_KERNEL_SCHED_INL
#define CYGONCE_KERNEL_SCHED_INL

//==========================================================================
//
//      sched.inl
//
//      Scheduler class inlines
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
// Date:        1997-09-09
// Purpose:     Define inlines for scheduler classes
// Description: Inline functions for the scheduler classes. These are
//              not defined in the header so that we have the option
//              of making them non-inline.
// Usage:
//              #include <cyg/kernel/sched.hxx>
//              ...
//              #include <cyg/kernel/sched.inl>
//              ...
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/instrmnt.h>

#include <cyg/kernel/thread.inl>   // we use some thread inlines here

// -------------------------------------------------------------------------
// Inlines for Cyg_Scheduler class

inline void Cyg_Scheduler::lock()
{
    // We do not need to do a read-modify-write sequence here because
    // the scheduler lock is strictly nesting. Even if we are interrupted
    // partway through the increment, the lock will be returned to the same
    // value before we are resumed/rescheduled.

    HAL_REORDER_BARRIER();

    sched_lock++;

    HAL_REORDER_BARRIER();

    CYG_INSTRUMENT_SCHED(LOCK,sched_lock,0);
};

inline void Cyg_Scheduler::unlock()
{
    // This is an inline wrapper for the real scheduler unlock function in
    // Cyg_Scheduler::unlock_inner().
    
    // Only do anything if the lock is about to go zero, otherwise we simply
    // decrement and return. As with lock() we do not need any special code
    // to decrement the lock counter.

    CYG_INSTRUMENT_SCHED(UNLOCK,sched_lock,0);

    HAL_REORDER_BARRIER();
    
    cyg_ucount32 __lock = sched_lock - 1;
    
    if( __lock == 0 ) unlock_inner();
    else sched_lock = __lock;

    HAL_REORDER_BARRIER();
}

inline void Cyg_Scheduler::unlock_simple()
{
    // This function decrements the lock, but does not call unlock_inner().
    // Therefore does not immediately allow another thread to run:
    // merely makes it possible for some other thread to run at some
    // indeterminate future time.  This is mainly for use by
    // debuggers, it should not normally be used anywhere else.

    CYG_INSTRUMENT_SCHED(UNLOCK,sched_lock,0);

    HAL_REORDER_BARRIER();
        
    if (sched_lock > 0)
        sched_lock = sched_lock - 1;

    HAL_REORDER_BARRIER();
}


// -------------------------------------------------------------------------
// Inlines for Cyg_SchedThread class

inline void Cyg_SchedThread::remove()
{
    if( queue != NULL )
    {
        queue->remove((Cyg_Thread *)this);
        queue = NULL;
    }
}

// -------------------------------------------------------------------------

#endif // ifndef CYGONCE_KERNEL_SCHED_INL
// EOF sched.inl
