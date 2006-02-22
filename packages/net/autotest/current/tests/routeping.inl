//==========================================================================
//
//      autotest/current/tests/routeping.inl
//
//      Simple pinging test, the server pings me and I ping her.
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

// ------------------------------------------------------------------------

externC int sscanf( char * /* str */, const char * /* format */, ... );

// ------------------------------------------------------------------------

static int route_add_s( struct sockaddr_in *target,
                        struct sockaddr_in *gw,
                        int maskbits,
                        int metric )
{
    struct ecos_rtentry route;
    struct sockaddr_in mask;

    int s;

    memcpy( &mask, gw, sizeof(*gw) );
    maskbits--;
    mask.sin_addr.s_addr = htonl( (0xfffffffful ^ ((0x80000000ul >> maskbits)-1)) );

    memset( &route, 0, sizeof(route) );

    memcpy( &route.rt_dst    ,  target, sizeof(*target) );
    memcpy( &route.rt_gateway,  gw    , sizeof(*gw) );
    memcpy( &route.rt_genmask, &mask  , sizeof(mask) ); 

    route.rt_flags = RTF_UP|RTF_GATEWAY;
    route.rt_metric = metric;

    route.rt_dev = NULL;

    diag_printf("INFO:<Route - dst: %s",
                inet_ntoa(((struct sockaddr_in *)&route.rt_dst)->sin_addr));
    diag_printf(", mask: %s",
                inet_ntoa(((struct sockaddr_in *)&route.rt_genmask)->sin_addr));
    diag_printf(", gateway: %s>\n",
                inet_ntoa(((struct sockaddr_in *)&route.rt_gateway)->sin_addr));

    s = socket( AF_INET, SOCK_DGRAM, 0 );
    if (s < 0) {
        perror( "socket" );
        return false;
    }
    if (ioctl(s, SIOCADDRT, &route)) {
        perror("SIOCADDRT");
        close(s);
        return false;
    }
    diag_printf( "PASS:<Route added OK>\n" );
    close(s);
    return true;
}



int route_add( char *target,
                      char *gw,
                      int maskbits,
                      int metric )
{
    struct sockaddr_in t_s, gw_s;
    int ints[4];

    memset( &t_s,  0, sizeof(t_s)  );
    memset( &gw_s, 0, sizeof(gw_s) );

    t_s.sin_len    = gw_s.sin_len    = sizeof(t_s);
    t_s.sin_family = gw_s.sin_family = AF_INET;

    if ( 4 != sscanf( target, "%d.%d.%d.%d", ints, ints+1, ints+2, ints+3 ) )
        CYG_TEST_FAIL( "sscanf of target IP addr" );
    else
        t_s.sin_addr.s_addr = htonl( 
            (ints[0] << 24) |
            (ints[1] << 16) |
            (ints[2] <<  8) |
            (ints[3]      )  );
    
    if ( 4 != sscanf( gw, "%d.%d.%d.%d", ints, ints+1, ints+2, ints+3 ) )
        CYG_TEST_FAIL( "sscanf of target IP addr" );
    else
        gw_s.sin_addr.s_addr = htonl( 
            (ints[0] << 24) |
            (ints[1] << 16) |
            (ints[2] <<  8) |
            (ints[3]      )  );

    return route_add_s( &t_s, &gw_s, maskbits, metric );
}

// ------------------------------------------------------------------------

#define NUM_PINGS 16
#define MAX_PACKET 4096
#define MIN_PACKET   64
#define MAX_SEND   4000

#define PACKET_ADD  ((MAX_SEND - MIN_PACKET)/NUM_PINGS)
#define nPACKET_ADD  1 

static unsigned char pkt1[MAX_PACKET], pkt2[MAX_PACKET];

#define UNIQUEID 0x1234

// Compute INET checksum
int
inet_cksum(u_short *addr, int len)
{
    register int nleft = len;
    register u_short *w = addr;
    register u_short answer;
    register u_int sum = 0;
    u_short odd_byte = 0;

    /*
     *  Our algorithm is simple, using a 32 bit accumulator (sum),
     *  we add sequential 16 bit words to it, and at the end, fold
     *  back all the carry bits from the top 16 bits into the lower
     *  16 bits.
     */
    while( nleft > 1 )  {
        sum += *w++;
        nleft -= 2;
    }

    /* mop up an odd byte, if necessary */
    if( nleft == 1 ) {
        *(u_char *)(&odd_byte) = *(u_char *)w;
        sum += odd_byte;
    }

    /*
     * add back carry outs from top 16 bits to low 16 bits
     */
    sum = (sum >> 16) + (sum & 0x0000ffff); /* add hi 16 to low 16 */
    sum += (sum >> 16);                     /* add carry */
    answer = ~sum;                          /* truncate to 16 bits */
    return (answer);
}

static int
errshorts, errinvalids, errbadids, errwronghosts, errsendtos, errrecvfroms;
#define ERRRESET() \
errshorts=errinvalids=errbadids=errwronghosts=errsendtos=errrecvfroms=0

static int
check_icmp(unsigned char *pkt, int len, 
          struct sockaddr_in *from, struct sockaddr_in *to)
{
    cyg_tick_count_t *tp, tv;
    struct ip *ip;
    struct icmp *icmp;
    tv = cyg_current_time();
    ip = (struct ip *)pkt;
    if (len < sizeof(*ip))
        if (ip->ip_v != IPVERSION) {
        errshorts++;
        return 0;
    }
    icmp = (struct icmp *)(pkt + sizeof(*ip));
    len -= (sizeof(*ip) + 8);
    tp = (cyg_tick_count_t *)&icmp->icmp_data;
    if (icmp->icmp_type != ICMP_ECHOREPLY) {
        errinvalids++;
        return 0;
    }
    if (icmp->icmp_id != UNIQUEID) {
        errbadids++;
        return 0;
    }
    if (from->sin_addr.s_addr != to->sin_addr.s_addr) {
        errwronghosts++;
        return 0;
    }
    return 1;
}

// expect is
// 0 - require failure
// 1 - require success
// 2 - don't care

#define FAILURE_REQUIRED 0
#define SUCCESS_REQUIRED 1
#define NOTHING_REQUIRED 2

static void
ping_host(int s, struct sockaddr_in *host, int expect )
{
    struct icmp *icmp = (struct icmp *)pkt1;
    int icmp_len = MIN_PACKET;
    int seq, ok_recv, bogus_recv;
    cyg_tick_count_t *tp;
    long *dp;
    struct sockaddr_in from;
    int i, len, fromlen;

    ERRRESET();
    ok_recv = 0;
    bogus_recv = 0;
    TNR_OFF();
    diag_printf("INFO:<PING server %s ....>\n", inet_ntoa(host->sin_addr));
    for (seq = 0;  seq < NUM_PINGS;  seq++, icmp_len += PACKET_ADD ) {
        TNR_ON();
        cyg_thread_delay( 47 ); // Half a second...
        // Build ICMP packet
        icmp->icmp_type = ICMP_ECHO;
        icmp->icmp_code = 0;
        icmp->icmp_cksum = 0;
        icmp->icmp_seq = seq;
        icmp->icmp_id = 0x1234;
        // Set up ping data
        tp = (cyg_tick_count_t *)&icmp->icmp_data;
        *tp++ = cyg_current_time();
        dp = (long *)tp;
        for (i = sizeof(*tp);  i < icmp_len;  i += sizeof(*dp)) {
            *dp++ = i;
        }
        // Add checksum
        icmp->icmp_cksum = inet_cksum( (u_short *)icmp, icmp_len+8);
        // Send it off
        if (sendto(s, icmp, icmp_len+8, 0, (struct sockaddr *)host, sizeof(*host)) < 0) {
            errsendtos++;
            continue;
        }
        // Wait for a response
        fromlen = sizeof(from);
        len = recvfrom(s, pkt2, sizeof(pkt2), 0, (struct sockaddr *)&from, &fromlen);
        if (len < 0) {
            errrecvfroms++;
            icmp_len = MIN_PACKET - PACKET_ADD; // just in case - long routes
        } else {
            if ( check_icmp(pkt2, len, &from, host) )
                ok_recv++;
            else
                bogus_recv++;
        }
    }
    TNR_OFF();
    diag_printf("INFO:<Sent %d packets, received %d OK, %d bad>\n", NUM_PINGS, ok_recv, bogus_recv);
    if ( errsendtos + errrecvfroms )
        diag_printf("INFO:<%d sendto errors, %d recvfrom errors>\n", errsendtos, errrecvfroms );

    if ( SUCCESS_REQUIRED == expect ) {
        CYG_TEST_CHECK( 0 < ok_recv, "No pings succeeded" );
        CYG_TEST_CHECK( 0 == errsendtos, "Sendto failures" );
#ifndef XFAIL
        CYG_TEST_CHECK( NUM_PINGS/2 <  ok_recv, "Not enough pings succeeded" );
        CYG_TEST_CHECK( NUM_PINGS/2 > bogus_recv, "Too many pings failed" );
#endif
    }
    if ( FAILURE_REQUIRED == expect ) {
        CYG_TEST_CHECK( 0 == ok_recv, "Pings succeeded" );
        CYG_TEST_CHECK( bogus_recv + errsendtos + errrecvfroms == NUM_PINGS, "Not enough failures" );
#ifndef XFAIL
        CYG_TEST_CHECK( 0 == bogus_recv, "Some pings got bogus recv" );
#endif
    }
    CYG_TEST_PASS( SUCCESS_REQUIRED == expect ? "Expected successful ping behaviour" :
                   FAILURE_REQUIRED == expect ? "Expected failure ping behaviour" :
                                                "Non-predicted ping behaviour" );

    TNR_ON();
    return;
}

static void
ping_test(struct bootp *bp)
{
    struct protoent *p;
    struct timeval tv;
    struct sockaddr_in host;
    int s;

    if ((p = getprotobyname("icmp")) == (struct protoent *)0) {
        pexit("getprotobyname");
        return;
    }
    s = socket(AF_INET, SOCK_RAW, p->p_proto);
    if (s < 0) {
        pexit("socket");
        return;
    }
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    // Set up host address
    host.sin_family = AF_INET;
    host.sin_len = sizeof(host);
    host.sin_addr = bp->bp_siaddr;
    host.sin_port = 0;
    ping_host(s, &host, SUCCESS_REQUIRED );
    // Now try a bogus host
    host.sin_addr.s_addr = htonl(ntohl(host.sin_addr.s_addr) + 32);
    ping_host(s, &host, NOTHING_REQUIRED ); // Success is optional
    close(s);
}

static void
station_ping(char *name, int expect)
{
    struct protoent *p;
    struct timeval tv;
    struct sockaddr_in host;
    int s;

    int ints[4];

    if ((p = getprotobyname("icmp")) == (struct protoent *)0) {
        pexit("getprotobyname");
        return;
    }
    s = socket(AF_INET, SOCK_RAW, p->p_proto);
    if (s < 0) {
        pexit("socket");
        return;
    }
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
    // Set up host address
    host.sin_family = AF_INET;
    host.sin_len = sizeof(host);
    host.sin_port = 0;

    if ( 4 != sscanf( name, "%d.%d.%d.%d", ints, ints+1, ints+2, ints+3 ) )
        CYG_TEST_FAIL( "sscanf of host IP addr" );
    else {
        host.sin_addr.s_addr = htonl( 
            (ints[0] << 24) |
            (ints[1] << 16) |
            (ints[2] <<  8) |
            (ints[3]      )  );
            
        ping_host(s, &host, expect );

    }
    close(s);
}

// ------------------------------------------------------------------------

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
                 filesize + 100 * i );
        autohost_tell( bp, order );
    }
}

#define TESTTIME (5 * 60) // Seconds

#define NUM_SESSIONS 10 // why not?

// ------------------------------------------------------------------------
//
// The plan is as follows: 
//
// The host(s) will be set up so as to have additional interfaces as follows:
//
// eth0-host -> 10.0.3.1 and maybe others
// eth1-host -> 10.0.4.1 and maybe others
//              10.0.5.x should not exist
//
// These additional addresses can exist as extra loopback addresses, lo0:0,
// lo0:1 and so on in a linux box.
//
// We want to ping all of the following:
//
// eth0-host		should always work
// eth0-host+32		not expected to work
// eth1-host		should always work
// eth1-host+32		not expected to work
// www.cygnus.com	should always work if default gw is correct
// 10.0.3.1		should only work if route via eth0-host is added
// 10.0.3.99		might work if route via eth0-host is added
// 10.0.4.1		should only work if route via eth0-host is added
// 10.0.4.99		might work if route via eth0-host is added
// 10.0.5.1		should never work
// 10.0.5.99		should never work
//
// So we test those places in the following contexts:
// 
// 1) No special routing added
// 2) route via eth0-host to 10.0.3.x is added
// 3) route via eth1-host to 10.0.4.x is added
// 4) Both routes are added
//
// and check for correct operation and correct failure in all cases.
//
// Because of the time taken, these are separate test cases which include
// this INL file after making suitable #defines.
//
// All the while, the hosts shall ping us, and when all is going we loop for
// the rest of the test time.
//

void
net_test(cyg_addrword_t param)
{
#ifdef DOHOSTPINGS
    int i;
    int numtests;
#endif
    int net3expect = FAILURE_REQUIRED;
    int net4expect = FAILURE_REQUIRED;

    cyg_tick_count_t ticks;
    CYG_TEST_INIT();
    CYG_TEST_INFO("Start PING routing test");
    init_all_network_interfaces();

    autohost_init();

    TNR_INIT();

#ifdef ADDROUTETONET_10_0_3_x
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        route_add( "10.0.3.0", inet_ntoa(eth0_bootp_data.bp_siaddr), 24, 1 );
        net3expect = SUCCESS_REQUIRED;
    }
#endif
#endif // ADDROUTETONET_10_0_3_x

#ifdef ADDROUTETONET_10_0_4_x
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up) {
        route_add( "10.0.4.0", inet_ntoa(eth1_bootp_data.bp_siaddr), 24, 1 );
        net4expect = SUCCESS_REQUIRED;
    }
#endif
#endif // ADDROUTETONET_10_0_4_x

    numtests = NUM_SESSIONS; // The number of pingers started OK
    i = numtests;
#ifdef DOHOSTPINGS
    // Now command the host to do ping to us...
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        do_ping_tests(&eth0_bootp_data, 1, TESTTIME, 64);
        do_ping_tests(&eth0_bootp_data, 1, TESTTIME, 2000);
        i -= 2;
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up && i > 0) {
        do_ping_tests(&eth1_bootp_data, 1, TESTTIME, 100);
        do_ping_tests(&eth1_bootp_data, 1, TESTTIME, 1800);
        i -= 2;
    }
#endif
#endif // DOHOSTPINGS
    numtests -= i; // Adjust to how many we *actually* requested

    ticks = cyg_current_time() + TESTTIME * 100; // FIXME - assume cS clock.

    // Let the server run for 5 minutes
    TNR_ON();

    while (1) {
#ifdef CYGHWR_NET_DRIVER_ETH0
        if (eth0_up)
            ping_test(&eth0_bootp_data);
        if ( ticks < cyg_current_time() ) break;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
        if (eth1_up)
            ping_test(&eth1_bootp_data);
        if ( ticks < cyg_current_time() ) break;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH0
        if (eth0_up) {
            station_ping( "10.0.3.1",  net3expect );
            if ( ticks < cyg_current_time() ) break;
            station_ping( "10.0.3.99", net3expect );
        }
        if ( ticks < cyg_current_time() ) break;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
        if (eth1_up) {
            station_ping( "10.0.4.1",  net4expect );
            if ( ticks < cyg_current_time() ) break;
            station_ping( "10.0.4.99", net4expect );
        }
        if ( ticks < cyg_current_time() ) break;
#endif
        station_ping( "10.0.5.1", FAILURE_REQUIRED ); // Not valid
        if ( ticks < cyg_current_time() ) break;
        station_ping( "10.0.5.99", FAILURE_REQUIRED ); // Not valid
        if ( ticks < cyg_current_time() ) break;
#ifdef PING_WWW_CYGNUS_COM
        station_ping( "205.180.83.41", NOTHING_REQUIRED ); // www.cygnus.com
        if ( ticks < cyg_current_time() ) break;
#endif
    };

    // Additional delay 'cos host may be slower than us - and it has to
    // complete a transfer anyway:
    cyg_thread_delay(  30    *100); // FIXME - assume cS clock.
    TNR_OFF();

    autohost_end( 0 ); // check for N pass messages from hosts

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

// EOF routeping.inl
