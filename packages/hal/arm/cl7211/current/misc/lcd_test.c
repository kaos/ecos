//==========================================================================
//
//        lcd_test.c
//
//        Cirrus CL7211 eval board LCD test code
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
// Date:          1999-09-01
// Description:   Tool used to test LCD controller
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_cl7211.h>  // Board definitions

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

#include "lcd_support.c"

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

static void
lcd_test(int depth)
{
    int i, j;
    diag_printf("Set depth %d\n", depth);
    lcd_on(depth);
    lcd_clear();
    for (j = 0;  j < 5;  j++) {
        for (i = 0;  i < width;  i++) {
            lcd_putc('A');
        }
    }
    lcd_putc('\n');
    lcd_putc('\n');
    cyg_thread_delay(5*100);
    for (j = 0;  j < 5;  j++) {
        for (i = FIRST_CHAR;  i <= LAST_CHAR;  i++) {
            lcd_putc(i);
        }
    }
    cyg_thread_delay(5*100);
}

static void
lcd_exercise(cyg_addrword_t p)
{
    diag_printf("LCD test here!\n");

//    lcd_test(1);
//    lcd_test(2);
    lcd_test(4);

    diag_printf("All done!\n");
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      lcd_exercise,      // entry
                      0,                 // entry parameter
                      "LCD_test_thread", // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()

