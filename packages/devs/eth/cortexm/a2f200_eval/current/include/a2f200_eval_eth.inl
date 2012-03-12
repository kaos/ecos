//==========================================================================
//
//      a2f200_eval.inl
//
//      Ethernet driver specifics for Actel Smartfusion (A2F200) board
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System. 
// Copyright (C) 2011 Free Software Foundation, Inc. 
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
// Author(s):    ccoutand
// Contributors:
// Date:         2011-05-04
// Purpose:
// Description:  Ethernet driver specifics for Actel Smartfusion (A2F200) board
//
//
//####DESCRIPTIONEND####
//
//==========================================================================


#ifndef CYGONCE_DEVS_A2F200_EVAL_ETH_INL
#define CYGONCE_DEVS_A2F200_EVAL_ETH_INL

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/var_io.h>

// Make sure packet size is multiple of words
#define CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_RX \
            ((CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_BUFSIZE_RX + 0x00000003) & (0xfffffffc))

#define CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_TX \
            ((CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_BUFSIZE_TX + 0x00000003) & (0xfffffffc))

#define RxBUFSIZE ( CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_RxNUM*CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_RX )
#define TxBUFSIZE ( CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_TxNUM*CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_TX )

#ifdef CYGPKG_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0

#ifdef CYGPKG_DEVS_ETH_PHY
ETH_PHY_REG_LEVEL_ACCESS_FUNS(eth0_phy,
                              a2fxxx_phy_init,
                              NULL,
                              a2fxxx_mdio_write,
                              a2fxxx_mdio_read);
#endif

// Declare TX / RX buffers
static cyg_uint32 a2fxxx_eth0_rxbufs[ RxBUFSIZE / 4 ];
static cyg_uint32 a2fxxx_eth0_txbufs[ TxBUFSIZE / 4 ];

// Declare BDs
static struct a2fxxx_bd
    a2fxxx_eth0_rxring[CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_RxNUM];
static struct a2fxxx_bd
    a2fxxx_eth0_txring[CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_TxNUM];

static struct a2fxxx_eth_info a2fxxx_eth0_info = {
#ifdef CYGDAT_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0_ESA
    .mac_address = CYGDAT_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0_ESA,
#endif
    .base        = CYGHWR_HAL_A2FXXX_MAC,
    .base_bb     = CYGHWR_HAL_A2FXXX_MAC_BB,
    .if_num      = 0,
    .init_rxbufs = (cyg_uint8 *) &a2fxxx_eth0_rxbufs[0],
    .init_txbufs = (cyg_uint8 *) &a2fxxx_eth0_txbufs[0],
    .init_rxring = &a2fxxx_eth0_rxring[0],
    .init_txring = &a2fxxx_eth0_txring[0],
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    .vector = CYGNUM_HAL_INTERRUPT_MAC0,
#endif
#ifdef CYGPKG_DEVS_ETH_PHY
    .phy = &eth0_phy,
#endif
};

ETH_DRV_SC(a2fxxx_eth0_sc,
           &a2fxxx_eth0_info,
           CYGDAT_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0_NAME,
           a2fxxx_eth_start,
           a2fxxx_eth_stop,
           a2fxxx_eth_control,
           a2fxxx_eth_can_send,
           a2fxxx_eth_send,
           a2fxxx_eth_recv,
           a2fxxx_eth_deliver,
           a2fxxx_eth_poll,
           a2fxxx_eth_int_vector);

NETDEVTAB_ENTRY(a2fxxx_netdev0,
                "a2fxxx_" CYGDAT_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0_NAME,
                a2fxxx_eth_init,
                &a2fxxx_eth0_sc);

#endif// CYGPKG_DEVS_ETH_CORTEXM_A2F200_EVAL_ETH0

#endif
