//=============================================================================
//
//      proc_io.h
//
//      Details of mcf5272 memory-mapped hardware
//
//=============================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2003,2004,2006,2008 Free Software Foundation, Inc.
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   bartv
// Date:        2003-06-04
//
//####DESCRIPTIONEND####
//=============================================================================

// This header gets #include'd twice by var_io.h. The first time is to
// define the peripherals which are shared with the 5282. The second
// time is for overriding, in case of small differences between this
// processor's on-chip peripherals and the 5282's.
#if !defined(CYGONCE_HAL_PROC_IO_H_FIRST)
# define CYGONCE_HAL_PROC_IO_H_FIRST

# include <pkgconf/system.h>
# include <pkgconf/hal_m68k_mcf5272.h>
# include CYGBLD_HAL_PLATFORM_H

# define HAL_MCFxxxx_HAS_MCF5282_RAMBAR             1
# define HAL_MCFxxxx_HAS_MCF5282_VBR                1
# define HAL_MCFxxxx_HAS_MCF5282_CACR_ACR           1

# define HAL_MCFxxxx_HAS_MCF5282_UART               2
# define HAL_MCFxxxx_UART0_BASE                     (HAL_MCFxxxx_MBAR + 0x00000100)
# define HAL_MCFxxxx_UART1_BASE                     (HAL_MCFxxxx_MBAR + 0x00000140)
# define HAL_MCFxxxx_UART0_RXFIFO_SIZE              24
# define HAL_MCFxxxx_UART1_RXFIFO_SIZE              24
# define HAL_MCFxxxx_HAS_MCF5282_QSPI               1
# define HAL_MCFxxxx_QSPI0_BASE                     (HAL_MCFxxxx_MBAR + 0x000000A0)
# define HAL_MCFxxxx_QSPI_SINGLETON_BASE            HAL_MCFxxxx_QSPI0_BASE
# define HAL_MCFxxxx_QSPI_SINGLETON_ISRVEC          CYGNUM_HAL_ISR_QSPI
# define HAL_MCFxxxx_QSPI_SINGLETON_ISRPRI          CYGNUM_HAL_M68K_MCF5272_QSPI_ISRPRI
# define HAL_MCFxxxx_HAS_MCF5282_ETH                1
# define HAL_MCFxxxx_ETH0_BASE                      (HAL_MCFxxxx_MBAR + 0x0840)

// ----------------------------------------------------------------------------
#elif !defined(CYGONCE_HAL_PROC_IO_H_SECOND)
# define CYGONCE_HAL_PROC_IO_H_SECOND

// Now to handle the differences between the MCF5272 and the MCF5282
// MBAR - controls access to the system integration module

#define HAL_MCF5272_MBAR_BA_MASK                    0xFFFF0000
#define HAL_MCF5272_MBAR_BA_SHIFT                   16
#define HAL_MCF5272_MBAR_SC                         (0x01 << 4)
#define HAL_MCF5272_MBAR_SD                         (0x01 << 3)
#define HAL_MCF5272_MBAR_UC                         (0x01 << 2)
#define HAL_MCF5272_MBAR_UD                         (0x01 << 1)
#define HAL_MCF5272_MBAR_V                          (0x01 << 0)

# undef  HAL_MCFxxxx_RAMBAR
# define HAL_MCFxxxx_RAMBAR                         0x0C04
# undef  HAL_MCFxxxx_RAMBAR_PRI_MASK
# undef  HAL_MCFxxxx_RAMBAR_PRI_SHIFT
# undef  HAL_MCFxxxx_RAMBAR_PRI_CPU_CPU
# undef  HAL_MCFxxxx_RAMBAR_PRI_CPU_DMA
# undef  HAL_MCFxxxx_RAMBAR_PRI_DMA_CPU
# undef  HAL_MCFxxxx_RAMBAR_PRI_DMA_DMA
# undef  HAL_MCFxxxx_RAMBAR_SPV

# undef  HAL_MCFxxxx_CACR_DISI
# undef  HAL_MCFxxxx_CACR_DISD
# undef  HAL_MCFxxxx_CACR_INVI
# undef  HAL_MCFxxxx_CACR_INVD
# undef  HAL_MCFxxxx_CACR_EUSP

// The UARTs. Five additional registers and some extra bits.
// Baud rates can be set more precisely using the fractional
// register.
# define HAL_MCFxxxx_UARTx_UABU                     0x20
# define HAL_MCFxxxx_UARTx_UABL                     0x24
# define HAL_MCFxxxx_UARTx_UTF                      0x28
# define HAL_MCFxxxx_UARTx_URF                      0x2C
# define HAL_MCFxxxx_UARTx_UFPD                     0x30

#define HAL_MCFxxxx_UARTx_UCR_ENAB                  (0x01 << 7)

#define HAL_MCFxxxx_UARTx_UISR_ABC                  (0x01 << 6)
#define HAL_MCFxxxx_UARTx_UISR_RXFIFO               (0x01 << 5)
#define HAL_MCFxxxx_UARTx_UISR_TXFIFO               (0x01 << 4)
#define HAL_MCFxxxx_UARTx_UISR_RXFTO                (0x01 << 3)

#define HAL_MCFxxxx_UARTx_UIMR_ABC                  (0x01 << 6)
#define HAL_MCFxxxx_UARTx_UIMR_RXFIFO               (0x01 << 5)
#define HAL_MCFxxxx_UARTx_UIMR_TXFIFO               (0x01 << 4)
#define HAL_MCFxxxx_UARTx_UIMR_RXFTO                (0x01 << 3)

// Automatic RTS control depending on fifo levels
#define HAL_MCFxxxx_UARTx_UACR_RTSL_MASK            (0x03 << 1)
#define HAL_MCFxxxx_UARTx_UACR_RTSL_DISABLED        (0x00 << 1)
#define HAL_MCFxxxx_UARTx_UACR_RTSL_25              (0x01 << 1)
#define HAL_MCFxxxx_UARTx_UACR_RTSL_50              (0x02 << 1)
#define HAL_MCFxxxx_UARTx_UACR_RTSL_75              (0x03 << 1)

// Fifo control
#define HAL_MCFxxxx_UARTx_UTF_TXS_MASK              (0x03 << 6)
#define HAL_MCFxxxx_UARTx_UTF_TXS_INHIBIT           (0x00 << 6)
#define HAL_MCFxxxx_UARTx_UTF_TXS_25                (0x01 << 6)
#define HAL_MCFxxxx_UARTx_UTF_TXS_50                (0x02 << 6)
#define HAL_MCFxxxx_UARTx_UTF_TXS_75                (0x03 << 6)
#define HAL_MCFxxxx_UARTx_UTF_FULL                  (0x01 << 5)
#define HAL_MCFxxxx_UARTx_UTF_TXB_MASK              (0x1F << 0)

#define HAL_MCFxxxx_UARTx_URF_RXS_MASK              (0x03 << 6)
#define HAL_MCFxxxx_UARTx_URF_RXS_INHIBIT           (0x00 << 6)
#define HAL_MCFxxxx_UARTx_URF_RXS_25                (0x01 << 6)
#define HAL_MCFxxxx_UARTx_URF_RXS_50                (0x02 << 6)
#define HAL_MCFxxxx_UARTx_URF_RXS_75                (0x03 << 6)
#define HAL_MCFxxxx_UARTx_URF_FULL                  (0x01 << 5)
#define HAL_MCFxxxx_UARTx_URF_RXB_MASK              (0x1F << 0)

#define HAL_MCFxxxx_UARTx_UFPD_FD_MASK              (0x0F << 0)

#undef  HAL_MCFxxxx_UARTx_SET_BAUD
#define HAL_MCFxxxx_UARTx_SET_BAUD(_base_, _baud_)                                                  \
    CYG_MACRO_START                                                                                 \
    cyg_uint8   _udu_   = ((cyg_uint8) ((CYGHWR_HAL_SYSTEM_CLOCK_HZ / (32 * (_baud_))) >> 8));      \
    cyg_uint8   _udl_   = ((cyg_uint8) ((CYGHWR_HAL_SYSTEM_CLOCK_HZ / (32 * (_baud_))) & 0x00FF));  \
    cyg_uint8   _ufpd_  = ((cyg_uint8) ((CYGHWR_HAL_SYSTEM_CLOCK_HZ / (2 * (_baud_))) & 0x000F));   \
    HAL_WRITE_UINT8((_base_) + HAL_MCFxxxx_UARTx_UBG1, _udu_);                                      \
    HAL_WRITE_UINT8((_base_) + HAL_MCFxxxx_UARTx_UBG2, _udl_);                                      \
    HAL_WRITE_UINT8((_base_) + HAL_MCFxxxx_UARTx_UFPD, _ufpd_);                                     \
    CYG_MACRO_END

// QSPI. It is not clear the QWR CPTQP bits are defined. Otherwise the device
// matches the 5282's exactly.

// Ethernet. There are many differences between the 5272 and the 5282
// implementations.
# undef  HAL_MCFxxxx_ETHx_ECR
# define HAL_MCFxxxx_ETHx_ECR           0x0000
# define HAL_MCFxxxx_ETHx_IVSR          0x000C
# undef  HAL_MCFxxxx_ETHx_MIBC
# undef  HAL_MCFxxxx_ETHx_RCR
# define HAL_MCFxxxx_ETHx_RCR           0x0104
# define HAL_MCFxxxx_ETHx_MFLR          0x0108
# undef  HAL_MCFxxxx_ETHx_TCR
# define HAL_MCFxxxx_ETHx_TCR           0x0144
# undef  HAL_MCFxxxx_ETHx_PALR
# define HAL_MCFxxxx_ETHx_PALR          0x03C0
# undef  HAL_MCFxxxx_ETHx_PAUR
# define HAL_MCFxxxx_ETHx_PAUR          0x03C4
# undef  HAL_MCFxxxx_ETHx_OPD
# undef  HAL_MCFxxxx_ETHx_IAUR
# undef  HAL_MCFxxxx_ETHx_IALR
# undef  HAL_MCFxxxx_ETHx_GAUR
# define HAL_MCFxxxx_ETHx_GAUR          0x03C8
# undef  HAL_MCFxxxx_ETHx_GALR
# define HAL_MCFxxxx_ETHx_GALR          0x03CC
# define HAL_MCFxxxx_ETHx_TFSR          0x00AC
# undef  HAL_MCFxxxx_ETHx_TFWR
# define HAL_MCFxxxx_ETHx_TFWR          0x00A4
# undef  HAL_MCFxxxx_ETHx_FRBR
# define HAL_MCFxxxx_ETHx_FRBR          0x008C
# undef  HAL_MCFxxxx_ETHx_FRSR
# define HAL_MCFxxxx_ETHx_FRSR          0x0090
# undef  HAL_MCFxxxx_ETHx_ERDSR
# define HAL_MCFxxxx_ETHx_ERDSR         0x03D0
# undef  HAL_MCFxxxx_ETHx_ETDSR
# define HAL_MCFxxxx_ETHx_ETDSR         0x03D4
# undef  HAL_MCFxxxx_ETHx_EMRBR
# define HAL_MCFxxxx_ETHx_EMRBR         0x03D8

# undef  HAL_MCFxxxx_ETHx_RMON_T_DROP
# undef  HAL_MCFxxxx_ETHx_RMON_T_PACKETS
# undef  HAL_MCFxxxx_ETHx_RMON_T_BC_PKT
# undef  HAL_MCFxxxx_ETHx_RMON_T_MC_PKT
# undef  HAL_MCFxxxx_ETHx_RMON_T_CRC_ALIGN
# undef  HAL_MCFxxxx_ETHx_RMON_T_UNDERSIZE
# undef  HAL_MCFxxxx_ETHx_RMON_T_OVERSIZE
# undef  HAL_MCFxxxx_ETHx_RMON_T_FRAG
# undef  HAL_MCFxxxx_ETHx_RMON_T_JAB
# undef  HAL_MCFxxxx_ETHx_RMON_T_COL
# undef  HAL_MCFxxxx_ETHx_RMON_T_P64
# undef  HAL_MCFxxxx_ETHx_RMON_T_P65TO127
# undef  HAL_MCFxxxx_ETHx_RMON_T_P128TO255
# undef  HAL_MCFxxxx_ETHx_RMON_T_P256TO511
# undef  HAL_MCFxxxx_ETHx_RMON_T_P512TO1023
# undef  HAL_MCFxxxx_ETHx_RMON_T_P1024TO2047
# undef  HAL_MCFxxxx_ETHx_RMON_T_PGTE2048
# undef  HAL_MCFxxxx_ETHx_RMON_T_OCTETS
# undef  HAL_MCFxxxx_ETHx_IEEE_T_DROP
# undef  HAL_MCFxxxx_ETHx_IEEE_T_FRAME_OK
# undef  HAL_MCFxxxx_ETHx_IEEE_T_1COL
# undef  HAL_MCFxxxx_ETHx_IEEE_T_MCOL
# undef  HAL_MCFxxxx_ETHx_IEEE_T_DEF
# undef  HAL_MCFxxxx_ETHx_IEEE_T_LCOL
# undef  HAL_MCFxxxx_ETHx_IEEE_T_EXCOL
# undef  HAL_MCFxxxx_ETHx_IEEE_T_MACERR
# undef  HAL_MCFxxxx_ETHx_IEEE_T_CSERR
# undef  HAL_MCFxxxx_ETHx_IEEE_T_SQE
# undef  HAL_MCFxxxx_ETHx_IEEE_T_FDXFC
# undef  HAL_MCFxxxx_ETHx_IEEE_T_OCTETS_OK
# undef  HAL_MCFxxxx_ETHx_RMON_R_PACKETS
# undef  HAL_MCFxxxx_ETHx_RMON_R_BC_PKT
# undef  HAL_MCFxxxx_ETHx_RMON_R_MC_PKT
# undef  HAL_MCFxxxx_ETHx_RMON_R_CRC_ALIGN
# undef  HAL_MCFxxxx_ETHx_RMON_R_UNDERSIZE
# undef  HAL_MCFxxxx_ETHx_RMON_R_OVERSIZE
# undef  HAL_MCFxxxx_ETHx_RMON_R_FRAG
# undef  HAL_MCFxxxx_ETHx_RMON_R_JAB
# undef  HAL_MCFxxxx_ETHx_RMON_R_RESVD_0
# undef  HAL_MCFxxxx_ETHx_RMON_R_P64
# undef  HAL_MCFxxxx_ETHx_RMON_R_P65TO127
# undef  HAL_MCFxxxx_ETHx_RMON_R_P128TO255
# undef  HAL_MCFxxxx_ETHx_RMON_R_P256TO511
# undef  HAL_MCFxxxx_ETHx_RMON_R_P512TO1023
# undef  HAL_MCFxxxx_ETHx_RMON_R_P1024TO2047
# undef  HAL_MCFxxxx_ETHx_RMON_R_GTE2048
# undef  HAL_MCFxxxx_ETHx_RMON_R_OCTETS
# undef  HAL_MCFxxxx_ETHx_IEEE_R_DROP
# undef  HAL_MCFxxxx_ETHx_IEEE_R_FRAME_OK
# undef  HAL_MCFxxxx_ETHx_IEEE_R_CRC
# undef  HAL_MCFxxxx_ETHx_IEEE_R_ALIGN
# undef  HAL_MCFxxxx_ETHx_IEEE_R_MACERR
# undef  HAL_MCFxxxx_ETHx_IEEE_R_FDXFC
# undef  HAL_MCFxxxx_ETHx_IEEE_R_OCTETS_OK

# undef  HAL_MCFxxxx_ETHx_EIR_LC
# undef  HAL_MCFxxxx_ETHx_EIR_RL
# undef  HAL_MCFxxxx_ETHx_EIR_UN
# define HAL_MCFxxxx_ETHx_EIR_UMINT     (0x01 << 21)

# undef  HAL_MCFxxxx_ETHx_EIMR_LC
# undef  HAL_MCFxxxx_ETHx_EIMR_RL
# undef  HAL_MCFxxxx_ETHx_EIMR_UN

# undef  HAL_MCFxxxx_ETHx_MIBC_MIB_DISABLE
# undef  HAL_MCFxxxx_ETHx_MIBC_MIB_IDLE

# undef  HAL_MCFxxxx_ETHx_RCR_MAX_FL_MASK
# undef  HAL_MCFxxxx_ETHx_RCR_MAX_FL_SHIFT
# undef  HAL_MCFxxxx_ETHx_RCR_MAX_FL_VALUE
# undef  HAL_MCFxxxx_ETHx_RCR_FCE
# undef  HAL_MCFxxxx_ETHx_RCR_BC_REJ

# undef  HAL_MCFxxxx_ETHx_TCR_RFC_PAUSE
# undef  HAL_MCFxxxx_ETHx_TCR_TFC_PAUSE

# undef  HAL_MCFxxxx_ETHx_OPD_OPCODE_MASK
# undef  HAL_MCFxxxx_ETHx_OPD_OPCODE_SHIFT
# undef  HAL_MCFxxxx_ETHx_OPD_PAUSE_DUR_MASK
# undef  HAL_MCFxxxx_ETHx_OPD_PAUSE_DUR_SHIFT

# define HAL_MCFxxxx_ETHx_RXBD_SH        (0x01 << 3)

# undef  HAL_MCFxxxx_ETHx_TXBD_ABC
# define HAL_MCFxxxx_ETHx_TXBD_DEF       (0x01 << 9)
# define HAL_MCFxxxx_ETHx_TXBD_HB        (0x01 << 8)
# define HAL_MCFxxxx_ETHx_TXBD_LC        (0x01 << 7)
# define HAL_MCFxxxx_ETHx_TXBD_RL        (0x01 << 6)
# define HAL_MCFxxxx_ETHx_TXBD_RC_MASK   (0x0F << 2)
# define HAL_MCFxxxx_ETHx_TXBD_RC_SHIFT  2
# define HAL_MCFxxxx_ETHx_TXBD_UN        (0x01 << 1)
# define HAL_MCFxxxx_ETHx_TXBD_CSL       (0x01 << 0)

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------
// Next the units specific to the 5272

// The ROM base address register. The ROM size depends on the particular
// processor, and that controls how many of bits 9 to 31 are significant.
#define HAL_MCF5272_ROMBAR_WP               (0x01 << 8)
// Mask cpu space and interrupt acknowledge accesses
#define HAL_MCF5272_ROMBAR_AS5              (0x01 << 5)
// Mask supervisor code accesses
#define HAL_MCF5272_ROMBAR_AS4              (0x01 << 4)
// Mask supervisor data accesses
#define HAL_MCF5272_ROMBAR_AS3              (0x01 << 3)
// Mask user code accesses
#define HAL_MCF5272_ROMBAR_AS2              (0x01 << 2)
// Mask user data accesses
#define HAL_MCF5272_ROMBAR_AS1              (0x01 << 1)
// Has the ROMBAR register been initialized yet?
#define HAL_MCF5272_ROMBAR_VALID            (0x01 << 0)

// ----------------------------------------------------------------------------
// System registers in the system integration module. PMR and DIR are
// 32-bit. SCR, SPR, and ALPR are 16-bit.

#define HAL_MCF5272_SCR                             0x0004
#define HAL_MCF5272_SPR                             0x0006
#define HAL_MCF5272_PMR                             0x0008
#define HAL_MCF5272_ALPR                            0x000E
#define HAL_MCF5272_DIR                             0x0010

// System configuration register
#define HAL_MCF5272_SCR_RSTSRC_MASK                 (0x03 << 12)
#define HAL_MCF5272_SCR_RSTSRC_RSTI                 (0x01 << 12)
#define HAL_MCF5272_SCR_RSTSRC_WATCHDOG             (0x02 << 12)
#define HAL_MCF5272_SCR_RSTSRC_RSTI_DRESETEN        (0x03 << 12)
#define HAL_MCF5272_SCR_PRIORITY_MASK               (0x01 << 8)
#define HAL_MCF5272_SCR_PRIORITY_ETHERNET           (0x00 << 8)
#define HAL_MCF5272_SCR_PRIORITY_CPU                (0x01 << 8)
#define HAL_MCF5272_SCR_AR_MASK                     (0x01 << 7)
#define HAL_MCF5272_SCR_AR_RELINQUISH               (0x00 << 7)
#define HAL_MCF5272_SCR_AR_INCLUDE                  (0x01 << 7)
#define HAL_MCF5272_SCR_AR_SOFTRST                  (0x01 << 6)
#define HAL_MCF5272_SCR_BUSLOCK_MASK                (0x01 << 3)
#define HAL_MCF5272_SCR_BUSLOCK_ARBITRATION         (0x00 << 3)
#define HAL_MCF5272_SCR_BUSLOCK_RETAIN              (0x01 << 3)
#define HAL_MCF5272_SCR_HWR_MASK                    (0x07 << 0)
#define HAL_MCF5272_SCR_HWR_128                     (0x00 << 0)
#define HAL_MCF5272_SCR_HWR_256                     (0x01 << 0)
#define HAL_MCF5272_SCR_HWR_512                     (0x02 << 0)
#define HAL_MCF5272_SCR_HWR_1024                    (0x03 << 0)
#define HAL_MCF5272_SCR_HWR_2048                    (0x04 << 0)
#define HAL_MCF5272_SCR_HWR_4096                    (0x05 << 0)
#define HAL_MCF5272_SCR_HWR_8192                    (0x06 << 0)
#define HAL_MCF5272_SCR_HWR_16384                   (0x07 << 0)

// System protection register
#define HAL_MCF5272_SPR_ADC                         (0x01 << 15)
#define HAL_MCF5272_SPR_WPV                         (0x01 << 14)
#define HAL_MCF5272_SPR_SMV                         (0x01 << 13)
#define HAL_MCF5272_SPR_PE                          (0x01 << 12)
#define HAL_MCF5272_SPR_HWT                         (0x01 << 11)
#define HAL_MCF5272_SPR_RPV                         (0x01 << 10)
#define HAL_MCF5272_SPR_EXT                         (0x01 << 9)
#define HAL_MCF5272_SPR_SUV                         (0x01 << 8)
#define HAL_MCF5272_SPR_ADCEN                       (0x01 << 7)
#define HAL_MCF5272_SPR_WPVEN                       (0x01 << 6)
#define HAL_MCF5272_SPR_SMVEN                       (0x01 << 5)
#define HAL_MCF5272_SPR_PEEN                        (0x01 << 4)
#define HAL_MCF5272_SPR_HWTEN                       (0x01 << 3)
#define HAL_MCF5272_SPR_RPVEN                       (0x01 << 2)
#define HAL_MCF5272_SPR_EXTEN                       (0x01 << 1)
#define HAL_MCF5272_SPR_SUVEN                       (0x01 << 0)

// Power management register. For power-down, 0 is enabled, 1 disabled.
// For wakeup 0 is disabled, 1 enabled
#define HAL_MCF5272_PMR_BDMPDN                      (0x01 << 31)
#define HAL_MCF5272_PMR_ENETPDN                     (0x01 << 26)
#define HAL_MCF5272_PMR_PLIPDN                      (0x01 << 25)
#define HAL_MCF5272_PMR_DRAMPDN                     (0x01 << 24)
#define HAL_MCF5272_PMR_DMAPDN                      (0x01 << 23)
#define HAL_MCF5272_PMR_PWMPDN                      (0x01 << 22)
#define HAL_MCF5272_PMR_QSPIPDN                     (0x01 << 21)
#define HAL_MCF5272_PMR_TIMERPDN                    (0x01 << 20)
#define HAL_MCF5272_PMR_GPIOPDN                     (0x01 << 19)
#define HAL_MCF5272_PMR_USBPDN                      (0x01 << 18)
#define HAL_MCF5272_PMR_UART1PDN                    (0x01 << 17)
#define HAL_MCF5272_PMR_UART0PDN                    (0x01 << 16)
#define HAL_MCF5272_PMR_USBWK                       (0x01 << 10)
#define HAL_MCF5272_PMR_UART1WK                     (0x01 << 9)
#define HAL_MCF5272_PMR_UART0WK                     (0x01 << 8)
#define HAL_MCF5272_PMR_MOS                         (0x01 << 5)
#define HAL_MCF5272_PMR_SLPEN                       (0x01 << 4)

// Device identification register
#define HAL_MCF5272_DIR_VERSION_MASK                (0x0F << 28)
#define HAL_MCF5272_DIR_VERSION_SHIFT               28
#define HAL_MCF5272_DIR_DESIGN_CENTER_MASK          (0x3F << 22)
#define HAL_MCF5272_DIR_DESIGN_CENTER_SHIFT         22
#define HAL_MCF5272_DIR_DEVICE_NUMBER_MASK          (0x3FF << 12)
#define HAL_MCF5272_DIR_DEVICE_NUMBER_SHIFT         12
#define HAL_MCF5272_DIR_JEDEC_ID_MASK               (0x7FF << 1)
#define HAL_MCF5272_DIR_JEDEC_ID_SHIFT              1

// ----------------------------------------------------------------------------
// The software watchdog. All registers are 2-bytes wide.

#define HAL_MCF5272_WRRR                            0x0280
#define HAL_MCF5272_WIRR                            0x0284
#define HAL_MCF5272_WCR                             0x0288
#define HAL_MCF5272_WER                             0x028C

// ----------------------------------------------------------------------------
// The interrupt controller. PIVR is a single byte, the others are 32-bit.
// The bits within the registers are defined by the ISR numbers in proc_intr.h
#define HAL_MCF5272_ICR1                            0x0020
#define HAL_MCF5272_ICR2                            0x0024
#define HAL_MCF5272_ICR3                            0x0028
#define HAL_MCF5272_ICR4                            0x002C
#define HAL_MCF5272_ISR                             0x0030
#define HAL_MCF5272_PITR                            0x0034
#define HAL_MCF5272_PIWR                            0x0038
#define HAL_MCF5272_PIVR                            0x003F

// ----------------------------------------------------------------------------
// Chip select module. All registers are 32-bit
#define HAL_MCF5272_CSBR0                           0x0040
#define HAL_MCF5272_CSOR0                           0x0044
#define HAL_MCF5272_CSBR1                           0x0048
#define HAL_MCF5272_CSOR1                           0x004C
#define HAL_MCF5272_CSBR2                           0x0050
#define HAL_MCF5272_CSOR2                           0x0054
#define HAL_MCF5272_CSBR3                           0x0058
#define HAL_MCF5272_CSOR3                           0x005C
#define HAL_MCF5272_CSBR4                           0x0060
#define HAL_MCF5272_CSOR4                           0x0064
#define HAL_MCF5272_CSBR5                           0x0068
#define HAL_MCF5272_CSOR5                           0x006C
#define HAL_MCF5272_CSBR6                           0x0070
#define HAL_MCF5272_CSOR6                           0x0074
#define HAL_MCF5272_CSBR7                           0x0078
#define HAL_MCF5272_CSOR7                           0x007C

#define HAL_MCF5272_CSBR_BA_MASK                    (0xFFFFF000)
#define HAL_MCF5272_CSBR_EBI_MASK                   (0x03 << 10)
#define HAL_MCF5272_CSBR_EBI_16_32                  (0x00 << 10)
#define HAL_MCF5272_CSBR_EBI_SDRAM                  (0x01 << 10)
#define HAL_MCF5272_CSBR_EBI_8                      (0x03 << 10)
#define HAL_MCF5272_CSBR_BW_MASK                    (0x03 << 8)
#define HAL_MCF5272_CSBR_BW_32                      (0x00 << 8)
#define HAL_MCF5272_CSBR_BW_8                       (0x01 << 8)
#define HAL_MCF5272_CSBR_BW_16                      (0x02 << 8)
#define HAL_MCF5272_CSBR_BW_CACHELINE               (0x03 << 8)
#define HAL_MCF5272_CSBR_SUPER                      (0x01 << 7)
#define HAL_MCF5272_CSBR_TT_MASK                    (0x03 << 5)
#define HAL_MCF5272_CSBR_TM_MASK                    (0x07 << 2)
#define HAL_MCF5272_CSBR_CTM                        (0x01 << 1)
#define HAL_MCF5272_CSBR_ENABLE                     (0x01 << 0)

#define HAL_MCF5272_CSOR_BA_MASK                    (0xFFFFF000)
#define HAL_MCF5272_CSOR_ASET                       (0x01 << 11)
#define HAL_MCF5272_CSOR_WRAH                       (0x01 << 10)
#define HAL_MCF5272_CSOR_RDAH                       (0x01 << 9)
#define HAL_MCF5272_CSOR_EXTBURST                   (0x01 << 8)
#define HAL_MCF5272_CSOR_WS_MASK                    (0x1F << 2)
#define HAL_MCF5272_CSOR_WS_SHIFT                   2
#define HAL_MCF5272_CSOR_RW                         (0x01 << 1)
#define HAL_MCF5272_CSOR_MRW                        (0x01 << 0)

// ----------------------------------------------------------------------------
// SDRAM controller. Two 16-bit registers.

#define HAL_MCF5272_SDCR                            0x0180
#define HAL_MCF5272_SDTR                            0x0184

#define HAL_MCF5272_SDCR_MCAS_MASK                  (0x03 << 13)
#define HAL_MCF5272_SDCR_MCAS_A7                    (0x00 << 13)
#define HAL_MCF5272_SDCR_MCAS_A8                    (0x01 << 13)
#define HAL_MCF5272_SDCR_MCAS_A9                    (0x02 << 13)
#define HAL_MCF5272_SDCR_MCAS_A10                   (0x03 << 13)
#define HAL_MCF5272_SDCR_BALOC_MASK                 (0x07 << 8)
#define HAL_MCF5272_SDCR_BALOC_A21_A20              (0x01 << 8)
#define HAL_MCF5272_SDCR_BALOC_A22_A21              (0x02 << 8)
#define HAL_MCF5272_SDCR_BALOC_A23_A22              (0x03 << 8)
#define HAL_MCF5272_SDCR_BALOC_A24_A23              (0x04 << 8)
#define HAL_MCF5272_SDCR_BALOC_A25_A24              (0x05 << 8)
#define HAL_MCF5272_SDCR_GSL                        (0x01 << 7)
#define HAL_MCF5272_SDCR_REG                        (0x01 << 4)
#define HAL_MCF5272_SDCR_INV                        (0x01 << 3)
#define HAL_MCF5272_SDCR_SLEEP                      (0x01 << 2)
#define HAL_MCF5272_SDCR_ACT                        (0x01 << 1)
#define HAL_MCF5272_SDCR_INIT                       (0x01 << 0)

#define HAL_MCF5272_SDTR_RTP_MASK                   (0x3F << 10)
#define HAL_MCF5272_SDTR_RTP_66                     (0x3D << 10)
#define HAL_MCF5272_SDTR_RTP_48                     (0x2B << 10)
#define HAL_MCF5272_SDTR_RTP_33                     (0x1D << 10)
#define HAL_MCF5272_SDTR_RTP_25                     (0x15 << 10)
#define HAL_MCF5272_SDTR_RTP_5                      (0x04 << 10)
#define HAL_MCF5272_SDTR_RC_MASK                    (0x03 << 8)
#define HAL_MCF5272_SDTR_RC_5                       (0x00 << 8)
#define HAL_MCF5272_SDTR_RC_6                       (0x01 << 8)
#define HAL_MCF5272_SDTR_RC_7                       (0x02 << 8)
#define HAL_MCF5272_SDTR_RC_8                       (0x03 << 8)
#define HAL_MCF5272_SDTR_RP_MASK                    (0x03 << 4)
#define HAL_MCF5272_SDTR_RP_1                       (0x00 << 4)
#define HAL_MCF5272_SDTR_RP_2                       (0x01 << 4)
#define HAL_MCF5272_SDTR_RP_3                       (0x02 << 4)
#define HAL_MCF5272_SDTR_RP_4                       (0x03 << 4)
#define HAL_MCF5272_SDTR_RCD_MASK                   (0x03 << 2)
#define HAL_MCF5272_SDTR_RCD_1                      (0x00 << 2)
#define HAL_MCF5272_SDTR_RCD_2                      (0x01 << 2)
#define HAL_MCF5272_SDTR_RCD_3                      (0x02 << 2)
#define HAL_MCF5272_SDTR_RCD_4                      (0x03 << 2)
#define HAL_MCF5272_SDTR_CLT_MASK                   (0x03 << 0)
#define HAL_MCF5272_SDTR_CLT_2                      (0x01 << 0)

// ----------------------------------------------------------------------------
// DMA controller. DCIR is 16-bit, the rest 32-bit
#define HAL_MCF5272_DMA_DCMR                        0x00E0
#define HAL_MCF5272_DMA_DCIR                        0x00E6
#define HAL_MCF5272_DMA_DBCR                        0x00E8
#define HAL_MCF5272_DMA_DSAR                        0x00EC
#define HAL_MCF5272_DMA_DDAR                        0x00F0

#define HAL_MCF5272_DMA_DCMR_RESET                  (0x01 << 31)
#define HAL_MCF5272_DMA_DCMR_EN                     (0x01 << 30)
#define HAL_MCF5272_DMA_DCMR_RQM_MASK               (0x03 << 18)
#define HAL_MCF5272_DMA_DCMR_RQM_DUAL               (0x03 << 18)
#define HAL_MCF5272_DMA_DCMR_DSTM_MASK              (0x03 << 13)
#define HAL_MCF5272_DMA_DCMR_DSTM_STATIC            (0x00 << 13)
#define HAL_MCF5272_DMA_DCMR_DSTM_INCREMENT         (0x01 << 13)
#define HAL_MCF5272_DMA_DCMR_DSTT_MASK              (0x07 << 10)
#define HAL_MCF5272_DMA_DCMR_DSTT_UDA               (0x01 << 10)
#define HAL_MCF5272_DMA_DCMR_DSTT_UCA               (0x02 << 10)
#define HAL_MCF5272_DMA_DCMR_DSTT_SDA               (0x05 << 10)
#define HAL_MCF5272_DMA_DCMR_DSTT_SCA               (0x06 << 10)
#define HAL_MCF5272_DMA_DCMR_DSTS_MASK              (0x03 << 8)
#define HAL_MCF5272_DMA_DCMR_DSTS_4                 (0x00 << 8)
#define HAL_MCF5272_DMA_DCMR_DSTS_1                 (0x01 << 8)
#define HAL_MCF5272_DMA_DCMR_DSTS_2                 (0x02 << 8)
#define HAL_MCF5272_DMA_DCMR_DSTS_16                (0x03 << 8)
#define HAL_MCF5272_DMA_DCMR_SRCM_MASK              (0x01 << 5)
#define HAL_MCF5272_DMA_DCMR_SRCM_STATIC            (0x00 << 5)
#define HAL_MCF5272_DMA_DCMR_SRCM_INCREMENT         (0x01 << 5)
#define HAL_MCF5272_DMA_DCMR_SRCT_MASK              (0x07 << 2)
#define HAL_MCF5272_DMA_DCMR_SRCT_UDA               (0x01 << 2)
#define HAL_MCF5272_DMA_DCMR_SRCT_UCA               (0x02 << 2)
#define HAL_MCF5272_DMA_DCMR_SRCT_SDA               (0x05 << 2)
#define HAL_MCF5272_DMA_DCMR_SRCT_SCA               (0x06 << 2)
#define HAL_MCF5272_DMA_DCMR_SRCS_MASK              (0x03 << 0)
#define HAL_MCF5272_DMA_DCMR_SRCS_4                 (0x00 << 0)
#define HAL_MCF5272_DMA_DCMR_SRCS_1                 (0x01 << 0)
#define HAL_MCF5272_DMA_DCMR_SRCS_2                 (0x02 << 0)
#define HAL_MCF5272_DMA_DCMR_SRCS_16                (0x03 << 0)

#define HAL_MCF5272_DMA_DIR_INVEN                   (0x01 << 12)
#define HAL_MCF5272_DMA_DIR_ASCEN                   (0x01 << 11)
#define HAL_MCF5272_DMA_DIR_TEEN                    (0x01 << 9)
#define HAL_MCF5272_DMA_DIR_TCEN                    (0x01 << 8)
#define HAL_MCF5272_DMA_DIR_INV                     (0x01 << 4)
#define HAL_MCF5272_DMA_DIR_ASC                     (0x01 << 3)
#define HAL_MCF5272_DMA_DIR_TE                      (0x01 << 1)
#define HAL_MCF5272_DMA_DIR_TC                      (0x01 << 0)

// ----------------------------------------------------------------------------
// USB
#define HAL_MCF5272_USB_FNR                         0x1002
#define HAL_MCF5272_USB_FNMR                        0x1006
#define HAL_MCF5272_USB_RFMR                        0x100A
#define HAL_MCF5272_USB_RFMMR                       0x100E
#define HAL_MCF5272_USB_FAR                         0x1013
#define HAL_MCF5272_USB_ASR                         0x1014
#define HAL_MCF5272_USB_DDR1                        0x1018
#define HAL_MCF5272_USB_DDR2                        0x101C
#define HAL_MCF5272_USB_SPECR                       0x1022
#define HAL_MCF5272_USB_EP0SR                       0x1026
#define HAL_MCF5272_USB_IEP0CFG                     0x1028
#define HAL_MCF5272_USB_OEP0CFG                     0x102C
#define HAL_MCF5272_USB_EP1CFG                      0x1030
#define HAL_MCF5272_USB_EP2CFG                      0x1034
#define HAL_MCF5272_USB_EP3CFG                      0x1038
#define HAL_MCF5272_USB_EP4CFG                      0x103C
#define HAL_MCF5272_USB_EP5CFG                      0x1040
#define HAL_MCF5272_USB_EP6CFG                      0x1044
#define HAL_MCF5272_USB_EP7CFG                      0x1048
#define HAL_MCF5272_USB_EP0CTL                      0x104C
#define HAL_MCF5272_USB_EP1CTL                      0x1052
#define HAL_MCF5272_USB_EP2CTL                      0x1056
#define HAL_MCF5272_USB_EP3CTL                      0x105A
#define HAL_MCF5272_USB_EP4CTL                      0x105E
#define HAL_MCF5272_USB_EP5CTL                      0x1062
#define HAL_MCF5272_USB_EP6CTL                      0x1066
#define HAL_MCF5272_USB_EP7CTL                      0x106A
#define HAL_MCF5272_USB_EP0ISR                      0x106C
#define HAL_MCF5272_USB_EP1ISR                      0x1072
#define HAL_MCF5272_USB_EP2ISR                      0x1076
#define HAL_MCF5272_USB_EP3ISR                      0x107A
#define HAL_MCF5272_USB_EP4ISR                      0x107E
#define HAL_MCF5272_USB_EP5ISR                      0x1082
#define HAL_MCF5272_USB_EP6ISR                      0x1086
#define HAL_MCF5272_USB_EP7ISR                      0x108A
#define HAL_MCF5272_USB_EP0IMR                      0x108C
#define HAL_MCF5272_USB_EP1IMR                      0x1092
#define HAL_MCF5272_USB_EP2IMR                      0x1096
#define HAL_MCF5272_USB_EP3IMR                      0x109A
#define HAL_MCF5272_USB_EP4IMR                      0x109E
#define HAL_MCF5272_USB_EP5IMR                      0x10A2
#define HAL_MCF5272_USB_EP6IMR                      0x10A6
#define HAL_MCF5272_USB_EP7IMR                      0x10AA
#define HAL_MCF5272_USB_EP0DR                       0x10AC
#define HAL_MCF5272_USB_EP1DR                       0x10B0
#define HAL_MCF5272_USB_EP2DR                       0x10B4
#define HAL_MCF5272_USB_EP3DR                       0x10B8
#define HAL_MCF5272_USB_EP4DR                       0x10BC
#define HAL_MCF5272_USB_EP5DR                       0x10C0
#define HAL_MCF5272_USB_EP6DR                       0x10C4
#define HAL_MCF5272_USB_EP7DR                       0x10C8
#define HAL_MCF5272_USB_EP0DPR                      0x10CE
#define HAL_MCF5272_USB_EP1DPR                      0x10D2
#define HAL_MCF5272_USB_EP2DPR                      0x10D6
#define HAL_MCF5272_USB_EP3DPR                      0x10DA
#define HAL_MCF5272_USB_EP4DPR                      0x10DE
#define HAL_MCF5272_USB_EP5DPR                      0x10E2
#define HAL_MCF5272_USB_EP6DPR                      0x10E6
#define HAL_MCF5272_USB_EP7DPR                      0x10EA
#define HAL_MCF5272_USB_CONFIG                      0x1400

#define HAL_MCF5272_USB_ASR_IF15_MASK               (0x03 << 30)
#define HAL_MCF5272_USB_ASR_IF15_SHIFT              30
#define HAL_MCF5272_USB_ASR_IF14_MASK               (0x03 << 28)
#define HAL_MCF5272_USB_ASR_IF14_SHIFT              28
#define HAL_MCF5272_USB_ASR_IF13_MASK               (0x03 << 26)
#define HAL_MCF5272_USB_ASR_IF13_SHIFT              26
#define HAL_MCF5272_USB_ASR_IF12_MASK               (0x03 << 24)
#define HAL_MCF5272_USB_ASR_IF12_SHIFT              24
#define HAL_MCF5272_USB_ASR_IF11_MASK               (0x03 << 22)
#define HAL_MCF5272_USB_ASR_IF11_SHIFT              22
#define HAL_MCF5272_USB_ASR_IF10_MASK               (0x03 << 20)
#define HAL_MCF5272_USB_ASR_IF10_SHIFT              20
#define HAL_MCF5272_USB_ASR_IF9_MASK                (0x03 << 18)
#define HAL_MCF5272_USB_ASR_IF9_SHIFT               18
#define HAL_MCF5272_USB_ASR_IF8_MASK                (0x03 << 16)
#define HAL_MCF5272_USB_ASR_IF8_SHIFT               16
#define HAL_MCF5272_USB_ASR_IF7_MASK                (0x03 << 14)
#define HAL_MCF5272_USB_ASR_IF7_SHIFT               14
#define HAL_MCF5272_USB_ASR_IF6_MASK                (0x03 << 12)
#define HAL_MCF5272_USB_ASR_IF6_SHIFT               12
#define HAL_MCF5272_USB_ASR_IF5_MASK                (0x03 << 10)
#define HAL_MCF5272_USB_ASR_IF5_SHIFT               10
#define HAL_MCF5272_USB_ASR_IF4_MASK                (0x03 << 8)
#define HAL_MCF5272_USB_ASR_IF4_SHIFT               8
#define HAL_MCF5272_USB_ASR_IF3_MASK                (0x03 << 6)
#define HAL_MCF5272_USB_ASR_IF3_SHIFT               6
#define HAL_MCF5272_USB_ASR_IF2_MASK                (0x03 << 4)
#define HAL_MCF5272_USB_ASR_IF2_SHIFT               4
#define HAL_MCF5272_USB_ASR_IF1_MASK                (0x03 << 2)
#define HAL_MCF5272_USB_ASR_IF1_SHIFT               2
#define HAL_MCF5272_USB_ASR_IF0_MASK                (0x03 << 0)
#define HAL_MCF5272_USB_ASR_IF0_SHIFT               0

#define HAL_MCF5272_USB_DDR1_wValue_MASK            (0x0FFFF << 16)
#define HAL_MCF5272_USB_DDR1_wValue_SHIFT           16
#define HAL_MCF5272_USB_DDR1_bRequest_MASK          (0x0FF << 8)
#define HAL_MCF5272_USB_DDR1_bRequest_SHIFT         8
#define HAL_MCF5272_USB_DDR1_bmRequestType_MASK     (0x0FF << 0)
#define HAL_MCF5272_USB_DDR1_bmRequestType_SHIFT    0
#define HAL_MCF5272_USB_DDR2_wLength_MASK           (0x0FFFF << 16)
#define HAL_MCF5272_USB_DDR2_wLength_SHIFT          16
#define HAL_MCF5272_USB_DDR2_wIndex_MASK            (0x0FFFF << 0)
#define HAL_MCF5272_USB_DDR2_wIndex_SHIFT           0

#define HAL_MCF5272_USB_SPECR_SPEC_MASK             (0x0FFF0 << 4)
#define HAL_MCF5272_USB_SPECR_SPEC_SHIFT            4
#define HAL_MCF5272_USB_SPECR_MRN_MASK              (0x0F << 0)
#define HAL_MCF5272_USB_SPECR_MRN_SHIFT             (0x0F << 0)

#define HAL_MCF5272_USB_EP0SR_CONFIG_MASK           (0x0F << 12)
#define HAL_MCF5272_USB_EP0SR_CONFIG_SHIFT          12
#define HAL_MCF5272_USB_EP0SR_WAKE_ST               (0x01 << 11)
#define HAL_MCF5272_USB_EP0SR_HALT_ST               (0x01 << 2)
#define HAL_MCF5272_USB_EP0SR_DIR_MASK              (0x01 << 1)
#define HAL_MCF5272_USB_EP0SR_DIR_IN                (0x01 << 1)
#define HAL_MCF5272_USB_EP0SR_DIR_OUT               (0x00 << 1)

#define HAL_MCF5272_USB_IEP0CFG_MAX_PACKET_MASK     (0x03FF << 22)
#define HAL_MCF5272_USB_IEP0CFG_MAX_PACKET_SHIFT    22
#define HAL_MCF5272_USB_IEP0CFG_FIFO_SIZE_MASK      (0x03FF << 11)
#define HAL_MCF5272_USB_IEP0CFG_FIFO_SIZE_SHIFT     11
#define HAL_MCF5272_USB_IEP0CFG_FIFO_ADDR_MASK      (0x03FF << 0)
#define HAL_MCF5272_USB_IEP0CFG_FIFO_ADDR_SHIFT     0

#define HAL_MCF5272_USB_OEP0CFG_MAX_PACKET_MASK     (0x03FF << 22)
#define HAL_MCF5272_USB_OEP0CFG_MAX_PACKET_SHIFT    22
#define HAL_MCF5272_USB_OEP0CFG_FIFO_SIZE_MASK      (0x03FF << 11)
#define HAL_MCF5272_USB_OEP0CFG_FIFO_SIZE_SHIFT     11
#define HAL_MCF5272_USB_OEP0CFG_FIFO_ADDR_MASK      (0x03FF << 0)
#define HAL_MCF5272_USB_OEP0CFG_FIFO_ADDR_SHIFT     0

#define HAL_MCF5272_USB_EPnCFG_MAX_PACKET_MASK      (0x03FF << 22)
#define HAL_MCF5272_USB_EPnCFG_MAX_PACKET_SHIFT     22
#define HAL_MCF5272_USB_EPnCFG_FIFO_SIZE_MASK       (0x03FF << 11)
#define HAL_MCF5272_USB_EPnCFG_FIFO_SIZE_SHIFT      11
#define HAL_MCF5272_USB_EPnCFG_FIFO_ADDR_MASK       (0x03FF << 0)
#define HAL_MCF5272_USB_EPnCFG_FIFO_ADDR_SHIFT      0

#define HAL_MCF5272_USB_EP0CTL_DEBUG                (0x01 << 18)
#define HAL_MCF5272_USB_EP0CTL_WOR_LVL              (0x01 << 17)
#define HAL_MCF5272_USB_EP0CTL_WOR_EN               (0x01 << 16)
#define HAL_MCF5272_USB_EP0CTL_CLK_SEL              (0x01 << 15)
#define HAL_MCF5272_USB_EP0CTL_RESUME               (0x01 << 14)
#define HAL_MCF5272_USB_EP0CTL_AFE_EN               (0x01 << 13)
#define HAL_MCF5272_USB_EP0CTL_BUS_PWR              (0x01 << 12)
#define HAL_MCF5272_USB_EP0CTL_USB_EN               (0x01 << 11)
#define HAL_MCF5272_USB_EP0CTL_CFG_RAM_VAL          (0x01 << 10)
#define HAL_MCF5272_USB_EP0CTL_CMD_ERR              (0x01 << 9)
#define HAL_MCF5272_USB_EP0CTL_CMD_OVER             (0x01 << 8)
#define HAL_MCF5272_USB_EP0CTL_CMD_CRC_ERR          (0x01 << 7)
#define HAL_MCF5272_USB_EP0CTL_CMD_OUT_LVL_MASK     (0x03 << 4)
#define HAL_MCF5272_USB_EP0CTL_CMD_OUT_LVL_25       (0x00 << 4)
#define HAL_MCF5272_USB_EP0CTL_CMD_OUT_LVL_50       (0x01 << 4)
#define HAL_MCF5272_USB_EP0CTL_CMD_OUT_LVL_75       (0x02 << 4)
#define HAL_MCF5272_USB_EP0CTL_CMD_OUT_LVL_100      (0x03 << 4)
#define HAL_MCF5272_USB_EP0CTL_CMD_IN_LVL_MASK      (0x03 << 2)
#define HAL_MCF5272_USB_EP0CTL_CMD_IN_LVL_25        (0x00 << 2)
#define HAL_MCF5272_USB_EP0CTL_CMD_IN_LVL_50        (0x01 << 2)
#define HAL_MCF5272_USB_EP0CTL_CMD_IN_LVL_75        (0x02 << 2)
#define HAL_MCF5272_USB_EP0CTL_CMD_IN_LVL_100       (0x03 << 2)
#define HAL_MCF5272_USB_EP0CTL_IN_DONE              (0x01 << 1)

#define HAL_MCF5272_USB_EPnCR_CRC_ERR               (0x01 << 7)
#define HAL_MCF5272_USB_EPnCR_ISO_MODE              (0x01 << 6)
#define HAL_MCF5272_USB_EPnCR_FIFO_LVL_MASK         (0x03 << 2)
#define HAL_MCF5272_USB_EPnCR_FIFO_LVL_25           (0x00 << 2)
#define HAL_MCF5272_USB_EPnCR_FIFO_LVL_50           (0x01 << 2)
#define HAL_MCF5272_USB_EPnCR_FIFO_LVL_75           (0x02 << 2)
#define HAL_MCF5272_USB_EPnCR_FIFO_LVL_100          (0x03 << 2)
#define HAL_MCF5272_USB_EPnCR_IN_DONE               (0x01 << 1)
#define HAL_MCF5272_USB_EPnCR_STALL                 (0x01 << 0)

#define HAL_MCF5272_USB_EP0IMR_DEV_CFG              (0x01 << 16)
#define HAL_MCF5272_USB_EP0IMR_VEND_REQ             (0x01 << 15)
#define HAL_MCF5272_USB_EP0IMR_FRM_MAT              (0x01 << 14)
#define HAL_MCF5272_USB_EP0IMR_ASOF                 (0x01 << 13)
#define HAL_MCF5272_USB_EP0IMR_SOF                  (0x01 << 12)
#define HAL_MCF5272_USB_EP0IMR_WAKE_CHG             (0x01 << 11)
#define HAL_MCF5272_USB_EP0IMR_RESUME               (0x01 << 10)
#define HAL_MCF5272_USB_EP0IMR_SUSPEND              (0x01 << 9)
#define HAL_MCF5272_USB_EP0IMR_RESET                (0x01 << 8)
#define HAL_MCF5272_USB_EP0IMR_OUT_EOT              (0x01 << 7)
#define HAL_MCF5272_USB_EP0IMR_OUT_EOP              (0x01 << 6)
#define HAL_MCF5272_USB_EP0IMR_OUT_LVL              (0x01 << 5)
#define HAL_MCF5272_USB_EP0IMR_IN_EOT               (0x01 << 4)
#define HAL_MCF5272_USB_EP0IMR_IN_EOP               (0x01 << 3)
#define HAL_MCF5272_USB_EP0IMR_UNHALT               (0x01 << 2)
#define HAL_MCF5272_USB_EP0IMR_HALT                 (0x01 << 1)
#define HAL_MCF5272_USB_EP0IMR_IN_LVL               (0x01 << 0)

#define HAL_MCF5272_USB_EP0ISR_DEV_CFG              HAL_MCF5272_USB_EP0IMR_DEV_CFG
#define HAL_MCF5272_USB_EP0ISR_VEND_REQ             HAL_MCF5272_USB_EP0IMR_VEND_REQ
#define HAL_MCF5272_USB_EP0ISR_FRM_MAT              HAL_MCF5272_USB_EP0IMR_FRM_MAT
#define HAL_MCF5272_USB_EP0ISR_ASOF                 HAL_MCF5272_USB_EP0IMR_ASOF
#define HAL_MCF5272_USB_EP0ISR_SOF                  HAL_MCF5272_USB_EP0IMR_SOF
#define HAL_MCF5272_USB_EP0ISR_WAKE_CHG             HAL_MCF5272_USB_EP0IMR_WAKE_CHG
#define HAL_MCF5272_USB_EP0ISR_RESUME               HAL_MCF5272_USB_EP0IMR_RESUME
#define HAL_MCF5272_USB_EP0ISR_SUSPEND              HAL_MCF5272_USB_EP0IMR_SUSPEND
#define HAL_MCF5272_USB_EP0ISR_RESET                HAL_MCF5272_USB_EP0IMR_RESET
#define HAL_MCF5272_USB_EP0ISR_OUT_EOT              HAL_MCF5272_USB_EP0IMR_OUT_EOT
#define HAL_MCF5272_USB_EP0ISR_OUT_EOP              HAL_MCF5272_USB_EP0IMR_OUT_EOP
#define HAL_MCF5272_USB_EP0ISR_OUT_LVL              HAL_MCF5272_USB_EP0IMR_OUT_LVL
#define HAL_MCF5272_USB_EP0ISR_IN_EOT               HAL_MCF5272_USB_EP0IMR_IN_EOT
#define HAL_MCF5272_USB_EP0ISR_IN_EOP               HAL_MCF5272_USB_EP0IMR_IN_EOP
#define HAL_MCF5272_USB_EP0ISR_UNHALT               HAL_MCF5272_USB_EP0IMR_UNHALT
#define HAL_MCF5272_USB_EP0ISR_HALT                 HAL_MCF5272_USB_EP0IMR_HALT
#define HAL_MCF5272_USB_EP0ISR_IN_LVL               HAL_MCF5272_USB_EP0IMR_IN_LVL

#define HAL_MCF5272_USB_EPnISR_HALT_ST              (0x01 << 15)
#define HAL_MCF5272_USB_EPnISR_DIR                  (0x01 << 14)
#define HAL_MCF5272_USB_EPnISR_PRES                 (0x01 << 13)
#define HAL_MCF5272_USB_EPnISR_EOT                  (0x01 << 4)
#define HAL_MCF5272_USB_EPnISR_EOP                  (0x01 << 3)
#define HAL_MCF5272_USB_EPnISR_UNHALT               (0x01 << 2)
#define HAL_MCF5272_USB_EPnISR_HALT                 (0x01 << 1)
#define HAL_MCF5272_USB_EPnISR_FIFO_LVL             (0x01 << 0)

#define HAL_MCF5272_USB_EPnIMR_EOT                  HAL_MCF5272_USB_EPnISR_EOT
#define HAL_MCF5272_USB_EPnIMR_EOP                  HAL_MCF5272_USB_EPnISR_EOP
#define HAL_MCF5272_USB_EPnIMR_UNHALT               HAL_MCF5272_USB_EPnISR_UNHALT
#define HAL_MCF5272_USB_EPnIMR_HALT                 HAL_MCF5272_USB_EPnISR_HALT
#define HAL_MCF5272_USB_EPnIMR_FIFO_LVL             HAL_MCF5272_USB_EPnISR_FIFO_LVL

// ----------------------------------------------------------------------------
// PLIC

#define HAL_MCF5272_PLIC_P0B1RR                     0x0300
#define HAL_MCF5272_PLIC_P1B1RR                     0x0304
#define HAL_MCF5272_PLIC_P2B1RR                     0x0308
#define HAL_MCF5272_PLIC_P3B1RR                     0x030C
#define HAL_MCF5272_PLIC_P0B2RR                     0x0310
#define HAL_MCF5272_PLIC_P1B2RR                     0x0314
#define HAL_MCF5272_PLIC_P2B2RR                     0x0318
#define HAL_MCF5272_PLIC_P3B2RR                     0x031C
#define HAL_MCF5272_PLIC_P0DDR                      0x0320
#define HAL_MCF5272_PLIC_P1DDR                      0x0321
#define HAL_MCF5272_PLIC_P2DDR                      0x0322
#define HAL_MCF5272_PLIC_P3DDR                      0x0323
#define HAL_MCF5272_PLIC_P0B1TR                     0x0328
#define HAL_MCF5272_PLIC_P1B1TR                     0x032C
#define HAL_MCF5272_PLIC_P2B1TR                     0x0330
#define HAL_MCF5272_PLIC_P3B1TR                     0x0334
#define HAL_MCF5272_PLIC_P0B2TR                     0x0338
#define HAL_MCF5272_PLIC_P1B2TR                     0x033C
#define HAL_MCF5272_PLIC_P2B2TR                     0x0340
#define HAL_MCF5272_PLIC_P3B2TR                     0x0344
#define HAL_MCF5272_PLIC_P0DTR                      0x0348
#define HAL_MCF5272_PLIC_P1DTR                      0x0349
#define HAL_MCF5272_PLIC_P2DTR                      0x034A
#define HAL_MCF5272_PLIC_P3DTR                      0x034B
#define HAL_MCF5272_PLIC_P0CR                       0x0350
#define HAL_MCF5272_PLIC_P1CR                       0x0352
#define HAL_MCF5272_PLIC_P2CR                       0x0354
#define HAL_MCF5272_PLIC_P3CR                       0x0356
#define HAL_MCF5272_PLIC_P0ICR                      0x0358
#define HAL_MCF5272_PLIC_P1ICR                      0x035A
#define HAL_MCF5272_PLIC_P2ICR                      0x035C
#define HAL_MCF5272_PLIC_P3ICR                      0x035E
#define HAL_MCF5272_PLIC_P0GMR                      0x0360
#define HAL_MCF5272_PLIC_P1GMR                      0x0362
#define HAL_MCF5272_PLIC_P2GMR                      0x0364
#define HAL_MCF5272_PLIC_P3GMR                      0x0366
#define HAL_MCF5272_PLIC_P0GMT                      0x0368
#define HAL_MCF5272_PLIC_P1GMT                      0x036A
#define HAL_MCF5272_PLIC_P2GMT                      0x036C
#define HAL_MCF5272_PLIC_P3GMT                      0x036E
#define HAL_MCF5272_PLIC_PGMTS                      0x0371
#define HAL_MCF5272_PLIC_PGMTA                      0x0372
#define HAL_MCF5272_PLIC_P0GCIR                     0x0374
#define HAL_MCF5272_PLIC_P1GCIR                     0x0375
#define HAL_MCF5272_PLIC_P2GCIR                     0x0376
#define HAL_MCF5272_PLIC_P3GCIR                     0x0377
#define HAL_MCF5272_PLIC_P0GCIT                     0x0378
#define HAL_MCF5272_PLIC_P1GCIT                     0x0379
#define HAL_MCF5272_PLIC_P2GCIT                     0x037A
#define HAL_MCF5272_PLIC_P3GCIT                     0x037B
#define HAL_MCF5272_PLIC_PGCITSR                    0x037F
#define HAL_MCF5272_PLIC_PDCSR                      0x0383
#define HAL_MCF5272_PLIC_P0PSR                      0x0384
#define HAL_MCF5272_PLIC_P1PSR                      0x0386
#define HAL_MCF5272_PLIC_P2PSR                      0x0388
#define HAL_MCF5272_PLIC_P3PSR                      0x038A
#define HAL_MCF5272_PLIC_PASR                       0x038C
#define HAL_MCF5272_PLIC_PLCR                       0x038F
#define HAL_MCF5272_PLIC_PDRQR                      0x0392
#define HAL_MCF5272_PLIC_P0SDR                      0x0394
#define HAL_MCF5272_PLIC_P1SDR                      0x0396
#define HAL_MCF5272_PLIC_P2SDR                      0x0398
#define HAL_MCF5272_PLIC_P3SDR                      0x039A
#define HAL_MCF5272_PLIC_PCSR                       0x039E

#define HAL_MCF5272_PLIC_PnCR_ON_OFF                (0x01 << 15)
#define HAL_MCF5272_PLIC_PnCR_M_MASK                (0x07 << 12)
#define HAL_MCF5272_PLIC_PnCR_M_SHIFT               12
#define HAL_MCF5272_PLIC_PnCR_IDL8                  (0x00 << 12)
#define HAL_MCF5272_PLIC_PnCR_IDL10                 (0x01 << 12)
#define HAL_MCF5272_PLIC_PnCR_GCI                   (0x02 << 12)
#define HAL_MCF5272_PLIC_PnCR_M_S                   (0x01 << 11)
#define HAL_MCF5272_PLIC_PnCR_G_S                   (0x01 << 10)
#define HAL_MCF5272_PLIC_PnCR_FSM                   (0x01 << 9)
#define HAL_MCF5272_PLIC_PnCR_ACT                   (0x01 << 8)
#define HAL_MCF5272_PLIC_PnCR_DMX                   (0x01 << 7)
#define HAL_MCF5272_PLIC_PnCR_SHB2                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PnCR_SHB1                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PnCR_ENB2                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PnCR_ENB1                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PLCR_LM3_MASK              (0x03 << 6)
#define HAL_MCF5272_PLIC_PLCR_LM3_SHIFT             6
#define HAL_MCF5272_PLIC_PLCR_LM2_MASK              (0x03 << 4)
#define HAL_MCF5272_PLIC_PLCR_LM2_SHIFT             4
#define HAL_MCF5272_PLIC_PLCR_LM1_MASK              (0x03 << 2)
#define HAL_MCF5272_PLIC_PLCR_LM1_SHIFT             2
#define HAL_MCF5272_PLIC_PLCR_LM0_MASK              (0x03 << 0)
#define HAL_MCF5272_PLIC_PLCR_LM0_SHIFT             0
#define HAL_MCF5272_PLIC_PLCR_LMn_NORMAL            (0x00)
#define HAL_MCF5272_PLIC_PLCR_LMn_AUTO_ECHO         (0x01)
#define HAL_MCF5272_PLIC_PLCR_LMn_LOCAL_LOOPBACK    (0x02)
#define HAL_MCF5272_PLIC_PLCR_LMn_REMOTE_LOOPBACK   (0x03)
#define HAL_MCF5272_PLIC_PLCR_LM3_NORMAL            (0x00 << 6)
#define HAL_MCF5272_PLIC_PLCR_LM3_AUTO_ECHO         (0x01 << 6)
#define HAL_MCF5272_PLIC_PLCR_LM3_LOCAL_LOOPBACK    (0x02 << 6)
#define HAL_MCF5272_PLIC_PLCR_LM3_REMOTE_LOOPBACK   (0x03 << 6)
#define HAL_MCF5272_PLIC_PLCR_LM2_NORMAL            (0x00 << 4)
#define HAL_MCF5272_PLIC_PLCR_LM2_AUTO_ECHO         (0x01 << 4)
#define HAL_MCF5272_PLIC_PLCR_LM2_LOCAL_LOOPBACK    (0x02 << 4)
#define HAL_MCF5272_PLIC_PLCR_LM2_REMOTE_LOOPBACK   (0x03 << 4)
#define HAL_MCF5272_PLIC_PLCR_LM1_NORMAL            (0x00 << 2)
#define HAL_MCF5272_PLIC_PLCR_LM1_AUTO_ECHO         (0x01 << 2)
#define HAL_MCF5272_PLIC_PLCR_LM1_LOCAL_LOOPBACK    (0x02 << 2)
#define HAL_MCF5272_PLIC_PLCR_LM1_REMOTE_LOOPBACK   (0x03 << 2)
#define HAL_MCF5272_PLIC_PLCR_LM0_NORMAL            (0x00 << 0)
#define HAL_MCF5272_PLIC_PLCR_LM0_AUTO_ECHO         (0x01 << 0)
#define HAL_MCF5272_PLIC_PLCR_LM0_LOCAL_LOOPBACK    (0x02 << 0)
#define HAL_MCF5272_PLIC_PLCR_LM0_REMOTE_LOOPBACK   (0x03 << 0)

#define HAL_MCF5272_PLIC_PnICR_IE                   (0x01 << 15)
#define HAL_MCF5272_PLIC_PnICR_GCR                  (0x01 << 11)
#define HAL_MCF5272_PLIC_PnICR_GCT                  (0x01 << 10)
#define HAL_MCF5272_PLIC_PnICR_GMR                  (0x01 << 9)
#define HAL_MCF5272_PLIC_PnICR_GMT                  (0x01 << 8)
#define HAL_MCF5272_PLIC_PnICR_DTIE                 (0x01 << 5)
#define HAL_MCF5272_PLIC_PnICR_B2TIE                (0x01 << 4)
#define HAL_MCF5272_PLIC_PnICR_B1TIE                (0x01 << 3)
#define HAL_MCF5272_PLIC_PnICR_DRIE                 (0x01 << 2)
#define HAL_MCF5272_PLIC_PnICR_B2RIE                (0x01 << 1)
#define HAL_MCF5272_PLIC_PnICR_B1RIE                (0x01 << 0)

#define HAL_MCF5272_PLIC_PnPSR_DTUE                 (0x01 << 11)
#define HAL_MCF5272_PLIC_PnPSR_B2TUE                (0x01 << 10)
#define HAL_MCF5272_PLIC_PnPSR_B1TUE                (0x01 << 9)
#define HAL_MCF5272_PLIC_PnPSR_DROE                 (0x01 << 8)
#define HAL_MCF5272_PLIC_PnPSR_B2ROE                (0x01 << 7)
#define HAL_MCF5272_PLIC_PnPSR_B1ROE                (0x01 << 6)
#define HAL_MCF5272_PLIC_PnPSR_DTDE                 (0x01 << 5)
#define HAL_MCF5272_PLIC_PnPSR_B2TDE                (0x01 << 4)
#define HAL_MCF5272_PLIC_PnPSR_B1TDE                (0x01 << 3)
#define HAL_MCF5272_PLIC_PnPSR_DRDF                 (0x01 << 2)
#define HAL_MCF5272_PLIC_PnPSR_B2RDE                (0x01 << 1)
#define HAL_MCF5272_PLIC_PnPSR_B1RDE                (0x01 << 0)

#define HAL_MCF5272_PLIC_PASR_3_MASK                (0x0F << 12)
#define HAL_MCF5272_PLIC_PASR_3_SHIFT               12
#define HAL_MCF5272_PLIC_PASR_2_MASK                (0x0F << 8)
#define HAL_MCF5272_PLIC_PASR_2_SHIFT               8
#define HAL_MCF5272_PLIC_PASR_1_MASK                (0x0F << 4)
#define HAL_MCF5272_PLIC_PASR_1_SHIFT               4
#define HAL_MCF5272_PLIC_PASR_0_MASK                (0x0F << 0)
#define HAL_MCF5272_PLIC_PASR_0_SHIFT               0
#define HAL_MCF5272_PLCI_PASR_GCRn                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PASR_GCTn                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PASR_GMRn                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PASR_GMTn                  (0x01 << 0)
#define HAL_MCF5272_PLIC_PASR_GCR3                  (0x01 << 15)
#define HAL_MCF5272_PLIC_PASR_GCT3                  (0x01 << 14)
#define HAL_MCF5272_PLIC_PASR_GMR3                  (0x01 << 13)
#define HAL_MCF5272_PLIC_PASR_GMT3                  (0x01 << 12)
#define HAL_MCF5272_PLIC_PASR_GCR2                  (0x01 << 11)
#define HAL_MCF5272_PLIC_PASR_GCT2                  (0x01 << 10)
#define HAL_MCF5272_PLIC_PASR_GMR2                  (0x01 << 9)
#define HAL_MCF5272_PLIC_PASR_GMT2                  (0x01 << 8)
#define HAL_MCF5272_PLIC_PASR_GCR1                  (0x01 << 7)
#define HAL_MCF5272_PLIC_PASR_GCT1                  (0x01 << 6)
#define HAL_MCF5272_PLIC_PASR_GMR1                  (0x01 << 5)
#define HAL_MCF5272_PLIC_PASR_GMT1                  (0x01 << 4)
#define HAL_MCF5272_PLIC_PASR_GCR0                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PASR_GCT0                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PASR_GMR0                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PASR_GMT0                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PnGMR_EOM                  (0x01 << 10)
#define HAL_MCF5272_PLIC_PnGMR_AB                   (0x01 << 9)
#define HAL_MCF5272_PLIC_PnGMR_MC                   (0x01 << 8)
#define HAL_MCF5272_PLIC_PnGMR_M_MASK               (0x0FF << 0)
#define HAL_MCF5272_PLIC_PnGMR_M_SHIFT              0

#define HAL_MCF5272_PLIC_PnGMT_L                    (0x01 << 9)
#define HAL_MCF5272_PLIC_PnGMT_R                    (0x01 << 8)
#define HAL_MCF5272_PLIC_PnGMT_M_MASK               (0x0FF << 0)
#define HAL_MCF5272_PLIC_PnGMT_M_SHIFT              0

#define HAL_MCF5272_PLIC_PGMTA_AR3                  (0x01 << 7)
#define HAL_MCF5272_PLIC_PGMTA_AR2                  (0x01 << 6)
#define HAL_MCF5272_PLIC_PGMTA_AR1                  (0x01 << 5)
#define HAL_MCF5272_PLIC_PGMTA_AR0                  (0x01 << 4)

#define HAL_MCF5272_PLIC_PGMTS_ACKn                 (0x01 << 4)
#define HAL_MCF5272_PLIC_PGMTS_ABn                  (0x01 << 0)
#define HAL_MCF5272_PLIC_PGMTS_3_SHIFT              3
#define HAL_MCF5272_PLIC_PGMTS_2_SHIFT              2
#define HAL_MCF5272_PLIC_PGMTS_1_SHIFT              1
#define HAL_MCF5272_PLIC_PGMTS_0_SHIFT              0
#define HAL_MCF5272_PLIC_PGMTS_ACK3                 (0x01 << 7)
#define HAL_MCF5272_PLIC_PGMTS_ACK2                 (0x01 << 6)
#define HAL_MCF5272_PLIC_PGMTS_ACK1                 (0x01 << 5)
#define HAL_MCF5272_PLIC_PGMTS_ACK0                 (0x01 << 4)
#define HAL_MCF5272_PLIC_PGMTS_AB3                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PGMTS_AB2                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PGMTS_AB1                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PGMTS_AB0                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PnGCIR_F                   (0x01 << 4)
#define HAL_MCF5272_PLIC_PnGCIR_C3                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PnGCIR_C2                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PnGCIR_C1                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PnGCIR_C0                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PnGCIT_R                   (0x01 << 4)
#define HAL_MCF5272_PLIC_PnGCIT_C3                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PnGCIT_C2                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PnGCIT_C1                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PnGCIT_C0                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PGCITSR_ACK3               (0x01 << 3)
#define HAL_MCF5272_PLIC_PGCITSR_ACK2               (0x01 << 2)
#define HAL_MCF5272_PLIC_PGCITSR_ACK1               (0x01 << 1)
#define HAL_MCF5272_PLIC_PGCITSR_ACK0               (0x01 << 0)

#define HAL_MCF5272_PLIC_PDCSR_DG1                  (0x01 << 5)
#define HAL_MCF5272_PLIC_PDCSR_DG0                  (0x01 << 4)
#define HAL_MCF5272_PLIC_PDCSR_DC3                  (0x01 << 3)
#define HAL_MCF5272_PLIC_PDCSR_DC2                  (0x01 << 2)
#define HAL_MCF5272_PLIC_PDCSR_DC1                  (0x01 << 1)
#define HAL_MCF5272_PLIC_PDCSR_DC0                  (0x01 << 0)

#define HAL_MCF5272_PLIC_PDRQR_SHDD1                (0x01 << 11)
#define HAL_MCF5272_PLIC_PDRQR_DCNT1                (0x01 << 10)
#define HAL_MCF5272_PLIC_PDRQR_SHDD0                (0x01 << 9)
#define HAL_MCF5272_PLIC_PDRQR_DCNT0                (0x01 << 8)
#define HAL_MCF5272_PLIC_PDRQR_DRQ_MASK             (0x03 << 0)
#define HAL_MCF5272_PLIC_PDRQR_DRQ_SHIFT            0

#define HAL_MCF5272_PLIC_PnSDR_FSW_MASK             (0x03 << 14)
#define HAL_MCF5272_PLIC_PnSDR_FSW_SHIFT            14
#define HAL_MCF5272_PLIC_PnSDR_FSW_1                (0x00 << 14)
#define HAL_MCF5272_PLIC_PnSDR_FSW_2                (0x01 << 14)
#define HAL_MCF5272_PLIC_PnSDR_FSW_8                (0x02 << 14)
#define HAL_MCF5272_PLIC_PnSDR_FSW_16               (0x03 << 14)
#define HAL_MCF5272_PLIC_PnSDR_SD_MASK              (0x03FF << 0)
#define HAL_MCF5272_PLIC_PnSDR_SD_SHIFT             0

#define HAL_MCF5272_PLIC_PCSR_NBP                   (0x01 << 15)
#define HAL_MCF5272_PLIC_PCSR_CKI_MASK              (0x03 << 6)
#define HAL_MCF5272_PLIC_PCSR_CKI_SHIFT             6
#define HAL_MCF5272_PLIC_PCSR_CKI_DCL0              (0x00 << 6)
#define HAL_MCF5272_PLIC_PCSR_CKI_FSC0              (0x01 << 6)
#define HAL_MCF5272_PLIC_PCSR_FDIV_MASK             (0x07 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_SHIFT            3
#define HAL_MCF5272_PLIC_PCSR_FDIV_4                (0x00 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_8                (0x01 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_16               (0x02 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_32               (0x03 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_64               (0x04 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_128              (0x05 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_192              (0x06 << 3)
#define HAL_MCF5272_PLIC_PCSR_FDIV_256              (0x07 << 3)
#define HAL_MCF5272_PLIC_PCSR_CMULT_MASK            (0x07 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_SHIFT           0
#define HAL_MCF5272_PLIC_PCSR_CMULT_2               (0x00 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_4               (0x01 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_8               (0x02 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_16              (0x03 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_32              (0x04 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_64              (0x05 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_128             (0x06 << 0)
#define HAL_MCF5272_PLIC_PCSR_CMULT_256             (0x07 << 0)

// ----------------------------------------------------------------------------
// The system timers. There are four timers at different MBAR offsets
//
// Some of these definitions could be re-used on other xxxx processors, but
// recent ones from the 5282 onwards have very different timer support.

#define HAL_MCF5272_TIMER0_BASE                     0x0200
#define HAL_MCF5272_TIMER1_BASE                     0x0220
#define HAL_MCF5272_TIMER2_BASE                     0x0240
#define HAL_MCF5272_TIMER3_BASE                     0x0260

// The register offsets for each timer. All registers are 16 bits
#define HAL_MCF5272_TIMER_TMR                       0x0000
#define HAL_MCF5272_TIMER_TRR                       0x0004
#define HAL_MCF5272_TIMER_TCR                       0x0008
#define HAL_MCF5272_TIMER_TCN                       0x000C
#define HAL_MCF5272_TIMER_TER                       0x0010

// The bits
#define HAL_MCF5272_TIMER_TMR_PS_MASK               0xFF00
#define HAL_MCF5272_TIMER_TMR_PS_SHIFT              8
#define HAL_MCF5272_TIMER_TMR_CE_MASK               (0x0003 << 6)
// OM is only available for timers 0 and 1
#define HAL_MCF5272_TIMER_TMR_OM                    (0x0001 << 5)
#define HAL_MCF5272_TIMER_TMR_ORI                   (0x0001 << 4)
#define HAL_MCF5272_TIMER_TMR_FRR                   (0x0001 << 3)
#define HAL_MCF5272_TIMER_TMR_CLK_MASK              (0x0003 << 1)
#define HAL_MCF5272_TIMER_TMR_CLK_STOP              (0x0000 << 1)
#define HAL_MCF5272_TIMER_TMR_CLK_MASTER            (0x0001 << 1)
#define HAL_MCF5272_TIMER_TMR_CLK_MASTER_DIV_16     (0x0002 << 1)
#define HAL_MCF5272_TIMER_TMR_CLK_TIN               (0x0003 << 1)
#define HAL_MCF5272_TIMER_TMR_RST                   (0x0001 << 0)

#define HAL_MCF5272_TIMER_TER_REF                   (0x0001 << 1)
#define HAL_MCF5272_TIMER_TER_CAP                   (0x0001 << 0)

// ----------------------------------------------------------------------------
// GPIO pins. PACNT/PBCNT/PDCNT are 32-bit. The others are 16-bit.

#define HAL_MCF5272_PACNT                           0x0080
#define HAL_MCF5272_PADDR                           0x0084
#define HAL_MCF5272_PADAT                           0x0086
#define HAL_MCF5272_PBCNT                           0x0088
#define HAL_MCF5272_PBDDR                           0x008C
#define HAL_MCF5272_PBDAT                           0x008E
#define HAL_MCF5272_PCDDR                           0x0094
#define HAL_MCF5272_PCDAT                           0x0096
#define HAL_MCF5272_PDCNT                           0x0098

// Then the bits. The direction and data registers are only
// relevant if the pin is controlled for GPIO, as per the
// appropriate 2-bit entry in the control register.
#define HAL_MCF5272_PACNT_PACNT15_MASK              (0x03 << 30)
#define HAL_MCF5272_PACNT_PACNT15_PA15              (0x00 << 30)
#define HAL_MCF5272_PACNT_PACNT15_DGNT1             (0x01 << 30)
#define HAL_MCF5272_PACNT_PACNT14_MASK              (0x03 << 28)
#define HAL_MCF5272_PACNT_PACNT14_PA14              (0x00 << 28)
#define HAL_MCF5272_PACNT_PACNT14_DREQ1             (0x01 << 28)
#define HAL_MCF5272_PACNT_PACNT13_MASK              (0x03 << 26)
#define HAL_MCF5272_PACNT_PACNT13_PA13              (0x00 << 26)
#define HAL_MCF5272_PACNT_PACNT13_DFSC3             (0x01 << 26)
#define HAL_MCF5272_PACNT_PACNT12_MASK              (0x03 << 24)
#define HAL_MCF5272_PACNT_PACNT12_PA12              (0x00 << 24)
#define HAL_MCF5272_PACNT_PACNT12_DFSC2             (0x01 << 24)
#define HAL_MCF5272_PACNT_PACNT11_MASK              (0x03 << 22)
#define HAL_MCF5272_PACNT_PACNT11_PA11              (0x00 << 22)
#define HAL_MCF5272_PACNT_PACNT11_QSPI_CS1          (0x02 << 22)
#define HAL_MCF5272_PACNT_PACNT10_MASK              (0x03 << 20)
#define HAL_MCF5272_PACNT_PACNT10_PA10              (0x00 << 20)
#define HAL_MCF5272_PACNT_PACNT10_DREQ0             (0x01 << 20)
#define HAL_MCF5272_PACNT_PACNT9_MASK               (0x03 << 18)
#define HAL_MCF5272_PACNT_PACNT9_PA9                (0x00 << 18)
#define HAL_MCF5272_PACNT_PACNT9_DGNT0              (0x01 << 18)
#define HAL_MCF5272_PACNT_PACNT8_MASK               (0x03 << 16)
#define HAL_MCF5272_PACNT_PACNT8_PA8                (0x00 << 16)
#define HAL_MCF5272_PACNT_PACNT8_FSC0_FSR0          (0x01 << 16)
#define HAL_MCF5272_PACNT_PACNT7_MASK               (0x03 << 14)
#define HAL_MCF5272_PACNT_PACNT7_PA7                (0x00 << 14)
#define HAL_MCF5272_PACNT_PACNT7_QSPI_CS3           (0x01 << 14)
#define HAL_MCF5272_PACNT_PACNT7_DOUT3              (0x02 << 14)
#define HAL_MCF5272_PACNT_PACNT6_MASK               (0x03 << 12)
#define HAL_MCF5272_PACNT_PACNT6_PA6                (0x00 << 12)
#define HAL_MCF5272_PACNT_PACNT6_USB_RXD            (0x01 << 12)
#define HAL_MCF5272_PACNT_PACNT5_MASK               (0x03 << 10)
#define HAL_MCF5272_PACNT_PACNT5_PA5                (0x00 << 10)
#define HAL_MCF5272_PACNT_PACNT5_USB_TXEN           (0x01 << 10)
#define HAL_MCF5272_PACNT_PACNT4_MASK               (0x03 << 8)
#define HAL_MCF5272_PACNT_PACNT4_PA4                (0x00 << 8)
#define HAL_MCF5272_PACNT_PACNT4_USB_SUSP           (0x01 << 8)
#define HAL_MCF5272_PACNT_PACNT3_MASK               (0x03 << 6)
#define HAL_MCF5272_PACNT_PACNT3_PA3                (0x00 << 6)
#define HAL_MCF5272_PACNT_PACNT3_USB_TN             (0x01 << 6)
#define HAL_MCF5272_PACNT_PACNT2_MASK               (0x03 << 4)
#define HAL_MCF5272_PACNT_PACNT2_PA2                (0x00 << 4)
#define HAL_MCF5272_PACNT_PACNT2_USB_RN             (0x01 << 4)
#define HAL_MCF5272_PACNT_PACNT1_MASK               (0x03 << 2)
#define HAL_MCF5272_PACNT_PACNT1_PA1                (0x00 << 2)
#define HAL_MCF5272_PACNT_PACNT1_USB_RP             (0x01 << 2)
#define HAL_MCF5272_PACNT_PACNT0_MASK               (0x03 << 0)
#define HAL_MCF5272_PACNT_PACNT0_PA0                (0x00 << 0)
#define HAL_MCF5272_PACNT_PACNT0_USB_TP             (0x01 << 0)

#define HAL_MCF5272_PBCNT_PBCNT15_MASK              (0x03 << 30)
#define HAL_MCF5272_PBCNT_PBCNT15_PB15              (0x00 << 30)
#define HAL_MCF5272_PBCNT_PBCNT15_E_MDC             (0x01 << 30)
#define HAL_MCF5272_PBCNT_PBCNT14_MASK              (0x03 << 28)
#define HAL_MCF5272_PBCNT_PBCNT14_PB14              (0x00 << 28)
#define HAL_MCF5272_PBCNT_PBCNT14_E_RXER            (0x01 << 28)
#define HAL_MCF5272_PBCNT_PBCNT13_MASK              (0x03 << 26)
#define HAL_MCF5272_PBCNT_PBCNT13_PB13              (0x00 << 26)
#define HAL_MCF5272_PBCNT_PBCNT13_E_RXD1            (0x01 << 26)
#define HAL_MCF5272_PBCNT_PBCNT12_MASK              (0x03 << 24)
#define HAL_MCF5272_PBCNT_PBCNT12_PB12              (0x00 << 24)
#define HAL_MCF5272_PBCNT_PBCNT12_E_RXD2            (0x01 << 24)
#define HAL_MCF5272_PBCNT_PBCNT11_MASK              (0x03 << 22)
#define HAL_MCF5272_PBCNT_PBCNT11_PB11              (0x00 << 22)
#define HAL_MCF5272_PBCNT_PBCNT11_E_RXD3            (0x01 << 22)
// The original user manual defined QSPI_CS for 0x02. This has
// been removed in the errata.
#define HAL_MCF5272_PBCNT_PBCNT10_MASK              (0x03 << 20)
#define HAL_MCF5272_PBCNT_PBCNT10_PB10              (0x00 << 20)
#define HAL_MCF5272_PBCNT_PBCNT10_E_TXD1            (0x01 << 20)
#define HAL_MCF5272_PBCNT_PBCNT9_MASK               (0x03 << 18)
#define HAL_MCF5272_PBCNT_PBCNT9_PB9                (0x00 << 18)
#define HAL_MCF5272_PBCNT_PBCNT9_E_TXD2             (0x01 << 18)
#define HAL_MCF5272_PBCNT_PBCNT8_MASK               (0x03 << 16)
#define HAL_MCF5272_PBCNT_PBCNT8_PB8                (0x00 << 16)
#define HAL_MCF5272_PBCNT_PBCNT8_E_TXD3             (0x01 << 16)
#define HAL_MCF5272_PBCNT_PBCNT7_MASK               (0x03 << 14)
#define HAL_MCF5272_PBCNT_PBCNT7_PB7                (0x00 << 14)
#define HAL_MCF5272_PBCNT_PBCNT7_TOUT0              (0x01 << 14)
#define HAL_MCF5272_PBCNT_PBCNT6_MASK               (0x03 << 12)
#define HAL_MCF5272_PBCNT_PBCNT6_PB6                (0x00 << 12)
#define HAL_MCF5272_PBCNT_PBCNT5_MASK               (0x03 << 10)
#define HAL_MCF5272_PBCNT_PBCNT5_PB5                (0x00 << 10)
#define HAL_MCF5272_PBCNT_PBCNT5_TA                 (0x01 << 10)
#define HAL_MCF5272_PBCNT_PBCNT4_MASK               (0x03 << 8)
#define HAL_MCF5272_PBCNT_PBCNT4_PB4                (0x00 << 8)
#define HAL_MCF5272_PBCNT_PBCNT4_URT0_CLK           (0x01 << 8)
#define HAL_MCF5272_PBCNT_PBCNT3_MASK               (0x03 << 6)
#define HAL_MCF5272_PBCNT_PBCNT3_PB3                (0x00 << 6)
#define HAL_MCF5272_PBCNT_PBCNT3_URT0_RTS           (0x01 << 6)
#define HAL_MCF5272_PBCNT_PBCNT2_MASK               (0x03 << 4)
#define HAL_MCF5272_PBCNT_PBCNT2_PB2                (0x00 << 4)
#define HAL_MCF5272_PBCNT_PBCNT2_URT0_CTS           (0x01 << 4)
#define HAL_MCF5272_PBCNT_PBCNT1_MASK               (0x03 << 2)
#define HAL_MCF5272_PBCNT_PBCNT1_PB1                (0x00 << 2)
#define HAL_MCF5272_PBCNT_PBCNT1_URT0_RXD           (0x01 << 2)
#define HAL_MCF5272_PBCNT_PBCNT0_MASK               (0x03 << 0)
#define HAL_MCF5272_PBCNT_PBCNT0_PB0                (0x00 << 0)
#define HAL_MCF5272_PBCNT_PBCNT0_URT0_TXD           (0x01 << 0)

#define HAL_MCF5272_PDCNT_PDCNT7_MASK               (0x03 << 14)
#define HAL_MCF5272_PDCNT_PDCNT7_HIGH               (0x00 << 14)
#define HAL_MCF5272_PDCNT_PDCNT7_PWM_OUT3           (0x01 << 14)
#define HAL_MCF5272_PDCNT_PDCNT7_TIN1               (0x02 << 14)
#define HAL_MCF5272_PDCNT_PDCNT6_MASK               (0x03 << 12)
#define HAL_MCF5272_PDCNT_PDCNT6_HIGH               (0x00 << 12)
#define HAL_MCF5272_PDCNT_PDCNT6_PWM_OUT2           (0x01 << 12)
#define HAL_MCF5272_PDCNT_PDCNT6_TOUT1              (0x02 << 12)
#define HAL_MCF5272_PDCNT_PDCNT5_MASK               (0x03 << 10)
#define HAL_MCF5272_PDCNT_PDCNT5_HIGH               (0x00 << 10)
#define HAL_MCF5272_PDCNT_PDCNT5_DIN3               (0x02 << 10)
#define HAL_MCF5272_PDCNT_PDCNT5_INT4               (0x03 << 10)
#define HAL_MCF5272_PDCNT_PDCNT4_MASK               (0x03 << 8)
#define HAL_MCF5272_PDCNT_PDCNT4_HIGH               (0x00 << 8)
#define HAL_MCF5272_PDCNT_PDCNT4_DOUT0              (0x01 << 8)
#define HAL_MCF5272_PDCNT_PDCNT4_URT1_TXD           (0x02 << 8)
#define HAL_MCF5272_PDCNT_PDCNT3_MASK               (0x03 << 6)
#define HAL_MCF5272_PDCNT_PDCNT3_HIGH               (0x00 << 6)
#define HAL_MCF5272_PDCNT_PDCNT3_URT1_RTS           (0x02 << 6)
#define HAL_MCF5272_PDCNT_PDCNT3_INT5               (0x03 << 6)
#define HAL_MCF5272_PDCNT_PDCNT2_MASK               (0x03 << 4)
#define HAL_MCF5272_PDCNT_PDCNT2_HIGH               (0x00 << 4)
#define HAL_MCF5272_PDCNT_PDCNT2_URT1_CTS           (0x02 << 4)
#define HAL_MCF5272_PDCNT_PDCNT2_QSPI_CS2           (0x03 << 4)
#define HAL_MCF5272_PDCNT_PDCNT1_MASK               (0x03 << 2)
#define HAL_MCF5272_PDCNT_PDCNT1_HIGH               (0x00 << 2)
#define HAL_MCF5272_PDCNT_PDCNT1_DIN0               (0x01 << 2)
#define HAL_MCF5272_PDCNT_PDCNT1_URT1_RXD           (0x02 << 2)
#define HAL_MCF5272_PDCNT_PDCNT0_MASK               (0x03 << 0)
#define HAL_MCF5272_PDCNT_PDCNT0_HIGH               (0x00 << 0)
#define HAL_MCF5272_PDCNT_PDCNT0_DCL0               (0x01 << 0)
#define HAL_MCF5272_PDCNT_PDCNT0_URT1_CLK           (0x02 << 0)

// ----------------------------------------------------------------------------
// PWM: six 8-bit registers, in three pairs
#define HAL_MCF5272_PWCR0                           0x00C0
#define HAL_MCF5272_PWCR1                           0x00C4
#define HAL_MCF5272_PWCR2                           0x00C8
#define HAL_MCF5272_PWWD0                           0x00D0
#define HAL_MCF5272_PWWD1                           0x00D4
#define HAL_MCF5272_PWWD2                           0x00D8

#define HAL_MCF5272_PWCR_EN                         (0x01 << 7)
#define HAL_MCF5272_PWCR_FRC1                       (0x01 << 6)
#define HAL_MCF5272_PWCR_LVL                        (0x01 << 5)
#define HAL_MCF5272_PWCR_CKSL_MASK                  (0x0F << 0)
#define HAL_MCF5272_PWCR_CKSL_SHIFT                 0
#define HAL_MCF5272_PWCR_CKSL_1                     (0x00 << 0)
#define HAL_MCF5272_PWCR_CKSL_2                     (0x01 << 0)
#define HAL_MCF5272_PWCR_CKSL_4                     (0x02 << 0)
#define HAL_MCF5272_PWCR_CKSL_8                     (0x03 << 0)
#define HAL_MCF5272_PWCR_CKSL_16                    (0x04 << 0)
#define HAL_MCF5272_PWCR_CKSL_32                    (0x05 << 0)
#define HAL_MCF5272_PWCR_CKSL_64                    (0x06 << 0)
#define HAL_MCF5272_PWCR_CKSL_128                   (0x07 << 0)
#define HAL_MCF5272_PWCR_CKSL_256                   (0x08 << 0)
#define HAL_MCF5272_PWCR_CKSL_512                   (0x09 << 0)
#define HAL_MCF5272_PWCR_CKSL_1024                  (0x0A << 0)
#define HAL_MCF5272_PWCR_CKSL_2048                  (0x0B << 0)
#define HAL_MCF5272_PWCR_CKSL_4096                  (0x0C << 0)
#define HAL_MCF5272_PWCR_CKSL_8192                  (0x0D << 0)
#define HAL_MCF5272_PWCR_CKSL_16384                 (0x0E << 0)
#define HAL_MCF5272_PWCR_CKSL_32768                 (0x0F << 0)

// ----------------------------------------------------------------------------
// Now allow platform-specific overrides and additions
#include <cyg/hal/plf_io.h>

#ifndef HAL_MCFxxxx_MBAR
# define HAL_MCFxxxx_MBAR       0x10000000
#endif

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PROC_IO_H_FIRST_SECOND
