//==========================================================================
//
//      autotest/current/tests/floodpingmux.c
//
//      Flood pinging test, the server floods me and I flood hir.
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

#define MAX_PACKET 4096

#define NUMTHREADS 3
#define STACK_SIZE (CYGNUM_HAL_STACK_SIZE_TYPICAL + MAX_PACKET + MAX_PACKET + 0x1000)
static char thread_stack[NUMTHREADS][STACK_SIZE];
static cyg_thread thread_data[NUMTHREADS];
static cyg_handle_t thread_handle[NUMTHREADS];

#define DO_DUMPSTATS( seq ) (0 == (0xffff & seq))

#ifdef CYGHWR_NET_DRIVER_ETH0
struct sockaddr_in host0;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
struct sockaddr_in host1;
#endif
static int sock;

static int uniqueid[3] = { 0x1234, 0x4321, 0xdead };

static int ok_recv[3] = { 0,0,0 };
static int bogus_recv[3] = { 0,0,0 };
static int pings_sent[3] = { 0,0,0 };

// ------------------------------------------------------------------------
static void dumpstats(void)
{
    TNR_OFF();
    diag_printf( "------------------------\n" );
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        diag_printf("%16s: Sent %d packets, received %d OK, %d bad\n",
                    inet_ntoa(host0.sin_addr), pings_sent[0],
                    ok_recv[0], bogus_recv[0]);
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up) {
        diag_printf("%16s: Sent %d packets, received %d OK, %d bad\n",
                    inet_ntoa(host1.sin_addr), pings_sent[1],
                    ok_recv[1], bogus_recv[1]);
    }
#endif
    if ( pings_sent[2] )
        diag_printf("Wierd!  %d unknown sends!\n", pings_sent[2] );
    if ( ok_recv[2] )
        diag_printf("Wierd!  %d unknown good recvs!\n", ok_recv[2] );
    if ( bogus_recv[2] )
        diag_printf("Wierd!  %d unknown bogus recvs!\n", bogus_recv[2] );
    diag_printf( "------------------------\n" );
    TNR_ON();
}


// ------------------------------------------------------------------------
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

// ------------------------------------------------------------------------
static void
show_icmp(unsigned char *pkt, int len, struct sockaddr_in *from)
{
    cyg_tick_count_t *tp, tv;
    struct ip *ip;
    struct icmp *icmp;
    int which = 2;
    tv = cyg_current_time();
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up && (from->sin_addr.s_addr == host0.sin_addr.s_addr) )
        which = 0;
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up && (from->sin_addr.s_addr == host1.sin_addr.s_addr) )
        which = 1;
#endif

    ip = (struct ip *)pkt;
    if ((len < sizeof(*ip)) || ip->ip_v != IPVERSION) {
        TNR_OFF();
        diag_printf("%s: Short packet or not IP! - Len: %d, Version: %d\n", 
                    inet_ntoa(from->sin_addr), len, ip->ip_v);
        TNR_ON();
        bogus_recv[which]++;
        return;
    }
    icmp = (struct icmp *)(pkt + sizeof(*ip));
    len -= (sizeof(*ip) + 8);
    tp = (cyg_tick_count_t *)&icmp->icmp_data;
    if (icmp->icmp_type != ICMP_ECHOREPLY) { 
        TNR_OFF();
        diag_printf("%s: Invalid ICMP - type: %d\n", 
                    inet_ntoa(from->sin_addr), icmp->icmp_type);
        TNR_ON();
        bogus_recv[which]++;
        return;
    }
    ok_recv[which]++;
    if (icmp->icmp_id != uniqueid[which]) {
        TNR_OFF();
        diag_printf("%s: ICMP received for wrong id - sent: %x, recvd: %x\n", 
                    inet_ntoa(from->sin_addr), uniqueid[which], icmp->icmp_id);
        TNR_ON();
    }
//    diag_printf("%d bytes from %s: ", len, inet_ntoa(from->sin_addr));
//    diag_printf("icmp_seq=%d", icmp->icmp_seq);
//    diag_printf(", time=%dms\n", (int)(tv - *tp)*10);
}

// ------------------------------------------------------------------------
static void
floodrecv(cyg_addrword_t p)
{
    unsigned char pkt[MAX_PACKET];
    struct sockaddr_in from;
    int len, fromlen;

    diag_printf("PING listener...\n" );
    for (;;) {
        // Wait for a response
        fromlen = sizeof(from);
        len = recvfrom(sock, pkt, sizeof(pkt), 0,
                       (struct sockaddr *)&from, &fromlen);
        if (len < 0)
            perror("recvfrom");
        else
            show_icmp(pkt, len, &from);
    }
}

// ------------------------------------------------------------------------
static void
pingsend( int seq, struct sockaddr_in *host,
          struct icmp *icmp, int icmp_len, int which )
{
    cyg_tick_count_t *tp;
    long *dp;
    int i;
    // Build ICMP packet for interface
    icmp->icmp_type = ICMP_ECHO;
    icmp->icmp_code = 0;
    icmp->icmp_cksum = 0;
    icmp->icmp_seq = seq;
    icmp->icmp_id = uniqueid[which];
    // Set up ping data
    tp = (cyg_tick_count_t *)&icmp->icmp_data;
    *tp++ = cyg_current_time();
    dp = (long *)tp;
    for (i = sizeof(*tp);  i < icmp_len;  i += sizeof(*dp))
        *dp++ = i;

    // Add checksum
    icmp->icmp_cksum = inet_cksum( (u_short *)icmp, icmp_len+8);
    // Send it off
    if (sendto(sock, icmp, icmp_len+8, MSG_DONTWAIT,
              (struct sockaddr *)host, sizeof(*host)) < 0) {
        perror("sendto");
    }
    pings_sent[which]++;
}

// ------------------------------------------------------------------------
static void
floodsend(cyg_addrword_t param)
{
    unsigned char pkt0[MAX_PACKET], pkt1[MAX_PACKET];

    struct icmp *icmp0 = (struct icmp *)pkt0;
    struct icmp *icmp1 = (struct icmp *)pkt1;

    int icmp_len = 64;
    int seq;

    for (seq = 0; 1 ; seq++) {
        if ( DO_DUMPSTATS( seq ) )
            dumpstats();

#ifdef CYGHWR_NET_DRIVER_ETH0
        if (eth0_up)
            pingsend( seq, &host0, icmp0, icmp_len, 0 );
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
        if (eth1_up)
            pingsend( seq, &host1, icmp1, icmp_len, 1 );
#endif
    }
}


// ------------------------------------------------------------------------

#include "autohost.inl"

static void
do_ping_tests(struct bootp *bp, int testtime, int filesize)
{
    char order[256];
    diag_printf( "INFO: telling %s to run 1 flood ping for %d seconds, %d bytes\n",
          inet_ntoa(bp->bp_siaddr), testtime, filesize );

    sprintf( order, "%s %s %d %d", "FLOOD_PING",
             inet_ntoa(bp->bp_yiaddr),
             testtime,
             filesize );
    autohost_tell( bp, order );
}

// ------------------------------------------------------------------------

#define TESTTIME (5 * 60) // Seconds

void
net_test(cyg_addrword_t param)
{
    struct protoent *p;

    diag_printf("Start Flood PING test\n");
    init_all_network_interfaces();
    diag_printf("Interfaces up:\n");

    if ((p = getprotobyname("icmp")) == (struct protoent *)0) {
        perror("getprotobyname");
        return;
    }
    sock = socket(AF_INET, SOCK_RAW, p->p_proto);
    if (sock < 0) {
        perror("tx socket");
        return;
    }

#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        host0.sin_family = AF_INET;
        host0.sin_len = sizeof(host0);
        host0.sin_addr = eth0_bootp_data.bp_siaddr;
        host0.sin_port = 0;
        diag_printf("PING server %16s\n", inet_ntoa(host0.sin_addr));
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up) {
        host1.sin_family = AF_INET;
        host1.sin_len = sizeof(host1);
        host1.sin_addr = eth1_bootp_data.bp_siaddr;
        host1.sin_port = 0;
        diag_printf("PING server %16s\n", inet_ntoa(host1.sin_addr));
    }
#endif
    autohost_init();

    TNR_INIT();
    
    // Now command the host to do ping to us...
#ifdef CYGHWR_NET_DRIVER_ETH0
    if (eth0_up) {
        do_ping_tests(&eth0_bootp_data, TESTTIME, 56);
    }
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
    if (eth1_up) {
        do_ping_tests(&eth1_bootp_data, TESTTIME, 56);
    }
#endif

    // And start the threads that make us ping them, and count the results
    cyg_thread_resume(thread_handle[1]);
    cyg_thread_resume(thread_handle[2]);

    // Let the server run for 5 minutes
    cyg_thread_delay(2*100); // let the stuff start up first
    TNR_ON();
    cyg_thread_delay(TESTTIME*100); // FIXME - assume cS clock.

    // Stop the threads that make us ping them.  Thread [2] is the pinger,
    // thread [1] the receiver so that might as well continue.
    cyg_thread_suspend(thread_handle[2]);

    // Additional delay 'cos host may be slower than us - and it has to
    // complete a transfer anyway:
    cyg_thread_delay(  30    *100); // FIXME - assume cS clock.
    TNR_OFF();

    autohost_end( 0
#ifdef CYGHWR_NET_DRIVER_ETH0
                  + eth0_up
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
                  + eth1_up
#endif
        ); // check for 2 pass messages from hosts

    diag_printf("After running:\n");
    dumpstats();
    TNR_OFF(); // dumpstats() turned it on again!
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
                     &thread_stack[0][0], // Stack
                      STACK_SIZE,        // Size
                      &thread_handle[0], // Handle
                      &thread_data[0]    // Thread data structure
            );
    cyg_thread_resume(thread_handle[0]);  // Start it

    // Create the secondary threads
    cyg_thread_create(11,                // Priority - just a number
                      floodrecv,         // entry
                      0,                 // entry parameter
                      "Flood Ping Recv", // Name
                     &thread_stack[1][0], // Stack
                      STACK_SIZE,        // Size
                      &thread_handle[1], // Handle
                      &thread_data[1]    // Thread data structure
            );
    cyg_thread_create(12,                // Priority - just a number
                      floodsend,         // entry
                      0,                 // entry parameter
                      "Flood Ping Send", // Name
                     &thread_stack[2][0], // Stack
                      STACK_SIZE,        // Size
                      &thread_handle[2], // Handle
                      &thread_data[2]    // Thread data structure
            );

    cyg_scheduler_start();
}

// EOF floodpingmux.c
