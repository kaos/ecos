//=============================================================================
//
//      profile.c
//
//      Support for profiling on x86 synthetic target
//
//=============================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003, 2005 eCosCentric Ltd
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    bartv
// Contributors: bartv
// Date:         2003-10-12
//
//####DESCRIPTIONEND####
//=============================================================================

#include <pkgconf/system.h>
#ifdef CYGPKG_PROFILE_GPROF
#include <pkgconf/hal_synth.h>
#include <pkgconf/hal_synth_i386.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/profile/profile.h>

#if 1
// Profiling support.
//
// The profile timer uses the ITIMER_PROF, which means we get a SIGPROF
// signal at the desired rate. The signal handler can obtain the address
// of the interrupted code via a sigcontext structure. The contents of
// the sigcontext structure and exactly how it gets passed to the signal
// handler depends on the architecture, hence this code is x86-specific.
//
// The results of this profiling code seem a lot poorer than on other
// targets, but it is not clear why. There may be some subtle
// interaction between the system and profiling clocks.
static void
synth_prof_sighandler(int sig, struct cyg_hal_sys_sigcontext context)
{
    __profile_hit((CYG_ADDRWORD) context.hal_eip);
    CYG_UNUSED_PARAM(int, sig);
}

int
hal_enable_profile_timer(int resolution)
{
    struct cyg_hal_sys_sigaction    action;
    struct cyg_hal_sys_sigset_t     mask;
    struct cyg_hal_sys_itimerval    timer;

    // We want profiling to be an atomic operation. __profile_hit() is
    // a very simple function which should return quickly, and there
    // is no need for a DSR or context switching. Hence everything
    // including SIGIO and SIGALRM are blocked, effectively giving the
    // profiling timer the highest priority.
    action.hal_mask     = 0xffffffff;
    action.hal_flags    = CYG_HAL_SYS_SA_RESTORER;
    action.hal_handler  = (void (*)(int)) &synth_prof_sighandler;
    action.hal_restorer = &cyg_hal_sys_restore;
    
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGPROF, &action, (struct cyg_hal_sys_sigaction*) 0)) {
        CYG_FAIL("Failed to install signal handler for SIGPROF");
    }

    // The resolution is limited by the underlying 100Hz system clock,
    // there is no hardware timer which can generate faster clock
    // interrupts.
    if (resolution < 10000) {
        resolution = 10000;
    } else {
        resolution  = (resolution + 5000) / 10000;
        resolution *= 10000;
    }
    timer.hal_it_interval.hal_tv_sec    = 0;
    timer.hal_it_interval.hal_tv_usec   = resolution;
    timer.hal_it_value.hal_tv_sec       = 0;
    timer.hal_it_value.hal_tv_usec      = resolution;
    if (0 != cyg_hal_sys_setitimer(CYG_HAL_SYS_ITIMER_PROF, &timer, (struct cyg_hal_sys_itimerval*) 0)) {
        CYG_FAIL("Failed to initialize the profiling itimer");
    }

    // Now unblock SIGPROF
    CYG_HAL_SYS_SIGEMPTYSET(&mask);
    CYG_HAL_SYS_SIGADDSET(&mask, CYG_HAL_SYS_SIGPROF);
    if (0 != cyg_hal_sys_sigprocmask(CYG_HAL_SYS_SIG_UNBLOCK, &mask, (cyg_hal_sys_sigset_t*)0)) {
        CYG_FAIL("Failed to unblock SIGPROF");
    }

    return resolution;
}

#else

// An alternative implementation that overloads the SIGALRM handler
// rather than using SIGPROF. It does not seem to work any better.
static void (*synth_profile_old_alrm_sighandler)(int);

static void
synth_profile_alrm_sighandler(int sig, struct cyg_hal_sys_sigcontext context)
{
    __profile_hit((CYG_ADDRWORD) context.hal_eip);
    (*synth_profile_old_alrm_sighandler)(sig);
}

int
hal_enable_profile_timer(int resolution)
{
    struct cyg_hal_sys_sigaction    action;

    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGALRM, (const struct cyg_hal_sys_sigaction*)0, &action)) {
        CYG_FAIL("Failed to retrieve old signal handler for SIGALRM");
    }
    synth_profile_old_alrm_sighandler   = action.hal_handler;
    action.hal_handler = (void (*)(int)) &synth_profile_alrm_sighandler;
    if (0 != cyg_hal_sys_sigaction(CYG_HAL_SYS_SIGALRM, &action, (struct cyg_hal_sys_sigaction*)0)) {
        CYG_FAIL("Failed to install new signal handler for SIGALRM");
    }

    return CYGNUM_HAL_RTC_PERIOD;
}

#endif

// mcount() can be implemented largely using compiler built-ins. However
// there are two complications. The generic profiling code assumes
// __profile_mcount() is called with interrupts disabled. Blocking interrupts
// won't stop the low-level signal handlers, so mcount() calls from those
// may get lost because of the nesting test but that is fairly harmless.
// Those signal handlers will complete before control returns here, i.e.
// we have strict nesting, so there is no risk of the nested flag remaining
// set while a context switch occurs.  Also if eCos itself is built with
// -pg then the compiler will insert a recursive call to mcount(), and
// we have to guard against that.
void
mcount(void)
{
    static int  nested = 0;
    int         enabled;

    HAL_DISABLE_INTERRUPTS(enabled);
    if (!nested) {
        nested = 1;
        __profile_mcount((CYG_ADDRWORD)__builtin_return_address(1),
                         (CYG_ADDRWORD)__builtin_return_address(0));
        nested = 0;
    }
    HAL_RESTORE_INTERRUPTS(enabled);
}

#endif
