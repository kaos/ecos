#ifndef CYGONCE_KERNEL_MQUEUE_HXX
#define CYGONCE_KERNEL_MQUEUE_HXX
/*========================================================================
//
//      mqueue.hxx
//
//      Message queues
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-05-09
// Purpose:       This file provides the interface for eCos message queues
// Description:   This differs from the message boxes also supported by
//                eCos primarily because the requirements of message
//                queues are driven by POSIX semantics. POSIX semantics are
//                more dynamic and therefore heavyweight than Mboxes,
//                including prioritization, and variable sized queues and
//                message lengths
// Usage:         #include <cyg/kernel/mqueue.hxx>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/kernel.h>          /* Configuration header */

/* INCLUDES */

#include <stddef.h>                  /* size_t */
#include <cyg/infra/cyg_type.h>      /* Types */
#include <cyg/infra/cyg_ass.h>       /* CYGDBG_DEFINE_CHECK_THIS,
                                        CYGDBG_USE_ASSERTS */
#include <cyg/kernel/sema.hxx>       /* Cyg_Counting_Semaphore */

/* CLASSES */

class Cyg_Mqueue {
public:
    typedef void (*callback_fn_t)(Cyg_Mqueue &q, CYG_ADDRWORD data);
    typedef void * (*qalloc_fn_t)(size_t len);
    typedef void (*qfree_fn_t)(void *ptr, size_t len);

    typedef enum {
        OK=0,
        NOMEM,
        WOULDBLOCK,
        INTR
    } qerr_t;

protected:
    struct qentry {
        struct qentry *next;
        unsigned int priority;
        size_t buflen;
        volatile bool busy;
        // data buffer follows here
        char *buf() const { return (char *)this + sizeof(*this); }
    };

    Cyg_Counting_Semaphore putsem, getsem;

    struct qentry *q;            // q entries in use
    struct qentry *freelist;     // q entries not in use
    void *queuespace;            // placeholder for the dynamically allocated
                                 // area

    size_t queuespacesize;

    qfree_fn_t free_fn;          // how to free queuespace when we destruct

    callback_fn_t callback;
    CYG_ADDRWORD callback_data;

    CYGDBG_DEFINE_CHECK_THIS

#ifdef CYGDBG_USE_ASSERTS
    long qlen;
    size_t msgsize;
#endif

public:

    Cyg_Mqueue( long maxmsgs, long maxmsgsize,
                qalloc_fn_t qalloc, qfree_fn_t qfree, qerr_t *err );
    ~Cyg_Mqueue();

    // put() copies len bytes of *buf into the queue at priority prio
    qerr_t put( const char *buf, size_t len, unsigned int prio,
                bool block=true);

    // get() returns the oldest highest priority message in the queue in *buf
    // and sets *prio to the priority (if prio is non-NULL) and *len to the
    // actual message size
    qerr_t get( char *buf, size_t *len, unsigned int *prio, bool block=true ); 

    // count() returns the number of messages in the queue
    long count();

    // Supply a callback function to call (with the supplied data argument)
    // when the queue goes from empty to non-empty (unless someone's already
    // doing a get()). This returns the old callback_fn, and if olddata is
    // non-NULL sets it to the old data (yes, really!)
    callback_fn_t setnotify( callback_fn_t callback_fn, CYG_ADDRWORD data,
                             CYG_ADDRWORD *olddata=NULL);
    
}; /* class Cyg_Mqueue */

#include <cyg/kernel/mqueue.inl>

#endif /* CYGONCE_KERNEL_MQUEUE_HXX multiple inclusion protection */

/* EOF mqueue.hxx */
