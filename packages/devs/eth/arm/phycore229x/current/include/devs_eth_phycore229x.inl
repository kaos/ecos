#ifndef CYGONCE_DEVS_ETH_PHYCORE229X_INL
#define CYGONCE_DEVS_ETH_PHYCORE229X_INL
//==========================================================================
//
//  devs_eth_phycore_229x.inl
//
//  phyCORE-LPC229x ethernet I/O definitions.
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2008 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    Uwe Kindler
// Contributors: 
// Date:         2007-11-24 
// Purpose:      
//####DESCRIPTIONEND####
//==========================================================================


//==========================================================================
//                               INCLUDES
//==========================================================================
#include <pkgconf/system.h>
#include <pkgconf/devs_eth_arm_phycore229x.h>
#include <cyg/hal/hal_intr.h>  

#ifdef CYGPKG_REDBOOT
  #include <pkgconf/redboot.h>
  #ifdef CYGSEM_REDBOOT_FLASH_CONFIG
    #include <redboot.h>
    #include <flash_config.h>
  #endif // CYGSEM_REDBOOT_FLASH_CONFIG
#endif // CYGPKG_REDBOOT 



//--------------------------------------------------------------------------
// Configure LAN91CXX driver
//
#define LAN91CXX_IS_LAN91C111	1
#define LAN91CXX_FORCE_10MHZ    1 // due a PCB tracking problem only
                                  // 10 MHz is possible
#define LAN91CXX_32BIT_RX       1 // 32 bit access


//--------------------------------------------------------------------------
// RedBoot ESA Flash configuration options
// When this option is enabled, RedBoot will keep configuration
// data in a separate block of FLASH memory. 
//
#if defined(CYGPKG_REDBOOT) && defined(CYGSEM_REDBOOT_FLASH_CONFIG)
RedBoot_config_option("Set " CYGDAT_DEVS_ETH_ARM_PHYCORE229X_NAME " network hardware address [MAC]",
                      eth0_esa,
                      ALWAYS_ENABLED, true,
                      CONFIG_BOOL, false
    );
RedBoot_config_option(CYGDAT_DEVS_ETH_ARM_PHYCORE229X_NAME " network hardware address [MAC]",
                      eth0_esa_data,
                      "eth0_esa", true,
                      CONFIG_ESA, 0
    );
#endif // CYGPKG_REDBOOT && CYGSEM_REDBOOT_FLASH_CONFIG


//--------------------------------------------------------------------------
// Application ESA Flash configuration options
// Note that this section *is* active in an application, outside 
// RedBoot, where the above section is not included. 
//
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
#include <cyg/hal/hal_if.h>

#ifndef CONFIG_ESA
    #define CONFIG_ESA (6)
#endif
#ifndef CONFIG_BOOL
    #define CONFIG_BOOL (1)
#endif


//--------------------------------------------------------------------------
// Provide ESA function
// Returns true if ESA is configured by flash, else false 
//
cyg_bool phycore229x_provide_esa(struct lan91cxx_priv_data *cpd)
{
    cyg_bool set_esa;
    int      ok;
    
    
    //
    // first we check, if the ESA should be set according to flash
    // configuration
    //
    ok = CYGACC_CALL_IF_FLASH_CFG_OP(CYGNUM_CALL_IF_FLASH_CFG_GET,
                                     "eth0_esa", 
                                     &set_esa, 
                                     CONFIG_BOOL);
    //
    // if esa should be set by flash configuration, then we store the
    // esa from flash in driver configuration data and return true
    //
    if (ok && set_esa) 
    {
        ok = CYGACC_CALL_IF_FLASH_CFG_OP(CYGNUM_CALL_IF_FLASH_CFG_GET,
                                         "eth0_esa_data", 
                                         cpd->enaddr, 
                                         CONFIG_ESA);
    }
    return ok && set_esa;
}
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT


//---------------------------------------------------------------------------
// Stores configuration data for generic smsc91xxx driver
//
static lan91cxx_priv_data lan91cxx_eth0_priv_data = {  
    base :          (unsigned short *)(CYGHWR_HAL_ARM_PHYCORE229X_ETH_MEM_AREA 
                                       + 0x0300),
    interrupt :      ( CYGHWR_HAL_ARM_PHYCORE229X_ETH_EINT + 
                       CYGNUM_HAL_INTERRUPT_EINT0) ,   
    //
    // if user selects a hardwired ESA in configuration then
    // we fill the ESA here
    //
#ifdef CYGSEM_DEVS_ETH_PHYCORE229X_SET_ESA
    enaddr        : CYGDAT_DEVS_ETH_PHYCORE229X_ESA,
    hardwired_esa : true,
#else
    hardwired_esa : false,
#endif 
#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    provide_esa   : &phycore229x_provide_esa,
#else
    provide_esa   : NULL,             // read MAC from associated EEPROM
#endif // CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
}; // lan91cxx_priv_data


//---------------------------------------------------------------------------
// Driver configuration
//
ETH_DRV_SC(
    lan91cxx_sc,
    &lan91cxx_eth0_priv_data,                   // driver specific data
    CYGDAT_DEVS_ETH_ARM_PHYCORE229X_NAME,       // device name
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


//---------------------------------------------------------------------------
// Entry into net device table
//
NETDEVTAB_ENTRY( 
    lan91cxx_netdev, 
    "lan91cxx_" CYGDAT_DEVS_ETH_ARM_PHYCORE229X_NAME,
    smsc_lan91cxx_init,
    &lan91cxx_sc
);

//---------------------------------------------------------------------------
#endif // CYGONCE_DEVS_ETH_PHYCORE229X_INL
