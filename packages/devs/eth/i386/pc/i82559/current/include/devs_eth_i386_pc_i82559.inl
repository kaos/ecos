//==========================================================================
//
//      devs/eth/mips/ocelot/include/devs_eth_mips_rm7000_ocelot.inl
//
//      PC i82559 ethernet I/O definitions.
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
// Date:        2001-01-25
// Purpose:     PC i82559 ethernet defintions
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_ETHR

#ifdef CYGPKG_DEVS_ETH_I386_PC_I82559_ETH0

#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE (CYGARC_UNCACHED_ADDRESS(CYGMEM_SECTION_pci_window))
#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE CYGMEM_SECTION_pci_window_SIZE

static I82559 i82559_eth0_priv_data = { 
#ifdef CYGSEM_DEVS_ETH_I386_PC_I82559_ETH0_SET_ESA
    hardwired_esa: 1,
    mac_address: CYGDAT_DEVS_ETH_I386_PC_I82559_ETH0_ESA
#else
    hardwired_esa: 0,
#endif
};

ETH_DRV_SC(i82559_sc0,
           &i82559_eth0_priv_data,      // Driver specific data
           CYGDAT_DEVS_ETH_I386_PC_I82559_ETH0_NAME, // Name for device
           i82559_start,
           i82559_stop,
           i82559_ioctl,
           i82559_can_send,
           i82559_send,
           i82559_recv,
           i82559_deliver,
           i82559_poll,
           i82559_int_vector
    );

NETDEVTAB_ENTRY(i82559_netdev0, 
                "i82559_" CYGDAT_DEVS_ETH_I386_PC_I82559_ETH0_NAME,
                i82559_init, 
                &i82559_sc0);

#endif // CYGPKG_DEVS_ETH_I386_PC_I82559_ETH0


// These arrays are used for sanity checking of pointers
I82559 *
i82559_priv_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_I386_PC_I82559_ETH0
    &i82559_eth0_priv_data,
#endif
};

#ifdef CYGDBG_USE_ASSERTS
// These are only used when assertions are enabled
cyg_netdevtab_entry_t *
i82559_netdev_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_I386_PC_I82559_ETH0
    &i82559_netdev0,
#endif
};

struct eth_drv_sc *
i82559_sc_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_I386_PC_I82559_ETH0
    &i82559_sc0,
#endif
};
#endif // CYGDBG_USE_ASSERTS

// EOF devs_eth_i386_pc_i82559.inl
