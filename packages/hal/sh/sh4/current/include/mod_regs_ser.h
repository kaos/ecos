//=============================================================================
//
//      mod_regs_ser.h
//
//      SCI, SCIF, and IRDA (serial) Module register definitions
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        2000-10-30
// Note:        All three serial module definitions kept in the same file
//              since they share some of the information.
//####DESCRIPTIONEND####
//
//=============================================================================

//++++++ Module SCI ++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//--------------------------------------------------------------------------
// Serial registers. All 8 bit registers.
#define CYGARC_REG_SCSMR1               0xFFE00000 // serial mode register
#define CYGARC_REG_SCBRR1               0xFFE00004 // bit rate register
#define CYGARC_REG_SCSCR1               0xFFE00008 // serial control register
#define CYGARC_REG_SCTDR1               0xFFE0000C // transmit data register
#define CYGARC_REG_SCSSR1               0xFFE00010 // serial status register
#define CYGARC_REG_SCRDR1               0xFFE00014 // receive data register
#define CYGARC_REG_SCRCMR1              0xFFE00018 
#define CYGARC_REG_SCSPTR1              0xFFE0001C // serial port register

// Serial Mode Register
#define CYGARC_REG_SCSMR1_CA            0x80 // communication mode
#define CYGARC_REG_SCSMR1_CHR           0x40 // character length (7 if set)
#define CYGARC_REG_SCSMR1_PE            0x20 // parity enable
#define CYGARC_REG_SCSMR1_OE            0x10 // parity mode
#define CYGARC_REG_SCSMR1_STOP          0x08 // stop bit length
#define CYGARC_REG_SCSMR1_MP            0x04 // multiprocessor mode
#define CYGARC_REG_SCSMR1_CKS1          0x02 // clock select 1
#define CYGARC_REG_SCSMR1_CKS0          0x01 // clock select 0
#define CYGARC_REG_SCSMR1_CKSx_MASK     0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR1_TIE           0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR1_RIE           0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR1_TE            0x20 // transmit enable
#define CYGARC_REG_SCSCR1_RE            0x10 // receive enable
#define CYGARC_REG_SCSCR1_MPIE          0x08 // multiprocessor interrupt enable
#define CYGARC_REG_SCSCR1_TEIE          0x04 // transmit-end interrupt enable
#define CYGARC_REG_SCSCR1_CKE1          0x02 // clock enable 1
#define CYGARC_REG_SCSCR1_CKE0          0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR1_TDRE          0x80 // transmit data register empty
#define CYGARC_REG_SCSSR1_RDRF          0x40 // receive data register full
#define CYGARC_REG_SCSSR1_ORER          0x20 // overrun error
#define CYGARC_REG_SCSSR1_FER           0x10 // framing error
#define CYGARC_REG_SCSSR1_PER           0x08 // parity error
#define CYGARC_REG_SCSSR1_TEND          0x04 // transmit end
#define CYGARC_REG_SCSSR1_MPB           0x02 // multiprocessor bit
#define CYGARC_REG_SCSSR1_MPBT          0x01 // multiprocessor bit transfer

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR1_CLEARMASK     0xf8


// Baud rate values calculation, depending on peripheral clock (Pf)
// n is CKS setting (0-3)
// N = (Pf/(64*2^(2n-1)*B))-1
// With CYGARC_SCBRR_CKSx providing the values 1, 4, 16, 64 we get
//       N = (Pf/(32*_CKS*B))-1
//
// The CYGARC_SCBRR_OPTIMAL_CKS macro should compute the minimal CKS
// setting for the given baud rate and peripheral clock.
//
// The error of the CKS+count value can be computed by:
//  E(%) = ((Pf/((N+1)*B*(64^(n-1)))-1)*100 
//
#define CYGARC_SCBRR_PRESCALE(_b_) \
((((CYGHWR_HAL_SH_ONCHIP_PERIPHERAL_SPEED/32/1/(_b_))-1)<256) ? 1 : \
 (((CYGHWR_HAL_SH_ONCHIP_PERIPHERAL_SPEED/32/4/(_b_))-1)<256) ? 4 : \
 (((CYGHWR_HAL_SH_ONCHIP_PERIPHERAL_SPEED/32/16/(_b_))-1)<256) ? 16 : 64)

// These two macros provide the static values we need to stuff into the
// registers.
#define CYGARC_SCBRR_CKSx(_b_) \
    ((1 == CYGARC_SCBRR_PRESCALE(_b_)) ? 0 : \
     (4 == CYGARC_SCBRR_PRESCALE(_b_)) ? 1 : \
     (16 == CYGARC_SCBRR_PRESCALE(_b_)) ? 2 : 3)
#define CYGARC_SCBRR_N(_b_)     \
    (((_b_) < 4800) ? 0 :       \
      ((_b_) > 115200) ? 0 :    \
       ((CYGHWR_HAL_SH_ONCHIP_PERIPHERAL_SPEED/32/CYGARC_SCBRR_PRESCALE(_b_)/(_b_))-1))


//++++++ Module IRDA +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_IRDA

//--------------------------------------------------------------------------
// IrDA (infrared data association). Can also act as regular serial w FIFO

#define CYGARC_REG_SCSMR1                0xFFE00000 // serial mode register
#define CYGARC_REG_SCBRR1                0xFFE00004 // bit rate register
#define CYGARC_REG_SCSCR1                0xFFE00008 // serial control register
#define CYGARC_REG_SCTDR1                0xFFE0000C // transmit data register
#define CYGARC_REG_SCSSR1                0xFFE00010 // serial status register
#define CYGARC_REG_SCRDR1                0xFFE00014 // receive data register
#define CYGARC_REG_SCRCMR1               0xFFE00018 
#define CYGARC_REG_SCSPTR1               0xFFE0001C // serial port register

// Serial Mode Register
#define CYGARC_REG_SCSMR1_IRMOD          0x80 // IrDA Mode
#define CYGARC_REG_SCSMR1_ICK_MASK       0x78 // IR pulse width
#define CYGARC_REG_SCSMR1_PSEL           0x04 // IR pulse selector(?)
#define CYGARC_REG_SCSMR1_CKS1           0x02 // clock select 1
#define CYGARC_REG_SCSMR1_CKS0           0x01 // clock select 0
#define CYGARC_REG_SCSMR1_CKSx_MASK      0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR1_TIE            0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR1_RIE            0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR1_TE             0x20 // transmit enable
#define CYGARC_REG_SCSCR1_RE             0x10 // receive enable
#define CYGARC_REG_SCSCR1_CKE1           0x02 // clock enable 1
#define CYGARC_REG_SCSCR1_CKE0           0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR1_PER_MASK       0xf000 // number of parity errors
#define CYGARC_REG_SCSSR1_PER_shift      12
#define CYGARC_REG_SCSSR1_FER_MASK       0x0f00 // number of framing errors
#define CYGARC_REG_SCSSR1_FER_shift      8
#define CYGARC_REG_SCSSR1_ER             0x0080 // receive error
#define CYGARC_REG_SCSSR1_TEND           0x0040 // transmit end
#define CYGARC_REG_SCSSR1_TDFE           0x0020 // transmit fifo data empty
#define CYGARC_REG_SCSSR1_BRK            0x0010 // break detection
#define CYGARC_REG_SCSSR1_FER            0x0008 // framing error
#define CYGARC_REG_SCSSR1_PER            0x0004 // parity error
#define CYGARC_REG_SCSSR1_RDF            0x0002 // receive fifo data full
#define CYGARC_REG_SCSSR1_DR             0x0001 // receive data ready

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR2_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR1_CLEARMASK      0xf3

// Serial FIFO Control Register
#define CYGARC_REG_SCFCR1_RTRG_MASK      0xc0   // receive fifo data trigger
#define CYGARC_REG_SCFCR1_RTRG_1         0x00   // trigger on 1 char 
#define CYGARC_REG_SCFCR1_RTRG_4         0x40   // trigger on 4 chars
#define CYGARC_REG_SCFCR1_RTRG_8         0x80   // trigger on 8 chars
#define CYGARC_REG_SCFCR1_RTRG_14        0xc0   // trigger on 14 chars 

#define CYGARC_REG_SCFCR1_TTRG_MASK      0x30   // transmit fifo data trigger
#define CYGARC_REG_SCFCR1_TTRG_8         0x00   // trigger on 8 chars 
#define CYGARC_REG_SCFCR1_TTRG_4         0x10   // trigger on 4 chars
#define CYGARC_REG_SCFCR1_TTRG_2         0x20   // trigger on 2 chars
#define CYGARC_REG_SCFCR1_TTRG_1         0x30   // trigger on 1 char

#define CYGARC_REG_SCFCR1_MCE            0x08   // modem control enable
#define CYGARC_REG_SCFCR1_TFRST          0x04   // transmit fifo reset
#define CYGARC_REG_SCFCR1_RFRST          0x02   // receive fifo reset
#define CYGARC_REG_SCFCR1_LOOP           0x01   // loop back test


// Serial FIFO Data Count Set Register
#define CYGARC_REG_SCFDR1_RCOUNT_MASK    0x001f // number of chars in r fifo
#define CYGARC_REG_SCFDR1_RCOUNT_shift   0
#define CYGARC_REG_SCFDR1_TCOUNT_MASK    0x1f00 // number of chars in t fifo
#define CYGARC_REG_SCFDR1_TCOUNT_shift   8

#endif // CYGARC_SH_MOD_IRDA

//++++++ Module SCIF +++++++++++++++++++++++++++++++++++++++++++++++++++++++
#ifdef CYGARC_SH_MOD_SCIF

//--------------------------------------------------------------------------
// Serial w FIFO registers

#define CYGARC_REG_SCSMR2               0xffe80000 // Serial mode
#define CYGARC_REG_SCBRR2               0xffe80004 // Bit rate
#define CYGARC_REG_SCSCR2               0xffe80008 // Serial control
#define CYGARC_REG_SCFTDR2              0xffe8000c // Transmit FIFO data
#define CYGARC_REG_SCFSR2               0xffe80010 // Serial status
#define CYGARC_REG_SCFRDR2              0xffe80014 // Receive data FIFO
#define CYGARC_REG_SCFCR2               0xffe80018 // FIFO control register
#define CYGARC_REG_SCFDR2               0xffe8001c // FIFO data count set
#define CYGARC_REG_SCSPTR2              0xffe80020 // serial port register
#define CYGARC_REG_SCLSR2               0xffe80024 // line status register

// Serial Mode Register
#define CYGARC_REG_SCSMR2_CHR            0x40 // character length (7 if set)
#define CYGARC_REG_SCSMR2_PE             0x20 // parity enable
#define CYGARC_REG_SCSMR2_OE             0x10 // parity mode
#define CYGARC_REG_SCSMR2_STOP           0x08 // stop bit length
#define CYGARC_REG_SCSMR2_CKS1           0x02 // clock select 1
#define CYGARC_REG_SCSMR2_CKS0           0x01 // clock select 0
#define CYGARC_REG_SCSMR2_CKSx_MASK      0x03 // mask

// Serial Control Register
#define CYGARC_REG_SCSCR2_TIE            0x80 // transmit interrupt enable
#define CYGARC_REG_SCSCR2_RIE            0x40 // receive interrupt enable
#define CYGARC_REG_SCSCR2_TE             0x20 // transmit enable
#define CYGARC_REG_SCSCR2_RE             0x10 // receive enable
#define CYGARC_REG_SCSCR2_CKE1           0x02 // clock enable 1
#define CYGARC_REG_SCSCR2_CKE0           0x01 // clock enable 0

// Serial Status Register
#define CYGARC_REG_SCSSR2_PER_MASK       0xf000 // number of parity errors
#define CYGARC_REG_SCSSR2_PER_shift      12
#define CYGARC_REG_SCSSR2_FER_MASK       0x0f00 // number of framing errors
#define CYGARC_REG_SCSSR2_FER_shift      8
#define CYGARC_REG_SCSSR2_ER             0x0080 // receive error
#define CYGARC_REG_SCSSR2_TEND           0x0040 // transmit end
#define CYGARC_REG_SCSSR2_TDFE           0x0020 // transmit fifo data empty
#define CYGARC_REG_SCSSR2_BRK            0x0010 // break detection
#define CYGARC_REG_SCSSR2_FER            0x0008 // framing error
#define CYGARC_REG_SCSSR2_PER            0x0004 // parity error
#define CYGARC_REG_SCSSR2_RDF            0x0002 // receive fifo data full
#define CYGARC_REG_SCSSR2_DR             0x0001 // receive data ready

// When clearing the status register, always write the value:
// CYGARC_REG_SCSSR2_CLEARMASK & ~bit
// to prevent other bits than the one of interest to be cleared.
#define CYGARC_REG_SCSSR2_CLEARMASK      0xf3

// Serial FIFO Control Register
#define CYGARC_REG_SCFCR2_RTRG_MASK      0xc0   // receive fifo data trigger
#define CYGARC_REG_SCFCR2_RTRG_1         0x00   // trigger on 1 char 
#define CYGARC_REG_SCFCR2_RTRG_4         0x40   // trigger on 4 chars
#define CYGARC_REG_SCFCR2_RTRG_8         0x80   // trigger on 8 chars
#define CYGARC_REG_SCFCR2_RTRG_14        0xc0   // trigger on 14 chars 

#define CYGARC_REG_SCFCR2_TTRG_MASK      0x30   // transmit fifo data trigger
#define CYGARC_REG_SCFCR2_TTRG_8         0x00   // trigger on 8 chars 
#define CYGARC_REG_SCFCR2_TTRG_4         0x10   // trigger on 4 chars
#define CYGARC_REG_SCFCR2_TTRG_2         0x20   // trigger on 2 chars
#define CYGARC_REG_SCFCR2_TTRG_1         0x30   // trigger on 1 char

#define CYGARC_REG_SCFCR2_MCE            0x08   // modem control enable
#define CYGARC_REG_SCFCR2_TFRST          0x04   // transmit fifo reset
#define CYGARC_REG_SCFCR2_RFRST          0x02   // receive fifo reset
#define CYGARC_REG_SCFCR2_LOOP           0x01   // loop back test


// Serial FIFO Data Count Set Register
#define CYGARC_REG_SCFDR2_RCOUNT_MASK    0x001f // number of chars in r fifo
#define CYGARC_REG_SCFDR2_RCOUNT_shift   0
#define CYGARC_REG_SCFDR2_TCOUNT_MASK    0x1f00 // number of chars in t fifo
#define CYGARC_REG_SCFDR2_TCOUNT_shift   8

#endif // CYGARC_SH_MOD_SCIF
