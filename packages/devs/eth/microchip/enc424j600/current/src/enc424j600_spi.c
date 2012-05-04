//==========================================================================
//
//      enc424j600_spi.c
//
//      Microchip enc424j600 Ethernet chip
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2010, 2012 Free Software Foundation, Inc.
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
// Author(s):    Ilija Stanislevik
// Contributors:
// Date:         2010-11-23
// Purpose:
// Description:
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <cyg/hal/hal_io.h>
#include <cyg/hal/hal_if.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/cyg_trac.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/drv_api.h>
#include <cyg/io/eth/netdev.h>
#include <cyg/io/eth/eth_drv.h>

#include <cyg/io/spi.h>                 // Common SPI API

#include "enc424j600_spi.h"
#include <cyg/io/eth/enc424j600_eth.h>

// Set ENC424J600_DEBUG to:
// 0 to suppress all printout
// 1 to print error reports
// 2 to print interrupt tracing
// 4 printout _init() progress
// 8 to print progress tracing data
// 0x10  for test with dummy stack
// 0x20  to printout status registers
#define ENC424J600_DEBUG (0)

#define ENC424J600_DONT_DROP_CS    (0)
#define ENC424J600_DO_DROP_CS      (1)
#define ENC424J600_TXBUF_START     (0)

#define ENC424J600_RXBUF_START (ENC424J600_TXBUF_START + CYGNUM_DEVS_ETH_ENC424J600_TXBUF_SIZE)

#ifndef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
#   define SPI_POLLING true
#else
#   define SPI_POLLING false
#endif

static enc424j600_priv_data_t eth0_enc424j600_priv_data;

ETH_DRV_SC(eth0_spi_sc,
           &eth0_enc424j600_priv_data,  // Driver specific data.
           CYGDAT_IO_ETH_ENC424J600_NAME,
           enc424j600_spi_start,
           enc424j600_spi_stop,
           enc424j600_spi_control,
           enc424j600_spi_can_send,
           enc424j600_spi_send,
           enc424j600_spi_recv,
           enc424j600_spi_deliver,
           enc424j600_spi_poll,
           enc424j600_spi_int_vector
);

NETDEVTAB_ENTRY(eth0_spi_netdev,
                "eth_spi",
                enc424j600_spi_init,
                &eth0_spi_sc);

// Local service functions

// Interface for enc424j600 "single byte instruction".
static void
simple_operation(enc424j600_priv_data_t *dpd, enum enc424j600_spi_opcode_1_e opcode)
{
    cyg_spi_transaction_begin(dpd->spi_service_device);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
            SPI_POLLING,
            1,
            & opcode,
            NULL,           // not interested in answer
            ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(dpd->spi_service_device);

    return;
}

// Interface for enc424j600 "banked N-byte instructions".
// Banked read, write, bit set or bit clear operation with control register
static int
banked_register_operation(enc424j600_priv_data_t *cpd,
    enum enc424j600_spi_opcode_e opcode,
    cyg_uint8 address,
    cyg_uint16 length,
    cyg_uint8 *buffer)
{
    if ((NULL == buffer) || (0 == length))
    {
#if ENC424J600_DEBUG & 1
        diag_printf("Wrong parm buffer=0x%x or length=%d.\n", (int)buffer, length);
#endif
        return -1;
    }

    if ((ENC424J600_ECON1H < address) || (opcode & ENC424J600_ECON1H))
    {
#if ENC424J600_DEBUG & 1
        diag_printf("Wrong address=0x%02x or opcode=0x%02x for banked operation.\n",
            address, opcode);
#endif
        return -1;
    }

    address |= opcode;  // combine them in single byte
    cyg_spi_transaction_begin(cpd->spi_service_device);
    // Send the opcode and address,
    cyg_spi_transaction_transfer(cpd->spi_service_device,
        SPI_POLLING,
        1,
        & address,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // then the data.
    cyg_spi_transaction_transfer(cpd->spi_service_device,
        SPI_POLLING,
        length,
        (ENC424J600_READCR == opcode) ? NULL : buffer,
        (ENC424J600_READCR == opcode) ? buffer : NULL,
        ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(cpd->spi_service_device);

    return 0;
}

// Interface for enc424j600 "unbanked N-byte instructions".
// Unbanked read, write, bit set or bit clear operation with control register
static int
unbanked_register_operation(enc424j600_priv_data_t *cpd,
    enum enc424j600_spi_opcode_u_e opcode,
    cyg_uint8 address,
    cyg_uint16 length,
    cyg_uint8 *buffer)
{
    if ((NULL == buffer) || (0 == length))
    {
#if ENC424J600_DEBUG & 1
        diag_printf("Wrong parm buffer=0x%x or length=%d.\n", (int)buffer, length);
#endif
        return -1;
    }

    cyg_spi_transaction_begin(cpd->spi_service_device);
    // Send the opcode,
    cyg_spi_transaction_transfer(cpd->spi_service_device,
        SPI_POLLING,
        1,
        & opcode,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // then the address
    cyg_spi_transaction_transfer(cpd->spi_service_device,
        SPI_POLLING,
        1,
        & address,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    // and finally the data.
    cyg_spi_transaction_transfer(cpd->spi_service_device,
        SPI_POLLING,
        length,
        (ENC424J600_READCRU == opcode) ? NULL : buffer,
        (ENC424J600_READCRU == opcode) ? buffer : NULL,
        ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(cpd->spi_service_device);

    return 0;
}

// Get ESA from the Ethernet chip, unbanked
static int
read_esa_u(enc424j600_priv_data_t *cpd, cyg_uint8 *esa_buf)
{
    int retval = -1;
    cyg_uint8 response_msg[ETHER_ADDR_LEN];

#if ENC424J600_DEBUG & 8
    diag_printf("read_esa_u()...\n");
#endif
    if (NULL == esa_buf)
    {
        return -1;
    }

    retval = unbanked_register_operation(cpd, ENC424J600_READCRU, ENC424J600_MAADR3_U, ETHER_ADDR_LEN,
                                         &response_msg[0]);
    if (0 != retval)
    {
        return retval;
    }
    *esa_buf++ = response_msg[4];
    *esa_buf++ = response_msg[5];
    *esa_buf++ = response_msg[2];
    *esa_buf++ = response_msg[3];
    *esa_buf++ = response_msg[0];
    *esa_buf   = response_msg[1];

    return 0;
}

// Set ESA into Ethernet chip, unbanked
static int
write_esa_u(enc424j600_priv_data_t *dpd, cyg_uint8 *esa_buf)
{
    int retval = -1;
    cyg_uint8 msg[8];

#if ENC424J600_DEBUG & 8
    diag_printf("write_esa_u()...1...\n");
#endif
    if (NULL == esa_buf)
    {
        return -1;
    }

    msg[4] = *esa_buf++;
    msg[5] = *esa_buf++;
    msg[2] = *esa_buf++;
    msg[3] = *esa_buf++;
    msg[0] = *esa_buf++;
    msg[1] = *esa_buf;

    retval = unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MAADR3_U, 6, &msg[0]);
    if (0 != retval)
    {
#if ENC424J600_DEBUG & 1
    diag_printf("Writing ESA into chip failed.\n");
#endif
        return retval;
    }

    return 0;
}

#if ENC424J600_DEBUG & 0x20
// Read data from PHY register
static void
read_phy(enc424j600_priv_data_t *dpd,
    const enum enc424j600_pr_address_e pr_address,
    cyg_uint16 *retval_p)
{
    cyg_uint8 aux[2] = {pr_address, 0x01};
    cyg_uint8 status;

    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIREGADR_U, 2, &aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MICMD_U, 2, &aux[0]);
    aux[0] |= ENC424J600_MIIRD;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MICMD_U, 2, &aux[0]);
    CYGACC_CALL_IF_DELAY_US(26);
    do
    {   // Check and busy wait for read operation to complete
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MISTAT_U, 1, &status);
    } while(0 != (status & ENC424J600_MISTAT_BUSY));

    aux[0] &= !ENC424J600_MIIRD; // Clear ENC424J600_MIIRD bit
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MICMD_U, 2, &aux[0]);

    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MIRD_U, 2, &aux[0]);
    *retval_p = aux[0] + 256 * aux[1];

    return;
}
#endif

// Write data to PHY register
static void
write_phy(enc424j600_priv_data_t *dpd,
    const enum enc424j600_pr_address_e pr_address,
    const cyg_uint16 value)
{
    cyg_uint8 aux[2] = {pr_address, 0x01};

    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIREGADR_U, 2, &aux[0]);
    aux[0] = value & 0xff;
    aux[1] = (value >> 8) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MIWR_U, 2, &aux[0]);

    return;
    // There must be no other MIIM operation in the following 25.6 us!
}

// Write value for on-chip pointer.
static void
set_pointer(enc424j600_priv_data_t *dpd,
    enum enc424j600_spi_bufferp_e oc_pointer,
    cyg_uint16 value
    )
{
    cyg_spi_transaction_begin(dpd->spi_service_device);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        2,
        (cyg_uint8 *) & value,
        NULL,           // not interested in answer
        ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(dpd->spi_service_device);
    return;
}

static inline void
write_to_tx_buffer(enc424j600_priv_data_t *dpd,
    cyg_uint16 length,
    cyg_uint8 *buffer)
{
    cyg_uint8 oc_pointer = ENC424J600_WGPDATA; // We use General Purpose Buffer

    cyg_spi_transaction_begin(dpd->spi_service_device);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        length,
        buffer,
        NULL,
        ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(dpd->spi_service_device);

    return;
}

static int
read_from_rx_buffer(enc424j600_priv_data_t *dpd,
    cyg_uint16 length,
    cyg_uint8 *buffer)
{
    cyg_uint8 oc_pointer = ENC424J600_RRXDATA;

//    if ((0 == length) || (NULL == buffer))
    if (0 == length)
    {
        return -1;
    }
    cyg_spi_transaction_begin(dpd->spi_service_device);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        1,
        & oc_pointer,
        NULL,           // not interested in answer
        ENC424J600_DONT_DROP_CS);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        length,
        NULL,
        buffer,
        ENC424J600_DO_DROP_CS);
    cyg_spi_transaction_end(dpd->spi_service_device);

    return 0;
}

static void
set_MAC(enc424j600_priv_data_t *dpd,
    cyg_uint8 ethernet_status   // As read from upper byte of ESTAT register
    )
{
    cyg_uint8 aux[2];

    if (0 != (ethernet_status & ENC424J600_PHYDPX))
    {   // Full duplex
#if ENC424J600_DEBUG & 0x20
        diag_printf("Full duplex\n");
#endif
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
        aux[0] |= ENC424J600_FULDPX;    // Set MAC to full duplex
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux[0]);

        aux[1] = 0;
        aux[0] = 0x15;  // Inter-packet gap
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MABBIPG_U, 2, &aux[0]);
    }
    else
    {   // Half duplex
#if ENC424J600_DEBUG & 0x20
        diag_printf("Half duplex\n");
#endif
        unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
        aux[0] &= !ENC424J600_FULDPX; // Set MAC to half duplex
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux[0]);

        aux[1] = 0;
        aux[0] = 0x12;  // Inter-packet gap
        unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MABBIPG_U, 2, &aux[0]);
    }

    return;
}

static void
enc424j600_spi_LINKevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 iflag = ENC424J600_LINKIF;
    cyg_uint8 aux[2];

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_LINKevent()....\n");
#endif

    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux[0]);
#if ENC424J600_DEBUG & 0x20
    diag_printf("ESTAT %02x%02x Link ", aux[1], aux[0]);
#endif
    if (0 != (aux[1] & ENC424J600_PHYLNK))
    {
        dpd->link_status = ENC424J600_LINK_ON;

#if ENC424J600_DEBUG & 0x20
        diag_printf("ON\n");
#endif
        // Link is just established. Set up the MAC
        set_MAC(dpd, aux[1]);
    }
    else
    {
        dpd->link_status = ENC424J600_LINK_OFF;

#if ENC424J600_DEBUG & 0x20
        diag_printf("OFF\n");
#endif
    }
#if ENC424J600_DEBUG & 0x20
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON1_U, 2, &aux[0]);
    diag_printf("MACON1 %02x%02x\n", aux[1], aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux[0]);
    diag_printf("MACON2 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHSTAT1, (cyg_uint16 *) &aux[0]);
    diag_printf("PHSTAT1 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHSTAT3, (cyg_uint16 *) &aux[0]);
    diag_printf("PHSTAT3 %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHANLPA, (cyg_uint16 *) &aux[0]);
    diag_printf("PHANLPA %02x%02x\n", aux[1], aux[0]);
    read_phy(dpd, ENC424J600_PHANA, (cyg_uint16 *) &aux[0]);
    diag_printf("PHANA %02x%02x\n", aux[1], aux[0]);
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ECON1L, 2, &aux[0]);
    diag_printf("ECON1 %02x%02x\n", aux[1], aux[0]);
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_ECON2_U, 2, &aux[0]);
    diag_printf("ECON2 %02x%02x\n", aux[1], aux[0]);

#endif

    // Clear Link interrupt flag
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRH, 1, &iflag);
}

static void
enc424j600_spi_PCFULevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 flags = ENC424J600_PCFULIF;

#if ENC424J600_DEBUG & 8
    diag_printf("\nenc424j600_spi_PCFULevent()....\n\n");
#endif
    simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement received packet counter
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &flags);
}

static void
enc424j600_spi_RXABTevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 flags = ENC424J600_RXABTIF;

#if ENC424J600_DEBUG & 8
    diag_printf("\nenc424j600_spi_RXABTevent()....\n\n");
#endif
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &flags);
}

static void
enc424j600_spi_TXevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 iflag = ENC424J600_TXIF;

#if ENC424J600_DEBUG & 8
    diag_printf("TX event.\n");
#endif

    dpd->txbusy = false;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &iflag);
}

static void
enc424j600_spi_TXABTevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 iflag = ENC424J600_TXABTIF;

#if ENC424J600_DEBUG & 8
    diag_printf("TXABT event.\n");
#endif

    dpd->txbusy = false;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 1, &iflag);
}

static void
enc424j600_spi_RXevent(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 pkcnt;    // Local copy of the packet counter
    cyg_uint8 rsv[2];
    cyg_uint16 packet_length;

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_RXevent()....\n");
#endif

    // Take info on single received packet
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 1, &pkcnt);
    if (0 != pkcnt)
    {
#if ENC424J600_DEBUG & 8
        diag_printf("There are %u packets pending in receive buffer.\n", pkcnt);
#endif
        // Prepare on-chip read pointer
        set_pointer(dpd, ENC424J600_RXRDPT, dpd->NextPacketPointer);
        // The two bytes to read next point to the next packet
        read_from_rx_buffer(dpd, 2, (cyg_uint8 *) & dpd->NextPacketPointer);

        // Next 6 bytes to read are the Read Status Vector. We need only the first two.
        read_from_rx_buffer(dpd, 2, & rsv[0]);
        read_from_rx_buffer(dpd, 4, NULL);
        packet_length = rsv[0] + rsv[1] * 256;
        sc->funs->eth_drv->recv(sc, packet_length);
    }

    return;
}

// - End of local service functions -------------------------

// - Higher level interface functions -----------------------

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
// This ISR is called when the Ethernet interrupt occurs
static cyg_uint32
enc424j600_spi_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_drv_interrupt_mask(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR);
    cyg_drv_interrupt_acknowledge(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR);

    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

static void
enc424j600_spi_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)data;
    struct cyg_netdevtab_entry *ndp = (struct cyg_netdevtab_entry *)(dpd->tab);
    struct eth_drv_sc *sc = (struct eth_drv_sc *)(ndp->device_instance);
#if (ENC424J600_DEBUG & 2)
    diag_printf("DSR: Interrupt! count %u\n", count);
#endif

#if (ENC424J600_DEBUG & 0x10)
    extern void fake_eth_drv_dsr(cyg_vector_t vector,
                            cyg_ucount32 count,
                            cyg_addrword_t data);

    fake_eth_drv_dsr( vector, count, (cyg_addrword_t)sc );
#else
    eth_drv_dsr( vector, count, (cyg_addrword_t)sc );
#endif

    return;
}
#endif // #ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

// Initialization of driver and chip

#define ENC424J600_INIT_SPI_READY_RETRY 100
#define ENC424J600_INIT_CLK_READY_RETRY 100
#define ENC424J600_INIT_RETRY_PERIOD_US 100

static bool
enc424j600_spi_init(struct cyg_netdevtab_entry *tab)
{
    int retval;
    cyg_uint8 aux_8[2];
    cyg_bool esa_configured = false;
#ifdef CYGSEM_DEVS_ETH_ENC424J600_SET_ESA
    cyg_uint8 esa_from_cdl[ETHER_ADDR_LEN] = CYGDAT_DEVS_ETH_ENC424J600_ESA;
#endif
    unsigned int i;  // loop counter

    struct eth_drv_sc *sc = (struct eth_drv_sc *)tab->device_instance;
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;

    dpd->tab = tab;

    CYG_DEVS_ETH_ENC424J600_PLF_INIT(tab);  // Platform HAL should define this macro

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    // Install interrupt handler.
    cyg_drv_interrupt_create(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR,
         CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_PRIORITY,
         (cyg_addrword_t)dpd, //  Data item passed to interrupt handler
         (cyg_ISR_t *)enc424j600_spi_isr,
         (cyg_DSR_t *)enc424j600_spi_dsr,
         &dpd->interrupt_handle,
         &dpd->interrupt_object);
    cyg_drv_interrupt_attach(dpd->interrupt_handle);
    cyg_drv_interrupt_configure(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR,
            ENC424J600_ETH_INTERRUPT_LEVEL_LOW, ENC424J600_ETH_INTERRUPT_EDGE_FALLING);
    cyg_drv_interrupt_acknowledge(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR);
    cyg_drv_interrupt_unmask(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR);
#if ENC424J600_DEBUG & 4
    diag_printf("%s(): Interrupt handler is installed.\n", __FUNCTION__);
#endif
#endif  //  #ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED

    // Check if the Ethernet chip is connected and listening
    for (i=0; i <= ENC424J600_INIT_SPI_READY_RETRY; i++)
    {
        aux_8[1] = (cyg_uint8)0x12;   // Test pattern
        aux_8[0] = (cyg_uint8)0x34;
        banked_register_operation(dpd, ENC424J600_WRITECR, ENC424J600_EUDASTL, 2, &aux_8[0]);
        banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_EUDASTL, 2, &aux_8[0]);
        // Verify the test pattern
        if ((0x12 == aux_8[1]) && (0x34 == aux_8[0]))
        {   // verified
            break;
        }
        CYGACC_CALL_IF_DELAY_US(ENC424J600_INIT_RETRY_PERIOD_US);
    }

    if (i > ENC424J600_INIT_SPI_READY_RETRY)
    {
        CYG_FAIL("enc424j600 SPI is not ready!\n");
        return false;
    }

#if ENC424J600_DEBUG & 4
        diag_printf("enc424j600 SPI is there.\n");
#endif

    // Check if ENC424J600_CLKRDY flag is set
    for (i=0; i <= ENC424J600_INIT_CLK_READY_RETRY; i++)
    {
        banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux_8[0]);
        if (0 == (aux_8[1] & ENC424J600_CLKRDY))
        {
            i++;
        }
        else
        {   // clock is ready
            break;
        }
        CYGACC_CALL_IF_DELAY_US(ENC424J600_INIT_RETRY_PERIOD_US);
    }
    if (i > ENC424J600_INIT_CLK_READY_RETRY)
    {
        CYG_FAIL("enc424j600 clock is not ready!\n");
        return false;
    }

#if ENC424J600_DEBUG & 4
        diag_printf("enc424j600 clock is ready.\n");
#endif

    // Issue a System Reset for Ethernet chip
    aux_8[0] = ENC424J600_ETHRST;
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_ECON2_U, 1, &aux_8[0]);
    CYGACC_CALL_IF_DELAY_US(25);
    // Check if the reset really happened
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_EUDASTL, 2, &aux_8[0]);
    if ((0 != aux_8[0]) || (0 != aux_8[1]))
    {
        CYG_FAIL("enc424j600 is not reset!\n");
        return false;
    }
#if ENC424J600_DEBUG & 4
    diag_printf("Successful reset of the Ethernet chip.\n");
#endif

    CYGACC_CALL_IF_DELAY_US(256);

#define CLOCKOUT_CODE(_freq_) CLOCKOUT_LABEL(_freq_)
#define CLOCKOUT_LABEL(_freq_) (ENC424J600_CLKO_##_freq_)

    // Set clock output frequency. External hardware may need it.
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_ECON2_U + 1, 1, &aux_8[1]);
    aux_8[1] &= 0xf0;
    aux_8[1] |= (cyg_uint8)CLOCKOUT_CODE(CYGNUM_DEVS_ETH_ENC424J600_CLOCKOUT_FREQUENCY);
#if ENC424J600_DEBUG & 4
    diag_printf("Setting clock out, ECON2H 0x%02x\n", aux_8[1]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ECON2_U + 1, 1, &aux_8[1]);


    // Find ESA - check possible sources in sequence and stop when
    // one provides the ESA:
    //   RedBoot option (via provide_esa)
    //   Set by application
    //   Set by configuration option
    //   Ethernet chip (manufacture-time set)

    if (NULL != dpd->provide_esa)
    {
        esa_configured = dpd->provide_esa(dpd);
# if ENC424J600_DEBUG & 4
        if (esa_configured)
            diag_printf("Got ESA from RedBoot option\n");
# endif
    }
    if (!esa_configured && dpd->hardwired_esa)
    {
        // ESA is already set in dpd->esa[]
#if ENC424J600_DEBUG & 4
        diag_printf("Got hardcoded ESA\n");
#endif
        esa_configured = true;
    }
#ifdef CYGSEM_DEVS_ETH_ENC424J600_SET_ESA
    else
    {
        memcpy(dpd->esa, esa_from_cdl, ETHER_ADDR_LEN);
        esa_configured = true;
# if ENC424J600_DEBUG & 4
        diag_printf("Got ESA from CDL.\n");
# endif
    }
#endif  //  #ifdef CYGSEM_DEVS_ETH_ENC424J600_SET_ESA

    if (!esa_configured)
    {
# if ENC424J600_DEBUG & 4
        diag_printf("ESA is not provided. Getting it from the chip.\n");
# endif
        retval = read_esa_u(dpd, &(dpd->esa[0]));
        if (0 != retval)
        {
            CYG_FAIL("Reading ESA from chip failed.\n");
            return false;
        }
    }
    else
    {
# if ENC424J600_DEBUG & 4
        diag_printf("Setting ESA into chip. Will last till next reset.\n");
# endif
        retval = write_esa_u(dpd, &(dpd->esa[0]));
        if (0 != retval)
        {
            CYG_FAIL("Writing ESA into chip failed.\n");
            return false;
        }
    }

# if ENC424J600_DEBUG & 4
    cyg_uint8 cesa[ETHER_ADDR_LEN] = {0,0,0,0,0,0};
    diag_printf("ESA %02x:%02x:%02x:%02x:%02x:%02x\n",
                dpd->esa[0], dpd->esa[1], dpd->esa[2],
                dpd->esa[3], dpd->esa[4], dpd->esa[5]);
    retval = read_esa_u(dpd, &cesa[0]);
    if (0 != retval)
    {
        diag_printf("Reading ESA from chip failed.\n");
    }
    diag_printf("Control reading from chip: ");
    diag_printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
        cesa[0], cesa[1], cesa[2],
        cesa[3], cesa[4], cesa[5]);
# endif

    // Set receive filters
    aux_8[0] = ENC424J600_BCEN | ENC424J600_UCEN | ENC424J600_RUNTEN | ENC424J600_CRCEN;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXFCON_U, 1, &aux_8[0]);

#ifdef CYGSEM_DEVS_ETH_ENC424J600_FLOWC_OnChip
#if ENC424J600_DEBUG & 4
    diag_printf("eth_spi_enc424j600_init() Setting auto flow control\n");
#endif
    // Duration of pause
    aux_8[0] = CYGNUM_DEVS_ETH_ENC424J600_FLOWC_PAUSE & 0xff;
    aux_8[1] = (CYGNUM_DEVS_ETH_ENC424J600_FLOWC_PAUSE >> 8 ) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_EPAUS_U, 2, &aux_8[0]);

    // Thresholds for flow control
    aux_8[0] = CYGNUM_DEVS_ETH_ENC424J600_FLOWC_LOWER_WATERMARK;
    aux_8[1] = CYGNUM_DEVS_ETH_ENC424J600_FLOWC_UPPER_WATERMARK;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXWM_U, 2, &aux_8[0]);

    aux_8[0] = ENC424J600_AUTOFC;  // Enable automatic flow control
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_ECON2_U, 1, &aux_8[0]);

#else
    // No flow control
    simple_operation(dpd, ENC424J600_FCDISABLE);
#endif

    // Set PHY speed and duplex
    aux_8[0] = aux_8[1] = 0;
#ifndef CYGNUM_DEVS_ETH_ENC424J600_NO_AUTO_NEGOTIATION
    aux_8[1] = ENC424J600_ANEN;  // Enable auto negotiation
#else
#   if(CYGNUM_DEVS_ETH_ENC424J600_SPEED == 100)
        aux_8[1] |= ENC424J600_SPD100;
#   endif
#   ifdef CYGSEM_DEVS_ETH_ENC424J600_FULL_DUPLEX
        aux_8[1] |= ENC424J600_PFULDPX;
#   endif
#endif
#if ENC424J600_DEBUG & 4
    diag_printf("Writing PHCON1 0x%02x%02x\n", aux_8[1], aux_8[0]);
#endif
    write_phy(dpd, ENC424J600_PHCON1, (aux_8[1] << 8) + aux_8[0]);
    CYGACC_CALL_IF_DELAY_US(26);

    // Compose and set PHY capabilities advertisement register
    aux_8[1] = 0;
    aux_8[0] = ENC424J600_IEEE802_3STD;
#ifdef CYGNUM_DEVS_ETH_ENC424J600_FLOWC_ENC424J600_ONCHIP_AUTO_FC
    aux_8[1] |= ENC424J600_ADPAUS_SYMM;
#endif

#ifdef CYGNUM_DEVS_ETH_ENC424J600_NO_AUTO_NEGOTIATION
#   ifdef CYGSEM_DEVS_ETH_ENC424J600_FULL_DUPLEX
#      if(CYGNUM_DEVS_ETH_ENC424J600_SPEED == 100)
           aux_8[1] |= ENC424J600_AD100FD;
#      else
           aux_8[0] |= ENC424J600_AD10FD;
#      endif
#   else
#      if(CYGNUM_DEVS_ETH_ENC424J600_SPEED == 100)
           aux_8[0] |= ENC424J600_AD100;
#      else
           aux_8[0] |= ENC424J600_AD10;
#      endif
#   endif
#else
    aux_8[1] |= ENC424J600_AD100FD;
    aux_8[0] |= ENC424J600_AD100 | ENC424J600_AD10FD | ENC424J600_AD10;
#endif

    write_phy(dpd, ENC424J600_PHANA, (aux_8[1] << 8) + aux_8[0]);

#if ENC424J600_DEBUG & 4
    diag_printf("Setting CRC generation\n");
#endif

    // Set ENC424J600_TXCRCEN, PADCFG
    unbanked_register_operation(dpd, ENC424J600_READCRU, ENC424J600_MACON2_U, 2, &aux_8[0]);
    aux_8[0] = 0xa0;      // Pad VLAN frames to 64bytes, others to 60
    aux_8[0] |= ENC424J600_TXCRCEN;  // Calculate and append CRC in transmit frames
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MACON2_U, 2, &aux_8[0]);

#if ENC424J600_DEBUG & 4
    diag_printf("Setting acceptable packet size\n");
#endif
    // Set acceptable packet size
    aux_8[0] = CYGNUM_DEVS_ETH_ENC424J600_ACCEPTABLE_PACKET_SIZE & 0xff;
    aux_8[1] = (CYGNUM_DEVS_ETH_ENC424J600_ACCEPTABLE_PACKET_SIZE >> 8) & 0xff;
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_MAMXFL_U, 2, &aux_8[0]);

#if ENC424J600_DEBUG & 2
    diag_printf("Initializing upper level driver\n");
#endif
    (sc->funs->eth_drv->init)(sc, dpd->esa);

#if ENC424J600_DEBUG & 4
    diag_printf("End of _init().\n");
#endif

    return true;
}

//
// This function is called to "start up" the interface.  It may be called
// multiple times, even when the hardware is already running.  It will be
// called whenever something "hardware oriented" changes and should leave
// the hardware ready to send/receive packets.
//
static void
enc424j600_spi_start(struct eth_drv_sc *sc, unsigned char *enaddr, int flags)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 aux[2];
    cyg_uint8 pkcnt;

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_start() begin.\n");
#endif
    // Disable packet reception
    simple_operation(dpd, ENC424J600_DISABLERX);

    // Enable interrupts in Ethernet chip
    simple_operation(dpd, ENC424J600_CLREIE); // First disable interrupts globally

    aux[0] = ENC424J600_PKTIE | ENC424J600_TXIE | ENC424J600_TXABTIE | ENC424J600_RXABTIE | ENC424J600_PCFULIE; // then set the desired ones
    aux[1] = ENC424J600_LINKIE;
    unbanked_register_operation(dpd, ENC424J600_BITFIELDSETU, ENC424J600_EIE_U, 2, &aux[0]);

    // Are there any packets left in receive buffer?
    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 1, &pkcnt);

    while (0 < pkcnt--)
    {
        simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement on-chip packet counter
    }                                       // This clears ENC424J600_PKTIF, if set.

    // Set the chip's receive buffer start address and buffer head.
    aux[0] = ENC424J600_RXBUF_START & 0xfe;          // make it even, just in case it's not
    aux[1] = (ENC424J600_RXBUF_START >> 8) & 0x7f;   // clear the MSB
#if ENC424J600_DEBUG & 8
    diag_printf("Setting receive buffer start address, ERXST 0x%02x%02x.\n",
                aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXST_U, 2, &aux[0]);
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXHEAD_U, 2, &aux[0]);

    // Next packet will be received at the buffer start address
    *(cyg_uint8 *) & dpd->NextPacketPointer = ENC424J600_RXBUF_START & 0xff;
    *(((cyg_uint8 *)&dpd->NextPacketPointer) + 1) = (ENC424J600_RXBUF_START >> 8 ) & 0xff;

    // Set receive buffer tail
    aux[1] = 0x5f;  // just bellow the end of on-chip SRAM
    aux[0] = 0xfe;
#if ENC424J600_DEBUG & 8
    diag_printf("Setting receive buffer tail, ERXTAIL 0x%02x%02x.\n",
                aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXTAIL_U, 2, &aux[0]);

    // Abort any pending transmission
    aux[0] = ENC424J600_TXRTS;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_ECON1L, 1, &aux[0]);
    dpd->txbusy = false;

    // Set start of transmit buffer
    aux[0] = ENC424J600_TXBUF_START & 0xff;
    aux[1] = (ENC424J600_TXBUF_START >> 8 ) & 0xff;
#if ENC424J600_DEBUG & 8
    diag_printf("Writing ETXST 0x%02x%02x.\n", aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ETXST_U, 2, &aux[0]);

    // Clear any remaining interrupt flags
    aux[0] = ENC424J600_TXIF | ENC424J600_TXABTIF | ENC424J600_RXABTIF | ENC424J600_PCFULIF;
    aux[1] = ENC424J600_LINKIF;
    banked_register_operation(dpd, ENC424J600_BITFIELDCLEAR, ENC424J600_EIRL, 2, &aux[0]);

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    simple_operation(dpd, ENC424J600_SETEIE);  // Enable interrupts
#endif

    // Enable packet reception
    simple_operation(dpd, ENC424J600_ENABLERX);

    banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_ESTATL, 2, &aux[0]);
    if (0 != (aux[1] & ENC424J600_PHYLNK))
    {
        if (ENC424J600_LINK_ON != dpd->link_status)
        {
            dpd->link_status = ENC424J600_LINK_ON;
            set_MAC(dpd, aux[1]);
        }
    }
    else
    {
        dpd->link_status = ENC424J600_LINK_OFF;
    }

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_start() end.\n");
#endif

    return;
}

// Stop receiving and sending packets
static void
enc424j600_spi_stop(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8 aux[2];

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_stop() begin.\n");
#endif
    // Disable packet reception
    simple_operation(dpd, ENC424J600_DISABLERX);

    // Disable interrupts in Ethernet chip
    aux[1] = ENC424J600_INTIE;
    aux[0] = ENC424J600_PKTIE | ENC424J600_TXIE | ENC424J600_TXABTIE | ENC424J600_RXABTIE | ENC424J600_PCFULIE;
    unbanked_register_operation(dpd, ENC424J600_BITFIELDCLEARU, ENC424J600_EIE_U, 2, &aux[0]);

    return;
}

static void
enc424j600_spi_deliver(struct eth_drv_sc *sc)
{

    enc424j600_spi_poll(sc);

    return;
}

// Special control operations
static int
enc424j600_spi_control(struct eth_drv_sc *sc, unsigned long key, void *data,
        int data_length)
{
    enc424j600_priv_data_t *dpd;
    unsigned char *esa;
    struct eth_drv_mc_list *mc_list;
    cyg_uint8 aux;

    if ((NULL == sc) || (NULL == data))
    {
#if ENC424J600_DEBUG & 1
        diag_printf("Wrong parameter(s) for enc424j600_spi_control().\n");
#endif
        return -1;
    }
    dpd = (enc424j600_priv_data_t *)sc->driver_private;
    switch (key)
    {
        case ETH_DRV_SET_MAC_ADDRESS:
            if (ETHER_ADDR_LEN != data_length)
            {
#if ENC424J600_DEBUG & 1
                diag_printf("Wrong data length for ETH_DRV_SET_MAC_ADDRESS.\n");
#endif
                return -1;
            }
            esa = (unsigned char *)data;
            // Write ESA in the chip. Will last till the next reset.
            return write_esa_u(dpd, esa);

#ifdef ETH_DRV_GET_MAC_ADDRESS
        case ETH_DRV_GET_MAC_ADDRESS:
            if (ETHER_ADDR_LEN > data_length)
            {
#if ENC424J600_DEBUG & 1
                diag_printf("Too small data length for ETH_DRV_GET_MAC_ADDRESS.\n");
#endif
                return -1;
            }
            esa = (unsigned char *)data;
            // Get ESA from the chip
            return read_esa_u(dpd, esa);
#endif

        case ETH_DRV_SET_MC_LIST:
        case ETH_DRV_SET_MC_ALL:
            // Note: this code always affects all multicast addresses if any
            // are desired.
            mc_list = data;
            simple_operation(dpd, ENC424J600_DISABLERX);
            aux = ENC424J600_MCEN;
            if (0 == mc_list->len)
            {   // Disable reception of multicast packets
                unbanked_register_operation(dpd,
                    ENC424J600_BITFIELDCLEARU, ENC424J600_ERXFCON_U, 1, &aux);
            }
            else
            {   // Enable reception of multicast packets
                unbanked_register_operation(dpd,
                    ENC424J600_BITFIELDSETU, ENC424J600_ERXFCON_U, 1, &aux);
            }
            simple_operation(dpd, ENC424J600_ENABLERX);

            return 0;

        default:
#if ENC424J600_DEBUG & 1
            diag_printf("Unsupported key %lx for enc424j600_spi_control().\n", key);
#endif
            return -1;
    }
}

// Continue transfer from chip's buffer where the enc424j600_spi_RXevent() stopped.
// Take one packet.
static void
enc424j600_spi_recv(struct eth_drv_sc *sc,
    struct eth_drv_sg *sg_list,
    int chunks)
{
    int chunk;
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint16 rbuf_tail;
    cyg_uint8 * aux_p;
    cyg_uint8 aux[2];

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_recv()...\n");
#endif

    for(chunk = 0; chunk < chunks; chunk++)
    {
        read_from_rx_buffer(dpd, (*sg_list).len, (cyg_uint8 *)(*sg_list).buf);
        sg_list++;
    }
    aux_p = (cyg_uint8 *) & dpd->NextPacketPointer;
    rbuf_tail = *aux_p + *(aux_p+1) * (cyg_uint16)256 ;
    rbuf_tail -= 2;
    if (rbuf_tail == ENC424J600_RXBUF_START)
    {   // Wrap over
        rbuf_tail = 0x5ffe;
    }
    aux[0] = rbuf_tail & 0xff;
    aux[1] = (rbuf_tail >> 8) & 0xff;
#if ENC424J600_DEBUG & 8
    diag_printf("Setting receive buffer tail, ERXTAIL 0x%02x%02x.\n",
        aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ERXTAIL_U, 2, &aux[0]);

    simple_operation(dpd, ENC424J600_SETPKTDEC);   // Decrement packet count. If zeroed,
                                        // ENC424J600_PKTIF interrupt flag will be reset.

    return;
}

// This routine is called to see if it is possible to send another packet.
// It will return non-zero if a transmit is possible, zero otherwise.
static int
enc424j600_spi_can_send(struct eth_drv_sc *sc)
{
    if (((enc424j600_priv_data_t *)sc->driver_private)->txbusy)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// This routine is called to send data to the hardware.
static void
enc424j600_spi_send(struct eth_drv_sc *sc,
    struct eth_drv_sg *sg_list,
    int sg_len,
    int total,
    unsigned long key)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    int block;
    cyg_uint8 aux[2];
    cyg_uint8 opcode = ENC424J600_WGPDATA;

    if (0 >= sg_len) return;

    // Check if there is enough room in transmit buffer
    if (total > ENC424J600_RXBUF_START - ENC424J600_TXBUF_START)
    {
      sc->funs->eth_drv->tx_done(sc, (CYG_ADDRESS)key, 0);
#if ENC424J600_DEBUG & 1
      diag_printf("enc424j600_spi_send(): Packet to send is too large (%d bytes).\n",
            total);
#endif
        return;
    }

    dpd->txbusy = true;
    dpd->txkey = key;

    aux[0] = ENC424J600_TXBUF_START & 0xff;
    aux[1] = (((cyg_uint8)ENC424J600_TXBUF_START) >> 8) & 0xff;
    set_pointer(dpd, ENC424J600_GPBWRPT, (aux[1] << 8) + aux[0]);

#if ENC424J600_DEBUG & 8
    diag_printf("Starting SPI transaction to send a packet.\n");
#endif
    cyg_spi_transaction_begin(dpd->spi_service_device);
    cyg_spi_transaction_transfer(dpd->spi_service_device,
        SPI_POLLING,
        1,
        & opcode,
        NULL,
        ENC424J600_DONT_DROP_CS);

    for (block = 0; block < sg_len; block++)
    {
        cyg_spi_transaction_transfer(dpd->spi_service_device,
            SPI_POLLING,
            sg_list->len,
            (cyg_uint8 *)sg_list->buf,
            NULL,
            (block == sg_len) ? ENC424J600_DO_DROP_CS : ENC424J600_DONT_DROP_CS );
        sg_list++;
    }
    cyg_spi_transaction_end(dpd->spi_service_device);

    aux[0] = total & 0xff;
    aux[1] = (total >> 8 ) & 0xff;
#if ENC424J600_DEBUG & 8
    diag_printf("Writing ETXLEN 0x%02x%02x.\n", aux[1], aux[0]);
#endif
    unbanked_register_operation(dpd, ENC424J600_WRITECRU, ENC424J600_ETXLEN_U, 2, &aux[0]);

#if ENC424J600_DEBUG & 8
    diag_printf("Setting request to send.\n");
#endif
    simple_operation(dpd, ENC424J600_SETTXRTS);

    sc->funs->eth_drv->tx_done(sc, (CYG_ADDRESS)key, 0);

    return;
}

static void
enc424j600_spi_poll(struct eth_drv_sc *sc)
{
    enc424j600_priv_data_t *dpd = (enc424j600_priv_data_t *)sc->driver_private;
    cyg_uint8   aux[2];
    cyg_uint16  iflags;

#if ENC424J600_DEBUG & 8
    diag_printf("enc424j600_spi_poll(%x)...\n", (cyg_uint32)sc);
#endif

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    // Globally disable interrupt source in Ethernet chip
    simple_operation(dpd, ENC424J600_CLREIE);
#endif
    do
    {
        // Figure out what caused the interrupt
        banked_register_operation(dpd, ENC424J600_READCR, ENC424J600_EIRL, 2, &aux[0]);
        aux[1] &= ENC424J600_LINKIF;
        aux[0] &= (ENC424J600_PKTIF | ENC424J600_TXIF | ENC424J600_TXABTIF | ENC424J600_RXABTIF | ENC424J600_PCFULIF);
        iflags = aux[0] + (cyg_uint16)256 * aux[1];
        if (0 != (aux[0] & ENC424J600_PKTIF))
        {   // Packets are pending in chip's receive buffer
            enc424j600_spi_RXevent(sc);
        }
        if ( 0 != (aux[1] & ENC424J600_LINKIF))
        {   // Link status changed
            enc424j600_spi_LINKevent(sc);
        }
        if (0 != (aux[0] & ENC424J600_TXIF))
        {   // Packet has been sent
            enc424j600_spi_TXevent(sc);
        }
        if (0 != (aux[0] & ENC424J600_TXABTIF))
        {   // Packet send has been aborted
            enc424j600_spi_TXABTevent(sc);
        }
        if ( 0 != (aux[0] & ENC424J600_PCFULIF))
        {   // Packet counter overflow
            enc424j600_spi_PCFULevent(sc);
        }
        if ( 0 != (aux[0] & ENC424J600_RXABTIF))
        {   // Incoming packet rejected due to buffer or counter overflow
            enc424j600_spi_RXABTevent(sc);
        }
    } while(0 != iflags);

#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    cyg_drv_interrupt_unmask(CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR);

    // Globally enable interrupt source in Ethernet chip
    simple_operation(dpd, ENC424J600_SETEIE);
#endif

    return;
}

static int
enc424j600_spi_int_vector(struct eth_drv_sc *sc)
{
#ifdef CYGINT_IO_ETH_INT_SUPPORT_REQUIRED
    return CYGNUM_DEVS_ETH_ENC424J600_INTERRUPT_VECTOR;
#else
    return 0;
#endif
}

// - End of higher level interface -----------------------------

// End of enc424j600_spi.c
