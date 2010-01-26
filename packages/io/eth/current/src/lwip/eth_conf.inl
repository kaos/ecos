//==========================================================================
//
//      src/lwip/eth_conf.inl
//
//      Static interface configuration
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2009 Free Software Foundation, Inc.
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
// Author(s):    Simon Kallweit
// Contributors:
// Date:         2009-06-09
// Purpose:      Static interface configuration
// Description:  
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/net_lwip.h>

// Ethernet device configuration
struct eth_conf {
    cyg_uint8 def;          // This is the default netif
    cyg_uint8 dhcp;         // Use DHCP client to get address
    cyg_uint8 addr[4];      // IPv4 address
    cyg_uint8 netmask[4];   // IPv4 netmask
    cyg_uint8 gateway[4];   // IPv4 gateway
};

// Ethernet device configuration table
static struct eth_conf eth_conf_table[] = {
// Ethernet device 0
#ifdef CYGPKG_LWIP_ETH0_CONF
    {
# ifdef CYGDAT_LWIP_ETH0_DEFAULT
        .def = 1,
# endif
# ifdef CYGDAT_LWIP_ETH0_DHCP
        .dhcp = 1,
# endif
# ifdef CYGDAT_LWIP_ETH0_NETCONF_ADDR
        .addr = { CYGDAT_LWIP_ETH0_NETCONF_ADDR },
# endif
# ifdef CYGDAT_LWIP_ETH0_NETCONF_NETMASK
        .netmask = { CYGDAT_LWIP_ETH0_NETCONF_NETMASK },
# endif
# ifdef CYGDAT_LWIP_ETH0_NETCONF_GATEWAY
        .gateway = { CYGDAT_LWIP_ETH0_NETCONF_GATEWAY },
# endif
    },
#endif // CYGPKG_LWIP_ETH0_CONF
// Ethernet device 1
#ifdef CYGPKG_LWIP_ETH1_CONF
    {
# ifdef CYGDAT_LWIP_ETH1_DEFAULT
        .def = 1,
# endif
# ifdef CYGDAT_LWIP_ETH1_DHCP
        .dhcp = 1,
# endif
# ifdef CYGDAT_LWIP_ETH1_NETCONF_ADDR
        .addr = { CYGDAT_LWIP_ETH1_NETCONF_ADDR },
# endif
# ifdef CYGDAT_LWIP_ETH1_NETCONF_NETMASK
        .netmask = { CYGDAT_LWIP_ETH1_NETCONF_NETMASK },
# endif
# ifdef CYGDAT_LWIP_ETH1_NETCONF_GATEWAY
        .gateway = { CYGDAT_LWIP_ETH1_NETCONF_GATEWAY },
# endif
    },
#endif // CYGPKG_LWIP_ETH1_CONF
// Ethernet device 2
#ifdef CYGPKG_LWIP_ETH2_CONF
    {
# ifdef CYGDAT_LWIP_ETH2_DEFAULT
        .def = 1,
# endif
# ifdef CYGDAT_LWIP_ETH2_DHCP
        .dhcp = 1,
# endif
# ifdef CYGDAT_LWIP_ETH2_NETCONF_ADDR
        .addr = { CYGDAT_LWIP_ETH2_NETCONF_ADDR },
# endif
# ifdef CYGDAT_LWIP_ETH2_NETCONF_NETMASK
        .netmask = { CYGDAT_LWIP_ETH2_NETCONF_NETMASK },
# endif
# ifdef CYGDAT_LWIP_ETH2_NETCONF_GATEWAY
        .gateway = { CYGDAT_LWIP_ETH2_NETCONF_GATEWAY },
# endif
    },
#endif // CYGPKG_LWIP_ETH2_CONF
// Ethernet device 3
#ifdef CYGPKG_LWIP_ETH3_CONF
    {
# ifdef CYGDAT_LWIP_ETH3_DEFAULT
        .def = 1,
# endif
# ifdef CYGDAT_LWIP_ETH3_DHCP
        .dhcp = 1,
# endif
# ifdef CYGDAT_LWIP_ETH3_NETCONF_ADDR
        .addr = { CYGDAT_LWIP_ETH3_NETCONF_ADDR },
# endif
# ifdef CYGDAT_LWIP_ETH3_NETCONF_NETMASK
        .netmask = { CYGDAT_LWIP_ETH3_NETCONF_NETMASK },
# endif
# ifdef CYGDAT_LWIP_ETH3_NETCONF_GATEWAY
        .gateway = { CYGDAT_LWIP_ETH3_NETCONF_GATEWAY },
# endif
    },
#endif // CYGPKG_LWIP_ETH3_CONF
};

#define NUM_ETH_CONF (sizeof(eth_conf_table) / sizeof(struct eth_conf))

static int eth_conf_index; 

static struct eth_conf *next_eth_conf(void)
{
    if (eth_conf_index < NUM_ETH_CONF)
        return &eth_conf_table[eth_conf_index++];
    
    return NULL;
}
