//==========================================================================
//
//      net/icmp.c
//
//      Stand-alone ICMP networking support for RedBoot
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

/*
 * Handle ICMP packets.
 */

static icmp_handler_t icmp_handler;

/*
 * Install a handler for incoming icmp packets.
 * Returns zero if successful, -1 if socket is already used.
 */
int
__icmp_install_listener(icmp_handler_t handler)
{
    if (icmp_handler) {
        return -1;
    }
    icmp_handler = handler;
    return 0;
}


/*
 * Remove the handler
 */
void
__icmp_remove_listener(void)
{
    icmp_handler = (icmp_handler_t)NULL;
}

void
__icmp_handler(pktbuf_t *pkt, ip_route_t *r)
{
    word cksum;

    if (pkt->icmp_hdr->type == ICMP_TYPE_ECHOREQUEST &&
	pkt->icmp_hdr->code == 0 &&
	__sum((word *)pkt->icmp_hdr, pkt->pkt_bytes, 0) == 0) {

	pkt->icmp_hdr->type = ICMP_TYPE_ECHOREPLY;
	pkt->icmp_hdr->checksum = 0;
        cksum = __sum((word *)pkt->icmp_hdr, pkt->pkt_bytes, 0);
	pkt->icmp_hdr->checksum = htons(cksum);
	
	BSPLOG(bsp_log("icmp: seq<%d>\n", pkt->icmp_hdr->seqnum));

	__ip_send(pkt, IP_PROTO_ICMP, r);
    } else if (icmp_handler) {
        (*icmp_handler)(pkt, r);
    }
    __pktbuf_free(pkt);
}


