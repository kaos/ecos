/*==========================================================================
//
//	dbg_gdb.c
//
//	GDB Debugging Interface
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
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1998-08-22
// Purpose:     GDB Debugging Interface
// Description:	Interface for calls from GDB stubs into the OS. These
//              currently mostly support thread awareness.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/kernel.h>
#include <pkgconf/hal.h>                // CYG_HAL_USE_ROM_MONITOR_CYGMON

#ifdef CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

#include <cyg/kernel/ktypes.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/sched.hxx>

#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.inl>

#include <cyg/hal/hal_arch.h>

#if !defined(CYG_HAL_USE_ROM_MONITOR_CYGMON) \
    && defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
// FIXME: This really needs to be handled in some other way.
#include <cyg/hal/hal_stub.h>
externC void __stub_copy_registers(target_register_t * dest,
                                   target_register_t *src);
#endif

extern "C"
{
#include <cyg/hal/dbg-threads-api.h>
};

#define USE_ID 1

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

//--------------------------------------------------------------------------

externC int dbg_thread_capabilities(struct dbg_capabilities * cpb)
{
    cpb->mask1 = has_thread_current     |
        has_thread_registers            |
        has_thread_reg_change           |
        has_thread_list                 |
        has_thread_info                 ;
    return 1 ; 
}

//--------------------------------------------------------------------------

static void dbg_make_threadref(Cyg_Thread *thread, threadref *ref )
{
    cyg_uint16 id = thread->get_unique_id();

#if USE_ID
    ((unsigned long *)ref)[0] = (unsigned long)thread;
    ((unsigned long *)ref)[1] = (unsigned long)swap32(id);
#else    
    ((unsigned long *)ref)[1] = (unsigned long)thread;
    ((unsigned long *)ref)[0] = (unsigned long)id;
#endif
    
}

static Cyg_Thread *dbg_get_thread( threadref *ref)
{
#if USE_ID

    cyg_uint16 id = 0;

    id = (cyg_uint16)swap32(((unsigned long *)ref)[1]);

    Cyg_Thread *th = Cyg_Thread::get_list_head();
    while( th != 0 )
    {
        if( th->get_unique_id() == id ) break;
        th = th->get_list_next();
    }

//    if( thread->get_unique_id() != id ) th = 0;

#else
 
    cyg_uint16 id = 0;

    Cyg_Thread *thread = (Cyg_Thread *)(((unsigned long *)ref)[1]);
    id = (cyg_uint16)(((unsigned long *)ref)[0]);

    // Validate the thread.
    Cyg_Thread *th = Cyg_Thread::get_list_head();
    while( th != 0 )
    {
        if( th == thread ) break;
        th = th->get_list_next();
    }

//    if( thread->get_unique_id() != id ) th = 0;

#endif
 
    return th;
}

//--------------------------------------------------------------------------

externC int dbg_currthread(threadref * varparm)
{
    Cyg_Thread *thread = Cyg_Scheduler::get_current_thread();

    dbg_make_threadref(thread, varparm );
  
    return 1 ; 
}

//--------------------------------------------------------------------------

externC int dbg_currthread_id(void)
{
    Cyg_Thread *thread = Cyg_Scheduler::get_current_thread();
    return thread->get_unique_id ();
}

//--------------------------------------------------------------------------

externC int dbg_threadlist(int startflag,
		   threadref * lastthreadid,
		   threadref * next_thread)
{
    Cyg_Thread *thread;
    if( startflag )
    {
        thread = Cyg_Thread::get_list_head();
        dbg_make_threadref(thread, next_thread);
    }
    else
    {
        thread = dbg_get_thread(lastthreadid);

        if( thread == 0 ) return 0;
        thread = thread->get_list_next();

        if( thread == 0 ) return 0;
        dbg_make_threadref(thread, next_thread);        
    }
    return 1 ;
}

//--------------------------------------------------------------------------
// Some support routines for manufacturing thread info strings

static char *dbg_addstr(char *s, char *t)
{
    while( (*s++ = *t++) != 0 );

    return s-1;
}

static char *dbg_addint(char *s, int n, int base)
{
    char buf[16];
    char sign = '+';
    cyg_count8 bpos;
    char *digits = "0123456789ABCDEF";

    if( n < 0 ) n = -n, sign = '-';
    
    /* Set pos to start */
    bpos = 0;

    /* construct digits into buffer in reverse order */
    if( n == 0 ) buf[bpos++] = '0';
    else while( n != 0 )
    {
        cyg_ucount8 d = n % base;
        buf[bpos++] = digits[d];
        n /= base;
    }

    /* set sign if negative. */
    if( sign == '-' )
    {
        buf[bpos] = sign;
    }
    else bpos--;

    /* Now write it out in correct order. */
    while( bpos >= 0 )
        *s++ = buf[bpos--];

    *s = 0;
    
    return s;
}

static char *dbg_adddec(char *s, int x)
{
    return dbg_addint(s, x, 10);
}

//--------------------------------------------------------------------------

externC int dbg_threadinfo(
		   threadref * threadid,
		   struct cygmon_thread_debug_info * info)
{
    static char statebuf[60];
    
    Cyg_Thread *thread = dbg_get_thread(threadid);
    if( thread == 0 ) return 0;

    info->context_exists        = 1;

    char *sbp = statebuf;
    char *s;

    if( thread->get_state() & Cyg_Thread::SUSPENDED )
    {
        sbp = dbg_addstr( sbp, "suspended+");
    }

    switch( thread->get_state() & ~Cyg_Thread::SUSPENDED )
    {
    case Cyg_Thread::RUNNING:
        s = "running";                  break;
    case Cyg_Thread::SLEEPING:
        s = "sleeping";                 break;
    case Cyg_Thread::COUNTSLEEP:
        s = "counted sleep";            break;
    case Cyg_Thread::CREATING:
        s = "creating";                 break;
    case Cyg_Thread::EXITED:
        s = "exited";                   break;
    default:
        s = "unknown state";            break;
    }

    sbp = dbg_addstr( sbp, s );
    sbp = dbg_addstr( sbp, ", Priority: " );
    sbp = dbg_adddec( sbp, thread->get_priority() );
    
    info->thread_display        = statebuf;

#ifdef CYGVAR_KERNEL_THREADS_NAME
    info->unique_thread_name    = thread->get_name();
#else
    info->unique_thread_name    = 0;
#endif

    info->more_display          = 0;

    return 1 ;
}

//--------------------------------------------------------------------------

externC int dbg_getthreadreg(
		     threadref * osthreadid,
		     int regcount, /* count of registers in the array */
		     void * regval)  /* fillin this array */
{
    Cyg_Thread *thread = dbg_get_thread(osthreadid);

    if( thread == 0 ) return 0;

    if( thread == Cyg_Scheduler::get_current_thread() )
    {
#if defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
        // We have no state for the current thread, Cygmon has
        // got that and we cannot get at it.
        return 0;
#elif defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
        // _registers hold the state of the current thread.
        extern target_register_t * _registers;

        __stub_copy_registers ((target_register_t *)regval, _registers);
#else
        return 0;
#endif
    }
    else
    {
        HAL_SavedRegisters *regs = thread->get_saved_context();
        if( regs == 0 ) return 0;

        HAL_GET_GDB_REGISTERS (regval, regs);
    }
    
    return 1 ;
}

//--------------------------------------------------------------------------
		   
externC int dbg_setthreadreg(
			    threadref * osthreadid, 
			    int regcount , /* number of registers */
			    void * regval) 
{
    Cyg_Thread *thread = dbg_get_thread(osthreadid);
    
    if( thread == 0 ) return 0;

    if( thread == Cyg_Scheduler::get_current_thread() )
    {
#if defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
        // We have no state for the current thread, Cygmon has
        // got that and we cannot get at it.
        return 0;
#elif defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
        // _registers hold the state of the current thread.
        extern target_register_t * _registers;

        __stub_copy_registers (_registers, (target_register_t *)regval);
#else
        return 0;
#endif
    }
    else
    {
        HAL_SavedRegisters *regs = thread->get_saved_context();
        if( regs == 0 ) return 0;

        HAL_SET_GDB_REGISTERS (regs, regval);
    }
    
    
    return 1;
}

#endif // CYGDBG_KERNEL_DEBUG_GDB_THREAD_SUPPORT

//--------------------------------------------------------------------------
// End of dbg_gdb.cxx
