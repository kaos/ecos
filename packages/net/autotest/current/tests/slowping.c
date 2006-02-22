//==========================================================================
//
//      autotest/current/tests/slowping.c
//
//      Simple pinging test, the server pings me and little else.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt,gthomas
// Contributors: hmt,gthomas
// Date:         2000-10-23
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>

#include <cyg/infra/testcase.h>         // testing infrastructure

#include <pkgconf/net.h>

#ifdef CYGBLD_DEVS_ETH_DEVICE_H    // Get the device config if it exists
#include CYGBLD_DEVS_ETH_DEVICE_H  // May provide CYGTST_DEVS_ETH_TEST_NET_REALTIME
#endif

#ifdef CYGPKG_NET_TESTS_USE_RT_TEST_HARNESS // do we use the rt test?
# ifdef CYGTST_DEVS_ETH_TEST_NET_REALTIME // Get the test ancilla if it exists
#  include CYGTST_DEVS_ETH_TEST_NET_REALTIME
# endif
#endif


// Fill in the blanks if necessary
#ifndef TNR_OFF
# define TNR_OFF()
#endif
#ifndef TNR_ON
# define TNR_ON()
#endif
#ifndef TNR_INIT
# define TNR_INIT()
#endif
#ifndef TNR_PRINT_ACTIVITY
# define TNR_PRINT_ACTIVITY()
#endif

// ------------------------------------------------------------------------

#include <errno.h>
#include <network.h>

#include "autohost.inl"

#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + 0x10000)
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

static void
do_ping_tests(struct bootp *bp, int N, int testtime, int filesize)
{
    int i;
    char order[256];
    diag_printf( "INFO: telling %s to run %d pings for %d seconds, %d bytes and up\n",
          inet_ntoa(bp->bp_siaddr), N, testtime, filesize );

    // For 2nd and subsequent orders, we vary the ping data size:
    for ( i = 0; i < N; i++ ) {
        sprintf( order, "%s %s %d %d", "SLOW_PING",
                 inet_ntoa(bp->bp_yiaddr),
                 testtime,
                 filesize + 50 * i );
        autohost_tell( bp, order );
    }
}

#define TESTTIME (5 * 60) // Seconds

#define NUM_SESSIONS 10 // why not?

void
net_test(cyg_addrword_t param)
{
    int i;
    int numtests;
    CYG_TEST_INIT();
    CYG_TEST_INFO("Start PING server test");
    init_all_network_interfaces();

    autohost_init();

    TNR_INIT();
    
    numtests = NUM_SESSIONS; // The number of pingers started OK
    i = numtests;
    // Now command the host to do ping to us...
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        // 2 small tests by default and a couple of larger ones
        do_ping_tests(&eth0_bootp_data, 2, TESTTIME, 64);
        do_ping_tests(&eth0_bootp_data, 2, TESTTIME, 1000);
        do_ping_tests(&eth0_bootp_data, 2, TESTTIME, 10000);
        i -= 6;
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up && i > 0) {
        // rest of the tests depending...
        do_ping_tests(&eth1_bootp_data, 1, TESTTIME, 64);
        i--;
        if ( i > 0 ) {
            do_ping_tests(&eth1_bootp_data, 1, TESTTIME, 4000);
            i--;
        }
        if ( i > 0 ) {
            do_ping_tests(&eth1_bootp_data, i, TESTTIME, 1200);
            i = 0;
        }
    }
#endif
    numtests -= i; // Adjust to how many we *actually* requested

    // Let the server run for 5 minutes
    cyg_thread_delay(2*100); // let the stuff start up first
    TNR_ON();
    cyg_thread_delay(TESTTIME*100); // FIXME - assume cS clock.
    // Additional delay 'cos host may be slower than us - and it has to
    // complete a transfer anyway:
    cyg_thread_delay(  30    *100); // FIXME - assume cS clock.
    TNR_OFF();

    autohost_end( numtests ); // check for N pass messages from hosts

    TNR_PRINT_ACTIVITY();
    CYG_TEST_EXIT("Done");
}

void
cyg_start(void)
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      net_test,          // entry
                      0,                 // entry parameter
                      "Network test",    // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
}

// EOF slowping.c
