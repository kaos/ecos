//==========================================================================
//
//      ping.c
//
//      Network utility - ping
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-01-22
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <redboot.h>
#include <net/net.h>

#ifndef CYGPKG_REDBOOT_NETWORKING
#error CYGPKG_REDBOOT_NETWORKING required!
#else

static void do_ping(int argc, char *argv[]);
RedBoot_cmd("ping", 
            "Network connectivity test",
            "[-v] [-n <count>] [-t <timeout>] [-i <IP_addr>] -h <IP_addr>",
            do_ping
    );

static bool icmp_received;
static icmp_header_t hold_hdr;

static void
handle_icmp(pktbuf_t *pkt, ip_route_t *src_route)
{
    icmp_header_t *icmp;

    icmp = pkt->icmp_hdr;
    memcpy(&hold_hdr, icmp, sizeof(*icmp));
    icmp_received = true;
}

static void
do_ping(int argc, char *argv[])
{
    struct option_info opts[7];
    long count, timeout, length, rate, start_time, end_time, timer, received, tries;
    char *local_ip_addr, *host_ip_addr;
    bool local_ip_addr_set, host_ip_addr_set, count_set, 
        timeout_set, length_set, rate_set, verbose;
    struct sockaddr_in local_addr, host_addr;
    ip_addr_t hold_addr;
    icmp_header_t *icmp;
    pktbuf_t *pkt;
    ip_header_t *ip;
    unsigned short cksum;
    ip_route_t dest_ip;

    init_opts(&opts[0], 'n', true, OPTION_ARG_TYPE_NUM, 
              (void **)&count, (bool *)&count_set, "<count> - number of packets to test");
    init_opts(&opts[1], 't', true, OPTION_ARG_TYPE_NUM, 
              (void **)&timeout, (bool *)&timeout_set, "<timeout> - max #ms per packet [rount trip]");
    init_opts(&opts[2], 'i', true, OPTION_ARG_TYPE_STR, 
              (void **)&local_ip_addr, (bool *)&local_ip_addr_set, "local IP address");
    init_opts(&opts[3], 'h', true, OPTION_ARG_TYPE_STR, 
              (void **)&host_ip_addr, (bool *)&host_ip_addr_set, "host IP address");
    init_opts(&opts[4], 'l', true, OPTION_ARG_TYPE_NUM, 
              (void **)&length, (bool *)&length_set, "<length> - size of payload");
    init_opts(&opts[5], 'v', false, OPTION_ARG_TYPE_FLG, 
              (void **)&verbose, (bool *)0, "verbose operation");
    init_opts(&opts[6], 'r', true, OPTION_ARG_TYPE_NUM, 
              (void **)&rate, (bool *)&rate_set, "<rate> - time between packets");
    if (!scan_opts(argc, argv, 1, opts, 7, (void **)0, 0, "")) {
        printf("PING - Invalid option specified\n");
        return;
    }   
    // Set defaults; this has to be done _after_ the scan, since it will
    // have destroyed all values not explicitly set.
    if (local_ip_addr_set) {
        if (!inet_aton(local_ip_addr, (in_addr_t *)&local_addr)) {
            printf("PING - Invalid IP address: %s\n", local_ip_addr);
            return;
        }
    } else {
        memcpy((in_addr_t *)&local_addr, __local_ip_addr, sizeof(__local_ip_addr));
    }
    if (host_ip_addr_set) {
        if (!inet_aton(host_ip_addr, (in_addr_t *)&host_addr)) {
            printf("PING - Invalid IP address: %s\n", host_ip_addr);
            return;
        }
        if (__arp_lookup((ip_addr_t *)&host_addr.sin_addr, &dest_ip) < 0) {
            printf("PING: Can't find address of server '%s'\n", host_ip_addr);
            return;
        }
    } else {
        printf("PING - host IP address required\n");
        return;
    }
#define DEFAULT_LENGTH   64
#define DEFAULT_COUNT    10
#define DEFAULT_TIMEOUT  1000
#define DEFAULT_RATE     1000
    if (!rate_set) {
        rate = DEFAULT_RATE;
    }
    if (!length_set) {
        length = DEFAULT_LENGTH;
    }
    if (!count_set) {
        count = DEFAULT_COUNT;
    }
    if (!timeout_set) {
        timeout = DEFAULT_TIMEOUT;
    }
    // Note: two prints here because 'inet_ntoa' returns a static pointer
    printf("Network PING - from %s",
           inet_ntoa((in_addr_t *)&local_addr));
    printf(" to %s\n",
           inet_ntoa((in_addr_t *)&host_addr));
    received = 0;    
    __icmp_install_listener(handle_icmp);
    for (tries = 0;  tries < count;  tries++) {
        // The network stack uses the global variable '__local_ip_addr'
        memcpy(hold_addr, __local_ip_addr, sizeof(hold_addr));
        memcpy(__local_ip_addr, &local_addr, sizeof(__local_ip_addr));
        // Build 'ping' request
        if ((pkt = __pktbuf_alloc(ETH_MAX_PKTLEN)) == NULL) {
            // Give up if no packets - something is wrong
            break;
        }

        icmp = pkt->icmp_hdr;
        ip = pkt->ip_hdr;
        pkt->pkt_bytes = length + sizeof(icmp_header_t);

	icmp->type = ICMP_TYPE_ECHOREQUEST;
        icmp->code = 0;
	icmp->checksum = 0;
        icmp->seqnum = htons(tries+1);
        cksum = __sum((word *)icmp, pkt->pkt_bytes, 0);
	icmp->checksum = htons(cksum);
	
        memcpy(ip->source, (in_addr_t *)&local_addr, sizeof(ip_addr_t));
        memcpy(ip->destination, (in_addr_t *)&host_addr, sizeof(ip_addr_t));
        ip->protocol = IP_PROTO_ICMP;
        ip->length = htons(pkt->pkt_bytes);

        __ip_send(pkt, IP_PROTO_ICMP, &dest_ip);
        __pktbuf_free(pkt);

        start_time = MS_TICKS();
        timer = start_time + timeout;
        icmp_received = false;
        while (!icmp_received && (MS_TICKS_DELAY() < timer)) {
            __enet_poll();
        } 
        end_time = MS_TICKS();

        timer = MS_TICKS() + rate;
        while (MS_TICKS_DELAY() < timer) {
            __enet_poll();
        } 

        // Clean up
        memcpy(__local_ip_addr, &hold_addr, sizeof(__local_ip_addr));

        if (icmp_received) {
            received++;
            if (verbose) {
                printf("icmp - seq: %ld, time: %ld (ticks)\n",
                       ntohs(hold_hdr.seqnum), end_time-start_time);
            }
        }
    }
    __icmp_remove_listener();
    // Report
    printf("PING - received %ld of %ld expected\n", received, count);
}

#endif //CYGPKG_REDBOOT_NETWORKING
