//========================================================================
//
//      dbg-threads-api.h
//
//      Supports thread-aware debugging
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
// Author(s):     Red Hat, nickg
// Contributors:  Red Hat, nickg
// Date:          1998-08-25
// Purpose:       
// Description:   These are the calls used to extract operating system
//                specific information used in supporting thread aware
//                debugging. The Operating Environment being debugged
//                needs to supply these functions.
// Usage:         This header is not to be included by user code.
//
//####DESCRIPTIONEND####
//
//========================================================================

#if !defined(DBG_THREADS_API_INCLUDED)
#define DBG_THREADS_API_INCLUDED 1

#include <cyg/infra/cyg_type.h>         /* externC */

#define has_thread_void       0 
#define has_thread_current    1
#define has_thread_registers  2
#define has_thread_reg_change 4
#define has_thread_list       8
#define has_thread_info       16

typedef unsigned char threadref[8] ;

struct dbg_capabilities
{
  unsigned long mask1  ;
} ;


/* fill in the list of thread aware capabilities */
externC int dbg_thread_capabilities(struct dbg_capabilities * cbp) ;


/* Fillin the identifier of the current thread */
/* return 1 if defined, 0 if not defined */
externC int dbg_currthread(threadref * varparm) ;

/* Return the unique ID number of the current thread. */
externC int dbg_currthread_id(void);

/* get the first or next member of the list of known threads */
externC int dbg_threadlist(int startflag,
                           threadref * lastthreadid,
                           threadref * next_thread
    ) ;

/* return 1 if next_threadid has been filled in with a value */
/* return 0 if there are none or no more */

/* The O.S can fill in the following information about a thread when queried.
   The structure of thise strings is determined by the O.S.
   It is display oriented, so figure out what the users need to see.
   Nulls are OK but GDB will fill some not so meaningful data.
   These pointers may be in the calles private structures, the info will
   get copied immediatly after the call to retreive it.
   */
struct cygmon_thread_debug_info
{
  threadref thread_id ;
  int context_exists ; /* To the point where examining its state,
                         registers and stack makes sense to GDB */
  char * thread_display ; /* As shown in thread status window, name, state */
  char * unique_thread_name ; /* human readable identifier, window label */
  char * more_display ;   /* more detailed info about thread.
                          priority, queuedepth, state, stack usage, statistics */
} ;




externC int dbg_threadinfo(
                          threadref * threadid,
                          struct cygmon_thread_debug_info * info) ;

/* Return 1 if threadid is defined and info copied, 0 otherwise */

/* The O.S should fillin the array of registers using values from the
saves context. The array MUST be in GDB register save order even if
the saved context is different or smaller. Do not alter the values of
registers which are NOT in the O.S. thread context. Their default values
have already been assigned.
*/

externC int dbg_getthreadreg(
                            threadref * osthreadid, 
                            int regcount, /* count of registers in the array */
                            void * regval) ; /* fillin this array */


/* The O.S. should scan through this list of registers which are in
GDB order and the O.S. should replace the values of all registers
which are defined in the saved context of thread or process identified
by osthreadid. Return 0 if the threadis does not map to a known
process or other error. Return 1 if the setting is successful.  */

externC int dbg_setthreadreg(
                            threadref * osthreadid, 
                            int regcount , /* number of registers */
                            void * regval) ;

/* Control over OS scheduler. With the scheduler locked it should not
   perform any rescheduling in response to interrupts.  */
externC int dbg_scheduler(
                          threadref * osthreadid,
                          int lock,     /* 0 == unlock, 1 == lock */
                          int mode);    /* 0 == step,   1 == continue */



#endif /* DBG_THREADS_API_INCLUDED */
