//==========================================================================
//
//      net/icmp.c
//
//      Stand-alone ICMP networking support for RedBoot
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
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
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
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


