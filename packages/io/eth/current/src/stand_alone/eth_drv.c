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
#include <cyg/hal/hal_if.h>
#include <eth_drv.h>
#include <netdev.h>

// High-level ethernet driver

// Define this to allow packets seen by this layer to be passed on to
// the previous logical layer, i.e. when stand-alone processing replaces
// system [eCos] processing.
//
// CAUTION! Observations indicate that this cannot work.  When TCP packets
// destined for the stand-alone layer are passed up, the eCos stack treats
// them as evil and sends "disconnect" responses.
//
#undef ETH_DRV_PASS_PACKETS
#define ETH_DRV_PASS_PACKETS

// Interfaces exported to drivers

static void eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr);
static void eth_drv_recv(struct eth_drv_sc *sc, int total_len);
static void eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRWORD key, int status);

struct eth_drv_funs eth_drv_funs = {eth_drv_init, eth_drv_recv, eth_drv_tx_done};

int net_debug;  // FIXME

unsigned char      __local_enet_addr[ETHER_ADDR_LEN+2];
struct eth_drv_sc *__local_enet_sc;

#ifdef ETH_DRV_PASS_PACKETS
//
// Horrible hack: In order to allow the stand-alone networking code to work
// alongside eCos (or any other stack), separate IP addresses must be used.
// When a packet arrives at the interface, we check to see which IP address
// it corresponds to and only pass it "up" if it's not for the stand-alone
// layer.
//
// tres degolas :-(
//
extern char __local_ip_addr[4]; 
#endif

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

// Used to "empty" hardware, even if there are no free buffers
static unsigned char no_pkt[MAX_ETH_MSG];

// Prototypes for functions used in this module
static void eth_drv_start(struct eth_drv_sc *sc);

extern int  start_console(void);
extern void end_console(int);
extern void printf(char *fmt, ...);
extern void dump_buf(CYG_ADDRWORD, int);

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

void
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

#if 0 // Not currently used.  Left in case it's needed in the future
//
// This [internal] function will be called to stop activity on an interface.
//
static void
eth_drv_stop(struct eth_drv_sc *sc)
{
    (sc->funs->stop)(sc);
}
#endif

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
static int packet_sent;

void
eth_drv_write(char *eth_hdr, char *buf, int len)
{
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    struct eth_drv_sc *sc = __local_enet_sc;
    int sg_len = 2;
    void *dbg = CYGACC_CALL_IF_DBG_DATA();
    int old_state;
    int wait_time = 5;  // Timeout before giving up

    if (dbg) {
        sc = (struct eth_drv_sc *)dbg;  // Use control from installed driver
        sc->funs->eth_drv_old = sc->funs->eth_drv;
        sc->funs->eth_drv = &eth_drv_funs;    // Substitute stand-alone driver
        old_state = sc->state;
        if (!old_state & ETH_DRV_STATE_ACTIVE) {
            // This interface not fully initialized, do it now
            (sc->funs->start)(sc, (unsigned char *)&__local_enet_addr, 0);
            sc->state |= ETH_DRV_STATE_ACTIVE;
        }
    }

    while (!(sc->funs->can_send)(sc)) {
        // Give driver a chance to service hardware
        (sc->funs->poll)(sc);
        CYGACC_CALL_IF_DELAY_US(2*100000);
        if (--wait_time <= 0) return;  // Give up on sending packet
    }

    sg_list[0].buf = (CYG_ADDRESS)eth_hdr;
    sg_list[0].len = 14;  // FIXME
    sg_list[1].buf = (CYG_ADDRESS)buf;
    sg_list[1].len = len;
    packet_sent = 0;
    (sc->funs->send)(sc, sg_list, sg_len, len+14, (CYG_ADDRWORD)&packet_sent);
    if (net_debug) {
        int old_console;
        old_console = start_console();
        printf("Ethernet send:\n");
        dump_buf((CYG_ADDRWORD)eth_hdr, 14);
        dump_buf((CYG_ADDRWORD)buf, len);
        end_console(old_console);
    }

    while (!packet_sent) {
        (sc->funs->poll)(sc);
    }
   
    if (dbg) {
//        if (!old_state & ETH_DRV_STATE_ACTIVE) {
//            // This interface was not fully initialized, shut it back down
//            (sc->funs->stop)(sc);
//        }
        sc->funs->eth_drv = sc->funs->eth_drv_old;
        sc->funs->eth_drv_old = (struct eth_drv_funs *)0;
    }
}

//
// This function is called from the hardware driver when an output operation
// has completed - i.e. the packet has been sent.
//
static void
eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRWORD key, int status)
{
    if ((int *)key == &packet_sent) {
        *(int *)key = 1;
    } else {
        // It's possible that this acknowledgement is for a different
        // [logical] driver.  Try and pass it on.
#ifdef DEBUG
        int old_console;
        old_console = start_console();
        printf("tx_done for other key: %x\n", key);
        end_console(old_console);
#endif
        (sc->funs->eth_drv_old->tx_done)(sc, key, status);
    }
}

//
// Receive one packet of data from the hardware, if available
//
int
eth_drv_read(char *eth_hdr, char *buf, int len)
{
    struct eth_drv_sc *sc = __local_enet_sc;
    struct eth_msg *msg;
    int res;
    void *dbg = CYGACC_CALL_IF_DBG_DATA();
    int old_state;

    if (dbg) {
        sc = (struct eth_drv_sc *)dbg;  // Use control from installed driver
        sc->funs->eth_drv_old = sc->funs->eth_drv;
        sc->funs->eth_drv = &eth_drv_funs;    // Substitute stand-alone driver
        old_state = sc->state;
        if (!old_state & ETH_DRV_STATE_ACTIVE) {
            // This interface not fully initialized, do it now
            (sc->funs->start)(sc, (unsigned char *)&__local_enet_addr, 0);
            sc->state |= ETH_DRV_STATE_ACTIVE;
        }
    }
    (sc->funs->poll)(sc);  // Give the driver a chance to fetch packets
    msg = eth_drv_msg_get(&eth_msg_full);
    if (msg) {
        memcpy(eth_hdr, msg->data, 14);
        memcpy(buf, &msg->data[14], msg->len-14);
        eth_drv_msg_put(&eth_msg_free, msg);
        res = msg->len;
    } else {
        res = 0;  // No packets available
    }
   
    if (dbg) {
        sc->funs->eth_drv = sc->funs->eth_drv_old;
        sc->funs->eth_drv_old = (struct eth_drv_funs *)0;
//        if (!old_state & ETH_DRV_STATE_ACTIVE) {
//            // This interface was not fully initialized, shut it back down
//            (sc->funs->stop)(sc);
//        }
    }
    return res;
}

#ifdef ETH_DRV_PASS_PACKETS
//
// This function is called to copy a message up to the next level.
// It is only used when this driver has usurped the processing of
// network functions.
//
static unsigned char *eth_drv_copy_recv_buf;
static void 
eth_drv_copy_recv(struct eth_drv_sc *sc,
                  struct eth_drv_sg *sg_list,
                  int sg_len)
{
    int i;   

    for (i = 0;  i < sg_len;  i++) {
        memcpy((unsigned char *)sg_list[i].buf, 
               eth_drv_copy_recv_buf, sg_list[i].len);
        eth_drv_copy_recv_buf += sg_list[i].len;
    }
}
#endif

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
    unsigned char *buf;

    msg = eth_drv_msg_get(&eth_msg_free);
    if (msg) {
        buf = msg->data;
    } else {
        int old_console;
        old_console = start_console();
        printf("%s: packet of %d bytes dropped\n", __FUNCTION__, total_len);
        end_console(old_console);
        buf = &no_pkt[0];
    }
    sg_list[0].buf = (CYG_ADDRESS)buf;
    sg_list[0].len = total_len;
    sg_len = 1;

    (sc->funs->recv)(sc, sg_list, sg_len);
    if (net_debug) {
        int old_console;
        old_console = start_console();
        printf("Ethernet recv:\n");
        dump_buf((CYG_ADDRWORD)buf, 14);
        dump_buf((CYG_ADDRWORD)buf+14, total_len-14);
        end_console(old_console);
    }
#ifdef ETH_DRV_PASS_PACKETS
    if (sc->funs->eth_drv_old != (struct eth_drv_funs *)0) {
        void (*hold_recv)(struct eth_drv_sc *sc,
                          struct eth_drv_sg *sg_list,
                          int sg_len);
        // See if this packet was for us.  If not, pass it upwards
        // This is a major layering violation!!
        if (memcmp(&__local_ip_addr, &buf[14+16], 4)) {
            hold_recv = sc->funs->recv;
            sc->funs->recv = eth_drv_copy_recv;
            eth_drv_copy_recv_buf = buf;
            // This calls into the 'other' driver, giving it a chance to
            // do something with this data (since it wasn't for us)
            (sc->funs->eth_drv_old->recv)(sc, total_len);
            sc->funs->recv = hold_recv;
        }
    }
#endif
    if (msg) {
        msg->len = total_len;
        eth_drv_msg_put(&eth_msg_full, msg);
    } else {
        dump_buf(sg_list[0].buf, sg_list[0].len);
    }
}
