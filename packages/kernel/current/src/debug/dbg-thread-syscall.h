
/* 
 * Copyright (c) 1998 Cygnus Support
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

/* This file should be included only by
     thread-syscall-relay.c and
     dbg-thread-demux.c
     */
     
     

enum dbg_syscall_ids 
  {
    dbg_null_func ,
    dbg_capabilities_func,
    dbg_currthread_func,
    dbg_threadlist_func,
    dbg_threadinfo_func,
    dbg_getthreadreg_func,
    dbg_setthreadreg_func
  } ;


union dbg_thread_syscall_parms
{
  struct
  {
    struct dbg_capabilities * abilities ;
  } cap_parms ;

  struct
  {
    threadref * ref ;
  } currthread_parms ;
  
  struct
  {
    int startflag ;
    threadref * lastid ;
    threadref * nextthreadid ;
  } threadlist_parms ;

  struct
  {
    threadref * ref ;
    struct cygmon_thread_debug_info * info ;
  } info_parms ;
  
  struct
  {
    threadref * thread ;
    int regcount ;
    void * registers ;
  } reg_parms ;
} ;


typedef int (*dbg_syscall_func) (enum dbg_syscall_ids id,
				 union dbg_thread_syscall_parms  * p ) ;
