//==========================================================================
//
//      if_at91.c
//
//
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2006, 2010 Free Software Foundation, Inc.
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
// Author(s):    Andrew Lunn, John Eigelaar
// Contributors:  
// Date:         2006-05-10
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/devs_eth_arm_at91.h>
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

// Set up the level of debug output
#if CYGPKG_DEVS_ETH_ARM_AT91_DEBUG_LEVEL > 0
   #define debug1_printf(args...) diag_printf(args)
#else
   #define debug1_printf(args...) 
#endif
#if CYGPKG_DEVS_ETH_ARM_AT91_DEBUG_LEVEL > 1
   #define debug2_printf(args...) diag_printf(args)
#else
   #define debug2_printf(args...) 
#endif

//Driver interface callbacks
#define _eth_drv_init(sc,mac)			\
  (sc->funs->eth_drv->init)(sc,(unsigned char *)mac)
#define _eth_drv_tx_done(sc,key,status)		\
  (sc->funs->eth_drv->tx_done)(sc,key,status) 
#define _eth_drv_recv(sc,len)			\
  (sc->funs->eth_drv->recv)(sc,len) 

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32
at91_eth_isr (cyg_vector_t vector, cyg_addrword_t data);
#endif

// --------------------------------------------------------------
// RedBoot configuration options for managing ESAs for us

// Decide whether to have redboot config vars for it...
#if defined(CYGSEM_REDBOOT_FLASH_CONFIG) && defined(CYGPKG_REDBOOT_NETWORKING)
   #include <redboot.h>
   #include <flash_config.h>

   #ifdef CYGSEM_DEVS_ETH_ARM_AT91_REDBOOT_HOLDS_ESA_ETH0
RedBoot_config_option("Network hardware address [MAC] for eth0",
                      eth0_esa_data,
                      ALWAYS_ENABLED, true,
                      CONFIG_ESA, 0);
   #endif

#endif  // CYGPKG_REDBOOT_NETWORKING && CYGSEM_REDBOOT_FLASH_CONFIG

// and initialization code to read them
// - independent of whether we are building RedBoot right now:
#ifdef CYGPKG_DEVS_ETH_ARM_AT91_REDBOOT_HOLDS_ESA

   #include <cyg/hal/hal_if.h>

   #ifndef CONFIG_ESA
      #define CONFIG_ESA (6)
   #endif

  #define CYGHWR_DEVS_ETH_ARM_AT91_GET_ESA( mac_address, ok )           \
  CYG_MACRO_START                                                       \
  ok = CYGACC_CALL_IF_FLASH_CFG_OP( CYGNUM_CALL_IF_FLASH_CFG_GET,       \
                                    "eth0_esa_data",                    \
                                    mac_address,                        \
                                    CONFIG_ESA);                        \
  CYG_MACRO_END

#endif // CYGPKG_DEVS_ETH_AT91_ETH_REDBOOT_HOLDS_ESA

//============================================================================

// Private Data structures

#ifndef AT91_EMAC_RX_BUFF_SIZE
#define AT91_EMAC_RX_BUFF_SIZE  128
#endif

// Receive Buffer Descriptor
typedef struct rbd_s
{
   cyg_uint32 addr;
   cyg_uint32 sr;
} rbd_t;

// Receive Buffer
typedef struct rb_s 
{
   cyg_uint8 rb[AT91_EMAC_RX_BUFF_SIZE];
} rb_t;

// Transmit Buffer Descriptor
typedef struct tbd_s
{
   cyg_uint32 addr;
   cyg_uint32 sr;
} tbd_t;

// AT91 Ethernet private data
typedef struct at91_eth_priv_s 
{
   cyg_uint32    intr_vector;
   char *esa_key;      // RedBoot 'key' for device ESA
   cyg_uint8 *enaddr;
   cyg_uint32 base;    // Base address of device
   eth_phy_access_t *phy;
   rbd_t rbd[CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS];
   rb_t  rb[CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS];
   tbd_t tbd[CYGNUM_DEVS_ETH_ARM_AT91_TX_BUFS];
   unsigned long curr_tx_key;
   cyg_bool tx_busy;
   cyg_uint32 last_tbd_idx;
   cyg_uint32 curr_tbd_idx;
   cyg_uint32 curr_rbd_idx;
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
   cyg_interrupt intr;
   cyg_handle_t  intr_handle;
#endif
} at91_eth_priv_t;

//============================================================================
// PHY access bits and pieces
// 

static void 
at91_mdio_enable(void)
{
   cyg_uint32 val;
   HAL_READ_UINT32(AT91_EMAC + AT91_EMAC_NCR, val);
   val |= AT91_EMAC_NCR_MPE;    /* enable management port */
   HAL_WRITE_UINT32(AT91_EMAC + AT91_EMAC_NCR, val);
}

static void 
at91_mdio_disable(void)
{
   cyg_uint32 val;
   HAL_READ_UINT32(AT91_EMAC + AT91_EMAC_NCR, val);
   val &= ~AT91_EMAC_NCR_MPE;    /* disable management port */
   HAL_WRITE_UINT32(AT91_EMAC + AT91_EMAC_NCR, val);
}

// Write one of the PHY registers via the MII bus
static void
at91_write_phy(int reg_addr, int phy_addr, unsigned short data)
{
   cyg_uint32 val, cnt=0;

   CYG_ASSERTC(reg_addr >= 0 && reg_addr <= AT91_EMAC_MAN_REGA_MASK);
   CYG_ASSERTC(phy_addr >= 0 && phy_addr <= AT91_EMAC_MAN_PHY_MASK);

   val = (AT91_EMAC_MAN_SOF  |
         AT91_EMAC_MAN_WR   |
         AT91_EMAC_MAN_CODE |
         AT91_EMAC_MAN_PHYA(phy_addr) |
         AT91_EMAC_MAN_REGA(reg_addr) |
         AT91_EMAC_MAN_DATA(data));

   HAL_WRITE_UINT32(AT91_EMAC + AT91_EMAC_MAN, val);

   /* Wait until IDLE bit in Network Status register is set */
   while (cnt < 1000000)
   {
      HAL_READ_UINT32((AT91_EMAC + AT91_EMAC_NSR), val);
      if (!(val & AT91_EMAC_NSR_IDLE))
         break;
   }
   CYG_ASSERTC(cnt < 1000000);
}


// Read one of the PHY registers via the MII bus
static bool
at91_read_phy(int reg_addr, int phy_addr, unsigned short *data)
{
   cyg_uint32 val;

   CYG_ASSERTC(reg_addr >= 0 && reg_addr <= AT91_EMAC_MAN_REGA_MASK);
   CYG_ASSERTC(phy_addr >= 0 && phy_addr <= AT91_EMAC_MAN_PHY_MASK);

   val = (AT91_EMAC_MAN_SOF  |
         AT91_EMAC_MAN_RD   |
         AT91_EMAC_MAN_CODE |
         AT91_EMAC_MAN_PHYA(phy_addr) |
         AT91_EMAC_MAN_REGA(reg_addr));


   HAL_WRITE_UINT32(AT91_EMAC + AT91_EMAC_MAN, val);
   /* Wait until IDLE bit in Network Status register is set */
   do
   {
      HAL_READ_UINT32((AT91_EMAC + AT91_EMAC_NSR), val);
   }while(!(val & AT91_EMAC_NSR_IDLE));
   
   HAL_DELAY_US(50);

   HAL_READ_UINT32(AT91_EMAC + AT91_EMAC_MAN, val);
   *data = val & AT91_EMAC_MAN_DATA_MASK;

   return (true);
}

// Enable the MDIO bit in MAC control register so that we can talk to
// the PHY. Also set the clock divider so that MDC is less than 2.5MHz.
static void 
at91_init_phy(void)
{
   cyg_uint32 cfg;
   cyg_uint32 div;

   HAL_READ_UINT32(AT91_EMAC + AT91_EMAC_NCFG, cfg);
   cfg &=~ AT91_EMAC_NCFG_CLK_MASK;

   div = (CYGNUM_HAL_ARM_AT91_CLOCK_SPEED / 2500000);
   if (div < 8)
   {
      cfg |= AT91_EMAC_NCFG_CLK_HCLK_8;
   }
   else if (div < 16)
   {
      cfg |= AT91_EMAC_NCFG_CLK_HCLK_16;
   }
   else if (div < 32)
   {
      cfg |= AT91_EMAC_NCFG_CLK_HCLK_32;
   }
   else if (div < 64)
   {
      cfg |= AT91_EMAC_NCFG_CLK_HCLK_64;
   }
   else
   {
      CYG_FAIL("Unable to program MII clock");
   }

   HAL_WRITE_UINT32(AT91_EMAC + AT91_EMAC_NCFG, cfg);
}

ETH_PHY_REG_LEVEL_ACCESS_FUNS(at91_phy, 
                              at91_init_phy,
                              NULL,
                              at91_write_phy,
                              at91_read_phy);

//======================================================================
// Receiver buffer handling

// Initialize the receiver buffers and descriptors
static void
at91_rb_init(at91_eth_priv_t *priv)
{
   int i;
   for (i = 0 ; i < CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS; i++)
   {
      priv->rbd[i].addr = ((cyg_uint32)&priv->rb[i]) & AT91_EMAC_RBD_ADDR_MASK;
      priv->rbd[i].sr = 0;
   }
   // Set the wrap bit on the last entry
   priv->rbd[CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS-1].addr |= 
     AT91_EMAC_RBD_ADDR_WRAP;
}

//======================================================================
// Transmit buffer handling

// Initialize the transmit buffer descriptors
static void 
at91_tb_init(at91_eth_priv_t *priv)
{
   int i;
   for (i = 0 ; i < CYGNUM_DEVS_ETH_ARM_AT91_TX_BUFS; i++)
   {
      priv->tbd[i].addr = 0;
      priv->tbd[i].sr = AT91_EMAC_TBD_SR_USED;
   }
   // Set the wrap bit on the last entry
   priv->tbd[CYGNUM_DEVS_ETH_ARM_AT91_TX_BUFS-1].sr |= AT91_EMAC_TBD_SR_WRAP;
}

//======================================================================
// Enable and Disable of the receiver and transmitter.

static void
at91_disable_rx(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl &= ~AT91_EMAC_NCR_RE;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}

static void
at91_disable_tx(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl &= ~AT91_EMAC_NCR_TX;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}

static void
at91_enable_rx(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl |= AT91_EMAC_NCR_RE;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}

static void
at91_enable_tx(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl |= AT91_EMAC_NCR_TX;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}

static void 
at91_enable(at91_eth_priv_t *priv)
{
   at91_enable_tx(priv);
   at91_enable_rx(priv);
}

static void 
at91_disable(at91_eth_priv_t *priv)
{
   at91_disable_tx(priv);
   at91_disable_rx(priv);
}

static void
at91_start_transmitter(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl |= AT91_EMAC_NCR_TSTART;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}


//======================================================================
// Initialization code

// Configure the pins so that the EMAC has control of them. This
// assumes the MII is used, not the RMII
static void
at91_cfg_pins(void)
{
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_EREFCK);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ECRS);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ECOL);

   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERXDV);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERX0);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERX1);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERX2);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERX3);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERXER);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ERXCK);

   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETXEN);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETX0);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETX1);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETX2);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETX3);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_ETXER);

   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_EMDC);
   HAL_ARM_AT91_PIO_CFG(AT91_EMAC_EMDIO);
}

// Set a specific address match to a given address. Packets received which
// match this address will be passed on.
static void
at91_set_mac(at91_eth_priv_t * priv, cyg_uint8 * enaddr, int sa)
{
   cyg_uint32 hi, lo;

   CYG_ASSERTC(sa > 0 && sa < 5);
   sa--;

   lo = ((enaddr[3] << 24) |
         (enaddr[2] << 16) |
         (enaddr[1] <<  8) |
         (enaddr[0]));

   hi = ((enaddr[5] <<  8) |
         (enaddr[4]));

   HAL_WRITE_UINT32(priv->base + AT91_EMAC_SA1L + (8*sa), lo);
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_SA1H + (8*sa), hi);
}

static void
at91_clear_stats(at91_eth_priv_t *priv)
{
   cyg_uint32 ctl;

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCR, ctl);
   ctl |= AT91_EMAC_NCR_CSR;
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCR, ctl);
}

// Enable and Disable of the receiver and transmitter.
// Initialize the interface. This configures the interface ready for use.
// Interrupts are grabbed etc. This means the start function has
// little to do except enable the receiver
static bool
at91_eth_init(struct cyg_netdevtab_entry *tab)
{
   struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   bool esa_ok = false;
   unsigned char enaddr[6] = { CYGPKG_DEVS_ETH_ARM_AT91_MACADDR};
   unsigned char enzero[6] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0};
   unsigned short phy_state = 0;
   cyg_uint32 ncfg = 0;

   debug1_printf("\nAT91_ETH: Initialising @ %x\n",priv->base);

   priv->tx_busy = false;
   priv->curr_tbd_idx = 0;
   priv->curr_rbd_idx = 0;

   // Enable the clock to the EMAC
   HAL_WRITE_UINT32(AT91_PMC + AT91_PMC_PCER, AT91_PMC_PCER_EMAC);
   HAL_WRITE_UINT32(AT91_PMC + AT91_PMC_PCER, AT91_PMC_PCER_PIOB);
   HAL_WRITE_UINT32(AT91_PMC + AT91_PMC_PCER, AT91_PMC_PCER_PIOA);

   //at91_disable(priv);
   at91_cfg_pins();

   /* Enable  IO Clock */
   HAL_WRITE_UINT32(priv->base+AT91_EMAC_USRIO,AT91_EMAC_USRIO_CLKEN);

   /* Disable all the interrupts for the moment            */
   /* The Start function actually enables all that we need */
   //HAL_WRITE_UINT32(priv->base + AT91_EMAC_IDR, 0x3FFF);

   // If we are building an interrupt enabled version, install the
   // interrupt handler
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
   debug1_printf("AT91_ETH: Installing Interrupts on IRQ %d\n",
		 priv->intr_vector);
   cyg_drv_interrupt_create(priv->intr_vector,
                            4,
                            (cyg_addrword_t)sc,
                            at91_eth_isr,
                            eth_drv_dsr,
                            &priv->intr_handle,
                            &priv->intr);

   cyg_drv_interrupt_attach(priv->intr_handle);
   cyg_drv_interrupt_unmask(priv->intr_vector);
#endif

#ifdef CYGHWR_DEVS_ETH_ARM_AT91_GET_ESA
   // Get MAC address from RedBoot configuration variables
   CYGHWR_DEVS_ETH_ARM_AT91_GET_ESA(&enaddr[0], esa_ok);
   // If this call fails myMacAddr is unchanged and MAC address from
   // CDL is used
#endif

   if (!esa_ok)
   {
      // Can't figure out ESA
      debug1_printf("AT91_ETH - Warning! ESA unknown\n");
   }
   debug1_printf("AT91_ETH: %02x:%02x:%02x:%02x:%02x:%02x\n",
                 enaddr[0],enaddr[1],enaddr[2],
                 enaddr[3],enaddr[4],enaddr[5]);

   // Give the EMAC its address
   at91_set_mac(priv, enaddr, 1);
   at91_set_mac(priv, enzero, 2);
   at91_set_mac(priv, enzero, 3);
   at91_set_mac(priv, enzero, 4);

   // Setup the receiver buffers and descriptors
   at91_rb_init(priv);

   // And tell the EMAC where the first receive buffer descriptor is
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_RBQP, (cyg_uint32)priv->rbd);

   // Setup the transmit descriptors
   at91_tb_init(priv);

   // And tell the EMAC where the first transmit buffer descriptor is
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_TBQP, (cyg_uint32)priv->tbd);

   // Setup the PHY
   CYG_ASSERTC(priv->phy);

   at91_mdio_enable();
   if (!_eth_phy_init(priv->phy))
   {
      at91_mdio_disable();
      return (false);
   }

   // Get the current mode and print it
   phy_state = _eth_phy_state(priv->phy);
   at91_mdio_disable();

   HAL_READ_UINT32(priv->base + AT91_EMAC_NCFG,ncfg);


   if ((phy_state & ETH_PHY_STAT_LINK) != 0)
   {
      if (((phy_state & ETH_PHY_STAT_100MB) != 0))
      {
         debug1_printf("AT91_ETH: 100Mbyte/s");
         ncfg |= AT91_EMAC_NCFG_SPD_100Mbps;
      }
      else
      {
         debug1_printf("AT91_ETH: 10Mbyte/s");
         ncfg &= ~(AT91_EMAC_NCFG_SPD_100Mbps);
      }
      if((phy_state & ETH_PHY_STAT_FDX))
      {
         debug1_printf(" Full Duplex\n");
         ncfg |= AT91_EMAC_NCFG_FD;
      }
      else
      {
         debug1_printf(" Half Duplex\n");
         ncfg &= ~(AT91_EMAC_NCFG_FD);
      }
   }
   else
   {
      debug1_printf("AT91_ETH: No Link\n");
   }


   //Setup the network configuration
   ncfg |= (AT91_EMAC_NCFG_RLCE);

   HAL_WRITE_UINT32(priv->base + AT91_EMAC_NCFG,ncfg);

   // Clear the Statistics counters;
   at91_clear_stats(priv);


   /* Clear the status registers */
   HAL_READ_UINT32(priv->base + AT91_EMAC_ISR,ncfg);
   HAL_READ_UINT32(priv->base + AT91_EMAC_RSR,ncfg);
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_RSR,ncfg);
   HAL_READ_UINT32(priv->base + AT91_EMAC_TSR,ncfg);
   HAL_WRITE_UINT32(priv->base + AT91_EMAC_TSR,ncfg);

   // Initialize the upper layer driver
   _eth_drv_init(sc,enaddr);

   return (true);
}

// This function is called to stop the interface.
static void 
at91_eth_stop(struct eth_drv_sc *sc)
{
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;

   at91_disable(priv);
}

// This function is called to "start up" the interface. It may be called
// multiple times, even when the hardware is already running.
static void
at91_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   cyg_uint32 bits;

   // Enable the interrupts we are interested in
   // TODO: We probably need to add at least the RBNA interrupt here
   //       as well in order to do some error handling
   bits = (AT91_EMAC_ISR_RCOM | AT91_EMAC_ISR_TCOM);

   HAL_WRITE_UINT32(priv->base + AT91_EMAC_IER, bits);

   // Enable the receiver and transmitter
   at91_enable(priv);
}

// This function is called for low level "control" operations
static int
at91_eth_control(struct eth_drv_sc *sc, unsigned long key,
                 void *data, int length)
{

   switch (key)
   {
      case ETH_DRV_SET_MAC_ADDRESS:
         {
            if(length >= ETHER_ADDR_LEN)
            {
               at91_eth_stop(sc);

               cyg_uint8 * enaddr = (cyg_uint8 *)data;
               debug1_printf("AT91_ETH: %02x:%02x:%02x:%02x:%02x:%02x\n",
                             enaddr[0],enaddr[1],enaddr[2],
                             enaddr[3],enaddr[4],enaddr[5]);

               at91_set_mac((at91_eth_priv_t *)sc->driver_private,enaddr,1);
               at91_eth_start(sc,enaddr,0);
               return 0;
            }
            return 1;
         }
      default:
         {
            diag_printf("%s.%d: key %lx\n", __FUNCTION__, __LINE__, key);
            return (1);
         }
   }

}

// This function is called to see if another packet can be sent.
// It should return the number of packets which can be handled.
// Zero should be returned if the interface is busy and can not send
// any more.
//
// We allocate one buffer descriptor per scatter/gather entry. We assume that
// a typical packet will not have more than 3 such entries, and so we say we
// can send a packet when we have 3 or more buffer descriptors free
//
// TODO: Implement what the comment actually says!
static int
at91_eth_can_send(struct eth_drv_sc *sc)
{
   int can_send;
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   if(priv->tx_busy)
   {
      can_send = 0;
   }
   else
   {
      can_send = 1;
   }
   return (can_send);
}

// This routine is called to send data to the hardware
static void
at91_eth_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len, 
              int total_len, unsigned long key)
{
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   int i;
   cyg_uint32 sr;

   priv->tx_busy = true;

   priv->last_tbd_idx = priv->curr_tbd_idx;

   for(i = 0;i<sg_len;i++)
   {
      priv->tbd[priv->curr_tbd_idx].addr = sg_list[i].buf;

      sr = (sg_list[i].len & AT91_EMAC_TBD_SR_LEN_MASK);
      // Set the End Of Frame bit in the last descriptor
      if(i == (sg_len-1))
      {
         sr |= AT91_EMAC_TBD_SR_EOF;
      }
      
      if(priv->curr_tbd_idx < (CYGNUM_DEVS_ETH_ARM_AT91_TX_BUFS-1))
      {
         priv->tbd[priv->curr_tbd_idx].sr = sr;
         priv->curr_tbd_idx++;
      }
      else
      {
         priv->tbd[priv->curr_tbd_idx].sr = (sr | AT91_EMAC_TBD_SR_WRAP);
         priv->curr_tbd_idx = 0;
      }
   }

   // Store away the key for when the transmit has completed
   // and we need to tell the stack which transmit has completed.
   priv->curr_tx_key = key;

   at91_start_transmitter(priv);
}

static void at91_reset_tbd(at91_eth_priv_t *priv)
{
     while(priv->curr_tbd_idx != priv->last_tbd_idx)
     {
        if(priv->last_tbd_idx == (CYGNUM_DEVS_ETH_ARM_AT91_TX_BUFS-1))
        {
           priv->tbd[priv->last_tbd_idx].sr = 
             (AT91_EMAC_TBD_SR_USED|AT91_EMAC_TBD_SR_WRAP);
           priv->last_tbd_idx = 0;
        }
        else
        {
           priv->tbd[priv->last_tbd_idx].sr = AT91_EMAC_TBD_SR_USED;
           priv->last_tbd_idx++;
        }
     }
}


//======================================================================

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
static cyg_uint32
at91_eth_isr (cyg_vector_t vector, cyg_addrword_t data)
{
   struct eth_drv_sc *sc = (struct eth_drv_sc *)data;
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   cyg_uint32 ret;
   cyg_uint32 isr;

   /* Get the interrupt status */
   HAL_READ_UINT32(priv->base+AT91_EMAC_ISR,isr);

   ret = CYG_ISR_HANDLED;

   //TODO: We should probably be handling some of the error interrupts as well
   if(isr & AT91_EMAC_ISR_TCOM)
   {
      ret = CYG_ISR_CALL_DSR;
   }

   if(isr & AT91_EMAC_ISR_RCOM)
   {
      ret = CYG_ISR_CALL_DSR;
   }
   cyg_interrupt_acknowledge(vector);
   return(ret);
}
#endif

static void 
at91_eth_deliver(struct eth_drv_sc *sc)
{
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;

   cyg_uint32 tsr;
   cyg_uint32 rsr;

   cyg_uint32 ctr;
   cyg_uint32 cnt;
   cyg_uint32 idx;

   /* Get the Transmit Status */
   HAL_READ_UINT32(priv->base+AT91_EMAC_TSR,tsr);
   HAL_WRITE_UINT32(priv->base+AT91_EMAC_TSR,tsr);

   /* Get the Receive Status */
   HAL_READ_UINT32(priv->base+AT91_EMAC_RSR,rsr);
   HAL_WRITE_UINT32(priv->base+AT91_EMAC_RSR,rsr);


   //TODO: The interrupts other than RCOMP and TCOMP needs to be
   //      handled properly especially stuff like RBNA which could have
   //      serious effects on driver performance

   /* Service the TX buffers */
   if (tsr&AT91_EMAC_TSR_COL)  //1
   {
      debug1_printf("AT91_ETH: Tx COL\n");
   }

   if (tsr&AT91_EMAC_TSR_RLE)  //2
   {
      debug1_printf("AT91_ETH: Tx RLE\n");
   }

   if (tsr&AT91_EMAC_TSR_BNQ)  //4
   {
      debug1_printf("AT91_ETH: Tx BEX\n");
   }

   if (tsr&AT91_EMAC_TSR_UND)  //6
   {
      debug1_printf("AT91_ETH: Tx UND\n");
   }

   /* Check that the last transmission is completed */
   if (tsr&AT91_EMAC_TSR_COMP) //5
   {
      at91_reset_tbd(priv);
      _eth_drv_tx_done(sc,priv->curr_tx_key,0);
      priv->tx_busy = false;
   }

   /* Service the RX buffers when we get something */
   if (rsr&AT91_EMAC_RSR_REC)
   {
      /* Do this all until we find the first EMAC Buffer */
      while (priv->rbd[priv->curr_rbd_idx].addr & AT91_EMAC_RBD_ADDR_OWNER_SW)
      {

         //Firstly walk through to either the first buffer that belongs 
         // to the controller or the first SOF
         while ((priv->rbd[priv->curr_rbd_idx].addr & 
		 AT91_EMAC_RBD_ADDR_OWNER_SW) && 
                !(priv->rbd[priv->curr_rbd_idx].sr & 
		  AT91_EMAC_RBD_SR_SOF))
         {
            priv->rbd[priv->curr_rbd_idx].addr &= 
	      ~(AT91_EMAC_RBD_ADDR_OWNER_SW);
            priv->curr_rbd_idx++;
            if (priv->curr_rbd_idx >= CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS)
            {
               priv->curr_rbd_idx = 0;
            }
         }

         /* Check that we did find a SOF*/
         if ((priv->rbd[priv->curr_rbd_idx].addr & 
	      AT91_EMAC_RBD_ADDR_OWNER_SW) && 
             (priv->rbd[priv->curr_rbd_idx].sr & AT91_EMAC_RBD_SR_SOF))
         {
            cnt = 0;
            for (ctr=0;ctr<CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS;ctr++)
            {
               idx = (ctr+priv->curr_rbd_idx)%CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS;
               cnt += (priv->rbd[idx].sr & AT91_EMAC_RBD_SR_LEN_MASK);
               if (priv->rbd[idx].sr & AT91_EMAC_RBD_SR_EOF)
               {
                  /* The recv function will adjust the current buffer idx 
                     after the buffer has been cleared
                   */
                  if (cnt)
                     _eth_drv_recv(sc,cnt);
                  break;
               }
            }
         }
      }
   }

   if (rsr&AT91_EMAC_RSR_BNA)
   {
      debug1_printf("AT91_ETH: Rx BNA\n");
   }
   if (rsr&AT91_EMAC_RSR_OVR)
   {
      debug1_printf("AT91_ETH: Rx OVR\n");
   }

}

static void
at91_eth_recv(struct eth_drv_sc *sc,
              struct eth_drv_sg *sg_list,
              int sg_len)
{
   at91_eth_priv_t *priv = (at91_eth_priv_t *)sc->driver_private;
   int i;
   cyg_uint32 bytes_in_buffer;
   cyg_uint32 bytes_in_list = 0;
   cyg_uint32 bytes_needed_list = 0;
   cyg_uint32 buffer_pos = 0;
   cyg_uint8 * sg_buf;
   cyg_uint32 total_bytes = 0;

   for(i = 0;i<sg_len;i++)
   {
      while(bytes_in_list < sg_list[i].len)
      {
         bytes_needed_list = sg_list[i].len - bytes_in_list;

         if(priv->rbd[priv->curr_rbd_idx].sr & AT91_EMAC_RBD_SR_EOF)
         {
	      bytes_in_buffer = 
		((priv->rbd[priv->curr_rbd_idx].sr & AT91_EMAC_RBD_SR_LEN_MASK)
		 - total_bytes) - buffer_pos;
         }
         else
         {
            bytes_in_buffer = AT91_EMAC_RX_BUFF_SIZE - buffer_pos;
         }

         sg_buf = (cyg_uint8 *)(sg_list[i].buf);

         if(bytes_needed_list < bytes_in_buffer)
         {
            if(sg_buf != NULL)
               memcpy(&sg_buf[bytes_in_list],
		      &priv->rb[priv->curr_rbd_idx].rb[buffer_pos],
		      bytes_needed_list);
            bytes_in_list += bytes_needed_list;
            buffer_pos += bytes_needed_list;
            total_bytes += bytes_needed_list;
         }
         else
         {
            if(sg_buf != NULL)
              memcpy(&sg_buf[bytes_in_list],
		     &priv->rb[priv->curr_rbd_idx].rb[buffer_pos],
		     bytes_in_buffer);
            bytes_in_list += bytes_in_buffer;
            total_bytes += bytes_in_buffer;

            /* Step our buffer on one */
            priv->rbd[priv->curr_rbd_idx].addr &= 
	      ~(AT91_EMAC_RBD_ADDR_OWNER_SW);
            priv->curr_rbd_idx++;
            if(priv->curr_rbd_idx >= CYGNUM_DEVS_ETH_ARM_AT91_RX_BUFS)
            {
               priv->curr_rbd_idx = 0;
            }
            buffer_pos = 0;
         }
      }
   }
}

// routine called to handle ethernet controller in polled mode
static void 
at91_eth_poll(struct eth_drv_sc *sc)
{
   /* Service the buffers */
   at91_eth_deliver(sc);
}

static int
at91_eth_int_vector(struct eth_drv_sc *sc)
{
   return(CYGNUM_HAL_INTERRUPT_EMAC);
}

at91_eth_priv_t at91_priv_data =
{
   .intr_vector = CYGNUM_HAL_INTERRUPT_EMAC,
   .base = AT91_EMAC,
   .phy = &at91_phy
};

ETH_DRV_SC(at91_sc,
           &at91_priv_data,       // Driver specific data
           "eth0",                // Name for this interface
           at91_eth_start,
           at91_eth_stop,
           at91_eth_control,
           at91_eth_can_send,
           at91_eth_send,
           at91_eth_recv,
           at91_eth_deliver,
           at91_eth_poll,
           at91_eth_int_vector);

NETDEVTAB_ENTRY(at91_netdev,
                "at91",
                at91_eth_init,
                &at91_sc);

// EOF if_at91.c
