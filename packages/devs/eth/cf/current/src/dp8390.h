//==========================================================================
//
//      dev/dp8390.h
//
//      National Semiconductor ethernet chip
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//####BSDCOPYRIGHTBEGIN####
//
// -------------------------------------------
//
// Portions of this software may have been derived from OpenBSD or other sources,
// and are covered by the appropriate copyright disclaimers included herein.
//
// -------------------------------------------
//
//####BSDCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2000-07-07
// Purpose:      
// Description:  
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

typedef volatile union {
    union {
        struct {
            unsigned char cr;
            unsigned char clda0;
            unsigned char clda1;
            unsigned char bndry;
            unsigned char tsr;
            unsigned char ncr;
            unsigned char fifo;
            unsigned char isr;
            unsigned char crda0;
            unsigned char crda1;
            unsigned char _reserved[2];
            unsigned char rsr;
            unsigned char fer;
            unsigned char cer;
            unsigned char missed;
            unsigned char data;
            unsigned char _reserved2[14];
            unsigned char reset;
        } page0;
        struct {
            unsigned char cr;
            unsigned char par[6];
            unsigned char curp;
            unsigned char mar[8];
        } page1;
        struct {
            unsigned char cr;
            unsigned char pstart;
            unsigned char pstop;
            unsigned char rnpp;
            unsigned char tpsr;
            unsigned char lnpp;
            unsigned char ach;
            unsigned char acl;
            unsigned char _reserved[4];
            unsigned char rcr;
            unsigned char tcr;
            unsigned char dcr;
            unsigned char imr;
        } page2;
        unsigned char data;
    } r;  // Read registers
    union {
        struct {
            unsigned char cr;
            unsigned char pstart;
            unsigned char pstop;
            unsigned char bndry;
            unsigned char tpsr;
            unsigned char tbcl;
            unsigned char tbch;
            unsigned char isr;
            unsigned char rsal;
            unsigned char rsah;
            unsigned char rbcl;
            unsigned char rbch;
            unsigned char rcr;
            unsigned char tcr;
            unsigned char dcr;
            unsigned char imr;
            unsigned char data;
            unsigned char _reserved[14];
            unsigned char reset;
        } page0;
        struct {
            unsigned char cr;
            unsigned char par[6];
            unsigned char curp;
            unsigned char mar[8];
        } page1;
        struct {
            unsigned char cr;
            unsigned char clda0;
            unsigned char clda1;
            unsigned char rnpp;
            unsigned char tpsr;
            unsigned char lnpp;
            unsigned char ach;
            unsigned char acl;
            unsigned char _reserved[8];
        } page2;
    } w;  // Write registers
} dp8390_regs; // __attribute__ ((aligned(1), packed));

// Command register - common to all pages

#define DP8390_CR_STOP    0x01   // Stop: software reset
#define DP8390_CR_START   0x02   // Start: initialize device
#define DP8390_CR_TXPKT   0x04   // Transmit packet
#define DP8390_CR_RDMA    0x08   // Read DMA  (recv data from device)
#define DP8390_CR_WDMA    0x10   // Write DMA (send data to device)
#define DP8390_CR_SEND    0x18   // Send packet
#define DP8390_CR_NODMA   0x20   // Remote (or no) DMA
#define DP8390_CR_PAGE0   0x00   // Page select
#define DP8390_CR_PAGE1   0x40
#define DP8390_CR_PAGE2   0x80
#define DP8390_CR_PAGEMSK 0x3F   // Used to mask out page bits

// Data configuration register

#define DP8390_DCR_WTS    0x01   // 1=16 bit word transfers
#define DP8390_DCR_BOS    0x02   // 1=Little Endian
#define DP8390_DCR_LAS    0x04   // 1=Single 32 bit DMA mode
#define DP8390_DCR_LS     0x08   // 1=normal mode, 0=loopback
#define DP8390_DCR_ARM    0x10   // 0=no send command (program I/O)
#define DP8390_DCR_FIFO_1 0x00   // FIFO threshold
#define DP8390_DCR_FIFO_2 0x20
#define DP8390_DCR_FIFO_4 0x40
#define DP8390_DCR_FIFO_6 0x60

#define DP8390_DCR_INIT (DP8390_DCR_LS|DP8390_DCR_FIFO_4)

// Interrupt status register

#define DP8390_ISR_RxP    0x01   // Packet received
#define DP8390_ISR_TxP    0x02   // Packet transmitted
#define DP8390_ISR_RxE    0x04   // Receive error
#define DP8390_ISR_TxE    0x08   // Transmit error
#define DP8390_ISR_OFLW   0x10   // Receive overflow
#define DP8390_ISR_CNT    0x20   // Tally counters need emptying
#define DP8390_ISR_RDC    0x40   // Remote DMA complete
#define DP8390_ISR_RESET  0x80   // Device has reset (shutdown, error)

// Interrupt mask register

#define DP8390_IMR_RxP    0x01   // Packet received
#define DP8390_IMR_TxP    0x02   // Packet transmitted
#define DP8390_IMR_RxE    0x04   // Receive error
#define DP8390_IMR_TxE    0x08   // Transmit error
#define DP8390_IMR_OFLW   0x10   // Receive overflow
#define DP8390_IMR_CNT    0x20   // Tall counters need emptying
#define DP8390_IMR_RDC    0x40   // Remote DMA complete

#define DP8390_IMR_All    0x3F   // Everything but remote DMA

// Receiver control register

#define DP8390_RCR_SEP    0x01   // Save bad(error) packets
#define DP8390_RCR_AR     0x02   // Accept runt packets
#define DP8390_RCR_AB     0x04   // Accept broadcast packets
#define DP8390_RCR_AM     0x08   // Accept multicast packets
#define DP8390_RCR_PROM   0x10   // Promiscuous mode
#define DP8390_RCR_MON    0x20   // Monitor mode - 1=accept no packets

// Receiver status register

#define DP8390_RSR_RxP    0x01   // Packet received
#define DP8390_RSR_CRC    0x02   // CRC error
#define DP8390_RSR_FRAME  0x04   // Framing error
#define DP8390_RSR_FO     0x08   // FIFO overrun
#define DP8390_RSR_MISS   0x10   // Missed packet
#define DP8390_RSR_PHY    0x20   // 0=pad match, 1=mad match
#define DP8390_RSR_DIS    0x40   // Receiver disabled
#define DP8390_RSR_DFR    0x80   // Receiver processing deferred

// Transmitter control register

#define DP8390_TCR_NOCRC  0x01   // 1=inhibit CRC
#define DP8390_TCR_NORMAL 0x00   // Normal transmitter operation
#define DP8390_TCR_LOCAL  0x02   // Internal NIC loopback
#define DP8390_TCR_INLOOP 0x04   // Full internal loopback
#define DP8390_TCR_OUTLOOP 0x08  // External loopback
#define DP8390_TCR_ATD    0x10   // Auto transmit disable
#define DP8390_TCR_OFFSET 0x20   // Collision offset adjust

// Transmit status register

#define DP8390_TSR_TxP    0x01   // Packet transmitted
#define DP8390_TSR_COL    0x04   // Collision (at least one)
#define DP8390_TSR_ABT    0x08   // Aborted because of too many collisions
#define DP8390_TSR_CRS    0x10   // Lost carrier
#define DP8390_TSR_FU     0x20   // FIFO underrun
#define DP8390_TSR_CDH    0x40   // Collision Detect Heartbeat
#define DP8390_TSR_OWC    0x80   // Collision outside normal window

// Page (buffer) allocation
#if 0
#define DP8390_RX_START   1
#define DP8390_RX_STOP    255-12
#define DP8390_TX_BUF1    (DP8390_RX_STOP+1)
#define DP8390_TX_BUF2    (DP8390_TX_BUF1+6)
#define DP8390_TX_STOP    255
#else
#if 0
#define DP8390_RX_START   1
#define DP8390_RX_STOP    127-12
#define DP8390_TX_BUF1    (DP8390_RX_STOP+1)
#define DP8390_TX_BUF2    (DP8390_TX_BUF1+6)
#define DP8390_TX_STOP    127
#else
#define DP8390_RX_START   0x60
#define DP8390_RX_STOP    0x80
#define DP8390_TX_BUF1    0x40
#define DP8390_TX_BUF2    0x50
#define DP8390_TX_STOP    127
#endif
#endif

#define IEEE_8023_MAX_FRAME         1518    // Largest possible ethernet frame
#define IEEE_8023_MIN_FRAME           64    // Smallest possible ethernet frame

#define SC_LPE_MANUF 0x0104

