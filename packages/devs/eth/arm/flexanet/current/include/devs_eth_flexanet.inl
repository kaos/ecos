//==========================================================================
//
//      devs/eth/arm/flexanet/..../include/devs_eth_flexanet.inl
//
//      Flexanet ethernet I/O definitions.
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
// Author(s):    Jordi Colomer <jco@ict.es>
// Contributors: Jordi Colomer
// Date:         2001-06-18
// Purpose:      Flexanet ethernet definitions
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_flexanet.h>
#include <cyg/hal/hal_intr.h>          
#include <cyg/hal/flexanet.h>

#define CYGNUM_DEVS_ETH_SMSC_LAN91CXX_SHIFT_ADDR    2

// MAC address is stored as a Redboot config option
#ifdef CYGPKG_REDBOOT
#include <pkgconf/redboot.h>
#ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#include <redboot.h>
#include <flash_config.h>

RedBoot_config_option("Network hardware address [MAC]",
                      flexanet_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0
    );
#endif
#endif

// ESA address fetch function
static void flexanet_get_ESA(struct lan91cxx_priv_data *cpd)
{
    // Fetch hardware address from RedBoot config
#if defined(CYGSEM_DEVS_ETH_ARM_FLEXANET_REDBOOT_ESA)
#if defined(CYGPKG_REDBOOT) && \
    defined(CYGSEM_REDBOOT_FLASH_CONFIG)
    flash_get_config("flexanet_esa", cpd->enaddr, CONFIG_ESA);
#else
#error "No RedBoot flash configuration to store ESA"
#endif
#else
    memcpy(cpd->enaddr, static_esa, 6);
#endif
}

static lan91cxx_priv_data lan91cxx_eth0_priv_data = { 

    config_enaddr : flexanet_get_ESA,
#ifndef CYGSEM_DEVS_ETH_ARM_FLEXANET_REDBOOT_ESA    
    enaddr: CYGDAT_DEVS_ETH_ARM_FLEXANET_ESA,
#endif
    base : (unsigned short *) SA1110_FHH_ETH_IOBASE,
    attbase : (unsigned char *) SA1110_FHH_ETH_MMBASE,
    interrupt : SA1110_IRQ_GPIO_ETH
};

ETH_DRV_SC(lan91cxx_sc,
           &lan91cxx_eth0_priv_data,          // Driver specific data
           CYGDAT_DEVS_ETH_ARM_FLEXANET_NAME, // Name for device
           lan91cxx_start,
           lan91cxx_stop,
           lan91cxx_control,
           lan91cxx_can_send,
           lan91cxx_send,
           lan91cxx_recv,
           lan91cxx_deliver,
           lan91cxx_poll,
           lan91cxx_int_vector
);

NETDEVTAB_ENTRY(lan91cxx_netdev, 
                "lan91cxx_" CYGDAT_DEVS_ETH_ARM_FLEXANET_NAME,
                smsc_lan91cxx_init,
                &lan91cxx_sc);

//EOF devs_eth_flexanet.inl


