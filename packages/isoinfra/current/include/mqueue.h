#ifndef CYGONCE_ISO_MQUEUE_H
#define CYGONCE_ISO_MQUEUE_H
/*========================================================================
//
//      mqueue.h
//
//      POSIX message queue functions
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
// Date:          2000-05-05
// Purpose:       This file provides the macros, types and functions
//                for message queues required by POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <mqueue.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

#ifdef _POSIX_MESSAGE_PASSING
# ifdef CYGBLD_ISO_MQUEUE_HEADER
#  include CYGBLD_ISO_MQUEUE_HEADER
# else

#include <sys/types.h>  /* size_t and ssize_t */

/* TYPES */

struct mq_attr {
    long mq_flags;    /* mqueue flags */
    long mq_maxmsg;   /* max number of messages */
    long mq_msgsize;  /* max message size */
    long mq_curmsgs;  /* number of messages currently queued */
};

typedef void *mqd_t;

#ifdef __cplusplus
extern "C" {
#endif

/* FUNCTIONS */

extern mqd_t
mq_open( const char * /* name */, int /* oflag */, ... );

extern int
mq_close( mqd_t /* mqdes */ );

extern int
mq_unlink( const char * /* name */ );

extern int
mq_send( mqd_t /* mqdes */, const char * /* msg_ptr */, size_t /* msg_len */,
         unsigned int /* msg_prio */ );

extern ssize_t
mq_receive( mqd_t /* mqdes */, char * /* msg_ptr */, size_t /* msg_len */,
            unsigned int * /* msg_prio */ );

#ifdef _POSIX_REALTIME_SIGNALS

struct sigevent;

extern int
mq_notify( mqd_t /* mqdes */, const struct sigevent * /* notification */ );
#endif

extern int
mq_setattr( mqd_t /* mqdes */, const struct mq_attr * /* mqstat */,
            struct mq_attr * /* omqstat */ );

extern int
mq_getattr( mqd_t /* mqdes */, struct mq_attr * /* mqstat */ );

#ifdef __cplusplus
}   /* extern "C" */
#endif


# endif  /* ifndef CYGBLD_ISO_MQUEUE_HEADER */
#endif   /* ifdef _POSIX_MESSAGE_PASSING */


#endif /* CYGONCE_ISO_MQUEUE_H multiple inclusion protection */

/* EOF mqueue.h */
