//========================================================================
//
//      dbg-thread-syscall.h
//
//      Supports thread-aware debugging
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Red Hat, nickg
// Contributors:  Red Hat, nickg
// Date:          1998-08-25
// Purpose:       
// Description:   Supports thread-aware debugging
// Usage:         This header is not to be included by user code.
//                This file should be included only by
//                thread-syscall-relay.c and dbg-thread-demux.c
//
//####DESCRIPTIONEND####
//
//========================================================================


enum dbg_syscall_ids 
  {
    dbg_null_func ,
    dbg_capabilities_func,
    dbg_currthread_func,
    dbg_threadlist_func,
    dbg_threadinfo_func,
    dbg_getthreadreg_func,
    dbg_setthreadreg_func,
    dbg_scheduler_func,
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
  struct
  {
    threadref * thread ;        /* 64-bit thread identifier */
    int lock;                   /* 0 == unlock, 1 == lock */
    int mode;                   /* 0 == short (step), 1 == continue */
  } scheduler_parms ;
} ;


typedef int (*dbg_syscall_func) (enum dbg_syscall_ids id,
                                 union dbg_thread_syscall_parms  * p ) ;
