/*==========================================================================
//
//      dbg-thread-demux.c
//
//      GDB Stub ROM system calls
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
// Author(s):   nickg
// Contributors: 
// Date:        1998-09-03
// Purpose:     GDB Stub ROM system calls
// Description: 
//
//####DESCRIPTIONEND####
//
//========================================================================*/


/* This file implements system calls out from the ROM debug stub into
   the operating environment.
   We assume they exist in the same address space.
   This file should be linked into your operating environment, your O.S.
   or whatever is managing multiple saved process contexts.
   Your O.S. needs to implement and provide
      dbg_thread_capabilities
      dbg_currthread
      dbg_threadlist
      dbg_threadinfo
      dbg_getthreadreg
      dbg_setthreadreg

   The debug stub will call this function by calling it indirectly
   vis a pre-assigned location possably somthing like a virtual vector table.
   Where this is exactly is platform specific.

   The O.S. should call patch_dbg_syscalls() and pass the address of the
   location to be patched with the dbg_thread_syscall_rmt function.
   Nothing really calls this by name.

   This scheme would also work if we wanted to use a real trapped system call.
   */

// -------------------------------------------------------------------------

#include <pkgconf/system.h>             // for CYGPKG... and STARTUP

#include <pkgconf/kernel.h>

#include <cyg/infra/cyg_type.h>

#include "cyg/hal/dbg-threads-api.h"
#include "cyg/hal/dbg-thread-syscall.h" 

// -------------------------------------------------------------------------

static int dbg_thread_syscall_rmt(
                       enum dbg_syscall_ids id,
                       union dbg_thread_syscall_parms * p
                       )
{
  switch (id)
    {
    case dbg_null_func : return 1 ;  /* test the syscall apparatus */

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT
    case dbg_capabilities_func :
      return dbg_thread_capabilities(p->cap_parms.abilities) ;
      break ;
    case dbg_currthread_func :
      return dbg_currthread(p->currthread_parms.ref) ;
      break ;
    case dbg_threadlist_func :
      return dbg_threadlist(p->threadlist_parms.startflag,
                            p->threadlist_parms.lastid,
                            p->threadlist_parms.nextthreadid) ;
      break ;
    case dbg_threadinfo_func :
      return dbg_threadinfo(p->info_parms.ref,
                            p->info_parms.info
                            ) ;
      break ;
    case dbg_getthreadreg_func :
      return dbg_getthreadreg(p->reg_parms.thread,
                              p->reg_parms.regcount,
                              p->reg_parms.registers) ;
      break ;
    case dbg_setthreadreg_func :
      return dbg_setthreadreg(p->reg_parms.thread,
                              p->reg_parms.regcount,
                              p->reg_parms.registers) ;
      break ;
    case dbg_scheduler_func :
      return dbg_scheduler(p->scheduler_parms.thread,
                           p->scheduler_parms.lock,
                           p->scheduler_parms.mode) ;
      break ;
#endif /* CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT */      
    default :
      return 0 ;  /* failure tue to non-implementation */
    }
}

// -------------------------------------------------------------------------
// When Cygmon calls us on the MIPS It has its own value in GP. We need to
// save that and set our own before proceeding.

#ifdef CYGPKG_HAL_MIPS
static int dbg_thread_syscall_rmt_1(
                       enum dbg_syscall_ids id,
                       union dbg_thread_syscall_parms * p
                       )
{

  register long gp_save;
  int r;
  asm volatile ( "move   %0,$28;"
                 ".extern _gp;"
                 "la     $gp,_gp;"
                 : "=r"(gp_save)
                 );

  r = dbg_thread_syscall_rmt( id, p );
  
  asm volatile ( "move   $gp,%0;" :: "r"(gp_save) );

  return r;
  
}
#endif  

// -------------------------------------------------------------------------

#define DBG_SYSCALL_THREAD_VEC_NUM 15

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

#ifdef CYG_HAL_SPARCLITE_SLEB
# include <cyg/hal/hal_cygm.h>
# ifdef CYG_HAL_USE_ROM_MONITOR_CYGMON
// then we support talking to CygMon...
#  undef DBG_SYSCALL_THREAD_VEC_NUM
#  define DBG_SYSCALL_THREAD_VEC_NUM BSP_VEC_MT_DEBUG
# endif
// otherwise this code is wrong for SPARClite but also not used.
#endif

#endif

void patch_dbg_syscalls(void * vector)
{
   dbg_syscall_func * f ;
   f = vector ;

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

#ifdef CYGPKG_HAL_MIPS
   f[DBG_SYSCALL_THREAD_VEC_NUM] = dbg_thread_syscall_rmt_1 ;
#else   
   f[DBG_SYSCALL_THREAD_VEC_NUM] = dbg_thread_syscall_rmt ;
#endif

#endif

}

// -------------------------------------------------------------------------
// End of dbg-thread-demux.c
