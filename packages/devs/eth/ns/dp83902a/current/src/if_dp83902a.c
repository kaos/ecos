//==========================================================================
//
//      dev/if_dp83902a.c
//
//      Ethernet device driver for NS DP83902a ethernet controller
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
// Contributors: gthomas, jskov
// Date:         2001-06-13
// Purpose:      
// Description:
//
// FIXME:        Will fail if pinged with large packets (1520 bytes)
//               Add promisc config
//               Add SNMP
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#define __ECOS
#include <eth_drv.h>
#include <netdev.h>

#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#include <pkgconf/io_eth_drivers.h>
#else
#include <cyg/hal/hal_if.h>
#endif

#include <cyg/io/dp83902a.h>

#define __WANT_DEVS
#include CYGDAT_DEVS_ETH_NS_DP83902A_INL
#undef __WANT_DEVS

// This ISR is called when the ethernet interrupt occurs
static cyg_uint32
dp83902a_isr(cyg_vector_t vector, cyg_addrword_t data)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)data;

    DEBUG_FUNCTION();
//    INCR_STAT( interrupts );

    cyg_drv_interrupt_mask(dp->interrupt);
    cyg_drv_interrupt_acknowledge(dp->interrupt);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

static void
dp83902a_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    // This conditioning out is necessary because of explicit calls to this
    // DSR - which would not ever be called in the case of a polled mode
    // usage ie. in RedBoot.
#ifdef CYGPKG_IO_ETH_DRIVERS_NET
    dp83902a_priv_data_t* dp = (dp83902a_priv_data_t *)data;
    struct cyg_netdevtab_entry *ndp = (struct cyg_netdevtab_entry *)(dp->tab);
    struct eth_drv_sc *sc = (struct eth_drv_sc *)(ndp->device_instance);

    DEBUG_FUNCTION();

    // but here, it must be a *sc:
    eth_drv_dsr( vector, count, (cyg_addrword_t)sc );
#else
# ifndef CYGPKG_REDBOOT
#  error Empty DP83902A ethernet DSR is compiled.  Is this what you want?
# endif
#endif
}


// The deliver function (ex-DSR)  handles the ethernet [logical] processing
static void
dp83902a_deliver(struct eth_drv_sc *sc)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;

    DEBUG_FUNCTION();

    // Service the interrupt:
    dp83902a_poll(sc);
    // Allow interrupts to happen again
    cyg_drv_interrupt_unmask(dp->interrupt);
}

static bool 
dp83902a_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    cyg_uint8* base = dp->base;
    int i;

    DEBUG_FUNCTION();

    dp->tab = tab;
    dp->cr_lock = 0;

    CYGHWR_NS_DP83902A_PLF_RESET(base);

    DEBUG_LINE();

    // Prepare ESA
    DP_OUT(base, DP_CR, DP_CR_NODMA | DP_CR_PAGE1);  // Select page 1
    if (dp->hardwired_esa) {
        // Force the NIC to use the specified ESA
        for (i = 0; i < 6; i++)
            DP_OUT(base, DP_P1_PAR0+i, dp->esa[i]);
    } else {
        // Use the address from the serial EEPROM
        for (i = 0; i < 6; i++)
            DP_IN(base, DP_P1_PAR0+i, dp->esa[i]);
    }
    DP_OUT(base, DP_CR, DP_CR_NODMA | DP_CR_PAGE0);  // Select page 0

    diag_printf("DP83902A - %s ESA: %02x:%02x:%02x:%02x:%02x:%02x\n",
                (dp->hardwired_esa) ? "static" : "eeprom",
                dp->esa[0],
                dp->esa[1],
                dp->esa[2],
                dp->esa[3],
                dp->esa[4],
                dp->esa[5] );

    cyg_drv_interrupt_create(
        dp->interrupt,
        0,                  // Priority - unused
        (cyg_addrword_t)dp,// Data item passed to ISR & DSR
        dp83902a_isr,          // ISR
        dp83902a_dsr,          // DSR
        &dp->interrupt_handle, // handle to intr obj
        &dp->interrupt_object ); // space for int obj

    cyg_drv_interrupt_attach(dp->interrupt_handle);
    cyg_drv_interrupt_unmask(dp->interrupt);

    // Initialize upper level driver
    (sc->funs->eth_drv->init)(sc, dp->esa);

    return true;
}

static void
dp83902a_stop(struct eth_drv_sc *sc)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    cyg_uint8 *base = dp->base;

    DEBUG_FUNCTION();

    CR_UP();
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_STOP);  // Brutal
    DP_OUT(base, DP_ISR, 0xFF);               // Clear any pending interrupts
    DP_OUT(base, DP_IMR, 0x00);               // Disable all interrupts
    CR_DOWN();

    dp->running = false;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
dp83902a_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    int i;

    DEBUG_FUNCTION();

    CR_UP();
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_STOP); // Brutal
    DP_OUT(base, DP_DCR, DP_DCR_INIT);
    DP_OUT(base, DP_RBCH, 0);                 // Remote byte count
    DP_OUT(base, DP_RBCL, 0);
    DP_OUT(base, DP_RCR, DP_RCR_MON);     // Accept no packets
    DP_OUT(base, DP_TCR, DP_TCR_LOCAL);   // Transmitter [virtually] off
    DP_OUT(base, DP_TPSR, DP_TX_BUF1);    // Transmitter start page
    dp->tx1 = dp->tx2 = 0;
    dp->tx_next = DP_TX_BUF1;
    dp->tx_started = false;
    DP_OUT(base, DP_PSTART, DP_RX_START); // Receive ring start page
    DP_OUT(base, DP_BNDRY, DP_RX_STOP-1); // Receive ring boundary
    DP_OUT(base, DP_PSTOP, DP_RX_STOP);   // Receive ring end page
    dp->rx_next = DP_RX_START-1;
    DP_OUT(base, DP_ISR, 0xFF);               // Clear any pending interrupts
    DP_OUT(base, DP_IMR, DP_IMR_All);     // Enable all interrupts
    DP_OUT(base, DP_CR, DP_CR_NODMA | DP_CR_PAGE1);  // Select page 1
    DP_OUT(base, DP_P1_CURP, DP_RX_START);   // Current page - next free page for Rx
    for (i = 0;  i < ETHER_ADDR_LEN;  i++) {
        DP_OUT(base, DP_P1_PAR0+i, enaddr[i]);
    }
    // Enable and start device
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_START);  
    DP_OUT(base, DP_TCR, DP_TCR_NORMAL); // Normal transmit operations
    DP_OUT(base, DP_RCR, DP_RCR_AB);  // Accept broadcast, no errors, no multicast
    dp->running = true;
    CR_DOWN();
}

//
// This routine is called to perform special "control" opertions
//
static int
dp83902a_control(struct eth_drv_sc *sc, unsigned long key,
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
dp83902a_can_send(struct eth_drv_sc *sc)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;

    DEBUG_FUNCTION();

    return ((dp->tx1 == 0) || (dp->tx2 == 0));
}

//
// This routine is called to start the transmitter.  It is split out from the
// data handling routine so it may be called either when data becomes first 
// available or when an Tx interrupt occurs
//

static void
dp83902a_start_xmit(struct eth_drv_sc *sc, int start_page, int len)
{    
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    cyg_uint8 *base = dp->base;

    DEBUG_FUNCTION();

#if DEBUG & 1
    diag_printf("Tx pkt %d len %d\n", start_page, len);
    if (dp->tx_started)
        diag_printf("TX already started?!?\n");
#endif

    CR_UP();
    DP_OUT(base, DP_ISR, (DP_ISR_TxP | DP_ISR_TxE));
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_START);
    DP_OUT(base, DP_TBCL, len & 0xFF);
    DP_OUT(base, DP_TBCH, len >> 8);
    DP_OUT(base, DP_TPSR, start_page);
    DP_OUT(base, DP_CR, DP_CR_NODMA | DP_CR_TXPKT | DP_CR_START);

    CR_DOWN();
    dp->tx_started = true;
}

//
// This routine is called to send data to the hardware.  It is known a-priori
// that there is free buffer space (dp->tx_next).
//
static void 
dp83902a_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
            int total_len, unsigned long key)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    int i, len, start_page, pkt_len;
    unsigned char *data;
    cyg_uint8 isr;
#if DEBUG & 4
    int dx;
#endif

    DEBUG_FUNCTION();

    pkt_len = total_len;
    if (pkt_len < IEEE_8023_MIN_FRAME) pkt_len = IEEE_8023_MIN_FRAME;
    start_page = dp->tx_next;
    if (dp->tx_next == DP_TX_BUF1) {
        dp->tx1 = start_page;
        dp->tx1_len = pkt_len;
        dp->tx1_key = key;
        dp->tx_next = DP_TX_BUF2;
    } else {
        dp->tx2 = start_page;
        dp->tx2_len = pkt_len;
        dp->tx2_key = key;
        dp->tx_next = DP_TX_BUF1;
    }
    CR_UP();

#if DEBUG & 5
    diag_printf("TX prep page %d len %d\n", start_page, pkt_len);
#endif

    DP_OUT(base, DP_ISR, DP_ISR_RDC);  // Clear end of DMA
    // Set these first, following the small print of the manual
    DP_OUT(base, DP_RBCL, 1);
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_RDMA | DP_CR_START);

#ifdef CYGHWR_NS_DP83902A_PLF_BROKEN_TX_DMA
    // Stall for a bit before continuing to work around random data
    // corruption problems on some platforms.
    CYGACC_CALL_IF_DELAY_US(1);
#endif

    // Send data to device buffer(s)
    DP_OUT(base, DP_RSAL, 0);
    DP_OUT(base, DP_RSAH, start_page);
    DP_OUT(base, DP_RBCL, pkt_len & 0xFF);
    DP_OUT(base, DP_RBCH, pkt_len >> 8);
    DP_OUT(base, DP_CR, DP_CR_WDMA | DP_CR_START);

    // Put data into buffer
    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned char *)sg_list[i].buf;
        len = sg_list[i].len;
#if DEBUG & 4
        diag_printf(" sg buf %08x len %08x\n ", data, len);
        dx = 0;
#endif
        while (len > 0) {
#ifdef CYGHWR_NS_DP83902A_PLF_16BIT_DATA
            cyg_uint16 tmp;
            tmp = *data++ << 8;
            len -= 2;
            if (len >= 0)
                tmp |= *data++;
            DP_OUT_DATA(dp->data, tmp);
#if DEBUG & 4
            diag_printf(" %04x", tmp);
            if (0 == (++dx % 8)) diag_printf("\n ");
#endif
#else
#if DEBUG & 4
            diag_printf(" %02x", *data);
            if (0 == (++dx % 16)) diag_printf("\n ");
#endif
            DP_OUT_DATA(dp->data, *data++);
            len--;
#endif
        }
#if DEBUG & 4
        diag_printf("\n");
#endif
    }
    if (total_len < pkt_len) {
#if DEBUG & 4
        diag_printf("  + %d bytes of padding\n", pkt_len - total_len);
#endif
        // Padding to 802.3 length was required
        for (i = total_len;  i < pkt_len;) {
#ifdef CYGHWR_NS_DP83902A_PLF_16BIT_DATA
            i += 2;
#else
            i++;
#endif
            DP_OUT_DATA(dp->data, 0);
        }
    }

#ifdef CYGHWR_NS_DP83902A_PLF_BROKEN_TX_DMA
    // After last data write, delay for a bit before accessing the
    // device again, or we may get random data corruption in the last
    // datum (on some platforms).
    CYGACC_CALL_IF_DELAY_US(1);
#endif

    // Wait for DMA to complete
    do {
        DP_IN(base, DP_ISR, isr);
    } while ((isr & DP_ISR_RDC) == 0);
    // Then disable DMA
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_START);
    CR_DOWN();

    // Start transmit if not already going
    if (!dp->tx_started) {
        if (start_page == dp->tx1) {
            dp->tx_int = 1;  // Expecting interrupt from BUF1
        } else {
            dp->tx_int = 2;  // Expecting interrupt from BUF2
        }
        dp83902a_start_xmit(sc, start_page, pkt_len);
    }
}

//
// This function is called when a packet has been received.  It's job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'dp83902a_recv' will be called to actually fetch it from the hardware.
//
static void
dp83902a_RxEvent(struct eth_drv_sc *sc, int stat)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    unsigned char rsr;
    unsigned char rcv_hdr[4];
    int i, len, pkt, cur;

    DEBUG_FUNCTION();

    DP_IN(base, DP_RSR, rsr);
    while (true) {
        CR_UP();
        // Read incoming packet header
        DP_OUT(base, DP_CR, DP_CR_PAGE1 | DP_CR_NODMA | DP_CR_START);
        DP_IN(base, DP_P1_CURP, cur);
        DP_OUT(base, DP_P1_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_START);
        DP_IN(base, DP_BNDRY, pkt);
        pkt += 1;
        if (pkt == cur) {
            CR_DOWN();
            break;
        }
        if (pkt == DP_RX_STOP) pkt = DP_RX_START;
        DP_OUT(base, DP_RBCL, sizeof(rcv_hdr));
        DP_OUT(base, DP_RBCH, 0);
        DP_OUT(base, DP_RSAL, 0);
        DP_OUT(base, DP_RSAH, pkt);
        if (dp->rx_next == pkt) {
            DP_OUT(base, DP_BNDRY, cur-1); // Update pointer
            CR_DOWN();
            return;
        }
        dp->rx_next = pkt;
        DP_OUT(base, DP_ISR, DP_ISR_RDC); // Clear end of DMA
        DP_OUT(base, DP_CR, DP_CR_RDMA | DP_CR_START);

        for (i = 0;  i < sizeof(rcv_hdr);) {
#ifdef CYGHWR_NS_DP83902A_PLF_16BIT_DATA
            cyg_uint16 tmp;
            DP_IN_DATA(dp->data, tmp);
            rcv_hdr[i++] = (tmp >> 8) & 0xff;
            rcv_hdr[i++] = tmp & 0xff;
#else
            DP_IN_DATA(dp->data, rcv_hdr[i++]);
#endif
        }
        CR_DOWN();

#if DEBUG & 5
        diag_printf("rx hdr %02x %02x %02x %02x\n",
                    rcv_hdr[0], rcv_hdr[1], rcv_hdr[2], rcv_hdr[3]);
#endif
        len = ((rcv_hdr[3] << 8) | rcv_hdr[2]) - sizeof(rcv_hdr);
        (sc->funs->eth_drv->recv)(sc, len);
        DP_OUT(base, DP_BNDRY, rcv_hdr[1]-1); // Update pointer
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
dp83902a_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    int i, mlen, len;
    unsigned char *data;
    cyg_uint8 saved_char = 0;
    bool saved;
#if DEBUG & 4
    int dx;
#endif

    DEBUG_FUNCTION();

    // Compute total packet length
    len = 0;
    for (i = 0;  i < sg_len;  i++) {
        len += sg_list[i].len;
    }

#if DEBUG & 5
    diag_printf("Rx packet %d length %d\n", dp->rx_next, len);
#endif

    CR_UP();

    // Read incoming packet data
    DP_OUT(base, DP_CR, DP_CR_PAGE0 | DP_CR_NODMA | DP_CR_START);
    DP_OUT(base, DP_RBCL, len & 0xFF);
    DP_OUT(base, DP_RBCH, len >> 8);
    DP_OUT(base, DP_RSAL, 4);           // Past header
    DP_OUT(base, DP_RSAH, dp->rx_next);
    DP_OUT(base, DP_ISR, DP_ISR_RDC); // Clear end of DMA
    DP_OUT(base, DP_CR, DP_CR_RDMA | DP_CR_START);

    saved = false;
    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned char *)sg_list[i].buf;
        if (data) {
            mlen = sg_list[i].len;
#if DEBUG & 4
            diag_printf(" sg buf %08x len %08x \n", data, mlen);
            dx = 0;
#endif
            while (0 < mlen) {
                // Saved byte from previous loop?
                if (saved) {
                    *data++ = saved_char;
                    mlen--;
                    saved = false;
                    continue;
                }

#ifdef CYGHWR_NS_DP83902A_PLF_16BIT_DATA
                {
                    cyg_uint16 tmp;
                    DP_IN_DATA(dp->data, tmp);
#if DEBUG & 4
                    diag_printf(" %04x", tmp);
                    if (0 == (++dx % 8)) diag_printf("\n ");
#endif
                    *data++ = (tmp >> 8) & 0xff;
                    mlen--;
                    if (0 == mlen) {
                        saved_char = tmp & 0xff;
                        saved = true;
                    } else {
                        *data++ = tmp & 0xff;
                        mlen--;
                    }
                }
#else
                {
                    cyg_uint8 tmp;
                    DP_IN_DATA(dp->data, tmp);
#if DEBUG & 4
                    diag_printf(" %02x", tmp);
                    if (0 == (++dx % 16)) diag_printf("\n ");
#endif
                    *data++ = tmp;;
                    mlen--;
                }
#endif
            }
#if DEBUG & 4
            diag_printf("\n");
#endif
        }
    }
    CR_DOWN();
}

static void
dp83902a_TxEvent(struct eth_drv_sc *sc, int stat)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    unsigned char tsr;
    unsigned long key;

    DEBUG_FUNCTION();

    DP_IN(base, DP_TSR, tsr);
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
        dp83902a_start_xmit(sc, dp->tx1, dp->tx1_len);
        dp->tx_int = 1;
    } else if (dp->tx2) {
        dp83902a_start_xmit(sc, dp->tx2, dp->tx2_len);
        dp->tx_int = 2;
    } else {
        dp->tx_int = 0;
    }
    // Tell higher level we sent this packet
    (sc->funs->eth_drv->tx_done)(sc, key, 0);
}

static void
dp83902a_BufEvent(struct eth_drv_sc *sc, int stat)
{
    // What to do if the receive buffers overflow?
    if (stat & DP_ISR_OFLW) {
        // Note: [so far] it seems safe to just ignore this condition
        // The Linux driver goes through extraordinary pains to handle
        // it, including totally shutting down the chip and restarting.
    }
}

static void
dp83902a_poll(struct eth_drv_sc *sc)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    cyg_uint8 *base = dp->base;
    unsigned char isr;

//    DEBUG_FUNCTION();

    CR_UP();
    DP_OUT(base, DP_CR, DP_CR_NODMA | DP_CR_PAGE0 | DP_CR_START);
    CR_DOWN();
    DP_IN(base, DP_ISR, isr);
    while (0 != isr) {
        DP_OUT(base, DP_ISR, isr);      // Clear set bits
        if (!dp->running) break;        // Is this necessary?
        // Check for tx_started on TX event since these may happen
        // spuriously it seems.
        if (isr & (DP_ISR_TxP|DP_ISR_TxE) && dp->tx_started) {
            dp83902a_TxEvent(sc, isr);
        }
        if (isr & (DP_ISR_RxP|DP_ISR_RxE)) {
            dp83902a_RxEvent(sc, isr);
        }
        if (isr & (DP_ISR_OFLW|DP_ISR_CNT)) {
            dp83902a_BufEvent(sc, isr);
        }
        DP_IN(base, DP_ISR, isr);
    }

    CYGHWR_NS_DP83902A_PLF_INT_CLEAR(dp);
}

static int
dp83902a_int_vector(struct eth_drv_sc *sc)
{
    struct dp83902a_priv_data *dp = (struct dp83902a_priv_data *)sc->driver_private;
    return dp->interrupt;
}
