//==========================================================================
//
//      synth_entry.c
//
//      Entry code for Linux synthetic target.
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   proven
// Contributors:proven, jskov, bartv
// Date:        1999-01-06
// Purpose:     Entry point for Linux synthetic target.
//
//####DESCRIPTIONEND####
//
//=========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_io.h>
#include CYGHWR_MEMORY_LAYOUT_H

/*------------------------------------------------------------------------*/
/* C++ support - run initial constructors                                 */

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void
cyg_hal_invoke_constructors (void)
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc *p = &__CTOR_END__[-1];
    
    cyg_hal_stop_constructors = 0;
    for (; p >= __CTOR_LIST__; p--) {
        (*p) ();
        if (cyg_hal_stop_constructors) {
            p--;
            break;
        }
    }
#else
    pfunc *p;

    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
        (*p) ();
#endif
}

// ----------------------------------------------------------------------------
// The low-level entry point is platform-specific, typically in the
// assember file vectors.S. However that entry point simply jumps
// directly here, with no further processing or stack manipulation.
// The HAL specification defines clearly what should happen during
// startup.

externC void    cyg_start( void );

void _linux_entry( void )
{
    void* new_top = (void*) 0;
    
    // "Initialize various cpu status registers, including disabling interrupts."
    // That is a no-op for the synthetic target, in particular interrupts are
    // already disabled.

    // "Set up any CPU memory controller to access ROM, RAM, and I/O devices
    // correctly".
    //
    // This involves using the brk() system call to allocate the RAM used
    // for the heaps. There are no variables mapped there so the system
    // will not have done this for us. Note that the implementation of
    // brk() (mm/mmap.c) differs from the documentation - the return
    // value is the new brk value, not an error code.
    new_top = (void*) (CYGMEM_REGION_ram + CYGMEM_REGION_ram_SIZE);
    if (new_top != cyg_hal_sys_brk(new_top)) {
        CYG_FAIL("Failed to initialize memory");
        cyg_hal_sys_exit(1);
    }
    
    // Again a no-op for the synthetic target. All memory is readily
    // accessible. Arguably the auxiliary should be started up here, but
    // instead that is left to platform initialization.

    // "Enable the cache". Effectively the synthetic target has no cache,
    // anything provided by the hardware is not readily accessible.

    // "Set up the stack pointer". The system starts up a program with a
    // suitable stack.

    // "Initialize any global pointer register". There is no such register.

    // Perform platform-specific initialization. Actually, all Linux
    // platforms can share this. It involves setting up signal handlers
    // and so on.
    hal_synthetic_target_init();

    // This is not a ROM startup, so no need to worry about copying the
    // .data section.

    // "Zero the .bss section". Linux will have done this for us.

    // "Create a suitable C stack frame". Already done.

    // Invoke the C++ constructors.
    cyg_hal_invoke_constructors();

    // "Call cyg_start()". OK.
    cyg_start();

    // "Drop into an infinite loop". Not a good idea for the synthetic
    // target. Instead, exit.
    cyg_hal_sys_exit(0);
}

//-----------------------------------------------------------------------------
// End of entry.c
