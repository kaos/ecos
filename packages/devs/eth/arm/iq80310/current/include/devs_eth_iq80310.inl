//==========================================================================
//
//      devs/eth/arm/iq80310/include/devs_eth_arm_iq80310.inl
//
//      IQ80310 ethernet I/O definitions.
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
// Author(s):   msalter
// Contributors:msalter
// Date:        2001-12-20
// Purpose:     IQ80310 ethernet defintions
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_ETHERNET
#include <cyg/hal/hal_cache.h>          // HAL_DCACHE_LINE_SIZE
#include <cyg/hal/plf_io.h>             // CYGARC_UNCACHED_ADDRESS

#ifdef CYGPKG_DEVS_ETH_ARM_IQ80310_ETH0

// Bus masters can get to all of SDRAM using direct mapping.
#define CYGHWR_INTEL_I82559_PCI_VIRT_TO_BUS( _x_ ) ((cyg_uint32)CYGARC_VIRT_TO_BUS(_x_))

#ifndef CYGSEM_DEVS_ETH_ARM_IQ80310_ETH0_SET_ESA
# define CYGHWR_DEVS_ETH_INTEL_I82559_HAS_ONE_EEPROM 0
# define CYGHWR_DEVS_ETH_INTEL_I82559_HAS_ONE_EEPROM_WITHOUT_CRC
#endif

#define MAX_PACKET_SIZE   1536
#define SIZEOF_DESCRIPTOR 16
#define MISC_MEM          1128     // selftest, ioctl and statistics

#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE \
  (((MAX_PACKET_SIZE + SIZEOF_DESCRIPTOR) * \
     (CYGNUM_DEVS_ETH_INTEL_I82559_MAX_TX_DESCRIPTORS + \
      CYGNUM_DEVS_ETH_INTEL_I82559_MAX_RX_DESCRIPTORS)) + \
   MISC_MEM)

static char pci_mem_buffer[CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE + HAL_DCACHE_LINE_SIZE];

#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE \
  (CYGARC_UNCACHED_ADDRESS(((unsigned)pci_mem_buffer + HAL_DCACHE_LINE_SIZE - 1) & ~(HAL_DCACHE_LINE_SIZE - 1)))

static I82559 i82559_eth0_priv_data = { 
#ifdef CYGSEM_DEVS_ETH_ARM_IQ80310_ETH0_SET_ESA
    hardwired_esa: 1,
    mac_address: CYGDAT_DEVS_ETH_ARM_IQ80310_ETH0_ESA
#else
    hardwired_esa: 0,
#endif
};

ETH_DRV_SC(i82559_sc0,
           &i82559_eth0_priv_data,      // Driver specific data
           CYGDAT_DEVS_ETH_ARM_IQ80310_ETH0_NAME, // Name for device
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
                "i82559_" CYGDAT_DEVS_ETH_ARM_IQ80310_ETH0_NAME,
                i82559_init, 
                &i82559_sc0);

#endif // CYGPKG_DEVS_ETH_ARM_IQ80310_ETH0


// These arrays are used for sanity checking of pointers
I82559 *
i82559_priv_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_IQ80310_ETH0
    &i82559_eth0_priv_data,
#endif
};

#ifdef CYGDBG_USE_ASSERTS
// These are only used when assertions are enabled
cyg_netdevtab_entry_t *
i82559_netdev_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_IQ80310_ETH0
    &i82559_netdev0,
#endif
};

struct eth_drv_sc *
i82559_sc_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_IQ80310_ETH0
    &i82559_sc0,
#endif
};
#endif // CYGDBG_USE_ASSERTS

// EOF devs_eth_arm_iq80310.inl
