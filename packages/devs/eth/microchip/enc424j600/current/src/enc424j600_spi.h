#ifndef CYGONCE_DEVS_ETH_ENC424J600_SPI_H_
#define CYGONCE_DEVS_ETH_ENC424J600_SPI_H_
//==========================================================================
//
//      enc424j600_spi.h
//
//      Microchip enc424j600 Ethernet chip
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2010 Free Software Foundation, Inc.
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

// Addresses of enc424j600 registers, banked access
// Banks 0, 1, 2 & 3
#define ENC424J600_EUDASTL      (0X16)
#define ENC424J600_EUDASTH      (0X17)
#define ENC424J600_EUDANDL      (0X18)
#define ENC424J600_EUDANDH      (0X19)
#define ENC424J600_ESTATL       (0x1a)
#define ENC424J600_ESTATH       (0x1b)
#define ENC424J600_EIRL         (0x1c)
#define ENC424J600_EIRH         (0x1d)
#define ENC424J600_ECON1L       (0x1e)
#define ENC424J600_ECON1H       (0x1f)

// Only bank 0
#define ENC424J600_ETXSTL       (0x00)
#define ENC424J600_ETXSTH       (0x01)
#define ENC424J600_ETXLENL      (0x02)
#define ENC424J600_ETXLENH      (0x03)
#define ENC424J600_ERXSTL       (0x04)
#define ENC424J600_ERXSTH       (0x05)
#define ENC424J600_ERXTAILL     (0x06)
#define ENC424J600_ERXTAILH     (0x07)
#define ENC424J600_ERXHEADL     (0x08)
#define ENC424J600_ERXHEADH     (0x09)

// Only bank 1
#define ENC424J600_ERXFCONL     (0x14)
#define ENC424J600_ERXFCONH     (0x15)

// Only bank 2
#define ENC424J600_MACON1L      (0x00)
#define ENC424J600_MACON1H      (0x01)
#define ENC424J600_MACON2L      (0x02)
#define ENC424J600_MACON2H      (0x03)
#define ENC424J600_MABBIPGL     (0x04)
#define ENC424J600_MABBIPGH     (0x05)
#define ENC424J600_MAMXFLL      (0x0a)
#define ENC424J600_MAMXFLH      (0x0b)
#define ENC424J600_MICMDL       (0x12)
#define ENC424J600_MICMDH       (0x13)
#define ENC424J600_MIREGADRL    (0x14)
#define ENC424J600_MIREGADRH    (0x15)
#define ENC424J600_EIRL         (0x1c)
#define ENC424J600_EIRH         (0x1d)
#define ENC424J600_ECON1L       (0x1e)
#define ENC424J600_ECON1H       (0x1f)

// Only bank 3
#define ENC424J600_MAADR3L      (0x00)
#define ENC424J600_MIWRL        (0x06)
#define ENC424J600_MIWRH        (0x07)
#define ENC424J600_MIRDL        (0x08)
#define ENC424J600_MIRDH        (0x09)
#define ENC424J600_MISTATL      (0x0a)
#define ENC424J600_MISTATH      (0x0b)
#define ENC424J600_EPAUSL       (0x0c)
#define ENC424J600_EPAUSH       (0x0d)
#define ENC424J600_ECON2L       (0x0e)
#define ENC424J600_ECON2H       (0x0f)
#define ENC424J600_ERXWML       (0x10)
#define ENC424J600_ERXWMH       (0x11)
#define ENC424J600_EIEL         (0x12)
#define ENC424J600_EIEH         (0x13)

// Addresses of enc424j600 registers, unbanked access
// Bank 0
#define ENC424J600_BANK0_Of     (0)
#define ENC424J600_ETXST_U      (ENC424J600_ETXSTL       + ENC424J600_BANK0_Of)
#define ENC424J600_ETXLEN_U     (ENC424J600_ETXLENL      + ENC424J600_BANK0_Of)
#define ENC424J600_EUDAST_U     (ENC424J600_EUDASTL      + ENC424J600_BANK0_Of)
#define ENC424J600_EUDAND_U     (ENC424J600_EUDANDL      + ENC424J600_BANK0_Of)
#define ENC424J600_ESTAT_U      (ENC424J600_ESTATL       + ENC424J600_BANK0_Of)
#define ENC424J600_ERXST_U      (ENC424J600_ERXSTL       + ENC424J600_BANK0_Of)
#define ENC424J600_ERXTAIL_U    (ENC424J600_ERXTAILL     + ENC424J600_BANK0_Of)
#define ENC424J600_ERXHEAD_U    (ENC424J600_ERXHEADL     + ENC424J600_BANK0_Of)

// Bank 1
#define ENC424J600_BANK1_Of     (0x20)
#define ENC424J600_ERXFCON_U   ( ENC424J600_ERXFCONL     + ENC424J600_BANK1_Of)

// Bank 2
#define ENC424J600_BANK2_Of     (0x40)
#define ENC424J600_MACON1_U     (ENC424J600_MACON1L      + ENC424J600_BANK2_Of)
#define ENC424J600_MACON2_U     (ENC424J600_MACON2L      + ENC424J600_BANK2_Of)
#define ENC424J600_MABBIPG_U    (ENC424J600_MABBIPGL     + ENC424J600_BANK2_Of)
#define ENC424J600_MAMXFL_U     (ENC424J600_MAMXFLL      + ENC424J600_BANK2_Of)
#define ENC424J600_MICMD_U      (ENC424J600_MICMDL       + ENC424J600_BANK2_Of)
#define ENC424J600_MIREGADR_U   (ENC424J600_MIREGADRL    + ENC424J600_BANK2_Of)
#define ENC424J600_EIR_U        (ENC424J600_EIRL         + ENC424J600_BANK2_Of)
#define ENC424J600_ECON1_U      (ENC424J600_ECON1L       + ENC424J600_BANK2_Of)

// Bank 3
#define ENC424J600_BANK3_Of     (0x60)
#define ENC424J600_MAADR3_U     (ENC424J600_MAADR3L      + ENC424J600_BANK3_Of)
#define ENC424J600_MIWR_U       (ENC424J600_MIWRL        + ENC424J600_BANK3_Of)
#define ENC424J600_MIRD_U       (ENC424J600_MIRDL        + ENC424J600_BANK3_Of)
#define ENC424J600_MISTAT_U     (ENC424J600_MISTATL      + ENC424J600_BANK3_Of)
#define ENC424J600_EPAUS_U      (ENC424J600_EPAUSL       + ENC424J600_BANK3_Of)
#define ENC424J600_ECON2_U      (ENC424J600_ECON2L       + ENC424J600_BANK3_Of)
#define ENC424J600_ERXWM_U      (ENC424J600_ERXWML       + ENC424J600_BANK3_Of)
#define ENC424J600_EIE_U        (ENC424J600_EIEL         + ENC424J600_BANK3_Of)

// Addresses of enc424j600 PHY registers, special access

enum enc424j600_pr_address_e
{
    ENC424J600_PHCON1  = 0,
    ENC424J600_PHSTAT1 = 1,
    ENC424J600_PHANA   = 4,
    ENC424J600_PHANLPA = 5,
    ENC424J600_PHANE   = 6,
    ENC424J600_PHCON2  = 0x11,
    ENC424J600_PHSTST2 = 0x1b,
    ENC424J600_PHSTAT3 = 0x1f
};

//---------- end of register addresses --------------------------------------------------

//---------- definitions of bits in control registers -----------------------------------

// Interrupt flags (EIR register) higher byte
#define ENC424J600_MODEXIF (0x40)
#define ENC424J600_HASHIF  (0x20)
#define ENC424J600_AESIF   (0x10)
#define ENC424J600_LINKIF  (0x08)    // PHY status has changed

// Interrupt flags (EIR register) lower byte
#define ENC424J600_PKTIF   (0x40)    // RX packet(s) pending
#define ENC424J600_DMAIF   (0x20)
#define ENC424J600_TXIF    (0x08)    // Transmit done
#define ENC424J600_TXABTIF (0x04)    // Packet transmission aborted due to an error
#define ENC424J600_RXABTIF (0x02)    // RX packet rejected due to lack of space / to many
                                     // pending packets
#define ENC424J600_PCFULIF (0x01)    // To many pending packets

// Interrupt enable bits (EIE register) higher byte
#define ENC424J600_INTIE   (0x80)    // Global interrupt enable
#define ENC424J600_MODEXIE (0x40)
#define ENC424J600_HASHIE  (0x20)
#define ENC424J600_AESIE   (0x10)
#define ENC424J600_LINKIE  (0x08)    // PHY status has changed

// Interrupt enable bits (EIE register) lower byte
#define ENC424J600_PKTIE   (0x40)    // RX packet(s) pending
#define ENC424J600_DMAIE   (0x20)
#define ENC424J600_TXIE    (0x08)    // Transmit done
#define ENC424J600_TXABTIE (0x04)    // Packet transmission aborted due to an error
#define ENC424J600_RXABTIE (0x02)    // RX packet rejected due to lack of space / too many
                                     // pending packets
#define ENC424J600_PCFULIE (0x01)    // Too many pending packets

// MICMD
#define ENC424J600_MIIRD       (0x01)
#define ENC424J600_MIISCAN     (0x02)

// MISTAT
#define ENC424J600_MISTAT_BUSY     (0x01)
#define ENC424J600_MISTAT_SCAN     (0x02)
#define ENC424J600_MISTAT_NVALID   (0x04)

//ESTAT upper byte
#define ENC424J600_PHYLNK      (0x01)
#define ENC424J600_PHYDPX      (0x04)
#define ENC424J600_CLKRDY      (0x10)
#define ENC424J600_RXBUSY      (0x20)
#define ENC424J600_FCIDLE      (0x40)
#define ENC424J600_INT         (0x80)

// ECON1 lower byte
#define ENC424J600_TXRTS       (0x02)

//ECON2 upper byte
#define ENC424J600_STRCH       (0x40)
#define ENC424J600_ETHEN       (0x80)

// ECON2 lower byte
#define ENC424J600_ETHRST      (0x10)
#define ENC424J600_AUTOFC      (0x80)

// MACON2 upper byte
#define ENC424J600_NOBKOFF     (0x10)
#define ENC424J600_BPEN        (0x20)
#define ENC424J600_DEFER       (0x40)

// MACON2 lower byte
#define ENC424J600_FULDPX      (0x01)
#define ENC424J600_HFRMEN      (0x04)
#define ENC424J600_PHDREN      (0x08)
#define ENC424J600_TXCRCEN     (0x10)

// ERXFCON lower byte
#define ENC424J600_BCEN        (0x01)    // accept broadcast packets
#define ENC424J600_MCEN        (0x02)    // accept multicast packets
#define ENC424J600_UCEN        (0x08)    // accept packets destined for local MAC address
#define ENC424J600_RUNTEN      (0x10)    // discard runt packets
#define ENC424J600_CRCEN       (0x40)    // discard packets with invalid CRC

// PHCON1 upper byte
#define ENC424J600_PFULDPX     (0x01)    // full duplex
#define ENC424J600_RENEG       (0x02)    // restart negotiation
#define ENC424J600_ANEN        (0x10)    // auto negotiation enable
#define ENC424J600_SPD100      (0x20)    // 100Mb/s

// PHANA upper byte
#define ENC424J600_ADPAUS_BOTH     (0x0c)    // both symmetric and asymmetric flow control
#define ENC424J600_ADPAUS_ASYM     (0x08)    // asymmetric flow control towards link partner
#define ENC424J600_ADPAUS_SYMM     (0x04)    // symmetric flow control
#define ENC424J600_ADPAUS_NONE     (0x00)    // no flow control
#define ENC424J600_AD100FD         (0x01)

// PHANA lower byte
#define ENC424J600_AD100           (0x80)
#define ENC424J600_AD10FD          (0x40)
#define ENC424J600_AD10            (0x20)
#define ENC424J600_IEEE802_3STD    (0x01)    //  IEEE Standard Selector Field bits

//---------- end of definitions of bits in control registers -------------------------

// enc424j600 SPI instructions opcodes

// Single byte instructions
enum enc424j600_spi_opcode_1_e
{
    ENC424J600_SETETHRST   = 0xca, // Issue System Reset
    ENC424J600_SETPKTDEC   = 0xcc, // Decrement packet counter
    ENC424J600_SETTXRTS    = 0xd4, // Send a packet
    ENC424J600_FCDISABLE   = 0xe0, // Disable flow control
    ENC424J600_FCSINGLE    = 0xe2, // Send a single pause frame
    ENC424J600_FCMULTIPLE  = 0xe4, // Keep sending pause frames
    ENC424J600_FCCLEAR     = 0xe6, // Send the final pause frame
    ENC424J600_ENABLERX    = 0xe8, // Enable receiver
    ENC424J600_DISABLERX   = 0xea, // Disable receiver
    ENC424J600_SETEIE      = 0xec, // Enable interrupts
    ENC424J600_CLREIE      = 0xee, // Disable interrupts
};

// Pointer definitions for 3-byte instructions
enum enc424j600_spi_bufferp_e
{
    ENC424J600_GPBRDPT     = 0x60, // General purpose buffer read pointer
    ENC424J600_RXRDPT      = 0x64, // Receive buffer read pointer
    ENC424J600_UDARDPT     = 0x68, // User-defined area read pointer
    ENC424J600_GPBWRPT     = 0x6c, // General purpose buffer write pointer
    ENC424J600_RXWRPT      = 0x70, // Receive buffer write pointer
    ENC424J600_UDAWRPT     = 0x74, // User-defined area write pointer

};

enum enc424j600_spi_opcode_3_e
{
    ENC424J600_WRITE3     = 0x00, // Pointer writing
    ENC424J600_READ3      = 0x02  // Pointer reading

};

// N-byte unbanked SFR instructions
enum enc424j600_spi_opcode_u_e
{
    ENC424J600_READCRU         = 0x20, // Read Control Register, Unbanked
    ENC424J600_WRITECRU        = 0x22, // Write Control Register, Unbanked
    ENC424J600_BITFIELDSETU    = 0x24, // Bit Field Set, Unbanked
    ENC424J600_BITFIELDCLEARU  = 0x26 // Bit Field Clear, Unbanked
};

// N-byte banked SFR instructions
enum enc424j600_spi_opcode_e
{
    ENC424J600_READCR          = 0x00, // Read Control Register, Banked
    ENC424J600_WRITECR         = 0x40, // Write Control Register, Banked
    ENC424J600_BITFIELDSET     = 0x80, // Bit Field Set, Banked
    ENC424J600_BITFIELDCLEAR   = 0xa0 // Bit Field Clear, Banked
};

// N-byte SRAM instructions
enum enc424j600_spi_opcode_sram_e
{
    ENC424J600_RGPDATA = 0x28,  // Read from General Purpose buffer
    ENC424J600_WGPDATA = 0x2a,  // Write to General Purpose buffer
    ENC424J600_RRXDATA = 0x2c,  // Read from Rx buffer
    ENC424J600_WRXDATA = 0x2e   // Write to Rx buffer
};

#define ENC424J600_ETH_INTERRUPT_LEVEL_HIGH    (true)
#define ENC424J600_ETH_INTERRUPT_LEVEL_LOW     (false)
#define ENC424J600_ETH_INTERRUPT_EDGE_RISING   (true)
#define ENC424J600_ETH_INTERRUPT_EDGE_FALLING  (false)

//#define ETH_DRV_GET_MAC_ADDRESS (0x1100)

#endif /* ifndef CYGONCE_DEVS_ETH_ENC424J600_SPI_H_ */
// End of enc424j600_spi.h
