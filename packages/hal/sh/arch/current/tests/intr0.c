//==========================================================================
//
//        intr0.c
//
//        Interrupt controls test
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
// Copyright (C) 1998, 1999, 2000, 20001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          2001-01-18
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>

#if defined(CYGPKG_KERNEL)

#include <pkgconf/kernel.h>

#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#include <cyg/hal/hal_intr.h>

// -------------------------------------------------------------------------

#define NTHREADS 1
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

// -------------------------------------------------------------------------

// This is (tick time * 1.5) 
#define TICK_DELAY (1500000 / CYGNUM_HAL_RTC_DENOMINATOR)

static void
entry0( cyg_addrword_t data )
{
    int tick;

    // Scheduler and thus timer interrupts are running by the
    // time we get here.

    // Wait for next tick
    tick = cyg_current_time();
    do {} while (cyg_current_time() == tick);
    tick = cyg_current_time();

    // Then mask timer interrupts
    HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_RTC);

    // and wait for the time when the next tick should have come
    // and check it didn't trigger an interrupt
    hal_delay_us(TICK_DELAY);
    CYG_TEST_CHECK(cyg_current_time() == tick, "Timer interrupt while masked");

    // Now change interrupt level, and make the check again. Changing
    // level should not affect interrupt mask state.
    HAL_INTERRUPT_SET_LEVEL(CYGNUM_HAL_INTERRUPT_RTC, 8);
    hal_delay_us(TICK_DELAY);
    CYG_TEST_CHECK(cyg_current_time() == tick, 
                   "Timer interrupt after changing level");

    // Finally unmask the interrupt and make sure it results in ticks.
    HAL_INTERRUPT_UNMASK(CYGNUM_HAL_INTERRUPT_RTC);
    hal_delay_us(TICK_DELAY);
    CYG_TEST_CHECK(cyg_current_time() != tick, 
                   "No timer interrupt after unmask");

    CYG_TEST_PASS_FINISH("SH intr0 test end");
}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "intr",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();
}

// -------------------------------------------------------------------------

#else // def CYGFUN_KERNEL_API_C
#define N_A_MSG "Kernel C API layer disabled"
#endif // def CYGFUN_KERNEL_API_C
#else // def CYGPKG_KERNEL
#define N_A_MSG "Needs kernel"
#endif // def CYGPKG_KERNEL

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG );
}
#endif // N_A_MSG

// -------------------------------------------------------------------------
// EOF intr0.c
