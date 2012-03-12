//==========================================================================
//
//      if_a2fxxx.c
//
//      Ethernet driver for Actel A2Fxxx device family
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
// Date:         2011-05-05
// Purpose:      Ethernet driver for Actel A2Fxxx device family
// Description:
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/system.h>
#include <pkgconf/devs_eth_cortexm_a2fxxx.h>

#ifdef CYGDAT_DEVS_CORTEXM_A2FXXX_ETH_CDL
# include CYGDAT_DEVS_CORTEXM_A2FXXX_ETH_CDL
#endif

#include <pkgconf/io_eth_drivers.h>

#ifdef CYGPKG_NET
# include <pkgconf/net.h>
#endif

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_if.h>
#include <cyg/hal/var_io.h>

#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>

#ifdef CYGPKG_DEVS_ETH_PHY
# include <cyg/io/eth_phy.h>
#endif

#include <errno.h>
#include <string.h>

static unsigned char enaddr[6];

// Buffer descriptor
struct a2fxxx_bd {
    cyg_uint32     des0;
    cyg_uint32     des1;
    cyg_uint32     des2;
    cyg_uint32     des3;
    cyg_uint32     private;
};

// Some statistics
#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
typedef struct {
    cyg_uint32     rx_mcast;
    cyg_uint32     rx_des_err;
    cyg_uint32     rx_collision;
    cyg_uint32     rx_crc_err;
    cyg_uint32     rx_too_long;
    cyg_uint32     tx_fifo_underflow;
    cyg_uint32     tx_frame_deferred;
    cyg_uint32     tx_late_collision;
    cyg_uint32     tx_no_carrier;
    cyg_uint32     tx_loss_carrier;
} a2fxxx_eth_stats;
#endif

// Internal data structure
struct a2fxxx_eth_info {
    volatile struct    a2fxxx_bd *txbd,
                      *rxbd;                   // Next Tx,Rx descriptor to use
    volatile struct    a2fxxx_bd *tbase,
                      *rbase;                  // First Tx,Rx descriptor
    volatile struct    a2fxxx_bd *tnext,
                      *rnext;                  // Next descriptor to check for interrupt
    cyg_uint8          txactive;               // Count of active Tx buffers
    unsigned long      txkey[CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_TxNUM];
    cyg_uint8          mac_address[6];         // mac (hardware) address
    cyg_uint8          *init_rxbufs;           // Initial base pointer of RX buffers
    cyg_uint8          *init_txbufs;           // Initial base pointer of TX buffers
    volatile struct    a2fxxx_bd *init_rxring; // Initial base pointer of RX ring
    volatile struct    a2fxxx_bd *init_txring; // Initial base pointer of TX ring
    cyg_uint32         rx_mac[48];             // Setup frame (192) bytes
    cyg_uint32         valid_mac;
#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
    a2fxxx_eth_stats   stats;                  // MAC statistics
#endif
    cyg_uint32         base;                   // MAC base address
    cyg_uint32         base_bb;                // MAC base address (bit-band)
    cyg_uint8          if_num;
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_uint8          vector;
    cyg_interrupt      interrupt;
    cyg_handle_t       interrupt_handle;
#endif
#ifdef CYGPKG_DEVS_ETH_PHY
    eth_phy_access_t   *phy;
#endif
};

static void     a2fxxx_phy_init(void);
static bool     a2fxxx_mdio_read(cyg_int32, cyg_int32, cyg_uint16 *);
static void     a2fxxx_mdio_write(cyg_int32, cyg_int32, cyg_uint16);

static void     a2fxxx_eth_int(struct eth_drv_sc *data);

#ifdef CYGDAT_DEVS_CORTEXM_A2FXXX_ETH_INL
# include CYGDAT_DEVS_CORTEXM_A2FXXX_ETH_INL
#endif

#include <cyg/infra/diag.h>

#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_CHATTER
# define a2fxxx_eth_printf(args...)   diag_printf(args)
#else
# define a2fxxx_eth_printf(args...)    /* NOOP */
#endif

// For fetching the ESA from RedBoot
#include <cyg/hal/hal_if.h>
#ifndef CONFIG_ESA
# define CONFIG_ESA 6
#endif

// ----------------------------------------------------------------
// Definition
//
//

#define RESET_FULL_DUPLEX                     0x00000001
#define RESET_100MB                           0x00000002

#define MDIO_PREAMBLE_SEQ                     0xffffffff
#define MDIO_START_SEQ                        0x01
#define MDIO_WRITE_SEQ                        0x01
#define MDIO_READ_SEQ                         0x02
#define MDIO_STOP_WRITE_SEQ                   0x02
#define MDIO_STOP_READ_SEQ                    0x00

#define CYGHWR_HAL_A2FXXX_MAC_CSR5_TI_BB(_base_)  \
    ( _base_ + ( CYGHWR_HAL_A2FXXX_MAC_CSR5 << 5 ) + (  0*sizeof(cyg_uint32) ) )
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_RI_BB(_base_)  \
    ( _base_ + ( CYGHWR_HAL_A2FXXX_MAC_CSR5 << 5 ) + (  6*sizeof(cyg_uint32) ) )
#define CYGHWR_HAL_A2FXXX_MAC_CSR5_NIS_BB(_base_) \
    ( _base_ + ( CYGHWR_HAL_A2FXXX_MAC_CSR5 << 5 ) + ( 16*sizeof(cyg_uint32) ) )

#define CYGHWR_HAL_A2FXXX_MAC_PRIVATE_SENT    BIT_(0)

#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
# define A2FXXX_INC_STATS(_x_)                 (qi->stats._x_++)
#endif

// ----------------------------------------------------------------
// MDIO API
//
//

#define MDIO_XFER_BITS( reg, reg_val, sequence, length )      \
{                                                             \
    cyg_uint32 i;                                             \
    for (i = (1 << (length-1)); i != 0; ) {                   \
        reg_val &= ~CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC;           \
        HAL_WRITE_UINT32( reg, reg_val );                     \
        HAL_DELAY_US(1);                                      \
        if( i & sequence )                                    \
            reg_val |= (  CYGHWR_HAL_A2FXXX_MAC_CSR9_MDO );   \
        else                                                  \
            reg_val &= ( ~CYGHWR_HAL_A2FXXX_MAC_CSR9_MDO );   \
        HAL_WRITE_UINT32( reg, reg_val );                     \
        reg_val |= CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC;            \
        HAL_WRITE_UINT32( reg, reg_val );                     \
        HAL_DELAY_US(1);                                      \
        i = i >> 1;                                           \
    }                                                         \
}

//
// Setup IOs
//
static void
a2fxxx_phy_init(void)
{
    a2fxxx_eth_printf("ETH A2Fxxx - PHY Init\n");
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_MDIO);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_MDC);
}

//
// Start MDIO transfer -> 32 bits of preamble ('1'), start bits,
// transfer type bits, phy address and register address bits
//
static void
a2fxxx_mdio_xfer_start(cyg_bool rd, cyg_uint8 phy_addr, cyg_uint8 addr)
{
    cyg_uint32      csr9_reg =
        (CYGHWR_HAL_A2FXXX_MAC + CYGHWR_HAL_A2FXXX_MAC_CSR9);
    cyg_uint32      csr9_data,
                    bit_sequence;

    // Enable MDIO write
    HAL_READ_UINT32(csr9_reg, csr9_data);
    csr9_data |=
        (CYGHWR_HAL_A2FXXX_MAC_CSR9_MDEN | CYGHWR_HAL_A2FXXX_MAC_CSR9_MDO |
         CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC);
    HAL_WRITE_UINT32(csr9_reg, csr9_data);
    HAL_DELAY_US(1);

    // Send preamble bits
    bit_sequence = MDIO_PREAMBLE_SEQ;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 32);

    // Send 2 start bits
    bit_sequence = MDIO_START_SEQ;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 2);

    // Send transfer type bits
    bit_sequence = (rd == true) ? MDIO_READ_SEQ : MDIO_WRITE_SEQ;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 2);

    // Send PHY address
    bit_sequence = (cyg_uint32)phy_addr;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 5);

    // Send register address
    bit_sequence = (cyg_uint32)addr;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 5);

    // Send TA sequence
    if (rd == true) {
        bit_sequence = MDIO_STOP_READ_SEQ;
        csr9_data &= ~(CYGHWR_HAL_A2FXXX_MAC_CSR9_MDEN);
        HAL_WRITE_UINT32(csr9_reg, csr9_data);
    } else {
        bit_sequence = MDIO_STOP_WRITE_SEQ;
    }
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 2);

    // now we can read or write the 16 bits data ...
}

//
// Read 16 bits data from MDIO interface
//
static cyg_bool
a2fxxx_mdio_xfer_read(cyg_uint16 *data)
{
    cyg_uint32      csr9_reg =
        (CYGHWR_HAL_A2FXXX_MAC + CYGHWR_HAL_A2FXXX_MAC_CSR9);
    cyg_uint32      csr9_data;
    *data = 0;
    int             i;

    // Read MDIO register
    HAL_READ_UINT32(csr9_reg, csr9_data);

    // Clock in the RX data
    for (i = (1 << 15); i != 0;) {
        csr9_data &= ~CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC;
        HAL_WRITE_UINT32(csr9_reg, csr9_data);
        HAL_DELAY_US(1);
        HAL_READ_UINT32(csr9_reg, csr9_data);
        if (csr9_data & CYGHWR_HAL_A2FXXX_MAC_CSR9_MDI)
            *data |= i;
        csr9_data |= CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC;
        HAL_WRITE_UINT32(csr9_reg, csr9_data);
        HAL_DELAY_US(1);
        i = i >> 1;
    }

    return true;
}

//
// Write 16 bits data to MDIO interface
//
static bool
a2fxxx_mdio_xfer_write(cyg_uint16 data)
{
    cyg_uint32      csr9_reg =
        (CYGHWR_HAL_A2FXXX_MAC + CYGHWR_HAL_A2FXXX_MAC_CSR9);
    cyg_uint32      csr9_data,
                    bit_sequence;

    // Read MDIO register
    HAL_READ_UINT32(csr9_reg, csr9_data);

    // Write data
    bit_sequence = (cyg_uint32)data;
    MDIO_XFER_BITS(csr9_reg, csr9_data, bit_sequence, 16);

    // Set back clock to low
    csr9_data &= ~CYGHWR_HAL_A2FXXX_MAC_CSR9_MDC;
    HAL_WRITE_UINT32(csr9_reg, csr9_data);

    return true;
}

//
// MDIO read register
//
static bool
a2fxxx_mdio_read(cyg_int32 addr, cyg_int32 phy_addr, cyg_uint16 *data)
{
    bool            res;
    a2fxxx_mdio_xfer_start(true, phy_addr, addr);
    res = a2fxxx_mdio_xfer_read(data);
    a2fxxx_eth_printf("ETH A2Fxxx - PHY %x(%x) - RX data %x\n", phy_addr, addr,
                      *data);
    return res;
}

//
// MDIO write register
//
static void
a2fxxx_mdio_write(cyg_int32 addr, cyg_int32 phy_addr, cyg_uint16 data)
{
    a2fxxx_mdio_xfer_start(false, phy_addr, addr);
    a2fxxx_eth_printf("ETH A2Fxxx - PHY %x(%x) - TX data %x\n", phy_addr, addr,
                      data);
    a2fxxx_mdio_xfer_write(data);
}


// ----------------------------------------------------------------
// Ethernet driver
//
//
#define A2FXXX_RESET_MAC( qi )                                \
{                                                             \
    cyg_uint32      swr_addr =                                \
          qi->base_bb + (CYGHWR_HAL_A2FXXX_MAC_CSR0 << 5);    \
    cyg_uint32      swr_data = 0x1;                           \
    HAL_WRITE_UINT32(swr_addr, swr_data);                     \
    do {                                                      \
        HAL_READ_UINT32(swr_addr, swr_data);                  \
    } while (swr_data);                                       \
}

//
// Deliver function (ex-DSR) handles the Ethernet [logical] processing
//
static void
a2fxxx_eth_deliver(struct eth_drv_sc *sc)
{
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *)sc->driver_private;
#endif
    a2fxxx_eth_int(sc);
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    // Allow interrupts to happen again
    cyg_drv_interrupt_unmask(qi->vector);
#endif
}

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
//
// This ISR is called when the Ethernet interrupt occurs
//
static int
a2fxxx_eth_isr(cyg_vector_t vector, cyg_addrword_t data,
               HAL_SavedRegisters * regs)
{
    a2fxxx_eth_printf("ETH A2Fxxx - ISR\n");
    cyg_drv_interrupt_mask(vector);
    cyg_drv_interrupt_acknowledge(vector);

    return (CYG_ISR_HANDLED | CYG_ISR_CALL_DSR);        // Run the DSR
}

void
a2fxxx_eth_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    a2fxxx_eth_deliver((struct eth_drv_sc *)data);
    // Allow interrupts to happen again
    cyg_drv_interrupt_unmask(vector);
}
#endif

//
// This function add MAC address to the list of allowed MAC address
// that the MAC layer can deliver to the application layer
//
static bool
a2fxxx_eth_add_mac ( struct eth_drv_sc *sc, cyg_uint8 *mac )
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *)sc->driver_private;
    cyg_uint8 idx = 12;
    struct a2fxxx_bd setup_bd;
    cyg_uint32 i, csr4, csr5, csr6, csr7;

    // If no MAC to be added, just re-send the setup frame
    if( mac != NULL )
    {
        // no more room
        if( qi->valid_mac == (cyg_uint32) -1 )
            return false;

        // Search empty spot
        while( qi->valid_mac & i ){
            i = i << 1;
            idx += 12;
        }

        qi->rx_mac[idx+0] = mac[0];
        qi->rx_mac[idx+1] = mac[1];
        qi->rx_mac[idx+4] = mac[2];
        qi->rx_mac[idx+5] = mac[3];
        qi->rx_mac[idx+8] = mac[4];
        qi->rx_mac[idx+9] = mac[6];
    }

    a2fxxx_eth_printf("ETH A2Fxxx - Send setup frame for %s\n", sc->dev_name);

    // Stop TX
    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);
    csr6 &= ~( CYGHWR_HAL_A2FXXX_MAC_CSR6_ST );
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);

    // Wait until controller is halted
    do {
        HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR5), csr5);
    } while( ((csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_MASK)
                                     != CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_STOP) );

    // Backup the current BD / interrupt mask
    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR4), csr4);
    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR7), csr7);

    // Disable interrupt
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR7), 0);

    // Setup descriptor for setup frame
    setup_bd.des0 = CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN;
    setup_bd.des1 = CYGHWR_HAL_A2FXXX_MAC_TDES1_TER |
                         CYGHWR_HAL_A2FXXX_MAC_TDES1_SET |
                         CYGHWR_HAL_A2FXXX_MAC_TDES1_TBS1(192);
    setup_bd.des2 = (cyg_uint32) &qi->rx_mac[0];
    setup_bd.des3 = (cyg_uint32) NULL;
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR4),
                      (cyg_uint32) &setup_bd);

    // Start transmission
    csr6 |= ( CYGHWR_HAL_A2FXXX_MAC_CSR6_ST );
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);

    // Poll for transmission completed
    do {
        HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR5), csr5);
    } while( ((csr5 & A2FXXX_MAC_TX_STATE_MASK) != A2FXXX_MAC_TX_STATE(SUSPEND)) );

    // Stop transmission
    csr6 &= ~( CYGHWR_HAL_A2FXXX_MAC_CSR6_ST );
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);

    // Wait until controller is halted
    do {
        HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR5), csr5);
    } while( ((csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_MASK)
                                    != CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_STOP) );

    // Restore BD
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR4), csr4);

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_TI_BB(qi->base_bb), 1);

    // Restore interrupt mask
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR7), csr7);

    return true;
}

//
// [re]Initialize the Ethernet controller
//   Done separately since shutting down the device requires a
//   full reconfiguration when re-enabling.
//   when
static bool
a2fxxx_eth_setup(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    volatile struct a2fxxx_bd *rxbd, *txbd;
    cyg_uint8 *RxBUF, *TxBUF;
    cyg_uint32 csr0, csr6 = 0, csr11;
    int i = 0;

    qi->valid_mac = 0;
    qi->rx_mac[5] = enaddr[0];
    qi->rx_mac[4] = enaddr[1];
    qi->rx_mac[3] = enaddr[2];
    qi->rx_mac[2] = enaddr[3];
    qi->rx_mac[1] = enaddr[4];
    qi->rx_mac[0] = enaddr[5];

    txbd = qi->init_txring;
    rxbd = qi->init_rxring;

    /* Init Rx / Tx ring base */
    qi->tbase = qi->txbd = qi->tnext = txbd;
    qi->rbase = qi->rxbd = qi->rnext = rxbd;
    qi->txactive = 0;

    RxBUF = qi->init_rxbufs;
    TxBUF = qi->init_txbufs;

    /* Initialize Rx BDs */
    for (i = 0; i < CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_RxNUM; i++)
    {
        rxbd->des0 = CYGHWR_HAL_A2FXXX_MAC_RDES0_OWN;
        rxbd->des1 =
          CYGHWR_HAL_A2FXXX_MAC_RDES1_RBS1(CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_RX);
        rxbd->des2 = (cyg_uint32) RxBUF;
        rxbd->des3 = (cyg_uint32) NULL;
        RxBUF += CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_RX;
        rxbd++;
    }
    rxbd--;
    rxbd->des1 |= CYGHWR_HAL_A2FXXX_MAC_RDES1_RER; // Last buffer

    /* Initialize Tx BDs */
    for (i = 0; i < CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_TxNUM; i++)
    {
        txbd->des1 = 0;
        txbd->des2 = (cyg_uint32) TxBUF;
        rxbd->des3 = (cyg_uint32) NULL;
        txbd->private = 0;
        TxBUF += CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ALIGN_BUFSIZE_TX;
        txbd++;
    }
    txbd--;
    txbd->des1 |= CYGHWR_HAL_A2FXXX_MAC_TDES1_TER; // Last buffer

    // Setup automatic polling
    csr0 = CYGHWR_HAL_A2FXXX_MAC_CSR0_CLEAR | CYGHWR_HAL_A2FXXX_MAC_CSR0_TAP(3) |
           CYGHWR_HAL_A2FXXX_MAC_CSR0_DSL(1);

    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR0),
                      (cyg_uint32) csr0);
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR3),
                      (cyg_uint32) (qi->rbase));
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR4),
                      (cyg_uint32) (qi->tbase));

    csr11 = CYGHWR_HAL_A2FXXX_MAC_CSR11_TT(7) |
            CYGHWR_HAL_A2FXXX_MAC_CSR11_NTP(1) |
            CYGHWR_HAL_A2FXXX_MAC_CSR11_RT(7) |
            CYGHWR_HAL_A2FXXX_MAC_CSR11_NRP(1);
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR11), (cyg_uint32) csr11);

    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);
    csr6 |= CYGHWR_HAL_A2FXXX_MAC_CSR6_CLEAR;

    csr6 |= ((flags & RESET_100MB) ? CYGHWR_HAL_A2FXXX_MAC_CSR6_TTM : 0x0) |
           ((flags & RESET_FULL_DUPLEX) ? CYGHWR_HAL_A2FXXX_MAC_CSR6_FD : 0x0);

#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_PROMISCUOUS
    csr6 |= CYGHWR_HAL_A2FXXX_MAC_CSR6_PR;
#endif

    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);

    return true;
}

//
// This function is called to shut down the interface.
//
static void
a2fxxx_eth_stop(struct eth_drv_sc *sc)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *)sc->driver_private;
    cyg_uint32 csr6, csr5;

    a2fxxx_eth_printf("ETH A2Fxxx - Stop\n");

    // Stop RX / TX
    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);
    csr6 &= ~( CYGHWR_HAL_A2FXXX_MAC_CSR6_SR | CYGHWR_HAL_A2FXXX_MAC_CSR6_ST );
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);

    // Stall until controller is halted
    do {
        HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR5), csr5);
    } while( ((csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_MASK) != CYGHWR_HAL_A2FXXX_MAC_CSR5_TS_STOP) &&
             ((csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_MASK) != CYGHWR_HAL_A2FXXX_MAC_CSR5_RS_STOP));

}

//
// Setup Ethernet IOs.
//
static void
a2fxxx_eth_io(void)
{
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_TXD0);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_TXD1);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_RXD0);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_RXD1);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_TXEN);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_CRSDV);
    CYGHWR_HAL_A2FXXX_GPIO_SET(CYGHWR_HAL_A2FXXX_MAC0_RXER);
}

//
// Initialize the interface - performed at system startup
// This function must set up the interface, including arranging to
// handle interrupts, etc, so that it may be "started" cheaply later.
//
static bool
a2fxxx_eth_init(struct cyg_netdevtab_entry *tab)
{
    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *)sc->driver_private;
    cyg_uint32      speed100 = 0, full_duplex = 0;
    bool            esa_ok;
    cyg_uint16      phy_state = 0;
    cyg_uint32      istate, csr7;
    bool            ret = true;

    a2fxxx_eth_printf("ETH A2Fxxx - Initialization for %s\n", sc->dev_name);

    // Release MAC controller
    CYGHWR_HAL_A2FXXX_PERIPH_RELEASE(CYGHWR_HAL_A2FXXX_PERIPH_SOFTRST(MAC));

    // Setup IOs
    a2fxxx_eth_io();

    // Reset MAC
    A2FXXX_RESET_MAC( qi );

    HAL_DISABLE_INTERRUPTS(istate);

    a2fxxx_eth_stop(sc);

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_drv_interrupt_create(qi->vector,
                             CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_ISR_PRIORITY,
                             (cyg_addrword_t)sc,    //  Data item passed to interrupt handler
                             (cyg_ISR_t *)a2fxxx_eth_isr,
                             (cyg_DSR_t *)eth_drv_dsr,
                             &qi->interrupt_handle, &qi->interrupt);
    cyg_drv_interrupt_attach(qi->interrupt_handle);
    cyg_drv_interrupt_unmask(qi->vector);
#endif

    esa_ok = CYGACC_CALL_IF_FLASH_CFG_OP(CYGNUM_CALL_IF_FLASH_CFG_GET,
                                         "a2fxxx_esa", enaddr, CONFIG_ESA);

    if (!esa_ok) {
        a2fxxx_eth_printf("ETH A2Fxxx - Warning! ESA unknown for %s\n", sc->dev_name);
        memcpy(&enaddr, &qi->mac_address, sizeof(enaddr));
    }

#ifdef CYGPKG_DEVS_ETH_PHY
    if (!_eth_phy_init(qi->phy)) {
        a2fxxx_eth_printf("ETH A2Fxxx - Failed to initialize PHY of %s\n",
                          sc->dev_name);
        ret = false;
        goto exit;
    }
    phy_state = _eth_phy_state(qi->phy);

    if (phy_state & ETH_PHY_STAT_100MB)
        speed100 = 1;
    if (phy_state & ETH_PHY_STAT_FDX)
        full_duplex = 1;
    if (phy_state & ETH_PHY_STAT_LINK) {
        a2fxxx_eth_printf("Ethernet Mode (%s): %sMbps/%s\n",
                          sc->dev_name,
                          (speed100 ? "100" : "10"),
                          (full_duplex ? "Full" : "Half"));
    } else {
        a2fxxx_eth_printf("ETH A2Fxxx - NO LINK on %s\n", sc->dev_name);
    }
#else
    a2fxxx_eth_printf("ETH A2Fxxx - No PHY interface specified for %s\n",
                      sc->dev_name);
    ret = false;
    goto exit;
#endif

    if (!a2fxxx_eth_setup(sc, enaddr, (full_duplex ? RESET_FULL_DUPLEX
                         : 0x00000000) | (speed100 ? RESET_100MB : 0x00000000)))
    {
      ret = false;
      goto exit;
    }

    a2fxxx_eth_add_mac( sc, NULL );

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_RI_BB(qi->base_bb), 1);
    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_TI_BB(qi->base_bb), 1);

    csr7 = ( CYGHWR_HAL_A2FXXX_MAC_CSR7_TIE | CYGHWR_HAL_A2FXXX_MAC_CSR7_RIE |
                    CYGHWR_HAL_A2FXXX_MAC_CSR7_NIE);
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR7), csr7);

    HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_NIS_BB(qi->base_bb),
                                                           (cyg_uint32) 0x1);

    // Call upper layer initialization
    (sc->funs->eth_drv->init)(sc, (unsigned char *) &enaddr);

exit:
    HAL_RESTORE_INTERRUPTS(istate);
    return ret;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
a2fxxx_eth_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *)sc->driver_private;
    cyg_uint32 csr6;

    a2fxxx_eth_printf("ETH A2Fxxx - Start\n");

    // Stop RX / TX
    HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);
    csr6 |= ( CYGHWR_HAL_A2FXXX_MAC_CSR6_SR | CYGHWR_HAL_A2FXXX_MAC_CSR6_ST );
    HAL_WRITE_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR6), csr6);
}

//
// This function is called for low level "control" operations
//
static int
a2fxxx_eth_control(struct eth_drv_sc *sc, unsigned long key,
                   void *data, int length)
{
    switch (key)
    {
    case ETH_DRV_SET_MAC_ADDRESS:
        return 0;
        break;
#ifdef ETH_DRV_SET_MC_ALL
    case ETH_DRV_SET_MC_ALL:
    case ETH_DRV_SET_MC_LIST:
        // TODO
        return 0;
        break;
#endif
    default:
        return 1;
        break;
    }
}

//
// This function is called to see if another packet can be sent.
// It should return the number of packets which can be handled.
// Zero should be returned if the interface is busy and can not send any more.
//
static int
a2fxxx_eth_can_send(struct eth_drv_sc *sc)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;

    return (qi->txactive < CYGNUM_DEVS_ETH_CORTEXM_A2FXXX_TxNUM);
}

//
// This routine is called to send data to the hardware.
static void
a2fxxx_eth_send(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list,
                int sg_len, int total_len, unsigned long key)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    volatile struct a2fxxx_bd *txbd, *txfirst;
    cyg_uint32 des1 = CYGHWR_HAL_A2FXXX_MAC_TDES1_TBS1(total_len);
    volatile cyg_uint8 *bp;
    cyg_int32 i, txindex, istate;

    a2fxxx_eth_printf("ETH A2Fxxx - transmit pkt, length %d\n", total_len);

    // Continue from current descriptor
    txbd = txfirst = qi->txbd;

    if ( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN) == CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN ) {
        a2fxxx_eth_printf("ETH A2Fxxx - Fatal error, no free BD for %s\n",
            sc->dev_name);
        a2fxxx_eth_printf("ETH A2Fxxx - Cannot transmit packet, active pkt in queue:%d\n",
            qi->txactive);
        return;
    }

    // Set up buffer
    bp = (cyg_uint8 *) txbd->des2;
    for (i = 0; i < sg_len; i++) {
        memcpy((void *) bp, (void *) sg_list[i].buf, sg_list[i].len);
        bp += sg_list[i].len;
    }

    if (txbd->des1 & CYGHWR_HAL_A2FXXX_MAC_TDES1_TER) {
        des1 |= CYGHWR_HAL_A2FXXX_MAC_TDES1_TER;
    }
    txbd->des1 =
      ( des1 | CYGHWR_HAL_A2FXXX_MAC_TDES1_LS | CYGHWR_HAL_A2FXXX_MAC_TDES1_FS |
        CYGHWR_HAL_A2FXXX_MAC_TDES1_IC );
    txindex = ((unsigned long) txbd - (unsigned long) qi->tbase)
                / sizeof(*txbd);
    qi->txkey[txindex] = key;

    qi->txactive++;

    HAL_DISABLE_INTERRUPTS(istate);
    // Give ownership to the MAC
    txbd->des0    = CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN;
    txbd->private = CYGHWR_HAL_A2FXXX_MAC_PRIVATE_SENT;

    // Remember the next buffer to try
    if (qi->txbd->des1 & CYGHWR_HAL_A2FXXX_MAC_TDES1_TER){
        qi->txbd = qi->tbase;
    }
    else {
        qi->txbd++;
    }
    HAL_RESTORE_INTERRUPTS(istate);
}

//
// This function is called for low level "control" operations
//
static void
a2fxxx_eth_TxEvent(struct eth_drv_sc *sc)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    volatile struct a2fxxx_bd *txbd;
    int key, txindex;

    txbd = qi->tnext;

    while ( ( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_OWN ) == 0) &&
              (txbd->private & CYGHWR_HAL_A2FXXX_MAC_PRIVATE_SENT) )
    {
        txindex = ((unsigned long) txbd - (unsigned long) qi->tbase)
                / sizeof(*txbd);
        if ((key = qi->txkey[txindex]) != 0)
        {
            qi->txkey[txindex] = 0;
            (sc->funs->eth_drv->tx_done)(sc, key, 0);
        }
        qi->txactive--;
#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
        if( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_LC) ) {
            A2FXXX_INC_STATS(tx_late_collision);
        }
        if( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_NC) ) {
            A2FXXX_INC_STATS(tx_no_carrier);
        }
        if( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_LO) ) {
            A2FXXX_INC_STATS(tx_loss_carrier);
        }
        if( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_DE) ) {
            A2FXXX_INC_STATS(tx_frame_deferred);
        }
        if( (txbd->des0 & CYGHWR_HAL_A2FXXX_MAC_TDES0_UF) ) {
            A2FXXX_INC_STATS(tx_fifo_underflow);
        }
#endif
        txbd->private &= ~CYGHWR_HAL_A2FXXX_MAC_PRIVATE_SENT;
        if (txbd->des1 & CYGHWR_HAL_A2FXXX_MAC_TDES1_TER){
            txbd = qi->tbase;
        } else {
            txbd++;
        }
    }
    // Remember where we left off
    qi->tnext = (struct a2fxxx_bd *) txbd;
}

//
// Interrupt vector
//
static int
a2fxxx_eth_int_vector(struct eth_drv_sc *sc)
{
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    return (qi->vector);
#else
    return 0;
#endif
}

//
// This function is called when a packet has been received.  It's job is
// to prepare to unload the packet from the hardware.  Once the length of
// the packet is known, the upper layer of the driver can be told.  When
// the upper layer is ready to unload the packet, the internal function
// 'fec_eth_recv' will be called to actually fetch it from the hardware.
//
static void
a2fxxx_eth_RxEvent(struct eth_drv_sc *sc)
{
  struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
  volatile struct a2fxxx_bd *rxbd, *rxfirst;
  int cnt_bd = 0;
  cyg_int32 len;

  rxbd = rxfirst = qi->rnext;
  while ((rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_OWN) == 0)
  {
    cnt_bd++;
    qi->rxbd = rxbd; // Save for callback

#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
    if( (rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_CE) ) {
        A2FXXX_INC_STATS(rx_crc_err);
    }
    if( (rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_CS) ) {
        A2FXXX_INC_STATS(rx_collision);
    }
    if( (rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_TL) ) {
        A2FXXX_INC_STATS(rx_too_long);
    }
    if( (rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_MF) ) {
        A2FXXX_INC_STATS(rx_mcast);
    }
    if( (rxbd->des0 & CYGHWR_HAL_A2FXXX_MAC_RDES0_DE) ) {
        A2FXXX_INC_STATS(rx_des_err);
    }
#endif

    len = (rxbd->des0 >> 16) & 0x3fff;
    a2fxxx_eth_printf("ETH A2Fxxx - Receive pkt, length %d\n", len);

    (sc->funs->eth_drv->recv)(sc, len - 4);
    if (rxbd->des1 & CYGHWR_HAL_A2FXXX_MAC_RDES1_RER){
        rxbd = qi->rbase;
    } else {
        rxbd++;
    }
  }
  // Remember where we left off
  qi->rnext = (struct a2fxxx_bd *) rxbd;
}

//
// Interrupt processing
//
static void
a2fxxx_eth_int(struct eth_drv_sc *sc)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    cyg_uint32 csr5;

     // Check interrupt status
     HAL_READ_UINT32( (qi->base + CYGHWR_HAL_A2FXXX_MAC_CSR5), csr5);

     if (( csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_TI ) != 0)
     {
         a2fxxx_eth_printf("%s - Transmit packet irq\n", sc->dev_name);
         a2fxxx_eth_TxEvent(sc);
         HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_TI_BB(qi->base_bb), 1);
     }

     if (( csr5 & CYGHWR_HAL_A2FXXX_MAC_CSR5_RI ) != 0)
     {
         a2fxxx_eth_printf("%s - Receive packet irq\n", sc->dev_name);
         a2fxxx_eth_RxEvent(sc);
         HAL_WRITE_UINT32( CYGHWR_HAL_A2FXXX_MAC_CSR5_RI_BB(qi->base_bb), 1);
     }
}

//
// Polling function
//
static void
a2fxxx_eth_poll(struct eth_drv_sc *sc)
{
     a2fxxx_eth_int(sc);
     CYGACC_CALL_IF_DELAY_US(500);
}

static void
a2fxxx_eth_recv(struct eth_drv_sc *sc, struct eth_drv_sg *sg_list, int sg_len)
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;
    cyg_uint8 *bp;
    int i;

    bp = (cyg_uint8 *) qi->rxbd->des2;
    for (i = 0; i < sg_len; i++)
    {
        if (sg_list[i].buf != 0)
        {
            memcpy((void *) sg_list[i].buf, bp, sg_list[i].len);
            bp += sg_list[i].len;
        }
    }
    qi->rxbd->des0 |= CYGHWR_HAL_A2FXXX_MAC_RDES0_OWN;
}


//
// Display ETH statistics
//
#ifdef CYGSEM_DEVS_ETH_CORTEXM_A2FXXX_STATS
externC void
a2fxxx_disp_stats(struct eth_drv_sc *sc , char flag )
{
    struct a2fxxx_eth_info *qi = (struct a2fxxx_eth_info *) sc->driver_private;

    diag_printf("Ethernet Statistics\n");

    diag_printf("%17s %10d\n",
        "RX MCAST",
        qi->stats.rx_mcast);
    diag_printf("%17s %10d\n",
        "RX DESC ERR",
        qi->stats.rx_des_err);
    diag_printf("%17s %10d\n",
        "RX COLLISION",
        qi->stats.rx_collision);
    diag_printf("%17s %10d\n",
        "RX CRC ERR",
        qi->stats.rx_crc_err);
    diag_printf("%17s %10d\n",
        "RX TOO LONG",
        qi->stats.rx_too_long);
    diag_printf("%17s %10d\n",
        "TX FIFO UNDERFLOW",
        qi->stats.tx_fifo_underflow);
    diag_printf("%17s %10d\n",
        "TX FRAME DEFERRED",
        qi->stats.tx_frame_deferred);
    diag_printf("%17s %10d\n",
        "TX LATE COLLISION",
        qi->stats.tx_late_collision);
    diag_printf("%17s %10d\n",
        "TX NO CARRIER",
        qi->stats.tx_no_carrier);
    diag_printf("%17s %10d\n",
        "TX LOSS CARRIER",
        qi->stats.tx_loss_carrier);
}
#endif

// EOF if_a2fxxx.c
