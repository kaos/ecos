//==========================================================================
//
//      net/bootp.c
//
//      Stand-alone minimal BOOTP support for RedBoot
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
// Date:         2000-07-14
// Purpose:      
// Description:  
//              
// This code is part of RedBoot (tm).
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <net/net.h>
#include <net/bootp.h>

extern int net_debug;

#define SHOULD_BE_RANDOM  0x12345555

/* How many milliseconds to wait before retrying the request */
#define RETRY_TIME  2000

static bootp_header_t *bp_info;

static void
bootp_handler(udp_socket_t *skt, char *buf, int len,
	      ip_route_t *src_route, word src_port)
{
    bootp_header_t *b;

    b = (bootp_header_t *)buf;
    if (bp_info) {
        if (len > sizeof *bp_info)
            len = sizeof *bp_info;
        memcpy(bp_info, b, len);
    }

    if (b->bp_op == BOOTREPLY && 
	!memcmp(b->bp_chaddr, __local_enet_addr, 6)) {
	memcpy(__local_ip_addr, &b->bp_yiaddr, 4);
    }
}


/*
 * Find our IP address and copy to __local_ip_addr.
 * Return zero if successful, -1 if not.
 */
int
__bootp_find_local_ip(bootp_header_t *info)
{
    udp_socket_t udp_skt;
    bootp_header_t b;
    ip_route_t     r;
    int            retry;
    unsigned long  start;

    bp_info = info;

    memset(&b, 0, sizeof(b));

    b.bp_op = BOOTREQUEST;
    b.bp_htype = HTYPE_ETHERNET;
    b.bp_hlen = 6;
    b.bp_xid = SHOULD_BE_RANDOM;

    __local_ip_addr[0] = 0;
    __local_ip_addr[1] = 0;
    __local_ip_addr[2] = 0;
    __local_ip_addr[3] = 0;

    memcpy(b.bp_chaddr, __local_enet_addr, 6);

    /* fill out route for a broadcast */
    r.ip_addr[0] = 255;
    r.ip_addr[1] = 255;
    r.ip_addr[2] = 255;
    r.ip_addr[3] = 255;
    r.enet_addr[0] = 255;
    r.enet_addr[1] = 255;
    r.enet_addr[2] = 255;
    r.enet_addr[3] = 255;
    r.enet_addr[4] = 255;
    r.enet_addr[5] = 255;

    /* setup a socket listener for bootp replies */
    __udp_install_listener(&udp_skt, IPPORT_BOOTPC, bootp_handler);

    retry = 3;
    while (retry-- > 0) {
	start = MS_TICKS();

	__udp_send((char *)&b, sizeof(b), &r, IPPORT_BOOTPS, IPPORT_BOOTPC);

	do {
	    __enet_poll();
	    if (__local_ip_addr[0] || __local_ip_addr[1] ||
		__local_ip_addr[2] || __local_ip_addr[3]) {
		/* success */
		__udp_remove_listener(IPPORT_BOOTPC);
		return 0;
	    }
	} while ((MS_TICKS_DELAY() - start) < RETRY_TIME);
    }

    /* timed out */
    __udp_remove_listener(IPPORT_BOOTPC);
    net_debug = 0;
    return -1;
}


