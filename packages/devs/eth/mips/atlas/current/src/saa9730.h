#ifndef CYGONCE_DEVS_ETH_MIPS_ATLAS_SAA9730_H
#define CYGONCE_DEVS_ETH_MIPS_ATLAS_SAA9730_H
/*==========================================================================
//
//      saa9730.h
//      Philips SAA9730 IO Chip Ethernet Interface
//
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     msalter
// Contributors:  msalter
// Date:          2000-12-09
// Description:   Definitions for Philips SAA9730 Ethernet module.
//
//####DESCRIPTIONEND####
*/

// QS6612 PHY definitions 

#define PHY_CONTROL                     0
#define PHY_STATUS                      1
#define PHY_REG31                       31

#define PHY_CONTROL_RESET               (1 << 15)
#define PHY_CONTROL_AUTO_NEG            (1 << 12)
#define PHY_CONTROL_RESTART_AUTO_NEG    (1 <<  9)

#define PHY_STATUS_LINK_UP              (1 << 2)

#define PHY_REG31_OPMODE_SHIFT           2
#define PHY_REG31_OPMODE_MSK             (7 << PHY_REG31_OPMODE_SHIFT)

#define OPMODE_AUTONEGOTIATE             0
#define OPMODE_10BASET_HALFDUPLEX        1
#define OPMODE_100BASEX_HALFDUPLEX       2
#define OPMODE_REPEATER_MODE             3
#define OPMODE_UNDEFINED                 4
#define OPMODE_10BASET_FULLDUPLEX        5
#define OPMODE_100BASEX_FULLDUPLEX       6
#define OPMODE_ISOLATE                   7

#define QS6612_PHY_ADDRESS              0
#define PHY_ADDRESS                     QS6612_PHY_ADDRESS

// Number of 6-byte entries in the CAM
#define SAA9730_CAM_ENTRIES                       10

// TX and RX packet size fixed at 2k bytes by hw
#define SAA9730_PACKET_SIZE   2048

// Number of TX buffers = number of RX buffers = 2,
// which is fixed according to HW requirements
#define SAA9730_BUFFERS                           2

// Number of RX packets per RX buffer
#define SAA9730_RXPKTS_PER_BUFFER                2

// Number of TX packets per TX buffer
#define SAA9730_TXPKTS_PER_BUFFER                1

// Minimum packet size
#define SAA9730_MIN_PACKET_SIZE                   60

// owner ship bit
#define SAA9730_BLOCK_OWNED_BY_SYSTEM             0
#define SAA9730_BLOCK_OWNED_BY_HARDWARE           1

// Default Rcv interrupt count
#define SAA9730_DEFAULT_RCV_INTERRUPT_CNT         4

// Default maxium transmit retry
#define SAA9730_DEFAULT_MAX_TXM_RETRY	      16

// Default time out value
#define SAA9730_DEFAULT_TIME_OUT_CNT              200

// MAX map registers
#define SAA9730__MAX_MAP_REGISTERS		      64

// Defines used by Interrupt code
#define  SAA9730_DMA_PACKET_SIZE                  2048
#define  SAA9730_VALID_PACKET                     0xC0000000
#define  SAA9730_FRAME_TYPELEN_OFFSET             12
#define  SAA9730_ETH_MIN_FRAME_SIZE               60
#define  SAA9730_DEST_ADDR_SIZE                   6
#define  SAA9730_SRC_ADDR_SIZE                    6
#define  SAA9730_TYPE_LEN_SIZE                    2

// MAC receive error
#define  SAA9730_MAC_GOOD_RX                      (0x00004000) << 11
#define  SAA9730_MAC_RCV_ALIGN_ERROR              (0x00000100) << 11
#define  SAA9730_MAC_RCV_CRC_ERROR                (0x00000200) << 11
#define  SAA9730_MAC_RCV_OVERFLOW                 (0x00000400) << 11

// This number is arbitrary and can be increased if needed
#define SAA9730_MAX_MULTICAST_ADDRESSES           20 

// These macros are machine dependent.  
// They convert the big endian representation to the little endian
// format, used by the DMA-engine of the LAN-controller on the PCI-bus.
// Convert by BE:
#ifdef MIPSEL
#define cpu_to_le32(x) (x)
#define le32_to_cpu(x) (x)
#else
#define cpu_to_le32(x) ((((cyg_uint32)(x)) << 24) |   \
                       ((0x0000FF00UL & ((cyg_uint32)(x))) <<  8) |   \
                       ((0x00FF0000UL & ((cyg_uint32)(x))) >>  8) |   \
                       (((cyg_uint32)(x))  >> 24))
#define le32_to_cpu(x) cpu_to_le32(x)
#endif
// Convert by LE:
#ifdef MIPSEB
#define cpu_to_be32( value ) (value)
#define be32_to_cpu( value ) (value)
#else
#define cpu_to_be32(x) ((((cyg_uint32)(x)) << 24) |   \
                       ((0x0000FF00UL & ((cyg_uint32)(x))) <<  8) |   \
                       ((0x00FF0000UL & ((cyg_uint32)(x))) >>  8) |   \
                       (((cyg_uint32)(x))  >> 24))
#define be32_to_cpu(x) cpu_to_be32(x)
#endif

// SAA9730 Event Manager Registers
#define SAA9730_EVM_ISR         *((volatile unsigned *)(__base + 0x02000))
#define SAA9730_EVM_IER         *((volatile unsigned *)(__base + 0x02004))
#define SAA9730_EVM_IMR         *((volatile unsigned *)(__base + 0x02008))

#define SAA9730_EVM_IER_SW      *((volatile unsigned *)(__base + 0x0202c))

#define SAA9730_EVM_LAN_INT     (1<<16)         // LAN interrupt bit
#define SAA9730_EVM_MASTER      (1<<0)          // Master interrupt bit

//  SAA9730 LAN Registers
#define SAA9730_TXBUFA          *((volatile unsigned *)(__base + 0x20400)) // TX buffer A

#define SAA9730_TXBUFB          *((volatile unsigned *)(__base + 0x20404)) // TX buffer B

#define SAA9730_RXBUFA          *((volatile unsigned *)(__base + 0x20408)) // RX buffer A

#define SAA9730_RXBUFB          *((volatile unsigned *)(__base + 0x2040C)) // RX buffer B

#define SAA9730_PKTCNT          *((volatile unsigned *)(__base + 0x20410)) // Packet count

#define SAA9730_OK2USE          *((volatile unsigned *)(__base + 0x20414)) // OK-to-use
#  define SAA9730_OK2USE_TXA  8 	   
#  define SAA9730_OK2USE_TXB  4 	   
#  define SAA9730_OK2USE_RXA  2 	   
#  define SAA9730_OK2USE_RXB  1 	   

#define SAA9730_DMACTL          *((volatile unsigned *)(__base + 0x20418)) // DMA control
#  define SAA9730_DMACTL_BLKINT           (1 << 31)
#  define SAA9730_DMACTL_MAXXFER_ANY      (0 << 18)
#  define SAA9730_DMACTL_MAXXFER_8        (1 << 18)
#  define SAA9730_DMACTL_MAXXFER_32       (2 << 18)
#  define SAA9730_DMACTL_MAXXFER_64       (3 << 18)
#  define SAA9730_DMACTL_ENDIAN_LITTLE    (0 << 16)
#  define SAA9730_DMACTL_ENDIAN_2143      (1 << 16)
#  define SAA9730_DMACTL_ENDIAN_4321      (2 << 16)
#  define SAA9730_DMACTL_RXINTCNT_SHIFT   8
#  define SAA9730_DMACTL_RXINTCNT_MSK     (0xff << SAA9730_DMACTL_RXINTCNT_SHIFT)
#  define SAA9730_DMACTL_ENTX             (1 << 7)
#  define SAA9730_DMACTL_ENRX             (1 << 6)
#  define SAA9730_DMACTL_RXFULL           (1 << 5)
#  define SAA9730_DMACTL_RXTOINT          (1 << 4)
#  define SAA9730_DMACTL_RXINT            (1 << 3)
#  define SAA9730_DMACTL_TXINT            (1 << 2)
#  define SAA9730_DMACTL_MACTXINT         (1 << 1)
#  define SAA9730_DMACTL_MACRXINT         (1 << 0)

#define SAA9730_TIMOUT          *((volatile unsigned *)(__base + 0x2041C)) // Time out

#define SAA9730_DMASTA          *((volatile unsigned *)(__base + 0x20420)) // DMA status
#  define SAA9730_DMASTA_TXABADR_MSK          (1 << 19)
#  define SAA9730_DMASTA_TXBBADR_MSK          (1 << 18)
#  define SAA9730_DMASTA_RXABADR_MSK          (1 << 17)
#  define SAA9730_DMASTA_RXBBADR_MSK          (1 << 16)
#  define SAA9730_DMASTA_RXBBADR_SHIFT        8
#  define SAA9730_DMASTA_RXPCKCNT_MASK        (0xff << SAA9730_DMASTA_RXPCKCNT_SHIFT)
#  define SAA9730_DMASTA_TXMACBUSY_MSK        (1 << 7)
#  define SAA9730_DMASTA_RXAFULL_MSK          (1 << 6)
#  define SAA9730_DMASTA_RXBFULL_MSK          (1 << 5)
#  define SAA9730_DMASTA_RXTOINT_MSK          (1 << 4)
#  define SAA9730_DMASTA_RXINT_MSK            (1 << 3)
#  define SAA9730_DMASTA_TXINT_MSK            (1 << 2)
#  define SAA9730_DMASTA_MACTXINT_MSK         (1 << 1)
#  define SAA9730_DMASTA_MACRXINT_MSK         (1 << 0)

#define SAA9730_DMATST          *((volatile unsigned *)(__base + 0x20424)) // DMA loop back
#  define SAA9730_DMATST_LPBACK           (1 << 24)
#  define SAA9730_DMATST_RESET            1

#define SAA9730_PAUSE           *((volatile unsigned *)(__base + 0x20430)) // Pause count

#define SAA9730_REMPAUSE        *((volatile unsigned *)(__base + 0x20434)) // Remote Pause count

#define SAA9730_MACCTL          *((volatile unsigned *)(__base + 0x20440)) // MAC control
#  define SAA9730_MACCTL_MISSRINT         (1 << 13)
#  define SAA9730_MACCTL_MISSROLL         (1 << 10)
#  define SAA9730_MACCTL_LOOP10           (1 << 7)
#  define SAA9730_MACCTL_CONMODE_AUTOMATIC  (0 << 5)
#  define SAA9730_MACCTL_CONMODE_FORCE_10MB (1 << 5)
#  define SAA9730_MACCTL_CONMODE_FORCE_MII  (2 << 5)
#  define SAA9730_MACCTL_LPBACK           (1 << 4)
#  define SAA9730_MACCTL_FULLDUP          (1 << 3)
#  define SAA9730_MACCTL_RESET            (1 << 2)
#  define SAA9730_MACCTL_HALTNOW          (1 << 1)
#  define SAA9730_MACCTL_HALTREQ          (1 << 0)

#define SAA9730_CAMCTL          *((volatile unsigned *)(__base + 0x20444)) // CAM control
#  define SAA9730_CAMCTL_COMPARE          (1 << 4)
#  define SAA9730_CAMCTL_NEGATE           (1 << 3)
#  define SAA9730_CAMCTL_BROADCAST        (1 << 2)
#  define SAA9730_CAMCTL_MULTICAST        (1 << 1)
#  define SAA9730_CAMCTL_UNICAST          (1 << 0)

#define SAA9730_TXCTL           *((volatile unsigned *)(__base + 0x20448)) // TX control
#  define SAA9730_TXCTL_COMPLINT            (1 << 14)
#  define SAA9730_TXCTL_TXPARINT            (1 << 13)
#  define SAA9730_TXCTL_LATECOLLINT         (1 << 12)
#  define SAA9730_TXCTL_EXCOLLINT           (1 << 11)
#  define SAA9730_TXCTL_CARRIERINT          (1 << 10)
#  define SAA9730_TXCTL_DEFERINT            (1 << 9)
#  define SAA9730_TXCTL_UNDERINT            (1 << 8)
#  define SAA9730_TXCTL_MII10               (1 << 7)
#  define SAA9730_TXCTL_SDPAUSE             (1 << 6)
#  define SAA9730_TXCTL_NOEXDEF             (1 << 5)
#  define SAA9730_TXCTL_FBACK               (1 << 4)
#  define SAA9730_TXCTL_NOCRC               (1 << 3)
#  define SAA9730_TXCTL_NOPAD               (1 << 2)
#  define SAA9730_TXCTL_TXHALT              (1 << 1)
#  define SAA9730_TXCTL_ENTX                (1 << 0)

#define SAA9730_TXSTA           *((volatile unsigned *)(__base + 0x2044C)) // TX status
#  define SAA9730_TXSTA_SQERR             (1 << 16)
#  define SAA9730_TXSTA_TXHALTED          (1 << 15)
#  define SAA9730_TXSTA_COMPLETION        (1 << 14)
#  define SAA9730_TXSTA_PARITYERR         (1 << 13)
#  define SAA9730_TXSTA_LATECOLLERR       (1 << 12)
#  define SAA9730_TXSTA_WAS10MB           (1 << 11)
#  define SAA9730_TXSTA_LOSTCARRIER       (1 << 10)
#  define SAA9730_TXSTA_EXDEFER           (1 << 9)
#  define SAA9730_TXSTA_UNDERRUN          (1 << 8)
#  define SAA9730_TXSTA_INTERRUPT         (1 << 7)
#  define SAA9730_TXSTA_PAUSED            (1 << 6)
#  define SAA9730_TXSTA_DEFERRED          (1 << 5)
#  define SAA9730_TXSTA_EXCOLL            (1 << 4)
#  define SAA9730_TXSTA_COLLISIONS_MASK   0xf

#define SAA9730_RXCTL           *((volatile unsigned *)(__base + 0x20450)) // RX control
#  define SAA9730_RXCTL_ENGOOD            (1 << 14)
#  define SAA9730_RXCTL_ENPARITY          (1 << 13)
#  define SAA9730_RXCTL_ENLONGERR         (1 << 11)
#  define SAA9730_RXCTL_ENOVER            (1 << 10)
#  define SAA9730_RXCTL_ENCRCERR          (1 << 9)
#  define SAA9730_RXCTL_ENALIGN           (1 << 8)
#  define SAA9730_RXCTL_IGNORECRC         (1 << 6)
#  define SAA9730_RXCTL_PASSCTL           (1 << 5)
#  define SAA9730_RXCTL_STRIPCRC          (1 << 4)
#  define SAA9730_RXCTL_SHORTEN           (1 << 3)
#  define SAA9730_RXCTL_LONGEN            (1 << 2)
#  define SAA9730_RXCTL_RXHALT            (1 << 1)
#  define SAA9730_RXCTL_ENRX              (1 << 0)

#define SAA9730_RXSTA           *((volatile unsigned *)(__base + 0x20454)) // RX status
#  define SAA9730_RXSTA_HALTED            (1 << 15)
#  define SAA9730_RXSTA_GOOD              (1 << 14)
#  define SAA9730_RXSTA_PARITY            (1 << 13)
#  define SAA9730_RXSTA_LONGERR           (1 << 11)
#  define SAA9730_RXSTA_OVERFLOW          (1 << 10)
#  define SAA9730_RXSTA_CRCERR            (1 << 9)
#  define SAA9730_RXSTA_ALIGNERR          (1 << 8)
#  define SAA9730_RXSTA_WAS10MB           (1 << 7)
#  define SAA9730_RXSTA_INTERRUPT         (1 << 6)
#  define SAA9730_RXSTA_CONTROLRCV        (1 << 5)

#define SAA9730_MDDATA          *((volatile unsigned *)(__base + 0x20458)) // PHY mgmt data
#  define SAA9730_MDDATA_DATA_MASK             (0xffff << SAA9730_MDDATA_DATA_SHIFT)

#define SAA9730_MDCTL           *((volatile unsigned *)(__base + 0x2045C)) // PHY mgmt control
#  define SAA9730_MDCTL_PRESUP            (1 << 12)
#  define SAA9730_MDCTL_BUSY              (1 << 11)
#  define SAA9730_MDCTL_WRITE             (1 << 10)
#  define SAA9730_MDCTL_PHY_SHIFT         5
#  define SAA9730_MDCTL_PHY_MASK          (0x1f << SAA9730_MDCTL_PHY_SHIFT)
#  define SAA9730_MDCTL_ADDR_MASK         0x1f

#define SAA9730_CAMADR          *((volatile unsigned *)(__base + 0x20460)) // CAM address
#  define SAA9730_CAMADR_ADDRESS_MASK          (0x1ff << SAA9730_CAMADR_ADDRESS_SHIFT)

#define SAA9730_CAMDAT          *((volatile unsigned *)(__base + 0x20464)) // CAM data

#define SAA9730_CAMENA          *((volatile unsigned *)(__base + 0x20468)) // CAM enable
#  define SAA9730_CAMENA_ENABLE_MASK          (0x3fffff << SAA9730_CAMENA_ENABLE_SHIFT)

#define SAA9730_DBGRXS          *((volatile unsigned *)(__base + 0x20508)) // DEBUG
#  define SAA9730_DBGRXS_RXPI_MASK           (0x3ff << 16)
#  define SAA9730_DBGRXS_RXPI_ERROR          (0x001 << 16)
#  define SAA9730_DBGRXS_RXDII_MASK          0x1ff
#  define SAA9730_DBGRXS_RXDII_ERROR         8


// ******** Packet control/status **********

#define TXPACKET_CTL_FLAG_MASK      (0x3 << 30)
#  define TX_EMPTY                  (0 << 30)
#  define TX_READY                  (2 << 30)
#  define TX_HWDONE                 (3 << 30)

#  define TXPACKET_CTL_IRQ_MASK     (1 << 29)
#  define TXPACKET_CTL_NOCRC_MASK   (1 << 28)
#  define TXPACKET_CTL_NOPAD_MASK   (1 << 27)
#  define TXPACKET_CTL_SIZE_MASK    0x7ff

#define TXPACKET_STATUS_FLAG_MASK     (0x3 << 30)
#  define TXPACKET_STATUS_SQERR       (1 << 27)
#  define TXPACKET_STATUS_TXHALTED    (1 << 26)
#  define TXPACKET_STATUS_COMPLETION  (1 << 25)
#  define TXPACKET_STATUS_PARITYERR   (1 << 24)
#  define TXPACKET_STATUS_LATECOLLERR (1 << 23)
#  define TXPACKET_STATUS_WAS10MB     (1 << 22)
#  define TXPACKET_STATUS_LOSTCARRIER (1 << 21)
#  define TXPACKET_STATUS_EXDEFER     (1 << 20)
#  define TXPACKET_STATUS_UNDERRUN    (1 << 19)
#  define TXPACKET_STATUS_COLLISIONS_SHIFT 11
#  define TXPACKET_STATUS_COLLISIONS_MASK  (0x1f << TXPACKET_STATUS_COLLISIONS_SHIFT)
#  define TXPACKET_STATUS_SIZE_MASK   0x7ff

#  define TXPACKET_STATUS_ERROR  (TXPACKET_STATUS_EXDEFER      | \
                                  TXPACKET_STATUS_LATECOLLERR  | \
                                  TXPACKET_STATUS_LOSTCARRIER  | \
                                  TXPACKET_STATUS_UNDERRUN     | \
                                  TXPACKET_STATUS_SQERR)

#  define RXPACKET_STATUS_FLAG_MASK       (0x3 << 30)
#  define RX_NDIS                         (0 << 30)
#  define RX_INVALID_STAT                 (1 << 30)
#  define RX_READY                        (2 << 30)
#  define RX_HWDONE                       (3 << 30)

#  define RXPACKET_STATUS_GOOD        (1 << 25)
#  define RXPACKET_STATUS_PARITY      (1 << 24)
#  define RXPACKET_STATUS_LONGERR     (1 << 22)
#  define RXPACKET_STATUS_OVERFLOW    (1 << 21)
#  define RXPACKET_STATUS_CRCERR      (1 << 20)
#  define RXPACKET_STATUS_ALIGNERR    (1 << 19)
#  define RXPACKET_STATUS_WAS10MB     (1 << 18)
#  define RXPACKET_STATUS_SIZE_MASK   0x7ff

#endif  // CYGONCE_DEVS_ETH_MIPS_ATLAS_SAA9730_H
