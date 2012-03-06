//==========================================================================
//
//      net/bootp.c
//
//      Stand-alone minimal BOOTP support for RedBoot
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2003 Free Software Foundation, Inc.
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
//               grant edwards
// Contributors: gthomas
//               grant edwards
// Date:         2011-03-18
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
#include <net/bootp.h>

#define RETRY_TIME_MS  2000
#define MAX_RETRIES    4

static unsigned xid;                   // transaction ID,  should be random/unique
static const ip_route_t broadcast = { {255, 255, 255, 255},
                                      {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };
static bootp_header_t *bp_info;

#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP
static const unsigned char dhcpCookie[] =           { 99, 130, 83, 99 };
static const unsigned char dhcpDiscover[] =         { 53, 1, 1 };
static const unsigned char dhcpRequest[] =          { 53, 1, 3 };
static const unsigned char dhcpRequestIP[] =        { 50, 4 };
static const unsigned char dhcpParamRequestList[] = { 55, 3, 1, 3, 6 };
static const unsigned char dhcpEnd[] =              { 255 };
#endif

// FSM states -- only NONE and DONE are used in BOOTP mode
static enum {
    DHCP_NONE = 0,
    DHCP_WAITING_FOR_OFFER,
    DHCP_WAITING_FOR_ACK,
    DHCP_DONE,
    DHCP_FAILED
} dhcpState;

#if !defined(CYGSEM_REDBOOT_NETWORKING_BOOTP_VERBOSE)
# define debug_printf(format, ...)     /* noop */
#else
# define debug_printf(format, ...)  diag_printf(format, ##__VA_ARGS__)
static const char *dhcpStateString[] =  { "NONE", "WAITING_FOR_OFFER", "WAITING_FOR_ACK", "DONE", "FAILED" };
# ifdef CYGSEM_REDBOOT_NETWORKING_DHCP
static const char *dhcpTypeString[] =  { "0x00", "DISCOVER", "OFFER", "REQUEST", "0x04", "ACK", "NAK", "0x07" };
# endif
#endif

// parse network configuration from a DHCP ACK packet or BOOTP REPLY
static void
parseConfig(bootp_header_t *bp, int len)
{
#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP
    unsigned char  *end,
                   *p;
    int             optlen;
#endif
    memcpy(__local_ip_addr, &bp->bp_yiaddr, sizeof __local_ip_addr);
#ifdef CYGSEM_REDBOOT_NETWORKING_USE_GATEWAY
    memcpy(__local_ip_gate, &bp->bp_giaddr, sizeof __local_ip_gate);
#endif

#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP

    // are there option fields to parse?
    if (memcmp(bp->bp_vend, dhcpCookie, sizeof dhcpCookie))
        return;

    p = bp->bp_vend + 4;
    end = (unsigned char *)bp + len;
    while (p < end) {
        unsigned char tag = *p;
        if (tag == TAG_END)
            break;
        if (tag == TAG_PAD)
            optlen = 1;
        else {
            optlen = p[1];
            p += 2;
            switch (tag) {
# ifdef CYGSEM_REDBOOT_NETWORKING_USE_GATEWAY
            case TAG_SUBNET_MASK:
                memcpy(__local_ip_mask, p, sizeof __local_ip_mask);
                break;
            case TAG_GATEWAY:
                memcpy(__local_ip_gate, p, sizeof __local_ip_gate);
                break;
# endif
# ifdef CYGPKG_REDBOOT_NETWORKING_DNS
            case TAG_DOMAIN_SERVER:
                memcpy(&__bootp_dns_addr, p, sizeof __bootp_dns_addr);
                __bootp_dns_set = 1;
                break;
# endif
            default:
                break;
            }
        }
        p += optlen;
    }
#endif
}

// functions used to prepare BOOTP/DHCP tx packets

// basic BOOTP request
static void
prep_bootp_request(bootp_header_t *b)
{
    memset(b, 0, sizeof *b);
    b->bp_op = BOOTREQUEST;
    b->bp_htype = HTYPE_ETHERNET;
    b->bp_hlen = sizeof __local_enet_addr;
    b->bp_xid = xid;
    memcpy(b->bp_chaddr, __local_enet_addr, sizeof b->bp_chaddr);
}

#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP

# define AddOption(p,d) do {memcpy(p,d,sizeof d); p += sizeof d;} while (0)

// add DHCP DISCOVER fields to a basic BOOTP request
static int
prep_dhcp_discover(bootp_header_t *b)
{
    unsigned char  *p = b->bp_vend;
    AddOption(p, dhcpCookie);
    AddOption(p, dhcpDiscover);
    AddOption(p, dhcpParamRequestList);
    AddOption(p, dhcpEnd);
    if (p < &b->bp_vend[BP_MIN_VEND_SIZE])
        p = &b->bp_vend[BP_MIN_VEND_SIZE];
    return p - (unsigned char *)b;
}

// add DHCP REQUEST fields to a basic BOOTP request using data from supplied DHCP OFFER
static int
prep_dhcp_request(bootp_header_t *b, bootp_header_t *offer)
{
    unsigned char  *p = b->bp_vend;
    AddOption(p, dhcpCookie);
    AddOption(p, dhcpRequest);
    AddOption(p, dhcpRequestIP);
    memcpy(p, &offer->bp_yiaddr, dhcpRequestIP[1]);
    p += dhcpRequestIP[1];                            // Ask for the address just given
    AddOption(p, dhcpParamRequestList);
    AddOption(p, dhcpEnd);
    if (p < &b->bp_vend[BP_MIN_VEND_SIZE])
        p = &b->bp_vend[BP_MIN_VEND_SIZE];
    return p - (unsigned char *)b;
}
#endif

// Macro used to change state of BOOTP/DHCP state machine
#define NewDhcpState(state)  do {dhcpState = state; debug_printf("DHCP state: %s\n",dhcpStateString[state]);}while(0)

// send BOOTP REQUEST or DHCP DISCOVER
static void
bootp_start(void)
{
    int             txSize;
    bootp_header_t  b;
    // send out a BOOTP request or DHCP DISCOVER
    prep_bootp_request(&b);            // basic BOOTP request
#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP
    debug_printf("DHCP  send: DISCOVER\n");
    NewDhcpState(DHCP_WAITING_FOR_OFFER);
    txSize = prep_dhcp_discover(&b);   // make it into DHCP DISCOVER
#else
    debug_printf("BOOTP send: REQUEST\n");
    txSize = sizeof(b);
#endif
    __udp_send((char *)&b, txSize, (ip_route_t*)&broadcast, IPPORT_BOOTPS, IPPORT_BOOTPC);
}

// save packet for use by other commands
static void
save_packet(void *b, int len)
{
    memset(bp_info, 0, sizeof *bp_info);
    if ((unsigned)len > sizeof *bp_info)
        len = sizeof *bp_info;
    memcpy(bp_info, b, len);
}

// Handler for received BOOTP/DHCP packets
static void
bootp_handler(udp_socket_t *skt, char *buf, int len, ip_route_t *src_route,
              word src_port)
{
    bootp_header_t *b;
#ifdef CYGSEM_REDBOOT_NETWORKING_DHCP
    int             txSize;
    int             type;
    bootp_header_t  txpkt;
    unsigned        expected = 0;
#endif

    b = (bootp_header_t *) buf;

    // only accept BOOTP REPLY responses
    if (b->bp_op != BOOTREPLY)
        return;

    // must be sent to me
    if (memcmp(b->bp_chaddr, __local_enet_addr, b->bp_hlen))
        return;

    // verify XID
    if (b->bp_xid != xid)
        return;

#if !defined(CYGSEM_REDBOOT_NETWORKING_DHCP)
    // simple BOOTP - this is all there is!
    debug_printf("BOOTP recv: REPLY\n");
    if (dhcpState != DHCP_DONE) {
        save_packet(b, len);
        parseConfig(b, len);
        NewDhcpState(DHCP_DONE);
    }
#else
    // DHCP support is enabled...

    // Check to see that it's a DHCP packet with a DHCP type field

    type = -1;
    if (!memcmp(b->bp_vend, dhcpCookie, sizeof dhcpCookie)) {
        unsigned char *p = b->bp_vend + 4;
        while (p < (unsigned char *)b + len) {
            if (*p == TAG_DHCP_MESS_TYPE) {
                type = p[2];
                break;
            }
            p += p[1] + 2;
        }
    }

    if (type == -1) {
        // apparently we have a BOOTP (but not not DHCP) server
        debug_printf("DHCP  recv: BOOTP-REPLY -- falling back to BOOTP mode\n");
        if (dhcpState != DHCP_DONE) {
            save_packet(b, len);
            parseConfig(b, len);
            NewDhcpState(DHCP_DONE);
        }
        return;
    }
        
    // it's a real DHCP packet
    debug_printf("DHCP  recv: %s [%d]\n", dhcpTypeString[type], type);

    switch (dhcpState) {
    case DHCP_WAITING_FOR_OFFER:
        if (type == (expected = DHCP_MESS_TYPE_OFFER)) {
            prep_bootp_request(&txpkt);
            txSize = prep_dhcp_request(&txpkt, b);
            debug_printf("DHCP  send: REQUEST\n");
            NewDhcpState(DHCP_WAITING_FOR_ACK);
            __udp_send((char *)&txpkt, txSize, (ip_route_t *)&broadcast, IPPORT_BOOTPS, IPPORT_BOOTPC);
            return;
        }
        break;

    case DHCP_WAITING_FOR_ACK:
        if (type == (expected = DHCP_MESS_TYPE_ACK)) {
            save_packet(b, len);
            parseConfig(b, len);
            NewDhcpState(DHCP_DONE);
            return;
        }
        break;

    default:
        debug_printf("DHCP packet ignored\n");
        return;
    }

    if (type == DHCP_MESS_TYPE_NAK && dhcpState != DHCP_DONE) {
        NewDhcpState(DHCP_FAILED);
        return;
    }

    debug_printf("DHCP packet ignored -- expected %d[%s]\n", expected, dhcpTypeString[expected]);
#endif
}


// Request IP configuration via BOOTP/DHCP.
// Return zero if successful, -1 if not.

int
__bootp_find_local_ip(bootp_header_t *info)
{
    udp_socket_t    udp_skt;
    int             retry;
    unsigned long   start;
    ip_addr_t       saved_ip_addr;

    bp_info = info;

    diag_printf("\nRequesting IP conf via BOOTP/DHCP...\n");

    memcpy(&xid, __local_enet_addr, sizeof xid);
    xid ^= (__local_enet_addr[4]<<16) + __local_enet_addr[5];
    xid ^= (unsigned)&retry + (unsigned)&__bootp_find_local_ip;

    debug_printf("XID: %08x\n",xid);

    memcpy(saved_ip_addr, __local_ip_addr, sizeof __local_ip_addr);    // save our IP in case of failure

    NewDhcpState(DHCP_NONE);

    __udp_install_listener(&udp_skt, IPPORT_BOOTPC, bootp_handler);

    retry = MAX_RETRIES;

    while (retry > 0) {
        start = MS_TICKS();
        memset(__local_ip_addr, 0, sizeof(__local_ip_addr));

        // send bootp REQUEST or dhcp DISCOVER
        bootp_start();

        // wait for timeout, user-abort, or for receive packet handler to fail/succeed
        while ((MS_TICKS_DELAY() - start) < RETRY_TIME_MS) {
            __enet_poll();
            if (dhcpState == DHCP_FAILED)
                break;
            if (dhcpState == DHCP_DONE)
                goto done;
            if (_rb_break(1))          // did user hit ^C?
                goto failed;
            MS_TICKS_DELAY();
        }
        --retry;
        ++xid;
        diag_printf("TIMEOUT%s\n", retry ? ", retrying..." : "");
    }

failed:
    diag_printf("FAIL\n");
    __udp_remove_listener(IPPORT_BOOTPC);
    memcpy(__local_ip_addr, saved_ip_addr, sizeof __local_ip_addr);    // restore prev IP
    return -1;

done:
    diag_printf("OK\n");
    __udp_remove_listener(IPPORT_BOOTPC);
    return 0;
}
