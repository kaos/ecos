//==========================================================================
//
//      dev/if_sc_lpe.c
//
//      Ethernet device driver for Socket Communications Compact Flash card
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
// Date:         2000-07-07
// Purpose:      
// Description:  hardware driver for LPCF+ ethernet
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/pcmcia.h>
#include <eth_drv.h>
#include <netdev.h>

#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#else
#include <cyg/hal/hal_if.h>
#define diag_printf printf
#endif

#include "dp8390.h"

struct sc_lpe_priv_data {
    struct cf_slot *slot;
    dp8390_regs *regs;
    int tx_next;           // First free Tx page
    int tx_int;            // Expecting interrupt from this buffer
    int rx_next;           // First free Rx page
    int tx1, tx2;          // Page numbers for Tx buffers
    unsigned long tx1_key, tx2_key;   // Used to ack when packet sent
    int tx1_len, tx2_len;
    bool tx_started, running;
    struct cyg_netdevtab_entry *tab;
} _sc_lpe_priv_data;

ETH_DRV_SC(sc_lpe_sc,
           &_sc_lpe_priv_data, // Driver specific data
           "eth0",             // Name for this interface
           sc_lpe_start,
           sc_lpe_stop,
           sc_lpe_control,
           sc_lpe_can_send,
           sc_lpe_send,
           sc_lpe_recv,
           sc_lpe_int, // deliver function, called from fast net thread
           sc_lpe_int,
           sc_lpe_int_vector
    );

NETDEVTAB_ENTRY(sc_lpe_netdev, 
                "sc_lpe", 
                sc_lpe_init, 
                &sc_lpe_sc);

// Updated to actual address when card is inserted
static unsigned char enaddr[] = { 0x00, 0xC0, 0x1B, 0x04, 0x31, 0xEE};

static void sc_lpe_int(struct eth_drv_sc *sc);

#ifdef CYGPKG_NET
#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
static char sc_lpe_card_handler_stack[STACK_SIZE];
static cyg_thread sc_lpe_card_handler_thread_data;
static cyg_handle_t sc_lpe_card_handler_thread_handle;
#endif  // CYGPKG_NET

static void
do_delay(int ticks)
{
#ifdef CYGPKG_KERNEL
    cyg_thread_delay(ticks);
#else
    CYGACC_CALL_IF_DELAY_US(10000*ticks);
#endif
}

//
// This runs as a separate thread to handle the card.  In particular, insertions
// and deletions need to be handled and they take time/coordination, thus the
// separate thread.
//
#ifdef CYGPKG_NET
static void
#else
static int
#endif
sc_lpe_card_handler(cyg_addrword_t param)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)param;
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    struct cf_slot *slot;
    struct cf_cftable cftable;
    struct cf_config config;
    int i, len, ptr, cor = 0;
    unsigned char buf[256], *cp;
    dp8390_regs *regs;
    unsigned char *vers_product, *vers_manuf, *vers_revision, *vers_date;
#ifndef CYGPKG_NET
    int tries = 0;
#endif
    bool first = true;

    slot = dp->slot;
    cyg_drv_dsr_lock();
    while (true) {
        cyg_drv_dsr_unlock();   // Give DSRs a chance to run (card insertion)
        cyg_drv_dsr_lock();
        if ((slot->state == CF_SLOT_STATE_Inserted) ||
            ((slot->state == CF_SLOT_STATE_Ready) && first)) {
            first = false;
            if (slot->state != CF_SLOT_STATE_Ready) {
                cf_change_state(slot, CF_SLOT_STATE_Ready);
            }
            if (slot->state != CF_SLOT_STATE_Ready) {
                diag_printf("CF card won't go ready!\n");
#ifndef CYGPKG_NET
                return false;
#else
                continue;
#endif
            }
            len = sizeof(buf);
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_MANFID, buf, &len, &ptr)) {
                if (*(short *)&buf[2] != SC_LPE_MANUF) {
                    diag_printf("Not a SC LPE, sorry\n");
                    continue;
                }
            } 
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_VERS_1, buf, &len, &ptr)) {
                // Find individual strings
                cp = &buf[4];
                vers_product = cp;
                while (*cp++) ;  // Skip to nul
                vers_manuf = cp;
                while (*cp++) ;  // Skip to nul
                vers_revision = cp;
                while (*cp++) ;  // Skip to nul
                vers_date = cp;
#ifndef CYGPKG_NET
                if (tries != 0) printf("\n");
                diag_printf("%s: %s %s %s\n", vers_manuf, vers_product, vers_revision, vers_date);
#endif
            }
            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_CONFIG, buf, &len, &ptr)) {
                if (cf_parse_config(buf, len, &config)) {
                    cor = config.base;
                }
            }
            if (!cor) {
//                diag_printf("Couldn't find COR pointer!\n");
                continue;
            }

            // Fetch hardware address from card - terrible, but not well defined
            // Note: at least one card has been known to not have a valid ESA anywhere
            if (slot->attr[0x1C0] != (unsigned char)0xFF) {
                for (i = 0;  i < ETHER_ADDR_LEN;  i++) {
                    enaddr[i] = slot->attr[0x1C0+(i*2)];
                }
            }

            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_CFTABLE_ENTRY, buf, &len, &ptr)) {
                if (cf_parse_cftable(buf, len, &cftable)) {
                    dp->regs = regs = (dp8390_regs *)&dp->slot->io[cftable.io_space.base[0]];
                    cf_set_COR(slot, cor, cftable.cor);
                    // Reset card (read issues RESET, write clears it)
                    regs->w.page0.reset = regs->r.page0.reset;
                    while ((regs->r.page0.isr & DP8390_ISR_RESET) == 0) ;  // Wait for card
                    // Initialize upper level driver
                    (sc->funs->eth_drv->init)(sc, enaddr);
                    // Tell system card is ready to talk
                    dp->tab->status = CYG_NETDEVTAB_STATUS_AVAIL;
#ifndef CYGPKG_NET
                    cyg_drv_dsr_unlock();
                    return true;
#endif
                } else {
                    diag_printf("Can't parse CIS\n");
                    continue;
                }
            } else {
                diag_printf("Can't fetch config info\n");
                continue;
            }
        } else if (slot->state == CF_SLOT_STATE_Removed) {
            diag_printf("Compact Flash card removed!\n");
        } else {
            cyg_drv_dsr_unlock();
            do_delay(50);  // FIXME!
#ifndef CYGPKG_NET
            if (tries == 0) printf("... Waiting for network card: ");
            printf(".");
            if (++tries == 10) {
                // 5 seconds have elapsed - give up
                return false;
            }
            cf_hwr_poll(slot);  // Check to see if card has been inserted
#endif
            cyg_drv_dsr_lock();
        }
    }
}

static bool 
sc_lpe_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;

    cf_init();  // Make sure Compact Flash subsystem is initialized
    dp->slot = cf_get_slot(0);
    dp->tab = tab;

#ifdef CYGPKG_NET
    // Create card handling [background] thread
    cyg_thread_create(CYGPKG_NET_THREAD_PRIORITY-1,          // Priority
                      sc_lpe_card_handler,                   // entry
                      (cyg_addrword_t)sc,                    // entry parameter
                      "SC LP-E card support",                // Name
                      &sc_lpe_card_handler_stack[0],         // Stack
                      STACK_SIZE,                            // Size
                      &sc_lpe_card_handler_thread_handle,    // Handle
                      &sc_lpe_card_handler_thread_data       // Thread data structure
            );
    cyg_thread_resume(sc_lpe_card_handler_thread_handle);    // Start it

    // Initialize environment, setup interrupt handler
    // eth_drv_dsr is used to tell the fast net thread to run the deliver funcion.
    cf_register_handler(dp->slot, eth_drv_dsr, sc);

    return false;  // Device is not ready until inserted, powered up, etc.
#else
    // Initialize card
    return sc_lpe_card_handler(sc);
#endif
}

static void
sc_lpe_stop(struct eth_drv_sc *sc)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;

    regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_STOP;  // Brutal
    regs->w.page0.isr = 0xFF;               // Clear any pending interrupts
    regs->w.page0.imr = 0x00;               // Disable all interrupts
    dp->running = false;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
sc_lpe_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    int i;

    regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_STOP;  // Brutal
    regs->w.page0.dcr = DP8390_DCR_INIT;
    regs->w.page0.rbch = 0;                 // Remote byte count
    regs->w.page0.rbcl = 0;
    regs->w.page0.rcr = DP8390_RCR_MON;     // Accept no packets
    regs->w.page0.tcr = DP8390_TCR_LOCAL;   // Transmitter [virtually] off
    regs->w.page0.tpsr = DP8390_TX_BUF1;    // Transmitter start page
    dp->tx1 = dp->tx2 = 0;
    dp->tx_next = DP8390_TX_BUF1;
    dp->tx_started = false;
    regs->w.page0.pstart = DP8390_RX_START; // Receive ring start page
    regs->w.page0.bndry = DP8390_RX_STOP-1; // Receive ring boundary
    regs->w.page0.pstop = DP8390_RX_STOP;   // Receive ring end page
    dp->rx_next = DP8390_RX_START-1;
    regs->w.page0.isr = 0xFF;               // Clear any pending interrupts
    regs->w.page0.imr = DP8390_IMR_All;     // Enable all interrupts
    regs->w.page0.cr = DP8390_CR_NODMA | DP8390_CR_PAGE1;  // Select page 1
    regs->w.page1.curp = DP8390_RX_START;   // Current page - next free page for Rx
    for (i = 0;  i < ETHER_ADDR_LEN;  i++) {
        regs->w.page1.par[i] = enaddr[i];
    }
    // Enable and start device
    regs->w.page0.cr = DP8390_CR_NODMA | DP8390_CR_START;  
    regs->w.page0.tcr = DP8390_TCR_NORMAL;   // Normal transmit operations
    regs->w.page0.rcr = DP8390_RCR_AB;  // Accept broadcast, no errors, no multicast
    dp->running = true;
}

//
// This routine is called to perform special "control" opertions
//
static int
sc_lpe_control(struct eth_drv_sc *sc, unsigned long key,
               void *data, int data_len)
{
    switch (key) {
    case ETH_DRV_SET_MAC_ADDRESS:
        return 0;
        break;
    default:
        return 1;
        break;
    }
}

//
// This routine is called to see if it is possible to send another packet.
// It will return non-zero if a transmit is possible, zero otherwise.
//
static int
sc_lpe_can_send(struct eth_drv_sc *sc)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    return ((dp->tx1 == 0) || (dp->tx2 == 0));
}

//
// This routine is called to start the transmitter.  It is split out from the
// data handling routine so it may be called either when data becomes first 
// available or when an Tx interrupt occurs
//

static void
sc_lpe_start_xmit(struct eth_drv_sc *sc, int start_page, int len)
{    
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;

    regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_START;
    regs->w.page0.tbcl = len & 0xFF;
    regs->w.page0.tbch = len >> 8;
    regs->w.page0.tpsr = start_page;
    regs->w.page0.cr = DP8390_CR_NODMA | DP8390_CR_TXPKT | DP8390_CR_START;
    dp->tx_started = true;
}

//
// This routine is called to send data to the hardware.  It is known a-priori
// that there is free buffer space (dp->tx_next).
//
static void 
sc_lpe_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
            int total_len, unsigned long key)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    int i, len, start_page, pkt_len;
    unsigned char *data;

    pkt_len = total_len;
    if (pkt_len < IEEE_8023_MIN_FRAME) pkt_len = IEEE_8023_MIN_FRAME;
    start_page = dp->tx_next;
    if (dp->tx_next == DP8390_TX_BUF1) {
        dp->tx1 = start_page;
        dp->tx1_len = pkt_len;
        dp->tx1_key = key;
        dp->tx_next = DP8390_TX_BUF2;
    } else {
        dp->tx2 = start_page;
        dp->tx2_len = pkt_len;
        dp->tx2_key = key;
        dp->tx_next = DP8390_TX_BUF1;
    }
    // Send data to device buffer(s)
    regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_START;
    regs->w.page0.rbcl = pkt_len & 0xFF;
    regs->w.page0.rbch = pkt_len >> 8;
    regs->w.page0.rsal = 0;
    regs->w.page0.rsah = start_page;
    regs->w.page0.isr = DP8390_ISR_RDC;  // Clear end of DMA
    regs->w.page0.cr = DP8390_CR_WDMA | DP8390_CR_START;
    // Put data into buffer
    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned char *)sg_list[i].buf;
        len = sg_list[i].len;
        while (len-- > 0) {
            regs->w.page0.data = *data++;
        }
    }
    if (total_len < pkt_len) {
        // Padding to 802.3 length was required
        for (i = total_len;  i < pkt_len;  i++) {
            regs->w.page0.data = 0;
        }
    }
    // Wait for DMA to complete
    while ((regs->r.page0.isr & DP8390_ISR_RDC) == 0) ;
    // Start transmit if not already going
    if (!dp->tx_started) {
        if (start_page == dp->tx1) {
            dp->tx_int = 1;  // Expecting interrupt from BUF1
        } else {
            dp->tx_int = 2;  // Expecting interrupt from BUF2
        }
        sc_lpe_start_xmit(sc, start_page, pkt_len);
    }
}

//
// This function is called when a packet has been received.  It's job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'sc_lpe_recv' will be called to actually fetch it from the hardware.
//
static void
sc_lpe_RxEvent(struct eth_drv_sc *sc, int stat)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    unsigned char rsr;
    unsigned char rcv_hdr[4];
    int i, len, pkt, cur;

    rsr = regs->r.page0.rsr;
    while (true) {
        // Read incoming packet header
        regs->w.page0.cr = DP8390_CR_PAGE1 | DP8390_CR_NODMA | DP8390_CR_START;
        cur = regs->r.page1.curp;
        regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_START;
        pkt = regs->r.page0.bndry + 1;
        if (pkt == cur) break;
        if (pkt == DP8390_RX_STOP) pkt = DP8390_RX_START;
        regs->w.page0.rbcl = sizeof(rcv_hdr);
        regs->w.page0.rbch = 0;
        regs->w.page0.rsal = 0;
        regs->w.page0.rsah = pkt;
        if (dp->rx_next == pkt) {
//            printf("sc_lpe - receiver confused, stat: %x, , rsr: %x\n", stat, rsr);
            regs->w.page0.bndry = cur-1;  // Update pointer
            return;
        }
        dp->rx_next = pkt;
        regs->w.page0.isr = DP8390_ISR_RDC;  // Clear end of DMA
        regs->w.page0.cr = DP8390_CR_RDMA | DP8390_CR_START;
        for (i = 0;  i < sizeof(rcv_hdr);  i++) {
            rcv_hdr[i] = regs->r.page0.data;
        }
        len = ((rcv_hdr[3] << 8) | rcv_hdr[2]) - sizeof(rcv_hdr);
        (sc->funs->eth_drv->recv)(sc, len);
        regs->w.page0.bndry = rcv_hdr[1]-1;  // Update pointer
    }
}

//
// This function is called as a result of the "eth_drv_recv()" call above.
// It's job is to actually fetch data for a packet from the hardware once
// memory buffers have been allocated for the packet.  Note that the buffers
// may come in pieces, using a scatter-gather list.  This allows for more
// efficient processing in the upper layers of the stack.
//
static void
sc_lpe_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    int i, mlen, len;
    unsigned char *data;

    // Compute total packet length
    len = 0;
    for (i = 0;  i < sg_len;  i++) {
        len += sg_list[i].len;
    }
    // Read incoming packet data
    regs->w.page0.cr = DP8390_CR_PAGE0 | DP8390_CR_NODMA | DP8390_CR_START;
    regs->w.page0.rbcl = len & 0xFF;
    regs->w.page0.rbch = len >> 8;
    regs->w.page0.rsal = 4;  // Past header
    regs->w.page0.rsah = dp->rx_next;
    regs->w.page0.isr = DP8390_ISR_RDC;  // Clear end of DMA
    regs->w.page0.cr = DP8390_CR_RDMA | DP8390_CR_START;
    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned char *)sg_list[i].buf;
        if (data) {
            mlen = sg_list[i].len;
            while (mlen >= sizeof(*data)) {
                *data++ = regs->r.page0.data;
                mlen -= sizeof(*data);
            }
        }
    }
}

static void
sc_lpe_TxEvent(struct eth_drv_sc *sc, int stat)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    unsigned char tsr;
    unsigned long key;

    tsr = regs->r.page0.tsr;
    if (dp->tx_int == 1) {
        key = dp->tx1_key;
        dp->tx1 = 0;
    } else {
        key = dp->tx2_key;
        dp->tx2 = 0;
    }
    // Start next packet if one is ready
    dp->tx_started = false;
    if (dp->tx1) {
        sc_lpe_start_xmit(sc, dp->tx1, dp->tx1_len);
        dp->tx_int = 1;
    } else if (dp->tx2) {
        sc_lpe_start_xmit(sc, dp->tx2, dp->tx2_len);
        dp->tx_int = 2;
    } else {
        dp->tx_int = 0;
    }
    // Tell higher level we sent this packet
    (sc->funs->eth_drv->tx_done)(sc, key, 0);
}

static void
sc_lpe_BufEvent(struct eth_drv_sc *sc, int stat)
{
    // What to do if the receive buffers overflow?
    if (stat & DP8390_ISR_OFLW) {
        // Note: [so far] it seems safe to just ignore this condition
        // The Linux driver goes through extraordinary pains to handle
        // it, including totally shutting down the chip and restarting.
    }
}

static void
sc_lpe_int(struct eth_drv_sc *sc)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    dp8390_regs *regs = dp->regs;
    unsigned char isr;

    regs->w.page0.cr = DP8390_CR_NODMA | DP8390_CR_PAGE0 | DP8390_CR_START;
    while ((isr = regs->r.page0.isr) != 0) {
        regs->w.page0.isr = isr;  // Clear set bits
        if (!dp->running) break;  // Is this necessary?
        if (isr & (DP8390_ISR_TxP|DP8390_ISR_TxE)) {
            sc_lpe_TxEvent(sc, isr);
        }
        if (isr & (DP8390_ISR_RxP|DP8390_ISR_RxE)) {
            sc_lpe_RxEvent(sc, isr);
        }
        if (isr & (DP8390_ISR_OFLW|DP8390_ISR_CNT)) {
            sc_lpe_BufEvent(sc, isr);
        }
    }
    cf_clear_interrupt(dp->slot);
}

static int
sc_lpe_int_vector(struct eth_drv_sc *sc)
{
    struct sc_lpe_priv_data *dp = (struct sc_lpe_priv_data *)sc->driver_private;
    return dp->slot->int_num;
}
