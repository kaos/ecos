//==========================================================================
//
//      devs/eth/arm/nano/..../include/devs_eth_nano.inl
//
//      nanoBridge ethernet I/O definitions.
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
// Author(s):    jskov, hmt
// Contributors: jskov
// Date:         2001-02-28
// Purpose:      nanoBridge ethernet defintions
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_...

// --------------------------------------------------------------
// Platform specifics:

#if 1 < CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT
#define CYGHWR_DEVS_ETH_INTEL_I82559_DEMUX_ALL
#endif // multiple devs, so demux_all needed

// define multiple interrupt handling anyway:

#define CYGHWR_DEVS_ETH_INTRS (SA11X0_GPIO_PIN_0 + SA11X0_GPIO_PIN_1)

// This brings in code to ensure missed interrupts are properly
// acknowledged so that another interrupt can occur in future.
// Only a problem with edge-triggered systems.

#define CYGHWR_DEVS_ETH_INTEL_I82559_MISSED_INTERRUPT(p_i82559) \
 (CYGHWR_DEVS_ETH_INTRS != (CYGHWR_DEVS_ETH_INTRS & *SA11X0_GPIO_PIN_LEVEL))

// This brings on code to perform a selective reset on the device if the CU
// wedges.

#define CYGHWR_DEVS_ETH_INTEL_I82559_DEAD_TO (368640) // 0.1S of OS timer

#define CYGHWR_DEVS_ETH_INTEL_I82559_RESET_TIMEOUT( anon_uint ) \
CYG_MACRO_START                                                 \
    (anon_uint) = *SA11X0_OSCR;                                 \
CYG_MACRO_END

#define CYGHWR_DEVS_ETH_INTEL_I82559_TIMEOUT_FIRED( anon_uint )         \
  ((*SA11X0_OSCR - (anon_uint)) > CYGHWR_DEVS_ETH_INTEL_I82559_DEAD_TO)

// The mask on an SA1110 is really an enable: 1 => enabled, 0 => masked.
// So to behave nestedly, we only need save the old value of the bits
// of interest.

#define CYGPRI_DEVS_ETH_INTEL_I82559_MASK_INTERRUPTS(p_i82559,old)      \
CYG_MACRO_START                                                         \
    int cpu;                                                            \
    HAL_DISABLE_INTERRUPTS( cpu );                                      \
    old = *SA11X0_ICMR;                                                 \
    *SA11X0_ICMR = old & ~CYGHWR_DEVS_ETH_INTRS;                        \
    old &= CYGHWR_DEVS_ETH_INTRS; /* old val */                         \
    HAL_RESTORE_INTERRUPTS( cpu );                                      \
CYG_MACRO_END

#define CYGPRI_DEVS_ETH_INTEL_I82559_UNMASK_INTERRUPTS(p_i82559,old)    \
CYG_MACRO_START                                                         \
    int cpu;                                                            \
    HAL_DISABLE_INTERRUPTS( cpu );                                      \
    (*SA11X0_ICMR |= (old & CYGHWR_DEVS_ETH_INTRS));                    \
    HAL_RESTORE_INTERRUPTS( cpu );                                      \
CYG_MACRO_END

#define CYGPRI_DEVS_ETH_INTEL_I82559_ACK_INTERRUPTS(p_i82559)   \
CYG_MACRO_START                                                 \
    /* Remove the latched edge in the PIC: */                   \
    *SA11X0_GPIO_EDGE_DETECT_STATUS = CYGHWR_DEVS_ETH_INTRS;    \
CYG_MACRO_END


// --------------------------------------------------------------

#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_BASE CYGHWR_HAL_ARM_NANO_PCI_MEM_MAP_BASE
#define CYGHWR_INTEL_I82559_PCI_MEM_MAP_SIZE CYGHWR_HAL_ARM_NANO_PCI_MEM_MAP_SIZE

#define CYGHWR_INTEL_I82559_PCI_VIRT_TO_BUS( _x_ ) \
  (((cyg_uint32)(_x_)) - CYGHWR_HAL_ARM_NANO_PCI_MEM_MAP_BASE + cyg_pci_window_real_base)

// --------------------------------------------------------------
// Construct the two interfaces

#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH0

static I82559 i82559_eth0_priv_data = { 
#ifdef CYGSEM_DEVS_ETH_ARM_NANO_ETH0_SET_ESA
    hardwired_esa: 1,
    mac_address: CYGDAT_DEVS_ETH_ARM_NANO_ETH0_ESA
#else
    hardwired_esa: 0,
#endif
};

ETH_DRV_SC(i82559_sc0,
           &i82559_eth0_priv_data,      // Driver specific data
           CYGDAT_DEVS_ETH_ARM_NANO_ETH0_NAME, // Name for device
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
                "i82559_" CYGDAT_DEVS_ETH_ARM_NANO_ETH0_NAME,
                i82559_init, 
                &i82559_sc0);

#endif // CYGPKG_DEVS_ETH_ARM_NANO_ETH0

#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH1

static I82559 i82559_eth1_priv_data = { 
#ifdef CYGSEM_DEVS_ETH_ARM_NANO_ETH1_SET_ESA
    hardwired_esa: 1,
    mac_address: CYGDAT_DEVS_ETH_ARM_NANO_ETH1_ESA
#else
    hardwired_esa: 0,
#endif
};

ETH_DRV_SC(i82559_sc1,
           &i82559_eth1_priv_data,      // Driver specific data
           CYGDAT_DEVS_ETH_ARM_NANO_ETH1_NAME, // Name for device
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

NETDEVTAB_ENTRY(i82559_netdev1, 
                "i82559_" CYGDAT_DEVS_ETH_ARM_NANO_ETH1_NAME,
                i82559_init, 
                &i82559_sc1);

#endif // CYGPKG_DEVS_ETH_ARM_NANO_ETH1

// --------------------------------------------------------------
// These arrays are used for sanity checking of pointers
I82559 *
i82559_priv_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH0
    &i82559_eth0_priv_data,
#endif
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH1
    &i82559_eth1_priv_data,
#endif
};

#ifdef CYGDBG_USE_ASSERTS
// These are only used when assertions are enabled
cyg_netdevtab_entry_t *
i82559_netdev_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH0
    &i82559_netdev0,
#endif
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH1
    &i82559_netdev1,
#endif
};

struct eth_drv_sc *
i82559_sc_array[CYGNUM_DEVS_ETH_INTEL_I82559_DEV_COUNT] = {
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH0
    &i82559_sc0,
#endif
#ifdef CYGPKG_DEVS_ETH_ARM_NANO_ETH1
    &i82559_sc1,
#endif
};
#endif // CYGDBG_USE_ASSERTS

// --------------------------------------------------------------
// Debugging

//#define CYGDBG_DEVS_ETH_INTEL_I82559_CHATTER 1

// --------------------------------------------------------------
// RedBoot configuration options for managing ESAs for us

// tell the driver there is no EEPROM on this board
#define CYGHWR_DEVS_ETH_INTEL_I82559_HAS_NO_EEPROM

// Decide whether to have redboot config vars for it...
#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#ifdef CYGPKG_REDBOOT_NETWORKING
#include <redboot.h>
#include <flash_config.h>

#ifdef CYGVAR_DEVS_ETH_I82559_ETH_REDBOOT_HOLDS_ESA_ETH0
RedBoot_config_option("Network hardware address [MAC] for eth0",
                      eth0_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#endif

#ifdef CYGVAR_DEVS_ETH_I82559_ETH_REDBOOT_HOLDS_ESA_ETH1
RedBoot_config_option("Network hardware address [MAC] for eth1",
                      eth1_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#endif

#endif
#endif
#endif

// and initialization code to read them
// - independent of whether we are building RedBoot right now:
#ifdef CYGPKG_DEVS_ETH_I82559_ETH_REDBOOT_HOLDS_ESA

#include <cyg/hal/hal_if.h>

#ifndef CONFIG_ESA
#define CONFIG_ESA (6)
#endif

#define CYGHWR_DEVS_ETH_INTEL_I82559_GET_ESA( p_i82559, mac_address, ok )       \
CYG_MACRO_START                                                                 \
    ok = false;                                                                 \
    if ( 0 == p_i82559->index )                                                 \
        ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,         \
                                          "eth0_esa", mac_address, CONFIG_ESA); \
    else if ( 1 == p_i82559->index )                                            \
        ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,         \
                                          "eth1_esa", mac_address, CONFIG_ESA); \
CYG_MACRO_END

#endif // CYGPKG_DEVS_ETH_I82559_ETH_REDBOOT_HOLDS_ESA

// --------------------------------------------------------------

// EOF devs_eth_nano.inl
