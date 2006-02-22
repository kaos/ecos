//==========================================================================
//
//      tests/auto/autohost.inl
//
//      Automated Testing by a Host Computer
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
// Date:         2000-10-18
// Purpose:      Automated testing of the eCos TCP/IP Network Stack
// Description:  

//	This .inl file defines routines which send /orders/ to the server
//	running on the host machine who served us DHCP, and receive
//	/results/ from the same machine or machines.
//
//	autohost_tell() sends the order to an app "awaitorder" which feeds
//	the order into obey.sh which obeys the order, for example, running
//	some tftp sessions to exercise the tftp server.
//
//	The /orders/ must come from a predefined list, to match the
//	selection which obey.sh is expecting, for example "TFTP_SERV_GET"
//	which has further parameters <target> <timeout>.  Additional
//	parameters are added/manipulated by obey.sh which then invokes a
//	relevent script (tftpget.sh) to do the test.
//
//	The test script's output is returned to us, and is collected by
//	autohost_getresults() which runs in a high-ist prio thread.  It
//	prints the output, which is expected to match the traditional eCos
//	PASS:<happy> FAIL:<eek!> sort of format, so that the test farm can
//	collect these messages from the testcase output, without any direct
//	connection to the server.
//
//	autohost_init() simply starts the autohost_getresults() thread.
//
//	The predefined list of orders is *not* defined here or anywhere
//	else besides obey.sh which does a switch statement for them, to
//	make it easier to extent; similarly the params each order takes are
//	not centrally recorded either.  It is however a convention that the
//	word following the /order/ is our own IP address.
//
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/infra/testcase.h>         // testing infrastructure

#include "memcheck.inl"                 // check on memory usage

externC int sprintf( char * /* str */, const char * /* format */, ... );

// ------------------------------------------------------------------------

#include <pkgconf/system.h>
#ifdef CYGPKG_IO_ETH_DRIVERS
#include <pkgconf/io_eth_drivers.h>
#endif

#if defined(XFAIL) || defined(XPASS)
// So that the test can override this automation
# if defined(XFAIL) && defined(XPASS)
#  error Both XFAIL and XPASS predefined
# endif
# undef XFAIL
# ifndef XPASS
#  define XFAIL "XFAIL"
# endif
#else
# ifdef CYGPKG_IO_ETH_DRIVERS_SIMULATED_FAILURES
// Then we must tell the test server to expect failures.
#  define XFAIL "XFAIL"
# endif
#endif // XFAIL/XPASS not predefined

// ------------------------------------------------------------------------

void
pexit(char *s)
{
    TNR_OFF();
    CYG_TEST_FAIL_EXIT( s );
}

// ------------------------------------------------------------------------

#define SOURCE_PORT 9980
#define RESULT_PORT 9990

static int
autohost_tell( struct bootp *bp, char *orders )
{
    int s_host;
    struct sockaddr_in slave, local;
    int one = 1;
    int len = strlen( orders );
    char ack[32];
    int ret;

    char complete_order[256];

    if ( len > 240 )
        pexit( "Orders too long" );

    memset(&slave, 0, sizeof(slave));
    slave.sin_family = AF_INET;
    slave.sin_len = sizeof(slave);
    slave.sin_port = htons(SOURCE_PORT);
    slave.sin_addr = bp->bp_siaddr; // Talk to our DHCP server

    s_host = socket(AF_INET, SOCK_STREAM, 0);
    if (s_host < 0) {
        pexit("stream socket");
    }

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_len = sizeof(local);
    local.sin_port = INADDR_ANY;
    local.sin_addr.s_addr = INADDR_ANY;
    if(bind(s_host, (struct sockaddr *) &local, sizeof(local)) < 0) {
        pexit("bind /source/ error");
   }
    if (setsockopt(s_host, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        pexit("setsockopt /source/ SO_REUSEADDR");
    }

    ret = connect(s_host, (struct sockaddr *)&slave, sizeof(slave));
    if ( ret < 0) {
        TNR_OFF();
        diag_printf(
            "Connect failed %d err %d, s_host %d, sin_port %x, sin_addr %08x\n", 
                    ret, errno, s_host, slave.sin_port, slave.sin_addr.s_addr );
        pexit("Can't connect to target");
    }

    sprintf( complete_order,
#ifdef XFAIL
             "%s %s", XFAIL,
#else
             "%s",
#endif
             orders );
    len = 1 + strlen( complete_order ); // Send the null also

    if ( len != write(s_host, (unsigned char *)complete_order, len ) ) {
        pexit("Can't send orders");
    }
    ret = read(s_host, (unsigned char *)&ack, sizeof(ack));
    if ( 0 >= ret ) {
        pexit("Can't get ACK");
    }
    close(s_host);
    return 1;
}

// ---------------------------------------------------------------------------

#define nHALTAFTERFAIL
#ifdef HALTAFTERFAIL
static cyg_alarm ahr_fail_alarm;
static cyg_handle_t ahr_fail_alarm_handle = 0;
static cyg_alarm_t ahr_fail_alarm_function;

void ahr_fail_alarm_function(cyg_handle_t alarm, cyg_addrword_t data)
{
    CYG_TEST_FAIL_EXIT( "Failure reported by autohost" );
}
#endif

static int result_passes = 0;
static int result_fails  = 0;

static void
autohost_getresults( cyg_addrword_t loop )
{
    int s_source, e_source;
    struct sockaddr_in e_source_addr, local;
    int one = 1;
    int len;
    char result[256];

    s_source = socket(AF_INET, SOCK_STREAM, 6 /* TCP */ );
    if (s_source < 0) {
        pexit("stream socket");
    }

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_len = sizeof(local);
    local.sin_port = ntohs(RESULT_PORT);
    local.sin_addr.s_addr = INADDR_ANY;
    if(bind(s_source, (struct sockaddr *) &local, sizeof(local)) < 0) {
        pexit("bind /source/ error");
    }
    if (setsockopt(s_source, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(one))) {
        pexit("setsockopt /source/ SO_REUSEADDR");
    }
#ifdef SO_REUSEPORT
    if (setsockopt(s_source, SOL_SOCKET, SO_REUSEPORT, &one, sizeof(one))) {
        pexit("setsockopt /source/ SO_REUSEPORT");
    }
#endif

    listen(s_source, SOMAXCONN);

    do {
        len = sizeof(e_source_addr);
        if ((e_source = accept(s_source,
                 (struct sockaddr *)&e_source_addr, &len)) < 0) {
            pexit("accept /source/");
        }
        
        // Wait for "source" to tell us the result
        if ( 0 >= (len = read(e_source, result, sizeof(result)))) {
            pexit("Can't read result");
        }

        result[len] = 0;
        len--;
        while( len && ('\r' == result[len] || '\n' == result[len]) )
            result[len--] = 0;

        TNR_OFF();
        diag_printf( "%s (from %s:%d)\n", result,
           inet_ntoa(e_source_addr.sin_addr), ntohs(e_source_addr.sin_port));
        TNR_ON();

        // Check for a pass message right now:
        if ( 'P' == result[0] &&
             'A' == result[1] &&
             'S' == result[2] &&
             'S' == result[3] ) {
            result_passes++;
        }
        else
        // Check for a failure right now:
        if ( 'F' == result[0] &&
             'A' == result[1] &&
             'I' == result[2] &&
             'L' == result[3] ) {
            result_fails++;
#ifdef HALTAFTERFAIL
            // Wait a bit for the others, if any, to come in before halting
            // the test, since the host is typically running multiple
            // threads to torment us.  Set an alarm to go FAIL_EXIT in 20S.
            if ( ! ahr_fail_alarm_handle ) {
                cyg_handle_t h;
                cyg_clock_to_counter(cyg_real_time_clock(), &h),
                cyg_alarm_create(
                    h,                       /* Attached to this ctr */
                    ahr_fail_alarm_function, /* Call-back function   */
                    0,                       /* Call-back data       */
                    &ahr_fail_alarm_handle,  /* Returned alarm obj   */
                    &ahr_fail_alarm          /* put alarm here       */
                    );
                cyg_alarm_initialize(
                    ahr_fail_alarm_handle,
                    cyg_current_time() + (20 * 100),
                    0 );
                cyg_alarm_enable( ahr_fail_alarm_handle );
            }
#endif
        }

        close( e_source );
    } while ( loop );

    close( s_source );
}

// ---------------------------------------------------------------------------

static void autohost_end( int expected_passes )
{
#ifdef HALTAFTERFAIL
    if ( ahr_fail_alarm_handle )
        cyg_alarm_disable( ahr_fail_alarm_handle );
#endif
#ifdef XFAIL
    if ( ! (result_passes >= expected_passes) )
        CYG_TEST_INFO( "XFAIL: Not enough PASS results posted" );
    if ( ! (0 == result_fails) )
        CYG_TEST_INFO( "XFAIL: FAIL results posted" );
    if ( ! (result_passes >= expected_passes && 0 == result_fails) )
        CYG_TEST_PASS( "XFAIL: Pass anyway" );
#else
    CYG_TEST_CHECK( result_passes >= expected_passes,
                    "Not enough PASS results posted" );
    CYG_TEST_CHECK( 0 == result_fails,
                    "FAIL results posted" );
#endif
    if ( result_passes >= expected_passes && 0 == result_fails )
        CYG_TEST_PASS( "Results from host[s] OK" );

    // Record and check final state of net stack memory consumption
    memcheck_final();
}

// ---------------------------------------------------------------------------

static char ahr_stack[ CYGNUM_HAL_STACK_SIZE_TYPICAL + 0x400 ];
static cyg_thread ahr_thread_data;

static void autohost_init( void )
{
    cyg_handle_t ahr_thread_handle;
    cyg_thread_create(    5,            // Priority - just a number
           autohost_getresults,         // entry
           1,                           // entry parameter
           "Autohost result monitor",   // Name
           &ahr_stack[0],               // Stack
           sizeof(ahr_stack),           // Size
           &ahr_thread_handle,          // Handle
           &ahr_thread_data             // Thread data structure
        );
    cyg_thread_resume(ahr_thread_handle);  // Start it

    // Record initial state of net stack memory consumption
    memcheck_init();
}

// EOF autohost.inl
