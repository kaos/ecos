//==========================================================================
//
//      eth_drv.c
//
//      Stand-alone hardware independent networking support for RedBoot
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

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <eth_drv.h>
#include <netdev.h>

// High-level ethernet driver

// Interfaces exported to drivers

static void eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr);
static void eth_drv_recv(struct eth_drv_sc *sc, int total_len);
static void eth_drv_tx_done(struct eth_drv_sc *sc, unsigned long key, int status);

struct eth_drv_funs eth_drv_funs = {eth_drv_init, eth_drv_recv, eth_drv_tx_done};

int net_debug;  // FIXME

unsigned char      __local_enet_addr[ETHER_ADDR_LEN+2];
struct eth_drv_sc *__local_enet_sc;

//
// Buffer 'get' support.  The problem is that this function only gets
// called when some data is required, but packets may arrive on the device
// at any time.  More particularly, on some devices when data arrive, all
// of that data needs to be consumed immediately or be lost.  This process
// is driven by interrupts, which in the stand-along case are simulated by
// calling the "poll" interface.
//
// Thus there will be a pool of buffers, some free and some full, to try
// and manage this.
//

#define MAX_ETH_MSG 1540
#define NUM_ETH_MSG   16

struct eth_msg {
    struct eth_msg *next, *prev;
    int len;   // Actual number of bytes in message
    unsigned char data[MAX_ETH_MSG];
};

struct eth_msg_hdr {
    struct eth_msg *first, *last;
};

static struct eth_msg_hdr eth_msg_free, eth_msg_full;
static struct eth_msg eth_msgs[NUM_ETH_MSG];

// Simple queue management functions

static void
eth_drv_msg_put(struct eth_msg_hdr *hdr, struct eth_msg *msg)
{
    if (hdr->first != (struct eth_msg *)hdr) {
        // Something already in queue
        hdr->last->next = msg;
        msg->prev = hdr->last;
        msg->next = (struct eth_msg *)hdr;
        hdr->last = msg;
    } else {
        hdr->first = hdr->last = msg;
        msg->next = msg->prev = (struct eth_msg *)hdr;
    }
}

static struct eth_msg *
eth_drv_msg_get(struct eth_msg_hdr *hdr)
{
    struct eth_msg *msg;
    if (hdr->first != (struct eth_msg *)hdr) {
        msg = hdr->first;
        hdr->first = msg->next;
        msg->next->prev = (struct eth_msg *)hdr;
    } else {
        msg = (struct eth_msg *)NULL;
    }
    return msg;
}

static void
eth_drv_buffers_init(void)
{
    int i;
    struct eth_msg *msg = eth_msgs;

    eth_msg_full.first = eth_msg_full.last = (struct eth_msg *)&eth_msg_full;
    eth_msg_free.first = eth_msg_free.last = (struct eth_msg *)&eth_msg_free;
    for (i = 0;  i < NUM_ETH_MSG;  i++, msg++) {
        eth_drv_msg_put(&eth_msg_free, msg);
    }
}

//
// This function is called during system initialization to register a
// network interface with the system.
//
static void
eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr)
{
    // Set up hardware address
    memcpy(&sc->sc_arpcom.esa, enaddr, ETHER_ADDR_LEN);
    memcpy(__local_enet_addr, enaddr, ETHER_ADDR_LEN);
    __local_enet_sc = sc;
    eth_drv_start(sc);
}

//
// This [internal] function will be called to stop activity on an interface.
//
static void
eth_drv_stop(struct eth_drv_sc *sc)
{
    (sc->funs->stop)(sc);
}

//
// This [internal] function will be called to start activity on an interface.
//
static void
eth_drv_start(struct eth_drv_sc *sc)
{
    // Perform any hardware initialization
    (sc->funs->start)(sc, (unsigned char *)&sc->sc_arpcom.esa, 0);
}

//
// Send a packet of data to the hardware
//
void
eth_drv_write(char *eth_hdr, char *buf, int len)
{
   struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
   struct eth_drv_sc *sc = __local_enet_sc;
   int sent = 0;
   int sg_len = 2;

   sg_list[0].buf = eth_hdr;
   sg_list[0].len = 14;  // FIXME
   sg_list[1].buf = buf;
   sg_list[1].len = len;
   (sc->funs->send)(sc, sg_list, sg_len, len+14, (unsigned long)&sent);
   if (net_debug) {
       printf("Ethernet send:\n");
       dump_buf(eth_hdr, 14);
       dump_buf(buf, len);
   }
   while (!sent) {
       (sc->funs->poll)(sc);
   }
}

//
// This function is called from the hardware driver when an output operation
// has completed - i.e. the packet has been sent.
//
static void
eth_drv_tx_done(struct eth_drv_sc *sc, unsigned long key, int status)
{
    *(int *)key = 1;
}

//
// Receive one packet of data from the hardware, if available
//
int
eth_drv_read(char *eth_hdr, char *buf, int len)
{
    struct eth_drv_sc *sc = __local_enet_sc;
    struct eth_msg *msg;

    (sc->funs->poll)(sc);  // Give the driver a chance to fetch packets
    msg = eth_drv_msg_get(&eth_msg_full);
    if (msg) {
        memcpy(eth_hdr, msg->data, 14);
        memcpy(buf, &msg->data[14], msg->len-14);
        eth_drv_msg_put(&eth_msg_free, msg);
        return msg->len;
    } else {
        return 0;  // No packets available
    }
}

//
// This function is called from a hardware driver to indicate that an input
// packet has arrived.  The routine will set up appropriate network resources
// to hold the data and call back into the driver to retrieve the data.
//
static void
eth_drv_recv(struct eth_drv_sc *sc, int total_len)
{
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    int               sg_len = 0;
    struct eth_msg *msg;
    static unsigned char no_pkt[MAX_ETH_MSG];
    unsigned char *buf;

    msg = eth_drv_msg_get(&eth_msg_free);
    if (msg) {
        buf = msg->data;
    } else {
        printf("%s: packet of %d bytes dropped\n", __FUNCTION__, total_len);
        buf = &no_pkt[0];
    }
    sg_list[0].buf = buf;
    sg_list[0].len = total_len;
    sg_len = 1;
    (sc->funs->recv)(sc, sg_list, sg_len);
    if (net_debug) {
        printf("Ethernet recv:\n");
        dump_buf(buf, 14);
        dump_buf(buf+14, total_len-14);
    }
    if (msg) {
        msg->len = total_len;
        eth_drv_msg_put(&eth_msg_full, msg);
    } else {
        dump_buf(sg_list[0].buf, sg_list[0].len);
    }
}

//
// Network initialization
//
int
net_init(void)
{
    cyg_netdevtab_entry_t *t;

    // Make sure the recv buffers are set up
    eth_drv_buffers_init();
    __pktbuf_init();
    // Initialize all network devices
    for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
        if (t->init(t)) {
            t->status = CYG_NETDEVTAB_STATUS_AVAIL;
        } else {
            // What to do if device init fails?
            t->status = 0;  // Device not [currently] available
        }
    }
    if (!__local_enet_sc) {
        printf("No network interfaces found\n");
        return -1;
    }    
    return 0;
}
