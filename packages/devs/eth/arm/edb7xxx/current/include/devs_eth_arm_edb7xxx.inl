//==========================================================================
//
//      devs_eth_arm_edb7xxx.inl
//
//      EDB7XXX ethernet I/O definitions.
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
// Date:        2001-11-14
// Purpose:     EDB7XXX ethernet defintions
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/hal/hal_intr.h>           // CYGNUM_HAL_INTERRUPT_EINT3
#include <cyg/hal/hal_if.h>

#ifdef CYGPKG_REDBOOT
# include <pkgconf/redboot.h>
# ifdef CYGSEM_REDBOOT_FLASH_CONFIG
#  include <redboot.h>
#  include <flash_config.h>
# endif
#endif

#ifdef __WANT_CONFIG

# define CS8900A_step 4

#endif

#ifdef __WANT_DEVS

#ifdef CYGPKG_DEVS_ETH_ARM_EDB7XXX_ETH0

#ifndef CYGSEM_DEVS_ETH_ARM_EDB7XXX_ETH0_SET_ESA
# if defined(CYGPKG_REDBOOT) && defined(CYGSEM_REDBOOT_FLASH_CONFIG)
RedBoot_config_option("Set " CYGDAT_DEVS_ETH_ARM_EDB7XXX_ETH0_NAME " network hardware address [MAC]",
                      eth0_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_BOOL, false
    );
RedBoot_config_option(CYGDAT_DEVS_ETH_ARM_EDB7XXX_ETH0_NAME " network hardware address [MAC]",
                      eth0_esa_data,
                      "eth0_esa", true,
                      CONFIG_ESA, 0
    );
# endif // CYGPKG_REDBOOT && CYGSEM_REDBOOT_FLASH_CONFIG

# ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
// Note that this section *is* active in an application, outside RedBoot,
// where the above section is not included.

#  include <cyg/hal/hal_if.h>

#  ifndef CONFIG_ESA
#   define CONFIG_ESA (6)
#  endif
#  ifndef CONFIG_BOOL
#   define CONFIG_BOOL (1)
#  endif

cyg_bool
_edb7xxx_provide_eth0_esa(struct cs8900a_priv_data* cpd)
{
    cyg_bool set_esa;
    int ok;
    ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,
                                      "eth0_esa", &set_esa, CONFIG_BOOL);
    if (ok && set_esa) {
        ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,
                                          "eth0_esa_data", cpd->esa, CONFIG_ESA);
    }
    return ok && set_esa;
}

# endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
#endif // ! CYGSEM_DEVS_ETH_ARM_EDB7XXX_ETH0_SET_ESA

static cs8900a_priv_data_t cs8900a_eth0_priv_data = { 
    base : (cyg_addrword_t) 0x20000000,
    interrupt: CYGNUM_HAL_INTERRUPT_EINT3,
#ifdef CYGSEM_DEVS_ETH_ARM_EDB7XXX_ETH0_SET_ESA
    esa : CYGDAT_DEVS_ETH_ARM_EDB7XXX_ETH0_ESA,
    hardwired_esa : true,
#else
    hardwired_esa : false,
# ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    provide_esa : &_edb7xxx_provide_eth0_esa,
# else
    provide_esa : NULL,
# endif
#endif
};

ETH_DRV_SC(cs8900a_sc,
           &cs8900a_eth0_priv_data, // Driver specific data
           CYGDAT_DEVS_ETH_ARM_EDB7XXX_ETH0_NAME,
           cs8900a_start,
           cs8900a_stop,
           cs8900a_control,
           cs8900a_can_send,
           cs8900a_send,
           cs8900a_recv,
           cs8900a_deliver,     // "pseudoDSR" called from fast net thread
           cs8900a_poll,        // poll function, encapsulates ISR and DSR
           cs8900a_int_vector);

NETDEVTAB_ENTRY(cs8900a_netdev, 
                "cs8900a_" CYGDAT_DEVS_ETH_ARM_EDB7XXX_ETH0_NAME,
                cs8900a_init, 
                &cs8900a_sc);

#endif // CYGPKG_DEVS_ETH_ARM_EDB7XXX_ETH0

#endif // __WANT_DEVS

// EOF devs_eth_arm_edb7xxx.inl
