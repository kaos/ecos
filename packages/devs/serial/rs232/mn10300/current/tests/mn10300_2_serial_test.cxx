//=================================================================
//
//        mn10300_2_serial_test.cxx
//
//        Test that the device API for the mn10300_2 device exists.
//
//=================================================================
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
// October 31, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     proven
// Contributors:  proven
// Date:          1998-10-08
// Description:   Tests basic serial functionality.
//####DESCRIPTIONEND####

// #include <pkgconf/devs.h>
#include <pkgconf/devices.h>		// For now include all the devices

#include <cyg/infra/testcase.h>         // Generic test macros

#ifdef CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME
#define CYG_DEVICE_SERIAL_RS232_NAME CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME

#define NTHREADS 1
#include <cyg/kernel/kernel.hxx>        // All the kernel specific stuff
#include <cyg/devs/common/testaux.hxx>

// #define CYG_DEVICE_INTERNAL
// #include <cyg/devs/serial/rs232/mn10300/serial_mn10300_2.hxx>
// #include <cyg/devs/serial/rs232/common/tests/serial_.ht>

char message1[] = "This is test message 1.\n";
char message2[] = "This is test message 2.\n";

/* Only call routines that do not require user input */
void entry0(CYG_ADDRWORD data)
{
    Cyg_IORB iorb1, iorb2;
    cyg_int32 baud_rate, i;

    iorb1.callback = NULL;
    iorb1.buffer = message1;
    iorb1.buffer_length = sizeof(message1) - 1;
// Because the testing infrastructure currently uses the hal
// We cannot do ANY calls to the device driver.
//    CYG_DEVICE_SERIAL_RS232_NAME.io_write_asynchronous(&iorb1);

    iorb2.buffer = message2;
    iorb2.buffer_length = sizeof(message2) - 1;
//    CYG_DEVICE_SERIAL_RS232_NAME.io_write_blocking(&iorb2);

    for (i = 0; i < 1000000; i++); // Delay to flush the tx buffer

    baud_rate = i;
//    if ((baud_rate = CYG_DEVICE_SERIAL_RS232_NAME.get_baud_rate()) >= 0) {
//        CYG_TEST_PASS_FAIL(
//          (CYG_DEVICE_SERIAL_RS232_NAME.set_baud_rate(baud_rate) >= 0),
//          "Setting baud rate to previous baud rate.");
//    } else {
//        CYG_TEST_FAIL("Could not get baud rate.");
//    }

    CYG_TEST_PASS_EXIT("serial_test");
}

/* Make calls to the rest of the API */
void entry_bogus(CYG_ADDRWORD data)
{
    Cyg_IORB iorb;
    char buf[1024];
    iorb.buffer = buf;
    iorb.buffer_length = sizeof(buf);

    // Set mode to interrupt
    CYG_DEVICE_SERIAL_RS232_NAME.set_kmode((cyg_uint32)1);
    // CYG_DEVICE_SERIAL_RS232_NAME.get_kmode();	// Doesn't exist yet

    // These could go in the entry0 routine
    CYG_DEVICE_SERIAL_RS232_NAME.get_line_mode();		
    CYG_DEVICE_SERIAL_RS232_NAME.set_line_mode(1);

    CYG_DEVICE_SERIAL_RS232_NAME.get_baud_rate();
    CYG_DEVICE_SERIAL_RS232_NAME.set_baud_rate(1);

    CYG_DEVICE_SERIAL_RS232_NAME.set_read_mode(1);
    // CYG_DEVICE_SERIAL_RS232_NAME.get_read_mode();	// Doesn't exist yet
    CYG_DEVICE_SERIAL_RS232_NAME.set_read_data(1, NULL, 0);
    CYG_DEVICE_SERIAL_RS232_NAME.io_read_asynchronous(&iorb);
    CYG_DEVICE_SERIAL_RS232_NAME.io_read_blocking(&iorb);
    CYG_DEVICE_SERIAL_RS232_NAME.io_read_cancel(&iorb);

    CYG_DEVICE_SERIAL_RS232_NAME.set_write_mode(1);
    // CYG_DEVICE_SERIAL_RS232_NAME.get_write_mode();	// Doesn't exist yet
    CYG_DEVICE_SERIAL_RS232_NAME.io_write_asynchronous(&iorb);
    CYG_DEVICE_SERIAL_RS232_NAME.io_write_blocking(&iorb);
    // CYG_DEVICE_SERIAL_RS232_NAME.io_write_cancel(&iorb);// Doesn't exist yet
}

externC void
cyg_start(void)
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_EXIT("Not reached");

    entry_bogus(0); // To prevent selective linking from GC all the bogus calls.
}

#else // CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME

externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_EXIT("N/A: required serial device not present");
}

#endif // CYG_DEVICE_SERIAL_RS232_MN10300_2_NAME

// EOF mn10300_2_serial_tests.cxx
