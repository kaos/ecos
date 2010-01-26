//==========================================================================
//
//      src/lwip/eth_drv.c
//
//      Hardware independent networking support for lwIP
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008, 2009 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2008-12-09
// Purpose:      Hardware independent networking support for lwIP.
// Description:  Based on the standalone driver for RedBoot.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/io_eth_drivers.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>
#include <cyg/io/eth/eth_drv.h>
#include <cyg/io/eth/netdev.h>
#include <string.h>

#include <cyg/hal/hal_tables.h>
#include <cyg/kernel/kapi.h>

#include <lwip/opt.h>
#include <lwip/ip.h>
#include <lwip/mem.h>
#include <lwip/pbuf.h>
#include <lwip/sys.h>
#include <lwip/dhcp.h>
#include <netif/etharp.h>

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT

#include <cyg/hal/hal_if.h>

// Use with care!  Local variable defined!
#define START_CONSOLE()                                                                 \
{   /* NEW BLOCK */                                                                     \
    int _cur_console =                                                                  \
        CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);      \
    {                                                                                   \
        int i;                                                                          \
        if ( CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,                 \
                                          "info_console_force", &i,                     \
                                          CYGNUM_FLASH_CFG_TYPE_CONFIG_BOOL ) ) {       \
            if ( i ) {                                                                  \
                if ( CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,         \
                                                  "info_console_number", &i,            \
                                                  CYGNUM_FLASH_CFG_TYPE_CONFIG_INT ) ){ \
                    /* Then i is the console to force it to: */                         \
                    CYGACC_CALL_IF_SET_CONSOLE_COMM( i );                               \
                }                                                                       \
            }                                                                           \
        }                                                                               \
    }

#define END_CONSOLE()                                   \
    CYGACC_CALL_IF_SET_CONSOLE_COMM(_cur_console);      \
}   /* END BLOCK */

#else
#define START_CONSOLE()
#define END_CONSOLE()
#endif
// ------------------------------------------------------------------------

//
// Control whether any special locking needs to take place if we intend to
// cooperate with a ROM monitor (e.g. RedBoot) using this hardware.
//
#if defined(CYGSEM_HAL_USE_ROM_MONITOR) && \
    defined(CYGSEM_HAL_VIRTUAL_VECTOR_DIAG) && \
   !defined(CYGSEM_HAL_VIRTUAL_VECTOR_CLAIM_COMMS)

// Indicate that special locking precautions are warranted.
#define _LOCK_WITH_ROM_MONITOR

// This defines the [well known] channel that RedBoot will use when it is
// using the network hardware for the debug channel.
#define RedBoot_TCP_CHANNEL CYGNUM_HAL_VIRTUAL_VECTOR_COMM_CHANNELS

// Define this if you ever need to call 'diag_printf()' from interrupt level
// code (ISR) and the debug channel might be using the network hardware. If
// this is not the case, then disabling interrupts here is over-kill.
//#define _LOCK_USING_INTERRUPTS
#endif

// Static device configurations

#include "eth_conf.inl"

// Interfaces exported to drivers

static void eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr);
static void eth_drv_recv(struct eth_drv_sc *sc, int total_len);
static void eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRWORD key, int status);

struct eth_drv_funs eth_drv_funs = {eth_drv_init, eth_drv_recv, eth_drv_tx_done};

static void eth_drv_send(struct netif *netif, struct pbuf *p);

#ifdef CYGDBG_IO_ETH_DRIVERS_DEBUG
int cyg_io_eth_net_debug = CYGDBG_IO_ETH_DRIVERS_DEBUG_VERBOSITY;
// Usually just the header is enough, the body slows things too much.
#define DIAG_DUMP_BUF_HDR( a, b ) if (0 < cyg_io_eth_net_debug) diag_dump_buf( (a), (b) )
#define DIAG_DUMP_BUF_BDY( a, b ) if (1 < cyg_io_eth_net_debug) diag_dump_buf( (a), (b) )
#else
#define DIAG_DUMP_BUF_HDR( a, b )
#define DIAG_DUMP_BUF_BDY( a, b )
#endif

#define MAX_ETH_MSG 1540

// Interface to lwIP glue code

extern void lwip_eth_drv_new(struct netif *netif);
extern void lwip_eth_drv_dsr(void);
extern err_t lwip_eth_drv_input(struct pbuf *p, struct netif *netif);



//
// Called by lwIP to do the actual transmission of a packet. The packet passed
// to this function may be chained. This function passes the data to the
// hardware driver
//
static err_t eth_netif_linkoutput(struct netif *netif, struct pbuf *p)
{
    eth_drv_send(netif, p);
    
    return ERR_OK;
}

//
// Called by lwIP when an IP packet should be sent.
//
static err_t eth_netif_output(struct netif *netif, struct pbuf *p,
                          struct ip_addr *ipaddr)
{
    // Resolve hardware address, then send (or queue) packet
    return etharp_output(netif, p, ipaddr);
}

//
// Called by lwIP to init the netif.
//
static err_t eth_netif_init(struct netif *netif)
{
    netif->name[0] = 'e';
    netif->name[1] = 't';
    netif->hwaddr_len = 6;
    netif->output = eth_netif_output;
    netif->linkoutput = eth_netif_linkoutput;
    netif->mtu = 1500;

    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;
    
    return ERR_OK;
}

//
// This function is called during system initialization to register a
// network interface with the system.
//
static void
eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr)
{
    struct netif *netif = &sc->sc_arpcom.ac_if;
    struct eth_conf *conf;
    struct ip_addr addr, netmask, gateway;
    
#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
    cyg_semaphore_init(&sc->sc_arpcom.send_sem, 1);
#endif

    // enaddr == 0 -> hardware init was incomplete (no ESA)
    if (enaddr == 0)
        return;
    
    netif->state = sc;

    // Configure the IP address
    conf = next_eth_conf();
    if (conf && !conf->dhcp) {
        IP4_ADDR(&addr,    conf->addr[0], conf->addr[1],
                           conf->addr[2], conf->addr[3]);
        IP4_ADDR(&netmask, conf->netmask[0], conf->netmask[1],
                           conf->netmask[2], conf->netmask[3]);
        IP4_ADDR(&gateway, conf->gateway[0], conf->gateway[1],
                           conf->gateway[2], conf->gateway[3]);
    } else {
        IP4_ADDR(&addr,    0, 0, 0, 0);
        IP4_ADDR(&netmask, 0, 0, 0, 0);
        IP4_ADDR(&gateway, 0, 0, 0, 0);
    }
    
    netif_add(netif, &addr, &netmask, &gateway, netif->state,
              eth_netif_init, lwip_eth_drv_input);
    
    lwip_eth_drv_new(netif);
    
    if (conf && conf->def)
        netif_set_default(netif);
    
    // Set up hardware address
    memcpy(netif->hwaddr, enaddr, ETHER_ADDR_LEN);
    (sc->funs->start)(sc, (unsigned char *) &netif->hwaddr, 0);
    
    if (conf && !conf->dhcp)
        netif_set_up(netif);
    
#if LWIP_DHCP
    // Start DHCP if configured
    if (conf && conf->dhcp) {
        dhcp_start(netif);
    }
#endif
}

//
// This function is called from the hardware driver when an output operation
// has completed - i.e. the packet has been sent.
//
static void
eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRWORD key, int status)
{
#ifdef CYGFUN_LWIP_MODE_SIMPLE
    // Stop polling
    cyg_bool *done = (cyg_bool *) key;
    *done = true;
#endif
#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
    // Wake-up the lwip-thread
    cyg_semaphore_post(&sc->sc_arpcom.send_sem);
#endif    
}

//
// Send a packet of data to the hardware.
//
static void
eth_drv_send(struct netif *netif, struct pbuf *p)
{
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    struct eth_drv_sc *sc = netif->state;
    int sg_len = 0;
    struct pbuf *q;
#ifdef _LOCK_WITH_ROM_MONITOR
#ifdef _LOCK_USING_INTERRUPTS
    cyg_uint32 ints;
#endif
    bool need_lock = false;
    int debug_chan;
#endif
#ifdef CYGFUN_LWIP_MODE_SIMPLE
    cyg_bool done = false;
    int wait_cycles = 100;
#endif
    
#ifdef CYGFUN_LWIP_MODE_SIMPLE
    // Wait until we can send
    while (!(sc->funs->can_send)(sc)) {
        // Give driver a chance to service hardware
        (sc->funs->poll)(sc);
        if (--wait_cycles <= 0) {
            START_CONSOLE();
            diag_printf("cannot send packet\n");
            END_CONSOLE();
            return;
        }
    }
#endif // CYGFUN_LWIP_MODE_SIMPLE
    
#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
    // Wait until we can send
    if ((sc->funs->can_send)(sc) < 1)
        cyg_semaphore_wait(&sc->sc_arpcom.send_sem);
    if ((sc->funs->can_send)(sc) < 1)
        CYG_FAIL("cannot send packet");
#endif // CYGFUN_LWIP_MODE_SEQUENTIAL
    
    // Create scatter list
    for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
        sg_list[sg_len].buf = (CYG_ADDRESS) q->payload;
        sg_list[sg_len++].len = q->len;
    }
    
#ifdef _LOCK_WITH_ROM_MONITOR
    // Firm lock on this portion of the driver.  Since we are about to
    // start messing with the actual hardware, it is imperative that the
    // current thread not loose control of the CPU at this time.  Otherwise,
    // the hardware could be left in an unusable state.  This caution is
    // only warranted if there is a possibility of some other thread trying
    // to use the hardware simultaneously.  The network stack would prevent
    // this implicitly since all accesses are controlled by the "splX()"
    // locks, but if there is a ROM monitor, such as RedBoot, also using
    // the hardware, all bets are off.

    // Note: these operations can be avoided if it were well known that
    // RedBoot was not using the network hardware for diagnostic I/O.  This
    // can be inferred by checking which I/O channel RedBoot is currently
    // hooked to.
    debug_chan = CYGACC_CALL_IF_SET_DEBUG_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);
    if (debug_chan == RedBoot_TCP_CHANNEL) {
        need_lock = true;
#ifdef _LOCK_USING_INTERRUPTS
        HAL_DISABLE_INTERRUPTS(ints);
#endif
        cyg_drv_dsr_lock();
    }
#endif // _LOCK_WITH_ROM_MONITOR

#ifdef CYGFUN_LWIP_MODE_SIMPLE
    // Tell hardware to send this packet
    if (sg_len)
        (sc->funs->send)(sc, sg_list, sg_len, p->tot_len, (CYG_ADDRWORD) &done);
    // Wait until packet has been sent
    wait_cycles = 100;
    while (!done) {
        (sc->funs->poll)(sc);
        if (--wait_cycles <= 0) {
            START_CONSOLE();
            diag_printf("packet failed to send\n");
            END_CONSOLE();
            break;
        }
    }
#endif // CYGFUN_LWIP_MODE_SIMPLE

#ifdef CYGFUN_LWIP_MODE_SEQUENTIAL
    // Tell hardware to send this packet
    if (sg_len)
        (sc->funs->send)(sc, sg_list, sg_len, p->tot_len, 0);
#endif // CYGFUN_LWIP_MODE_SEQUENTIAL

#ifdef _LOCK_WITH_ROM_MONITOR
    // Unlock the driver & hardware.  It can once again be safely shared.
    if (need_lock) {
        cyg_drv_dsr_unlock();
#ifdef _LOCK_USING_INTERRUPTS
        HAL_RESTORE_INTERRUPTS(ints);
#endif
    }
#endif // _LOCK_WITH_ROM_MONITOR
#undef _LOCK_WITH_ROM_MONITOR
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
    struct netif *netif = &sc->sc_arpcom.ac_if;
    struct pbuf *p, *q;
    int sg_len = 0;
    
    CYG_ASSERT(total_len != 0, "total_len is zero!");
    CYG_ASSERT(total_len >= 0, "total_len is negative!");
    
    CYGARC_HAL_SAVE_GP();

    if ((total_len > MAX_ETH_MSG) || (total_len < 0))
        total_len = MAX_ETH_MSG;

    // Allocate buffer to store received packet
    p = pbuf_alloc(PBUF_RAW, total_len, PBUF_POOL);
    if (p == NULL) {
        START_CONSOLE();
        diag_printf("cannot allocate pbuf to receive packet\n");
        END_CONSOLE();
        return;
    }

    // Create scatter list
    for (q = p; q != NULL && sg_len < MAX_ETH_DRV_SG; q = q->next) {
        sg_list[sg_len].buf = (CYG_ADDRESS) q->payload;
        sg_list[sg_len++].len = q->len;
    }
    
    // Get buffers from hardware
    (sc->funs->recv)(sc, sg_list, sg_len);
    
    // Pass received packet to the interface
    netif->input(p, netif);

    CYGARC_HAL_RESTORE_GP();
}

//
// DSR called from the low level driver.
//
void
eth_drv_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *) data;

    sc->state |= ETH_DRV_NEEDS_DELIVERY;
    lwip_eth_drv_dsr();
}
