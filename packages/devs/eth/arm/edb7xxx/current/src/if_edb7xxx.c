//==========================================================================
//
//      dev/if_edb7xxx.c
//
//      Ethernet device driver for Cirrus Logic EDB7xxx using CS8900
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
// Contributors: gthomas
// Date:         2000-01-10
// Purpose:      
// Description:  hardware driver for CS8900 ethernet
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

// Ethernet device driver for Cirrus Logic EDB7xxx
// Based on CS8900A

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_edb7xxx.h>
#ifdef CYGPKG_NET
#include <pkgconf/net.h>
#include <cyg/kernel/kapi.h>
#endif
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <netdev.h>
#include <eth_drv.h>

#ifndef CYGSEM_ARM_EDB7XXX_SET_ESA
#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <redboot.h>
#include <flash_config.h>
RedBoot_config_option("Network hardware address [MAC]",
                      edb7xxx_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#endif
#endif
#endif

#define INTS_DONT_WORK
#undef  INTS_DONT_WORK

#ifdef INTS_DONT_WORK
#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_MINIMUM
static char cs8900_fake_int_stack[STACK_SIZE];
static cyg_thread cs8900_fake_int_thread_data;
static cyg_handle_t cs8900_fake_int_thread_handle;
static void cs8900_fake_int(cyg_addrword_t);
#endif

#define  CS8900_BASE 0x20000000
#include "cs8900.h"
#define ETHER_ADDR_LEN 6

extern int net_debug; // FIXME

struct cs8900_priv_data {
    int txbusy;            // A packet has been sent
    unsigned long txkey;   // Used to ack when packet sent
#ifdef CYGPKG_NET
    cyg_tick_count_t txstart;
#endif
} _cs8900_priv_data;

ETH_DRV_SC(edb7xxx_sc,
           &_cs8900_priv_data, // Driver specific data
           "eth0",             // Name for this interface
           cs8900_start,
           cs8900_stop,
           cs8900_control,
           cs8900_can_send,
           cs8900_send,
           cs8900_recv,
           cs8900_deliver,     // "pseudoDSR" called from fast net thread
           cs8900_int,         // poll function, encapsulates ISR and DSR
           cs8900_int_vector);

NETDEVTAB_ENTRY(edb7xxx_netdev, 
                "edb7xxx", 
                edb7xxx_cs8900_init, 
                &edb7xxx_sc);

#ifdef CYGSEM_ARM_EDB7XXX_SET_ESA
static unsigned char enaddr[] = CYGDAT_ARM_EDB7XXX_ESA;
#else
static unsigned char enaddr[ETHER_ADDR_LEN];
#endif

static void cs8900_int(struct eth_drv_sc *sc);
static cyg_interrupt cs8900_interrupt;
static cyg_handle_t  cs8900_interrupt_handle;

// This ISR is called when the ethernet interrupt occurs
static int
cs8900_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT3);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

// The deliver function (ex-DSR)  handles the ethernet [logical] processing
static void
cs8900_deliver(struct eth_drv_sc *sc)
{
    cs8900_int(sc);
    // Allow interrupts to happen again
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT3);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT3);
}

static int
cs8900_int_vector(struct eth_drv_sc *sc)
{
    return (CYGNUM_HAL_INTERRUPT_EINT3);
}

static bool 
edb7xxx_cs8900_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    unsigned short chip_type, chip_rev, chip_status;
    int i;

    // Initialize environment, setup interrupt handler
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT3,
                             99, // Priority - what goes here?
                             (cyg_addrword_t)sc, //  Data item passed to interrupt handler
                             (cyg_ISR_t *)cs8900_isr,
                             (cyg_DSR_t *)eth_drv_dsr, // The logical driver DSR
                             &cs8900_interrupt_handle,
                             &cs8900_interrupt);
    cyg_drv_interrupt_attach(cs8900_interrupt_handle);
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT3);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT3);

#ifdef INTS_DONT_WORK
    cyg_thread_create(1,                 // Priority
                      cs8900_fake_int,   // entry
                      0,                 // entry parameter
                      "CS8900 int",      // Name
                      &cs8900_fake_int_stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &cs8900_fake_int_thread_handle,    // Handle
                      &cs8900_fake_int_thread_data       // Thread data structure
            );
    cyg_thread_resume(cs8900_fake_int_thread_handle);  // Start it
#endif

    chip_type = get_reg(PP_ChipID);
    chip_rev = get_reg(PP_ChipRev);

#if 0
    diag_printf("CS8900 - type: %x, rev: %x\n", chip_type, chip_rev);
#endif

    // Fetch hardware address
#if defined(CYGPKG_REDBOOT) && \
    defined(CYGSEM_REDBOOT_FLASH_CONFIG) && \
    !defined(CYGSEM_ARM_EDB7XXX_SET_ESA)
    flash_get_config("edb7xxx_esa", enaddr, CONFIG_ESA);
#elif !defined(CYGSEM_ARM_EDB7XXX_SET_ESA)
    for (i = 0;  i < ETHER_ADDR_LEN;  i += 2) {
        unsigned short esa_reg = get_reg(PP_IA+i);
        enaddr[i] = esa_reg & 0xFF;
        enaddr[i+1] = esa_reg >> 8;
    }
#endif

    put_reg(PP_SelfCtl, PP_SelfCtl_Reset);  // Reset chip
    while ((get_reg(PP_SelfStat) & PP_SelfStat_InitD) == 0) ;  

    chip_status = get_reg(PP_SelfStat);
#if 0
    diag_printf("CS8900 - status: %x (%sEEPROM present)\n", chip_status,
                chip_status&PP_SelfStat_EEPROM ? "" : "no ");
#endif

    // Set up hardware address
    for (i = 0;  i < ETHER_ADDR_LEN;  i += 2) {
        put_reg(PP_LAF+i, 0xFFFF);
        put_reg(PP_IA+i, enaddr[i] | (enaddr[i+1] << 8));
    }

    // Initialize upper level driver
    (sc->funs->eth_drv->init)(sc, enaddr);

    return true;
}

static void
cs8900_stop(struct eth_drv_sc *sc)
{
    put_reg(PP_SelfCtl, PP_SelfCtl_Reset);  // Reset chip
    while ((get_reg(PP_SelfStat) & PP_SelfStat_InitD) == 0) ;  
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
cs8900_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    unsigned short stat;
    struct cs8900_priv_data *cpd = (struct cs8900_priv_data *)sc->driver_private;

    put_reg(PP_BusCtl, PP_BusCtl_MemoryE);  // Disable interrupts, memory mode
    put_reg(PP_IntReg, PP_IntReg_IRQ0);  // Only possibility
    put_reg(PP_RxCFG, PP_RxCFG_RxOK | PP_RxCFG_CRC | 
                      PP_RxCFG_RUNT | PP_RxCFG_EXTRA);
    put_reg(PP_RxCTL, PP_RxCTL_RxOK | PP_RxCTL_Broadcast | 
                      PP_RxCTL_IA);
    put_reg(PP_TxCFG, PP_TxCFG_TxOK | PP_TxCFG_Collision | 
                      PP_TxCFG_CRS | PP_TxCFG_SQE | PP_TxCFG_Late | 
                      PP_TxCFG_Jabber | PP_TxCFG_16Collisions);
    put_reg(PP_BufCFG, PP_BufCFG_TxRDY | PP_BufCFG_TxUE | PP_BufCFG_RxMiss | 
                       PP_BufCFG_TxCol | PP_BufCFG_Miss | PP_BufCFG_SWI);
    put_reg(PP_IntReg, PP_IntReg_IRQ0);  // Only possibility
    put_reg(PP_LineCTL, PP_LineCTL_Rx | PP_LineCTL_Tx);
    // Clear Interrupt Status Queue before enabling interrupts
    while ((stat = CS8900_ISQ) != 0) ;
    cpd->txbusy = 0;
    put_reg(PP_BusCtl, PP_BusCtl_EnableIRQ);
}

//
// This routine is called to perform special "control" opertions
//
static int
cs8900_control(struct eth_drv_sc *sc, unsigned long key,
               void *data, int data_length)
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
cs8900_can_send(struct eth_drv_sc *sc)
{
    struct cs8900_priv_data *cpd = (struct cs8900_priv_data *)sc->driver_private;
    unsigned short stat;

    stat = get_reg(PP_LineStat);
    if ((stat & PP_LineStat_LinkOK) == 0) {
        return false;  // Link not connected
    }
#ifdef CYGPKG_NET
    // Horrible hack!
    if (cpd->txbusy > 0) {
        cyg_tick_count_t now = cyg_current_time();
        if ((now - cpd->txstart) > 25) {
            // 250ms is more than enough to transmit one frame
            diag_printf("CS8900: Tx interrupt lost\n");
            cpd->txbusy = 0;
            // Free up the buffer (with error indication)
            (sc->funs->eth_drv->tx_done)(sc, cpd->txkey, 1);
        }
    }
#endif
    return (cpd->txbusy == 0);
}

//
// This routine is called to send data to the hardware.
static void 
cs8900_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
            int total_len, unsigned long key)
{
    struct cs8900_priv_data *cpd = (struct cs8900_priv_data *)sc->driver_private;
    int i;
    int len;
    unsigned char *data;
    unsigned short saved_data = 0, *sdata;
    unsigned short stat;
    bool odd_byte = false;

    // Mark xmitter busy
    cpd->txbusy = 1;
    cpd->txkey = key;
#ifdef CYGPKG_NET
    cpd->txstart = cyg_current_time();
#endif
    // Start the xmit sequence

    // The hardware indicates that there are options as to when the actual
    // packet transmission will start wrt moving of data into the transmit
    // buffer.  However, impirical results seem to indicate that if the
    // packet is large and transmission is allowed to start before the
    // entire packet has been pushed into the buffer, the hardware gets
    // confused and the packet is lost, along with a "lost" Tx interrupt.
    // This may be a case of the copy loop below being interrupted, e.g.
    // a system timer interrupt, and the hardware getting unhappy that 
    // not all of the data was provided before the transmission should
    // have completed (i.e. buffer underrun).
    // For now, the solution is to not allow this overlap.
//    CS8900_TxCMD = PP_TxCmd_TxStart_5;  // Start more-or-less immediately
    CS8900_TxCMD = PP_TxCmd_TxStart_Full;  // Start only when all data sent to chip

    CS8900_TxLEN = total_len;
    stat = get_reg(PP_BusStat);  // This actually starts the xmit

    // Put data into buffer
    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned char *)sg_list[i].buf;
        len = sg_list[i].len;
        if (len > 0) {
            /* Finish the last word. */
            if (odd_byte) {
                saved_data |= (*data++ << 8);
                CS8900_RTDATA = saved_data;
                len--;
                odd_byte = false;
            }
            /* Output contiguous words. */
            sdata = (unsigned short *)data;
            while (len > 1) {
                CS8900_RTDATA = *sdata++;
                len -= sizeof(unsigned short);
            }
            /* Save last byte, if necessary. */
            if (len == 1) {
                data = (unsigned char *)sdata;
                saved_data = *data;
                odd_byte = true;
            }
        }
    }
    if (odd_byte) {
        CS8900_RTDATA = saved_data;
    }
}

//
// This function is called when a packet has been received.  It's job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'cs8900_recv' will be called to actually fetch it from the hardware.
//
static void
cs8900_RxEvent(struct eth_drv_sc *sc)
{
    unsigned short stat, len;

    stat = CS8900_RTDATA;
    len = CS8900_RTDATA;
    if (net_debug) {
        diag_printf("RxEvent - stat: %x, len: %d\n", stat, len);
    }
    (sc->funs->eth_drv->recv)(sc, len);
}

//
// This function is called as a result of the "eth_drv_recv()" call above.
// It's job is to actually fetch data for a packet from the hardware once
// memory buffers have been allocated for the packet.  Note that the buffers
// may come in pieces, using a scatter-gather list.  This allows for more
// efficient processing in the upper layers of the stack.
//
static void
cs8900_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    int i, mlen;
    unsigned short *data, val;
    unsigned char *cp, cval;

    for (i = 0;  i < sg_len;  i++) {
        data = (unsigned short *)sg_list[i].buf;
        mlen = sg_list[i].len;
        while (mlen >= sizeof(*data)) {
            val = CS8900_RTDATA;
            if (data) {
                *data++ = val;
            }
            mlen -= sizeof(*data);
        }
        if (mlen) {
            // Fetch last odd byte
            cval = CS8900_RTDATA & 0xFF;
            if ((cp = (unsigned char *)data) != 0) {
                *cp = cval;
            }
        }
    }
}

static void
cs8900_TxEvent(struct eth_drv_sc *sc, int stat)
{
    struct cs8900_priv_data *cpd = (struct cs8900_priv_data *)sc->driver_private;

    stat = get_reg(PP_TER);
    if (net_debug) {
        diag_printf("Tx event: %x\n", stat);
    }
    cpd->txbusy = 0;
    (sc->funs->eth_drv->tx_done)(sc, cpd->txkey, 0);
}

static void
cs8900_BufEvent(struct eth_drv_sc *sc, int stat)
{
    if (stat & PP_BufCFG_RxMiss) {
    }
    if (stat & PP_BufCFG_TxUE) {
    }
}

static void
cs8900_int(struct eth_drv_sc *sc)
{
    unsigned short event;

    while ((event = CS8900_ISQ) != 0) {
        switch (event & ISQ_EventMask) {
        case ISQ_RxEvent:
            cs8900_RxEvent(sc);
            break;
        case ISQ_TxEvent:
            cs8900_TxEvent(sc, event);
            break;
        case ISQ_BufEvent:
            cs8900_BufEvent(sc, event);
            break;
        case ISQ_RxMissEvent:
            // Receive miss counter has overflowed
            break;
        case ISQ_TxColEvent:
            // Transmit collision counter has overflowed
            break;
        default:
            diag_printf("%s: Unknown event: %x\n", __FUNCTION__, event);
            break;
        }
    }
}

#ifdef INTS_DONT_WORK
void
cs8900_fake_int(cyg_addrword_t param)
{
    int s;

    while (true) {
        cyg_thread_delay(5);
        s = splnet();
        cs8900_int(&edb7xxx_sc);
        splx(s);
    }
}
#endif

