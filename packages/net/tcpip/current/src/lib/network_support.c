//==========================================================================
//
//      lib/net_support.c
//
//      Misc network support functions
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// BOOTP support

#include <pkgconf/net.h>
#undef _KERNEL
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/errno.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <net/route.h>

#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>

#include <bootp.h>
#include <network.h>
#include <arpa/inet.h>

#ifdef CYGHWR_NET_DRIVER_ETH0
struct bootp eth0_bootp_data;
cyg_bool_t   eth0_up = false;
const char  *eth0_name = "eth0";
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
struct bootp eth1_bootp_data;
cyg_bool_t   eth1_up = false;
const char  *eth1_name = "eth1";
#endif

#define _string(s) #s
#define string(s) _string(s)

#if defined(CYGHWR_NET_DRIVER_ETH0_ADDRS_IP) \
 || defined(CYGHWR_NET_DRIVER_ETH1_ADDRS_IP)
//
// Internal function which builds up a fake BOOTP database for
// an interface.
//

static unsigned char *
add_tag(unsigned char *vp,
        unsigned char tag,
        void *val,
        int len)
{
    int i;
    unsigned char *xp = (unsigned char *)val;
    *vp++ = tag;
    *vp++ = len;
    for (i = 0;  i < len;  i++) {
        *vp++ = *xp++;
    }
    return vp;
}

static void
build_bootp_record(struct bootp *bp,
                   const char *addrs_ip,
                   const char *addrs_netmask,
                   const char *addrs_broadcast,
                   const char *addrs_gateway,
                   const char *addrs_server)
{
    int i;
    in_addr_t addr;
    unsigned char *vp;
    unsigned char cookie[] = VM_RFC1048;

    bzero(bp, sizeof(struct bootp));
    bp->bp_op = BOOTREPLY;
    bp->bp_htype = HTYPE_ETHERNET;
    bp->bp_hlen = 6;
    for (i = 0;  i < bp->bp_hlen;  i++) {
        bp->bp_chaddr[i] = 0xFF;
    }
    bp->bp_ciaddr.s_addr = inet_addr(addrs_ip);
    bp->bp_yiaddr.s_addr = inet_addr(addrs_ip);
    bp->bp_siaddr.s_addr = inet_addr(addrs_server);
    bp->bp_giaddr.s_addr = inet_addr(addrs_gateway);
    vp = &bp->bp_vend[0];
    bcopy(&cookie, vp, sizeof(cookie));
    vp += sizeof(cookie);
    addr = inet_addr(addrs_netmask);
    vp = add_tag(vp, TAG_SUBNET_MASK, &addr, sizeof(in_addr_t));
    addr = inet_addr(addrs_broadcast);
    vp = add_tag(vp, TAG_IP_BROADCAST, &addr, sizeof(in_addr_t));
    *vp = TAG_END;
}
#endif

//
// Initialize network interface[s] using BOOTP/DHCP
//
void
init_all_network_interfaces(void)
{
#ifdef CYGHWR_NET_DRIVER_ETH0
    if ( ! eth0_up ) { // Make this call idempotent
#ifdef CYGHWR_NET_DRIVER_ETH0_BOOTP
        // Perform a complete initialization, using BOOTP/DHCP
        eth0_up = true;
        if (do_bootp(eth0_name, &eth0_bootp_data)) {
#ifdef CYGHWR_NET_DRIVER_ETH0_BOOTP_SHOW
            show_bootp(eth0_name, &eth0_bootp_data);
#endif
        } else {
            diag_printf("BOOTP failed on eth0\n");
            eth0_up = false;
        }
#elif defined(CYGHWR_NET_DRIVER_ETH0_ADDRS_IP)
        eth0_up = true;
        build_bootp_record(&eth0_bootp_data,
                       string(CYGHWR_NET_DRIVER_ETH0_ADDRS_IP),
                       string(CYGHWR_NET_DRIVER_ETH0_ADDRS_NETMASK),
                       string(CYGHWR_NET_DRIVER_ETH0_ADDRS_BROADCAST),
                       string(CYGHWR_NET_DRIVER_ETH0_ADDRS_GATEWAY),
                       string(CYGHWR_NET_DRIVER_ETH0_ADDRS_SERVER));
        show_bootp(eth0_name, &eth0_bootp_data);
#endif
    }
#endif // CYGHWR_NET_DRIVER_ETH0
#ifdef CYGHWR_NET_DRIVER_ETH1
    if ( ! eth1_up ) { // Make this call idempotent
#ifdef CYGHWR_NET_DRIVER_ETH1_BOOTP
        // Perform a complete initialization, using BOOTP/DHCP
        eth1_up = true;
        if (do_bootp(eth1_name, &eth1_bootp_data)) {
#ifdef CYGHWR_NET_DRIVER_ETH1_BOOTP_SHOW
            show_bootp(eth1_name, &eth1_bootp_data);
#endif
        } else {
            diag_printf("BOOTP failed on eth1\n");
            eth1_up = false;
        }
#elif defined(CYGHWR_NET_DRIVER_ETH1_ADDRS_IP)
        eth1_up = true;
        build_bootp_record(&eth1_bootp_data,
                       string(CYGHWR_NET_DRIVER_ETH1_ADDRS_IP),
                       string(CYGHWR_NET_DRIVER_ETH1_ADDRS_NETMASK),
                       string(CYGHWR_NET_DRIVER_ETH1_ADDRS_BROADCAST),
                       string(CYGHWR_NET_DRIVER_ETH1_ADDRS_GATEWAY),
                       string(CYGHWR_NET_DRIVER_ETH1_ADDRS_SERVER));
        show_bootp(eth1_name, &eth1_bootp_data);
#endif
    }
#endif // CYGHWR_NET_DRIVER_ETH1
#ifdef CYGHWR_NET_DRIVER_ETH0
#ifndef CYGHWR_NET_DRIVER_ETH0_MANUAL
    if (eth0_up) {
        if (!init_net(eth0_name, &eth0_bootp_data)) {
            diag_printf("Network initialization failed for eth0\n");
            eth0_up = false;
        }
    }
#endif
#endif
#ifdef CYGHWR_NET_DRIVER_ETH1
#ifndef CYGHWR_NET_DRIVER_ETH1_MANUAL
    if (eth1_up) {
        if (!init_net(eth1_name, &eth1_bootp_data)) {
            diag_printf("Network initialization failed for eth1\n");
            eth1_up = false;
        }
    }
#endif
#endif
}
