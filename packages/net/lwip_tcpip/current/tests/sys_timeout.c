//==========================================================================
//
//      sys_timeout.c
//
//      Simple test-case for the lwip system timeout functionality.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2009 Free Software Foundation
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2009-05-18
// Purpose:
// Description:  Simple test-case for the lwip system timeout functionality.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

#include <pkgconf/net_lwip.h>

#include <lwip.h>

#define TIMERS 10

#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
#if MEMP_NUM_SYS_TIMEOUT >= (TIMERS + 6)

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static char main_stack[STACK_SIZE];
static cyg_thread main_thread;
static cyg_handle_t main_handle;

static char timeout_stack[STACK_SIZE];

static int timeouts = 0;

static void timeout_fn(void * arg)
{
    int i = (int) arg;

    diag_printf("Timeout timer #%d\n", i);

    if (i != timeouts++)
        CYG_TEST_FAIL_EXIT("Timeout out of order");

    if (timeouts == TIMERS)
        CYG_TEST_FINISH("Test successful");
}

static void timeout_thread_entry(void *arg)
{
    int i;

    for (i = 0; i < TIMERS; i++) {
        diag_printf("Adding timer #%d\n", i);
        sys_timeout((i + 1) * 100, timeout_fn, (void *) i);
    }
    sys_msleep(0);
}

void main_thread_entry(cyg_addrword_t p)
{
    cyg_lwip_sequential_init();
    cyg_lwip_thread_new("timeout", timeout_thread_entry, NULL,
                        timeout_stack, STACK_SIZE, 7);
}

externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_INFO("Testing timeouts");

    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(
        10,                 // Priority - just a number
        main_thread_entry,  // Entry
        0,                  // Entry parameter
        "main",         // Name
        main_stack,         // Stack
        STACK_SIZE,         // Size
        &main_handle,       // Handle
        &main_thread        // Thread data structure
    );
    cyg_thread_resume(main_handle);
    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

#else // MEMP_NUM_SYS_TIMEOUT >= (TIMERS + 6)
#define N_A_MSG "Timeout memory pool is too small"
#endif // MEMP_NUM_SYS_TIMEOUT >= (TIMERS + 6)

#else // CYGFUN_LWIP_MODE_SEQUENTIAL
#define N_A_MSG "Not configured in 'Sequential' mode"
#endif // CYGFUN_LWIP_MODE_SEQUENTIAL

#ifdef N_A_MSG
externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(N_A_MSG);
}
#endif // N_A_MSG
