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

//
// Support for user handlers of additional ethernet packets (nonIP)
//

#define NUM_EXTRA_HANDLERS 4
static struct {
    int type;
    pkt_handler_t handler;
} eth_handlers[NUM_EXTRA_HANDLERS];

pkt_handler_t 
__eth_install_listener(int eth_type, pkt_handler_t handler)
{
    int i, empty;
    pkt_handler_t old;

    if (eth_type > 0x800 || handler != (pkt_handler_t)0) {
	empty = -1;
	for (i = 0;  i < NUM_EXTRA_HANDLERS;  i++) {
	    if (eth_handlers[i].type == eth_type) {
		// Replace existing handler
		old = eth_handlers[i].handler;
		eth_handlers[i].handler = handler;
		return old;
	    }
	    if (eth_handlers[i].type == 0) {
		empty = i;
	    }
	}
	if (empty >= 0) {
	    // Found a free slot
	    eth_handlers[empty].type = eth_type;
	    eth_handlers[empty].handler = handler;
	    return (pkt_handler_t)0;
	}
    }
    printf("** Warning: can't install listener for ethernet type 0x%02x\n", eth_type);
    return (pkt_handler_t)0;
}

void 
__eth_remove_listener(int eth_type)
{
    int i;
    
    for (i = 0;  i < NUM_EXTRA_HANDLERS;  i++) {
        if (eth_handlers[i].type == eth_type) {
            eth_handlers[i].type = 0;
        }
    }    
}

/*
 * Non-blocking poll of ethernet link. Process packets until no more
 * are available.
 */
void
__enet_poll(void)
{
    pktbuf_t *pkt;
    eth_header_t eth_hdr;
    int i, type;
    static bool was_exhausted = false;

    while (true) {
        /*
         * Try to get a free pktbuf and return if none
         * are available.
         */
        if ((pkt = __pktbuf_alloc(ETH_MAX_PKTLEN)) == NULL) {
            if (!was_exhausted) {
                int old = start_console();  // Force output to standard port
                printf("__enet_poll: no more buffers\n");
                __pktbuf_dump();
                was_exhausted = true;
                end_console(old);
            } 
            return;
        }
        was_exhausted = false;  // Report the next time we're out of buffers

        if ((pkt->pkt_bytes = eth_drv_read((char *)&eth_hdr, (char *)pkt->buf,
                                           ETH_MAX_PKTLEN)) > 0) {
#if ENET_STATS
            ++num_received;
#endif
            switch (type = ntohs(eth_hdr.type)) {

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
	        if (type > 0x800) {
		    for (i = 0;  i < NUM_EXTRA_HANDLERS;  i++) {
			if (eth_handlers[i].type == type) {
			    (eth_handlers[i].handler)(pkt, &eth_hdr);
			}
		    }
		}
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
