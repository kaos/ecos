#ifndef CYGONCE_KERNEL_SEMA_HXX
#define CYGONCE_KERNEL_SEMA_HXX

//==========================================================================
//
//      sema.hxx
//
//      Semaphore class declarations
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
// Purpose:     Define Semaphore class interfaces
// Description: The classes defined here provide the APIs for binary
//              and counting semaphores.
// Usage:       #include <cyg/kernel/sema.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>         // assertion macros
#include <cyg/kernel/thread.hxx>

// -------------------------------------------------------------------------
// Binary semaphore. This has only two states: posted and not-posted.

class Cyg_Binary_Semaphore
{
    cyg_bool            state;          // The binary semaphore state

    Cyg_ThreadQueue     queue;          // Queue of waiting threads

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Binary_Semaphore(               // Constructor
        cyg_bool init_state = false     // Initial state value
        );

    ~Cyg_Binary_Semaphore();            // Destructor
        
    cyg_bool    wait();                 // Wait until state == true

    cyg_bool    trywait();              // Set state false if possible
        
    void        post();                 // Increment count

    cyg_bool    posted();               // Get current state
    
};

// -------------------------------------------------------------------------
// Counting semaphore. This implements the usual counter based semaphore.

class Cyg_Counting_Semaphore
{
    cyg_count32         count;          // The semaphore count

    Cyg_ThreadQueue     queue;          // Queue of waiting threads

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Counting_Semaphore(             // Constructor
        cyg_count32 init_count = 0      // Initial count value
        );

    ~Cyg_Counting_Semaphore();          // Destructor
        
    cyg_bool    wait();                 // Wait until decrement

#ifdef CYGFUN_KERNEL_THREADS_TIMER
    cyg_bool    wait( cyg_tick_count timeout );
#endif                                  // Wait until decrement or timeout

    cyg_bool    trywait();              // Try to decrement
        
    void        post();                 // Increment count

    cyg_count32 peek();                 // Get current count value
    
    inline
    cyg_bool    waiting()               // Is anyone waiting?
    {
        return !queue.empty();
    }

};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_SEMA_HXX
// EOF sema.hxx
