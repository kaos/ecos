//==========================================================================
//
//      devs_eth_cf.inl
//
//      CF (PCMCIA) ethernet I/O definitions.
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
// Author(s):   jskov
// Contributors:jskov
// Date:        2001-06-15
// Purpose:     PCMCIA ethernet defintions
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_ETHR
#include <cyg/hal/hal_if.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/io/pcmcia.h>

#ifdef __WANT_CONFIG

#undef CYGHWR_NS_DP83902A_PLF_INT_CLEAR
#define CYGHWR_NS_DP83902A_PLF_INT_CLEAR(_dp_)                  \
    CYG_MACRO_START                                             \
    struct cf_slot* slot = (struct cf_slot*) (_dp_)->plf_priv;  \
    cf_clear_interrupt(slot);                                   \
    CYG_MACRO_END

#endif // __WANT_CONFIG

#ifdef __WANT_DEVS

externC int cyg_sc_lpe_int_vector(struct eth_drv_sc *sc);
externC bool cyg_sc_lpe_init(struct cyg_netdevtab_entry *tab);

#ifdef CYGPKG_DEVS_ETH_CF_ETH0

static dp83902a_priv_data_t dp83902a_eth0_priv_data = { 
#ifdef CYGSEM_DEVS_ETH_CF_ETH0_SET_ESA
    esa : CYGDAT_DEVS_ETH_CF_ETH0_ESA,
    hardwired_esa : true,
#else
    hardwired_esa : false,
#endif
};

ETH_DRV_SC(dp83902a_sc,
           &dp83902a_eth0_priv_data, // Driver specific data
           CYGDAT_DEVS_ETH_CF_ETH0_NAME,
           dp83902a_start,
           dp83902a_stop,
           dp83902a_control,
           dp83902a_can_send,
           dp83902a_send,
           dp83902a_recv,
           dp83902a_deliver,     // "pseudoDSR" called from fast net thread
           dp83902a_poll,        // poll function, encapsulates ISR and DSR
           cyg_sc_lpe_int_vector);

NETDEVTAB_ENTRY(dp83902a_netdev, 
                "dp83902a_" CYGDAT_DEVS_ETH_CF_ETH0_NAME,
                cyg_sc_lpe_init, 
                &dp83902a_sc);
#endif // CYGPKG_DEVS_ETH_CF_ETH0

#endif // __WANT_DEVS

// EOF devs_eth_cf.inl
