//========================================================================
//
//      dbg-threads-syscall.c
//
//      Pseudo system calls for multi-threaded debug support
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
// Description:   Pseudo system calls to bind system specific multithread
//                debug support with a ROM monitor, cygmon. We call it
//                Cygmon, but the feature lives in libstub.
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#if !defined(CYGPKG_KERNEL) && defined(CYGDBG_HAL_DEBUG_GDB_THREAD_SUPPORT)

/* Only include this code if we do not have a kernel. Otherwise the kernel
 * supplies these functions for the app we are linked with.
 */

#include <cyg/hal/dbg-threads-api.h>
#include <cyg/hal/dbg-thread-syscall.h>


static dbg_syscall_func * dbg_syscall_ptr ;

static union dbg_thread_syscall_parms tcall ;

/* ----- INIT_THREADS_SYSCALL --------------------------------------- */
/* Some external bing and configuration logic knows how to setup
   the ststem calls. In the first implementation, we have used a vector
   in the secondary vector table. This functions allows us to isolate that
   sort of system specific detail. Similarly, we do not export the
   specific detail of a dbg_syscall_func.
 */


void init_threads_syscall(void * vector)
{
   dbg_syscall_ptr = vector ; /* AH, the easy compatability of the
   void pointer*/
} /* init_threads_syscall */

/* All forms of failure return 0 */
/* Whether non support, incomplete initialization, unknown thread */
static __inline__ int dbg_thread_syscall(
				     enum dbg_syscall_ids id)
{
  dbg_syscall_func f ; /* double indirect via */
  if (0 == dbg_syscall_ptr) return 0; /* dbg_syscall_ptr never init'd */
  if (0 ==(f = *dbg_syscall_ptr)) return 0 ;  /* vector not initialized */
  return (*f)(id,&tcall);
}




/* ------- INIT_THREAD_SYSCALL -------------------------------------------  */
/* Received the address of the entry in the secondary interrupt vector table */
/* This table is the interchange between the O.S. and Cygmon/libstub         */
/* This could get more complex so, I am doing it with a function
   rather than exposing the internals.
 */
void init_thread_syscall(void * vector)
{
  dbg_syscall_ptr = vector ;
}

int dbg_thread_capabilities(struct dbg_capabilities * cbp)
{
#if 0    
  tcall.cap_parms.abilities = cbp ;
  return dbg_thread_syscall(dbg_capabilities_func) ;
#else
    cbp->mask1 = has_thread_current     |
        has_thread_registers            |
        has_thread_reg_change           |
        has_thread_list                 |
        has_thread_info                 ;
    return 1 ; 
#endif  
}

int dbg_currthread(threadref * varparm)
{
  tcall.currthread_parms.ref = varparm ;
  return dbg_thread_syscall(dbg_currthread_func) ;
}


int dbg_threadlist(
		   int startflag,
		   threadref * lastthreadid,
		   threadref * next_thread
		   )
{
  tcall.threadlist_parms.startflag = startflag ;
  tcall.threadlist_parms.lastid = lastthreadid ;
  tcall.threadlist_parms.nextthreadid = next_thread ;
  return dbg_thread_syscall(dbg_threadlist_func) ;
}

int dbg_threadinfo(
		   threadref * threadid,
		   struct cygmon_thread_debug_info * info)
{
  tcall.info_parms.ref = threadid ;
  tcall.info_parms.info = info ;
  return dbg_thread_syscall(dbg_threadinfo_func) ;
}

int dbg_getthreadreg(
		     threadref * osthreadid,
		     int regcount, /* count of registers in the array */
		     void * regval)  /* fillin this array */
{
  tcall.reg_parms.thread =    osthreadid ;
  tcall.reg_parms.regcount =  regcount ;
  tcall.reg_parms.registers = regval ;
  return dbg_thread_syscall(dbg_getthreadreg_func) ;
}

int dbg_setthreadreg(
		     threadref * osthreadid, 
		     int regcount , /* number of registers */
		     void * regval)
{
  tcall.reg_parms.thread =    osthreadid ;
  tcall.reg_parms.regcount =  regcount ;
  tcall.reg_parms.registers =  regval ;
  return dbg_thread_syscall(dbg_setthreadreg_func) ;
} /* dbg_setthreadreg */

int dbg_scheduler(threadref *thread_id, int lock, int mode)
{
  tcall.scheduler_parms.thread    = thread_id;
  tcall.scheduler_parms.lock      = lock ;
  tcall.scheduler_parms.mode      = mode ;

  return dbg_thread_syscall(dbg_scheduler_func) ;
}



#if (CYG_BYTEORDER == CYG_LSBFIRST)

unsigned long swap32(unsigned long x)
{
    unsigned long r = 0;

    r |= (x>>24)&0xFF;
    r |= ((x>>16)&0xFF)<<8;
    r |= ((x>>8)&0xFF)<<16;
    r |= ((x)&0xFF)<<24;

    return r;
}

#else

#define swap32(x) ((unsigned long)(x))

#endif

int dbg_currthread_id(void)
{
    threadref ref;
    if( dbg_currthread( &ref ) )
        return (cyg_uint16)swap32(((unsigned long *)ref)[1]);
    else return 0;
}

#endif
