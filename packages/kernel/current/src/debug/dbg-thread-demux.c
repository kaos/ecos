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

#include <pkgconf/kernel.h>

#include "cyg/hal/dbg-threads-api.h"
#include "dbg-thread-syscall.h" 

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
#endif /* CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT */      
    default :
      return 0 ;  /* failure tue to non-implementation */
    }
}

// -------------------------------------------------------------------------
// When Cygmon calls us on the TX39 It has its own value in GP. We need to
// save that and set our own before proceeding.

#ifdef CYG_HAL_TX39
static int dbg_thread_syscall_rmt_1(
		       enum dbg_syscall_ids id,
		       union dbg_thread_syscall_parms * p
		       )
{

  register long gp_save;
  int r;
  asm ( "move   %0,$28;"
        ".extern _gp;"
        "la     $gp,_gp;"
        : "=r"(gp_save)
      );

  r = dbg_thread_syscall_rmt( id, p );
  
  asm ( "move   $gp,%0;" :: "r"(gp_save) );

  return r;
  
}
#endif  

// -------------------------------------------------------------------------

void patch_dbg_syscalls(void * vector)
{
   dbg_syscall_func * f ;
   f = vector ;
#ifdef CYG_HAL_TX39
   *f = dbg_thread_syscall_rmt_1 ;
#else   
   *f = dbg_thread_syscall_rmt ;
#endif   
}

// -------------------------------------------------------------------------
// End of dbg-thread-demux.c
