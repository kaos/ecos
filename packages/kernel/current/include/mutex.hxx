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
    
    cyg_atomic          locked;         // true if locked

    Cyg_Thread          *owner;         // Current locking thread

    Cyg_ThreadQueue     queue;          // Queue of waiting threads
    
public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Mutex();                        // Create in unlocked state

    ~Cyg_Mutex();                       // Destructor
        
    cyg_bool    lock();                 // lock and/or wait

    cyg_bool    trylock();              // try to lock and return success

    void        unlock();               // unlock

    void        release();              // release all waiting threads
    
};

// -------------------------------------------------------------------------
// Condition variable.

class Cyg_Condition_Variable
{
    Cyg_Mutex           *mutex;         // Associated mutex

    Cyg_ThreadQueue     queue;          // Queue of waiting threads
    
public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Condition_Variable(
        Cyg_Mutex &mutex                // linked mutex
        );

    ~Cyg_Condition_Variable();          // Destructor
        
    void     wait();                    // Wait for condition to be true

    void     signal();                  // Set cond true, wake one thread

    void     broadcast();               // Set cond true, wake all threads

#ifdef CYGMFN_KERNEL_SYNCH_CONDVAR_TIMED_WAIT

    // Wait until a signal or timeout expiry
    cyg_bool wait( cyg_tick_count timeout );

#endif
    
};


// -------------------------------------------------------------------------

#endif // ifndef CYGONCE_KERNEL_MUTEX_HXX
// EOF mutex.hxx
