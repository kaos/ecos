#ifndef CYGONCE_KERNEL_MBOXT2_HXX
#define CYGONCE_KERNEL_MBOXT2_HXX

//==========================================================================
//
//	mboxt2.hxx
//
//	Mboxt2 (Message Box/Mailbox) class declarations
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
// Purpose:	Define Mboxt2 class interfaces
// Description:	The classes defined here provide the APIs for mboxt2es.
// Usage:	#include <cyg/kernel/mboxt2.hxx>
//		#include <cyg/kernel/mboxt2.inl>
//		
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>
#include <cyg/infra/cyg_ass.h>            // assertion macros
#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>

// -------------------------------------------------------------------------
// Message/Mail Box.  This template implements a queue of T's.
// Implemented as a template for maximal flexibility; one would hope
// that only one, with T==(void *) and the same number of them,
// is ever used without very good reason.

// Cyg_Mboxt2 has a fixed size array of T's; one size fits all.

template <class T, cyg_count32 QUEUE_SIZE>
class Cyg_Mboxt2
{
private:
    cyg_count32         base;           // index of first used slot
    cyg_count32         count;          // count of used slots
    Cyg_ThreadQueue     get_threadq;    // Queue of waiting threads
#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
    Cyg_ThreadQueue     put_threadq;    // Queue of waiting threads
#endif
    static const
    cyg_count32		size = QUEUE_SIZE;
    T			itemqueue[ QUEUE_SIZE ];
                                        // queue of items
    // private utility functions
    // wake up a thread from some queue
    inline void		wakeup_winner( const T &msg );
#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
    inline void		wakeup_putter( void );
#endif

public:

    CYGDBG_DEFINE_CHECK_THIS
    
    Cyg_Mboxt2();                        // Constructor
    ~Cyg_Mboxt2();                       // Destructor
        
    cyg_bool	get( T &ritem );        // get an item; wait if none
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    cyg_bool	get( T &ritem, cyg_tick_count abs_timeout );
#endif
    cyg_bool	tryget( T &ritem );     // just one attempt

    cyg_bool	peek_item( T &ritem );  // get next item without
                                        // removing it

#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
    cyg_bool	put( const T item );    // put an item; wait if full
#ifdef CYGFUN_KERNEL_THREADS_TIMER
    cyg_bool	put( const T item, cyg_tick_count abs_timeout );
#endif
#endif
    cyg_bool	tryput( const T item ); // fails if Q full

    inline
    cyg_count32 peek()                  // Get current count value
    {
        return count;
    }

    inline
    cyg_bool	waiting_to_get()        // Any threads waiting?
    {
        return ! get_threadq.empty();
    }

    inline
    cyg_bool	waiting_to_put()        // Any threads waiting?
    {
#ifdef CYGMFN_KERNEL_SYNCH_MBOXT_PUT_CAN_WAIT
        return ! put_threadq.empty();
#else
        return false;
#endif
    }
};

// -------------------------------------------------------------------------
#endif // ifndef CYGONCE_KERNEL_MBOXT2_HXX
// EOF mboxt2.hxx
