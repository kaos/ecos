//==========================================================================
//
//      net/enet.c
//
//      Stand-alone ethernet [link-layer] support for RedBoot
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

#include <redboot.h>
#include <net/net.h>
#include <eth_drv.h>       // Logical driver interfaces

#define ENET_STATS 1

#ifdef ENET_STATS
static int num_ip = 0;
static int num_arp = 0;
#ifdef NET_SUPPORT_RARP
static int num_rarp = 0;
#endif
static int num_received = 0;
static int num_transmitted = 0;
#endif

/*
 * Non-blocking poll of ethernet link. Process packets until no more
 * are available.
 */
void
__enet_poll(void)
{
    pktbuf_t *pkt;
    eth_header_t eth_hdr;

    while (true) {
        /*
         * Try to get a free pktbuf and return if none
         * are available.
         */
        if ((pkt = __pktbuf_alloc(ETH_MAX_PKTLEN)) == NULL) {
            printf("__enet_poll: no more buffers\n");
            __pktbuf_dump();
            while (1) ;
            return;
        }

        if ((pkt->pkt_bytes = eth_drv_read((char *)&eth_hdr, (char *)pkt->buf,
                                           ETH_MAX_PKTLEN)) > 0) {
#if ENET_STATS
            ++num_received;
#endif
            switch (ntohs(eth_hdr.type)) {

            case ETH_TYPE_IP:
#if ENET_STATS
                ++num_ip;
#endif
                pkt->ip_hdr = (ip_header_t *)pkt->buf;
                __ip_handler(pkt, &eth_hdr.source);
                break;

            case ETH_TYPE_ARP:
#if ENET_STATS
                ++num_arp;
#endif
                pkt->arp_hdr = (arp_header_t *)pkt->buf;
                __arp_handler(pkt);
                break;

#ifdef NET_SUPPORT_RARP
            case ETH_TYPE_RARP:
#if ENET_STATS
                ++num_rarp;
#endif
                pkt->arp_hdr = (arp_header_t *)pkt->buf;
                __rarp_handler(pkt);
                break;
#endif

            default:
                __pktbuf_free(pkt);
                break;
            }
        } else {
            __pktbuf_free(pkt);
            break;
        } 
    }
}



/*
 * Send an ethernet packet.
 */
void
__enet_send(pktbuf_t *pkt, enet_addr_t *dest, int eth_type)
{
    eth_header_t eth_hdr;

    // Set up ethernet header
    memcpy(&eth_hdr.destination, dest, sizeof(enet_addr_t));
    memcpy(&eth_hdr.source, __local_enet_addr, sizeof(enet_addr_t));
    eth_hdr.type = htons(eth_type);

    eth_drv_write((char *)&eth_hdr, (char *)pkt->buf, pkt->pkt_bytes);
#if ENET_STATS
    ++num_transmitted;
#endif
}


#ifdef __LITTLE_ENDIAN__

unsigned long  
ntohl(unsigned long x)
{
    return (((x & 0x000000FF) << 24) |
            ((x & 0x0000FF00) <<  8) |
            ((x & 0x00FF0000) >>  8) |
            ((x & 0xFF000000) >> 24));
}

unsigned long
ntohs(unsigned short x)
{
    return (((x & 0x00FF) << 8) |
            ((x & 0xFF00) >> 8));
}

#endif
