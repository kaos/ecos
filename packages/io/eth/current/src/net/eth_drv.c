//==========================================================================
//
//      ecos/eth_drv.c
//
//      Hardware independent ethernet driver
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
// Date:         2000-01-10
// Purpose:      Hardware independent ethernet driver
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// High-level ethernet driver

#include <sys/param.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/mbuf.h>
#include <sys/socket.h>

#include <net/if.h>
#include <net/if_dl.h>
#include <net/if_types.h>
#include <net/netisr.h>

#ifdef INET
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/in_var.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>
#endif

#if NBPFILTER > 0
#include <net/bpf.h>
#include <net/bpfdesc.h>
#endif

#include <cyg/infra/cyg_ass.h>
#include <cyg/hal/drv_api.h>
#include <pkgconf/hal.h>
#include <cyg/hal/hal_if.h>
#include <eth_drv.h>
#include <netdev.h>

static int  eth_drv_ioctl(struct ifnet *, u_long, caddr_t);
static void eth_drv_send(struct ifnet *);

extern int net_debug;  // FIXME

// Interfaces exported to drivers

static void eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr);
static void eth_drv_recv(struct eth_drv_sc *sc, int total_len);
static void eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRESS key, int status);

struct eth_drv_funs eth_drv_funs = {eth_drv_init, eth_drv_recv, eth_drv_tx_done};

//
// This function is called during system initialization to register a
// network interface with the system.
//
static void
eth_drv_init(struct eth_drv_sc *sc, unsigned char *enaddr)
{
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;

    // Set up hardware address
    bcopy(enaddr, &sc->sc_arpcom.ac_enaddr, ETHER_ADDR_LEN);

    // Initialize ifnet structure
    bcopy((void *)sc->dev_name, ifp->if_xname, IFNAMSIZ);
    ifp->if_softc = sc;
    ifp->if_start = eth_drv_send;
    ifp->if_ioctl = eth_drv_ioctl;
    ifp->if_flags =
        IFF_BROADCAST | IFF_SIMPLEX | IFF_NOTRAILERS | IFF_MULTICAST;
    sc->state = 0;

    // Attach the interface
    if_attach(ifp);
    ether_ifattach(ifp);

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_DIAG
// Set up interfaces so debug environment can share this device
    {
        void *dbg = CYGACC_CALL_IF_DBG_DATA();
        if (!dbg) {
            CYGACC_CALL_IF_DBG_DATA_SET((void *)sc);
        }
    }
#endif
}

//
// This [internal] function will be called to stop activity on an interface.
//
static void
eth_drv_stop(struct eth_drv_sc *sc)
{
    (sc->funs->stop)(sc);
    sc->state &= ~ETH_DRV_STATE_ACTIVE;
}

//
// This [internal] function will be called to start activity on an interface.
//
static void
eth_drv_start(struct eth_drv_sc *sc)
{
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;
    // Perform any hardware initialization
    (sc->funs->start)(sc, (unsigned char *)&sc->sc_arpcom.ac_enaddr, 0);
    // Set 'running' flag, and clear output active flag.
    ifp->if_flags |= IFF_RUNNING;
    ifp->if_flags &= ~IFF_OACTIVE;
    sc->state |= ETH_DRV_STATE_ACTIVE;
    eth_drv_send(ifp);  // Try and start up transmit
}

//
// This function supports "I/O control" operations on an interface.
//
static int  
eth_drv_ioctl(struct ifnet *ifp, u_long cmd, caddr_t data)
{
    struct eth_drv_sc *sc = ifp->if_softc;
    struct ifaddr *ifa = (struct ifaddr *) data;
    struct ifreq *ifr = (struct ifreq *)data;
    int     s, error = 0;

    s = splnet();

    if ((error = ether_ioctl(ifp, &sc->sc_arpcom, cmd, data)) > 0) {
        splx(s);
        return error;
    }

    switch (cmd) {

    case SIOCSIFADDR:
        ifp->if_flags |= IFF_UP;

        switch (ifa->ifa_addr->sa_family) {
#ifdef INET
        case AF_INET:
            eth_drv_start(sc);
            arp_ifinit(&sc->sc_arpcom, ifa);
            break;
#endif
        default:
            eth_drv_start(sc);
            break;
        }
        break;

    case SIOCGIFHWADDR:
        // Get hardware (MAC) address
        ifr->ifr_hwaddr.sa_family = AF_INET;
        bcopy(&sc->sc_arpcom.ac_enaddr, &ifr->ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
        break;

    case SIOCSIFHWADDR:
        // Set hardware (MAC) address
        bcopy(&ifr->ifr_hwaddr.sa_data, &sc->sc_arpcom.ac_enaddr, ETHER_ADDR_LEN);
        if ((sc->funs->control)(sc, ETH_DRV_SET_MAC_ADDRESS,
                                &sc->sc_arpcom.ac_enaddr, ETHER_ADDR_LEN)) {
            error = EINVAL;
        }
        break;

#ifdef SIOCGIFSTATS
    case SIOCGIFSTATS:
#ifdef SIOCGIFSTATSUD
    case SIOCGIFSTATSUD:
#endif
        // Get interface statistics:
        if ((sc->funs->control)(sc, (cmd == SIOCGIFSTATS)
                                ? ETH_DRV_GET_IF_STATS
                                : ETH_DRV_GET_IF_STATS_UD,
                                data, 0 ) ) {
            error = EINVAL;
        }
        break;
#endif // SIOCGIFSTATS

    case SIOCSIFFLAGS:
        if ((ifp->if_flags & IFF_UP) == 0 &&
            (ifp->if_flags & IFF_RUNNING) != 0) {
            /*
             * If interface is marked down and it is running, then
             * stop it.
             */
            eth_drv_stop(sc);
            ifp->if_flags &= ~IFF_RUNNING;
        } else
            if ((ifp->if_flags & IFF_UP) != 0 &&
                (ifp->if_flags & IFF_RUNNING) == 0) {
                /*
                 * If interface is marked up and it is stopped, then
                 * start it.
                 */
                eth_drv_start(sc);
            } else {
                /*
                 * Reset the interface to pick up changes in any other
                 * flags that affect hardware registers.
                 */
                eth_drv_stop(sc);
                eth_drv_start(sc);
            }
        break;

#if 0
    case SIOCADDMULTI:
    case SIOCDELMULTI:
        /* Update our multicast list. */
        error = (cmd == SIOCADDMULTI) ?
            ether_addmulti(ifr, &sc->sc_arpcom) :
                ether_delmulti(ifr, &sc->sc_arpcom);

        if (error == ENETRESET) {
            /*
             * Multicast list has changed; set the hardware filter
             * accordingly.
             */
            eth_drv_stop(sc);	/* XXX for ds_setmcaf? */
            eth_drv_start(sc);
            error = 0;
        }
        break;
#endif

    default:
        error = EINVAL;
        break;
    }

    splx(s);
    return (error);
}

//
// This routine is called to start transmitting if there is data
// available.
//
static void 
eth_drv_send(struct ifnet *ifp)
{
    struct eth_drv_sc *sc = ifp->if_softc;
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    int sg_len;
    struct mbuf *m0, *m;
    int len, total_len;
    unsigned char *data;

    // This is now only called from network threads, so no guarding is
    // required; locking is in place via the splfoo() mechanism already.

    if ((ifp->if_flags & IFF_RUNNING) != IFF_RUNNING) {
         return;
    }

    while ((sc->funs->can_send)(sc) > 0) {
        IF_DEQUEUE(&ifp->if_snd, m0);
        if (m0 == 0) {
            break;
        }

        if (net_debug)
            diag_printf("Sending %d bytes\n", m0->m_pkthdr.len);

        /* We need to use m->m_pkthdr.len, so require the header */
        if ((m0->m_flags & M_PKTHDR) == 0)
            panic("eth_drv_send: no header mbuf");

#if NBPFILTER > 0
        /* Tap off here if there is a BPF listener. */
        if (ifp->if_bpf)
            bpf_mtap(ifp->if_bpf, m0);
#endif

        // Extract data pointers (don't actually move data here)
        sg_len = 0;  total_len = 0;
        for (m = m0; m ; m = m->m_next) {
            data = mtod(m, u_char *);
            len = m->m_len;
            total_len += len;
            sg_list[sg_len].buf = (CYG_ADDRESS)data;
            sg_list[sg_len].len = len;
            if ( len )
                sg_len++;
            if (net_debug) {
                diag_printf("xmit %d bytes at %x sg[%d]\n", len, data, sg_len);
                if ( 1 & net_debug )
                    diag_dump_buf(data, len);
            }
            if ( MAX_ETH_DRV_SG < sg_len ) {
                diag_printf("too many mbufs to tx, %d > %d\n", sg_len, MAX_ETH_DRV_SG );
                sg_len = 0;
                break; // drop it on the floor
            }
        }

        // Tell hardware to send this packet
        if ( sg_len )
            (sc->funs->send)(sc, sg_list, sg_len, total_len, (unsigned long)m0);
    }
}

//
// This function is called from the hardware driver when an output operation
// has completed - i.e. the packet has been sent.
//
static struct mbuf *mbuf_key;

static void
eth_drv_tx_done(struct eth_drv_sc *sc, CYG_ADDRESS key, int status)
{
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;
    struct mbuf *m0 = (struct mbuf *)key;
    // Check for errors here (via 'status')
    // Done with packet
    mbuf_key = m0;
    m_freem(m0);
    // Start another if possible
    eth_drv_send(ifp);
}

//
// This function is called from a hardware driver to indicate that an input
// packet has arrived.  The routine will set up appropriate network resources
// (mbuf's) to hold the data and call back into the driver to retrieve the data.
//
static void
eth_drv_recv(struct eth_drv_sc *sc, int total_len)
{
    struct ifnet *ifp = &sc->sc_arpcom.ac_if;
    struct ether_header _eh, *eh=&_eh;
    struct mbuf *top, **mp, *m;
    int i, mlen;
    unsigned char *data;
    struct eth_drv_sg sg_list[MAX_ETH_DRV_SG];
    int sg_len;

    if ((ifp->if_flags & IFF_RUNNING) != IFF_RUNNING) {
        return;  // Interface not up, ignore this request
    }

    CYG_ASSERT( 0 != total_len, "total_len is zero!" );
    CYG_ASSERT( 0 <= total_len, "total_len is negative!" );
    CYG_ASSERT( sizeof( struct ether_header ) <= total_len,
                "No ether header here!" );

    if ( total_len < sizeof( struct ether_header ) )
        // Our arithmetic below would go wrong
        return;

    /* Pull packet off interface. */
    MGETHDR(m, M_DONTWAIT, MT_DATA);
    if (m == 0) {
        diag_printf("warning: eth_recv out of MBUFs\n");
    }

    // Set up buffers
    // Unload ethernet header separately so IP/UDP/TCP headers are aligned
    sg_list[0].buf = (CYG_ADDRESS)eh;
    sg_list[0].len = sizeof(*eh);
    sg_len = 1;

    // Compute total length (minus ethernet header)
    total_len -= sizeof(*eh);

    top = 0;
    mlen = MHLEN;
    mp = &top;

    if (m) {
        m->m_pkthdr.rcvif = ifp;
        m->m_pkthdr.len = total_len;
    } else {
        sg_list[sg_len].buf = (CYG_ADDRESS)0;
        sg_list[sg_len].len = min(total_len, MCLBYTES);
        sg_len++;
        total_len = 0;
    }

    while (total_len > 0) {
        if (top) {
            MGET(m, M_DONTWAIT, MT_DATA);
            if (m == 0) {
                m_freem(top);
                panic("out of MBUFs [2]");
            }
            mlen = MLEN;
        }
        if (total_len >= MINCLSIZE) {
            MCLGET(m, M_DONTWAIT);
            if ((m->m_flags & M_EXT) == 0) {
                m_freem(top);
                diag_printf("warning: eth_recv out of MBUFs\n");
                sg_list[sg_len].buf = (CYG_ADDRESS)0;
                sg_list[sg_len].len = min(total_len, MCLBYTES);
                sg_len++;
                top = 0;
                break;
            }
            mlen = MCLBYTES;
        }
        m->m_len = mlen = min(total_len, mlen);
        total_len -= mlen;
        data = mtod(m, caddr_t);
        sg_list[sg_len].buf = (CYG_ADDRESS)data;
        sg_list[sg_len].len = mlen;
        sg_len++;
        *mp = m;
        mp = &m->m_next;
    }

    // Ask hardware to unload buffers
    (sc->funs->recv)(sc, sg_list, sg_len);

    if (net_debug) {
        for (i = 0;  i < sg_len;  i++) {
            if (sg_list[i].buf) {
                diag_printf("rx %d bytes at %x sg[%d]\n", sg_list[i].len, sg_list[i].buf, i);
                if ( 1 & net_debug )
                    diag_dump_buf((void *)sg_list[i].buf, sg_list[i].len);
            }
        }
    }

    m = top;
    if (m == 0) {
        ifp->if_ierrors++;
        return;
    }

    ifp->if_ipackets++;


#if NBPFILTER > 0
#error FIXME - Need mbuf with ethernet header attached
    /*
     * Check if there's a BPF listener on this interface.
     * If so, hand off the raw packet to bpf.
     */
    if (ifp->if_bpf)
        bpf_mtap(ifp->if_bpf, m);
#endif

    // Push data into protocol stacks
    ether_input(ifp, eh, m);
}


// ------------------------------------------------------------------------
// DSR to schedule network delivery thread

extern void ecos_synch_eth_drv_dsr(void); // from ecos/timeout.c in net stack

void
eth_drv_dsr(cyg_vector_t vector,
            cyg_ucount32 count,
            cyg_addrword_t data)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)data;

    sc->state |= ETH_DRV_NEEDS_DELIVERY;

    ecos_synch_eth_drv_dsr(); // [request] run delivery function for this dev
}

// This is called from the delivery thread, to do just that:
void eth_drv_run_deliveries( void )
{
    cyg_netdevtab_entry_t *t;
    for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
        struct eth_drv_sc *sc = (struct eth_drv_sc *)t->device_instance;
        int state = sc->state;
        sc->state &=~ETH_DRV_NEEDS_DELIVERY;
        if ( ETH_DRV_NEEDS_DELIVERY & state ) {
            (*sc->funs->deliver)(sc);
        }
    }
}


// ------------------------------------------------------------------------





#ifdef CYGPKG_IO_PCMCIA
// Lookup a 'netdev' entry, assuming that it is an ethernet device.
cyg_netdevtab_entry_t * 
eth_drv_netdev(char *name)
{
    cyg_netdevtab_entry_t *t;
    struct eth_drv_sc *sc;
    for (t = &__NETDEVTAB__[0]; t != &__NETDEVTAB_END__; t++) {
        sc = (struct eth_drv_sc *)t->device_instance;
        if (strcmp(sc->dev_name, name) == 0) {
            return t;
        }
    }
    return (cyg_netdevtab_entry_t *)NULL;
}
#endif // CYGPKG_IO_PCMCIA

// EOF eth_drv.c
