//==========================================================================
//
//      if_lpc2xxx.c
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
// Contributors: ilijak
// Date:         2008-08-10
// Description:  Hardware driver for LPC2xxx on-chip EMAC peripheral
//
//               This driver was originally written for LPC2468 and may
//               require some modifications to work on other LPC2xxx
//               variants as well.
//
//####DESCRIPTIONEND####
//
//========================================================================*/


#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/devs_eth_arm_lpc2xxx.h>
#include <pkgconf/io_eth_drivers.h>
#if defined(CYGPKG_REDBOOT)
   #include <pkgconf/redboot.h>
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
#include <pkgconf/net.h>
#include <net/if.h>  /* Needed for struct ifnet */
#endif
#include <cyg/io/eth/eth_drv_stats.h>


//===========================================================================
//                                DEFINES
//===========================================================================
#ifndef CYGARC_HAL_LPC2XXX_REG_EMAC_BASE
#error "CYGARC_HAL_LPC2XXX_REG_EMAC_BASE not defined by varaint/platform."
#else
#define EMAC_BASE CYGARC_HAL_LPC2XXX_REG_EMAC_BASE
#endif

//
// MAC registers
//
#define EMAC_MAC1   (EMAC_BASE + 0x0000)
#define EMAC_MAC2   (EMAC_BASE + 0x0004)
#define EMAC_IPGT   (EMAC_BASE + 0x0008)
#define EMAC_IPGR   (EMAC_BASE + 0x000C)
#define EMAC_CLRT   (EMAC_BASE + 0x0010)
#define EMAC_MAXF   (EMAC_BASE + 0x0014)
#define EMAC_SUPP   (EMAC_BASE + 0x0018)
#define EMAC_TEST   (EMAC_BASE + 0x001C)
#define EMAC_MCFG   (EMAC_BASE + 0x0020)
#define EMAC_MCMD   (EMAC_BASE + 0x0024)
#define EMAC_MADR   (EMAC_BASE + 0x0028)
#define EMAC_MWTD   (EMAC_BASE + 0x002C)
#define EMAC_MRDD   (EMAC_BASE + 0x0030)
#define EMAC_MIND   (EMAC_BASE + 0x0034)
#define EMAC_SA0    (EMAC_BASE + 0x0040)
#define EMAC_SA1    (EMAC_BASE + 0x0044)
#define EMAC_SA2    (EMAC_BASE + 0x0048)

//
// Control registers
//
#define EMAC_CMD            (EMAC_BASE + 0x0100)
#define EMAC_STAT           (EMAC_BASE + 0x0104)
#define EMAC_RXDESC         (EMAC_BASE + 0x0108)
#define EMAC_RXSTAT         (EMAC_BASE + 0x010C)
#define EMAC_RXDESC_NUM     (EMAC_BASE + 0x0110)
#define EMAC_RX_PROD_IDX    (EMAC_BASE + 0x0114)
#define EMAC_RX_CONSUME_IDX (EMAC_BASE + 0x0118)
#define EMAC_TXDESC         (EMAC_BASE + 0x011C)
#define EMAC_TXSTAT         (EMAC_BASE + 0x0120)
#define EMAC_TXDESC_NUM     (EMAC_BASE + 0x0124)
#define EMAC_TX_PROD_IDX    (EMAC_BASE + 0x0128)
#define EMAC_TX_CONSUME_IDX (EMAC_BASE + 0x012C)
#define EMAC_TSV0           (EMAC_BASE + 0x0158)
#define EMAC_TSV1           (EMAC_BASE + 0x015C)
#define EMAC_RSV            (EMAC_BASE + 0x0160)
#define EMAC_FLOWCTRL_CNT   (EMAC_BASE + 0x0170)
#define EMAC_FLOWCTRL_STAT  (EMAC_BASE + 0x0174)

//
// Rx filter registers
//
#define EMAC_RXFILT_CTRL      (EMAC_BASE + 0x0200)
#define EMAC_RXFILT_WOL_STAT  (EMAC_BASE + 0x0204)
#define EMAC_RXFILT_WOL_CLR   (EMAC_BASE + 0x0204)
#define EMAC_HASH_FILT_L      (EMAC_BASE + 0x0210)
#define EMAC_HASH_FILT_H      (EMAC_BASE + 0x0214)

//
// Module control registers
//
#define EMAC_INT_STATUS       (EMAC_BASE + 0x0FE0)
#define EMAC_INT_EN           (EMAC_BASE + 0x0FE4)
#define EMAC_INT_CLR          (EMAC_BASE + 0x0FE8)
#define EMAC_INT_SET          (EMAC_BASE + 0x0FEC)
#define EMAC_POWER_DOWN       (EMAC_BASE + 0x0FF4)
#define EMAC_MODULE_ID        (EMAC_BASE + 0x0FFC)

//
// Register bits MCMD
//
#define  MCMD_WRITE           0x00
#define  MCMD_READ            0x01

//
// Register bits COMMAND register
//
#define CMD_RX_ENABLE        (1 << 0)
#define CMD_TX_ENABLE        (1 << 1)
#define CMD_REG_RESET        (1 << 3)
#define CMD_TX_RESET         (1 << 4)
#define CMD_RX_RESET         (1 << 5)
#define CMD_PASS_RUNT_FRAME  (1 << 6)
#define CMD_PASS_RX_FILTER   (1 << 7)
#define CMD_TX_FLOW_CONTROL  (1 << 8)
#define CMD_RMII             (1 << 9)
#define CMD_FULL_DUPLEX      (1 << 10)

//
// Register bits SUPP register
//
#define SUPP_SPEED_100MB     (1 << 8)

#define  EMAC_OLD_MODULE_ID  ((0x3902 << 16) | 0x2000)

//
// Register bits MAC1
//
#define MAC1_RX_EN                (1 << 0)
#define MAC1_PASS_ALL_RX_FRAMES   (1 << 1)
#define MAC1_RX_FLOW_CTRL         (1 << 2)
#define MAC1_TX_FLOW_CTRL         (1 << 3)
#define MAC1_LOOPBACK             (1 << 4)
#define MAC1_RESET_TX             (1 << 8)
#define MAC1_RESET_MCS_TX         (1 << 9)
#define MAC1_RESET_RX             (1 << 10)
#define MAC1_RESET_MCS_RX         (1 << 11)
#define MAC1_RESET_SIM            (1 << 14)
#define MAC1_SOFT_RESET           (1 << 15)

//
// Register bits MAC2
//
#define MAC2_FDX                  (1 << 0)
#define MAC2_FRAME_LEN_CHECK      (1 << 1)
#define MAC2_HUGE_FRAME_EN        (1 << 2)
#define MAC2_DELAYED_CRC          (1 << 3)
#define MAC2_CRC_EN               (1 << 4)
#define MAC2_PAD_CRC_EN           (1 << 5)
#define MAC2_VLAN_PAD_EN          (1 << 6)
#define MAC2_AUTO_DETECT_PAD_EN   (1 << 7)
#define MAC2_PURE_PREAMBLE        (1 << 8)
#define MAC2_LONG_PREAMBLE        (1 << 9)
#define MAC2_NO_BACKOFF           (1 << 12)
#define MAC2_BACK_PRESS           (1 << 13)
#define MAC2_EXCESS_DEFER         (1 << 14)

//
// Register bits RXFILT_CTRL
//
#define RXFILT_CTRL_ACC_UNICAST            (1 << 0)
#define RXFILT_CTRL_ACC_BROADCAST          (1 << 1)
#define RXFILT_CTRL_ACC_MULTICAST          (1 << 2)
#define RXFILT_CTRL_ACC_UNICAST_HASH       (1 << 3)
#define RXFILT_CTRL_ACC_MULTICAST_HASH     (1 << 4)
#define RXFILT_CTRL_ACC_PERFECT            (1 << 5)
#define RXFILT_CTRL_MAGIG_PKT_WOL          (1 << 12)
#define RXFILT_CTRL_RXFILT_WOL             (1 << 13)

//
// Register bits MCFG
//
#define MCFG_SCAN_INCR      (1 << 0)
#define MCFG_SUPP_PREAMBLE  (1 << 1)
#define MCFG_RESET_MII_MGMT (1 << 15)

//
// Register bits MIND
//
#define MIND_BUSY      (1 << 0)
#define MIND_SCANNING  (1 << 1)
#define MIND_NOT_VALID (1 << 2)
#define MIND_LINK_FAIL (1 << 3)

//
// EMAC interrupts
//
#define EMAC_INT_RXOVERRUN  (1 << 0)
#define EMAC_INT_RXERROR    (1 << 1)
#define EMAC_INT_RXFINISHED (1 << 2)
#define EMAC_INT_RXDONE     (1 << 3)
#define EMAC_INT_TXUNDERRUN (1 << 4)
#define EMAC_INT_TXERROR    (1 << 5)
#define EMAC_INT_TXFINISHED (1 << 6)
#define EMAC_INT_TXDONE     (1 << 7)
#define EMAC_INT_SOFTINT    (1 << 12)
#define EMAC_INT_WOL        (1 << 13)


//===========================================================================
//                ETHERNET RAM AND DMA CONFIGURATION
//===========================================================================
#ifdef CYGHWR_HAL_LPC_EMAC_RAM_AHB
# include <cyg/infra/cyg_type.h>
# define EMAC_RAM_MEM_SECTION   CYGBLD_ATTRIB_SECTION(CYGHWR_HAL_LPC_EMAC_MEM_SECTION)
# define EMAC_RAM_BASE          (&emac_ahb_ram)
# define EMAC_RAM_SIZE          sizeof(emac_ahb_ram)
# define EMAC_BLOCK_SIZE        CYGHWR_HAL_LPC_EMAC_BLOCK_SIZE
#else // Backward compatibility
# define EMAC_RAM_BASE          0x7FE00000
# define EMAC_RAM_SIZE          0x00004000
# define EMAC_BLOCK_SIZE        0x600
#endif // CYGHWR_HAL_LPC_EMAC_RAM_AHB

//
// EMAC Descriptor TX and RX Control fields
//
#define EMAC_TX_DESC_INT        0x80000000
#define EMAC_TX_DESC_LAST       0x40000000
#define EMAC_TX_DESC_CRC        0x20000000
#define EMAC_TX_DESC_PAD        0x10000000
#define EMAC_TX_DESC_HUGE       0x08000000
#define EMAC_TX_DESC_OVERRIDE   0x04000000
#define EMAC_RX_DESC_INT        0x80000000

//
// EMAC Descriptor status related definition
//
#define TX_DESC_STATUS_ERR      0x80000000
#define TX_DESC_STATUS_NODESC   0x40000000
#define TX_DESC_STATUS_UNDERRUN 0x20000000
#define TX_DESC_STATUS_LCOL     0x10000000
#define TX_DESC_STATUS_ECOL     0x08000000
#define TX_DESC_STATUS_EDEFER   0x04000000
#define TX_DESC_STATUS_DEFER    0x02000000
#define TX_DESC_STATUS_COLCNT   0x01E00000

#define RX_DESC_STATUS_ERR      0x80000000
#define RX_DESC_STATUS_LAST     0x40000000
#define RX_DESC_STATUS_NODESC   0x20000000
#define RX_DESC_STATUS_OVERRUN  0x10000000
#define RX_DESC_STATUS_ALGNERR  0x08000000
#define RX_DESC_STATUS_RNGERR   0x04000000
#define RX_DESC_STATUS_LENERR   0x02000000
#define RX_DESC_STATUS_SYMERR   0x01000000
#define RX_DESC_STATUS_CRCERR   0x00800000
#define RX_DESC_STATUS_BCAST    0x00400000
#define RX_DESC_STATUS_MCAST    0x00200000
#define RX_DESC_STATUS_FAILFLT  0x00100000
#define RX_DESC_STATUS_VLAN     0x00080000
#define RX_DESC_STATUS_CTLFRAM  0x00040000

#define DESC_SIZE_MASK          0x000007FF

//
// This type defines a descriptor
//
typedef struct descriptor_st
{
    cyg_uint32 packet;
    cyg_uint32 control;
} descriptor_t;

//
// This type defines a rx status
// (double word aligned)
//
typedef struct rx_status_st
{
    cyg_uint32 info;
    cyg_uint32 hash_crc;
} rx_status_t;

//
// This type defines a tx status
//
typedef cyg_uint32 tx_status_t; 
typedef union eth_buf_st
{
    cyg_uint32 dwords[EMAC_BLOCK_SIZE / sizeof(cyg_uint32)];
    cyg_uint8  bytes[EMAC_BLOCK_SIZE];
} eth_buf_t;


//
// Ethernet RAM and buffer configuration type
// The descriptors and statuses require a certain alignment. By defining each
// data type we assure this alignment.
// The TX descriptor array, the TX status array and the RX descriptor array
// need to be aligned on a 4 Byte (32 bit) address boundary. The RX status
// array need to be aligned on a 8 Byte (64 bit) address boundary.
//
typedef struct eth_ram_cfg_st
{
    volatile rx_status_t  rx_status[CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS];
    volatile descriptor_t rx_descr[CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS];
    volatile tx_status_t  tx_status[CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS];
    volatile descriptor_t tx_descr[CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS];
    volatile eth_buf_t    rx_buf[CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS];
} eth_ram_cfg_t;

#ifdef CYGHWR_HAL_LPC_EMAC_RAM_AHB
volatile static eth_ram_cfg_t emac_ahb_ram EMAC_RAM_MEM_SECTION;
#endif

#if CYGPKG_DEVS_ETH_ARM_LPC2XXX_DEBUG_LEVEL < 3
#if CYGPKG_DEVS_ETH_ARM_LPC2XXX_DEBUG_LEVEL > 0
   #define debug1_printf(args...) diag_printf(args)
#else
   #define debug1_printf(args...)
#endif
#if CYGPKG_DEVS_ETH_ARM_LPC2XXX_DEBUG_LEVEL > 1
   #define debug2_printf(args...) diag_printf(args)
#else
   #define debug2_printf(args...)
#endif
#else // CYGPKG_DEVS_ETH_ARM_LPC2XXX_DEBUG_LEVEL > 3
//
// Debug level 3 is a special kind of debug level. It prints the same debug
// messages like level 2 but it forces the debug output to the second serial
// port. This may be necessary for debugging the ethernet driver if the
// stand-alone network stack from redboot is used. It is possible to
// print debug messages while an application is loaded and run via GDB
// TCP/IP connection
//
static int cur;
#define debug1_printf(args...)                                               \
{                                                                            \
   cur =                                                                     \
   CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);\
   CYGACC_CALL_IF_SET_CONSOLE_COMM(1);                                       \
   diag_printf(args);                                                        \
   CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);                                     \
}
#define debug2_printf(args...)                                               \
{                                                                            \
   cur =                                                                     \
   CYGACC_CALL_IF_SET_CONSOLE_COMM(CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);\
   CYGACC_CALL_IF_SET_CONSOLE_COMM(1);                                       \
   diag_printf(args);                                                        \
   CYGACC_CALL_IF_SET_CONSOLE_COMM(cur);                                     \
}
#endif // CYGPKG_DEVS_ETH_ARM_LPC2XXX_DEBUG_LEVEL > 3


//Driver interface callbacks
#define _eth_drv_init(sc,mac)           \
  (sc->funs->eth_drv->init)(sc,(unsigned char *)mac)
#define _eth_drv_tx_done(sc,key,status)     \
  (sc->funs->eth_drv->tx_done)(sc,key,status)
#define _eth_drv_recv(sc,len)           \
  (sc->funs->eth_drv->recv)(sc,len)

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32
lpc2xxx_eth_isr (cyg_vector_t vector, cyg_addrword_t data);
#endif


// --------------------------------------------------------------
// RedBoot configuration options for managing ESAs for us

// Decide whether to have redboot config vars for it...
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && defined(CYGPKG_REDBOOT_NETWORKING)
#include <redboot.h>
#include <flash_config.h>

#ifdef CYGSEM_DEVS_ETH_ARM_LPC2XXX_REDBOOT_HOLDS_ESA_ETH0
RedBoot_config_option("Network hardware address [MAC] for eth0",
                      eth0_esa_data,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0);
#endif // CYGSEM_DEVS_ETH_ARM_LPC2XXX_REDBOOT_HOLDS_ESA_ETH0
#endif  // CYGPKG_REDBOOT_NETWORKING && CYGSEM_REDBOOT_FLASH_CONFIG

// and initialization code to read them
// - independent of whether we are building RedBoot right now:
#ifdef CYGPKG_DEVS_ETH_ARM_LPC2XXX_REDBOOT_HOLDS_ESA
#include <cyg/hal/hal_if.h>
#ifndef CONFIG_ESA
    #define CONFIG_ESA (6)
#endif

#define CYGHWR_DEVS_ETH_ARM_LPC2XXX_GET_ESA( mac_address, ok )        \
  CYG_MACRO_START                                                     \
  ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,     \
                                    "eth0_esa_data",                  \
                                    mac_address,                      \
                                    CONFIG_ESA);                      \
  CYG_MACRO_END
#endif // CYGPKG_DEVS_ETH_ARM_LPC2XXX_REDBOOT_HOLDS_ESA

//
// LPC2xxx Ethernet private data
// The LPC2468 has a single on-chip ethernet device, so all device-specific
// data could be held in statics. However, in case this driver ever gets
// re-used for a chip with multiple ethernet devices that data is held
// in a driver-specific structure.
//
typedef struct lpc2xxx_eth_priv_s
{
   cyg_uint32              intr_vector;
   cyg_uint8              *enaddr;
   cyg_uint32              base;      // Base address of device
   eth_phy_access_t       *phy;
   cyg_uint32              cur_tx_key;
   cyg_uint32              total_len;
   cyg_uint8               iterator;
   cyg_bool                tx_busy;
   volatile eth_ram_cfg_t *ram;
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
   cyg_interrupt intr;
   cyg_handle_t  intr_handle;
#endif
} lpc2xxx_eth_priv_t;


//===========================================================================
//                           PHY SPECIFIC STUFF
//===========================================================================


//===========================================================================
// Write one of the PHY registers via the MII bus
//===========================================================================
static void
lpc2xxx_write_phy(int reg_addr, int phy_addr, unsigned short data)
{
    cyg_uint32 regval;

    HAL_WRITE_UINT32(EMAC_MCMD, MCMD_WRITE);
    regval = (phy_addr << 8) | reg_addr;
    HAL_WRITE_UINT32(EMAC_MADR, regval);
    HAL_WRITE_UINT32(EMAC_MWTD, data);

    do
    {
        HAL_READ_UINT32(EMAC_MIND, regval);
    }
    while (regval != 0);
}


//===========================================================================
// Read one of the PHY registers via the MII bus
//===========================================================================
static bool
lpc2xxx_read_phy(int reg_addr, int phy_addr, unsigned short *data)
{
    cyg_uint32 regval;

    HAL_WRITE_UINT32(EMAC_MCMD, 0);
    regval = (phy_addr << 8) | reg_addr;
    HAL_WRITE_UINT32(EMAC_MADR, regval);
    HAL_WRITE_UINT32(EMAC_MCMD, MCMD_READ);

    do
    {
        HAL_READ_UINT32(EMAC_MIND, regval);
    }
    while ((regval & ~MIND_LINK_FAIL) != 0);

    if (regval & MIND_LINK_FAIL)
    {
        debug1_printf("LPC2XXX_ETH: PHY link fail\n");
    }

    HAL_WRITE_UINT32(EMAC_MCMD, 0x0000);
    HAL_READ_UINT32(EMAC_MRDD, regval);
    debug2_printf("LPC2XXX_ETH: read result %08x\n", regval);
    *data = regval & 0xFFFF;
    return true;
}


//===========================================================================
// Init device for phy access
//===========================================================================
static void lpc2xxx_init_phy(void)
{
    int        i;
    cyg_uint32 regval;

    //
    // host clock divided by 20, no suppress preamble, no scan increment
    // Reset management hardware
    //
    HAL_WRITE_UINT32(EMAC_MCFG, 0x0018 | MCFG_RESET_MII_MGMT);
    for (i = 0; i < 0x40; ++i)
    {
        asm volatile ("  nop");
    }
    HAL_WRITE_UINT32(EMAC_MCFG, 0x0018);
    HAL_WRITE_UINT32(EMAC_MCMD, MCMD_WRITE);

    HAL_READ_UINT32(EMAC_CMD, regval);
    HAL_WRITE_UINT32(EMAC_CMD, regval | CMD_RMII); // RMII configuration
    HAL_WRITE_UINT32(EMAC_SUPP, 0x800 | SUPP_SPEED_100MB);
    HAL_DELAY_US(50);
    HAL_WRITE_UINT32(EMAC_SUPP, SUPP_SPEED_100MB);
    HAL_DELAY_US(1000);
}


//===========================================================================
// PHY specific data structures
//===========================================================================
ETH_PHY_REG_LEVEL_ACCESS_FUNS(lpc2xxx_phy,
                              lpc2xxx_init_phy,
                              NULL,
                              lpc2xxx_write_phy,
                              lpc2xxx_read_phy);


//===========================================================================
// Hardware driver specific data
//===========================================================================                              
lpc2xxx_eth_priv_t lpc2xxx_priv_data =
{
   .intr_vector = CYGNUM_HAL_INTERRUPT_ETH,
   .base        = EMAC_BASE,
   .phy         = &lpc2xxx_phy,
   .ram         =  (volatile eth_ram_cfg_t*)EMAC_RAM_BASE,
   .total_len   = 0,
   .iterator    = 0,
   .tx_busy     = false
};


//===========================================================================
// Initialize TX descriptors
// To avoid copy operations the higher-level sg_list entries will be 
// directly put into the device's TX descriptors (ring buffer).
//===========================================================================
static void lpc2xxx_eth_txdesc_init(lpc2xxx_eth_priv_t * priv)
{
    int i;

    debug1_printf("\nLPC2XXX_ETH: Initialising TX descriptors\n");
    debug2_printf("&priv->ram->tx_descr[0]:  0x%08X\n", 
                  (cyg_uint32)priv->ram->tx_descr);
    debug2_printf("&priv->ram->tx_status[0]: 0x%08X\n",
                  (cyg_uint32)priv->ram->tx_descr);
    debug2_printf("No of TX descr: %d\n", 
                  CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS);

    //
    // Init status for all TX descriptors
    //
    for (i = 0; i < CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS; i++)
    {
        priv->ram->tx_status[i] = 0;
    }

    //
    // Initialise TX pointers
    //
    HAL_WRITE_UINT32(EMAC_TXDESC, (cyg_uint32)priv->ram->tx_descr);
    HAL_WRITE_UINT32(EMAC_TXSTAT, (cyg_uint32)priv->ram->tx_status);
    HAL_WRITE_UINT32(EMAC_TXDESC_NUM, CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS - 1);
    HAL_WRITE_UINT32(EMAC_TX_PROD_IDX, 0);
}


//===========================================================================
// Initialize RX descriptors
// The driver currently supports up to 4 rx buffers. In the current 
// implementation the size of the RX buffers is fixed to 1536 (0x600) bytes.
// Incoming frames are then copied to higher-level's code sg lists.
//===========================================================================
static void lpc2xxx_eth_rxdesc_init(lpc2xxx_eth_priv_t * priv)
{
    int i;

    debug1_printf("\nLPC2XXX_ETH: Initialising RX descriptors\n");
    debug2_printf("&priv->ram->rx_descr[0]:  0x%08X\n",
                  (cyg_uint32)priv->ram->rx_descr);
    debug2_printf("&priv->ram->rx_status[0]: 0x%08X\n",
                  (cyg_uint32)priv->ram->rx_status);
    debug2_printf("No of TX descr: %d\n", CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS);

    //
    // Initialise RX descriptors - store pointers to receive buffers and
    // RX status, two words, status info. and status hash CRC.
    //
    for (i = 0; i < CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS; ++i)
    {
        priv->ram->rx_descr[i].packet  = (cyg_uint32)priv->ram->rx_buf[i].bytes;
        debug2_printf("priv->ram->rx_descr[%d].packet (0x%08X) "
                      "= &priv->ram->rx_buf[%d].bytes[0] (0x%08X)\n", 
                      i, (cyg_uint32)&priv->ram->rx_descr[i].packet, 
                      i, (cyg_uint32)priv->ram->rx_buf[i].bytes);
        priv->ram->rx_descr[i].control = (EMAC_RX_DESC_INT 
                                       | ((EMAC_BLOCK_SIZE - 1)
                                          & DESC_SIZE_MASK));
        debug2_printf("priv->ram->rx_descr[%d].control (0x%08X) = 0x%08X\n", 
                      i, (cyg_uint32)&priv->ram->rx_descr[i].control, 
                      priv->ram->rx_descr[i].control);
        priv->ram->rx_status[i].info     = 0;
        priv->ram->rx_status[i].hash_crc = 0;
    }

    //
    // Initialise RX pointers
    //
    HAL_WRITE_UINT32(EMAC_RXDESC, (cyg_uint32)priv->ram->rx_descr);
    HAL_WRITE_UINT32(EMAC_RXSTAT, (cyg_uint32)priv->ram->rx_status);
    HAL_WRITE_UINT32(EMAC_RXDESC_NUM, CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS - 1);
    HAL_WRITE_UINT32(EMAC_RX_CONSUME_IDX, 0);
}


//===========================================================================
// Set a specific address match to a given address. Packets received which
// match this address will be passed on.
//===========================================================================
static void lpc2xxx_set_mac(lpc2xxx_eth_priv_t * priv, cyg_uint8 * enaddr)
{
    HAL_WRITE_UINT32(EMAC_SA0, (enaddr[5] << 8 | enaddr[4]));
    HAL_WRITE_UINT32(EMAC_SA1, (enaddr[3] << 8 | enaddr[2]));
    HAL_WRITE_UINT32(EMAC_SA2, (enaddr[1] << 8 | enaddr[0]));
}


//===========================================================================
// This function is called to stop the interface.
//===========================================================================
static void lpc2xxx_eth_stop(struct eth_drv_sc *sc)
{
    cyg_uint32          regval;

    // Disable the receiver and transmitter
    HAL_READ_UINT32(EMAC_MAC1, regval);
    HAL_WRITE_UINT32(EMAC_MAC1, regval & ~MAC1_RX_EN);
    HAL_READ_UINT32(EMAC_CMD, regval);
    HAL_WRITE_UINT32(EMAC_CMD, regval & ~(CMD_RX_ENABLE | CMD_TX_ENABLE));
}


//===========================================================================
// This function is called to "start up" the interface. It may be called
// multiple times, even when the hardware is already running.
//===========================================================================
static void
lpc2xxx_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    cyg_uint32  regval;
    
    //
    // To prevent overflow in the receive DMA engine the receive DMA engine 
    // should be enabled by setting the RxEnable bit in the Command register 
    // before enabling the receive datapath in the MAC by setting the 
    // RECEIVE ENABLE bit in the MAC1 register
    //
    HAL_READ_UINT32(EMAC_CMD, regval);
    HAL_WRITE_UINT32(EMAC_CMD, regval | CMD_RX_ENABLE | CMD_TX_ENABLE);
    HAL_READ_UINT32(EMAC_MAC1, regval);
    HAL_WRITE_UINT32(EMAC_MAC1, regval | MAC1_RX_EN);
}


//===========================================================================
// Enable and Disable of the receiver and transmitter.
// Initialize the interface. This configures the interface ready for use.
// Interrupts are grabbed etc. This means the start function has
// little to do except enable the receiver
//===========================================================================
static bool lpc2xxx_eth_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc    = (struct eth_drv_sc *)tab->device_instance;
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    bool esa_ok = false;
    unsigned char enaddr[6] = { CYGPKG_DEVS_ETH_ARM_LPC2XXX_MACADDR};
    unsigned short phy_state = 0;
    cyg_uint32 regval;

    debug1_printf("\nLPC2XXX_ETH: Initialising 0x%08X\n",priv->base);

    //
    // turn on the ethernet MAC clock in PCONP, bit 30 and then do
    // a short delay
    //
    HAL_READ_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE +
                    CYGARC_HAL_LPC24XX_REG_PCONP, regval);
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_SCB_BASE +
                     CYGARC_HAL_LPC24XX_REG_PCONP,
                     regval | CYGARC_HAL_LPC24XX_REG_PCONP_ENET);
    HAL_DELAY_US(50);

    //
    // Because of a device errate in Rev '-' devices we need to check the
    // module ID to setup the RMII mode properly
    //
    HAL_READ_UINT32(EMAC_MODULE_ID, regval);
    if (EMAC_OLD_MODULE_ID == regval)
    {
        //
        // On Rev. '-', EMAC_MODULE_ID should be equal to
        // EMAC_OLD_MODULE_ID, P1.6 should be set -
        // selects P1[0,1,4,6,8,9,10,14,15]
        //
        HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_PIN_BASE +
                        CYGARC_HAL_LPC24XX_REG_PINSEL2, 0x50151105);
    }
   else
   {
        //
        // on Rev. 'A', EMAC_MODULE_ID should not equal to
        // OLD_EMAC_MODULE_ID, P1.6 should not be set.
        // selects P1[0,1,4,8,9,10,14,15]
	    //
        HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_PIN_BASE +
                         CYGARC_HAL_LPC24XX_REG_PINSEL2, 0x50150105);

    } // if (EMAC_OLD_MODULE_ID == regval)

    // selects P1[17:16]
    HAL_WRITE_UINT32(CYGARC_HAL_LPC24XX_REG_PIN_BASE +
                     CYGARC_HAL_LPC24XX_REG_PINSEL3, 0x00000005);
    //
    // reset MAC modules, tx, mcs_tx, rx, mcs_rx, simulation and soft reset,
    // reset datapaths and host registers, disable Tx and RX and do a
    // short delay
    //
    HAL_WRITE_UINT32(EMAC_MAC1, MAC1_RESET_TX
    		                  | MAC1_RESET_MCS_TX
    		                  | MAC1_RESET_RX
    		                  | MAC1_RESET_MCS_RX
    		                  | MAC1_RESET_SIM
    		                  | MAC1_SOFT_RESET);
    HAL_WRITE_UINT32(EMAC_CMD, CMD_REG_RESET
    		                 | CMD_TX_RESET
    		                 | CMD_RX_RESET);
    HAL_DELAY_US(10);
    HAL_WRITE_UINT32(EMAC_MAC1, 0x0);
    lpc2xxx_eth_stop(sc); // disable transmitter and receiver
    
    HAL_WRITE_UINT32(EMAC_MAC2, 0); // initialize MAC2 register to default val.
    HAL_WRITE_UINT32(EMAC_IPGR, 0x12);   // manual recommends value 0x12
    HAL_WRITE_UINT32(EMAC_CLRT, 0x370F); // Using recommended value from manual.
    HAL_WRITE_UINT32(EMAC_MAXF, 0x0600); // manual recommends value 0x600

#ifdef CYGHWR_DEVS_ETH_ARM_LPC2XXX_GET_ESA
    // Get MAC address from RedBoot configuration variables
    CYGHWR_DEVS_ETH_ARM_LPC2XXX_GET_ESA(&enaddr[0], esa_ok);
    // If this call fails myMacAddr is unchanged and MAC address from
    // CDL is used
#endif

    if (!esa_ok)
    {
       // Can't figure out ESA
       debug1_printf("LPC2XXX_ETH - Warning! ESA unknown\n");
    }
    debug1_printf("LPC2XXX_ETH: %02x:%02x:%02x:%02x:%02x:%02x\n",
                  enaddr[0],enaddr[1],enaddr[2],
                  enaddr[3],enaddr[4],enaddr[5]);

    // Give the EMAC its address
    lpc2xxx_set_mac(priv, enaddr);
   
    // Setup the PHY
    CYG_ASSERTC(priv->phy);
    if (!_eth_phy_init(priv->phy))
    {
       return (false);
    }

    // Get the current mode and print it
    phy_state = _eth_phy_state(priv->phy);
    if (phy_state & ETH_PHY_STAT_LINK)
    {
        cyg_uint32 cmd;
        HAL_READ_UINT32(EMAC_CMD, cmd);
        cmd |= CMD_PASS_RUNT_FRAME;
    	HAL_READ_UINT32(EMAC_SUPP, regval);
        if (phy_state & ETH_PHY_STAT_100MB)
        {
            HAL_WRITE_UINT32(EMAC_SUPP, regval | SUPP_SPEED_100MB);
            debug1_printf("LPC2XXX_ETH: 100Mbyte/s");
        }
        else
        {
            HAL_WRITE_UINT32(EMAC_SUPP, regval &~ SUPP_SPEED_100MB);
            debug1_printf("LPC2XXX_ETH: 10Mbyte/s");
        }
        if(phy_state & ETH_PHY_STAT_FDX)
        {
            HAL_WRITE_UINT32(EMAC_MAC2, MAC2_FDX | MAC2_CRC_EN 
                             | MAC2_PAD_CRC_EN);
            // manual recommends value of 0x15 for FDX
            HAL_WRITE_UINT32(EMAC_IPGT, 0x15); 
            cmd |= CMD_FULL_DUPLEX;
            debug1_printf(" Full Duplex\n");
        }
        else
        {
            HAL_WRITE_UINT32(EMAC_MAC2, MAC2_CRC_EN | MAC2_PAD_CRC_EN);
            // manual recommends value of 0x12 for HDX
            HAL_WRITE_UINT32(EMAC_IPGT, 0x12); 
            debug1_printf(" Half Duplex\n");
        }
        HAL_WRITE_UINT32(EMAC_CMD, cmd);
    }
    else
    {
       debug1_printf("LPC2XXX_ETH: No Link\n");
    }
    
    lpc2xxx_eth_txdesc_init(priv);
    lpc2xxx_eth_rxdesc_init(priv);
    
    // set up the Rx filter
    // [0]-AllUnicast, [1]-AllBroadCast, [2]-AllMulticast, [3]-UnicastHash
    // [4]-MulticastHash, [5]-Perfect, [12]-MagicPacketEnWoL, 
    // [13]-RxFilterEnWoL
    HAL_WRITE_UINT32(EMAC_RXFILT_CTRL, RXFILT_CTRL_ACC_BROADCAST
    		                         | RXFILT_CTRL_ACC_PERFECT);
    //
    // If we are building an interrupt enabled version, install the
    // interrupt handler
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    debug1_printf("LPC2XXX_ETH: Installing Interrupts on IRQ %d\n",
                  priv->intr_vector);
    cyg_drv_interrupt_create(priv->intr_vector,
                             CYGPKG_DEVS_ETH_ARM_LPC2XXX_INTPRIO,
                             (cyg_addrword_t)sc,
                             lpc2xxx_eth_isr,
                             eth_drv_dsr,
                             &priv->intr_handle,
                             &priv->intr);

    cyg_drv_interrupt_attach(priv->intr_handle);
    cyg_drv_interrupt_unmask(priv->intr_vector);
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

    // Initialize the upper layer driver
    _eth_drv_init(sc, enaddr);
    
    // clear all interrupts
    HAL_WRITE_UINT32(EMAC_INT_CLR, 0xFFFF);
    
    //
    // Enable the interrupts we are interested in (not SoftInt and WoL)
    // We only want interrupts if a whole frame was transmitted so we 
    // enable only the TXFINISHED interrupt and not the TXDONE interrupt
    //
    HAL_WRITE_UINT32(EMAC_INT_EN, EMAC_INT_RXOVERRUN
    		                    | EMAC_INT_RXFINISHED
    		                    | EMAC_INT_RXDONE
    		                    | EMAC_INT_TXUNDERRUN
    		                    | EMAC_INT_TXERROR
    		                    | EMAC_INT_TXFINISHED);

    return (true);
}


//===========================================================================
// This function is called for low level "control" operations
//===========================================================================
static int
lpc2xxx_eth_control(struct eth_drv_sc *sc, unsigned long key,
                    void *data, int length)
{
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    switch (key)
    {
        case ETH_DRV_SET_MAC_ADDRESS:
        {
            if(length >= ETHER_ADDR_LEN)
            {
               lpc2xxx_eth_stop(sc);

               cyg_uint8 * enaddr = (cyg_uint8 *)data;
               debug1_printf("LPC2XXX_ETH: %02x:%02x:%02x:%02x:%02x:%02x\n",
                             enaddr[0],enaddr[1],enaddr[2],
                             enaddr[3],enaddr[4],enaddr[5]);

               lpc2xxx_set_mac(priv, enaddr);
               lpc2xxx_eth_start(sc,enaddr,0);
               return 0;
            }
            return 1;
        }

#ifdef CYGPKG_NET
        case ETH_DRV_GET_IF_STATS:
        {
            // todo
        }
#endif

        default:
        {
            diag_printf("%s.%d: key %lx\n", __FUNCTION__, __LINE__, key);
            return (1);
        }
   } // switch (key)
}


//===========================================================================
// This function is called to see if another packet can be sent.
// It should return the number of packets which can be handled.
// Zero should be returned if the interface is busy and can not send
// any more.
//===========================================================================
static int lpc2xxx_eth_can_send(struct eth_drv_sc *sc)
{
    int                 can_send;
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    if (priv->tx_busy)
    {
        can_send = 0;
    }
    else
    {
        can_send = 1;
    }

    debug2_printf("can_send: %d\n", can_send);
    return (can_send);
}

//===========================================================================
// This routine is called to send data to the hardware
// For tx the current code supports a single outgoing transmission at a
// time. This does limit outgoing bandwidth a bit, but only a bit, and
// saves memory and complexity.
//===========================================================================
static void
lpc2xxx_eth_send(struct        eth_drv_sc *sc,
                 struct        eth_drv_sg *sg_list,
                 int           sg_len,
                 int           total_len,
                 unsigned long key)
{
    cyg_uint32 tx_producer_idx;
    cyg_uint32 tx_consumer_idx;
    cyg_uint32 tx_desc_ctrl;
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    int i;

    priv->tx_busy = true;
    //
    // Store addresses of entries in scatter/gather list into tx descriptors
    //
    HAL_READ_UINT32(EMAC_TX_PROD_IDX, tx_producer_idx);
    HAL_READ_UINT32(EMAC_TX_CONSUME_IDX, tx_consumer_idx);
    debug2_printf("TPI: %d TCI: %d\n", tx_producer_idx, tx_consumer_idx);
    debug2_printf("TX: sg_len %d, total_len %d, key 0x%08lX\n",
                  sg_len, total_len, key);
    for(i = 0; i < sg_len; i++)
    {
        priv->ram->tx_descr[tx_producer_idx].packet = sg_list[i].buf;
        tx_desc_ctrl = (sg_list[i].len - 1) | EMAC_TX_DESC_INT;
        if (i == (sg_len - 1))
        {
            tx_desc_ctrl |= EMAC_TX_DESC_LAST;
        }
        priv->ram->tx_descr[tx_producer_idx].control = tx_desc_ctrl;
        priv->ram->tx_status[tx_producer_idx] = 0;

        tx_producer_idx = (tx_producer_idx + 1) %
                           CYGNUM_DEVS_ETH_ARM_LPC2XXX_TX_BUFS;
    }

    debug2_printf("TPI: %d TCI: %d\n", tx_producer_idx, tx_consumer_idx);
    // Store away the key for the time when the transmit has completed
    // and we need to tell the stack which transmit has completed
    // and then start the transmitter by incrementing the tx producer
    // index
    priv->cur_tx_key = key;
    HAL_WRITE_UINT32(EMAC_TX_PROD_IDX, tx_producer_idx);
}


//===========================================================================
// EMAC ISR
//===========================================================================
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32
lpc2xxx_eth_isr (cyg_vector_t vector, cyg_addrword_t data)
{
    debug2_printf("ISR\n");
    cyg_drv_interrupt_mask(vector);
    cyg_interrupt_acknowledge(vector);
    return CYG_ISR_CALL_DSR;
}
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED


//===========================================================================
// The high level DSR thaqt does all the work
//===========================================================================
static void lpc2xxx_eth_deliver(struct eth_drv_sc *sc)
{
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    cyg_uint32          intstatus;

    HAL_READ_UINT32(EMAC_INT_STATUS, intstatus);   
#ifndef CYGPKG_REDBOOT
    //
    // In redbot the ethernet driver is polled and this debug message would
    // pollute the redboot console
    //
    debug2_printf("INT_STATUS: %x\n", intstatus);
#endif // CYGPKG_REDBOOT

    if (intstatus & EMAC_INT_RXOVERRUN)
    {
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_RXOVERRUN);
        debug1_printf("LPC2XXX_ETH: RX overrun\n");
    }

    if (intstatus & EMAC_INT_TXUNDERRUN)
    {
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_TXUNDERRUN);
        debug1_printf("LPC2XXX_ETH: TX underrun\n");
    }

    if (intstatus & EMAC_INT_TXERROR)
    {
        cyg_uint32 tx_consumer_idx;
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_TXERROR);
        HAL_READ_UINT32(EMAC_TX_CONSUME_IDX, tx_consumer_idx);
        debug1_printf("LPC2XXX_ETH: TX error, status %08x\n",
                      priv->ram->tx_status[tx_consumer_idx]);
    }

    if (intstatus & EMAC_INT_SOFTINT)
    {
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_SOFTINT);
    }

    if (intstatus & EMAC_INT_WOL)
    {
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_WOL);
        debug1_printf("LPC2XXX_ETH: WOL\n");
    }

    //
    // TxFinishedInt
    // Interrupt triggered when all transmit descriptors have been
    // processed i.e. on the transition to the situation where
    // ProduceIndex == ConsumeIndex.
    //
    if (intstatus & EMAC_INT_TXFINISHED)
    {
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_TXFINISHED);
        _eth_drv_tx_done(sc, priv->cur_tx_key, 0);
        debug2_printf("TX finished - key: 0x%08X\n", priv->cur_tx_key);
        priv->tx_busy = false;
    }

    //
    // RxDoneInt
    // Interrupt triggered when a receive descriptor has been processed
    // while the Interrupt bit in the Control field of the descriptor was set.
    //
    if (intstatus & (EMAC_INT_RXDONE | EMAC_INT_RXFINISHED))
    {
        cyg_uint32 rx_producer_idx;
        cyg_uint32 rx_consumer_idx;

        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_RXDONE);
        HAL_WRITE_UINT32(EMAC_INT_CLR, EMAC_INT_RXFINISHED);
        HAL_READ_UINT32(EMAC_RX_PROD_IDX, rx_producer_idx);
        HAL_READ_UINT32(EMAC_RX_CONSUME_IDX, rx_consumer_idx);
        debug2_printf("\n\nRPIdx %d RCIdx %d\n", rx_producer_idx, 
                      rx_consumer_idx);

        //
        // Iterate through array of rx descriptors to check if the end of
        // frame was received.
        //
        while (priv->iterator != rx_producer_idx)
        {
            priv->total_len += ((priv->ram->rx_status[priv->iterator].info
                	             & DESC_SIZE_MASK) + 1);
            debug2_printf("total_len %d rx_status[%d] = %x rx_len %d\n",
               		      priv->total_len, priv->iterator,
               		      priv->ram->rx_status[priv->iterator].info,
               		      ((priv->ram->rx_status[priv->iterator].info
                	      & DESC_SIZE_MASK) + 1));
            if (priv->ram->rx_status[priv->iterator].info & RX_DESC_STATUS_LAST)
            {
                //
                // We found the end of the frame and have something to receive
                // so we can call _eth_drv_recv() now
                //
                if (priv->total_len)
                {
                    _eth_drv_recv(sc, priv->total_len);
                    HAL_READ_UINT32(EMAC_RX_CONSUME_IDX, rx_consumer_idx);
                    debug2_printf("RPIdx %d RCIdx %d Iterator %d\n",
                    		      rx_producer_idx, rx_consumer_idx,
                    		      priv->iterator);
                    priv->total_len = 0;
                } // if (total_len)
                else
                {
                    //
                    // If we do not call _eth_drv_recv then we need to set the
                    // consumer index here to indicate that the buffers are
                    // free for reception
                    //
                    HAL_WRITE_UINT32(EMAC_RX_CONSUME_IDX, priv->iterator);
                } // if (priv->total_len)
            } // if (priv->ram->rx_status[iterator].info & RX_DESC_STATUS_LAST)
            priv->iterator = (priv->iterator + 1) % 
                              CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS;
        }//if (priv->ram->rx_status[priv->iterator].info & RX_DESC_STATUS_LAST)

    } // if (intstatus & EMAC_INT_RXDONE)
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_drv_interrupt_unmask(priv->intr_vector);
#endif // CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
}


//===========================================================================
// This routine is called to receive data from the hardware
//===========================================================================
static void lpc2xxx_eth_recv(struct eth_drv_sc *sc,
                             struct eth_drv_sg *sg_list,
                             int    sg_len)
{
    lpc2xxx_eth_priv_t *priv = (lpc2xxx_eth_priv_t *)sc->driver_private;
    int i;
    cyg_uint32 rx_producer_idx;
    cyg_uint32 rx_consumer_idx;
    cyg_uint32 bytes_in_buffer;
    cyg_uint32 bytes_in_list = 0;
    cyg_uint32 bytes_needed_list = 0;
    cyg_uint32 buffer_pos = 0;
    cyg_uint32 total_bytes = 0;
    cyg_uint8 *sg_buf;
    cyg_uint8 *hw_rxbuf;

    HAL_READ_UINT32(EMAC_RX_PROD_IDX, rx_producer_idx);
    HAL_READ_UINT32(EMAC_RX_CONSUME_IDX, rx_consumer_idx);
    debug2_printf("lpc2xxx_eth_recv: sg_len %d\n", sg_len);

    for(i = 0; i < sg_len; ++i)
    {
        bytes_in_list = 0;
        while (bytes_in_list < sg_list[i].len)
        {
            bytes_needed_list = sg_list[i].len - bytes_in_list;
            debug2_printf("bil: %d sg_list[%d].len: %d bnl: %d\n",
                          bytes_in_list,
                          i,
                          sg_list[i].len,
                          bytes_needed_list); 
            bytes_in_buffer = (priv->ram->rx_status[rx_consumer_idx].info &
                               DESC_SIZE_MASK) + 1;
            debug2_printf("bib: %d buffer_pos: %d\n", bytes_in_buffer, 
                          buffer_pos);
            bytes_in_buffer -= buffer_pos;
            sg_buf   = (cyg_uint8 *)(sg_list[i].buf);
            hw_rxbuf = (cyg_uint8 *)priv->ram->rx_buf[rx_consumer_idx].bytes;
            if(bytes_needed_list < bytes_in_buffer)
            {
            	if(sg_buf)
                {
                    debug2_printf("(1) memcpy(0x%08X, 0x%08X, %d) "
                                  "[0x%08X - 0x%08X]\n", 
                                  (cyg_uint32)&sg_buf[bytes_in_list], 
                                  (cyg_uint32)&hw_rxbuf[buffer_pos],
                                   bytes_needed_list, 
                                  (cyg_uint32)&sg_buf[bytes_in_list],
                                  (cyg_uint32)&sg_buf[bytes_in_list] + 
                                  bytes_needed_list);
                                  
                    memcpy(&sg_buf[bytes_in_list], 
                           (cyg_uint8 *)&hw_rxbuf[buffer_pos],
                           bytes_needed_list);
                }
                bytes_in_list += bytes_needed_list;
                buffer_pos += bytes_needed_list;
                total_bytes += bytes_needed_list;
            }
            else
            {
            	if(sg_buf)
                {
                    debug2_printf("(2) memcpy(0x%08X, 0x%08X, %d)" 
                                  "[0x%08X - 0x%08X]\n", 
                                  (cyg_uint32)&sg_buf[bytes_in_list], 
                                  (cyg_uint32)&hw_rxbuf[buffer_pos],
                                  bytes_in_buffer, 
                                  (cyg_uint32)&sg_buf[bytes_in_list],
                                  (cyg_uint32)&sg_buf[bytes_in_list] + 
                                  bytes_in_buffer); 
                                                     
                    memcpy(&sg_buf[bytes_in_list], 
                           (cyg_uint8 *)&hw_rxbuf[buffer_pos],
                           bytes_in_buffer);
                }
                priv->ram->rx_status[rx_consumer_idx].info = 0;
                priv->ram->rx_status[rx_consumer_idx].hash_crc = 0;
                rx_consumer_idx = (rx_consumer_idx + 1) %
                                  CYGNUM_DEVS_ETH_ARM_LPC2XXX_RX_BUFS;
                HAL_WRITE_UINT32(EMAC_RX_CONSUME_IDX, rx_consumer_idx);
                debug2_printf("RCIdx++ %d\n", rx_consumer_idx);
                bytes_in_list += bytes_in_buffer;
                total_bytes += bytes_in_buffer;
                buffer_pos = 0;
                if (bytes_in_list < sg_list[i].len)
                {
                    debug2_printf("bytes_in_list < sg_list[i].len");
                }
            }
        } // while (bytes_in_list < sg_list[i].len)
    } // for(i = 0; i < sg_len; ++i)
}


//===========================================================================
// routine called to handle ethernet controller in polled mode
//===========================================================================
static void lpc2xxx_eth_poll(struct eth_drv_sc *sc)
{
    lpc2xxx_eth_deliver(sc);
}


//===========================================================================
// routine called to handle ethernet controller in polled mode
//===========================================================================
static int lpc2xxx_eth_int_vector(struct eth_drv_sc *sc)
{
   return CYGNUM_HAL_INTERRUPT_ETH;
}


//===========================================================================
// Ethernet driver instance
//===========================================================================
ETH_DRV_SC(lpc2xxx_sc,
           &lpc2xxx_priv_data,       // Driver specific data
           "eth0",                   // Name for this interface
           lpc2xxx_eth_start,
           lpc2xxx_eth_stop,
           lpc2xxx_eth_control,
           lpc2xxx_eth_can_send,
           lpc2xxx_eth_send,
           lpc2xxx_eth_recv,
           lpc2xxx_eth_deliver,
           lpc2xxx_eth_poll,
           lpc2xxx_eth_int_vector);

NETDEVTAB_ENTRY(lpc2xxx_netdev,
                "lpc2xxx",
                lpc2xxx_eth_init,
                &lpc2xxx_sc);
//---------------------------------------------------------------------------
// EOF if_lpc2xxx.c
