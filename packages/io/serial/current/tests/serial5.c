//==========================================================================
//
//        serial5.c
//
//        Test data duplex receive and send.
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jskov
// Contributors:  jskov
// Date:          1999-03-19
// Description:   Test the duplex receive and send capabilities of 
//                the serial driver.
// Requirements:  This test requires the ser_filter on the host side.
//####DESCRIPTIONEND####

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

// Package requirements
#if defined(CYGPKG_IO_SERIAL) && defined(CYGPKG_KERNEL)

#include <pkgconf/kernel.h>

// Package option requirements
#if defined(CYGFUN_KERNEL_API_C)

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>
unsigned char stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
cyg_thread thread_data;
cyg_handle_t thread_handle;

#include "ser_test_protocol.inl"

//---------------------------------------------------------------------------
// Serial test main function.
void
serial_test( void )
{
    cyg_io_handle_t ser_handle;

    test_open_ser(&ser_handle);

    // We need the filter for this test.
    test_ping(ser_handle);


#if defined(CYGPKG_HAL_MN10300)         \
    || defined(CYGPKG_HAL_ARM_AEB)      \
    || 0
    {
        // Some boards are too slow to run the driver in interrupt mode
        // at the default 38400 baud when doing this test.
        cyg_ser_cfg_t cfg = { 
            CYGNUM_SERIAL_BAUD_9600, CYGNUM_SERIAL_WORD_LENGTH_8, 
            CYGNUM_SERIAL_STOP_1, CYGNUM_SERIAL_PARITY_NONE };

        change_config(ser_handle, &cfg);
    }
#endif

    // Start out slow, then go for many tests. Each cycle causes
    // 512 bytes to be sent over the wire.
    test_binary(ser_handle,    1, MODE_DUPLEX_ECHO);
    test_binary(ser_handle,    2, MODE_DUPLEX_ECHO);
    test_binary(ser_handle,    5, MODE_DUPLEX_ECHO);

#if 0 // Disable these until the ser_filter produces status output.
    test_binary(ser_handle,  128, MODE_DUPLEX_ECHO);
    test_binary(ser_handle,  512, MODE_DUPLEX_ECHO);
    test_binary(ser_handle, 1024, MODE_DUPLEX_ECHO);
#endif

    CYG_TEST_PASS_FINISH("serial5 test OK");
}

void
cyg_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(10,                   // Priority - just a number
                      (cyg_thread_entry_t*)serial_test,         // entry
                      0,                    // 
                      "serial_thread",     // Name
                      &stack[0],            // Stack
                      CYGNUM_HAL_STACK_SIZE_TYPICAL,           // Size
                      &thread_handle,       // Handle
                      &thread_data          // Thread data structure
        );
    cyg_thread_resume(thread_handle);
    cyg_scheduler_start();
}

#else // CYGFUN_KERNEL_API_C
#define N_A_MSG "Needs kernel C API"
#endif

#else // CYGPKG_IO_SERIAL && CYGPKG_KERNEL
#define N_A_MSG "Needs IO/serial and Kernel"
#endif

#ifdef N_A_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG);
}
#endif // N_A_MSG
// EOF serial5.c
