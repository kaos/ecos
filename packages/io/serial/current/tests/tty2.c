//==========================================================================
//
//        tty2.c
//
//        Test simple string output.
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
// by Cygnus are Copyright (C) 1998, 1999 Cygnus Solutions.  
// All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          1999-03-17
// Description:   Test simple string output.
// 
//####DESCRIPTIONEND####

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#if defined(CYGPKG_IO_SERIAL) && defined(CYGPKG_KERNEL)

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#ifdef CYGFUN_KERNEL_API_C
#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>
unsigned char stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
cyg_thread thread_data;
cyg_handle_t thread_handle;
#endif
#endif

#include "ser_test_protocol.inl"

//---------------------------------------------------------------------------
// TTY test main function.
void
tty_test( void )
{
    char test_msg1[]="This is a test message!\n";
    char test_msg2[]="$O5468697320697320612074657374206d657373616765210d0a#12";
    int msglen;
    cyg_io_handle_t tty_handle;

    test_open_tty(&tty_handle);

    CYG_TEST_INFO("Writing a raw string to the TTY device...");
    msglen = strlen(&test_msg1[0]);
    Tcyg_io_write(tty_handle, &test_msg1[0], &msglen);

    CYG_TEST_INFO("Writing a GDB encoded string to the TTY device...");
    msglen = strlen(&test_msg2[0]);
    Tcyg_io_write(tty_handle, &test_msg2[0], &msglen);

    CYG_TEST_PASS_FINISH("tty2 test OK");
}

void
cyg_start(void)
{
    CYG_TEST_INIT();
#ifdef CYGFUN_KERNEL_API_C
    cyg_thread_create(10,               // Priority - just a number
                      (cyg_thread_entry_t*)tty_test, // entry
                      0,                // 
                      "tty_thread",  // Name
                      &stack[0],        // Stack
                      CYGNUM_HAL_STACK_SIZE_TYPICAL, // Size
                      &thread_handle,   // Handle
                      &thread_data      // Thread data structure
        );
    cyg_thread_resume(thread_handle);
    cyg_scheduler_start();
#else
    tty_test();
#endif
}

#else // ifdef CYGPKG_IO_SERIAL

void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA("Requires serial device driver and kernel");
}

#endif
// EOF tty2.c
