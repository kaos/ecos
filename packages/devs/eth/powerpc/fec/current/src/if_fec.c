//==========================================================================
//
//      dev/if_fec.c
//
//      Fast ethernet device driver for PowerPC MPC8xxT boards
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
// Date:         2001-01-21
// Purpose:      
// Description:  hardware driver for MPC8xxT FEC
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Ethernet device driver for MPC8xx FEC

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_powerpc_fec.h>

#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#endif

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/ppc_regs.h>

#include <netdev.h>
#include <eth_drv.h>

#include "fec.h"

static unsigned char fec_eth_rxbufs[CYGNUM_DEVS_ETH_POWERPC_FEC_RxNUM+1]
                                   [CYGNUM_DEVS_ETH_POWERPC_FEC_BUFSIZE];
static struct fec_bd fec_eth_rxring[CYGNUM_DEVS_ETH_POWERPC_FEC_RxNUM];
static unsigned char fec_eth_txbufs[CYGNUM_DEVS_ETH_POWERPC_FEC_TxNUM+1]
                                   [CYGNUM_DEVS_ETH_POWERPC_FEC_BUFSIZE];
static struct fec_bd fec_eth_txring[CYGNUM_DEVS_ETH_POWERPC_FEC_TxNUM];

static struct fec_eth_info fec_eth0_info;
static unsigned char _default_enaddr[] = { 0x08, 0x00, 0x3E, 0x28, 0x7A, 0xBA};
static unsigned char enaddr[6];
#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <redboot.h>
#include <flash_config.h>
RedBoot_config_option("Network hardware address [MAC]",
                      fec_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#endif
#endif

#ifdef CYGPKG_REDBOOT
#define os_printf printf
#else
#define os_printf diag_printf
#endif

// For fetching the ESA from RedBoot
#include <cyg/hal/hal_if.h>
#ifndef CONFIG_ESA
#define CONFIG_ESA 6
#endif

ETH_DRV_SC(fec_eth0_sc,
           &fec_eth0_info,   // Driver specific data
           "eth0",             // Name for this interface
           fec_eth_start,
           fec_eth_stop,
           fec_eth_control,
           fec_eth_can_send,
           fec_eth_send,
           fec_eth_recv,
           fec_eth_deliver,
           fec_eth_int,
           fec_eth_int_vector);

NETDEVTAB_ENTRY(fec_netdev, 
                "fec_eth", 
                fec_eth_init, 
                &fec_eth0_sc);

#ifdef CYGPKG_NET
static cyg_interrupt fec_eth_interrupt;
static cyg_handle_t  fec_eth_interrupt_handle;
#endif
static void          fec_eth_int(struct eth_drv_sc *data);

#define FEC_ETH_INT CYGNUM_HAL_INTERRUPT_SIU_LVL1

#ifdef CYGPKG_NET
// This ISR is called when the ethernet interrupt occurs
static int
fec_eth_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_drv_interrupt_mask(FEC_ETH_INT);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}
#endif

// Deliver function (ex-DSR) handles the ethernet [logical] processing
static void
fec_eth_deliver(struct eth_drv_sc * sc)
{
    fec_eth_int(sc);
#ifdef CYGPKG_NET
    // Allow interrupts to happen again
    cyg_drv_interrupt_acknowledge(FEC_ETH_INT);
    cyg_drv_interrupt_unmask(FEC_ETH_INT);
#endif
}

//
// PHY unit access (via MII channel)
//
static void
phy_write(int reg, int addr, unsigned short data)
{
    volatile EPPC *eppc = (volatile EPPC *)eppc_base();
    volatile struct fec *fec = (volatile struct fec *)((unsigned char *)eppc + FEC_OFFSET);

    fec->iEvent = iEvent_MII;    
    fec->MiiData = MII_Start | MII_Write | MII_Phy(addr) | MII_Reg(reg) | MII_TA | data;
    while (!(fec->iEvent & iEvent_MII)) ;
}

static unsigned short
phy_read(int reg, int addr)
{
    volatile EPPC *eppc = (volatile EPPC *)eppc_base();
    volatile struct fec *fec = (volatile struct fec *)((unsigned char *)eppc + FEC_OFFSET);

    fec->iEvent = iEvent_MII;    
    fec->MiiData = MII_Start | MII_Read | MII_Phy(addr) | MII_Reg(reg) | MII_TA;
    while (!(fec->iEvent & iEvent_MII)) ;
    return fec->MiiData & 0x0000FFFF;
}

//
// Initialize the interface - performed at system startup
// This function must set up the interface, including arranging to
// handle interrupts, etc, so that it may be "started" cheaply later.
//
static bool 
fec_eth_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    volatile EPPC *eppc = (volatile EPPC *)eppc_base();
    volatile struct fec *fec = (volatile struct fec *)((unsigned char *)eppc + FEC_OFFSET);
    struct fec_bd *rxbd, *txbd;
    unsigned char *RxBUF, *TxBUF;
    unsigned short phy_state = 0;
    int cache_state;
    int i;
    unsigned long proc_rev;
    bool esa_ok;

    // Ensure consistent state between cache and what the FEC sees
    HAL_DCACHE_IS_ENABLED(cache_state);
    HAL_DCACHE_SYNC();
    HAL_DCACHE_DISABLE();

    qi->fec = fec;

#ifdef CYGPKG_NET
    // Set up to handle interrupts
    cyg_drv_interrupt_create(FEC_ETH_INT,
                             CYGARC_SIU_PRIORITY_HIGH,
                             (cyg_addrword_t)sc, //  Data item passed to interrupt handler
                             (cyg_ISR_t *)fec_eth_isr,
                             (cyg_DSR_t *)eth_drv_dsr,
                             &fec_eth_interrupt_handle,
                             &fec_eth_interrupt);
    cyg_drv_interrupt_attach(fec_eth_interrupt_handle);
    cyg_drv_interrupt_acknowledge(FEC_ETH_INT);
    cyg_drv_interrupt_unmask(FEC_ETH_INT);
#endif

    // Shut down ethernet, in case it is already running
    fec->eControl = eControl_RESET;
    eppc->pip_pbdat &= ~0x00004000;  // Reset PHY chip
    CYGACC_CALL_IF_DELAY_US(1000);   // 1ms
    eppc->pip_pbdat |= 0x00004000;   // Enable PHY chip

    fec->iMask  = 0x0000000;  // Disables all interrupts
    fec->iEvent = 0xFFFFFFFF; // Clear all interrupts
    fec->iVector = (1<<29);   // Caution - must match FEC_ETH_INT above

    txbd = fec_eth_txring;
    rxbd = fec_eth_rxring;
    qi->tbase = txbd;
    qi->txbd = txbd;    
    qi->tnext = txbd;
    qi->rbase = rxbd;
    qi->rxbd = rxbd;
    qi->rnext = rxbd;

#define ROUNDUP(b,s) (((unsigned long)(b) + (s-1)) & ~(s-1))
    RxBUF = (unsigned char *)ROUNDUP(&fec_eth_rxbufs[0][0], 32);
    TxBUF = (unsigned char *)ROUNDUP(&fec_eth_txbufs[0][0], 32);

    // setup buffer descriptors
    for (i = 0;  i < CYGNUM_DEVS_ETH_POWERPC_FEC_RxNUM;  i++) {
        rxbd->length = 0;
        rxbd->buffer = RxBUF;
        rxbd->ctrl   = FEC_BD_Rx_Empty;
        RxBUF += CYGNUM_DEVS_ETH_POWERPC_FEC_BUFSIZE;
        rxbd++;
    }
    rxbd--;
    rxbd->ctrl |= FEC_BD_Rx_Wrap;  // Last buffer
    for (i = 0;  i < CYGNUM_DEVS_ETH_POWERPC_FEC_TxNUM;  i++) {
        txbd->length = 0;
        txbd->buffer = TxBUF;
        txbd->ctrl   = 0;
        TxBUF += CYGNUM_DEVS_ETH_POWERPC_FEC_BUFSIZE;
        txbd++;
    }
    txbd--;
    txbd->ctrl |= FEC_BD_Tx_Wrap;  // Last buffer

    // Set up parallel port for connection to ethernet tranceiver
    eppc->pio_pdpar = 0x1FFF;
    CYGARC_MFSPR( proc_rev, CYGARC_REG_PVR );
#define PROC_REVB 0x0020
    if ((proc_rev & 0x0000FFFF) == PROC_REVB) {
        eppc->pio_pddir = 0x1C58;
    } else {
        eppc->pio_pddir = 0x1FFF;
    }

    // Reset interrupts
    fec->iMask  = 0x00000000;  // No interrupts enabled
    fec->iEvent = 0xFFFFFFFF;  // Clear all interrupts

    // Initialize shared PRAM
    fec->RxRing = fec_eth_rxring;
    fec->TxRing = fec_eth_txring;

    // Size of receive buffers
    fec->RxBufSize = CYGNUM_DEVS_ETH_POWERPC_FEC_BUFSIZE;

    // Receiver control
    fec->RxControl = RxControl_MII | RxControl_DRT;
//    fec->RxControl = RxControl_MII | RxControl_LOOP | RxControl_PROM;
    fec->RxHash = IEEE_8023_MAX_FRAME; // Largest possible ethernet frame

    // Transmit control
    fec->TxControl = 0;

    // Use largest possible Tx FIFO
    fec->TxWater = 3;

    // DMA control
    fec->FunCode = ((2<<29) | (2<<27) | (0<<24));

    // MII speed control (50MHz)
    fec->MiiSpeed = 0x14;

    // Group address hash
    fec->hash[0] = 0;
    fec->hash[1] = 0;

    // Device physical address
#ifdef CYGPKG_REDBOOT
    esa_ok = flash_get_config("fec_esa", enaddr, CONFIG_ESA);
#else
    esa_ok = CYGACC_CALL_IF_FLASH_CFG_OP(CYGNUM_CALL_IF_FLASH_CFG_GET,         
                                         "fec_esa", enaddr, CONFIG_ESA);
#endif
    if (!esa_ok) {
        // Can't figure out ESA
        diag_printf("FEC_ETH - Warning! ESA unknown\n");
        memcpy(&enaddr, &_default_enaddr, sizeof(enaddr));
    }
    memcpy((void *)&fec->addr[0], &enaddr[0], sizeof(enaddr));
    // os_printf("FEC ESA = %08x/%08x\n", fec->addr[0], fec->addr[1]);

    // Enable device
    fec->eControl = eControl_EN | eControl_MUX;

    // Enable transceiver (PHY)    
    phy_write(PHY_BMCR, 0, PHY_BMCR_RESET);
    for (i = 0;  i < 10;  i++) {
        phy_state = phy_read(PHY_BMCR, 0);
        if (!(phy_state & PHY_BMCR_RESET)) break;
    }
    if (phy_state & PHY_BMCR_RESET) {
        os_printf("FEC: Can't get PHY unit to reset: %x\n", phy_state);
        return false;
    }
    phy_write(PHY_BMCR, 0, PHY_BMCR_AUTO_NEG);
    // os_printf("MII: %x/%x\n", phy_state, phy_read(PHY_BMCR, 0));

#ifdef CYGPKG_NET
    // Set up for interrupts
    fec->iMask = iEvent_all & ~iEvent_MII;
#endif

    if (cache_state)
        HAL_DCACHE_ENABLE();

    // Initialize upper level driver
    (sc->funs->eth_drv->init)(sc, (unsigned char *)&enaddr);

    return true;
}
 
//
// This function is called to shut down the interface.
//
static void
fec_eth_stop(struct eth_drv_sc *sc)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;

    // Disable the device!
    qi->fec->eControl &= ~eControl_EN;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
fec_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;

    // Enable the device!
    qi->fec->eControl |= eControl_EN;
    qi->fec->RxUpdate = 0x0F0F0F0F;  // Any write tells machine to look for work
}

//
// This function is called for low level "control" operations
//
static int
fec_eth_control(struct eth_drv_sc *sc, unsigned long key,
                  void *data, int length)
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
// This function is called to see if another packet can be sent.
// It should return the number of packets which can be handled.
// Zero should be returned if the interface is busy and can not send any more.
//
static int
fec_eth_can_send(struct eth_drv_sc *sc)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    volatile struct fec_bd *txbd = qi->txbd;

    return ((txbd->ctrl & FEC_BD_Tx_Ready) == 0);
}

//
// This routine is called to send data to the hardware.
static void 
fec_eth_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
               int total_len, unsigned long key)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    volatile struct fec_bd *txbd, *txfirst;
    volatile char *bp;
    int i, txindex, cache_state;

    // Find a free buffer
    txbd = txfirst = qi->txbd;
    while (txbd->ctrl & FEC_BD_Tx_Ready) {
        // This buffer is busy, move to next one
        if (txbd->ctrl & FEC_BD_Tx_Wrap) {
            txbd = qi->tbase;
        } else {
            txbd++;
        }
        if (txbd == txfirst) {
#ifdef CYGPKG_NET
            panic ("No free xmit buffers");
#else
            os_printf("FEC Ethernet: No free xmit buffers\n");
#endif
        }
    }
    // Remember the next buffer to try
    if (txbd->ctrl & FEC_BD_Tx_Wrap) {
        qi->txbd = qi->tbase;
    } else {
        qi->txbd = txbd+1;
    }
    txindex = ((unsigned long)txbd - (unsigned long)qi->tbase) / sizeof(*txbd);
    qi->txkey[txindex] = key;
    // Set up buffer
    txbd->length = total_len;
    bp = txbd->buffer;
    for (i = 0;  i < sg_len;  i++) {
        memcpy((void *)bp, (void *)sg_list[i].buf, sg_list[i].len);
        bp += sg_list[i].len;
    }
    // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_FLUSH(txbd->buffer, txbd->length);  // Make sure no stale data
    }
    // Send it on it's way
    txbd->ctrl |= FEC_BD_Tx_Ready | FEC_BD_Tx_Last | FEC_BD_Tx_TC;
    if (cache_state) {
        HAL_DCACHE_FLUSH(fec_eth_txring, sizeof(fec_eth_txring));  // Make sure no stale data
    }
    qi->fec->TxUpdate = 0x0F0F0F0F;  // Any write tells machine to look for work
}

//
// This function is called when a packet has been received.  It's job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'fec_eth_recv' will be called to actually fetch it from the hardware.
//
static void
fec_eth_RxEvent(struct eth_drv_sc *sc)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    volatile struct fec_bd *rxbd;
    int cache_state;

    // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_FLUSH(fec_eth_rxring, sizeof(fec_eth_rxring));  // Make sure no stale data
    }
    rxbd = qi->rnext;
    while ((rxbd->ctrl & FEC_BD_Rx_Empty) == 0) {
        qi->rxbd = rxbd;  // Save for callback
        (sc->funs->eth_drv->recv)(sc, rxbd->length);
        rxbd->ctrl |= FEC_BD_Rx_Empty;
        if (rxbd->ctrl & FEC_BD_Rx_Wrap) {
            rxbd = qi->rbase;
        } else {
            rxbd++;
        }
    }
    // Remember where we left off
    qi->rnext = (struct fec_bd *)rxbd;
    if (cache_state) {
        HAL_DCACHE_FLUSH(fec_eth_rxring, sizeof(fec_eth_rxring));  // Make sure no stale data
    }
    qi->fec->RxUpdate = 0x0F0F0F0F;  // Any write tells machine to look for work
}

//
// This function is called as a result of the "eth_drv_recv()" call above.
// It's job is to actually fetch data for a packet from the hardware once
// memory buffers have been allocated for the packet.  Note that the buffers
// may come in pieces, using a scatter-gather list.  This allows for more
// efficient processing in the upper layers of the stack.
//
static void
fec_eth_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    unsigned char *bp;
    int i, cache_state;

    bp = (unsigned char *)qi->rxbd->buffer;
    // Note: the MBX860 does not seem to snoop/invalidate the data cache properly!
    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_INVALIDATE(qi->rxbd->buffer, qi->rxbd->length);  // Make sure no stale data
    }
    for (i = 0;  i < sg_len;  i++) {
        if (sg_list[i].buf != 0) {
            memcpy((void *)sg_list[i].buf, bp, sg_list[i].len);
            bp += sg_list[i].len;
        }
    }
}

static void
fec_eth_TxEvent(struct eth_drv_sc *sc, int stat)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    volatile struct fec_bd *txbd;
    int txindex, cache_state;

    HAL_DCACHE_IS_ENABLED(cache_state);
    if (cache_state) {
        HAL_DCACHE_FLUSH(fec_eth_txring, sizeof(fec_eth_txring));  // Make sure no stale data
    }
    txbd = qi->tnext;
    // Note: TC field is used to indicate the buffer has/had data in it
    while ((txbd->ctrl & (FEC_BD_Tx_Ready|FEC_BD_Tx_TC)) == FEC_BD_Tx_TC) {
        txindex = ((unsigned long)txbd - (unsigned long)qi->tbase) / sizeof(*txbd);
        (sc->funs->eth_drv->tx_done)(sc, qi->txkey[txindex], 0);
        txbd->ctrl &= ~FEC_BD_Tx_TC;
        if (txbd->ctrl & FEC_BD_Tx_Wrap) {
            txbd = qi->tbase;
        } else {
            txbd++;
        }
    }
    // Remember where we left off
    qi->tnext = (struct fec_bd *)txbd;
    if (cache_state) {
        HAL_DCACHE_FLUSH(fec_eth_txring, sizeof(fec_eth_txring));  // Make sure no stale data
    }
}

//
// Interrupt processing
//
static void          
fec_eth_int(struct eth_drv_sc *sc)
{
    struct fec_eth_info *qi = (struct fec_eth_info *)sc->driver_private;
    unsigned long event;

    while ((event = qi->fec->iEvent) != 0) {
        if ((event & iEvent_TFINT) != 0) {
            fec_eth_TxEvent(sc, event);
        }
        if ((event & iEvent_RFINT) != 0) {
            fec_eth_RxEvent(sc);
        }
        qi->fec->iEvent = event;  // Reset the bits we handled
    }
}

//
// Interrupt vector
//
static int          
fec_eth_int_vector(struct eth_drv_sc *sc)
{
    return (FEC_ETH_INT);
}

