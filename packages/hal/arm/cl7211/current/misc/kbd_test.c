//==========================================================================
//
//        kbd_test.c
//
//        Cirrus CL72xx eval board ASCII keyboard test
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1999-05-15
// Description:   Tool used to test ASCII keyboard
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_cl7211.h>  // Board definitions

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

#include "lcd_support.c"
#include "kbd_support.c"

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

static void
kbd_exercise(cyg_addrword_t p)
{
    cyg_uint8 ch;
    diag_printf("KBD test here!\n");

    // Initialize keyboard driver
    kbd_init();

    while (TRUE) {
        ch = kbd_getc();
        diag_printf("CH = 0x%x:'%c'\n", ch, ch);
    }

    diag_printf("All done!\n");
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(11,                // Priority - just a number
                      kbd_exercise,      // entry
                      0,                 // initial parameter
                      "KBD_thread",      // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    // Let 'em fly...
    cyg_scheduler_start();
} // cyg_package_start()

