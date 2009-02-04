//=============================================================================
//
//      loopback.c
//
//      Standalone SPI loopback test.
//
//=============================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Chris Holgate
// Date:        2008-11-27
// Purpose:     STM32 SPI loopback test
// Description: Standalone SPI loopback test.
// Usage:       Compile as a standalone application.
//
//####DESCRIPTIONEND####
//
//=============================================================================

//=============================================================================
// This is a quick loopback test for the STM32 SPI driver.  It only checks
// the data transfer functionality - chip select handling will require
// testing with external devices.  In order to run the test, the MOSI and
// MISO pins for the test port need to be shorted together to provide an
// external loopback.  Don't do this on a bus which has external devices
// attached unless you first make sure that none of them are connected to
// the chip select used by the test harness.
// The default port and chip select used for this test are SPI bus 1,
// chip select 0.  These can be changed by editing the loopback_device
// data structure directly.
// Note that this is intended to be run as a standalone test and not as part
// of the standard board tests since it requires a hardware modification.
//=============================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>         // Test macros
#include <cyg/infra/cyg_ass.h>          // Assertion macros
#include <cyg/infra/diag.h>             // Diagnostic output

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL
#include <cyg/kernel/kapi.h>

#include <cyg/io/spi.h>                 // Common SPI API
#include <cyg/io/spi_stm32.h>           // STM32 data structures

#include <string.h>

//---------------------------------------------------------------------------
// Thread data structures.

cyg_uint8 stack [CYGNUM_HAL_STACK_SIZE_TYPICAL];
cyg_thread thread_data;
cyg_handle_t thread_handle;

externC cyg_spi_cortexm_stm32_bus_t cyg_spi_stm32_bus2;

//---------------------------------------------------------------------------
// SPI loopback device driver data structures.

cyg_spi_cortexm_stm32_device_t loopback_device = {
    .spi_device.spi_bus = &cyg_spi_stm32_bus1.spi_bus,
    .dev_num = 0 ,                      // Only 1 device. 
    .cl_pol = 1,
    .cl_pha = 1,
    .cl_brate = 8000000,                // Nominal 8Mhz.
    .cs_up_udly = 1,
    .cs_dw_udly = 1,
    .tr_bt_udly = 1,
    .bus_16bit = false,
};

//---------------------------------------------------------------------------

const char tx_data[] = "Testing, testing, 12, 123.";
const char tx_data1[] = "Testing extended API...";
const char tx_data2[] = "Testing extended API for a second transaction.";

char rx_data [sizeof(tx_data)];
char rx_data1 [sizeof(tx_data1)];
char rx_data2 [sizeof(tx_data2)];

//---------------------------------------------------------------------------
// Run single loopback transaction using simple transfer API call.

void run_test_1 (cyg_bool polled)
{
    diag_printf ("Test 1 : Simple transfer test (polled = %d).\n", polled ? 1 : 0);
    cyg_spi_transfer (&loopback_device.spi_device, polled, sizeof (tx_data), 
        (const cyg_uint8*) &tx_data[0], (cyg_uint8*) &rx_data[0]);

    diag_printf ("    Tx data : %s\n", tx_data);
    diag_printf ("    Rx data : %s\n", rx_data);
    CYG_ASSERT (memcmp (tx_data, rx_data, sizeof (tx_data)) == 0,
        "Simple transfer loopback failed - mismatched data.\n");
}

//---------------------------------------------------------------------------
// Run two loopback transactions using extended transfer API.

void run_test_2 (cyg_bool polled)
{
    diag_printf ("Test 2 : Extended API test (polled = %d).\n", polled ? 1 : 0);
    cyg_spi_transaction_begin (&loopback_device.spi_device);
    cyg_spi_transaction_transfer (&loopback_device.spi_device, polled, sizeof (tx_data1), 
        (const cyg_uint8*) &tx_data1[0], (cyg_uint8*) &rx_data1[0], false);
    cyg_spi_transaction_transfer (&loopback_device.spi_device, polled, sizeof (tx_data2), 
        (const cyg_uint8*) &tx_data2[0], (cyg_uint8*) &rx_data2[0], false);
    cyg_spi_transaction_end (&loopback_device.spi_device);

    diag_printf ("    Tx data 1 : %s\n", tx_data1);
    diag_printf ("    Rx data 1 : %s\n", rx_data1);
    diag_printf ("    Tx data 2 : %s\n", tx_data2);
    diag_printf ("    Rx data 2 : %s\n", rx_data2);
    CYG_ASSERT (memcmp (tx_data1, rx_data1, sizeof (tx_data1)) == 0,
        "Simple transfer loopback failed - mismatched data (transfer 1).\n");
    CYG_ASSERT (memcmp (tx_data2, rx_data2, sizeof (tx_data2)) == 0,
        "Simple transfer loopback failed - mismatched data (transfer 2).\n");
}

//---------------------------------------------------------------------------
// Run all PL022 SPI interface loopback tests.

void run_tests (void)
{
    diag_printf ("Running STM32 SPI driver loopback tests.\n");
    run_test_1 (true); 
    run_test_1 (false); 
    run_test_2 (true); 
    run_test_2 (false); 
    CYG_TEST_PASS_FINISH ("Loopback tests ran OK");
}

//---------------------------------------------------------------------------
// User startup - tests are run in their own thread.

void cyg_user_start(void)
{
    CYG_TEST_INIT();
    cyg_thread_create(
        10,                                   // Arbitrary priority
        (cyg_thread_entry_t*) run_tests,      // Thread entry point
        0,                                    // 
        "test_thread",                        // Thread name
        &stack[0],                            // Stack
        CYGNUM_HAL_STACK_SIZE_TYPICAL,        // Stack size
        &thread_handle,                       // Thread handle
        &thread_data                          // Thread data structure
    );
    cyg_thread_resume(thread_handle);
    cyg_scheduler_start();
}

//=============================================================================
