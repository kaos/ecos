//=============================================================================
//
//      hal_startup.c
//
//      HAL bootstrap code
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven, jskov
// Date:        1998-10-06
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#include <cyg/kernel/kapi.h>            // cyg_scheduler_lock
#endif

#include <cyg/hal/hal_intr.h>           // interrupt stuff
#include <cyg/hal/hal_arch.h>           // Architecture specific definitions

#include <cyg/infra/diag.h>             // diag_printf
#include <cyg/infra/cyg_ass.h>          // ASSERTION macros


//----------------------------------------------------------------------------
// ISR tables
volatile CYG_ADDRESS    cyg_hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRWORD   cyg_hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRESS    cyg_hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// VSR table
volatile CYG_ADDRESS    cyg_hal_vsr_table[CYGNUM_HAL_VSR_COUNT];

// Interrupts start disabled
volatile int            cyg_hal_interrupts_disabled = 1;
volatile int            cyg_hal_interrupts_deferred;
volatile int            cyg_hal_interrupts_unhandled[CYGNUM_HAL_ISR_COUNT];

//-----------------------------------------------------------------------------

typedef cyg_uint32 (*callsig_t)(int vector, cyg_uint32);

externC void interrupt_end(CYG_ADDRESS, CYG_ADDRESS, CYG_ADDRESS);
extern volatile cyg_uint32 cyg_scheduler_sched_lock;

externC void cyg_hal_default_interrupt_vsr(int vector)
{
    callsig_t func;
    cyg_uint32 ret;

    if (cyg_hal_interrupts_disabled) {
        cyg_hal_interrupts_unhandled[vector] = 1; 
        cyg_hal_interrupts_deferred = 1;
        return;
    }

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    // Increment the scheduler lock when the kernel is present.
    cyg_scheduler_sched_lock++;
#endif

    cyg_hal_interrupts_unhandled[vector] = 0; 
    func = (callsig_t) cyg_hal_interrupt_handlers[vector];
    ret = func(vector, cyg_hal_interrupt_data[vector]);

#ifdef CYGFUN_HAL_COMMON_KERNEL_SUPPORT
    // We only need to call _interrupt_end() when there is a kernel
    // present to do any tidying up.
    interrupt_end(ret, cyg_hal_interrupt_objects[vector], (CYG_ADDRESS) NULL);
#endif
}

externC void cyg_hal_default_exception_vsr(int vector)
{
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && \
    defined(CYGPKG_HAL_EXCEPTIONS)

    {
        int __pre_state = cyg_hal_interrupts_disabled++;

        cyg_hal_deliver_exception( vector, 0 );

        cyg_hal_interrupts_disabled = __pre_state;
    }

#else

    CYG_FAIL("Exception!!!");
    
#endif    
}

//---------------------------------------------------------------------------
// Default ISR

externC cyg_uint32
cyg_hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    diag_printf("Interrupt: %d\n", vector);

    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}

//-----------------------------------------------------------------------------
// Initialization on the VSRs.

// Mark signals NODEFER (i.e., allow a signal to interrupt a running
// signal handler). This is necessary because we may switch to another
// thread in the signal handler and thus delay (indefinitely) the
// return of the "signal handler", postponing new signals during that
// time.
#define LINUX_SA_NODEFER    0x40000000

struct hal_sigaction {
    void (*hal_handler)(int);
    long hal_mask;
    int hal_flags;
    void (*hal_bogus)(int);
} act, oact;
extern int cyg_hal_sys_sigaction(int, const struct hal_sigaction*, 
                                 struct hal_sigaction*);
externC void cyg_hal_isr_init(void)
{
    int i;

    // ISR setup
    for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++) {
        cyg_hal_interrupt_handlers[i] = (CYG_ADDRESS) &cyg_hal_default_isr;
    }

    // Interrupt VSR setup
    act.hal_handler = cyg_hal_default_interrupt_vsr;
    act.hal_mask = 0;
    act.hal_flags = LINUX_SA_NODEFER;
    cyg_hal_sys_sigaction(CYGNUM_HAL_INTERRUPT_RTC,  &act, &oact);

    // Exception VSR setup
    act.hal_handler = cyg_hal_default_exception_vsr;
    act.hal_mask = 0;
    act.hal_flags = LINUX_SA_NODEFER;
    cyg_hal_sys_sigaction(CYGNUM_HAL_VECTOR_SIGSEGV, &act, &oact);
    cyg_hal_sys_sigaction(CYGNUM_HAL_VECTOR_SIGBUS,  &act, &oact);
    cyg_hal_sys_sigaction(CYGNUM_HAL_VECTOR_SIGFPE,  &act, &oact);
}

//-----------------------------------------------------------------------------
// End of hal_startup.c
