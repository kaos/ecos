#ifndef CYGONCE_IF_FREESCALE_ENET_IO_H
#define CYGONCE_IF_FREESCALE_ENET_IO_H
//=============================================================================
//
//      if_freescale_enet_io.h
//
//      Variant specific registers
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Ilija Kocho <ilijak@siva.com.mk>
// Date:          2011-06-05
// Purpose:       Freescale ENET specific registers
// Description:
// Usage:         #include <cyg/io/if_freescale_enet_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================


//=============================================================================
// Freescale ENET Ethernet controller

// ENET Register offsets relative to   CYGADDR_IO_ETH_FREESCALE_ENET_BASE
// Note: CYGADDR_IO_ETH_FREESCALE_ENET_BASE is provided by HAL.
//       Typically you can find CYGADDR_IO_ETH_FREESCALE_ENET_BASE in var_io.h
//-----------------------------------------------------------------------------
// Interrupt Event Register
#define FREESCALE_ENET_REG_EIR                 0x004
// Interrupt Mask Register
#define FREESCALE_ENET_REG_EIMR                0x008
// Receive Descriptor Active Register
#define FREESCALE_ENET_REG_RDAR                0x010
// Transmit Descriptor Active Register
#define FREESCALE_ENET_REG_TDAR                0x014
// Ethernet Control Register
#define FREESCALE_ENET_REG_ECR                 0x024
// MII Management Frame Register
#define FREESCALE_ENET_REG_MMFR                0x040
// MII Speed Control Register
#define FREESCALE_ENET_REG_MSCR                0x044
// MIB Control Register
#define FREESCALE_ENET_REG_MIBC                0x064
// Receive Control Register
#define FREESCALE_ENET_REG_RCR                 0x084
// Transmit Control Register
#define FREESCALE_ENET_REG_TCR                 0x0C4
// Physical Address Lower Register
#define FREESCALE_ENET_REG_PALR                0x0E4
// Physical Address Upper Register
#define FREESCALE_ENET_REG_PAUR                0x0E8
// Opcode/Pause Duration Register
#define FREESCALE_ENET_REG_OPD                 0x0EC
// Descriptor Individual Upper Address Register
#define FREESCALE_ENET_REG_IAUR                0x118
// Descriptor Individual Lower Address Register
#define FREESCALE_ENET_REG_IALR                0x11C
// Descriptor Group Upper Address Register
#define FREESCALE_ENET_REG_GAUR                0x120
// Descriptor Group Lower Address Register
#define FREESCALE_ENET_REG_GALR                0x124
// Transmit FIFO Watermark Register
#define FREESCALE_ENET_REG_TFWR                0x144
// Receive Descriptor Ring Start Register
#define FREESCALE_ENET_REG_RDSR                0x180
// Transmit Buffer Descriptor Ring Start Register
#define FREESCALE_ENET_REG_TDSR                0x184
// Maximum Receive Buffer Size Register
#define FREESCALE_ENET_REG_MRBR                0x188
// Receive FIFO Section Full Threshold
#define FREESCALE_ENET_REG_RSFL                0x190
// Receive FIFO Section Empty Threshold
#define FREESCALE_ENET_REG_RSEM                0x194
// Receive FIFO Almost Empty Threshold
#define FREESCALE_ENET_REG_RAEM                0x198
// Receive FIFO Almost Full Threshold
#define FREESCALE_ENET_REG_RAFL                0x19C
// Transmit FIFO Section Empty Threshold
#define FREESCALE_ENET_REG_TSEM                0x1A0
// Transmit FIFO Almost Empty Threshold
#define FREESCALE_ENET_REG_TAEM                0x1A4
// Transmit FIFO Almost Full Threshold
#define FREESCALE_ENET_REG_TAFL                0x1A8
// Transmit Inter-Packet Gap
#define FREESCALE_ENET_REG_TIPG                0x1AC
// Frame Truncation Length
#define FREESCALE_ENET_REG_FTRL                0x1B0
// Transmit Accelerator Function Configuration
#define FREESCALE_ENET_REG_TACC                0x1C0
// Receive Accelerator Function Configuration
#define FREESCALE_ENET_REG_RACC                0x1C4
// Count of frames not counted correctly (RMON_T_DROP).
// NOTE: Counter not implemented (read 0 always) as not applicable.
#define FREESCALE_ENET_REG_RMON_T_DROP         0x200
// RMON Tx packet count (RMON_T_PACKETS)
#define FREESCALE_ENET_REG_RMON_T_PACKETS      0x204
// RMON Tx Broadcast Packets (RMON_T_BC_PKT)
#define FREESCALE_ENET_REG_RMON_T_BC_PKT       0x208
// RMON Tx Multicast Packets (RMON_T_MC_PKT)
#define FREESCALE_ENET_REG_RMON_T_MC_PKT       0x20C
// RMON Tx Packets w CRC/Align error (RMON_T_CRC_ALIGN)
#define FREESCALE_ENET_REG_RMON_T_CRC_ALIGN    0x210
// RMON Tx Packets < 64 bytes
#define FREESCALE_ENET_REG_RMON_T_UNDERSIZE    0x214
// RMON Tx Packets > MAX_FL bytes
#define FREESCALE_ENET_REG_RMON_T_OVERSIZE     0x218
// RMON Tx Packets < 64 bytes
#define FREESCALE_ENET_REG_RMON_T_FRAG         0x21C
// RMON Tx Packets > MAX_FL bytes
#define FREESCALE_ENET_REG_RMON_T_JAB          0x220
// RMON Tx collision count (RMON_T_COL)
#define FREESCALE_ENET_REG_RMON_T_COL          0x224
// RMON Tx 64 byte packets (RMON_T_P64)
#define FREESCALE_ENET_REG_RMON_T_P64          0x228
// RMON Tx 65 to 127 byte packets (RMON_T_P65TO127)
#define FREESCALE_ENET_REG_RMON_T_P65TO127     0x22C
// RMON Tx 128 to 255 byte packets (RMON_T_P128TO255)
#define FREESCALE_ENET_REG_RMON_T_P128TO255    0x230
// RMON Tx 256 to 511 byte packets (RMON_T_P256TO511)
#define FREESCALE_ENET_REG_RMON_T_P256TO511    0x234
// RMON Tx 512 to 1023 byte packets (RMON_T_P512TO1023)
#define FREESCALE_ENET_REG_RMON_T_P512TO1023   0x238
// RMON Tx 1024 to 2047 byte packets (RMON_T_P1024TO2047)
#define FREESCALE_ENET_REG_RMON_T_P1024TO2047  0x23C
// RMON Tx packets w > 2048 bytes (RMON_T_P_GTE2048)
#define FREESCALE_ENET_REG_RMON_T_P_GTE2048    0x240
// RMON Tx Octets (RMON_T_OCTETS)
#define FREESCALE_ENET_REG_RMON_T_OCTETS       0x244
// Count of frames not counted correctly (IEEE_T_DROP).
//   NOTE: Counter not implemented (read 0 always) as not applicable.
#define FREESCALE_ENET_REG_IEEE_T_DROP         0x248
// Frames Transmitted OK (IEEE_T_FRAME_OK)
#define FREESCALE_ENET_REG_IEEE_T_FRAME_OK     0x24C
// Frames Transmitted with Single Collision (IEEE_T_1COL)
#define FREESCALE_ENET_REG_IEEE_T_1COL         0x250
// Frames Transmitted with Multiple Collisions (IEEE_T_MCOL)
#define FREESCALE_ENET_REG_IEEE_T_MCOL         0x254
// Frames Transmitted after Deferral Delay (IEEE_T_DEF)
#define FREESCALE_ENET_REG_IEEE_T_DEF          0x258
// Frames Transmitted with Late Collision (IEEE_T_LCOL)
#define FREESCALE_ENET_REG_IEEE_T_LCOL         0x25C
// Frames Transmitted with Excessive Collisions (IEEE_T_EXCOL)
#define FREESCALE_ENET_REG_IEEE_T_EXCOL        0x260
// Frames Transmitted with Tx FIFO Underrun (IEEE_T_MACERR)
#define FREESCALE_ENET_REG_IEEE_T_MACERR       0x264
// Frames Transmitted with Carrier Sense Error (IEEE_T_CSERR)
#define FREESCALE_ENET_REG_IEEE_T_CSERR        0x268
// Frames Transmitted with SQE Error (IEEE_T_SQE).
//   NOTE: Counter not implemented (read 0 always)
//         as no SQE information is available.
#define FREESCALE_ENET_REG_IEEE_T_SQE          0x26C
// Flow Control Pause frames transmitted (IEEE_T_FDXFC)
#define FREESCALE_ENET_REG_IEEE_T_FDXFC        0x270
// Octet count for Frames Transmitted w/o Error (IEEE_T_OCTETS_OK).
//    NOTE: Counts total octets (includes header and FCS fields).
#define FREESCALE_ENET_REG_IEEE_T_OCTETS_OK    0x274
// RMON Rx packet count (RMON_R_PACKETS)
#define FREESCALE_ENET_REG_RMON_R_PACKETS      0x284
// RMON Rx Broadcast Packets (RMON_R_BC_PKT)
#define FREESCALE_ENET_REG_RMON_R_BC_PKT       0x288
// RMON Rx Multicast Packets (RMON_R_MC_PKT)
#define FREESCALE_ENET_REG_RMON_R_MC_PKT       0x28C
// RMON Rx Packets w CRC/Align error (RMON_R_CRC_ALIGN)
#define FREESCALE_ENET_REG_RMON_R_CRC_ALIGN    0x290
// RMON Rx Packets < 64 bytes
#define FREESCALE_ENET_REG_RMON_R_UNDERSIZE    0x294
// RMON Rx Packets > MAX_FL bytes
#define FREESCALE_ENET_REG_RMON_R_OVERSIZE     0x298
// RMON Rx Packets < 64 bytes
#define FREESCALE_ENET_REG_RMON_R_FRAG         0x29C
// RMON Rx Packets > MAX_FL bytes
#define FREESCALE_ENET_REG_RMON_R_JAB          0x2A0
// Reserved (RMON_R_RESVD_0)
#define FREESCALE_ENET_REG_RMON_R_RESVD_0      0x2A4
// RMON Rx 64 byte packets (RMON_R_P64)
#define FREESCALE_ENET_REG_RMON_R_P64          0x2A8
// RMON Rx 65 to 127 byte packets (RMON_R_P65TO127)
#define FREESCALE_ENET_REG_RMON_R_P65TO127     0x2AC
// RMON Rx 128 to 255 byte packets (RMON_R_P128TO255)
#define FREESCALE_ENET_REG_RMON_R_P128TO255    0x2B0
// RMON Rx 256 to 511 byte packets (RMON_R_P256TO511)
#define FREESCALE_ENET_REG_RMON_R_P256TO511    0x2B4
// RMON Rx 512 to 1023 byte packets (RMON_R_P512TO1023)
#define FREESCALE_ENET_REG_RMON_R_P512TO1023   0x2B8
// RMON Rx 1024 to 2047 byte packets (RMON_R_P1024TO2047)
#define FREESCALE_ENET_REG_RMON_R_P1024TO2047  0x2BC
// RMON Rx packets w > 2048 bytes (RMON_R_P_GTE2048)
#define FREESCALE_ENET_REG_RMON_R_P_GTE2048    0x2C0
// RMON Rx Octets (RMON_R_OCTETS)
#define FREESCALE_ENET_REG_RMON_R_OCTETS       0x2C4
// Count of frames not counted correctly (IEEE_R_DROP).
//    NOTE: Counter increments if a frame with valid/
//          missing SFD character is detected and has been dropped.
//          None of the other counters increments if
//          this counter increments.
#define FREESCALE_ENET_REG_RMON_R_DROP         0x2C8
// Frames Received OK (IEEE_R_FRAME_OK)
#define FREESCALE_ENET_REG_RMON_R_FRAME_OK     0x2CC
// Frames Received with CRC Error (IEEE_R_CRC)
#define FREESCALE_ENET_REG_IEEE_R_CRC          0x2D0
// Frames Received with Alignment Error (IEEE_R_ALIGN)
#define FREESCALE_ENET_REG_IEEE_R_ALIGN        0x2D4
// Receive Fifo Overflow count (IEEE_R_MACERR)
#define FREESCALE_ENET_REG_IEEE_R_MACERR       0x2D8
// Flow Control Pause frames received (IEEE_R_FDXFC)
#define FREESCALE_ENET_REG_IEEE_R_FDXFC        0x2DC
// Octet count for Frames Rcvd w/o Error (IEEE_R_OCTETS_OK).
//    Counts total octets (includes header and FCS fields).
#define FREESCALE_ENET_REG_IEEE_R_OCTETS_OK    0x2E0
// Timer Control Register
#define FREESCALE_ENET_REG_ATCR                0x400
// Timer Value Register
#define FREESCALE_ENET_REG_ATVR                0x404
// Timer Offset Register
#define FREESCALE_ENET_REG_ATOFF               0x408
// Timer Period Register
#define FREESCALE_ENET_REG_ATPER               0x40C
// Timer Correction Register
#define FREESCALE_ENET_REG_ATCOR               0x410
// Time-Stamping Clock Period Register
#define FREESCALE_ENET_REG_ATINC               0x414
// Timestamp of Last Transmitted Frame
#define FREESCALE_ENET_REG_ATSTMP              0x418
// Timer Global Status Register
#define FREESCALE_ENET_REG_TGSR                0x604

// IEEE 1588 Timers
// Timer 0 Control Status Register
#define FREESCALE_ENET_REG_TCSR0               0x608
// Timer 0Control Capture Register
#define FREESCALE_ENET_REG_TCCR0               0x60c
// Timer 1 Control Status Register
#define FREESCALE_ENET_REG_TCSR1               0x610
// Timer 1 Control Capture Register
#define FREESCALE_ENET_REG_TCCR1               0x614
// Timer 2 Control Status Register
#define FREESCALE_ENET_REG_TCSR2               0x618
// Timer 2 Control Capture Register
#define FREESCALE_ENET_REG_TCCR2               0x61C
// Timer 3 Control Status Register
#define FREESCALE_ENET_REG_TCSR3               0x620
// Timer 3 Control Capture Register
#define FREESCALE_ENET_REG_TCCR3               0x624



// ----------------------------------------------------------------------------
// ENET Register Masks

// EIR Bit Fields
#define FREESCALE_ENET_EIR_TS_TIMER_M                  0x8000
#define FREESCALE_ENET_EIR_TS_TIMER_S                  15
#define FREESCALE_ENET_EIR_TS_AVAIL_M                  0x10000
#define FREESCALE_ENET_EIR_TS_AVAIL_S                  16
#define FREESCALE_ENET_EIR_WAKEUP_M                    0x20000
#define FREESCALE_ENET_EIR_WAKEUP_S                    17
#define FREESCALE_ENET_EIR_PLR_M                       0x40000
#define FREESCALE_ENET_EIR_PLR_S                       18
#define FREESCALE_ENET_EIR_UN_M                        0x80000
#define FREESCALE_ENET_EIR_UN_S                        19
#define FREESCALE_ENET_EIR_RL_M                        0x100000
#define FREESCALE_ENET_EIR_RL_S                        20
#define FREESCALE_ENET_EIR_LC_M                        0x200000
#define FREESCALE_ENET_EIR_LC_S                        21
#define FREESCALE_ENET_EIR_EBERR_M                     0x400000
#define FREESCALE_ENET_EIR_EBERR_S                     22
#define FREESCALE_ENET_EIR_MII_M                       0x800000
#define FREESCALE_ENET_EIR_MII_S                       23
#define FREESCALE_ENET_EIR_RXB_M                       0x1000000
#define FREESCALE_ENET_EIR_RXB_S                       24
#define FREESCALE_ENET_EIR_RXF_M                       0x2000000
#define FREESCALE_ENET_EIR_RXF_S                       25
#define FREESCALE_ENET_EIR_TXB_M                       0x4000000
#define FREESCALE_ENET_EIR_TXB_S                       26
#define FREESCALE_ENET_EIR_TXF_M                       0x8000000
#define FREESCALE_ENET_EIR_TXF_S                       27
#define FREESCALE_ENET_EIR_GRA_M                       0x10000000
#define FREESCALE_ENET_EIR_GRA_S                       28
#define FREESCALE_ENET_EIR_BABT_M                      0x20000000
#define FREESCALE_ENET_EIR_BABT_S                      29
#define FREESCALE_ENET_EIR_BABR_M                      0x40000000
#define FREESCALE_ENET_EIR_BABR_S                      30
// EIMR Bit Fields
#define FREESCALE_ENET_EIMR_TS_TIMER_M                 0x8000
#define FREESCALE_ENET_EIMR_TS_TIMER_S                 15
#define FREESCALE_ENET_EIMR_TS_AVAIL_M                 0x10000
#define FREESCALE_ENET_EIMR_TS_AVAIL_S                 16
#define FREESCALE_ENET_EIMR_WAKEUP_M                   0x20000
#define FREESCALE_ENET_EIMR_WAKEUP_S                   17
#define FREESCALE_ENET_EIMR_PLR_M                      0x40000
#define FREESCALE_ENET_EIMR_PLR_S                      18
#define FREESCALE_ENET_EIMR_UN_M                       0x80000
#define FREESCALE_ENET_EIMR_UN_S                       19
#define FREESCALE_ENET_EIMR_RL_M                       0x100000
#define FREESCALE_ENET_EIMR_RL_S                       20
#define FREESCALE_ENET_EIMR_LC_M                       0x200000
#define FREESCALE_ENET_EIMR_LC_S                       21
#define FREESCALE_ENET_EIMR_EBERR_M                    0x400000
#define FREESCALE_ENET_EIMR_EBERR_S                    22
#define FREESCALE_ENET_EIMR_MII_M                      0x800000
#define FREESCALE_ENET_EIMR_MII_S                      23
#define FREESCALE_ENET_EIMR_RXB_M                      0x1000000
#define FREESCALE_ENET_EIMR_RXB_S                      24
#define FREESCALE_ENET_EIMR_RXF_M                      0x2000000
#define FREESCALE_ENET_EIMR_RXF_S                      25
#define FREESCALE_ENET_EIMR_TXB_M                      0x4000000
#define FREESCALE_ENET_EIMR_TXB_S                      26
#define FREESCALE_ENET_EIMR_TXF_M                      0x8000000
#define FREESCALE_ENET_EIMR_TXF_S                      27
#define FREESCALE_ENET_EIMR_GRA_M                      0x10000000
#define FREESCALE_ENET_EIMR_GRA_S                      28
#define FREESCALE_ENET_EIMR_BABT_M                     0x20000000
#define FREESCALE_ENET_EIMR_BABT_S                     29
#define FREESCALE_ENET_EIMR_BABR_M                     0x40000000
#define FREESCALE_ENET_EIMR_BABR_S                     30
// RDAR Bit Fields
#define FREESCALE_ENET_RDAR_RDAR_M                     0x1000000
#define FREESCALE_ENET_RDAR_RDAR_S                     24
// TDAR Bit Fields
#define FREESCALE_ENET_TDAR_TDAR_M                     0x1000000
#define FREESCALE_ENET_TDAR_TDAR_S                     24
// ECR Bit Fields
#define FREESCALE_ENET_ECR_RESET_M                     0x1
#define FREESCALE_ENET_ECR_RESET_S                     0
#define FREESCALE_ENET_ECR_ETHEREN_M                   0x2
#define FREESCALE_ENET_ECR_ETHEREN_S                   1
#define FREESCALE_ENET_ECR_MAGICEN_M                   0x4
#define FREESCALE_ENET_ECR_MAGICEN_S                   2
#define FREESCALE_ENET_ECR_SLEEP_M                     0x8
#define FREESCALE_ENET_ECR_SLEEP_S                     3
#define FREESCALE_ENET_ECR_EN1588_M                    0x10
#define FREESCALE_ENET_ECR_EN1588_S                    4
#define FREESCALE_ENET_ECR_DBGEN_M                     0x40
#define FREESCALE_ENET_ECR_DBGEN_S                     6
#define FREESCALE_ENET_ECR_STOPEN_M                    0x80
#define FREESCALE_ENET_ECR_STOPEN_S                    7
// MMFR Bit Fields
#define FREESCALE_ENET_MMFR_DATA_M                     0xFFFF
#define FREESCALE_ENET_MMFR_DATA_S                     0
#define FREESCALE_ENET_MMFR_DATA(__val)                 \
        VALUE_(FREESCALE_ENET_MMFR_DATA_S, __val)
#define FREESCALE_ENET_MMFR_TA_M                       0x30000
#define FREESCALE_ENET_MMFR_TA_S                       16
#define FREESCALE_ENET_MMFR_TA(__val)                   \
        VALUE_(FREESCALE_ENET_MMFR_TA_S, __val)
#define FREESCALE_ENET_MMFR_RA_M                       0x7C0000
#define FREESCALE_ENET_MMFR_RA_S                       18
#define FREESCALE_ENET_MMFR_RA(__val)                   \
        VALUE_(FREESCALE_ENET_MMFR_RA_S, __val)
#define FREESCALE_ENET_MMFR_PA_M                       0xF800000
#define FREESCALE_ENET_MMFR_PA_S                       23
#define FREESCALE_ENET_MMFR_PA(__val)                   \
        VALUE_(FREESCALE_ENET_MMFR_PA_S, __val)
#define FREESCALE_ENET_MMFR_OP_M                       0x30000000
#define FREESCALE_ENET_MMFR_OP_S                       28
#define FREESCALE_ENET_MMFR_OP(__val)                   \
        VALUE_(FREESCALE_ENET_MMFR_OP_S, __val)
#define FREESCALE_ENET_MMFR_ST_M                       0xC0000000
#define FREESCALE_ENET_MMFR_ST_S                       30
#define FREESCALE_ENET_MMFR_ST(__val)                   \
        VALUE_(FREESCALE_ENET_MMFR_ST_S, __val)
// MSCR Bit Fields
#define FREESCALE_ENET_MSCR_MII_SPEED_M                0x7E
#define FREESCALE_ENET_MSCR_MII_SPEED_S                1
#define FREESCALE_ENET_MSCR_MII_SPEED(__val)            \
        VALUE_(FREESCALE_ENET_MSCR_MII_SPEED_S, __val)
#define FREESCALE_ENET_MSCR_DIS_PRE_M                  0x80
#define FREESCALE_ENET_MSCR_DIS_PRE_S                  7
#define FREESCALE_ENET_MSCR_HOLDTIME_M                 0x700
#define FREESCALE_ENET_MSCR_HOLDTIME_S                 8
#define FREESCALE_ENET_MSCR_HOLDTIME(__val)             \
        VALUE_(FREESCALE_ENET_MSCR_HOLDTIME_S, __val)
// MIBC Bit Fields
#define FREESCALE_ENET_MIBC_MIB_CLEAR_M                0x20000000
#define FREESCALE_ENET_MIBC_MIB_CLEAR_S                29
#define FREESCALE_ENET_MIBC_MIB_IDLE_M                 0x40000000
#define FREESCALE_ENET_MIBC_MIB_IDLE_S                 30
#define FREESCALE_ENET_MIBC_MIB_DIS_M                  0x80000000
#define FREESCALE_ENET_MIBC_MIB_DIS_S                  31
// RCR Bit Fields
#define FREESCALE_ENET_RCR_LOOP_M                      0x1
#define FREESCALE_ENET_RCR_LOOP_S                      0
#define FREESCALE_ENET_RCR_DRT_M                       0x2
#define FREESCALE_ENET_RCR_DRT_S                       1
#define FREESCALE_ENET_RCR_MII_MODE_M                  0x4
#define FREESCALE_ENET_RCR_MII_MODE_S                  2
#define FREESCALE_ENET_RCR_PROM_M                      0x8
#define FREESCALE_ENET_RCR_PROM_S                      3
#define FREESCALE_ENET_RCR_BC_REJ_M                    0x10
#define FREESCALE_ENET_RCR_BC_REJ_S                    4
#define FREESCALE_ENET_RCR_FCE_M                       0x20
#define FREESCALE_ENET_RCR_FCE_S                       5
#define FREESCALE_ENET_RCR_RMII_MODE_M                 0x100
#define FREESCALE_ENET_RCR_RMII_MODE_S                 8
#define FREESCALE_ENET_RCR_RMII_10T_M                  0x200
#define FREESCALE_ENET_RCR_RMII_10T_S                  9
#define FREESCALE_ENET_RCR_PADEN_M                     0x1000
#define FREESCALE_ENET_RCR_PADEN_S                     12
#define FREESCALE_ENET_RCR_PAUFWD_M                    0x2000
#define FREESCALE_ENET_RCR_PAUFWD_S                    13
#define FREESCALE_ENET_RCR_CRCFWD_M                    0x4000
#define FREESCALE_ENET_RCR_CRCFWD_S                    14
#define FREESCALE_ENET_RCR_CFEN_M                      0x8000
#define FREESCALE_ENET_RCR_CFEN_S                      15
#define FREESCALE_ENET_RCR_MAX_FL_M                    0x3FFF0000
#define FREESCALE_ENET_RCR_MAX_FL_S                    16
#define FREESCALE_ENET_RCR_MAX_FL(__val)                \
        VALUE_(FREESCALE_ENET_RCR_MAX_FL_S, __val)
#define FREESCALE_ENET_RCR_NLC_M                       0x40000000
#define FREESCALE_ENET_RCR_NLC_S                       30
#define FREESCALE_ENET_RCR_GRS_M                       0x80000000
#define FREESCALE_ENET_RCR_GRS_S                       31
// TCR Bit Fields
#define FREESCALE_ENET_TCR_GTS_M                       0x1
#define FREESCALE_ENET_TCR_GTS_S                       0
#define FREESCALE_ENET_TCR_FDEN_M                      0x4
#define FREESCALE_ENET_TCR_FDEN_S                      2
#define FREESCALE_ENET_TCR_TFC_PAUSE_M                 0x8
#define FREESCALE_ENET_TCR_TFC_PAUSE_S                 3
#define FREESCALE_ENET_TCR_RFC_PAUSE_M                 0x10
#define FREESCALE_ENET_TCR_RFC_PAUSE_S                 4
#define FREESCALE_ENET_TCR_ADDSEL_M                    0xE0
#define FREESCALE_ENET_TCR_ADDSEL_S                    5
#define FREESCALE_ENET_TCR_ADDSEL(__val)                \
        VALUE_(FREESCALE_ENET_TCR_ADDSEL_S, __val)
#define FREESCALE_ENET_TCR_ADDINS_M                    0x100
#define FREESCALE_ENET_TCR_ADDINS_S                    8
#define FREESCALE_ENET_TCR_CRCFWD_M                    0x200
#define FREESCALE_ENET_TCR_CRCFWD_S                    9
// PALR Bit Fields
#define FREESCALE_ENET_PALR_PADDR1_M                   0xFFFFFFFF
#define FREESCALE_ENET_PALR_PADDR1_S                   0
#define FREESCALE_ENET_PALR_PADDR1(__val)               \
        VALUE_(FREESCALE_ENET_PALR_PADDR1_S, __val)
// PAUR Bit Fields
#define FREESCALE_ENET_PAUR_TYPE_M                     0xFFFF
#define FREESCALE_ENET_PAUR_TYPE_S                     0
#define FREESCALE_ENET_PAUR_TYPE(__val)                 \
        VALUE_(FREESCALE_ENET_PAUR_TYPE_S, __val)
#define FREESCALE_ENET_PAUR_PADDR2_M                   0xFFFF0000
#define FREESCALE_ENET_PAUR_PADDR2_S                   16
#define FREESCALE_ENET_PAUR_PADDR2(__val)               \
        VALUE_(FREESCALE_ENET_PAUR_PADDR2_S, __val)
// OPD Bit Fields
#define FREESCALE_ENET_OPD_PAUSE_DUR_M                 0xFFFF
#define FREESCALE_ENET_OPD_PAUSE_DUR_S                 0
#define FREESCALE_ENET_OPD_PAUSE_DUR(__val)             \
        VALUE_(FREESCALE_ENET_OPD_PAUSE_DUR_S, __val)
#define FREESCALE_ENET_OPD_OPCODE_M                    0xFFFF0000
#define FREESCALE_ENET_OPD_OPCODE_S                    16
#define FREESCALE_ENET_OPD_OPCODE(__val)                \
        VALUE_(FREESCALE_ENET_OPD_OPCODE_S, __val)
// IAUR Bit Fields
#define FREESCALE_ENET_IAUR_IADDR1_M                   0xFFFFFFFF
#define FREESCALE_ENET_IAUR_IADDR1_S                   0
#define FREESCALE_ENET_IAUR_IADDR1(__val)               \
        VALUE_(FREESCALE_ENET_IAUR_IADDR1_S, __val)
// IALR Bit Fields
#define FREESCALE_ENET_IALR_IADDR2_M                   0xFFFFFFFF
#define FREESCALE_ENET_IALR_IADDR2_S                   0
#define FREESCALE_ENET_IALR_IADDR2(__val)               \
        VALUE_(FREESCALE_ENET_IALR_IADDR2_S, __val)
// GAUR Bit Fields
#define FREESCALE_ENET_GAUR_GADDR1_M                   0xFFFFFFFF
#define FREESCALE_ENET_GAUR_GADDR1_S                   0
#define FREESCALE_ENET_GAUR_GADDR1(__val)               \
        VALUE_(FREESCALE_ENET_GAUR_GADDR1_S, __val)
// GALR Bit Fields
#define FREESCALE_ENET_GALR_GADDR2_M                   0xFFFFFFFF
#define FREESCALE_ENET_GALR_GADDR2_S                   0
#define FREESCALE_ENET_GALR_GADDR2(__val)               \
        VALUE_(FREESCALE_ENET_GALR_GADDR2_S, __val)
// TFWR Bit Fields
#define FREESCALE_ENET_TFWR_TFWR_M                     0x3F
#define FREESCALE_ENET_TFWR_TFWR_S                     0
#define FREESCALE_ENET_TFWR_TFWR(__val)                 \
        VALUE_(FREESCALE_ENET_TFWR_TFWR_S, __val)
#define FREESCALE_ENET_TFWR_STRFWD_M                   0x100
#define FREESCALE_ENET_TFWR_STRFWD_S                   8

#define FREESCALE_ENET_TFWR_TFWR_64                   0x0
#define FREESCALE_ENET_TFWR_TFWR_64A                  0x1
#define FREESCALE_ENET_TFWR_TFWR_128                  0x2
#define FREESCALE_ENET_TFWR_TFWR_192                  0x3
#define FREESCALE_ENET_TFWR_TFWR_4032                 0x3F
// RDSR Bit Fields
#define FREESCALE_ENET_RDSR_R_DES_START_M              0xFFFFFFF8
#define FREESCALE_ENET_RDSR_R_DES_START_S              3
#define FREESCALE_ENET_RDSR_R_DES_START(__val)          \
        VALUE_(FREESCALE_ENET_RDSR_R_DES_S, __val)
// TDSR Bit Fields
#define FREESCALE_ENET_TDSR_X_DES_START_M              0xFFFFFFF8
#define FREESCALE_ENET_TDSR_X_DES_START_S              3
#define FREESCALE_ENET_TDSR_X_DES_START(__val)          \
        VALUE_(FREESCALE_ENET_TDSR_X_DES_S, __val)
// MRBR Bit Fields
#define FREESCALE_ENET_MRBR_R_BUF_SIZE_M               0x3FF0
#define FREESCALE_ENET_MRBR_R_BUF_SIZE_S               4
#define FREESCALE_ENET_MRBR_R_BUF_SIZE(__val)           \
        VALUE_(FREESCALE_ENET_MRBR_R_BUF_S, __val)
// RSFL Bit Fields
#define FREESCALE_ENET_RSFL_RX_SECTION_FULL_M          0xFF
#define FREESCALE_ENET_RSFL_RX_SECTION_FULL_S          0
#define FREESCALE_ENET_RSFL_RX_SECTION_FULL(__val)      \
        VALUE_(FREESCALE_ENET_RSFL_RX_S, __val)
// RSEM Bit Fields
#define FREESCALE_ENET_RSEM_RX_SECTION_EMPTY_M         0xFF
#define FREESCALE_ENET_RSEM_RX_SECTION_EMPTY_S         0
#define FREESCALE_ENET_RSEM_RX_SECTION_EMPTY(__val)     \
        VALUE_(FREESCALE_ENET_RSEM_RX_S, __val)
// RAEM Bit Fields
#define FREESCALE_ENET_RAEM_RX_ALMOST_EMPTY_M          0xFF
#define FREESCALE_ENET_RAEM_RX_ALMOST_EMPTY_S          0
#define FREESCALE_ENET_RAEM_RX_ALMOST_EMPTY(__val)      \
        VALUE_(FREESCALE_ENET_RAEM_RX_ALMOST_EMPTY_S, __val)
// RAFL Bit Fields
#define FREESCALE_ENET_RAFL_RX_ALMOST_FULL_M           0xFF
#define FREESCALE_ENET_RAFL_RX_ALMOST_FULL_S           0
#define FREESCALE_ENET_RAFL_RX_ALMOST_FULL(__val)       \
        VALUE_(FREESCALE_ENET_RAFL_RX_ALMOST_FULL_S, __val)
// TSEM Bit Fields
#define FREESCALE_ENET_TSEM_TX_SECTION_EMPTY_M         0xFF
#define FREESCALE_ENET_TSEM_TX_SECTION_EMPTY_S         0
#define FREESCALE_ENET_TSEM_TX_SECTION_EMPTY(__val)     \
        VALUE_(FREESCALE_ENET_TSEM_TX_S, __val)
// TAEM Bit Fields
#define FREESCALE_ENET_TAEM_TX_ALMOST_EMPTY_M          0xFF
#define FREESCALE_ENET_TAEM_TX_ALMOST_EMPTY_S          0
#define FREESCALE_ENET_TAEM_TX_ALMOST_EMPTY(__val)      \
        VALUE_(FREESCALE_ENET_TAEM_TX_ALMOST_EMPTY_S, __val)
// TAFL Bit Fields
#define FREESCALE_ENET_TAFL_TX_ALMOST_FULL_M           0xFF
#define FREESCALE_ENET_TAFL_TX_ALMOST_FULL_S           0
#define FREESCALE_ENET_TAFL_TX_ALMOST_FULL(__val)       \
        VALUE_(FREESCALE_ENET_TAFL_TX_ALMOST_FULL_S, __val)
// TIPG Bit Fields
#define FREESCALE_ENET_TIPG_IPG_M                      0x1F
#define FREESCALE_ENET_TIPG_IPG_S                      0
#define FREESCALE_ENET_TIPG_IPG(__val)                  \
        VALUE_(FREESCALE_ENET_TIPG_IPG_S, __val)
// FTRL Bit Fields
#define FREESCALE_ENET_FTRL_TRUNC_FL_M                 0x3FFF
#define FREESCALE_ENET_FTRL_TRUNC_FL_S                 0
#define FREESCALE_ENET_FTRL_TRUNC_FL(__val)             \
        VALUE_(FREESCALE_ENET_FTRL_TRUNC_FL_S, __val)
// TACC Bit Fields
#define FREESCALE_ENET_TACC_S16_M                      0x1
#define FREESCALE_ENET_TACC_S16_S                      0
#define FREESCALE_ENET_TACC_IPCHK_M                    0x8
#define FREESCALE_ENET_TACC_IPCHK_S                    3
#define FREESCALE_ENET_TACC_PROCHK_M                   0x10
#define FREESCALE_ENET_TACC_PROCHK_S                   4
// RACC Bit Fields
#define FREESCALE_ENET_RACC_PADREM_M                   0x1
#define FREESCALE_ENET_RACC_PADREM_S                   0
#define FREESCALE_ENET_RACC_IPDIS_M                    0x2
#define FREESCALE_ENET_RACC_IPDIS_S                    1
#define FREESCALE_ENET_RACC_PRODIS_M                   0x4
#define FREESCALE_ENET_RACC_PRODIS_S                   2
#define FREESCALE_ENET_RACC_LINEDIS_M                  0x40
#define FREESCALE_ENET_RACC_LINEDIS_S                  6
#define FREESCALE_ENET_RACC_S16_M                      0x80
#define FREESCALE_ENET_RACC_S16_S                      7
// ATCR Bit Fields
#define FREESCALE_ENET_ATCR_EN_M                       0x1
#define FREESCALE_ENET_ATCR_EN_S                       0
#define FREESCALE_ENET_ATCR_OFFEN_M                    0x4
#define FREESCALE_ENET_ATCR_OFFEN_S                    2
#define FREESCALE_ENET_ATCR_OFFRST_M                   0x8
#define FREESCALE_ENET_ATCR_OFFRST_S                   3
#define FREESCALE_ENET_ATCR_PEREN_M                    0x10
#define FREESCALE_ENET_ATCR_PEREN_S                    4
#define FREESCALE_ENET_ATCR_PINPER_M                   0x80
#define FREESCALE_ENET_ATCR_PINPER_S                   7
#define FREESCALE_ENET_ATCR_RESTART_M                  0x200
#define FREESCALE_ENET_ATCR_RESTART_S                  9
#define FREESCALE_ENET_ATCR_CAPTURE_M                  0x800
#define FREESCALE_ENET_ATCR_CAPTURE_S                  11
#define FREESCALE_ENET_ATCR_SLAVE_M                    0x2000
#define FREESCALE_ENET_ATCR_SLAVE_S                    13
// ATVR Bit Fields
#define FREESCALE_ENET_ATVR_ATIME_M                    0xFFFFFFFF
#define FREESCALE_ENET_ATVR_ATIME_S                    0
#define FREESCALE_ENET_ATVR_ATIME(__val)                \
        VALUE_(FREESCALE_ENET_ATVR_ATIME_S, __val)
// ATOFF Bit Fields
#define FREESCALE_ENET_ATOFF_OFFSET_M                  0xFFFFFFFF
#define FREESCALE_ENET_ATOFF_OFFSET_S                  0
#define FREESCALE_ENET_ATOFF_OFFSET(__val)              \
        VALUE_(FREESCALE_ENET_ATOFF_OFFSET_S, __val)
// ATPER Bit Fields
#define FREESCALE_ENET_ATPER_PERIOD_M                  0xFFFFFFFF
#define FREESCALE_ENET_ATPER_PERIOD_S                  0
#define FREESCALE_ENET_ATPER_PERIOD(__val)              \
        VALUE_(FREESCALE_ENET_ATPER_PERIOD_S, __val)
// ATCOR Bit Fields
#define FREESCALE_ENET_ATCOR_COR_M                     0x7FFFFFFF
#define FREESCALE_ENET_ATCOR_COR_S                     0
#define FREESCALE_ENET_ATCOR_COR(__val)                 \
        VALUE_(FREESCALE_ENET_ATCOR_COR_S, __val)
// ATINC Bit Fields
#define FREESCALE_ENET_ATINC_INC_M                     0x7F
#define FREESCALE_ENET_ATINC_INC_S                     0
#define FREESCALE_ENET_ATINC_INC(__val)                 \
        VALUE_(FREESCALE_ENET_ATINC_INC_S, __val)
#define FREESCALE_ENET_ATINC_INC_CORR_M                0x7F00
#define FREESCALE_ENET_ATINC_INC_CORR_S                8
#define FREESCALE_ENET_ATINC_INC_CORR(__val)            \
        VALUE_(FREESCALE_ENET_ATINC_INC_CORR_S, __val)
// ATSTMP Bit Fields
#define FREESCALE_ENET_ATSTMP_TIMESTAMP_M              0xFFFFFFFF
#define FREESCALE_ENET_ATSTMP_TIMESTAMP_S              0
#define FREESCALE_ENET_ATSTMP_TIMESTAMP(__val)          \
        VALUE_(FREESCALE_ENET_ATSTMP_TIMESTAMP_S, __val)
// TGSR Bit Fields
#define FREESCALE_ENET_TGSR_TF0_M                      0x1
#define FREESCALE_ENET_TGSR_TF0_S                      0
#define FREESCALE_ENET_TGSR_TF1_M                      0x2
#define FREESCALE_ENET_TGSR_TF1_S                      1
#define FREESCALE_ENET_TGSR_TF2_M                      0x4
#define FREESCALE_ENET_TGSR_TF2_S                      2
#define FREESCALE_ENET_TGSR_TF3_M                      0x8
#define FREESCALE_ENET_TGSR_TF3_S                      3
// TCSR Bit Fields
#define FREESCALE_ENET_TCSR_TDRE_M                     0x1
#define FREESCALE_ENET_TCSR_TDRE_S                     0
#define FREESCALE_ENET_TCSR_TMODE_M                    0x3C
#define FREESCALE_ENET_TCSR_TMODE_S                    2
#define FREESCALE_ENET_TCSR_TMODE(__val)                \
        VALUE_(FREESCALE_ENET_TCSR_TMODE_S, __val)
#define FREESCALE_ENET_TCSR_TIE_M                      0x40
#define FREESCALE_ENET_TCSR_TIE_S                      6
#define FREESCALE_ENET_TCSR_TF_M                       0x80
#define FREESCALE_ENET_TCSR_TF_S                       7
// TCCR Bit Fields
#define FREESCALE_ENET_TCCR_TCC_M                      0xFFFFFFFF
#define FREESCALE_ENET_TCCR_TCC_S                      0
#define FREESCALE_ENET_TCCR_TCC(__val)                  \
        VALUE_(FREESCALE_ENET_TCCR_TCC_S, __val)

#define FREESCALE_ENET_EIR_ERROR     \
       (FREESCALE_ENET_EIR_PLR_M   | \
        FREESCALE_ENET_EIR_UN_M    | \
        FREESCALE_ENET_EIR_RL_M    | \
        FREESCALE_ENET_EIR_LC_M    | \
        FREESCALE_ENET_EIR_EBERR_M | \
        FREESCALE_ENET_EIR_BABT_M  | \
        FREESCALE_ENET_EIR_BABR_M)

// EOF if_freescale_enet_io.h
#endif // CYGONCE_IF_FREESCALE_ENET_IO_H
