#ifndef CYGONCE_DEVS_COMMON_QUEUET_HXX
#define CYGONCE_DEVS_COMMON_QUEUET_HXX

//==========================================================================
//
//      queuet.hxx
//
//      Generic queue template
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
// Author(s):   proven
// Contributors:  proven
// Date:        1998-04-21
// Purpose:     Create a template atomic queue 
// Description: This file contains the implementations of
//              Cyg_Queuet
//
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/kernel/ktypes.h>          // base kernel types
#include <cyg/infra/cyg_ass.h>		// assertion macros
#include <cyg/infra/cyg_trac.h>		// tracing macros


/*
 * Queue that allows two threads to interact with a queue
 * without locking. One thread does queueing of useable buffers
 * and dequeing of done buffers and one thread is the consumer.
 *
 * Only n-1 elements may be queued at any time for a queue of n bytes.
 */

template <class T, cyg_count32 QUEUE_SIZE>
class Cyg_Queuet
{
private:
    cyg_ucount32 		next_free;
    cyg_ucount32 		last_free;
    cyg_ucount32 		next_inuse;
    cyg_ucount32 		last_inuse;
    static const cyg_ucount32 	queue_size = QUEUE_SIZE;
    T 			      * queue[ QUEUE_SIZE ];

public: /* Routines for dealing with the queue */
    inline T *
    get_next_inuse(T * done)
    {
    	T * ret;

    	CYG_ASSERT (next_inuse < queue_size, "next_inuse out of bounds");
    	CYG_ASSERT (last_inuse < queue_size, "last_inuse out of bounds");

	if (done) {
	    (++last_inuse) %= queue_size;
	}
    	if ((ret = queue[next_inuse]) != NULL) {
    	    (++next_inuse) %= queue_size;
        }
    	return ret;
    }
    
    inline void
    enqueue(T * next)
    {
        CYG_ASSERT (next_free < queue_size, "next_free out of bounds");
        CYG_ASSERT (queue[next_free] == NULL, "next_free is not NULL");
        CYG_ASSERT (min_free(), "not enough space to enqueue");

        queue[next_free++] = next;
        next_free %= queue_size;
    }

    inline T *
    Cyg_Queuet::dequeue()
    {
        T * ret;

        CYG_ASSERT (last_free < queue_size, "last_free out of bounds");
        CYG_ASSERT (last_free != last_inuse, "no elements to dequeue");
        CYG_ASSERT (queue[last_free] != NULL, "queued element is NULL");

        ret = queue[last_free];
        queue[last_free++] = NULL;
        last_free %= queue_size;
        return ret;
    }

    /* Assume producer is checking */
    inline cyg_ucount32
    Cyg_Queuet::min_done()
    {
        cyg_count32 tmp;
        cyg_ucount32 ret;

        if (queue[last_free] != NULL) {
    	    tmp = last_inuse - last_free;
    	    ret = (tmp < 0 ? tmp + queue_size : tmp);
        } else {
	    CYG_ASSERT (last_free == next_free,
	      "last_free element is bogus");
	    ret = 0;
        }
        return ret;
    }

    /* Assume producer is checking */
    cyg_ucount32
    Cyg_Queuet::min_free()
    {
        cyg_count32 tmp;
        cyg_ucount32 ret;

        CYG_ASSERT (next_free < queue_size, "next_free out of bounds");
        CYG_ASSERT (last_free < queue_size, "last_free out of bounds");

    	tmp = last_free - next_free;
    	ret = (tmp <= 0 ? tmp + queue_size : tmp);
	/* Only n - 1 elements may be queued */
        return (ret - 1);
    }
};

// -------------------------------------------------------------------------
#endif // CYGONCE_DEVS_COMMON_QUEUET_HXX
// End of queuet.hxx


