//==========================================================================
//
//      simple.c
//
//      lwIP simple mode (single-threaded) support.
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008, 2009 Free Software Foundation
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2008-12-08
// Purpose:
// Description:  lwIP simple mode (single-threaded) support.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>
#include <pkgconf/system.h>
#include <pkgconf/net_lwip.h>

#include "lwip.h"
#include "lwip/opt.h"
#include "lwip/sys.h"
#include "lwip/memp.h"
#include "lwip/ip_addr.h"
#include "lwip/init.h"
#include "lwip/tcp.h"
#include "lwip/ip_frag.h"
#include "lwip/dhcp.h"
#include "lwip/autoip.h"
#include "lwip/igmp.h"
#include "lwip/dns.h"
#include "lwip/memp.h"
#include "lwip/netif.h"

#include "netif/loopif.h"
#include "netif/slipif.h"
#include "netif/etharp.h"
#include "netif/ppp/ppp.h"
#include "netif/ppp/chat.h"

#include <cyg/infra/diag.h>
#include <cyg/kernel/kapi.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
#include <cyg/hal/hal_if.h>            // HAL_CTRLC_CHECK
#endif

#ifdef CYGPKG_LWIP_ETH
#include <cyg/io/eth/eth_drv.h>
#include <cyg/io/eth/netdev.h>
#endif

#if LWIP_HAVE_LOOPIF && defined(CYGIMP_LWIP_LOOPIF_INSTANCE)
static struct netif loopif;
#endif

#if LWIP_HAVE_SLIPIF && defined(CYGIMP_LWIP_SLIPIF_INSTANCE)
static struct netif slipif;
#endif

#ifdef CYGPKG_LWIP_ETH

// Ethernet driver table
CYG_HAL_TABLE_BEGIN(__NETDEVTAB__, netdev);
CYG_HAL_TABLE_END(__NETDEVTAB_END__, netdev);

// Received packet
struct recv_packet {
    struct netif *netif;
    struct pbuf *p;
};

#define MAX_RECV_PACKETS    (PBUF_POOL_SIZE + 1)
static struct recv_packet recv_packet[MAX_RECV_PACKETS];
static int recv_packet_read;
static int recv_packet_write;
static int recv_packet_count;

#endif // CYGPKG_LWIP_ETH

// lwIP interval timer
struct lwip_timer {
    cyg_uint32 time;            // Time counter in ms
    cyg_uint32 interval;        // Interval in ms
    void (*timer_func)(void);   // lwIP timer to call
};

// List of lwIP interval timers
static struct lwip_timer lwip_timers[] = {
#if LWIP_TCP
    { 0, TCP_TMR_INTERVAL,          tcp_tmr },
#endif
#if LWIP_ARP
    { 0, ARP_TMR_INTERVAL,          etharp_tmr },
#endif
#if LWIP_DHCP
    { 0, DHCP_FINE_TIMER_MSECS,     dhcp_fine_tmr },
    { 0, DHCP_COARSE_TIMER_MSECS,   dhcp_coarse_tmr },
#endif
#if IP_REASSEMBLY
    { 0, IP_TMR_INTERVAL,           ip_reass_tmr },
#endif
#if LWIP_AUTOIP
    { 0, AUTOIP_TMR_INTERVAL,       autoip_tmr },
#endif
#if LWIP_IGMP
    { 0, IGMP_TMR_INTERVAL,         igmp_tmr },
#endif
#if LWIP_DNS
    { 0, DNS_TMR_INTERVAL,          dns_tmr },
#endif
#if PPP_SUPPORT
    { 0, PPP_TMR_INTERVAL,          ppp_tmr },
    { 0, CHAT_TMR_INTERVAL,         chat_tmr },
#endif
};

#define NUM_LWIP_TIMERS (sizeof(lwip_timers) / sizeof(lwip_timers[0]))

#define TICKS_TO_MS(_ticks_) \
    ((_ticks_) * (CYGNUM_HAL_RTC_NUMERATOR / CYGNUM_HAL_RTC_DENOMINATOR / 1000000LL))

static cyg_tick_count_t last_ticks;


#if (LWIP_HAVE_LOOPIF && defined(CYGIMP_LWIP_LOOPIF_INSTANCE)) || \
    (LWIP_HAVE_SLIPIF && defined(CYGIMP_LWIP_SLIPIF_INSTANCE))
    
//
// Sets an IP address.
//
static void
set_ip_addr(struct ip_addr *addr, u8_t a, u8_t b, u8_t c, u8_t d)
{
    IP4_ADDR(addr, a, b, c, d);
}

//
// Process an incoming IP or ethernet packet.
//
static err_t
simple_input(struct pbuf *p, struct netif *netif)
{
#if LWIP_ARP
    if (netif->flags & NETIF_FLAG_ETHARP) {
        ethernet_input(p, netif);
    } else
#endif
    {
        ip_input(p, netif);
    }

    return ERR_OK;
}

#endif // (LWIP_HAVE_LOOPIF && defined(CYGIMP_LWIP_LOOPIF_INSTANCE)) ||
       // (LWIP_HAVE_SLIPIF && defined(CYGIMP_LWIP_SLIPIF_INSTANCE))

#ifdef CYGFUN_LWIP_SHOW_NETIF_CONFIG

static void
show_netif_config(struct netif *netif)
{
    diag_printf(
        "%c%c%d: IP: %d.%d.%d.%d Submask: %d.%d.%d.%d Gateway: %d.%d.%d.%d\n",
        netif->name[0], netif->name[1], netif->num,
        ip4_addr1(&netif->ip_addr), ip4_addr2(&netif->ip_addr),
        ip4_addr3(&netif->ip_addr), ip4_addr4(&netif->ip_addr),
        ip4_addr1(&netif->netmask), ip4_addr2(&netif->netmask),
        ip4_addr3(&netif->netmask), ip4_addr4(&netif->netmask),
        ip4_addr1(&netif->gw), ip4_addr2(&netif->gw),
        ip4_addr3(&netif->gw), ip4_addr4(&netif->gw)
    );
}

static void
netif_status_callback(struct netif *netif)
{
    if (netif_is_up(netif))
        show_netif_config(netif);
}

#endif // CYGFUN_LWIP_SHOW_NETIF_CONFIG


//
// Initialize lwIP for simple (single-threaded) mode.
//
void
cyg_lwip_simple_init(void)
{
    static int initialized;

    // Only initialize once
    if (initialized)
        return;
    initialized = 1;

    // Initialize the lwIP stack
    lwip_init();

#if LWIP_HAVE_LOOPIF && defined(CYGIMP_LWIP_LOOPIF_INSTANCE)
    {
        struct ip_addr addr, netmask, gateway;

        // Setup default loopback device instance
        set_ip_addr(&addr, CYGDAT_LWIP_LOOPIF_ADDR);
        set_ip_addr(&netmask, CYGDAT_LWIP_LOOPIF_NETMASK);
        set_ip_addr(&gateway, CYGDAT_LWIP_LOOPIF_GATEWAY);
        netif_add(&loopif, &addr, &netmask, &gateway, NULL,
                  loopif_init, simple_input);
#ifdef CYGDAT_LWIP_LOOPIF_DEFAULT
        netif_set_default(&loopif);
#endif
#ifdef CYGFUN_LWIP_SHOW_NETIF_CONFIG
        netif_set_status_callback(&loopif, netif_status_callback);
#endif
        netif_set_up(&loopif);
    }
#endif

#if LWIP_HAVE_SLIPIF && defined(CYGIMP_LWIP_SLIPIF_INSTANCE)
    {
        struct ip_addr addr, netmask, gateway;

        // Setup default SLIP device instance
        set_ip_addr(&addr, CYGDAT_LWIP_SLIPIF_ADDR);
        set_ip_addr(&netmask, CYGDAT_LWIP_SLIPIF_NETMASK);
        set_ip_addr(&gateway, CYGDAT_LWIP_SLIPIF_GATEWAY);
        netif_add(&slipif, &addr, &netmask, &gateway, NULL,
                  slipif_init, simple_input);
#ifdef CYGDAT_LWIP_SLIPIF_DEFAULT
        netif_set_default(&slipif);
#endif
#ifdef CYGFUN_LWIP_SHOW_NETIF_CONFIG
        netif_set_status_callback(&slipif, netif_status_callback);
#endif
        netif_set_up(&slipif);
    }
#endif

#if PPP_SUPPORT
    {
        // Setup PPP instance
    }
#endif

#ifdef CYGPKG_LWIP_ETH
    {
        cyg_netdevtab_entry_t *t;

        // Initialize network devices
        for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
            if (t->init(t)) {
                t->status = CYG_NETDEVTAB_STATUS_AVAIL;
            } else {
                // Device not [currently] available
                t->status = 0;
            }
        }
    }
#endif
    
    cyg_lwip_simple_restart();
}

//
// Restarts lwIP when not polled for a longer period.
//
void
cyg_lwip_simple_restart(void)
{
    last_ticks = cyg_current_time();
}

//
// Polls the lwIP stack.
//
void
cyg_lwip_simple_poll(void)
{
    cyg_tick_count_t ticks;
    cyg_uint32 delta;
    int i;
    
#if LWIP_HAVE_SLIPIF && defined(CYGIMP_LWIP_SLIPIF_INSTANCE)
    // Poll SLIP device
    slipif_poll(&slipif);
#endif

#if PPP_SUPPORT
    // Poll PPP device
#endif

#ifdef CYGPKG_LWIP_ETH
    {
        cyg_netdevtab_entry_t *t;

        // Poll ethernet devices
        for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
            struct eth_drv_sc *sc = (struct eth_drv_sc *)t->device_instance;
            if (sc->state & ETH_DRV_NEEDS_DELIVERY) {
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)
                cyg_bool was_ctrlc_int;
#endif
                sc->state &= ~ETH_DRV_NEEDS_DELIVERY;
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)
                was_ctrlc_int = HAL_CTRLC_CHECK(sc->funs->int_vector(sc),
                                                (int) sc);
                if (!was_ctrlc_int) // Fall through and run normal code
#endif
                    sc->funs->deliver(sc);
            }
        }

        // Deliver received packets
        while (recv_packet_count > 0) {
            ethernet_input(recv_packet[recv_packet_read].p,
                           recv_packet[recv_packet_read].netif);
            recv_packet_read = (recv_packet_read + 1) % MAX_RECV_PACKETS;
            recv_packet_count--;
        }
    }
#endif

    // Process timers
    ticks = cyg_current_time();
    delta = TICKS_TO_MS(ticks - last_ticks);
    last_ticks = ticks;

    // The following check 'delta > 0' rejects a potential wrap-around in the
    // system ticks counter.
    if (delta > 0) {
        for (i = 0; i < NUM_LWIP_TIMERS; i++) {
            lwip_timers[i].time += delta;
            if (lwip_timers[i].time >= lwip_timers[i].interval) {
                lwip_timers[i].timer_func();
                lwip_timers[i].time -= lwip_timers[i].interval;
            }
        }
    }
}

#ifdef CYGPKG_LWIP_ETH

//
// Called from the network driver to indicate a new netif.
//
void
lwip_eth_drv_new(struct netif *netif)
{
#ifdef CYGFUN_LWIP_SHOW_NETIF_CONFIG
    netif_set_status_callback(netif, netif_status_callback);
#endif
}

//
// Called from network driver to indicate interrupt.
//
void
lwip_eth_drv_dsr(void)
{
}

//
// Called from the network driver when a packet was received.
//
err_t
lwip_eth_drv_input(struct pbuf *p, struct netif *netif)
{
    if (recv_packet_count >= MAX_RECV_PACKETS)
        return ERR_MEM;
    
    recv_packet[recv_packet_write].netif = netif;
    recv_packet[recv_packet_write].p = p;
    recv_packet_write = (recv_packet_write + 1) % MAX_RECV_PACKETS;
    recv_packet_count++;
    
    return ERR_OK;
}

#endif // CYGPKG_LWIP_ETH
