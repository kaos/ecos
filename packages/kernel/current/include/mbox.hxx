#ifndef CYGONCE_KERNEL_MBOX_HXX
#define CYGONCE_KERNEL_MBOX_HXX

//==========================================================================
//
//      mbox.hxx
//
//      Plain (void *) Mbox (Message Box/Mailbox) class declarations
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
// Author(s):   hmt
// Contributors:        hmt
// Date:        1998-02-10
// Purpose:     Define Mbox class interfaces
// Description: The classes defined here provide the APIs for mboxes.
// Usage:       #include <cyg/kernel/mbox.hxx>
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>            // assertion macros
#include <cyg/kernel/thread.hxx>

#ifdef CYGIMP_MBOX_USE_MBOXT_PLAIN
#include <cyg/kernel/mboxt.hxx>
#else
#include <cyg/kernel/mboxt2.hxx>
#endif

// -------------------------------------------------------------------------
// Message/Mail Box.  This class implements a queue of void * items using
// the Cyg_Mbox<Type, QSize> template class.

#ifndef CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE
// default is 10 elements
#define CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE (10)
#endif

// Cyg_Mbox has a fixed size array of (void *)s; one size fits all.
// Because of this, we can simplify the API by returning a NULL for
// "failed" conditions.  Ergo a NULL message is illegal.  BFD.

class Cyg_Mbox
{
private:
#ifdef CYGIMP_MBOX_USE_MBOXT_PLAIN
    Cyg_Mboxt<void *, CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE> m;
#else
    Cyg_Mboxt2<void *, CYGNUM_KERNEL_SYNCH_MBOX_QUEUE_SIZE> m;
#endif

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Mbox();                         // Constructor
    ~Cyg_Mbox();                        // Destructor
        
    void *      get();                  // get an item; wait if none
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    void *      get( cyg_tick_count timeout );
#endif
    void *      tryget();               // just one attempt

    void *      peek_item();            // Get next item to be returned
                                        // without removing it

#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT   // then we support it too
    cyg_bool    put( void *item );      // put an item; wait if full
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    cyg_bool    put( void *item, cyg_tick_count timeout );
#endif
#endif
    cyg_bool    tryput( void *item );   // fails if Q full

    inline
    cyg_count32 peek()                  // Get current count value
    {
        return m.peek();
    }

    inline
    cyg_bool    waiting_to_get()        // Any threads waiting to get?
    {
        return m.waiting_to_get();
    }
    inline
    cyg_bool    waiting_to_put()        // Any threads waiting to put?
    {
        return m.waiting_to_put();
    }
};



// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MBOX_HXX
// End of mbox.hxx
