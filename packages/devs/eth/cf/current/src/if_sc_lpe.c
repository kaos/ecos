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

#include <cyg/io/dp83902a.h>

#define DP_DATA         0x10
#define DP_CARD_RESET   0x1f

#define SC_LPE_MANUF 0x0104



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
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t*)sc->driver_private;
    struct cf_slot *slot;
    struct cf_cftable cftable;
    struct cf_config config;
    int i, len, ptr, cor = 0;
    unsigned char buf[256], *cp;
    cyg_uint8* base;
    unsigned char *vers_product, *vers_manuf, *vers_revision, *vers_date;
#ifndef CYGPKG_NET
    int tries = 0;
#endif
    bool first = true;

    slot = (struct cf_slot*)dp->plf_priv;
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
            if (!dp->hardwired_esa) {
                if (slot->attr[0x1C0] != (unsigned char)0xFF) {
                    for (i = 0;  i < ETHER_ADDR_LEN;  i++)
                        dp->esa[i] = slot->attr[0x1C0+(i*2)];
                } else {
                    diag_printf("No valid ESA found in CIS! Hardwiring to 08:88:87:65:43:21\n");
                    dp->esa[0] = 0x08;
                    dp->esa[1] = 0x88;
                    dp->esa[2] = 0x87;
                    dp->esa[3] = 0x65;
                    dp->esa[4] = 0x43;
                    dp->esa[5] = 0x21;
                }
            }

            ptr = 0;
            if (cf_get_CIS(slot, CF_CISTPL_CFTABLE_ENTRY, buf, &len, &ptr)) {
                if (cf_parse_cftable(buf, len, &cftable)) {
                    cyg_uint8 tmp;
                    // Initialize dp83902a IO details
                    dp->base = base = (cyg_uint8*)&slot->io[cftable.io_space.base[0]];
                    dp->data = base + DP_DATA;
                    dp->interrupt = slot->int_num;
                    cf_set_COR(slot, cor, cftable.cor);
                    // Reset card  (read issues RESET, write clears it)
                    HAL_READ_UINT8(base+DP_CARD_RESET, tmp);
                    HAL_WRITE_UINT8(base+DP_CARD_RESET, tmp);
                    // Wait for card
                    do {
                        DP_IN(base, DP_ISR, tmp);
                    } while (0 == (tmp & DP_ISR_RESET));
                    // Initialize upper level driver
                    (sc->funs->eth_drv->init)(sc, dp->esa);
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

bool 
cyg_sc_lpe_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    struct cf_slot* slot;

    cf_init();  // Make sure Compact Flash subsystem is initialized
    slot = dp->plf_priv = (void*)cf_get_slot(0);
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
    cf_register_handler(slot, eth_drv_dsr, sc);

    return false;  // Device is not ready until inserted, powered up, etc.
#else
    // Initialize card
    return sc_lpe_card_handler(sc);
#endif
}

int
cyg_sc_lpe_int_vector(struct eth_drv_sc *sc)
{
    dp83902a_priv_data_t *dp = (dp83902a_priv_data_t *)sc->driver_private;
    struct cf_slot* slot = (struct cf_slot*)dp->plf_priv;

    return slot->int_num;
}
