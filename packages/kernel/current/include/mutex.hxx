#ifndef CYGONCE_KERNEL_MUTEX_HXX
#define CYGONCE_KERNEL_MUTEX_HXX

//==========================================================================
//
//      mutex.hxx
//
//      Mutex class declarations
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
// Purpose:     Define Mutex class interfaces
// Description: The classes defined here provide the APIs for mutexes
//              and condition variables.
// Usage:       #include <cyg/kernel/mutex.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros

#include <cyg/kernel/thread.hxx>

// -------------------------------------------------------------------------
// Mutex.

class Cyg_Mutex
{
    friend class Cyg_Condition_Variable;
    
    cyg_atomic          locked;         // true if locked. This may seem
                                        // redundant due to "owner" below,
                                        // but is intentionally present for
                                        // future SMP support.

    Cyg_Thread          *owner;         // Current locking thread

    Cyg_ThreadQueue     queue;          // Queue of waiting threads

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC

public:    
    enum cyg_protcol
    {
        NONE = 0,                       // no inversion protocol
        INHERIT,                        // priority inheritance protocol
        CEILING                         // priority ceiling protocol
    };

private:    
    cyg_protcol protocol;               // this mutex's protocol
    
#endif    
    
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING

private:    
    cyg_priority        ceiling;        // mutex priority ceiling
    
#endif
    
public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Mutex();                        // Create in unlocked state

#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_DYNAMIC
    
    Cyg_Mutex( cyg_protcol protocol );  // Create with defined protocol

#endif
    
    ~Cyg_Mutex();                       // Destructor
        
    cyg_bool    lock();                 // lock and/or wait

    cyg_bool    trylock();              // try to lock and return success

    void        unlock();               // unlock

    void        release();              // release all waiting threads

    // Get the current owning thread
    inline Cyg_Thread *get_owner() { return owner; }
    
#ifdef CYGSEM_KERNEL_SYNCH_MUTEX_PRIORITY_INVERSION_PROTOCOL_CEILING

    // set ceiling priority for priority ceiling protocol
    void        set_ceiling( cyg_priority priority );

    cyg_priority get_ceiling(void) { return ceiling; };
    
#endif
    
};

// -------------------------------------------------------------------------
// Condition variable.

class Cyg_Condition_Variable
{
    Cyg_Mutex           *mutex;         // Associated mutex

    Cyg_ThreadQueue     queue;          // Queue of waiting threads

    // Private internal implementation function for wait operations
    cyg_bool wait_inner( Cyg_Mutex *mutex );

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT

    // Private internal implementation function for timed wait operations
    cyg_bool wait_inner( Cyg_Mutex *mutex, cyg_tick_count timeout );

#endif
    
public:

    CYGDBG_DEFINE_CHECK_THIS

    Cyg_Condition_Variable();           // simple constructor
    
    Cyg_Condition_Variable(
        Cyg_Mutex &mutex                // linked mutex
        );

    ~Cyg_Condition_Variable();          // Destructor
        

    void     signal();                  // Set cond true, wake one thread

    void     broadcast();               // Set cond true, wake all threads

    // Wait for condition to be true
    inline cyg_bool wait() { return wait_inner( mutex ); }
    
#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT

    // Wait until a signal or timeout expiry
    inline cyg_bool wait( cyg_tick_count timeout )
    { return wait_inner( mutex, timeout ); }

#endif

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_WAIT_MUTEX

    // Wait for condition to be true using the supplied mutex    
    inline cyg_bool wait( Cyg_Mutex &mx ) { return wait_inner( &mx ); }
    

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT

    // Wait until a signal or timeout expiry, using the supplied mutex
    inline cyg_bool wait( Cyg_Mutex &mx, cyg_tick_count timeout )
    { return wait_inner( &mx, timeout ); }        
    
#endif    
#endif    

    // Return a pointer to this variables thread queue. Used mainly
    // for testing whether a thread is on the queue for a particular
    // cv.
    inline Cyg_ThreadQueue *get_queue() { return &queue; };
    
};


// -------------------------------------------------------------------------

#endif // ifndef CYGONCE_KERNEL_MUTEX_HXX
// EOF mutex.hxx
