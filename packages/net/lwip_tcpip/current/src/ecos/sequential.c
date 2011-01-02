//==========================================================================
//
//      sequential.c
//
//      lwIP sequential mode (multi-threaded) support.
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
// Date:         2008-12-02
// Purpose:
// Description:  lwIP sequential mode (multi-threaded) support.
//
//####DESCRIPTIONEND####
//
//==========================================================================

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
#include "lwip/tcpip.h"

#include "netif/loopif.h"
#include "netif/slipif.h"
#include "netif/etharp.h"
#include "netif/ppp/ppp.h"

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

static sys_thread_t eth_thread_handle;
static cyg_sem_t eth_thread_sem;

static void eth_thread(void *data);

#endif

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
// Called when initialization of the tcpip thread is done.
//
static void
tcpip_init_done(void *arg)
{
    cyg_sem_t *sem = arg;
    
#if LWIP_HAVE_LOOPIF && defined(CYGIMP_LWIP_LOOPIF_INSTANCE)
    {
        struct ip_addr addr, netmask, gateway;

        // Setup default loopback device instance
        set_ip_addr(&addr, CYGDAT_LWIP_LOOPIF_ADDR);
        set_ip_addr(&netmask, CYGDAT_LWIP_LOOPIF_NETMASK);
        set_ip_addr(&gateway, CYGDAT_LWIP_LOOPIF_GATEWAY);
        netif_add(&loopif, &addr, &netmask, &gateway, NULL,
                  loopif_init, tcpip_input);
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
                  slipif_init, tcpip_input);
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
        
        // Initialize ethernet delivery thread
        cyg_semaphore_init(&eth_thread_sem, 0);
        eth_thread_handle = sys_thread_new(ETH_THREAD_NAME,
                                           eth_thread,
                                           NULL,
                                           ETH_THREAD_STACKSIZE,
                                           ETH_THREAD_PRIO);
        
        // Initialize network devices
        for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
            if (t->init(t)) {
                t->status = CYG_NETDEVTAB_STATUS_AVAIL;
#ifdef CYGFUN_LWIP_SHOW_NETIF_CONFIG
                {
                    struct eth_drv_sc* sc = (struct eth_drv_sc*) (t->device_instance);
                    struct netif *netif = &sc->sc_arpcom.ac_if;
                    netif_set_status_callback(netif, netif_status_callback);
                }
#endif
            } else {
                // Device not [currently] available
                t->status = 0;
            }
        }
    }
#endif

    // Notify initialization thread
    cyg_semaphore_post(sem);
}

//
// Initialize lwIP for simple (single-threaded) mode.
//
void
cyg_lwip_sequential_init(void)
{
    static int initialized;
    cyg_sem_t sem;

    // Only initialize once
    if (initialized)
        return;
    initialized = 1;
    
    // Initialize the lwIP stack
    cyg_semaphore_init(&sem, 0);
    tcpip_init(tcpip_init_done, (void *) &sem);
    
    // Wait until initialized
    cyg_semaphore_wait(&sem);
    cyg_semaphore_destroy(&sem);
}

#ifdef CYGPKG_LWIP_ETH

static void
eth_thread(void *data)
{
    cyg_netdevtab_entry_t *t;
    
    while (1) {
        cyg_semaphore_wait(&eth_thread_sem);
        
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
    }
}

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
    cyg_semaphore_post(&eth_thread_sem);
}

//
// Called from the network driver when a packet was received.
//
err_t
lwip_eth_drv_input(struct pbuf *p, struct netif *netif)
{
    return tcpip_input(p, netif);
}

#endif // CYGPKG_LWIP_ETH
