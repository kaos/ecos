//==========================================================================
//
//      if_freescale_enet.c
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
// Author(s):    Ilija Kocho <ilijak@siva.com.mk>
// Contributors:
// Date:         2011-06-04
// Description:  Hardware driver for Freescale ENET peripheral
//
//               This driver was originally written for Kinetis and may
//               require some modifications to work on other Freescale
//               families such as ColdFire+.
//
//####DESCRIPTIONEND####
//
//========================================================================*/


#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/devs_eth_freescale_enet.h>
#include <pkgconf/io_eth_drivers.h>

#if defined(CYGPKG_REDBOOT)
# include <pkgconf/redboot.h>
#endif

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>
#include <cyg/io/eth/eth_drv_stats.h>
#include <cyg/io/eth_phy.h>
#include <errno.h>
#include <string.h>

#ifdef CYGPKG_NET
# include <pkgconf/net.h>
# include <net/if.h>  //Needed for struct ifnet
#endif

#include <cyg/hal/hal_endian.h>
#include <cyg/io/eth/if_freescale_enet_bd.h>
#include <cyg/io/eth/if_freescale_enet_io.h>

// Some debugging helpers ---------------------------------------------------

#define DEBUG_ENET CYGPKG_DEVS_ETH_FREESCALE_ENET_DEBUG_LEVEL

#if DEBUG_ENET >= 3
# define debug3_printf(args...) diag_printf(args)
#else
# define debug3_printf(args...)
#endif

#if DEBUG_ENET >= 2
# define debug2_printf(args...) diag_printf(args)
#else
# define debug2_printf(args...)
#endif

#if DEBUG_ENET >= 1
# define debug1_printf(args...) diag_printf(args)
#else
# define debug1_printf(args...)
#endif


#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#define enet_eth_send enet_eth_send_nocopy
#define enet_eth_deliver enet_eth_deliver_nocopy
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#define enet_eth_send enet_eth_send_copying
#define enet_eth_deliver enet_eth_deliver_copying
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY


// Some delay macros ------------------------------------------------------
// Busy waiting delay
#define ENET_WAIT_US(__us) HAL_DELAY_US(__us)
// Delays / Timeouts
#define FREESCALE_ENET_RESET_DELAY   1      // Enet reset [us]
#define FREESCALE_ENET_ENABLE_DELAY  1      // Enet eneble [us]

#define FREESCALE_ENET_PHY_INIT_TOUT 100    // Phy start up [ms]
#define FREESCALE_ENET_MII_WRITE_TOUT 2000  // Phy write timeout [us]
#define FREESCALE_ENET_MII_READ_TOUT  2000  // Phy read timeout [us]

#ifndef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
// If kernel is available ENET_ETH_WAIT should be a normal,
// not a busy-waiting delay.
# ifdef CYGFUN_KERNEL_API_C
#  define ENET_ETH_WAIT(__tick)  cyg_thread_delay(__tick)
# elif !defined(CYGPKG_KERNEL)
#  define ENET_ETH_WAIT(__ms)  HAL_DELAY_US(__ms * 1000)
# else
#  error Can not define ENET_ETH_WAIT().
# endif
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Resources provided by HAL ==============================================
// Ethernet RAM and DMA configuration --------------------------------------

#ifdef CYGHWR_HAL_ENET_MEM_SECTION
# include <cyg/infra/cyg_type.h>
# define ENET_RAM_MEM_SECTION CYGBLD_ATTRIB_SECTION(CYGHWR_HAL_ENET_MEM_SECTION)
#else
# define ENET_RAM_MEM_SECTION
#endif // CYGHWR_HAL_ENET_MEM_SECTION


// IRQ masking --------------------------------------------------------------
//

#define FREESCALE_ENET_EIR_MASK_M (FREESCALE_ENET_EIR_TXF_M |     \
            FREESCALE_ENET_EIR_TXB_M | FREESCALE_ENET_EIR_RXF_M | \
                                   FREESCALE_ENET_EIR_RXB_M)

#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define FREESCALE_ENET_EIMR_MASK_M  (FREESCALE_ENET_EIMR_RXF_M | \
                                     FREESCALE_ENET_EIMR_TXF_M)
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define FREESCALE_ENET_EIMR_MASK_M  FREESCALE_ENET_EIMR_RXF_M
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY


// Buffer configuration macros ----------------------------------------------------
//

#define ENET_RXBD_ALIGN \
            CYGBLD_ATTRIB_ALIGN(CYGNUM_DEVS_ETH_FREESCALE_ENET_RXBD_ALIGN)
#define ENET_TXBD_ALIGN \
            CYGBLD_ATTRIB_ALIGN(CYGNUM_DEVS_ETH_FREESCALE_ENET_TXBD_ALIGN)
#define ENET_RXBUF_ALIGN \
            CYGBLD_ATTRIB_ALIGN(CYGNUM_DEVS_ETH_FREESCALE_ENET_RXBUF_ALIGN)
#define ENET_TXBUF_ALIGN \
            CYGBLD_ATTRIB_ALIGN(CYGNUM_DEVS_ETH_FREESCALE_ENET_TXBUF_ALIGN)

#ifdef  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define ENET_TXKEY_FLAG  ENET_TXBD_TO2
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY


// ENET statistics ----------------------------------------------------
//

#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS_DEBUG
# define ENET_STAT_PRINTF(args...) diag_printf(args)
#else //CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS_DEBUG
# define ENET_STAT_PRINTF(args...)
#endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS_DEBUG

#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS
typedef struct freescale_enet_stats_s {
    cyg_uint32 TR;     // Truncated
    cyg_uint32 CR;     // CRC or frame error
    cyg_uint32 OV;     // Overrun
    cyg_uint32 LG;     // Frame length violation
    cyg_uint32 NO;     // Non octet aligned frame
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    cyg_uint32 ME;     // MAC error
    cyg_uint32 CE;     // Receive CRC error
    cyg_uint32 PE;     // PHY error
    cyg_uint32 FRAG;   // IPv4 Fragment
    cyg_uint32 ICE;    // IP header chechsum error
    cyg_uint32 PCR;    // Protocol checksum error
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
} freescale_enet_stats_t;

void freescale_enet_stats_init(freescale_enet_stats_t *stats_p) {
    stats_p->TR = 0;
    stats_p->CR = 0;
    stats_p->OV = 0;
    stats_p->LG = 0;
    stats_p->NO = 0;
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    stats_p->ME = 0;
    stats_p->CE = 0;
    stats_p->PE = 0;
    stats_p->FRAG = 0;
    stats_p->ICE = 0;
    stats_p->PCR = 0;
#endif //CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
}

# define FREESCALE_ENET_STATS_INIT(__poin) freescale_enet_stats_init(__poin)
# define FREESCALE_ENET_STAT_COUNT(__poin,__val) (__poin->enet_stats.__val++)

# define FREESCALE_ENET_STAT_DO(__ctrl,__poin,__val)                \
CYG_MACRO_START                                                     \
    if(__ctrl & ENET_RXBD_##__val){                                 \
        FREESCALE_ENET_STAT_COUNT(__poin,__val);                    \
        ENET_STAT_PRINTF(#__val "(%d) ", __poin->enet_stats.__val); \
    }                                                               \
CYG_MACRO_END

#else

# define FREESCALE_ENET_STATS_INIT(__poin)
# define FREESCALE_ENET_STAT_COUNT(_poin,_val)
# define FREESCALE_ENET_STAT_DO(__ctrl,__poin,__val)

#endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS

// --------------------------------------------------------------
// RedBoot configuration options for managing ESAs for us

// Decide whether to have redboot config vars for it...
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && defined(CYGPKG_REDBOOT_NETWORKING)
#include <redboot.h>
#include <flash_config.h>

#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_REDBOOT_HOLDS_ESA_ETH0
RedBoot_config_option("Network hardware address [MAC] for eth0",
                      eth0_esa_data,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0);
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_REDBOOT_HOLDS_ESA_ETH0
#endif  // CYGPKG_REDBOOT_NETWORKING && CYGSEM_REDBOOT_FLASH_CONFIG

// and initialization code to read them
// - independent of whether we are building RedBoot right now:
#ifdef CYGPKG_DEVS_ETH_FREESCALE_ENET_REDBOOT_HOLDS_ESA
#include <cyg/hal/hal_if.h>
#ifndef CONFIG_ESA
    #define CONFIG_ESA (6)
#endif

#define CYGHWR_DEVS_ETH_FREESCALE_ENET_GET_ESA( mac_address, ok )        \
  CYG_MACRO_START                                                     \
  ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,     \
                                    "eth0_esa_data",                  \
                                    mac_address,                      \
                                    CONFIG_ESA);                      \
  CYG_MACRO_END
#endif // CYGPKG_DEVS_ETH_FREESCALE_ENET_REDBOOT_HOLDS_ESA


// ENET device ===============================================================
// Freescale ENET driver private data ----------------------------------------

typedef struct freescale_enet_priv_t {
    CYG_ADDRWORD enet_base;               // ENET base address
    eth_phy_access_t *phy_p;              // PHY access
    cyg_uint16 max_frame_len;             // Maximal frame length
    cyg_uint8 rxbd_num;                   // Number of Rx buffer descriptors
    cyg_uint8 txbd_num;                   // Number of Tx buffer descriptors
    cyg_uint8 pins_n;                     // Number of (R)MII pins
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_uint8 txbd_avail;                 // Avalable Tx buffer descriptors
    cyg_uint8 txkey_num;                  // Number of Tx key entries
    cyg_uint8 txbd_can_send_min;          // Minimal number of avail
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_uint8 res0;                       // Reserved
    cyg_uint16 txbuf_size;                // Tx buffer size
    cyg_uint8  *txbuf_p;                  // Tx buffer pool
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_uint16 rxbuf_size;                // Rx buffer size
    cyg_uint8 fc_pause;                   // Flow control pause length
    cyg_uint8 flags;                      // Various flags
    cyg_uint8 *rxbuf_p;                   // Rx buffer pool
    enet_bd_t *rxbd_pool_p;               // Rx buffer descriptor pool
    enet_bd_t *txbd_pool_p;               // TX buffer descriptor poll
    enet_bd_t *rxbd_p;                    // Current Rx BD
    enet_bd_t *txbd_head_p;               // Current Tx BD head
    enet_bd_t *txbd_tail_p;               // Current Tx BD tail
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_uint32 *txkey_pool_p;             // Tx key queue pool
    cyg_uint32 *txkey_head_p;             // Last Txkey entry
    cyg_uint32 *txkey_tail_p;             // Last Txkey released
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    const cyg_uint32 *pins_p;             // (R)MII pin configuration data
    cyg_uint8 *enaddr;                    // Default ethernet (MAC) address
    cyg_uint32 rx_intr_vector;
    cyg_uint32 rx_intr_prio;
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_interrupt rx_interrupt;
    cyg_handle_t  rx_interrupt_handle;
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_uint32 tx_intr_vector;
    cyg_uint32 tx_intr_prio;
# ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_interrupt tx_interrupt;
    cyg_handle_t  tx_interrupt_handle;
# endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS
    freescale_enet_stats_t enet_stats;
#endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS
} freescale_enet_priv_t;

// Forward declarations ======================================================

static void enet_write_phy(int reg_addr, int phy_addr, unsigned short data);
static bool enet_read_phy(int reg_addr, int phy_addr, unsigned short *data_p);
static void enet_init_phy(void);
static void enet_eth_phy_set(freescale_enet_priv_t *enet_priv_p);
static bool eth_phy_init_wait(eth_phy_access_t *phy_p, cyg_uint32 ms);
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32 enet_eth_isr(cyg_vector_t vector, cyg_addrword_t data);
#endif

// Device instances ===========================================================
//

// ENET0 ======================================================================
// (R)MII pins - Provided by HAL ----------------------------------------------

static const cyg_uint32 const enet0_pins[] = {
    // Both RMII and MII interface
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_MDIO,
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_MDC,
#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_PHY_RXER_USE
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_RXER,
#endif
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_RXD1,
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_RXD0,
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_TXEN,
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_TXD0,
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_TXD1,
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_PHY_RMII
    // RMII interface only
    CYGHWR_IO_FREESCALE_ENET0_PIN_RMII0_CRS_DV
#else
    // MII interface only
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_RXD3,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_RXD2,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_RXCLK,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_TXD2,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_TXCLK,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_TXD3,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_CRS,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MIIO_TXER,
    CYGHWR_IO_FREESCALE_ENET0_PIN_MII0_COL
#endif
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_1588
    ,
    // IEEE 1588 timers
    CYGHWR_IO_FREESCALE_ENET0_PIN_1588_CLKIN,

    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR0,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR1,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR2,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR3,

    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR0,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR1,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR2,
    CYGHWR_IO_FREESCALE_ENET0_PIN_E0_1588_TMR3
#endif
};

// Buffer and buffer descriptor memory --------------------------------------

// Buffer descriptors
enet_bd_t enet0_rxbd_pool[CYGNUM_DEVS_ETH_FREESCALE_ENET0_RX_BUFS]
ENET_RAM_MEM_SECTION ENET_RXBD_ALIGN;
enet_bd_t enet0_txbd_pool[CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS]
                         ENET_RAM_MEM_SECTION ENET_TXBD_ALIGN;

#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
// Tx key queue
cyg_uint32 enet0_txkey_pool[CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS]
                         ENET_RAM_MEM_SECTION;
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Buffers
cyg_uint8 enet0_rxbuf[CYGNUM_DEVS_ETH_FREESCALE_ENET0_RX_BUFS *
                      CYGNUM_DEVS_ETH_FREESCALE_ENET0_RXBUF_SIZE]
                      ENET_RAM_MEM_SECTION ENET_RXBUF_ALIGN;
#ifndef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
cyg_uint8 enet0_txbuf[CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS *
                      CYGNUM_DEVS_ETH_FREESCALE_ENET0_TXBUF_SIZE]
                      ENET_RAM_MEM_SECTION ENET_TXBUF_ALIGN;
#endif //  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

#ifdef  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# if CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS >= 8
#  define ENET0_TXBD_CAN_SEND_MIN 3
# elif CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS >= 6
#  define ENET0_TXBD_CAN_SEND_MIN 2
# else
#  define ENET0_TXBD_CAN_SEND_MIN 1
# endif
#endif

// PHY interface entry -----------------------------------------------------
static cyg_uint8 enet0_macaddr[16] =
                           {CYGPKG_DEVS_ETH_FREESCALE_ENET0_MACADDR};

//

ETH_PHY_REG_LEVEL_ACCESS_FUNS(freescale_enet0_phy,
                              enet_init_phy,
                              NULL,
                              enet_write_phy,
                              enet_read_phy);

// ENET0 device private data -------------------------------------------------
freescale_enet_priv_t enet0_eth0_priv = {
    .enet_base = CYGADDR_IO_ETH_FREESCALE_ENET0_BASE,
    .phy_p       = &freescale_enet0_phy,
    .max_frame_len = CYGNUM_DEVS_ETH_FREESCALE_ENET0_MAX_FRAME_LEN,
    .rxbd_num    = CYGNUM_DEVS_ETH_FREESCALE_ENET0_RX_BUFS,
    .rxbuf_size  = CYGNUM_DEVS_ETH_FREESCALE_ENET0_RXBUF_SIZE,
    .rxbuf_p     = enet0_rxbuf,
    .txbd_num    = CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS,
    .rxbd_pool_p = enet0_rxbd_pool,
    .txbd_pool_p = enet0_txbd_pool,
    .rxbd_p      = NULL,
    .txbd_head_p = NULL,
    .txbd_tail_p = NULL,
    .fc_pause    = CYGNUM_ETH_FREESCALE_ENET0_OPD_PAUSE_DUR,
    .flags       = CYGSEM_ETH_FREESCALE_ENET0_RCR_FCE,
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    .txbd_avail  = CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS,
    .txkey_pool_p = enet0_txkey_pool,
    .txkey_num    = CYGNUM_DEVS_ETH_FREESCALE_ENET0_TX_BUFS,
    .txbd_can_send_min = ENET0_TXBD_CAN_SEND_MIN,
    .txkey_head_p = NULL,
    .txkey_tail_p = NULL,
#else
    .txbuf_size   = CYGNUM_DEVS_ETH_FREESCALE_ENET0_TXBUF_SIZE,
    .txbuf_p      = enet0_txbuf,
#endif //  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    .pins_p      = enet0_pins,
    .pins_n      = sizeof(enet0_pins)/sizeof(enet0_pins[0]),
    .enaddr   = enet0_macaddr,
    .rx_intr_vector = CYGNUM_FREESCALE_ENET0_RECEIVE_INT_VECTOR,
    .rx_intr_prio = CYGPKG_DEVS_ETH_FREESCALE_ENET0_INTPRIO
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    , .tx_intr_vector = CYGNUM_FREESCALE_ENET0_TRANSMIT_INT_VECTOR,
    .tx_intr_prio = CYGPKG_DEVS_ETH_FREESCALE_ENET0_INTPRIO
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
};

// Ethernet interface entry -------------------------------------------------
ETH_DRV_SC(enet0_eth0_sc,
           &enet0_eth0_priv,                      // Driver specific data
           CYGDAT_IO_ETH_IF_FREESCALE_ENET0_NAME, // Name for this interface
           enet_eth_start,
           enet_eth_stop,
           enet_eth_control,
           enet_eth_can_send,
           enet_eth_send,
           enet_eth_recv,
           enet_eth_deliver,
           enet_eth_poll,
           enet_eth_int_vector);

NETDEVTAB_ENTRY(enet0_netdev,
                "enet0_eth",
                enet_eth_init,
                &enet0_eth0_sc);

// End ENET0 =================================================================


// Driver code ===============================================================
//

// Buffer and buffer descriptor (BD) macros ==================================
// BD Iterators --------------------------------------------------------------

// Generic BD iterator template used by Rx and Tx iterators
#define ENET_NEXT_BD(__bd_p,__priv_p,__wrap,__pool)    \
CYG_MACRO_START                                        \
        __bd_p = __bd_p->ctrl & __wrap ?               \
              __priv_p->__pool##_pool_p : __bd_p + 1;  \
CYG_MACRO_END

// Rx BD Iterator ------------------------------------------
#define ENET_RXBD_NEXT(__bd_p,__priv_p) \
        ENET_NEXT_BD(__bd_p, __priv_p, ENET_RXBD_W, rxbd)

// Tx BD Iterator
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
// Tx BD Iterator - NoCopy option --------------------------
# define ENET_BD_AVAIL_INC(__priv_p) (__priv_p->txbd_avail++)
# define ENET_BD_AVAIL_DEC(__priv_p) (__priv_p->txbd_avail--)

# define ENET_TXBD_ALLOC(__bd_p,__priv_p) \
CYG_MACRO_START \
    ENET_NEXT_BD(__bd_p, __priv_p, ENET_TXBD_W, txbd); \
    ENET_BD_AVAIL_DEC(__priv_p); \
CYG_MACRO_END

# define ENET_TXBD_FREE(__bd_p,__priv_p) \
CYG_MACRO_START \
    ENET_NEXT_BD(__bd_p, __priv_p, ENET_TXBD_W, txbd); \
    ENET_BD_AVAIL_INC(__priv_p); \
CYG_MACRO_END

# define ENET_TXBD_TRYFREE(__bd_p,__priv_p) \
CYG_MACRO_START \
    ENET_NEXT_BD(__bd_p, __priv_p, ENET_TXBD_W, txbd); \
CYG_MACRO_END

// Tx key iterator -----------------------------------------
# define ENET_TXKEY_NEXT(__bd_p,__priv_p)                                   \
CYG_MACRO_START                                                             \
    __bd_p = (__bd_p == (__priv_p->txkey_pool_p + __priv_p->txkey_num-1)) ? \
    __priv_p->txkey_pool_p : __bd_p + 1;                                    \
CYG_MACRO_END

#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Tx BD Iterator - Copying option -------------------------
# define ENET_TXBD_NEXT(__bd_p,__priv_p) \
        ENET_NEXT_BD(__bd_p, __priv_p, ENET_TXBD_W, txbd)

#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Buffer descriptor initialization functions ================================
//

#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define ENET_RESET_BD_TX(__priv_p) \
CYG_MACRO_START \
    __priv_p->txbd_head_p = __priv_p->txbd_pool_p + __priv_p->txbd_num - 1; \
    __priv_p->txbd_tail_p = __priv_p->txbd_head_p; \
    __priv_p->txbd_avail = __priv_p->txbd_num; \
    __priv_p->txkey_head_p = __priv_p->txkey_pool_p + __priv_p->txkey_num - 1; \
    __priv_p->txkey_tail_p = __priv_p->txkey_head_p; \
CYG_MACRO_END
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define ENET_RESET_BD_TX(__priv_p) \
CYG_MACRO_START \
    __priv_p->txbd_head_p = __priv_p->txbd_pool_p + __priv_p->txbd_num - 1; \
    __priv_p->txbd_tail_p = __priv_p->txbd_head_p; \
CYG_MACRO_END
#endif //

#define ENET_INIT_BD_TX(__priv_p) ENET_RESET_BD_TX(__priv_p)


// Initialize a buffer descriptor --------------------------------------------
// Generic function for initialization of a buffer descriptor

static void
enet_init_bd(enet_bd_t *bd_p, cyg_uint32 bd_size, cyg_uint16 cntrl_init,
             cyg_uint16 cntrl_wrap, cyg_uint8 *buff_p, cyg_uint32 buf_size
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
             , cyg_uint32 *key_p
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
             , cyg_uint32 ebd_ctrl
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
             )
{
    enet_bd_t *bd_end;
    debug1_printf("ENET: Inint_bd:\n");
    for(bd_end = bd_p + bd_size; bd_p < bd_end; bd_p++){
        debug1_printf("ENET:    %p -> ", bd_p);
        bd_p->ctrl = cntrl_init;
        bd_p->len = 0;
        bd_p->buffer_p = (cyg_uint8 *)CYG_CPU_TO_BE32((cyg_uint32)buff_p);
        debug1_printf("cntrl=0x%04x len=%d buffer=%p[%p]",
                      bd_p->ctrl, bd_p->len, bd_p->buffer_p, buff_p);
        buff_p += (buf_size);
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        bd_p->ebd_ctrl = ebd_ctrl;
        bd_p->hlen_proto = 0;
        bd_p->payload_csum = 0;
        bd_p->bdu = ENET_TXBD_BDU;
        bd_p->timestamp_1588 = 0;
        bd_p->reserved[0] = 0;
        bd_p->reserved[1] = 0;
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
        if(key_p) {
            *key_p = 0;
            debug1_printf(" tx_key[%p]=%x", key_p, *key_p);
            key_p++;
        }
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
        debug1_printf("\n");
    }
    debug1_printf("ENET: end, bd_p=%p\n", bd_p);
    bd_p--;
    bd_p->ctrl |=  cntrl_wrap;
    debug1_printf("ENET:    %p -> ", bd_p);
    debug1_printf("cntrl=0x%04x len=%d buffer=%p\n",
                  bd_p->ctrl, bd_p->len, bd_p->buffer_p);
}


// Initialize buffer descriptors and buffers --------------------------------
//

static void
enet_init_buffers(freescale_enet_priv_t *enet_priv_p)
{
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;

    // Initialize receive descriptor ring
    enet_priv_p->rxbd_p = enet_priv_p->rxbd_pool_p;
    enet_init_bd(enet_priv_p->rxbd_pool_p, enet_priv_p->rxbd_num,
                 ENET_RXBD_E, ENET_RXBD_W, enet_priv_p->rxbuf_p, enet_priv_p->rxbuf_size
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
                 , NULL
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                 , ENET_RXBD_INT
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                 );
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RDSR,
                     (cyg_uint32)enet_priv_p->rxbd_pool_p);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_MRBR, enet_priv_p->rxbuf_size);

    // Initialize transmit descriptor ring
    ENET_INIT_BD_TX(enet_priv_p);
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    enet_init_bd(enet_priv_p->txbd_pool_p, enet_priv_p->txbd_num, 0, ENET_TXBD_W,
                 NULL, 0, enet_priv_p->txkey_pool_p
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    enet_init_bd(enet_priv_p->txbd_pool_p, enet_priv_p->txbd_num, 0, ENET_TXBD_W,
                 enet_priv_p->txbuf_p, enet_priv_p->txbuf_size
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                 , ENET_TXBD_INT
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_TX_IP
                 | ENET_TXBD_IINS
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_TX_IP
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_TX_PROT
                 | ENET_TXBD_PINS
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_TX_PROT
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_1588
                 | ENET_TXBD_TS
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_1588
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                 );

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_TDSR,
                     (cyg_uint32)enet_priv_p->txbd_pool_p);
    enet_priv_p->txbd_tail_p = enet_priv_p->txbd_head_p;
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_TFWR,
                     FREESCALE_ENET_TFWR_STRFWD_M);
#else //CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_TFWR,
                     FREESCALE_ENET_TFWR_TFWR(FREESCALE_ENET_TFWR_TFWR_192));
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED

}

// Hardware initialization functions =========================================

// Configure ENET (R)MII pins ------------------------------------------------
// Note: pin acces uses services from HAL.

static void
enet_cfg_pins(freescale_enet_priv_t *enet_priv_p)
{
    const cyg_uint32 *pin_p;

    if((pin_p = enet_priv_p->pins_p)) {
        for(;
            pin_p < enet_priv_p->pins_p + enet_priv_p->pins_n;
            CYGHWR_IO_FREESCALE_ENET_PIN(*pin_p++));
    }
}

// Set a MAC address match. -------------------------------------------------
// Packets received which match this address will be passed on.

void
enet_set_mac_addr (CYG_ADDRWORD enet_base, const cyg_uint8 *addr_p)
{
    cyg_uint32 regval;

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_PALR,
                     (cyg_uint32)((addr_p[0]<<24) + (addr_p[1]<<16) +
                                  (addr_p[2]<<8) + addr_p[3]));
    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_PAUR, regval);
    regval &= 0x0000ffff;
    regval |= (cyg_uint32)((addr_p[4]<<24) + (addr_p[5]<<16));
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_PAUR, regval);
}


// Initialize ENET device ----------------------------------------------------
//

static bool
enet_eth_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *) sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
#ifdef CYGHWR_DEVS_ETH_FREESCALE_ENET_GET_ESA
    bool esa_ok = false;
#endif
    enet_cfg_pins(enet_priv_p);

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR, 0);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR, 0xFFFFFFFF);

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_ECR,
                     FREESCALE_ENET_ECR_RESET_M);
    ENET_WAIT_US(FREESCALE_ENET_RESET_DELAY);

#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_ECR,
                     FREESCALE_ENET_ECR_EN1588_M);
#else // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_ECR, 0);
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_drv_interrupt_create(enet_priv_p->rx_intr_vector,
                             enet_priv_p->rx_intr_prio,
                             (cyg_addrword_t)sc,
                             (cyg_ISR_t *)enet_eth_isr,
                             (cyg_DSR_t *)eth_drv_dsr,
                             &enet_priv_p->rx_interrupt_handle,
                             &enet_priv_p->rx_interrupt);
    cyg_drv_interrupt_attach(enet_priv_p->rx_interrupt_handle);
# ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_drv_interrupt_create(enet_priv_p->tx_intr_vector,
                             enet_priv_p->tx_intr_prio,
                             (cyg_addrword_t)sc,
                             (cyg_ISR_t *)enet_eth_isr,
                             (cyg_DSR_t *)eth_drv_dsr,
                             &enet_priv_p->tx_interrupt_handle,
                             &enet_priv_p->tx_interrupt);
    cyg_drv_interrupt_attach(enet_priv_p->tx_interrupt_handle);
# endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

#ifdef CYGHWR_DEVS_ETH_FREESCALE_ENET_GET_ESA
    // Get MAC address from RedBoot configuration variables
    CYGHWR_DEVS_ETH_FREESCALE_ENET_GET_ESA(&enet_priv_p->enaddr[0], esa_ok);
    // If this call fails myMacAddr is unchanged and MAC address from
    // CDL is used
    if (!esa_ok) {
       // Can't figure out ESA
       debug1_printf("Freescale ENET - Warning! ESA unknown\n");
    }
#endif
    // Set the source address for the controller
    enet_set_mac_addr (enet_base, enet_priv_p->enaddr);

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_IALR, 0x00000000);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_IAUR, 0x00000000);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_GALR, 0x00000000);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_GAUR, 0x00000000);

    if(eth_phy_init_wait(enet_priv_p->phy_p, FREESCALE_ENET_PHY_INIT_TOUT)) {
        //   Initialize upper level driver.
        (sc->funs->eth_drv->init)(sc, (cyg_uint8 *)enet_priv_p->enaddr);
        return true;
    } else {
        return false;
    }
}

// Start ENET ----------------------------------------------------------------

static void
enet_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr_p, int flags)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    cyg_uint32 enet_reg;


    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_ECR, enet_reg);
    if(!(enet_reg & FREESCALE_ENET_ECR_ETHEREN_M)) {
        // Set Speed, duplex, etc.
        enet_eth_phy_set(enet_priv_p);

        // Init Buffers
        enet_init_buffers(enet_priv_p);

        // Enable the device!

        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                         FREESCALE_ENET_EIR_MASK_M);
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
        cyg_drv_interrupt_unmask(enet_priv_p->rx_intr_vector);
# ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
        cyg_drv_interrupt_unmask(enet_priv_p->tx_intr_vector);
# endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR,
                         FREESCALE_ENET_EIMR_MASK_M);
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

        // Flow controll
        if(enet_priv_p->flags & CYGSEM_ETH_FREESCALE_ENET0_RCR_FCE) {
            debug1_printf("ENET: Flow control: RCR enabled.\n");
            HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_RCR, enet_reg);
            enet_reg |= FREESCALE_ENET_RCR_FCE_M;
            HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RCR, enet_reg);
        } else {
            debug1_printf("ENET: Flow control: RCR disabled.\n");
            HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_RCR, enet_reg);
            enet_reg &= ~FREESCALE_ENET_RCR_FCE_M;
            HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RCR, enet_reg);
        }
        debug1_printf("ENET: Flow control: RCR 0x%08x.\n", enet_reg);

        // Flow controll pause duration
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_OPD, enet_reg);
        enet_reg &= 0xffff000;
        enet_reg |= enet_priv_p->fc_pause;
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_OPD, enet_reg);
        debug1_printf("ENET: Flow control: pause %08x\n", enet_reg);

        // Enable ENET...
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_ECR, enet_reg);
        enet_reg |= FREESCALE_ENET_ECR_ETHEREN_M;
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_ECR, enet_reg);
        // ...and wait a bit to settle
        ENET_WAIT_US(FREESCALE_ENET_ENABLE_DELAY);
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RDAR,
                         FREESCALE_ENET_RDAR_RDAR_M);
#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS
        FREESCALE_ENET_STATS_INIT(&enet_priv_p->enet_stats);
#endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_STATS
    }
}

// Stop ENET ----------------------------------------------------------------

static void
enet_eth_stop(struct eth_drv_sc *sc)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    cyg_uint32 ecr;
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    enet_bd_t *tx_bd_head_p, *tx_bd_tail_p;
    cyg_uint32 *tx_key_tail_p;
    void (*tx_done)(struct eth_drv_sc *sc, CYG_ADDRESS key, int status);
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_drv_interrupt_mask(enet_priv_p->rx_intr_vector);
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_drv_interrupt_mask(enet_priv_p->tx_intr_vector);
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR, 0x00000000);
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                         FREESCALE_ENET_EIR_MASK_M);

    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_ECR, ecr);
    ecr &= ~FREESCALE_ENET_ECR_ETHEREN_M;
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_ECR, ecr);

#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    // Free eventual unsent buffers.
    tx_bd_head_p = enet_priv_p->txbd_head_p;
    tx_bd_tail_p = enet_priv_p->txbd_tail_p;
    tx_key_tail_p = enet_priv_p->txkey_tail_p;
    tx_done = sc->funs->eth_drv->tx_done;
    while(tx_bd_tail_p != tx_bd_head_p) {
        ENET_TXBD_FREE(tx_bd_tail_p, enet_priv_p);
        if(tx_bd_tail_p->ctrl & ENET_TXKEY_FLAG) {
            tx_bd_tail_p->ctrl &= ~ENET_TXKEY_FLAG;
            (tx_done)(sc, *tx_key_tail_p, 0);
            *tx_key_tail_p = 0;
            ENET_TXKEY_NEXT(tx_key_tail_p, enet_priv_p);
        }
    }
    enet_priv_p->txbd_tail_p = tx_bd_tail_p;
    enet_priv_p->txkey_tail_p = tx_key_tail_p;
#endif //CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
}

// ENET processing functions =================================================
// ENET control function -----------------------------------------------------

static int
enet_eth_control(struct eth_drv_sc *sc, unsigned long key,
                  void *data_p, int length)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    int retcode = 1;

    switch (key) {
    case ETH_DRV_SET_MAC_ADDRESS:
        if(length >= ETHER_ADDR_LEN) {
            enet_eth_stop(sc);
            enet_set_mac_addr(enet_base, (cyg_uint8 *)data_p);
            enet_eth_start(sc, (cyg_uint8 *)data_p, 0);
            retcode = 0;
        }
        break;
    default:
        break;
    }
    return retcode;
}

// Can send ------------------------------------------------------------------
//     NoCopy: returns number of free Tx buffer descriprors
//     Copying: returns 1 if there is at least 1 free Tx buffer descriotor

static int
enet_eth_can_send(struct eth_drv_sc *sc)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
#ifdef  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    cyg_int32 avail = enet_priv_p->txbd_avail;
# if DEBUG_ENET >= 2
    if(avail < enet_priv_p->txbd_can_send_min)
        diag_printf("ENET can_send: available = %d buffer descriptors.\n",
                      avail);
# endif // DEBUG_ENET
    return (int) avail < enet_priv_p->txbd_can_send_min ? 0 : avail;
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
    enet_bd_t *tx_bd_p = enet_priv_p->txbd_head_p;

    ENET_TXBD_NEXT(tx_bd_p, enet_priv_p);
    debug2_printf("ENET: can send: %d\n", !(tx_bd_p->ctrl & ENET_TXBD_R));
    if(!(tx_bd_p->ctrl & ENET_TXBD_R))
        return 1;
    else
        return 0;
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
}

// Send ----------------------------------------------------------------------
//
// Trigger ENET for sending --------------------------------------------------

static void inline
enet_eth_send_trigger(freescale_enet_priv_t *enet_priv_p)
{
    cyg_uint32 tdar;

    do {
        HAL_READ_UINT32(enet_priv_p->enet_base + FREESCALE_ENET_REG_TDAR, tdar);
    } while(tdar & FREESCALE_ENET_TDAR_TDAR_M);
    tdar = FREESCALE_ENET_TDAR_TDAR_M;
    HAL_WRITE_UINT32(enet_priv_p->enet_base + FREESCALE_ENET_REG_TDAR, tdar);
}

#define ENET_ETH_SEND_RDY(__bd_p,_ad_flag) \
CYG_MACRO_START \
    cyg_uint16 ctrl; \
    ctrl = __bd_p->ctrl & ENET_TXBD_W; \
    ctrl |= ENET_TXBD_R | ENET_TXBD_TC | (_ad_flag); \
    __bd_p->ctrl = ctrl; \
CYG_MACRO_END

// There different send functios for NoCopy and Copying driver options.
#ifdef  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Send - NoCopy version.
//      This version does not copy SG buffer but writes SG buf pointer
//      in ENET BD.
//      Note: ENET requires that buf pointer is evenly divisible by 8.

static void
enet_eth_send_nocopy(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list,
                     int sg_len, int total_len, unsigned long key)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    enet_bd_t *tx_bd_p;
    enet_bd_t *tx_bd0_p=NULL;
    cyg_uint32 *tx_key_p;
    struct eth_drv_sg *sg_list_z;

    if(enet_priv_p->txbd_avail < sg_len) {
        debug1_printf("ENET Send: Error! sg_len %d avail %d\n", sg_len, enet_priv_p->txbd_avail);
        (sc->funs->eth_drv->tx_done)(sc, key, 0);
        return;
    }
    debug3_printf("ENET Send: sg_len %d avail %d\n", sg_len, enet_priv_p->txbd_avail);
    tx_bd_p=enet_priv_p->txbd_head_p;
    for(sg_list_z=sg_list+sg_len; sg_list<sg_list_z; sg_list++) {
        ENET_TXBD_ALLOC(tx_bd_p, enet_priv_p);
        tx_bd_p->buffer_p=(cyg_uint8 *)CYG_CPU_TO_BE32(sg_list->buf);
        tx_bd_p->len=CYG_CPU_TO_BE16(sg_list->len);
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        tx_bd_p->bdu = 0;
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        if(tx_bd0_p) {
            ENET_ETH_SEND_RDY(tx_bd_p, 0);
        } else {
            tx_bd0_p=tx_bd_p;
        }
    }
    tx_key_p=enet_priv_p->txkey_head_p;
    *tx_key_p = key;
    ENET_TXKEY_NEXT(tx_key_p, enet_priv_p);
    if(tx_bd_p != tx_bd0_p) {
        debug3_printf("ENET Send2: Key[%p] = 0x%x\n", tx_key_p, key);
        tx_bd_p->ctrl |= (ENET_TXBD_L | ENET_TXKEY_FLAG); // is last and has key
        ENET_ETH_SEND_RDY(tx_bd0_p, 0);
    } else {
        debug3_printf("ENET Send1: Key[%p] = 0x%x\n", tx_key_p, key);
        ENET_ETH_SEND_RDY(tx_bd0_p, ENET_TXBD_L | ENET_TXKEY_FLAG);
    }
    enet_priv_p->txkey_head_p = tx_key_p;
    enet_priv_p->txbd_head_p = tx_bd_p;
    enet_eth_send_trigger(enet_priv_p);
}

#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Send - Copying version.
//      This version physically copies SG buf(s) in private ENET  buffer(s).
//      This version is usefull if upper layer stack does not guarantee
//      buffers aligned to 8 byte boundary.
//      Also requires no Tx interrupts.

static void
enet_eth_send_copying(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list,
                      int sg_len, int total_len, unsigned long key)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    enet_bd_t *tx_bd_p;
    struct eth_drv_sg *sg_list_z;
    cyg_uint16 ctrl;
    cyg_uint8* txbuf_p;
    cyg_uint32 txbuf_k;
    enet_bd_t *tx_bd_save_p;

    tx_bd_p=enet_priv_p->txbd_head_p;
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    while(!(tx_bd_p->bdu & ENET_TXBD_BDU)){
        debug1_printf("ENET Send: Waiting for BDU %p\n", tx_bd_p);
    }
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    debug2_printf("ENET Send: sg_len %d total_len %d\n", sg_len, total_len);
    tx_bd_save_p=tx_bd_p;
    ENET_TXBD_NEXT(tx_bd_p, enet_priv_p);
    txbuf_p = (cyg_uint8 *)CYG_CPU_TO_BE32((cyg_uint32)tx_bd_p->buffer_p);
    txbuf_k = 0;
    for(sg_list_z=sg_list+sg_len; sg_list<sg_list_z; sg_list++) {
        if((txbuf_k += sg_list->len) > enet_priv_p->txbuf_size) {
            debug1_printf("ENET Send: Buffer overflow: %d (max = %d)\n", txbuf_k,
                          enet_priv_p->txbuf_size);
            enet_priv_p->txbd_head_p = tx_bd_save_p;
            (sc->funs->eth_drv->tx_done)(sc, key, 0);
            return;
        }
        memcpy(txbuf_p, (cyg_uint8 *)sg_list->buf, sg_list->len);
        txbuf_p += sg_list->len;
    }
    tx_bd_p->len=CYG_CPU_TO_BE16(total_len);
    ctrl = tx_bd_p->ctrl & ENET_TXBD_W;
    ctrl |= ENET_TXBD_R | ENET_TXBD_TC | ENET_TXBD_L;
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    tx_bd_p->bdu = 0;
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    tx_bd_p->ctrl = ctrl;
    enet_priv_p->txbd_head_p = tx_bd_p;
    debug2_printf("ENET Send: trigger, done.\n");
    enet_eth_send_trigger(enet_priv_p);
    (sc->funs->eth_drv->tx_done)(sc, key, 0);
}

#endif //  CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Receive ------------------------------------------------------------------

static void
enet_eth_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    cyg_uint8 *rxbuffer_p;
    struct eth_drv_sg *sg_list_z;
    enet_bd_t *rx_bd_p;

    debug3_printf("ENET recv: sg_len=%d first len=%d\n", sg_len, sg_list->len);
    rx_bd_p=enet_priv_p->rxbd_p;
    rxbuffer_p=(cyg_uint8 *)CYG_CPU_TO_BE32((cyg_uint32)rx_bd_p->buffer_p);
    for(sg_list_z=sg_list+sg_len; sg_list<sg_list_z; sg_list++) {
        if(sg_list->buf) {
            memcpy((cyg_uint8 *)sg_list->buf, rxbuffer_p, sg_list->len);
            rxbuffer_p+=sg_list->len;
        }
    }
}

// Reception error check -----------------------------------------------------

static inline bool
enet_eth_recv_error(enet_bd_t *rx_bd_p, freescale_enet_priv_t *enet_priv_p)
{
    bool errcod=false;
    cyg_uint32 ctrl = rx_bd_p->ctrl;

    if(ctrl & ENET_RXBD_TR) {
        ENET_STAT_PRINTF("ENET: Ethernet trunc error ctrl = 0x%04x\n",
                         rx_bd_p->ctrl);
        FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, TR);
        errcod = true;
    } else if(ctrl & ENET_RXBD_L) {
        if(ctrl & (ENET_RXBD_CR | ENET_RXBD_OV | ENET_RXBD_LG | ENET_RXBD_NO))
        {
            // Ethernet fault processing
            ENET_STAT_PRINTF("ENET: Ethernet error ctrl = 0x%04x => ",
                             rx_bd_p->ctrl);
            FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, CR);
            FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, OV);
            FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, LG);
            FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, NO);
            ENET_STAT_PRINTF("\n");
            errcod = true;
        }

#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        ctrl = rx_bd_p->ebd_ctrl;
        if(ctrl & (ENET_RXBD_ME | ENET_RXBD_PE | ENET_RXBD_CE | ENET_RXBD_FRAG
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_RX_IP
                   | ENET_RXBD_ICE
#  ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_RX_PROT
                   | ENET_RXBD_PCR
#  endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_RX_PROT
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_ACCEL_RX_IP
                   ))
        {
            cyg_uint32 head_len = (rx_bd_p->hlen_proto >> 11) & 0x1f;
            if(head_len || (ctrl & (ENET_RXBD_ME | ENET_RXBD_PE |
                                    ENET_RXBD_CE | ENET_RXBD_FRAG )))
            {
                // Enhanced bd fault processing
                ENET_STAT_PRINTF("ENET: Enhanced bd error ctrl = 0x%08x => ",
                                 rx_bd_p->ebd_ctrl);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, ME);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, PE);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, CE);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, FRAG);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, ICE);
                FREESCALE_ENET_STAT_DO(ctrl, enet_priv_p, PCR);
                ENET_STAT_PRINTF("\n");
                ENET_STAT_PRINTF("ENET:   HeadLen=%d[0x%x] Proto=%d[0x%x]\n",
                                 head_len, head_len,
                                 rx_bd_p->hlen_proto & 0xff,
                                 rx_bd_p->hlen_proto & 0xff);
                errcod = true;
            }
        }
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    }
    if(errcod) {
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        debug2_printf("ENET: Bad msg ctrl = 0x%04x ebd_ctrl = 0x%08x\n",
                      rx_bd_p->ctrl, rx_bd_p->ebd_ctrl);
#else // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
        debug2_printf("ENET: Bad msg ctrl = 0x%04x\n", rx_bd_p->ctrl);
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
    }
    return errcod;
}

// Deliver -------------------------------------------------------------------
#define ENET_DELIVER_NEED_SERVICE_RX FREESCALE_ENET_EIMR_RXF_M
#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define ENET_DELIVER_NEED_SERVICE_TX FREESCALE_ENET_EIMR_TXF_M
# define ENET_DELIVER_NEED_SERVICE (ENET_DELIVER_NEED_SERVICE_RX | \
                                    ENET_DELIVER_NEED_SERVICE_TX)
#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY
# define ENET_DELIVER_NEED_SERVICE (ENET_DELIVER_NEED_SERVICE_RX)
#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

#define ENET_DELIVER_NEED_SERVICE_SET(__var,__rt) \
CYG_MACRO_START \
    __var |= ENET_DELIVER_NEED_SERVICE_##__rt; \
CYG_MACRO_END

#define ENET_DELIVER_NEED_SERVICE_CLEAR(__var,__rt) \
CYG_MACRO_START \
    __var &= ~ENET_DELIVER_NEED_SERVICE_##__rt; \
CYG_MACRO_END

// There different deliver functios for NoCopy and Copying driver options.

#ifdef CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Deliver - NoCopy varsion
//      This deliver vresion is compatible with NoCopy send version.

static void
enet_eth_deliver_nocopy(struct eth_drv_sc * sc)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    enet_bd_t *tx_bd_tail_p, *tx_bd_tail_save_p;
    cyg_uint32 *tx_key_tail_p;
    enet_bd_t *rx_bd_p;
    cyg_uint16 ctrl;
    cyg_uint32 eir;
    cyg_uint32 need_service;
    void (*recv)(struct eth_drv_sc *sc, int total_len);
    void (*tx_done)(struct eth_drv_sc *sc, CYG_ADDRESS key, int status);

    recv = sc->funs->eth_drv->recv;
    tx_done = sc->funs->eth_drv->tx_done;

    tx_bd_tail_p = enet_priv_p->txbd_tail_p;
    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
    need_service = eir & ENET_DELIVER_NEED_SERVICE;
    do {
        //Clear all ENET Interrupt events
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                         FREESCALE_ENET_EIR_MASK_M);
        if(eir & FREESCALE_ENET_EIR_ERROR) {
            debug2_printf("ENET: EIR Error 0x%08x\n",
                          eir & FREESCALE_ENET_EIR_ERROR);
            HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                             FREESCALE_ENET_EIR_ERROR);
        }

        // RX Event processing
        if(need_service & ENET_DELIVER_NEED_SERVICE_RX) {
            rx_bd_p=enet_priv_p->rxbd_p;
            ctrl=rx_bd_p->ctrl;
            if(ctrl & ENET_RXBD_E) {
                ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, RX);
            } else {
                if(!enet_eth_recv_error(rx_bd_p, enet_priv_p)) {
                    (recv)(sc, CYG_CPU_TO_BE16(rx_bd_p->len));
                }
                ctrl &= ENET_RXBD_W;
                ctrl |= ENET_RXBD_E;
                rx_bd_p->ctrl = ctrl;
# ifdef  CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                rx_bd_p->bdu = 0;
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                // Inform ENET that free Rx buffers have beeen freed.
                HAL_WRITE_UINT32(enet_priv_p->enet_base +
                                 FREESCALE_ENET_REG_RDAR,
                                 FREESCALE_ENET_RDAR_RDAR_M);

                ENET_RXBD_NEXT(rx_bd_p, enet_priv_p);
                enet_priv_p->rxbd_p=rx_bd_p;

                if(rx_bd_p->ctrl & ENET_RXBD_E) {
                    ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, RX);
                }
            }
        }

        // TX Event processing
        if(need_service & ENET_DELIVER_NEED_SERVICE_TX) {
            if(enet_priv_p->txbd_num == enet_priv_p->txbd_avail) {
                debug2_printf("ENET: Tx nothing to send\n");
                ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, TX);
            } else {
                tx_bd_tail_save_p=tx_bd_tail_p;
                ENET_TXBD_TRYFREE(tx_bd_tail_p, enet_priv_p);
                ctrl = tx_bd_tail_p->ctrl;
                if(ctrl & ENET_TXBD_R){
                    tx_bd_tail_p=tx_bd_tail_save_p;
                    ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, TX);
                } else {
                    ENET_BD_AVAIL_INC(enet_priv_p);
                    if(ctrl & ENET_TXKEY_FLAG){
# ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                        while(!(tx_bd_tail_p->bdu & ENET_TXBD_BDU)){
                            debug1_printf("ENET Send: Waiting for BDU %p\n", tx_bd_tail_p);
                        }
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                        tx_key_tail_p = enet_priv_p->txkey_tail_p;
                        debug3_printf("ENET Deliver: Key[%p] = 0x%x\n",
                                      tx_key_tail_p, *tx_key_tail_p);
                        ENET_TXKEY_NEXT(tx_key_tail_p, enet_priv_p);
                        enet_priv_p->txkey_tail_p = tx_key_tail_p;
                        ctrl &= ~ENET_TXKEY_FLAG;
                        tx_bd_tail_p->ctrl = ctrl;
                        (tx_done)(sc, *tx_key_tail_p, 0);
                    }
                    if(enet_priv_p->txbd_avail == enet_priv_p->txbd_num) {
                        debug3_printf("ENET: Tx All sent.\n");
                        ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, TX);
                    }
                }
            }
        }

        // Check if Interrupt had arrived in meantime
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
        need_service |= eir & ENET_DELIVER_NEED_SERVICE;
    } while(need_service);
    debug3_printf("ENET: Delivered.\n");
    enet_priv_p->txbd_tail_p = tx_bd_tail_p;
# ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
#  ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    //Unmask all ENET interrupts
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR,
                     FREESCALE_ENET_EIMR_MASK_M);
#  else // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    cyg_drv_interrupt_unmask(enet_priv_p->tx_intr_vector);
    cyg_drv_interrupt_unmask(enet_priv_p->rx_intr_vector);
#  endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
# endif //CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
}

#else // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Deliver - Copying varsion
//      This deliver vresion is compatible with Copying send version.

static void
enet_eth_deliver_copying(struct eth_drv_sc * sc)
{
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    enet_bd_t *rx_bd_p;
    cyg_uint16 ctrl;
    cyg_uint32 eir;
    cyg_uint32 need_service;
    void (*recv)(struct eth_drv_sc *sc, int total_len);

    recv = sc->funs->eth_drv->recv;
    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
    need_service = eir & ENET_DELIVER_NEED_SERVICE;
    do {
        //Clear all ENET Interrupt events
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                         FREESCALE_ENET_EIR_MASK_M);
        if(eir & FREESCALE_ENET_EIR_ERROR) {
            debug2_printf("ENET: EIR Error 0x%08x\n",
                          eir & FREESCALE_ENET_EIR_ERROR);
            HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR,
                             FREESCALE_ENET_EIR_ERROR);
        }

        // RX Event processing
        if(need_service & ENET_DELIVER_NEED_SERVICE_RX) {
            rx_bd_p=enet_priv_p->rxbd_p;
            ctrl=rx_bd_p->ctrl;
            if(ctrl & ENET_RXBD_E) {
                ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, RX);
            } else {
                if(!enet_eth_recv_error(rx_bd_p, enet_priv_p)) {
                    (recv)(sc, CYG_CPU_TO_BE16(rx_bd_p->len));
                }
                ctrl &= ENET_RXBD_W;
                ctrl |= ENET_RXBD_E;
                rx_bd_p->ctrl = ctrl;
# ifdef  CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                rx_bd_p->bdu = 0;
# endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_BD_ENHANCED
                // Inform ENET that Rx buffers have been freed.
                HAL_WRITE_UINT32(enet_priv_p->enet_base +
                                 FREESCALE_ENET_REG_RDAR,
                                 FREESCALE_ENET_RDAR_RDAR_M);

                ENET_RXBD_NEXT(rx_bd_p, enet_priv_p);
                enet_priv_p->rxbd_p=rx_bd_p;

                if(rx_bd_p->ctrl & ENET_RXBD_E) {
                    ENET_DELIVER_NEED_SERVICE_CLEAR(need_service, RX);
                }
            }
        }

        // Check if Interrupt had arrived in meantime
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
        need_service |= eir & ENET_DELIVER_NEED_SERVICE;
    } while(need_service);

# ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
#  ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    //Unmask all ENET interrupts
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR,
                     FREESCALE_ENET_EIMR_MASK_M);
#  else // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
//    cyg_drv_interrupt_unmask(enet_priv_p->tx_intr_vector);
    cyg_drv_interrupt_unmask(enet_priv_p->rx_intr_vector);
#  endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
# endif //CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
}

#endif // CYGOPT_ETH_FREESCALE_ENET_TX_NOCOPY

// Poll ---------------------------------------------------------------------
static void
enet_eth_poll(struct eth_drv_sc *sc)
{
    enet_eth_deliver(sc);
}

// ISR ----------------------------------------------------------------------
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32
enet_eth_isr(cyg_vector_t vector, cyg_addrword_t data)
{
#ifdef CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    struct eth_drv_sc *sc = (struct eth_drv_sc *)data;
    freescale_enet_priv_t *enet_priv_p =
          (freescale_enet_priv_t *)sc->driver_private;
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;

    //Mask ENET Interrupts
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIMR, 0x00000000);
#else // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    cyg_drv_interrupt_mask(vector);
#endif // CYGOPT_DEVS_ETH_FREESCALE_ENET_IRQ_FASTMASK
    cyg_drv_interrupt_acknowledge(vector);

    return CYG_ISR_CALL_DSR;
}

#endif //CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

static int
enet_eth_int_vector(struct eth_drv_sc *sc)
{
    return CYGNUM_FREESCALE_ENET0_RECEIVE_INT_VECTOR;
}

// PHY related stuff =========================================================
// MDIO interface

// Polling based waiting function for MDIO transactions
static bool
freescale_enet_busywait_mii(CYG_ADDRWORD enet_base, cyg_uint32 counter)
{
    cyg_uint32 eir;
    bool done=0;

    for(; counter; counter--) {
        ENET_WAIT_US(2);
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
        if(eir & FREESCALE_ENET_EIR_MII_M) {
            eir |= FREESCALE_ENET_EIR_MII_M;
            HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
            done=1;
            break;
        }
    }
    return done;
}

// Write a PHY register via the MDIO interface -------------------------------

static void
enet_write_phy(int reg_addr, int phy_addr, unsigned short data)
{
    CYG_ADDRWORD enet_base = enet0_eth0_priv.enet_base;
    cyg_uint32 eir;

    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
    eir |= FREESCALE_ENET_EIR_MII_M;
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_MMFR,
                     FREESCALE_ENET_MMFR_ST(01)       |
                     FREESCALE_ENET_MMFR_OP(01)       |
                     FREESCALE_ENET_MMFR_PA(phy_addr) |
                     FREESCALE_ENET_MMFR_RA(reg_addr) |
                     FREESCALE_ENET_MMFR_TA(02)       |
                     FREESCALE_ENET_MMFR_DATA(data));
    freescale_enet_busywait_mii(enet_base, FREESCALE_ENET_MII_WRITE_TOUT);
}

// Read a PHY register via the MDIO interface --------------------------------

static bool
enet_read_phy(int reg_addr, int phy_addr, unsigned short *data_p)
{
    bool done = 0;
    volatile cyg_uint32 mmfr;
    CYG_ADDRWORD enet_base = enet0_eth0_priv.enet_base;
    cyg_uint32 eir;

    HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);
    eir |= FREESCALE_ENET_EIR_MII_M;
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_EIR, eir);

    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_MMFR,
                     FREESCALE_ENET_MMFR_ST(01)       |
                     FREESCALE_ENET_MMFR_OP(02)       |
                     FREESCALE_ENET_MMFR_PA(phy_addr) |
                     FREESCALE_ENET_MMFR_RA(reg_addr) |
                     FREESCALE_ENET_MMFR_TA(02));
    if((done = freescale_enet_busywait_mii(enet_base,
                                           FREESCALE_ENET_MII_READ_TOUT))) {
        HAL_READ_UINT32(enet_base + FREESCALE_ENET_REG_MMFR, mmfr);
        *data_p = (unsigned short)(mmfr & FREESCALE_ENET_MMFR_DATA_M);
    }
    return done;
}


// PHY initialization functions ----------------------------------------------
// Init ENET device for phy access -------------------------------------------
//
static void
enet_init_phy(void)
{
    CYG_ADDRWORD enet_base = enet0_eth0_priv.enet_base;
    cyg_uint32 mii_speed;

    mii_speed = (2 * CYGHWR_FREESCALE_ENET_MII_MDC_HAL_CLOCK / 5000000 + 1);
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_MSCR,
                     FREESCALE_ENET_MSCR_MII_SPEED(mii_speed) |
                     FREESCALE_ENET_MSCR_HOLDTIME(0)
                     );
}

// Set ENET device according to PHY stat -------------------------------------
//
static void
enet_eth_phy_set(freescale_enet_priv_t *enet_priv_p)
{
    CYG_ADDRWORD enet_base = enet_priv_p->enet_base;
    unsigned short phy_state = 0;
    cyg_uint32 regval;

    phy_state=_eth_phy_state(enet_priv_p->phy_p);
#ifdef CYGSEM_DEVS_ETH_FREESCALE_ENET_PHY_RMII
    regval = FREESCALE_ENET_RCR_MAX_FL(enet_priv_p->max_frame_len)
          | FREESCALE_ENET_RCR_RMII_MODE_M | FREESCALE_ENET_RCR_MII_MODE_M;
    if((phy_state & ETH_PHY_STAT_LINK) && !(phy_state & ETH_PHY_STAT_100MB)) {
        // Operate in 10MB mode
        regval |= FREESCALE_ENET_RCR_RMII_10T_M;
    }
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RCR, regval);
#else // CYGSEM_DEVS_ETH_FREESCALE_ENET_PHY_RMII
    HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_RCR,
                     FREESCALE_ENET_RCR_MAX_FL(enet_priv_p->max_frame_len) |
                     FREESCALE_ENET_RCR_MII_MODE_M);
#endif // CYGSEM_DEVS_ETH_FREESCALE_ENET_PHY_RMII
    if(phy_state & ETH_PHY_STAT_FDX) { // Operate in full-duplex mode
        HAL_WRITE_UINT32(enet_base + FREESCALE_ENET_REG_TCR,
                         FREESCALE_ENET_TCR_FDEN_M);
    }
}


// PHY may need some start-up time -------------------------------------------
//

static bool
eth_phy_init_wait(eth_phy_access_t *phy_p, cyg_uint32 ms)
{
    cyg_uint32 cnt;

    for(cnt=0; cnt<ms; cnt++) {
        if(_eth_phy_init(phy_p))
            return true;
        ENET_WAIT_US(1000);
    }
    return false;
}

//----------------------------------------------------------------------------
// EOF if_freescale_enet.c
