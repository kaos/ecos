//==========================================================================
//
//        flow2.c
//
//        Test duplex receive and send with flow control.
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
// Author(s):     jskov,jlarmour
// Contributors:  
// Date:          2000-07-27
// Description:   Test the duplex receive and send capabilities of 
//                the serial driver with flow control.
// Requirements:  This test requires the ser_filter on the host side.
// 
//####DESCRIPTIONEND####

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // test macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif
#ifdef CYGPKG_IO_SERIAL
# include <pkgconf/io_serial.h>
#endif

// Package requirements
#ifndef CYGPKG_IO_SERIAL
# define NA_MSG "Requires I/O serial package"
#elif !defined(CYGFUN_KERNEL_API_C)
# define NA_MSG "Requires kernel C API"
#elif !defined(CYGPKG_IO_SERIAL_FLOW_CONTROL)
# define NA_MSG "Requires serial flow control"
#endif

#ifdef NA_MSG
void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG);
}
#else

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>
unsigned char stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
cyg_thread thread_data;
cyg_handle_t thread_handle;

#include "ser_test_protocol.inl"


//---------------------------------------------------------------------------
// run the tests

static void
run_tests( cyg_io_handle_t ser_handle )
{
    // Start slowly, then go for max size.
    {
        test_binary(ser_handle,             16, MODE_DUPLEX_ECHO);
        test_binary(ser_handle,            128, MODE_DUPLEX_ECHO);
        test_binary(ser_handle,            256, MODE_DUPLEX_ECHO);
        test_binary(ser_handle,           1024, MODE_DUPLEX_ECHO);
    }
}

//---------------------------------------------------------------------------
// Serial test main function.

void
serial_test( void )
{
    cyg_io_handle_t ser_handle;
    cyg_ser_cfg_t *cfg=&test_configs[0];
    cyg_ser_cfg_t new_cfg;
    int count = sizeof(test_configs) / sizeof(cyg_ser_cfg_t);
    int i;

    test_open_ser(&ser_handle);

    // We need the filter for this test.
    test_ping(ser_handle);

    // Choose the configuration with the fastest baud rate, to be most
    // provocative. Start at 1 coz cfg already points at 0
    for (i=1; i<count; i++) {
        if (cfg->baud_rate < test_configs[i].baud_rate)
            cfg=&test_configs[i];
    }

    // Set flow control from configuration
    // Choose software first

#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_SOFTWARE
    CYG_TEST_INFO("Setting software flow control");

    new_cfg = *cfg;
    new_cfg.flags |= CYGNUM_SERIAL_FLOW_XONXOFF_RX |
                     CYGNUM_SERIAL_FLOW_XONXOFF_TX;
    if (ENOERR == change_config(ser_handle, &new_cfg))
        run_tests( ser_handle );
#endif

    // hardware flow control
#ifdef CYGOPT_IO_SERIAL_FLOW_CONTROL_HW
    CYG_TEST_INFO("Setting hardware flow control");

    new_cfg = *cfg;
    new_cfg.flags |= CYGNUM_SERIAL_FLOW_RTSCTS_RX|CYGNUM_SERIAL_FLOW_RTSCTS_TX;
    if (ENOERR == change_config(ser_handle, &new_cfg))
        run_tests( ser_handle );
#endif
 
    CYG_TEST_PASS_FINISH("flow2 test OK");
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

#endif // ifndef NA_MSG

// EOF flow2.c
