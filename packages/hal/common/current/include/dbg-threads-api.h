#if !defined(DBG_THREADS_API_INCLUDED)
#define DBG_THREADS_API_INCLUDED 1
/* These are the calls used to extract operating system specific information
   used in supporting thread aware debugging.
   The Operating Environment being debugged needs to supply these functions.
 */

/* 
 * Copyright (c) 1998,1999 Cygnus Solutions
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

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
extern int dbg_thread_capabilities(struct dbg_capabilities * cbp) ;


/* Fillin the identifier of the current thread */
/* return 1 if defined, 0 if not defined */
extern int dbg_currthread(threadref * varparm) ;

/* Return the unique ID number of the current thread. */
extern int dbg_currthread_id(void);

/* get the first or next member of the list of known threads */
extern int dbg_threadlist(int startflag,
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




extern int dbg_threadinfo(
                          threadref * threadid,
                          struct cygmon_thread_debug_info * info) ;

/* Return 1 if threadid is defined and info copied, 0 otherwise */

/* The O.S should fillin the array of registers using values from the
saves context. The array MUST be in GDB register save order even if
the saved context is different or smaller. Do not alter the values of
registers which are NOT in the O.S. thread context. Their default values
have already been assigned.
*/

extern int dbg_getthreadreg(
                            threadref * osthreadid, 
                            int regcount, /* count of registers in the array */
                            void * regval) ; /* fillin this array */


/* The O.S. should scan through this list of registers which are in
GDB order and the O.S. should replace the values of all registers
which are defined in the saved context of thread or process identified
by osthreadid. Return 0 if the threadis does not map to a known
process or other error. Return 1 if the setting is successful.  */

extern int dbg_setthreadreg(
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
