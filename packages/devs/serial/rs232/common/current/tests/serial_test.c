//=================================================================
//
//        serial_test.c
//
//        Test that the device API for the serial device exists.
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

#include <pkgconf/system.h>
#include <cyg/infra/testcase.h>         // Generic test macros
#ifdef CYGPKG_KERNEL
# include <pkgconf/kernel.h>
#endif

#ifndef CYGFUN_KERNEL_API_C

externC void
cyg_start(void)
{
    CYG_TEST_INIT();

    CYG_TEST_PASS_EXIT("N/A: Testing is not applicable "
                       "to this configuration");
}

#else

#include <cyg/kernel/kapi.h>            // All the kernel specific stuff
#include <cyg/devs/common/table.h>	// All the device specific stuff

#define NTHREADS 1
#define STACKSIZE 4096

static cyg_handle_t thread[NTHREADS];
static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];

char message1[] = "This is test message 1.\n";
char message2[] = "This is test message 2.\n";

/* Only call routines that do not require user input */
void entry0(CYG_ADDRWORD data)
{
    Cyg_IORB iorb1, iorb2;
    cyg_int32 baud_rate, i;

    /* Only call routines if there is a serial device in the system */
    if ( NULL == Cyg_Device_Table[0].name ||
         /* strcmp is not necessarily around */
         's'  != Cyg_Device_Table[0].name[0] ||
         'e'  != Cyg_Device_Table[0].name[1] ||
         'r'  != Cyg_Device_Table[0].name[2] ||
         'i'  != Cyg_Device_Table[0].name[3] ||
         'a'  != Cyg_Device_Table[0].name[4] ||
         'l'  != Cyg_Device_Table[0].name[5] ) {
        CYG_TEST_PASS_EXIT( "N/A: no serial device in Cyg_Device_Table" );
        return;
    }

    iorb1.callback = NULL;
    iorb1.buffer = message1;
    iorb1.buffer_length = sizeof(message1) - 1;
    cyg_write_asynchronous(Cyg_Device_Table, &iorb1);

    iorb2.buffer = message2;
    iorb2.buffer_length = sizeof(message2) - 1;
    cyg_write_blocking(Cyg_Device_Table, &iorb2);

    for (i = 0; i < 1000000; i++); // Delay to flush the tx buffer

    if ((cyg_serial_rs232_get_baud_rate(Cyg_Device_Table, &baud_rate )) >= 0) {
        CYG_TEST_PASS_FAIL(
          (cyg_serial_rs232_set_baud_rate(Cyg_Device_Table, baud_rate,&i) >= 0),
          "Setting baud rate to previous baud rate.");
    } else {
        CYG_TEST_FAIL("Could not get baud rate.");
    }

    CYG_TEST_PASS_EXIT("serial_test");
}

/* Make calls to the rest of the API */
void entry_bogus(CYG_ADDRWORD data)
{
    Cyg_IORB iorb;
    char buf[1024];
    cyg_uint32 old;
    iorb.buffer = buf;
    iorb.buffer_length = sizeof(buf);

    // Set mode to interrupt
    cyg_serial_rs232_set_kmode(Cyg_Device_Table, (cyg_uint32)1);
    // cyg_serial_rs232_get_kmode(Cyg_Device_Table);	// Doesn't exist yet

    // These could go in the entry0 routine
    cyg_serial_rs232_get_line_mode(Cyg_Device_Table, &old);		
    cyg_serial_rs232_set_line_mode(Cyg_Device_Table, 1, &old);

    cyg_serial_rs232_get_baud_rate(Cyg_Device_Table, &old);
    cyg_serial_rs232_set_baud_rate(Cyg_Device_Table, 1, &old);

    cyg_serial_rs232_get_read_mode(Cyg_Device_Table, &old);
    cyg_serial_rs232_set_read_mode(Cyg_Device_Table, 1, &old);

    cyg_serial_rs232_set_read_data(Cyg_Device_Table, 1, NULL, 0);
    cyg_read_asynchronous(Cyg_Device_Table, &iorb);
    cyg_read_blocking(Cyg_Device_Table, &iorb);
    cyg_read_cancel(Cyg_Device_Table, &iorb);

    // cyg_serial_rs232_set_write_mode(Cyg_Device_Table, 1);// Doesn't exist yet
    // cyg_serial_rs232_get_write_mode(Cyg_Device_Table);// Doesn't exist yet
    cyg_write_asynchronous(Cyg_Device_Table, &iorb);
    cyg_write_blocking(Cyg_Device_Table, &iorb);
    // cyg_write_cancel(Cyg_Device_Table, &iorb);	// Doesn't exist yet
}

externC void
cyg_start(void)
{
    CYG_TEST_INIT();

    cyg_thread_create(4, entry0, (cyg_addrword_t)0, "serial_test",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_EXIT("Not reached");

    entry_bogus(0); // To prevent selective linking from GC all the bogus calls.
}

#endif // ifdef CYGFUN_KERNEL_API_C

// EOF serial_test.cxx
