#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H

//=============================================================================
//
//      var_io.h
//
//      Details of mcfxxxx memory-mapped hardware
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
//####DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
// 
//####DESCRIPTIONEND####
//=============================================================================

// Most coldfire processors share some/most of their on-chip
// peripherals with the mcf5282, so that is treated as the base
// device. The goal is to reduce the effort needed to support each
// ColdFire processor. The processor HAL still has the ability to
// override any of the settings to cope with differences between
// its processor and the 5282, with a small risk that device drivers
// are tied too closely to the 5282 implementation and need extra
// effort. The processor HAL also has to supply some information that
// is inherently too processor-specific, e.g. interrupt assignments
// and GPIO pin management.
//
// This header is #include'd by hal_io.h from the architectural HAL.
// It then #include's proc_io.h for the first time, giving the processor
// HAL a chance to define symbols such as:
//
//   HAL_MCFxxxx_HAS_MCF5282_WD
//   HAL_MCFxxxx_WD_BASE
//   HAL_MCFxxxx_HAS_MCF5282_ETH
//   HAL_MCFxxxx_ETH0_BASE
//
// The value of HAS_MCF5282_<module> corresponds to the number of
// devices. The _BASE symbols correspond to the absolute base
// addresses, e.g. (IPSBAR+0x0C00). For singleton devices there will
// be only a single base addresses, otherwise separate base addresses
// should be supplied for each device instance.
//
// Once the processor HAL has defined which mcf5282-compatible devices
// are present, this header file provides the appropriate I/O
// definitions. proc_io.h is then #include'd again so that it can undo
// some of these definitions, e.g. when some h/w functionality
// available on the 5282 is not present, or to extend the definitions
// if a device offers more than functionality than the 5282
// equivalent. Definitions for devices not present on a 5282 can be
// provided by either the first or second pass of proc_io.h. Finally
// proc_io.h will include plf_io.h, allowing platform-specific code to
// override some of the definitions or to provide additional ones for
// off-chip devices.
//
// During the second pass through proc_io.h, if a processor HAL
// #undef's a var_io.h definition then that can cause problems with
// device driver portability. There is no perfect way to handle that,
// if h/w is not 100% compatible then device drivers will struggle.
// Additional symbols will typically not be used by device drivers so
// do not cause any portability problems, but eCos will not exploit
// the extra functionality either.
//
// Symbols take the form:
//   HAL_MCFxxxx_<module>_<register>
//   HAL_MCFxxxx_<module>x_<register>
//   HAL_MCFxxxx_<module>_<register>_<field>
//   HAL_MCFxxxx_<module>x_<register>_<field>
//
// The x version is used when there may be multiple instances of a
// given device, e.g. multiple UARTs or multiple interrupt
// controllers. 

// First read in the processor-specific header so we know which
// peripherals are common. The header will be read in again at
// the end to allow the processor HAL to override some of the
// settings defined here.
#include <cyg/hal/proc_io.h>

// ----------------------------------------------------------------------------
// System registers. Note that these are not memory-mapped, they are
// accessed via the movec instruction.

// The cache control register and the access control registers
// go together.
#ifdef HAL_MCFxxxx_HAS_MCF5282_CACR_ACR
# define HAL_MCFxxxx_CACR                           0x0002
# define HAL_MCFxxxx_CACR_CENB                      (0x01 << 31)
# define HAL_MCFxxxx_CACR_CPD                       (0x01 << 28)
# define HAL_MCFxxxx_CACR_CPDI                      (0x01 << 28)
# define HAL_MCFxxxx_CACR_CFRZ                      (0x01 << 27)
# define HAL_MCFxxxx_CACR_CINV                      (0x01 << 24)
# define HAL_MCFxxxx_CACR_DISI                      (0x01 << 23)
# define HAL_MCFxxxx_CACR_DISD                      (0x01 << 22)
# define HAL_MCFxxxx_CACR_INVI                      (0x01 << 21)
# define HAL_MCFxxxx_CACR_INVD                      (0x01 << 20)
# define HAL_MCFxxxx_CACR_CEIB                      (0x01 << 10)
# define HAL_MCFxxxx_CACR_DCM                       (0x01 << 9)
# define HAL_MCFxxxx_CACR_DBWE                      (0x01 << 8)
# define HAL_MCFxxxx_CACR_DWP                       (0x01 << 5)
# define HAL_MCFxxxx_CACR_EUSP                      (0x01 << 4)
# define HAL_MCFxxxx_CACR_CLNF_MASK                 (0x03 << 0)
# define HAL_MCFxxxx_CACR_CLNF_SHIFT                0

// The two access control registers. These provide some control
// over external memory accesses.
# define HAL_MCFxxxx_ACR0                           0x0004
# define HAL_MCFxxxx_ACR1                           0x0005

# define HAL_MCFxxxx_ACRx_AB_MASK                   (0x00FF << 24)
# define HAL_MCFxxxx_ACRx_AB_SHIFT                  24
# define HAL_MCFxxxx_ACRx_AM_MASK                   (0x00FF << 16)
# define HAL_MCFxxxx_ACRx_AM_SHIFT                  16
# define HAL_MCFxxxx_ACRx_EN                        (0x01 << 15)
# define HAL_MCFxxxx_ACRx_SM_MASK                   (0x03 << 13)
# define HAL_MCFxxxx_ACRx_SM_SHIFT                  13
# define HAL_MCFxxxx_ACRx_SM_USER_ONLY              (0x00 << 13)
# define HAL_MCFxxxx_ACRx_SM_SUPERVISOR_ONLY        (0x01 << 13)
# define HAL_MCFxxxx_ACRx_SM_ALWAYS                 (0x02 << 13)
# define HAL_MCFxxxx_ACRx_CM                        (0x01 <<  6)
# define HAL_MCFxxxx_ACRx_BUFW                      (0x01 <<  5)
# define HAL_MCFxxxx_ACRx_BWE                       (0x01 <<  5)
# define HAL_MCFxxxx_ACRx_WP                        (0x01 <<  2)
#endif  // HAL_MCFxxxx_HAS_MCF5282_CACR_ACR

#ifdef HAL_MCFxxxx_HAS_MCF5282_RAMBAR
# define HAL_MCFxxxx_RAMBAR                         0x0C05
# define HAL_MCFxxxx_RAMBAR_BA_MASK                 (0x00FFFF0000 << 0)
# define HAL_MCFxxxx_RAMBAR_BA_SHIFT                0
# define HAL_MCFxxxx_RAMBAR_PRI_MASK                (0x03 << 10)
# define HAL_MCFxxxx_RAMBAR_PRI_SHIFT               10
# define HAL_MCFxxxx_RAMBAR_PRI_CPU_CPU             (0x03 << 10)
# define HAL_MCFxxxx_RAMBAR_PRI_CPU_DMA             (0x02 << 10)
# define HAL_MCFxxxx_RAMBAR_PRI_DMA_CPU             (0x01 << 10)
# define HAL_MCFxxxx_RAMBAR_PRI_DMA_DMA             (0x00 << 10)
# define HAL_MCFxxxx_RAMBAR_SPV                     (0x01 << 9)
# define HAL_MCFxxxx_RAMBAR_WP                      (0x01 << 8)
# define HAL_MCFxxxx_RAMBAR_CI                      (0x01 << 5)
# define HAL_MCFxxxx_RAMBAR_SC                      (0x01 << 4)
# define HAL_MCFxxxx_RAMBAR_SD                      (0x01 << 3)
# define HAL_MCFxxxx_RAMBAR_UC                      (0x01 << 2)
# define HAL_MCFxxxx_RAMBAR_UD                      (0x01 << 1)
# define HAL_MCFxxxx_RAMBAR_V                       (0x01 << 0)
#endif

#ifdef HAL_MCFxxxx_HAS_MCF5282_FLASHBAR
# define HAL_MCFxxxx_FLASHBAR                       0x0C04
# define HAL_MCFxxxx_FLASHBAR_BA_MASK               (0x00FFF80000 << 0)
# define HAL_MCFxxxx_FLASHBAR_BA_SHIFT              0
# define HAL_MCFxxxx_FLASHBAR_WP                    (0x01 << 8)
# define HAL_MCFxxxx_FLASHBAR_CI                    (0x01 << 5)
# define HAL_MCFxxxx_FLASHBAR_SC                    (0x01 << 4)
# define HAL_MCFxxxx_FLASHBAR_SD                    (0x01 << 3)
# define HAL_MCFxxxx_FLASHBAR_UC                    (0x01 << 2)
# define HAL_MCFxxxx_FLASHBAR_UD                    (0x01 << 1)
# define HAL_MCFxxxx_FLASHBAR_V                     (0x01 << 0)
#endif

#ifdef HAL_MCFxxxx_HAS_MCF5282_VBR
# define HAL_MCFxxxx_VBR                            0x0801
#endif

// ----------------------------------------------------------------------------
// SCM. For convenience this encapsulates the SCM system control
// module, the PMM power management module, the CCM chip configuration
// module, and the CLOCK clock module. These are interrelated in
// various ways, so are likely to be all present or all absent.

#ifdef HAL_MCFxxxx_HAS_MCF5282_SCM_PMM_CLOCK_CCM
// IPSBAR, 32 bits, controls the base address of the on-chip
// peripherals. This is equivalent to the MBAR register on other
// processors, but is only accessible via IPSBAR rather than as a
// control register. This is a bit strange - in theory IPSBAR could be
// disabled, making it inaccessible without a reset.
# define HAL_MCFxxxx_SCM_IPSBAR                     0x00000000
// RAMBAR, 32 bits. In practise this should always be a copy of the
// control register, or rather of the bits that are supported in the SCM.
# define HAL_MCFxxxx_SCM_RAMBAR                     0x00000008
// Core reset status register, 8 bits.
# define HAL_MCFxxxx_SCM_CRSR                       0x00000010
// Core watchdog control register, 8 bits.
# define HAL_MCFxxxx_SCM_CWCR                       0x00000011
// Core watchdog service register, 8 bits
# define HAL_MCFxxxx_SCM_CWSR                       0x00000013
// Bus master park register, 32 bits
# define HAL_MCFxxxx_SCM_MPARK                      0x0000001C
// Master privilege register, 8 bits
# define HAL_MCFxxxx_SCM_MPR                        0x00000020
// The access control registers, 9 * 8 bits.
# define HAL_MCFxxxx_SCM_PACR0                      0x00000024
# define HAL_MCFxxxx_SCM_PACR1                      0x00000025
# define HAL_MCFxxxx_SCM_PACR2                      0x00000026
# define HAL_MCFxxxx_SCM_PACR3                      0x00000027
# define HAL_MCFxxxx_SCM_PACR4                      0x00000028
# define HAL_MCFxxxx_SCM_PACR5                      0x0000002A
# define HAL_MCFxxxx_SCM_PACR6                      0x0000002B
# define HAL_MCFxxxx_SCM_PACR7                      0x0000002C
# define HAL_MCFxxxx_SCM_PACR8                      0x0000002E
# define HAL_MCFxxxx_SCM_GPACR0                     0x00000030
# define HAL_MCFxxxx_SCM_GPACR1                     0x00000031


# define HAL_MCFxxxx_SCM_IPSBAR_BA_MASK             0xC0000000
# define HAL_MCFxxxx_SCM_IPSBAR_BA_SHIFT            30
# define HAL_MCFxxxx_SCM_IPSBAR_V                   (0x01 << 0)

# define HAL_MCFxxxx_SCM_RAMBAR_BA_MASK             (0x00FFFF << 16)
# define HAL_MCFxxxx_SCM_RAMBAR_BA_SHIFT            16
# define HAL_MCFxxxx_SCM_RAMBAR_BDE                 (0x01 << 9)

# define HAL_MCFxxxx_SCM_CRSR_EXT                   (0x01 << 7)
# define HAL_MCFxxxx_SCM_CRSR_CWDR                  (0x01 << 5)

# define HAL_MCFxxxx_SCM_CWCR_CWE                   (0x01 << 7)
# define HAL_MCFxxxx_SCM_CWCR_CWRI                  (0x01 << 6)
# define HAL_MCFxxxx_SCM_CWCR_CWT_MASK              (0x07 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_SHIFT             3
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_9               (0x00 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_11              (0x01 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_13              (0x02 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_15              (0x03 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_19              (0x04 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_23              (0x05 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_27              (0x06 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWT_2_31              (0x07 << 3)
# define HAL_MCFxxxx_SCM_CWCR_CWTA                  (0x01 << 2)
# define HAL_MCFxxxx_SCM_CWCR_CWTAVAL               (0x01 << 1)
# define HAL_MCFxxxx_SCM_CWCR_CWTIF                 (0x01 << 0)

// The magic values that should be written to the watchdog service register
# define HAL_MCFxxxx_SCM_CWSR_MAGIC0                0x0055
# define HAL_MCFxxxx_SCM_CWSR_MAGIC1                0x00AA

// Bus master parking. M3 is ethernet, M2 is DMA, M1 is internal,
// M0 is the ColdFire core.
# define HAL_MCFxxxx_SCM_MPARK_M2_P_EN              (0x01 << 25)
# define HAL_MCFxxxx_SCM_MPARK_BCR24BIT             (0x01 << 24)
# define HAL_MCFxxxx_SCM_MPARK_M3_PRTY_MASK         (0x03 << 22)
# define HAL_MCFxxxx_SCM_MPARK_M3_PRTY_SHIFT        22
# define HAL_MCFxxxx_SCM_MPARK_M2_PRTY_MASK         (0x03 << 20)
# define HAL_MCFxxxx_SCM_MPARK_M2_PRTY_SHIFT        20
# define HAL_MCFxxxx_SCM_MPARK_M0_PRTY_MASK         (0x03 << 18)
# define HAL_MCFxxxx_SCM_MPARK_M0_PRTY_SHIFT        18
# define HAL_MCFxxxx_SCM_MPARK_M1_PRTY_MASK         (0x03 << 16)
# define HAL_MCFxxxx_SCM_MPARK_M1_PRTY_SHIFT        16
# define HAL_MCFxxxx_SCM_MPARK_FIXED                (0x01 << 14)
# define HAL_MCFxxxx_SCM_MPARK_TIMEOUT              (0x01 << 13)
# define HAL_MCFxxxx_SCM_MPARK_PRKLAST              (0x01 << 12)
# define HAL_MCFxxxx_SCM_MPARK_LCKOUT_TIME_MASK     (0x0F << 8)
# define HAL_MCFxxxx_SCM_MPARK_LCKOUT_TIME_SHIFT    8

# define HAL_MCFxxxx_SCM_MPR_M3                     (0x01 << 3)
# define HAL_MCFxxxx_SCM_MPR_M2                     (0x01 << 2)
# define HAL_MCFxxxx_SCM_MPR_M1                     (0x01 << 1)
# define HAL_MCFxxxx_SCM_MPR_M0                     (0x01 << 0)

// Each access control register is split into two nibbles
# define HAL_MCFxxxx_SCM_PACRx_LOCK1                (0x01 << 7)
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_CTRL1_MASK    (0x07 << 4)
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_CTRL1_SHIFT   4
# define HAL_MCFxxxx_SCM_PACRx_LOCK0                (0x01 << 3)
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_CTRL0_MASK    (0x07 << 0)
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_CTRL0_SHIFT   0

// The access granted. Separated into supervisor and user.
// R for read access, W for write access, N for no-access.
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_RW_RW         0x04
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_RW_R          0x05
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_RW_N          0x00
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_R_R           0x02
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_R_N           0x01
# define HAL_MCFxxxx_SCM_PACRx_ACCESS_N_N           0x07

// Exactly which PACR nibbles control which on-chip peripherals
// is a processor property so defined in proc_io.h

# define HAL_MCFxxxx_SCM_GPACR_LOCK                 (0x01 << 7)
# define HAL_MCFxxxx_SCM_GPACR_ACCESS_CTRL_MASK     (0x0F << 0)
# define HAL_MCFxxxx_SCM_GPACR_ACCESS_CTRL_SHIFT    0

// Read/Write/eXecute/No access in supervisor and user modes
# define HAL_MCFxxxx_SCM_GPACR_RW_N                 0x00
# define HAL_MCFxxxx_SCM_GPACR_R_N                  0x01
# define HAL_MCFxxxx_SCM_GPACR_R_R                  0x02
# define HAL_MCFxxxx_SCM_GPACR_RW_RW                0x04
# define HAL_MCFxxxx_SCM_GPACR_RW_R                 0x05
# define HAL_MCFxxxx_SCM_GPACR_N_N                  0x07
# define HAL_MCFxxxx_SCM_GPACR_RWX_N                0x08
# define HAL_MCFxxxx_SCM_GPACR_RX_N                 0x09
# define HAL_MCFxxxx_SCM_GPACR_RX_RX                0x0A
# define HAL_MCFxxxx_SCM_GPACR_X_N                  0x0B
# define HAL_MCFxxxx_SCM_GPACR_RWX_RWX              0x0C
# define HAL_MCFxxxx_SCM_GPACR_RWX_RX               0x0D
# define HAL_MCFxxxx_SCM_GPACR_RWX_X                0x0F

// Power management. This can get confusing because there is an
// overlap with the chip configuration module, the reset module, and
// the system controller module. There are two registers, one in the
// SCM part of the address space, the other in the reset
// controller/chip configuration. Assume a single device.

// 8-bit interrupt control register
# define HAL_MCFxxxx_PMM_LPICR                      0x00000012
// 8-bit control register.
# define HAL_MCFxxxx_PMM_LPCR                       0x00110007

# define HAL_MCFxxxx_PMM_LPICR_ENBSTOP                  (0x01 << 7)
# define HAL_MCFxxxx_PMM_LPICR_XLPM_IPL_MASK            (0x07 << 4)
# define HAL_MCFxxxx_PMM_LPICR_XLPM_IPL_SHIFT           4

# define HAL_MCFxxxx_PMM_LPCR_LPMD_MASK                 (0x03 << 6)
# define HAL_MCFxxxx_PMM_LPCR_LPMD_SHIFT                6
# define HAL_MCFxxxx_PMM_LPCR_LPMD_STOP                 (0x03 << 6)
# define HAL_MCFxxxx_PMM_LPCR_LPMD_WAIT                 (0x02 << 6)
# define HAL_MCFxxxx_PMM_LPCR_LPMD_DOZE                 (0x01 << 6)
# define HAL_MCFxxxx_PMM_LPCR_LPMD_RUN                  (0x00 << 6)
# define HAL_MCFxxxx_PMM_LPCR_STPMD_MASK                (0x03 << 3)
# define HAL_MCFxxxx_PMM_LPCR_STPMD_SHIFT               3
# define HAL_MCFxxxx_PMM_LPCR_STPMD_CLKOUT_PLL_OSC_PMM  (0x00 << 3)
# define HAL_MCFxxxx_PMM_LPCR_STPMD_PLL_OSC_PMM         (0x01 << 3)
# define HAL_MCFxxxx_PMM_LPCR_STPMD_OSC_PMM             (0x02 << 3)
# define HAL_MCFxxxx_PMM_LPCR_STPMD_PMM                 (0x03 << 3)
# define HAL_MCFxxxx_PMM_LPCR_LVDSE                     (0x01 << 1)

// The clock module. Assume a single device.
// Synthesizer control register, 16 bits
# define HAL_MCFxxxx_CLOCK_SYNCR                    0x00120000
// Synthesizer status register, 8 bits
# define HAL_MCFxxxx_CLOCK_SYNSR                    0x00120002

# define HAL_MCFxxxx_CLOCK_SYNCR_LOLRE              (0x01 << 15)
# define HAL_MCFxxxx_CLOCK_SYNCR_MFD_MASK           (0x07 << 12)
# define HAL_MCFxxxx_CLOCK_SYNCR_MFD_SHIFT          12
# define HAL_MCFxxxx_CLOCK_SYNCR_LOCRE              (0x01 << 11)
# define HAL_MCFxxxx_CLOCK_SYNCR_RFD_MASK           (0x07 << 8)
# define HAL_MCFxxxx_CLOCK_SYNCR_RFD_SHIFT          8
# define HAL_MCFxxxx_CLOCK_SYNCR_LOCEN              (0x01 << 7)
# define HAL_MCFxxxx_CLOCK_SYNCR_DISCLK             (0x01 << 6)
# define HAL_MCFxxxx_CLOCK_SYNCR_FWKUP              (0x01 << 5)
# define HAL_MCFxxxx_CLOCK_SYNCR_STPMD_MASK         (0x03 << 2)
# define HAL_MCFxxxx_CLOCK_SYNCR_STPMD_SHIFT        2

# define HAL_MCFxxxx_CLOCK_SYNSR_PLLMODE            (0x01 << 7)
# define HAL_MCFxxxx_CLOCK_SYNSR_PLLSEL             (0x01 << 6)
# define HAL_MCFxxxx_CLOCK_SYNSR_PLLREF             (0x01 << 5)
# define HAL_MCFxxxx_CLOCK_SYNSR_LOCKS              (0x01 << 4)
# define HAL_MCFxxxx_CLOCK_SYNSR_LOCK               (0x01 << 3)
# define HAL_MCFxxxx_CLOCK_SYNSR_LOCS               (0x01 << 2)

// Chip configuration module
// Four 16-bit registers: chip configuration, low-power control, reset
// configuration, and chip identification. LPCR is described in the
// power management section.
# define HAL_MCFxxxx_CCM_CCR                        0x00110004
# define HAL_MCFxxxx_CCM_RCON                       0x00110008
# define HAL_MCFxxxx_CCM_CIR                        0x0011000A

# define HAL_MCFxxxx_CCM_CCR_LOAD                   (0x01 << 15)
# define HAL_MCFxxxx_CCM_CCR_MODE_MASK              (0x07 << 8)
# define HAL_MCFxxxx_CCM_CCR_MODE_SHIFT             8
# define HAL_MCFxxxx_CCM_CCR_MODE_SINGLE_CHIP       (0x06 << 8)
# define HAL_MCFxxxx_CCM_CCR_MODE_MASTER            (0x07 << 8)
# define HAL_MCFxxxx_CCM_CCR_SZEN                   (0x01 << 6)
# define HAL_MCFxxxx_CCM_CCR_PSTEN                  (0x01 << 5)
# define HAL_MCFxxxx_CCM_CCR_BME                    (0x01 << 3)
# define HAL_MCFxxxx_CCM_CCR_BMT_MASK               (0x07 << 0)
# define HAL_MCFxxxx_CCM_CCR_BMT_SHIFT              0
# define HAL_MCFxxxx_CCM_CCR_BMT_65536              0x00
# define HAL_MCFxxxx_CCM_CCR_BMT_32768              0x01
# define HAL_MCFxxxx_CCM_CCR_BMT_16384              0x02
# define HAL_MCFxxxx_CCM_CCR_BMT_8192               0x03
# define HAL_MCFxxxx_CCM_CCR_BMT_4096               0x04
# define HAL_MCFxxxx_CCM_CCR_BMT_2048               0x05
# define HAL_MCFxxxx_CCM_CCR_BMT_1024               0x06
# define HAL_MCFxxxx_CCM_CCR_BMT_512                0x07

# define HAL_MCFxxxx_CCM_RCON_RCSC_MASK             (0x03 << 8)
# define HAL_MCFxxxx_CCM_RCON_RCSC_SHIFT            8
# define HAL_MCFxxxx_CCM_RCON_RPLLSEL               (0x01 << 7)
# define HAL_MCFxxxx_CCM_RCON_RPLLREF               (0x01 << 6)
# define HAL_MCFxxxx_CCM_RCON_RLOAD                 (0x01 << 5)
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_MASK           (0x03 << 3)
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_SHIFT          3
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_INTERNAL       (0x00 << 3)
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_16             (0x01 << 3)
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_8              (0x02 << 3)
# define HAL_MCFxxxx_CCM_RCON_BOOTPS_32             (0x03 << 3)
# define HAL_MCFxxxx_CCM_RCON_BOOTSEL               (0x01 << 2)
# define HAL_MCFxxxx_CCM_RCON_MODE                  (0x01 << 0)

# define HAL_MCF5282_CCM_CIR_PIN_MASK               (0x00FF << 8)
# define HAL_MCF5282_CCM_CIR_PIN_SHIFT              8
# define HAL_MCF5282_CCM_CIR_PRN_MASK               (0x00FF << 0)
# define HAL_MCF5282_CCM_CIR_PRN_SHIFT              0

#endif  // HAL_MCFxxxx_HAS_MCF5282_SCM_PMM_CLOCK_CCM

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_RST
// Reset controller
// Two 8-bit registers, reset control and reset status
# define HAL_MCFxxxx_RST_RCR                        0x00000000
# define HAL_MCFxxxx_RST_RSR                        0x00000001

# define HAL_MCFxxxx_RST_RCR_SOFTRST                (0x01 << 7)
# define HAL_MCFxxxx_RST_RCR_FRCRSTOUT              (0x01 << 6)
# define HAL_MCFxxxx_RST_RCR_LVDF                   (0x01 << 4)
# define HAL_MCFxxxx_RST_RCR_LVDIE                  (0x01 << 3)
# define HAL_MCFxxxx_RST_RCR_LVDRE                  (0x01 << 2)
# define HAL_MCFxxxx_RST_RCR_LVDE                   (0x01 << 0)

# define HAL_MCFxxxx_RST_RSR_LVD                    (0x01 << 6)
# define HAL_MCFxxxx_RST_RSR_SOFT                   (0x01 << 5)
# define HAL_MCFxxxx_RST_RSR_WDR                    (0x01 << 4)
# define HAL_MCFxxxx_RST_RSR_POR                    (0x01 << 3)
# define HAL_MCFxxxx_RST_RSR_EXT                    (0x01 << 2)
# define HAL_MCFxxxx_RST_RSR_LOC                    (0x01 << 1)
# define HAL_MCFxxxx_RST_RSR_LOL                    (0x01 << 0)
#endif

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_WD
// Watchdog timer. Only a single device.
// Four 16-bit registers: control, modulus, count and service
# define HAL_MCFxxxx_WD_WCR                         0x00000000
# define HAL_MCFxxxx_WD_WMR                         0x00000002
# define HAL_MCFxxxx_WD_WCNTR                       0x00000004
# define HAL_MCFxxxx_WD_WSR                         0x00000006

# define HAL_MCFxxxx_WD_WCR_WAIT                    (0x01 << 3)
# define HAL_MCFxxxx_WD_WCR_DOZE                    (0x01 << 2)
# define HAL_MCFxxxx_WD_WCR_HALTED                  (0x01 << 1)
# define HAL_MCFxxxx_WD_WCR_EN                      (0x01 << 0)

// The modulus and count registers are just 16 bits of counter data
// The magic numbers for the service register
# define HAL_MCF5282_WD_WSR_MAGIC0                  0x5555
# define HAL_MCF5282_WD_WSR_MAGIC1                  0xAAAA

#endif  // HAL_MCFxxxx_HAS_MCF5282_WD

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_CS
// Chip select module. Assume a single device.

// Seven sets of registers, a 16-bit address register, a 32-bit
// mask register and a 16-bit control register
# define HAL_MCFxxxx_CS_CSAR0                       0x00000080
# define HAL_MCFxxxx_CS_CSMR0                       0x00000084
# define HAL_MCFxxxx_CS_CSCR0                       0x0000008A
# define HAL_MCFxxxx_CS_CSAR1                       0x0000008C
# define HAL_MCFxxxx_CS_CSMR1                       0x00000090
# define HAL_MCFxxxx_CS_CSCR1                       0x00000096
# define HAL_MCFxxxx_CS_CSAR2                       0x00000098
# define HAL_MCFxxxx_CS_CSMR2                       0x0000009C
# define HAL_MCFxxxx_CS_CSCR2                       0x000000A2
# define HAL_MCFxxxx_CS_CSAR3                       0x000000A4
# define HAL_MCFxxxx_CS_CSMR3                       0x000000A8
# define HAL_MCFxxxx_CS_CSCR3                       0x000000AE
# define HAL_MCFxxxx_CS_CSAR4                       0x000000B0
# define HAL_MCFxxxx_CS_CSMR4                       0x000000B4
# define HAL_MCFxxxx_CS_CSCR4                       0x000000BA
# define HAL_MCFxxxx_CS_CSAR5                       0x000000BC
# define HAL_MCFxxxx_CS_CSMR5                       0x000000C0
# define HAL_MCFxxxx_CS_CSCR5                       0x000000C6
# define HAL_MCFxxxx_CS_CSAR6                       0x000000C8
# define HAL_MCFxxxx_CS_CSMR6                       0x000000CC
# define HAL_MCFxxxx_CS_CSCR6                       0x000000D2

// The address register is just the top 16 bits of the address.
# define HAL_MCFxxxx_CS_CSMRx_BAM_MASK              (0x00FFFF << 16)
# define HAL_MCFxxxx_CS_CSMRx_BAM_SHIFT             16
# define HAL_MCFxxxx_CS_CSMRx_WP                    (0x01 << 8)
# define HAL_MCFxxxx_CS_CSMRx_AM                    (0x01 << 6)
# define HAL_MCFxxxx_CS_CSMRx_CI                    (0x01 << 5)
# define HAL_MCFxxxx_CS_CSMRx_SC                    (0x01 << 4)
# define HAL_MCFxxxx_CS_CSMRx_SD                    (0x01 << 3)
# define HAL_MCFxxxx_CS_CSMRx_UC                    (0x01 << 2)
# define HAL_MCFxxxx_CS_CSMRx_UD                    (0x01 << 1)
# define HAL_MCFxxxx_CS_CSMRx_V                     (0x01 << 0)

# define HAL_MCFxxxx_CS_CSCRx_WS_MASK               (0x000F << 10)
# define HAL_MCFxxxx_CS_CSCRx_WS_SHIFT              10
# define HAL_MCFxxxx_CS_CSCRx_AA                    (0x01 << 8)
# define HAL_MCFxxxx_CS_CSCRx_PS_MASK               (0x03 << 6)
# define HAL_MCFxxxx_CS_CSCRx_PS_SHIFT              6
# define HAL_MCFxxxx_CS_CSCRx_PS_32                 (0x00 << 6)
# define HAL_MCFxxxx_CS_CSCRx_PS_8                  (0x01 << 6)
# define HAL_MCFxxxx_CS_CSCRx_PS_16                 (0x02 << 6)
# define HAL_MCFxxxx_CS_CSCRx_BEM                   (0x01 << 5)
# define HAL_MCFxxxx_CS_CSCRx_BSTR                  (0x01 << 4)
# define HAL_MCFxxxx_CS_CSCRx_BSTW                  (0x01 << 3)

#endif  // HAL_MCFxxxx_HAS_MCF5282_CS

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_SDRAMC
// SDRAM controller. Assume a single device.

// A 16-bit overall control register
# define HAL_MCFxxxx_SDRAMC_DCR                     0x00000040
// Two sets of two 32-bit registers
# define HAL_MCFxxxx_SDRAMC_DACR0                   0x00000048
# define HAL_MCFxxxx_SDRAMC_DMR0                    0x0000004C
# define HAL_MCFxxxx_SDRAMC_DACR1                   0x00000050
# define HAL_MCFxxxx_SDRAMC_DMR1                    0x00000054

# define HAL_MCFxxxx_SDRAMC_DCR_NAM                 (0x01 << 13)
# define HAL_MCFxxxx_SDRAMC_DCR_COC                 (0x01 << 12)
# define HAL_MCFxxxx_SDRAMC_DCR_IS                  (0x01 << 11)
# define HAL_MCFxxxx_SDRAMC_DCR_RTIM_MASK           (0x03 << 9)
# define HAL_MCFxxxx_SDRAMC_DCR_RTIM_SHIFT          9
# define HAL_MCFxxxx_SDRAMC_DCR_RC_MASK             (0x01FF << 0)
# define HAL_MCFxxxx_SDRAMC_DCR_RC_SHIFT            0

# define HAL_MCFxxxx_SDRAMC_DACRx_BA_MASK           (0x03FFF << 18)
# define HAL_MCFxxxx_SDRAMC_DACRx_BA_SHIFT          18
# define HAL_MCFxxxx_SDRAMC_DACRx_RE                (0x01 << 15)
# define HAL_MCFxxxx_SDRAMC_DACRx_CASL_MASK         (0x03 << 12)
# define HAL_MCFxxxx_SDRAMC_DACRx_CASL_SHIFT        12
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_MASK          (0x07 << 8)
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_SHIFT         8
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_17            0x00
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_18            0x01
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_19            0x02
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_20            0x03
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_21            0x04
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_22            0x05
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_23            0x06
# define HAL_MCFxxxx_SDRAMC_DACRx_CBM_24            0x07
# define HAL_MCFxxxx_SDRAMC_DACRx_IMRS              (0x01 << 6)
# define HAL_MCFxxxx_SDRAMC_DACRx_PS_MASK           (0x03 << 4)
# define HAL_MCFxxxx_SDRAMC_DACRx_PS_SHIFT          4
# define HAL_MCFxxxx_SDRAMC_DACRx_PS_32             (0x00 << 4)
# define HAL_MCFxxxx_SDRAMC_DACRx_PS_8              (0x01 << 4)
# define HAL_MCFxxxx_SDRAMC_DACRx_PS_16             (0x02 << 4)
# define HAL_MCFxxxx_SDRAMC_DACRx_IP                (0x01 << 3)

# define HAL_MCFxxxx_SDRAMC_DMRx_BA_MASK            (0x03FFF << 18)
# define HAL_MCFxxxx_SDRAMC_DMRx_BA_SHIFT           18
# define HAL_MCFxxxx_SDRAMC_DMRx_WP                 (0x01 << 8)
# define HAL_MCFxxxx_SDRAMC_DMRx_CI                 (0x01 << 6)
# define HAL_MCFxxxx_SDRAMC_DMRx_AM                 (0x01 << 5)
# define HAL_MCFxxxx_SDRAMC_DMRx_SC                 (0x01 << 4)
# define HAL_MCFxxxx_SDRAMC_DMRx_SD                 (0x01 << 3)
# define HAL_MCFxxxx_SDRAMC_DMRx_UC                 (0x01 << 2)
# define HAL_MCFxxxx_SDRAMC_DMRx_UD                 (0x01 << 1)
# define HAL_MCFxxxx_SDRAMC_DMRx_V                  (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_SDRAMC

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_CFM

// The CFM module - on-chip flash. Assume a single device.
// Configuration register, 16 bits.
# define HAL_MCFxxxx_CFM_CR                         0x0000
// Clock divider register, 8 bits.
# define HAL_MCFxxxx_CFM_CLKD                       0x0002
// Security register, 32 bits
# define HAL_MCFxxxx_CFM_SEC                        0x0008
// Protection register, 32 bits
# define HAL_MCFxxxx_CFM_PROT                       0x0010
// Supervisor access register, 32 bits
# define HAL_MCFxxxx_CFM_SACC                       0x0014
// Data access register, 32 bits
# define HAL_MCFxxxx_CFM_DACC                       0x0018
// User status register, 8 bits
# define HAL_MCFxxxx_CFM_USTAT                      0x0020
// Command register, 8 bits
# define HAL_MCFxxxx_CFM_CMD                        0x0024

# define HAL_MCFxxxx_CFM_CR_LOCK                    (0x01 << 10)
# define HAL_MCFxxxx_CFM_CR_PVIE                    (0x01 << 9)
# define HAL_MCFxxxx_CFM_CR_AEIE                    (0x01 << 8)
# define HAL_MCFxxxx_CFM_CR_CBEIE                   (0x01 << 7)
# define HAL_MCFxxxx_CFM_CR_CCIE                    (0x01 << 6)
# define HAL_MCFxxxx_CFM_CR_KEYACC                  (0x01 << 5)

# define HAL_MCFxxxx_CFM_CLKD_DIVLD                 (0x01 << 7)
# define HAL_MCFxxxx_CFM_CLKD_PRDIV8                (0x01 << 6)
# define HAL_MCFxxxx_CFM_CLKD_DIV_MASK              (0x3F << 0)
# define HAL_MCFxxxx_CFM_CLKD_DIV_SHIFT             0

# define HAL_MCFxxxx_CFM_SEC_KEYEN                  (0x01 << 31)
# define HAL_MCFxxxx_CFM_SEC_SECSTAT                (0x01 << 30)
# define HAL_MCFxxxx_CFM_SEC_SEC_MASK               (0x0FFFF << 0)
# define HAL_MCFxxxx_CFM_SEC_SEC_SHIFT              0
# define HAL_MCFxxxx_CFM_SEC_SEC_MAGIC              0x4AC8

// The PROT protection register is just 32 bits, each bit protecting
// one flash sector. 0 means not protected.

// The SACC supervisor access register similarly is 32 bits controlling
// access to the flash. 0 allows access in both user and supervisor modes.

// The DACC data access register is 32 bits determining whether or not
// each sector can contain code. 0 means code is possible.

# define HAL_MCFxxxx_CFM_USTAT_CBEIF                (0x01 << 7)
# define HAL_MCFxxxx_CFM_USTAT_CCIF                 (0x01 << 6)
# define HAL_MCFxxxx_CFM_USTAT_PVIOL                (0x01 << 5)
# define HAL_MCFxxxx_CFM_USTAT_ACCERR               (0x01 << 4)
# define HAL_MCFxxxx_CFM_USTAT_BLANK                (0x01 << 2)

# define HAL_MCFxxxx_CFM_CMD_RDARY1                 0x05
# define HAL_MCFxxxx_CFM_CMD_PGERSVER               0x06
# define HAL_MCFxxxx_CFM_CMD_PGM                    0x20
# define HAL_MCFxxxx_CFM_CMD_PGERS                  0x40
# define HAL_MCFxxxx_CFM_CMD_MASERS                 0x41

# if !defined(__ASSEMBLER__) && !defined(__LDI__)
// On reset some of the CFM settings are initialized from a structure
// @ offset 0x00000400 in the flash. When booting from the internal
// flash the FLASHBAR register is initialized to address 0, read-only
// and valid. Presumably it is expected that the first 1K will be
// filled with exception vectors, so the next 24 bytes are used for
// flash settings. Real code can follow afterwards.
typedef struct hal_mcfxxxx_cfm_security_settings {
    cyg_uint64      cfm_backdoor_key;
    cyg_uint32      cfm_prot;
    cyg_uint32      cfm_sacc;
    cyg_uint32      cfm_dacc;
    cyg_uint32      cfm_sec;
} hal_mcfxxxx_cfm_security_settings;
# endif

#endif  // HAL_MCFxxxx_HAS_MCF5282_CFM

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_INTC
// Support one or more INTC controllers, depending on the number of
// interrup sources.

// Two 32-bit interrupt pending registers
# define HAL_MCFxxxx_INTCx_IPRH                     0x0000
# define HAL_MCFxxxx_INTCx_IPRL                     0x0004
// Two 32-bit interrupt mask registers
# define HAL_MCFxxxx_INTCx_IMRH                     0x0008
# define HAL_MCFxxxx_INTCx_IMRL                     0x000C
// Two 32-bit interrupt force registers
# define HAL_MCFxxxx_INTCx_INTFRCH                  0x0010
# define HAL_MCFxxxx_INTCx_INTFRCL                  0x0014
// 8-bit interrupt request level register
# define HAL_MCFxxxx_INTCx_IRLR                     0x0018
// 8-bit interrupt acknowledge level/priority register
# define HAL_MCFxxxx_INTCx_IACKLPR                  0x0019
// 64 8-bit interrupt control registers, determining the priorities
// of each interrupt source. ICR00 is actually invalid but useful for
// array indexing.
# define HAL_MCFxxxx_INTCx_ICR00                    0x0040
// 8-bit ACK registers. These can be checked in an interrupt handler
// to chain, avoiding the overheads of another interrupt, but at
// the cost of a couple of extra cycles even when no other interrupts
// are pending. Unless the interrupt load is very heavy this is
// likely to be a bad trade off.
# define HAL_MCFxxxx_INTCx_SWIACK                   0x00E0
# define HAL_MCFxxxx_INTCx_L1IACK                   0x00E4
# define HAL_MCFxxxx_INTCx_L2IACK                   0x00E8
# define HAL_MCFxxxx_INTCx_L3IACK                   0x00EC
# define HAL_MCFxxxx_INTCx_L4IACK                   0x00F0
# define HAL_MCFxxxx_INTCx_L5IACK                   0x00F4
# define HAL_MCFxxxx_INTCx_L6IACK                   0x00F8
# define HAL_MCFxxxx_INTCx_L7IACK                   0x00FC

// Global IACK registers, all 8-bits
# define HAL_MCFxxxx_INTC_GSWACKR                   0x0FE0
# define HAL_MCFxxxx_INTC_GL1IACKR                  0x0FE4
# define HAL_MCFxxxx_INTC_GL2IACKR                  0x0FE8
# define HAL_MCFxxxx_INTC_GL3IACKR                  0x0FEC
# define HAL_MCFxxxx_INTC_GL4IACKR                  0x0FF0
# define HAL_MCFxxxx_INTC_GL5IACKR                  0x0FF4
# define HAL_MCFxxxx_INTC_GL6IACKR                  0x0FF8
# define HAL_MCFxxxx_INTC_GL7IACKR                  0x0FFC

// The pending, mask and force registers are just 32 bits with one bit
// per interrupt source.
//
// The IRLR registers is just 8 bits with one bit per interrupt
// priority level. Priority level 0 corresponds to no pending
// interrupts, of course.

# define HAL_MCFxxxx_INTCx_IACKLPR_LEVEL_MASK       (0x07 << 4)
# define HAL_MCFxxxx_INTCx_IACKLPR_LEVEL_SHIFT      4
# define HAL_MCFxxxx_INTCx_IACKLPR_LEVEL_PRI_MASK   (0x0F << 0)
# define HAL_MCFxxxx_INTCx_IACKLPR_LEVEL_PRI_SHIFT  0

# define HAL_MCFxxxx_INTCx_ICRxx_IL_MASK            (0x07 << 3)
# define HAL_MCFxxxx_INTCx_ICRxx_IL_SHIFT           3
# define HAL_MCFxxxx_INTCx_ICRxx_IP_MASK            (0x07 << 0)
# define HAL_MCFxxxx_INTCx_ICRxx_IP_SHIFT           0

# ifdef HAL_MCFxxxx_HAS_INTC_SIMCIMETC
#  undef  HAL_MCFxxxx_INTCx_IACKLPR
#  define HAL_MCFxxxx_INTCx_ICONFIG                 0x0000001A
#  define HAL_MCFxxxx_INTCx_SIMR                    0x0000001C
#  define HAL_MCFxxxx_INTCx_CIMR                    0x0000001D
#  define HAL_MCFxxxx_INTCx_CLMASK                  0x0000001E
#  define HAL_MCFxxxx_INTCx_SLMASK                  0x0000001F

#  define HAL_MCFxxxx_INTCx_ICONFIG_ELVLPRI_MASK    (0x7F << 9)
#  define HAL_MCFxxxx_INTCx_ICONFIG_ELVLPRI_SHIFT   9
#  define HAL_MCFxxxx_INTCx_ICONFIG_EMASK           (0x01 << 5)
# endif

#endif  // HAL_MCFxxxx_HAS_MCF5282_INTC

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_EPORT
// Edge port module, pins which can be configured to trigger interrupts.

// Pin assignment register, 16 bits
# define HAL_MCFxxxx_EPORTx_EPPAR                   0x0000
// Data direction register, 8 bits
# define HAL_MCFxxxx_EPORTx_EPDDR                   0x0002
// Interrupt enable register, 8 bits
# define HAL_MCFxxxx_EPORTx_EPIER                   0x0003
// Data register (output), 8 bits
# define HAL_MCFxxxx_EPORTx_EPDR                    0x0004
// Pin data register (input), 8 bits
# define HAL_MCFxxxx_EPORTx_EPPDR                   0x0005
// Flag register
# define HAL_MCFxxxx_EPORTx_EPFR                    0x0006

// EPPAR is split into 2 bits per pin, the others use 1 bit per pin
// with bit 0 unused.
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA7_MASK        (0x03 << 14)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA7_SHIFT       14
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA6_MASK        (0x03 << 12)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA6_SHIFT       12
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA5_MASK        (0x03 << 10)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA5_SHIFT       10
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA4_MASK        (0x03 << 8)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA4_SHIFT       8
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA3_MASK        (0x03 << 6)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA3_SHIFT       6
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA2_MASK        (0x03 << 4)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA2_SHIFT       4
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA1_MASK        (0x03 << 2)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA1_SHIFT       2
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA0_MASK        (0x03 << 0)
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPA0_SHIFT       0

# define HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_LEVEL       0x00
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_RISING      0x01
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_FALLING     0x02
# define HAL_MCFxxxx_EPORTx_EPPAR_EPPAx_BOTH        0x03

#endif  // HAL_MCFxxxx_HAS_MCF5282_EPORT

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_DMA
// DMA. Support an arbitrary number of channels, although
// the DMA_REQC register effectively limits the number to 8.

// DMA request controller, 1 32-bit register. This is assumed relative
// to SCM. It is in addition to the per-channel register definitions.
# define HAL_MCFxxxx_DMA_REQC                       0x00000014

#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 7)
# define HAL_MCFxxxx_DMA_REQC_DMAC7_MASK            (0x0F << 28)
# define HAL_MCFxxxx_DMA_REQC_DMAC7_SHIFT           28
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 6)
# define HAL_MCFxxxx_DMA_REQC_DMAC6_MASK            (0x0F << 24)
# define HAL_MCFxxxx_DMA_REQC_DMAC6_SHIFT           24
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 5)
# define HAL_MCFxxxx_DMA_REQC_DMAC5_MASK            (0x0F << 20)
# define HAL_MCFxxxx_DMA_REQC_DMAC5_SHIFT           20
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 4)
# define HAL_MCFxxxx_DMA_REQC_DMAC4_MASK            (0x0F << 16)
# define HAL_MCFxxxx_DMA_REQC_DMAC4_SHIFT           16
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 3)
# define HAL_MCFxxxx_DMA_REQC_DMAC3_MASK            (0x0F << 12)
# define HAL_MCFxxxx_DMA_REQC_DMAC3_SHIFT           12
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 2)
# define HAL_MCFxxxx_DMA_REQC_DMAC2_MASK            (0x0F << 8)
# define HAL_MCFxxxx_DMA_REQC_DMAC2_SHIFT           8
#endif
#if (HAL_MCFxxxx_HAS_MCF5282_DMA > 1)
# define HAL_MCFxxxx_DMA_REQC_DMAC1_MASK            (0x0F << 4)
# define HAL_MCFxxxx_DMA_REQC_DMAC1_SHIFT           4
#endif
# define HAL_MCFxxxx_DMA_REQC_DMAC0_MASK            (0x0F << 0)
# define HAL_MCFxxxx_DMA_REQC_DMAC0_SHIFT           0

// Each DMA channel has 32-bit source, destination and control
// registers, a count register (up to 24 bits), and a 1-byte status
// register
# define HAL_MCFxxxx_DMAx_SARx                      0x00000000
# define HAL_MCFxxxx_DMAx_DARx                      0x00000004
# define HAL_MCFxxxx_DMAx_DCRx                      0x00000008
# define HAL_MCFxxxx_DMAx_BCRx                      0x0000000C
# define HAL_MCFxxxx_DMAx_DSRx                      0x00000010

# define HAL_MCFxxxx_DMAx_DCRx_INT                  (0x01 << 31)
# define HAL_MCFxxxx_DMAx_DCRx_EEXT                 (0x01 << 30)
# define HAL_MCFxxxx_DMAx_DCRx_CS                   (0x01 << 29)
# define HAL_MCFxxxx_DMAx_DCRx_AA                   (0x01 << 28)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_MASK             (0x07 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_SHIFT            25
# define HAL_MCFxxxx_DMAx_DCRx_BWC_PRIORITY         (0x00 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_512_16384        (0x01 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_1024_32768       (0x02 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_2048_65536       (0x03 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_4096_131072      (0x04 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_8192_262144      (0x05 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_16384_524288     (0x06 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_BWC_32678_1048576    (0x07 << 25)
# define HAL_MCFxxxx_DMAx_DCRx_SINC                 (0x01 << 22)
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_MASK           (0x03 << 20)
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_SHIFT          20
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_LONGWORD       (0x00 << 20)
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_BYTE           (0x01 << 20)
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_WORD           (0x02 << 20)
# define HAL_MCFxxxx_DMAx_DCRx_SSIZE_LINE           (0x03 << 20)
# define HAL_MCFxxxx_DMAx_DCRx_DINC                 (0x01 << 19)
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_MASK           (0x03 << 17)
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_SHIFT          17
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_LONGWORD       (0x00 << 17)
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_BYTE           (0x01 << 17)
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_WORD           (0x02 << 17)
# define HAL_MCFxxxx_DMAx_DCRx_DSIZE_LINE           (0x03 << 17)
# define HAL_MCFxxxx_DMAx_DCRx_START                (0x01 << 16)
# define HAL_MCFxxxx_DMAx_DCRx_AT                   (0x01 << 15)

# define HAL_MCFxxxx_DMAx_DSRx_CE                   (0x01 << 6)
# define HAL_MCFxxxx_DMAx_DSRx_BES                  (0x01 << 5)
# define HAL_MCFxxxx_DMAx_DSRx_BED                  (0x01 << 4)
# define HAL_MCFxxxx_DMAx_DSRx_REQ                  (0x01 << 2)
# define HAL_MCFxxxx_DMAx_DSRx_BSY                  (0x01 << 1)
# define HAL_MCFxxxx_DMAx_DSRx_DONE                 (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_DMA

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_PIT
// Programmable interrupt timer modules

# define HAL_MCFxxxx_PITx_PCSR                      0x00000000
# define HAL_MCFxxxx_PITx_PMR                       0x00000002
# define HAL_MCFxxxx_PITx_PCNTR                     0x00000004

# define HAL_MCFxxxx_PITx_PCSR_PRE_MASK             (0x0F < 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_SHIFT            8
# define HAL_MCFxxxx_PITx_PCSR_PRE_2                (0x00 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_4                (0x01 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_8                (0x02 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_16               (0x03 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_32               (0x04 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_64               (0x05 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_128              (0x06 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_256              (0x07 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_512              (0x08 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_1024             (0x09 << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_2048             (0x0A << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_4096             (0x0B << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_8192             (0x0C << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_16384            (0x0D << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_32768            (0x0E << 8)
# define HAL_MCFxxxx_PITx_PCSR_PRE_65536            (0x0F << 8)
# define HAL_MCFxxxx_PITx_PCSR_DOZE                 (0x01 << 6)
# define HAL_MCFxxxx_PITx_PCSR_HALTED               (0x01 << 5)
# define HAL_MCFxxxx_PITx_PCSR_DBG                  HAL_MCFxxxx_PITx_PCSR_HALTED
# define HAL_MCFxxxx_PITx_PCSR_OVW                  (0x01 << 4)
# define HAL_MCFxxxx_PITx_PCSR_PIE                  (0x01 << 3)
# define HAL_MCFxxxx_PITx_PCSR_PIF                  (0x01 << 2)
# define HAL_MCFxxxx_PITx_PCSR_RLD                  (0x01 << 1)
# define HAL_MCFxxxx_PITx_PCSR_EN                   (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_PIT

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_GPT

// General purpose timer modules
// 8-bit input capture/output compare register
# define HAL_MCFxxxx_GPTx_IOS                       0x0000
// 8-bit compare force register
# define HAL_MCFxxxx_GPTx_CFORC                     0x0001
// 8-bit output compare 3 mask register
# define HAL_MCFxxxx_GPTx_OC3M                      0x0002
// 8-bit output compare 3 data register
# define HAL_MCFxxxx_GPTx_OC3D                      0x0003
// 16-bit counter registers
# define HAL_MCFxxxx_GPTx_CNT                       0x0004
// 8-bit system control register 1
# define HAL_MCFxxxx_GPTx_SCR1                      0x0006
// 8-bit toggle-on-overflow register
# define HAL_MCFxxxx_GPTx_TOV                       0x0008
// 8-bit control register 1
# define HAL_MCFxxxx_GPTx_CTL1                      0x0009
// 8-bit control register 2
# define HAL_MCFxxxx_GPTx_CTL2                      0x000B
// 8-bit interrupt enable register
# define HAL_MCFxxxx_GPTx_IE                        0x000C
// 8-bit system control register 2
# define HAL_MCFxxxx_GPTx_SCR2                      0x000D
// 8-bit flag registers
# define HAL_MCFxxxx_GPTx_FLG1                      0x000E
# define HAL_MCFxxxx_GPTx_FLG2                      0x000F
// 16-bit channel registers
# define HAL_MCFxxxx_GPTx_C0                        0x0010
# define HAL_MCFxxxx_GPTx_C1                        0x0012
# define HAL_MCFxxxx_GPTx_C2                        0x0014
# define HAL_MCFxxxx_GPTx_C3                        0x0016
// 8-bit pulse accumulator control register
# define HAL_MCFxxxx_GPTx_PACTL                     0x0018
// 8-bit pulse accumulator flag register
# define HAL_MCFxxxx_GPTx_PAFLG                     0x0019
// 16-bit pulse accumulator counter register
# define HAL_MCFxxxx_GPTx_PACNT                     0x001A
// 8-bit port data registers
# define HAL_MCFxxxx_GPTx_PORT                      0x001D
// 8-bit port data direction register
# define HAL_MCFxxxx_GPTx_DDR                       0x001E

# define HAL_MCFxxxx_GPTx_SCR1_GPTEN                (0x01 << 7)
# define HAL_MCFxxxx_GPTx_SCR1_TFFCA                (0x01 << 4)

# define HAL_MCFxxxx_GPTx_SCR2_TOI                  (0x01 << 7)
# define HAL_MCFxxxx_GPTx_SCR2_PUPT                 (0x01 << 5)
# define HAL_MCFxxxx_GPTx_SCR2_RDPT                 (0x01 << 4)
# define HAL_MCFxxxx_GPTx_SCR2_TCRE                 (0x01 << 3)
# define HAL_MCFxxxx_GPTx_SCR2_PR_MASK              (0x07 << 0)
# define HAL_MCFxxxx_GPTx_SCR2_PR_SHIFT             0
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV1              0x00
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV2              0x01
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV4              0x02
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV8              0x03
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV16             0x04
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV32             0x05
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV64             0x06
# define HAL_MCFxxxx_GPTx_SCR2_PR_DIV128            0x07

# define HAL_MCFxxxx_GPTx_FLG2_TOF                  (0x01 << 7)

# define HAL_MCFxxxx_GPTx_PACTL_PAE                 (0x01 << 6)
# define HAL_MCFxxxx_GPTx_PACTL_PAMOD               (0x01 << 5)
# define HAL_MCFxxxx_GPTx_PACTL_PEDGE               (0x01 << 4)
# define HAL_MCFxxxx_GPTx_PACTL_CLK_MASK            (0x03 << 2)
# define HAL_MCFxxxx_GPTx_PACTL_CLK_SHIFT           2
# define HAL_MCFxxxx_GPTx_PACTL_CLK_PRESCALER       (0x00 << 2)
# define HAL_MCFxxxx_GPTx_PACTL_CLK_PACLK           (0x01 << 2)
# define HAL_MCFxxxx_GPTx_PACTL_CLK_PACLK_256       (0x02 << 2)
# define HAL_MCFxxxx_GPTx_PACTL_CLK_PACLK_65536     (0x03 << 2)
# define HAL_MCFxxxx_GPTx_PACTL_PAOVI               (0x01 << 1)
# define HAL_MCFxxxx_GPTx_PACTL_PAI                 (0x01 << 0)

# define HAL_MCFxxxx_GPTx_PAFLG_PAOVF               (0x01 << 1)
# define HAL_MCFxxxx_GPTx_PAFLG_PAIF                (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_GPT

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_DTMR

// DMA timers
// 16-bit timer mode registers
# define HAL_MCFxxxx_DTMRx                          0x0000
// 8-bit extended mode registers
# define HAL_MCFxxxx_DTXMRx                         0x0002
// 8-bit event registers
# define HAL_MCFxxxx_DTERx                          0x00003
// 32-bit reference, capture and counter registers
# define HAL_MCFxxxx_DTRRx                          0x0004
# define HAL_MCFxxxx_DTCRx                          0x0008
# define HAL_MCFxxxx_DTCNx                          0x000C

# define HAL_MCFxxxx_DTMRx_PS_MASK                  (0x0FF << 8)
# define HAL_MCFxxxx_DTMRx_PS_SHIFT                 8
# define HAL_MCFxxxx_DTMRx_CE_MASK                  (0x03 << 6)
# define HAL_MCFxxxx_DTMRx_CE_SHIFT                 6
# define HAL_MCFxxxx_DTMRx_CE_DISABLE               (0x00 << 6)
# define HAL_MCFxxxx_DTMRx_CE_RISING                (0x01 << 6)
# define HAL_MCFxxxx_DTMRx_CE_FALLING               (0x02 << 6)
# define HAL_MCFxxxx_DTMRx_CE_ANY                   (0x03 << 6)
# define HAL_MCFxxxx_DTMRx_OM                       (0x01 << 5)
# define HAL_MCFxxxx_DTMRx_ORRI                     (0x01 << 4)
# define HAL_MCFxxxx_DTMRx_FRR                      (0x01 << 3)
# define HAL_MCFxxxx_DTMRx_CLK_MASK                 (0x03 << 1)
# define HAL_MCFxxxx_DTMRx_CLK_SHIFT                1
# define HAL_MCFxxxx_DTMRx_CLK_STOP                 (0x00 << 1)
# define HAL_MCFxxxx_DTMRx_CLK_DIV_1                (0x01 << 1)
# define HAL_MCFxxxx_DTMRx_CLK_DIV_16               (0x02 << 1)
# define HAL_MCFxxxx_DTMRx_CLK_DTINn                (0x03 << 1)
# define HAL_MCFxxxx_DTMRx_RST                      (0x01 << 0)

# define HAL_MCFxxxx_DTXMRx_DMAEN                   (0x01 << 7)
# define HAL_MCFxxxx_DTXMRx_MODE16                  (0x01 << 1)

# define HAL_MCFxxxx_DTERx_REF                      (0x01 << 1)
# define HAL_MCFxxxx_DTERx_CAP                      (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_DTMR
    
// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_ETH
// Ethernet. Allow for multiple devices.

# define HAL_MCFxxxx_ETHx_EIR                       0x0004
# define HAL_MCFxxxx_ETHx_EIMR                      0x0008
# define HAL_MCFxxxx_ETHx_RDAR                      0x0010
# define HAL_MCFxxxx_ETHx_TDAR                      0x0014
# define HAL_MCFxxxx_ETHx_ECR                       0x0024
# define HAL_MCFxxxx_ETHx_MDATA                     0x0040
# define HAL_MCFxxxx_ETHx_MMFR                      0x0040
# define HAL_MCFxxxx_ETHx_MSCR                      0x0044
# define HAL_MCFxxxx_ETHx_MIBC                      0x0064
# define HAL_MCFxxxx_ETHx_RCR                       0x0084
# define HAL_MCFxxxx_ETHx_TCR                       0x00C4
# define HAL_MCFxxxx_ETHx_PALR                      0x00E4
# define HAL_MCFxxxx_ETHx_PAUR                      0x00E8
# define HAL_MCFxxxx_ETHx_OPD                       0x00EC
# define HAL_MCFxxxx_ETHx_IAUR                      0x0118
# define HAL_MCFxxxx_ETHx_IALR                      0x011C
# define HAL_MCFxxxx_ETHx_GAUR                      0x0120
# define HAL_MCFxxxx_ETHx_GALR                      0x0124
# define HAL_MCFxxxx_ETHx_TFWR                      0x0144
# define HAL_MCFxxxx_ETHx_FRBR                      0x014C
# define HAL_MCFxxxx_ETHx_FRSR                      0x0150
# define HAL_MCFxxxx_ETHx_ERDSR                     0x0180
# define HAL_MCFxxxx_ETHx_ETDSR                     0x0184
# define HAL_MCFxxxx_ETHx_EMRBR                     0x0188

# define HAL_MCFxxxx_ETHx_RMON_T_DROP               0x0200
# define HAL_MCFxxxx_ETHx_RMON_T_PACKETS            0x0204
# define HAL_MCFxxxx_ETHx_RMON_T_BC_PKT             0x0208
# define HAL_MCFxxxx_ETHx_RMON_T_MC_PKT             0x020C
# define HAL_MCFxxxx_ETHx_RMON_T_CRC_ALIGN          0x0210
# define HAL_MCFxxxx_ETHx_RMON_T_UNDERSIZE          0x0214
# define HAL_MCFxxxx_ETHx_RMON_T_OVERSIZE           0x0218
# define HAL_MCFxxxx_ETHx_RMON_T_FRAG               0x021C
# define HAL_MCFxxxx_ETHx_RMON_T_JAB                0x0220
# define HAL_MCFxxxx_ETHx_RMON_T_COL                0x0224
# define HAL_MCFxxxx_ETHx_RMON_T_P64                0x0228
# define HAL_MCFxxxx_ETHx_RMON_T_P65TO127           0x022C
# define HAL_MCFxxxx_ETHx_RMON_T_P128TO255          0x0230
# define HAL_MCFxxxx_ETHx_RMON_T_P256TO511          0x0234
# define HAL_MCFxxxx_ETHx_RMON_T_P512TO1023         0x0238
# define HAL_MCFxxxx_ETHx_RMON_T_P1024TO2047        0x023C
# define HAL_MCFxxxx_ETHx_RMON_T_PGTE2048           0x0240
# define HAL_MCFxxxx_ETHx_RMON_T_OCTETS             0x0244
# define HAL_MCFxxxx_ETHx_IEEE_T_DROP               0x0248
# define HAL_MCFxxxx_ETHx_IEEE_T_FRAME_OK           0x024C
# define HAL_MCFxxxx_ETHx_IEEE_T_1COL               0x0250
# define HAL_MCFxxxx_ETHx_IEEE_T_MCOL               0x0254
# define HAL_MCFxxxx_ETHx_IEEE_T_DEF                0x0258
# define HAL_MCFxxxx_ETHx_IEEE_T_LCOL               0x025C
# define HAL_MCFxxxx_ETHx_IEEE_T_EXCOL              0x0260
# define HAL_MCFxxxx_ETHx_IEEE_T_MACERR             0x0264
# define HAL_MCFxxxx_ETHx_IEEE_T_CSERR              0x0268
# define HAL_MCFxxxx_ETHx_IEEE_T_SQE                0x026C
# define HAL_MCFxxxx_ETHx_IEEE_T_FDXFC              0x0270
# define HAL_MCFxxxx_ETHx_IEEE_T_OCTETS_OK          0x0274
# define HAL_MCFxxxx_ETHx_RMON_R_PACKETS            0x0284
# define HAL_MCFxxxx_ETHx_RMON_R_BC_PKT             0x0288
# define HAL_MCFxxxx_ETHx_RMON_R_MC_PKT             0x028C
# define HAL_MCFxxxx_ETHx_RMON_R_CRC_ALIGN          0x0290
# define HAL_MCFxxxx_ETHx_RMON_R_UNDERSIZE          0x0294
# define HAL_MCFxxxx_ETHx_RMON_R_OVERSIZE           0x0298
# define HAL_MCFxxxx_ETHx_RMON_R_FRAG               0x029C
# define HAL_MCFxxxx_ETHx_RMON_R_JAB                0x02A0
# define HAL_MCFxxxx_ETHx_RMON_R_RESVD_0            0x02A4
# define HAL_MCFxxxx_ETHx_RMON_R_P64                0x02A8
# define HAL_MCFxxxx_ETHx_RMON_R_P65TO127           0x02AC
# define HAL_MCFxxxx_ETHx_RMON_R_P128TO255          0x02B0
# define HAL_MCFxxxx_ETHx_RMON_R_P256TO511          0x02B4
# define HAL_MCFxxxx_ETHx_RMON_R_P512TO1023         0x02B8
# define HAL_MCFxxxx_ETHx_RMON_R_P1024TO2047        0x02BC
# define HAL_MCFxxxx_ETHx_RMON_R_GTE2048            0x02C0
# define HAL_MCFxxxx_ETHx_RMON_R_OCTETS             0x02C4
# define HAL_MCFxxxx_ETHx_IEEE_R_DROP               0x02C8
# define HAL_MCFxxxx_ETHx_IEEE_R_FRAME_OK           0x02CC
# define HAL_MCFxxxx_ETHx_IEEE_R_CRC                0x02D0
# define HAL_MCFxxxx_ETHx_IEEE_R_ALIGN              0x02D4
# define HAL_MCFxxxx_ETHx_IEEE_R_MACERR             0x02D8
# define HAL_MCFxxxx_ETHx_IEEE_R_FDXFC              0x02DC
# define HAL_MCFxxxx_ETHx_IEEE_R_OCTETS_OK          0x02E0

# define HAL_MCFxxxx_ETHx_EIR_HBERR                 (0x01 << 31)
# define HAL_MCFxxxx_ETHx_EIR_BABR                  (0x01 << 30)
# define HAL_MCFxxxx_ETHx_EIR_BABT                  (0x01 << 29)
# define HAL_MCFxxxx_ETHx_EIR_GRA                   (0x01 << 28)
# define HAL_MCFxxxx_ETHx_EIR_TXF                   (0x01 << 27)
# define HAL_MCFxxxx_ETHx_EIR_TXB                   (0x01 << 26)
# define HAL_MCFxxxx_ETHx_EIR_RXF                   (0x01 << 25)
# define HAL_MCFxxxx_ETHx_EIR_RXB                   (0x01 << 24)
# define HAL_MCFxxxx_ETHx_EIR_MII                   (0x01 << 23)
# define HAL_MCFxxxx_ETHx_EIR_EBERR                 (0x01 << 22)
# define HAL_MCFxxxx_ETHx_EIR_LC                    (0x01 << 21)
# define HAL_MCFxxxx_ETHx_EIR_RL                    (0x01 << 20)
# define HAL_MCFxxxx_ETHx_EIR_UN                    (0x01 << 19)

# define HAL_MCFxxxx_ETHx_EIMR_HBERR                HAL_MCFxxxx_ETHx_EIR_HBERR
# define HAL_MCFxxxx_ETHx_EIMR_BABR                 HAL_MCFxxxx_ETHx_EIR_BABR
# define HAL_MCFxxxx_ETHx_EIMR_BABT                 HAL_MCFxxxx_ETHx_EIR_BABT
# define HAL_MCFxxxx_ETHx_EIMR_GRA                  HAL_MCFxxxx_ETHx_EIR_GRA
# define HAL_MCFxxxx_ETHx_EIMR_TXF                  HAL_MCFxxxx_ETHx_EIR_TXF
# define HAL_MCFxxxx_ETHx_EIMR_TXB                  HAL_MCFxxxx_ETHx_EIR_TXB
# define HAL_MCFxxxx_ETHx_EIMR_RXF                  HAL_MCFxxxx_ETHx_EIR_RXF
# define HAL_MCFxxxx_ETHx_EIMR_RXB                  HAL_MCFxxxx_ETHx_EIR_RXB
# define HAL_MCFxxxx_ETHx_EIMR_MII                  HAL_MCFxxxx_ETHx_EIR_MII
# define HAL_MCFxxxx_ETHx_EIMR_EBERR                HAL_MCFxxxx_ETHx_EIR_EBERR
# define HAL_MCFxxxx_ETHx_EIMR_LC                   HAL_MCFxxxx_ETHx_EIR_LC
# define HAL_MCFxxxx_ETHx_EIMR_RL                   HAL_MCFxxxx_ETHx_EIR_RL
# define HAL_MCFxxxx_ETHx_EIMR_UN                   HAL_MCFxxxx_ETHx_EIR_UN

# define HAL_MCFxxxx_ETHx_RDAR_R_DES_ACTIVE         (0x01 << 24)
# define HAL_MCFxxxx_ETHx_TDAR_X_DES_ACTIVE         (0x01 << 24)

# define HAL_MCFxxxx_ETHx_ECR_ETHER_EN              (0x01 << 1)
# define HAL_MCFxxxx_ETHx_ECR_RESET                 (0x01 << 0)

# define HAL_MCFxxxx_ETHx_MMFR_ST_MASK              (0x03 << 30)
# define HAL_MCFxxxx_ETHx_MMFR_ST_SHIFT             30
# define HAL_MCFxxxx_ETHx_MMFR_ST_VALUE             (0x01 << 30)
# define HAL_MCFxxxx_ETHx_MMFR_OP_MASK              (0x03 << 28)
# define HAL_MCFxxxx_ETHx_MMFR_OP_READ              (0x02 << 28)
# define HAL_MCFxxxx_ETHx_MMFR_OP_WRITE             (0x01 << 28)
# define HAL_MCFxxxx_ETHx_MMFR_PA_MASK              (0x1F << 23)
# define HAL_MCFxxxx_ETHx_MMFR_PA_SHIFT             23
# define HAL_MCFxxxx_ETHx_MMFR_RA_MASK              (0x1F << 18)
# define HAL_MCFxxxx_ETHx_MMFR_RA_SHIFT             18
# define HAL_MCFxxxx_ETHx_MMFR_TA_MASK              (0x03 << 16)
# define HAL_MCFxxxx_ETHx_MMFR_TA_SHIFT             16
# define HAL_MCFxxxx_ETHx_MMFR_TA_VALUE             (0x02 << 16)
# define HAL_MCFxxxx_ETHx_MMFR_DATA_MASK            (0x0FFFF << 0)
# define HAL_MCFxxxx_ETHx_MMFR_DATA_SHIFT           0

# define HAL_MCFxxxx_ETHx_MSCR_DIS_PREAMBLE         (0x01 << 7)
# define HAL_MCFxxxx_ETHx_MSCR_MII_SPEED_MASK       (0x3F << 1)
# define HAL_MCFxxxx_ETHx_MSCR_MII_SPEED_SHIFT      1

# define HAL_MCFxxxx_ETHx_MIBC_MIB_DISABLE          (0x01 << 31)
# define HAL_MCFxxxx_ETHx_MIBC_MIB_IDLE             (0x01 << 30)

# define HAL_MCFxxxx_ETHx_RCR_MAX_FL_MASK           (0x03FFF << 16)
# define HAL_MCFxxxx_ETHx_RCR_MAX_FL_SHIFT          16
# define HAL_MCFxxxx_ETHx_RCR_MAX_FL_VALUE          (1518 << 16)
# define HAL_MCFxxxx_ETHx_RCR_FCE                   (0x01 << 5)
# define HAL_MCFxxxx_ETHx_RCR_BC_REJ                (0x01 << 4)
# define HAL_MCFxxxx_ETHx_RCR_PROM                  (0x01 << 3)
# define HAL_MCFxxxx_ETHx_RCR_MII_MODE              (0x01 << 2)
# define HAL_MCFxxxx_ETHx_RCR_DRT                   (0x01 << 1)
# define HAL_MCFxxxx_ETHx_RCR_LOOP                  (0x01 << 0)

# define HAL_MCFxxxx_ETHx_TCR_RFC_PAUSE             (0x01 << 4)
# define HAL_MCFxxxx_ETHx_TCR_TFC_PAUSE             (0x01 << 3)
# define HAL_MCFxxxx_ETHx_TCR_FDEN                  (0x01 << 2)
# define HAL_MCFxxxx_ETHx_TCR_HBC                   (0x01 << 1)
# define HAL_MCFxxxx_ETHx_TCR_GTS                   (0x01 << 0)

# define HAL_MCFxxxx_ETHx_OPD_OPCODE_MASK           (0x0FFFF << 16)
# define HAL_MCFxxxx_ETHx_OPD_OPCODE_SHIFT          16
# define HAL_MCFxxxx_ETHx_OPD_PAUSE_DUR_MASK        (0x0FFFF << 0)
# define HAL_MCFxxxx_ETHx_OPD_PAUSE_DUR_SHIFT       0

# define HAL_MCFxxxx_ETHx_TFWR_X_WMRK_MASK          (0x03 << 0)
# define HAL_MCFxxxx_ETHx_TFWR_X_WMRK_SHIFT         0
# define HAL_MCFxxxx_ETHx_TFWR_X_WMRK_64            (0x00 << 0)
# define HAL_MCFxxxx_ETHx_TFWR_X_WMRK_128           (0x02 << 0)
# define HAL_MCFxxxx_ETHx_TFWR_X_WMRK_192           (0x03 << 0)

// This is the data structure for a buffer descriptor.
# if !defined( __ASSEMBLER__) && !defined(__LDI__)
typedef struct hal_mcfxxxx_eth_buffer_descriptor {
    cyg_uint16      ethbd_flags;
    cyg_uint16      ethbd_length;
    cyg_uint8*      ethbd_buffer;
} hal_mcfxxxx_eth_buffer_descriptor;
# endif

# define HAL_MCFxxxx_ETHx_RXBD_E                    (0x01 << 15)
# define HAL_MCFxxxx_ETHx_RXBD_RO1                  (0x01 << 14)
# define HAL_MCFxxxx_ETHx_RXBD_W                    (0x01 << 13)
# define HAL_MCFxxxx_ETHx_RXBD_RO2                  (0x01 << 12)
# define HAL_MCFxxxx_ETHx_RXBD_L                    (0x01 << 11)
# define HAL_MCFxxxx_ETHx_RXBD_M                    (0x01 << 8)
# define HAL_MCFxxxx_ETHx_RXBD_BC                   (0x01 << 7)
# define HAL_MCFxxxx_ETHx_RXBD_MC                   (0x01 << 6)
# define HAL_MCFxxxx_ETHx_RXBD_LG                   (0x01 << 5)
# define HAL_MCFxxxx_ETHx_RXBD_NO                   (0x01 << 4)
# define HAL_MCFxxxx_ETHx_RXBD_CR                   (0x01 << 2)
# define HAL_MCFxxxx_ETHx_RXBD_OV                   (0x01 << 1)
# define HAL_MCFxxxx_ETHx_RXBD_TR                   (0x01 << 0)

# define HAL_MCFxxxx_ETHx_TXBD_R                    (0x01 << 15)
# define HAL_MCFxxxx_ETHx_TXBD_TO1                  (0x01 << 14)
# define HAL_MCFxxxx_ETHx_TXBD_W                    (0x01 << 13)
# define HAL_MCFxxxx_ETHx_TXBD_TO2                  (0x01 << 12)
# define HAL_MCFxxxx_ETHx_TXBD_L                    (0x01 << 11)
# define HAL_MCFxxxx_ETHx_TXBD_TC                   (0x01 << 10)
# define HAL_MCFxxxx_ETHx_TXBD_ABC                  (0x01 << 9)

#endif  // HAL_MCFxxxx_HAS_MCF5282_ETH

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_QSPI

// QSPI
// Six 16-bit registers: mode, delay, wrap, interrupt, address and data
# define HAL_MCFxxxx_QSPIx_QMR                      0x0000
# define HAL_MCFxxxx_QSPIx_QDLYR                    0x0004
# define HAL_MCFxxxx_QSPIx_QWR                      0x0008
# define HAL_MCFxxxx_QSPIx_QIR                      0x000C
# define HAL_MCFxxxx_QSPIx_QAR                      0x0010
# define HAL_MCFxxxx_QSPIx_QDR                      0x0014

# define HAL_MCFxxxx_QSPIx_QMR_MSTR                 (0x01 << 15)
# define HAL_MCFxxxx_QSPIx_QMR_DOHIE                (0x01 << 14)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_MASK            (0x0F << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_SHIFT           10
# define HAL_MCFxxxx_QSPIx_QMR_BITS_16              (0x00 << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_8               (0x08 << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_9               (0x09 << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_10              (0x0A << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_11              (0x0B << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_12              (0x0C << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_13              (0x0D << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_14              (0x0E << 10)
# define HAL_MCFxxxx_QSPIx_QMR_BITS_15              (0x0F << 10)
# define HAL_MCFxxxx_QSPIx_QMR_CPOL                 (0x01 << 9)
# define HAL_MCFxxxx_QSPIx_QMR_CPHA                 (0x01 << 8)
# define HAL_MCFxxxx_QSPIx_QMR_BAUD_MASK            (0x0FF << 0)
# define HAL_MCFxxxx_QSPIx_QMR_BAUD_SHIFT           0

# define HAL_MCFxxxx_QSPIx_QDLYR_SPE                (0x01 << 15)
# define HAL_MCFxxxx_QSPIx_QDLYR_QCD_MASK           (0x07F << 8)
# define HAL_MCFxxxx_QSPIx_QDLYR_QCD_SHIFT          8
# define HAL_MCFxxxx_QSPIx_QDLYR_DTL_MASK           (0x0FF << 0)
# define HAL_MCFxxxx_QSPIx_QDLYR_DTL_SHIFT          0

# define HAL_MCFxxxx_QSPIx_QWR_HALT                 (0x01 << 15)
# define HAL_MCFxxxx_QSPIx_QWR_WREN                 (0x01 << 14)
# define HAL_MCFxxxx_QSPIx_QWR_WRTO                 (0x01 << 13)
# define HAL_MCFxxxx_QSPIx_QWR_CSIV                 (0x01 << 12)
# define HAL_MCFxxxx_QSPIx_QWR_ENDQP_MASK           (0x0F << 8)
# define HAL_MCFxxxx_QSPIx_QWR_ENDQP_SHIFT          8
# define HAL_MCFxxxx_QSPIx_QWR_CPTQP_MASK           (0x0F << 4)
# define HAL_MCFxxxx_QSPIx_QWR_CPTQP_SHIFT          4
# define HAL_MCFxxxx_QSPIx_QWR_NEWQP_MASK           (0x0F << 0)
# define HAL_MCFxxxx_QSPIx_QWR_NEWQP_SHIFT          0

# define HAL_MCFxxxx_QSPIx_QIR_WCEFB                (0x01 << 15)
# define HAL_MCFxxxx_QSPIx_QIR_ABRTB                (0x01 << 14)
# define HAL_MCFxxxx_QSPIx_QIR_ABRTL                (0x01 << 12)
# define HAL_MCFxxxx_QSPIx_QIR_WCEFE                (0x01 << 11)
# define HAL_MCFxxxx_QSPIx_QIR_ABRTE                (0x01 << 10)
# define HAL_MCFxxxx_QSPIx_QIR_SPIFE                (0x01 << 8)
# define HAL_MCFxxxx_QSPIx_QIR_WCEF                 (0x01 << 3)
# define HAL_MCFxxxx_QSPIx_QIR_ABRT                 (0x01 << 2)
# define HAL_MCFxxxx_QSPIx_QIR_SPIF                 (0x01 << 0)

# define HAL_MCFxxxx_QSPIx_QCRn_CONT                (0x01 << 15)
# define HAL_MCFxxxx_QSPIx_QCRn_BITSE               (0x01 << 14)
# define HAL_MCFxxxx_QSPIx_QCRn_DT                  (0x01 << 13)
# define HAL_MCFxxxx_QSPIx_QCRn_DSCK                (0x01 << 12)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_MASK        (0x0F << 8)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_SHIFT       8
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_CS0         (0x01 << 8)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_CS1         (0x02 << 8)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_CS2         (0x04 << 8)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_CS3         (0x08 << 8)
# define HAL_MCFxxxx_QSPI_QCRn_QSPI_CS_CSn(__n)     ((__n) << 8)
# define HAL_MCFxxxx_QSPIx_QCRn_QSPI_CS_CSn(__n)    ((__n) << 8)

# define HAL_MCFxxxx_QSPIx_QAR_TX_BASE              0x0000
# define HAL_MCFxxxx_QSPIx_QAR_RX_BASE              0x0010
# define HAL_MCFxxxx_QSPIx_QAR_COMMAND_BASE         0x0020

# define HAL_MCFxxxx_QSPIx_DEV_TO_BAUD(_dev_setting_arg_, _baud_)       \
    CYG_MACRO_START                                                     \
    cyg_uint32 _dev_setting_    = (cyg_uint32) (_dev_setting_arg_);     \
    cyg_uint32 _result_;                                                \
    if (0 == _dev_setting_) {                                           \
        _result_ = 0;                                                   \
    } else {                                                            \
        _result_ = CYGHWR_HAL_SYSTEM_CLOCK_HZ / (2 * _dev_setting_);    \
    }                                                                   \
    _baud_ = _result_;                                                  \
    CYG_MACRO_END

// When calculating a device setting, if the baud rate cannot be supported exactly
// then switch to the next slowest setting.
# define HAL_MCFxxxx_QSPIx_BAUD_TO_DEV(_baud_arg_, _dev_setting_)       \
    CYG_MACRO_START                                                     \
    cyg_uint32 _baud_   = (cyg_uint32) (_baud_arg_);                    \
    cyg_uint32 _result_;                                                \
    if (0 == _baud_) {                                                  \
        _result_ = 0;                                                   \
    } else {                                                            \
        _result_ = CYGHWR_HAL_SYSTEM_CLOCK_HZ / (2 * _baud_);           \
        if (_baud_ != (CYGHWR_HAL_SYSTEM_CLOCK_HZ / (2 * _result_))) {  \
            _result_++;                                                 \
        }                                                               \
        if (_result_ < 2) {                                             \
            _result_ = 2;                                               \
        } else if (_result_ > 255) {                                    \
            _result_ = 255;                                             \
        }                                                               \
    }                                                                   \
    _dev_setting_ = _result_;                                           \
    CYG_MACRO_END

#endif  // HAL_MCFxxxx_HAS_MCF5282_QSPI

// ----------------------------------------------------------------------------

#ifdef HAL_MCFxxxx_HAS_MCF5282_UART
// All registers are a single byte.
# define HAL_MCFxxxx_UARTx_UMR                      0x00
# define HAL_MCFxxxx_UARTx_USR                      0x04
# define HAL_MCFxxxx_UARTx_UCSR                     0x04
# define HAL_MCFxxxx_UARTx_UCR                      0x08
# define HAL_MCFxxxx_UARTx_URB                      0x0C
# define HAL_MCFxxxx_UARTx_UTB                      0x0C
# define HAL_MCFxxxx_UARTx_UIPCR                    0x10
# define HAL_MCFxxxx_UARTx_UACR                     0x10
# define HAL_MCFxxxx_UARTx_UISR                     0x14
# define HAL_MCFxxxx_UARTx_UIMR                     0x14
# define HAL_MCFxxxx_UARTx_UBG1                     0x18
# define HAL_MCFxxxx_UARTx_UBG2                     0x1C

# define HAL_MCFxxxx_UARTx_UIP                      0x34
# define HAL_MCFxxxx_UARTx_UOP1                     0x38
# define HAL_MCFxxxx_UARTx_UOP0                     0x3C

# define HAL_MCFxxxx_UARTx_UMR1_RXRTS               (0x01 << 7)
# define HAL_MCFxxxx_UARTx_UMR1_FFULL               (0x01 << 6)
# define HAL_MCFxxxx_UARTx_UMR1_ERR                 (0x01 << 5)
# define HAL_MCFxxxx_UARTx_UMR1_PM_MASK             (0x03 << 3)
# define HAL_MCFxxxx_UARTx_UMR1_PM_SHIFT            3
# define HAL_MCFxxxx_UARTx_UMR1_PM_WITH             (0x00 << 3)
# define HAL_MCFxxxx_UARTx_UMR1_PM_FORCE            (0x01 << 3)
# define HAL_MCFxxxx_UARTx_UMR1_PM_NO               (0x02 << 3)
# define HAL_MCFxxxx_UARTx_UMR1_PM_MULTIDROP        (0x03 << 3)
# define HAL_MCFxxxx_UARTx_UMR1_PT                  (0x01 << 2)
# define HAL_MCFxxxx_UARTx_UMR1_BC_MASK             (0x03 << 0)
# define HAL_MCFxxxx_UARTx_UMR1_BC_SHIFT            0
# define HAL_MCFxxxx_UARTx_UMR1_BC_5                (0x00 << 0)
# define HAL_MCFxxxx_UARTx_UMR1_BC_6                (0x01 << 0)
# define HAL_MCFxxxx_UARTx_UMR1_BC_7                (0x02 << 0)
# define HAL_MCFxxxx_UARTx_UMR1_BC_8                (0x03 << 0)

# define HAL_MCFxxxx_UARTx_UMR2_CM_MASK             (0x03 << 6)
# define HAL_MCFxxxx_UARTx_UMR2_CM_SHIFT            6
# define HAL_MCFxxxx_UARTx_UMR2_CM_NORMAL           (0x00 << 6)
# define HAL_MCFxxxx_UARTx_UMR2_CM_AUTO             (0x01 << 6)
# define HAL_MCFxxxx_UARTx_UMR2_CM_LOCAL            (0x02 << 6)
# define HAL_MCFxxxx_UARTx_UMR2_CM_REMOTE           (0x03 << 6)
# define HAL_MCFxxxx_UARTx_UMR2_TXRTS               (0x01 << 5)
# define HAL_MCFxxxx_UARTx_UMR2_TXCTS               (0x01 << 4)
# define HAL_MCFxxxx_UARTx_UMR2_SB_MASK             (0x0f << 0)
# define HAL_MCFxxxx_UARTx_UMR2_SB_SHIFT            0
// These two constants are only valid for 6-8 bits. 5 bit needs
// to be treated specially.
# define HAL_MCFxxxx_UARTx_UMR2_SB_1                (0x07 << 0)
# define HAL_MCFxxxx_UARTx_UMR2_SB_2                (0x0f << 0)

# define HAL_MCFxxxx_UARTx_USR_RB                   (0x01 << 7)
# define HAL_MCFxxxx_UARTx_USR_FE                   (0x01 << 6)
# define HAL_MCFxxxx_UARTx_USR_PE                   (0x01 << 5)
# define HAL_MCFxxxx_UARTx_USR_OE                   (0x01 << 4)
# define HAL_MCFxxxx_UARTx_USR_TXEMP                (0x01 << 3)
# define HAL_MCFxxxx_UARTx_USR_TXRDY                (0x01 << 2)
# define HAL_MCFxxxx_UARTx_USR_FFULL                (0x01 << 1)
# define HAL_MCFxxxx_UARTx_USR_RXRDY                (0x01 << 0)

# define HAL_MCFxxxx_UARTx_UCSR_RCS_MASK            (0x0f << 4)
# define HAL_MCFxxxx_UARTx_UCSR_RCS_SHIFT           4
# define HAL_MCFxxxx_UARTx_UCSR_RCS_CLKIN           (0x0D << 4)
# define HAL_MCFxxxx_UARTx_UCSR_RCS_DTINDIV16       (0x0E << 4)
# define HAL_MCFxxxx_UARTx_UCSR_RCS_DTIN            (0x0F << 4)
# define HAL_MCFxxxx_UARTx_UCSR_TCS_MASK            (0x0f << 0)
# define HAL_MCFxxxx_UARTx_UCSR_TCS_SHIFT           (0x0f << 0)
# define HAL_MCFxxxx_UARTx_UCSR_TCS_CLKIN           (0x0D << 0)
# define HAL_MCFxxxx_UARTx_UCSR_TCS_DTINDIV16       (0x0E << 0)
# define HAL_MCFxxxx_UARTx_UCSR_TCS_DTIN            (0x0F << 0)

# define HAL_MCFxxxx_UARTx_UCR_MISC_MASK            (0x07 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_SHIFT           4
# define HAL_MCFxxxx_UARTx_UCR_MISC_NOP             (0x00 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_RMRP            (0x01 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_RR              (0x02 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_RT              (0x03 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_RES             (0x04 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_RBCI            (0x05 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_STARTB          (0x06 << 4)
# define HAL_MCFxxxx_UARTx_UCR_MISC_STOPB           (0x07 << 4)
# define HAL_MCFxxxx_UARTx_UCR_TC_MASK              (0x03 << 2)
# define HAL_MCFxxxx_UARTx_UCR_TC_SHIFT             2
# define HAL_MCFxxxx_UARTx_UCR_TC_NOP               (0x00 << 2)
# define HAL_MCFxxxx_UARTx_UCR_TC_TE                (0x01 << 2)
# define HAL_MCFxxxx_UARTx_UCR_TC_TD                (0x02 << 2)
# define HAL_MCFxxxx_UARTx_UCR_RC_MASK              (0x03 << 0)
# define HAL_MCFxxxx_UARTx_UCR_RC_SHIFT             0
# define HAL_MCFxxxx_UARTx_UCR_RC_NOP               (0x00 << 0)
# define HAL_MCFxxxx_UARTx_UCR_RC_RE                (0x01 << 0)
# define HAL_MCFxxxx_UARTx_UCR_RC_RD                (0x02 << 0)

# define HAL_MCFxxxx_UARTx_UIPCR_COS                (0x01 << 4)
# define HAL_MCFxxxx_UARTx_UIPCR_CTS                (0x01 << 0)

# define HAL_MCFxxxx_UARTx_UACR_IEC                 (0x01 << 0)

// ABC, RXFIFO, TXFIFO and RXFTO are not always available
# define HAL_MCFxxxx_UARTx_UISR_COS                 (0x01 << 7)
# define HAL_MCFxxxx_UARTx_UISR_DB                  (0x01 << 2)
# define HAL_MCFxxxx_UARTx_UISR_FFUL                (0x01 << 1)
# define HAL_MCFxxxx_UARTx_UISR_RXRDY               (0x01 << 1)
# define HAL_MCFxxxx_UARTx_UISR_TXRDY               (0x01 << 0)

# define HAL_MCFxxxx_UARTx_UIMR_COS                 (0x01 << 7)
# define HAL_MCFxxxx_UARTx_UIMR_DB                  (0x01 << 2)
# define HAL_MCFxxxx_UARTx_UIMR_FFUL                (0x01 << 1)
# define HAL_MCFxxxx_UARTx_UIMR_RXRDY               (0x01 << 1)
# define HAL_MCFxxxx_UARTx_UIMR_TXRDY               (0x01 << 0)

# define HAL_MCFxxxx_UARTx_UIP_CTS                  (0x01 << 0)
# define HAL_MCFxxxx_UARTx_UOP_RTS                  (0x01 << 0)

// The baud rate depends on the system clock. There is no fractional
// precision register.
# define HAL_MCFxxxx_UARTx_SET_BAUD(_base_, _baud_)                                                                 \
    CYG_MACRO_START                                                                                                 \
    cyg_uint8   _udu_   = ((cyg_uint8) (((CYGHWR_HAL_SYSTEM_CLOCK_MHZ * 1000000) / (32 * (_baud_))) >> 8));         \
    cyg_uint8   _udl_   = ((cyg_uint8) (((CYGHWR_HAL_SYSTEM_CLOCK_MHZ * 1000000) / (32 * (_baud_))) & 0x00FF));     \
    HAL_WRITE_UINT8((_base_) + HAL_MCFxxxx_UARTx_UBG1, _udu_);                                                      \
    HAL_WRITE_UINT8((_base_) + HAL_MCFxxxx_UARTx_UBG2, _udl_);                                                      \
    CYG_MACRO_END

#endif  // HAL_MCFxxxx_HAS_MCF5282_UART

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_I2C
// I2C
// Five 8-bit registers: address, frequency divider, control, status, data
# define HAL_MCFxxxx_I2Cx_ADR                       0x0000
# define HAL_MCFxxxx_I2Cx_FDR                       0x0004
# define HAL_MCFxxxx_I2Cx_CR                        0x0008
# define HAL_MCFxxxx_I2Cx_SR                        0x000C
# define HAL_MCFxxxx_I2Cx_DR                        0x0010

# define HAL_MCFxxxx_I2Cx_CR_IEN                    (0x01 << 7)
# define HAL_MCFxxxx_I2Cx_CR_IIEN                   (0x01 << 6)
# define HAL_MCFxxxx_I2Cx_CR_MSTA                   (0x01 << 5)
# define HAL_MCFxxxx_I2Cx_CR_MTX                    (0x01 << 4)
# define HAL_MCFxxxx_I2Cx_CR_TXAK                   (0x01 << 3)
# define HAL_MCFxxxx_I2Cx_CR_RSTA                   (0x01 << 2)

# define HAL_MCFxxxx_I2Cx_SR_ICF                    (0x01 << 7)
# define HAL_MCFxxxx_I2Cx_SR_IAAS                   (0x01 << 6)
# define HAL_MCFxxxx_I2Cx_SR_IBB                    (0x01 << 5)
# define HAL_MCFxxxx_I2Cx_SR_IAL                    (0x01 << 4)
# define HAL_MCFxxxx_I2Cx_SR_SRW                    (0x01 << 2)
# define HAL_MCFxxxx_I2Cx_SR_IIF                    (0x01 << 1)
# define HAL_MCFxxxx_I2Cx_SR_RXAK                   (0x01 << 0)

#endif // HAL_MCFxxxx_HAS_MCF5282_I2C
    
// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_CAN
// FLEXCAN
// 16-bit module configuration register
# define HAL_MCFxxxx_CANx_MCR                               0x0000
// Three 8-bit control registers and an 8-bit prescaler
# define HAL_MCFxxxx_CANx_CTRL0                             0x0006
# define HAL_MCFxxxx_CANx_CTRL1                             0x0007
# define HAL_MCFxxxx_CANx_PRESDIV                           0x0008
# define HAL_MCFxxxx_CANx_CTRL2                             0x0009
// 16-bit free runxxng timer
# define HAL_MCFxxxx_CANx_TIMER                             0x000A
// 32-bit global and buffer mask registers
# define HAL_MCFxxxx_CANx_RXGMASK                           0x0010
# define HAL_MCFxxxx_CANx_RX14MASK                          0x0014
# define HAL_MCFxxxx_CANx_RX15MASK                          0x0018
// 16-bit error and status
# define HAL_MCFxxxx_CANx_ESTAT                             0x0020
// 16-bit interrup mask and flags
# define HAL_MCFxxxx_CANx_IMASK                             0x0022
# define HAL_MCFxxxx_CANx_IFLAG                             0x0024
// Two 8-bit error counters
# define HAL_MCFxxxx_CANx_RXECTR                            0x0026
# define HAL_MCFxxxx_CANx_TXECTR                            0x0027
// 16 32-bit message buffers start here
# define HAL_MCFxxxx_CANx_BUF                               0x0080

# define HAL_MCFxxxx_CANx_MCR_STOP                          (0x01 << 15)
# define HAL_MCFxxxx_CANx_MCR_FRZ                           (0x01 << 14)
# define HAL_MCFxxxx_CANx_MCR_HALT                          (0x01 << 12)
# define HAL_MCFxxxx_CANx_MCR_NOTRDY                        (0x01 << 11)
# define HAL_MCFxxxx_CANx_MCR_WAKEMSK                       (0x01 << 10)
# define HAL_MCFxxxx_CANx_MCR_SOFTRST                       (0x01 << 9)
# define HAL_MCFxxxx_CANx_MCR_FRZACK                        (0x01 << 8)
# define HAL_MCFxxxx_CANx_MCR_SUPV                          (0x01 << 7)
# define HAL_MCFxxxx_CANx_MCR_SELFWAKE                      (0x01 << 6)
# define HAL_MCFxxxx_CANx_MCR_APS                           (0x01 << 5)
# define HAL_MCFxxxx_CANx_MCR_STOPACK                       (0x01 << 4)

# define HAL_MCFxxxx_CANx_CTRL0_BOFFMSK                     (0x01 << 7)
# define HAL_MCFxxxx_CANx_CTRL0_ERRMASK                     (0x01 << 6)
# define HAL_MCFxxxx_CANx_CTRL0_RXMODE                      (0x01 << 2)
# define HAL_MCFxxxx_CANx_CTRL0_RXMODE_0_DOMINANT           (0x00 << 2)
# define HAL_MCFxxxx_CANx_CTRL0_RXMODE_1_DOMINANT           (0x01 << 2)
# define HAL_MCFxxxx_CANx_CTRL0_TXMODE_MASK                 (0x03 << 0)
# define HAL_MCFxxxx_CANx_CTRL0_TXMODE_SHIFT                0
# define HAL_MCFxxxx_CANx_CTRL0_TXMODE_FULL_0_DOMINANT      (0x00 << 0)
# define HAL_MCFxxxx_CANx_CTRL0_TXMODE_FULL_1_DOMINANT      (0x01 << 0)
# define HAL_MCFxxxx_CANx_CTRL0_TXMODE_OPEN_0_DOMINANT      (0x02 << 0)

# define HAL_MCFxxxx_CANx_CTRL1_SAMP                        (0x01 << 7)
# define HAL_MCFxxxx_CANx_CTRL1_TSYNC                       (0x01 << 5)
# define HAL_MCFxxxx_CANx_CTRL1_LBUF                        (0x01 << 4)
# define HAL_MCFxxxx_CANx_CTRL1_LOM                         (0x01 << 3)
# define HAL_MCFxxxx_CANx_CTRL1_PROPSEG_MASK                (0x07 << 0)
# define HAL_MCFxxxx_CANx_CTRL1_PROPSEG_SHIFT               0

# define HAL_MCFxxxx_CANx_CTRL2_RJW_MASK                    (0x03 << 6)
# define HAL_MCFxxxx_CANx_CTRL2_RJW_SHIFT                   6
# define HAL_MCFxxxx_CANx_CTRL2_PSEG1_MASK                  (0x07 << 3)
# define HAL_MCFxxxx_CANx_CTRL2_PSEG1_SHIFT                 3
# define HAL_MCFxxxx_CANx_CTRL2_PSEG2_MASK                  (0x07 << 0)
# define HAL_MCFxxxx_CANx_CTRL2_PSEG2_SHIFT                 0

# define HAL_MCFxxxx_CANx_ESTAT_BITERR_MASK                 (0x03 << 14)
# define HAL_MCFxxxx_CANx_ESTAT_BITERR_SHIFT                14
# define HAL_MCFxxxx_CANx_ESTAT_BITERR_NONE                 (0x00 << 14)
# define HAL_MCFxxxx_CANx_ESTAT_BITERR_DOMINANT_RECESSIVE   (0x01 << 14)
# define HAL_MCFxxxx_CANx_ESTAT_BITERR_RECESSIVE_DOMINANT   (0x02 << 14)
# define HAL_MCFxxxx_CANx_ESTAT_ACKERR                      (0x01 << 13)
# define HAL_MCFxxxx_CANx_ESTAT_CRCERR                      (0x01 << 12)
# define HAL_MCFxxxx_CANx_ESTAT_FORMERR                     (0x01 << 11)
# define HAL_MCFxxxx_CANx_ESTAT_STUFFERR                    (0x01 << 10)
# define HAL_MCFxxxx_CANx_ESTAT_TXWARN                      (0x01 << 9)
# define HAL_MCFxxxx_CANx_ESTAT_RXWARN                      (0x01 << 8)
# define HAL_MCFxxxx_CANx_ESTAT_IDLE                        (0x01 << 7)
# define HAL_MCFxxxx_CANx_ESTAT_TX_RX                       (0x01 << 6)
# define HAL_MCFxxxx_CANx_ESTAT_FCS_MASK                    (0x03 << 4)
# define HAL_MCFxxxx_CANx_ESTAT_FCS_SHIFT                   4
# define HAL_MCFxxxx_CANx_ESTAT_FCS_ERROR_ACTIVE            (0x00 << 4)
# define HAL_MCFxxxx_CANx_ESTAT_FCS_ERROR_PASSIVE           (0x01 << 4)
# define HAL_MCFxxxx_CANx_ESTAT_BOFFINT                     (0x01 << 2)
# define HAL_MCFxxxx_CANx_ESTAT_ERRINT                      (0x01 << 1)
# define HAL_MCFxxxx_CANx_ESTAT_WAKEINT                     (0x01 << 0)

#endif  // HAL_MCFxxxx_HAS_MCF5282_CAN

// Some ColdFire processors have a variation of the CAN device where all
// the registers are 32-bit.
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_CAN_32BIT_REGS
#  define HAL_MCFxxxx_CANx_MCR                      0x0000
#  define HAL_MCFxxxx_CANx_CTRL                     0x0004
#  define HAL_MCFxxxx_CANx_TIMER                    0x0008
#  define HAL_MCFxxxx_CANx_RXGMASK                  0x0010
#  define HAL_MCFxxxx_CANx_RX14MASK                 0x0014
#  define HAL_MCFxxxx_CANx_RX15MASK                 0x0018
#  define HAL_MCFxxxx_CANx_ERRCNT                   0x001C
#  define HAL_MCFxxxx_CANx_ERRSTAT                  0x0020
#  define HAL_MCFxxxx_CANx_IMASK                    0x0028
#  define HAL_MCFxxxx_CANx_IFLAG                    0x0030
#  define HAL_MCFxxxx_CANx_BUF                      0x0080

#  define HAL_MCFxxxx_CANx_MCR_MDIS                 (0x01 << 31)
#  define HAL_MCFxxxx_CANx_MCR_FRZ                  (0x01 << 30)
#  define HAL_MCFxxxx_CANx_MCR_HALT                 (0x01 << 28)
#  define HAL_MCFxxxx_CANx_MCR_NOTRDY               (0x01 << 27)
#  define HAL_MCFxxxx_CANx_MCR_SOFTRST              (0x01 << 25)
#  define HAL_MCFxxxx_CANx_MCR_FRZACK               (0x01 << 24)
#  define HAL_MCFxxxx_CANx_MCR_SUPV                 (0x01 << 23)
#  define HAL_MCFxxxx_CANx_MCR_LPMACK               (0x01 << 20)
#  define HAL_MCFxxxx_CANx_MCR_MAXMB_MASK           (0x0F << 0)
#  define HAL_MCFxxxx_CANx_MCR_MAXMB_SHIFT          0

#  define HAL_MCFxxxx_CANx_CTRL_PRESDIV_MASK        (0x00FF << 24)
#  define HAL_MCFxxxx_CANx_CTRL_PRESDIV_SHIFT       24
#  define HAL_MCFxxxx_CANx_CTRL_RJW_MASK            (0x03 << 22)
#  define HAL_MCFxxxx_CANx_CTRL_RJW_SHIFT           22
#  define HAL_MCFxxxx_CANx_CTRL_PSEG1_MASK          (0x07 << 19)
#  define HAL_MCFxxxx_CANx_CTRL_PSEG1_SHIFT         19
#  define HAL_MCFxxxx_CANx_CTRL_PSEG2_MASK          (0x07 << 16)
#  define HAL_MCFxxxx_CANx_CTRL_PSEG2_SHIFT         16
#  define HAL_MCFxxxx_CANx_CTRL_BOFFMSK             (0x01 << 15)
#  define HAL_MCFxxxx_CANx_CTRL_ERRMSK              (0x01 << 14)
#  define HAL_MCFxxxx_CANx_CTRL_CLKSRC              (0x01 << 13)
#  define HAL_MCFxxxx_CANx_CTRL_LPB                 (0x01 << 12)
#  define HAL_MCFxxxx_CANx_CTRL_SMP                 (0x01 << 7)
#  define HAL_MCFxxxx_CANx_CTRL_BOFFREC             (0x01 << 6)
#  define HAL_MCFxxxx_CANx_CTRL_TSYN                (0x01 << 5)
#  define HAL_MCFxxxx_CANx_CTRL_LBUF                (0x01 << 4)
#  define HAL_MCFxxxx_CANx_CTRL_LOM                 (0x01 << 3)
#  define HAL_MCFxxxx_CANx_CTRL_PROPSEG_MASK        (0x07 << 0)
#  define HAL_MCFxxxx_CANx_CTRL_PROPSEG_SHIFT       0

#  define HAL_MCFxxxx_CANx_ERRCNT_RXECTR_MASK       (0x00FF << 8)
#  define HAL_MCFxxxx_CANx_ERRCNT_RXECTR_SHIFT      8
#  define HAL_MCFxxxx_CANx_ERRCNT_TXECTR_MASK       (0x00FF << 0)
#  define HAL_MCFxxxx_CANx_ERRCNT_TXECTR_SHIFT      0

#  define HAL_MCFxxxx_CANx_ERRSTAT_BIT1ERR          (0x01 << 15)
#  define HAL_MCFxxxx_CANx_ERRSTAT_BIT0ERR          (0x01 << 14)
#  define HAL_MCFxxxx_CANx_ERRSTAT_ACKERR           (0x01 << 13)
#  define HAL_MCFxxxx_CANx_ERRSTAT_CRCERR           (0x01 << 12)
#  define HAL_MCFxxxx_CANx_ERRSTAT_FRMERR           (0x01 << 11)
#  define HAL_MCFxxxx_CANx_ERRSTAT_STFERR           (0x01 << 10)
#  define HAL_MCFxxxx_CANx_ERRSTAT_TXWRN            (0x01 << 9)
#  define HAL_MCFxxxx_CANx_ERRSTAT_RXWRN            (0x01 << 8)
#  define HAL_MCFxxxx_CANx_ERRSTAT_IDLE             (0x01 << 7)
#  define HAL_MCFxxxx_CANx_ERRSTAT_TXRX             (0x01 << 6)
#  define HAL_MCFxxxx_CANx_ERRSTAT_FLTCONF_MASK     (0x03 << 4)
#  define HAL_MCFxxxx_CANx_ERRSTAT_FLTCONF_SHIFT    4
#  define HAL_MCFxxxx_CANx_ERRSTAT_BOFFINT          (0x01 << 2)
#  define HAL_MCFxxxx_CANx_ERRSTAT_ERRINT           (0x01 << 1)
#endif

// ----------------------------------------------------------------------------
#ifdef HAL_MCFxxxx_HAS_MCF5282_QADC
// Queued analog-to-digital converter

// Configuration register, 16 bits
# define HAL_MCFxxxx_QADC_QADCMCR                   0x0000
// Test register, 16 bits. This is only usable in factory test mode
# define HAL_MCFxxxx_QADC_QADCTEST                  0x0002
// Two port data registers, 8 bits each
# define HAL_MCFxxxx_QADC_PORTQA                    0x0006
# define HAL_MCFxxxx_QADC_PORTQB                    0x0007
// Two port direction registers, 8 bits each
# define HAL_MCFxxxx_QADC_DDRQA                     0x0008
# define HAL_MCFxxxx_QADC_DDRQB                     0x0009
// Three control registers, 16 bits each
# define HAL_MCFxxxx_QADC_QACR0                     0x000A
# define HAL_MCFxxxx_QADC_QACR1                     0x000C
# define HAL_MCFxxxx_QADC_QACR2                     0x000E
// Two status registers, 16 bits each
# define HAL_MCFxxxx_QADC_QASR0                     0x0010
# define HAL_MCFxxxx_QADC_QASR1                     0x0012
// Command word table, 64 entries * 16 bits each
# define HAL_MCFxxxx_QADC_CCW                       0x0200
// Right-justified unsigned results, 64 entries * 16 bits
# define HAL_MCFxxxx_QADC_RJURR                     0x0280
// Left-justified signed results, 64 entries * 16 bits
# define HAL_MCFxxxx_QADC_LJSRR                     0x0300
// Left-justified unsigned results, 64 entries * 16 bits
# define HAL_MCFxxxx_QADC_LJURR                     0x0380

# define HAL_MCFxxxx_QADC_QADCMCR_QSTOP             (0x01 << 15)
# define HAL_MCFxxxx_QADC_QADCMCR_QDBG              (0x01 << 14)
# define HAL_MCFxxxx_QADC_QADCMCR_QSUPV             (0x01 << 7)

// The port data and direction registers just provide control
// over four signals apiece.

# define HAL_MCFxxxx_QADC_QACR0_MUX                 (0x01 << 15)
# define HAL_MCFxxxx_QADC_QACR0_TRG                 (0x01 << 14)
# define HAL_MCFxxxx_QADC_QACR0_QPR_MASK            (0x7F << 0)
# define HAL_MCFxxxx_QADC_QACR0_QPR_SHIFT           0
// The actual prescaler is (2 * (QPR+1)), except for a QPR value
// of 0 which gives a prescaler of 4.

// QACR1 and QACR2 control queues 1 and 2 respectively
# define HAL_MCFxxxx_QADC_QACRn_CIE                 (0x01 << 15)
# define HAL_MCFxxxx_QADC_QACRn_PIE                 (0x01 << 14)
# define HAL_MCFxxxx_QADC_QACRn_SSE                 (0x01 << 13)
# define HAL_MCFxxxx_QADC_QACRn_MQ_MASK             (0x1F << 8)
# define HAL_MCFxxxx_QADC_QACRn_MQ_SHIFT            8
// There are 32 different modes, see the manual for details.

// QACR2 has some additional bits to support resume operations
# define HAL_MCFxxxx_QADC_QACR2_RESUME              (0x01 << 7)
# define HAL_MCFxxxx_QADC_QACR2_BQ_MASK             (0x7F << 0)
# define HAL_MCFxxxx_QADC_QACR2_BQ_SHIFT            0

# define HAL_MCFxxxx_QADC_QASR0_CF1                 (0x01 << 15)
# define HAL_MCFxxxx_QADC_QASR0_PF1                 (0x01 << 14)
# define HAL_MCFxxxx_QADC_QASR0_CF2                 (0x01 << 13)
# define HAL_MCFxxxx_QADC_QASR0_PF2                 (0x01 << 12)
# define HAL_MCFxxxx_QADC_QASR0_TOR1                (0x01 << 11)
# define HAL_MCFxxxx_QADC_QASR0_TOR2                (0x01 << 10)
# define HAL_MCFxxxx_QADC_QASR0_QS_MASK             (0x0F << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_SHIFT            6
# define HAL_MCFxxxx_QADC_QASR0_QS_IDLE_IDLE        (0x00 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_IDLE_PAUSED      (0x01 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_IDLE_ACTIVE      (0x02 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_IDLE_PENDING     (0x03 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_PAUSED_IDLE      (0x04 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_PAUSED_PAUSED    (0x05 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_PAUSED_ACTIVE    (0x06 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_PAUSED_PENDING   (0x07 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_ACTIVE_IDLE      (0x08 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_ACTIVE_PAUSED    (0x09 << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_ACTIVE_SUSPENDED (0x0A << 6)
# define HAL_MCFxxxx_QADC_QASR0_QS_ACTIVE_PENDING   (0x0B << 6)
# define HAL_MCFxxxx_QADC_QASR0_CWP_MASK            (0x3F << 0)
# define HAL_MCFxxxx_QADC_QASR0_CWP_SHIFT           0

# define HAL_MCFxxxx_QADC_QASR1_CWPQ1_MASK          (0x3F << 8)
# define HAL_MCFxxxx_QADC_QASR1_CWPQ1_SHIFT         8
# define HAL_MCFxxxx_QADC_QASR1_CWPQ_MASK           (0x3F << 0)
# define HAL_MCFxxxx_QADC_QASR1_CWPQ_SHIFT          0

# define HAL_MCFxxxx_QADC_CCW_PAUSE                 (0x01 << 9)
# define HAL_MCFxxxx_QADC_CCW_BYP                   (0x01 << 8)
# define HAL_MCFxxxx_QADC_CCW_IST_MASK              (0x03 << 6)
# define HAL_MCFxxxx_QADC_CCW_IST_SHIFT             6
# define HAL_MCFxxxx_QADC_CCW_IST_2                 (0x00 << 6)
# define HAL_MCFxxxx_QADC_CCW_IST_4                 (0x01 << 6)
# define HAL_MCFxxxx_QADC_CCW_IST_8                 (0x02 << 6)
# define HAL_MCFxxxx_QADC_CCW_IST_16                (0x03 << 6)
# define HAL_MCFxxxx_QADC_CCW_CHAN_MASK             (0x3F << 0)
# define HAL_MCFxxxx_QADC_CCW_CHAN_SHIFT            0

# define HAL_MCFxxxx_QADC_CCW_CHAN_AN0              0
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN1              1
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN2              2
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN3              3
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN52             52
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN53             53
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN55             55
# define HAL_MCFxxxx_QADC_CCW_CHAN_AN56             56
# define HAL_MCFxxxx_QADC_CCW_CHAN_ETRIG1           55
# define HAL_MCFxxxx_QADC_CCW_CHAN_ETRIG2           56
# define HAL_MCFxxxx_QADC_CCW_CHAN_LOWREF           60              
# define HAL_MCFxxxx_QADC_CCW_CHAN_HIGHREF          61
# define HAL_MCFxxxx_QADC_CCW_CHAN_MIDREF           62
# define HAL_MCFxxxx_QADC_CCW_CHAN_EOQ              63

#endif  // HAL_MCFxxxx_HAS_MCF5282_QADC

// ----------------------------------------------------------------------------
// Pulse width modulation unit.
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_PWM
# define HAL_MCFxxxx_PWMx_PWME                              0x0000
# define HAL_MCFxxxx_PWMx_PWMPOL                            0x0001
# define HAL_MCFxxxx_PWMx_PWMCLK                            0x0002
# define HAL_MCFxxxx_PWMx_PWMPRCLK                          0x0003
# define HAL_MCFxxxx_PWMx_PWMCAE                            0x0004
# define HAL_MCFxxxx_PWMx_PWMCTL                            0x0005
# define HAL_MCFxxxx_PWMx_PWMSCLA                           0x0008
# define HAL_MCFxxxx_PWMx_PWMSCLB                           0x0009
# define HAL_MCFxxxx_PWMx_PWMCNT0                           0x000C
# define HAL_MCFxxxx_PWMx_PWMCNT1                           0x000D
# define HAL_MCFxxxx_PWMx_PWMCNT2                           0x000E
# define HAL_MCFxxxx_PWMx_PWMCNT3                           0x000F
# define HAL_MCFxxxx_PWMx_PWMCNT4                           0x0010
# define HAL_MCFxxxx_PWMx_PWMCNT5                           0x0011
# define HAL_MCFxxxx_PWMx_PWMCNT6                           0x0012
# define HAL_MCFxxxx_PWMx_PWMCNT7                           0x0013
# define HAL_MCFxxxx_PWMx_PWMPER0                           0x0014
# define HAL_MCFxxxx_PWMx_PWMPER1                           0x0015
# define HAL_MCFxxxx_PWMx_PWMPER2                           0x0016
# define HAL_MCFxxxx_PWMx_PWMPER3                           0x0017
# define HAL_MCFxxxx_PWMx_PWMPER4                           0x0018
# define HAL_MCFxxxx_PWMx_PWMPER5                           0x0019
# define HAL_MCFxxxx_PWMx_PWMPER6                           0x001A
# define HAL_MCFxxxx_PWMx_PWMPER7                           0x001B
# define HAL_MCFxxxx_PWMx_PWMDTY0                           0x001C
# define HAL_MCFxxxx_PWMx_PWMDTY1                           0x001D
# define HAL_MCFxxxx_PWMx_PWMDTY2                           0x001E
# define HAL_MCFxxxx_PWMx_PWMDTY3                           0x001F
# define HAL_MCFxxxx_PWMx_PWMDTY4                           0x0020
# define HAL_MCFxxxx_PWMx_PWMDTY5                           0x0021
# define HAL_MCFxxxx_PWMx_PWMDTY6                           0x0022
# define HAL_MCFxxxx_PWMx_PWMDTY7                           0x0023
# define HAL_MCFxxxx_PWMx_PWMSDN                            0x0024
 
# define HAL_MCFxxxx_PWMx_PWME_PWME7                        (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWME_PWME5                        (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWME_PWME3                        (0x01 << 3)
# define HAL_MCFxxxx_PWMx_PWME_PWME1                        (0x01 << 1)
# define HAL_MCFxxxx_PWMx_PWMPOL_PPOL7                      (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWMPOL_PPOL5                      (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWMPOL_PPOL3                      (0x01 << 3)
# define HAL_MCFxxxx_PWMx_PWMPOL_PPOL1                      (0x01 << 1)
# define HAL_MCFxxxx_PWMx_PWMCLK_PCLK7                      (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWMCLK_PCLK5                      (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWMCLK_PCLK3                      (0x01 << 3)
# define HAL_MCFxxxx_PWMx_PWMCLK_PCLK1                      (0x01 << 1)
# define HAL_MCFxxxx_PWMx_PWMPRCLK_PCKB_MASK                (0x07 << 4)
# define HAL_MCFxxxx_PWMx_PWMPRCLK_PCKB_SHIFT               4
# define HAL_MCFxxxx_PWMx_PWMPRCLK_PCKA_MASK                (0x07 << 0)
# define HAL_MCFxxxx_PWMx_PWMPRCLK_PCKA_SHIFT               0
# define HAL_MCFxxxx_PWMx_PWMCAE_CAE7                       (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWMCAE_CAE5                       (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWMCAE_CAE3                       (0x01 << 3)
# define HAL_MCFxxxx_PWMx_PWMCAE_CAE1                       (0x01 << 1)
# define HAL_MCFxxxx_PWMx_PWMCTL_CON67                      (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWMCTL_CON45                      (0x01 << 6)
# define HAL_MCFxxxx_PWMx_PWMCTL_CON23                      (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWMCTL_CON01                      (0x01 << 4)
# define HAL_MCFxxxx_PWMx_PWMCTL_PSWAI                      (0x01 << 3)
# define HAL_MCFxxxx_PWMx_PWMCTL_PFRZ                       (0x01 << 2)
# define HAL_MCFxxxx_PWMx_PWMSDN_IF                         (0x01 << 7)
# define HAL_MCFxxxx_PWMx_PWMSDN_IE                         (0x01 << 6)
# define HAL_MCFxxxx_PWMx_PWMSDN_RESTART                    (0x01 << 5)
# define HAL_MCFxxxx_PWMx_PWMSDN_LVL                        (0x01 << 4)
# define HAL_MCFxxxx_PWMx_PWMSDN_PWM7IN                     (0x01 << 2)
# define HAL_MCFxxxx_PWMx_PWMSDN_PWM7IL                     (0x01 << 1)
# define HAL_MCFxxxx_PWMx_PWMSDN_SDNEN                      (0x01 << 0)
#endif  // HAS_MCFxxxx_PWM

// ----------------------------------------------------------------------------
// Real-time clock.
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_RTC
# define HAL_MCFxxxx_RTC_HOURMIN                    0x00
# define HAL_MCFxxxx_RTC_SECONDS                    0x04
# define HAL_MCFxxxx_RTC_ALRM_HM                    0x08
# define HAL_MCFxxxx_RTC_ALRM_SEC                   0x0C
# define HAL_MCFxxxx_RTC_CR                         0x10
# define HAL_MCFxxxx_RTC_ISR                        0x14
# define HAL_MCFxxxx_RTC_IER                        0x18
# define HAL_MCFxxxx_RTC_STPWCH                     0x1C
# define HAL_MCFxxxx_RTC_DAYS                       0x20
# define HAL_MCFxxxx_RTC_ALRM_DAY                   0x24

# define HAL_MCFxxxx_RTC_HOURMIN_HOURS_MASK         (0x1F << 8)
# define HAL_MCFxxxx_RTC_HOURMIN_HOURS_SHIFT        8
# define HAL_MCFxxxx_RTC_HOURMIN_MINUTES_MASK       (0x3F << 0)
# define HAL_MCFxxxx_RTC_HOURMIN_MINUTES_SHIFT      0
# define HAL_MCFxxxx_RTC_SECONDS_SECONDS_MASK       (0x3F << 0)
# define HAL_MCFxxxx_RTC_SECONDS_SECONDS_SHIFT      0
# define HAL_MCFxxxx_RTC_ALRM_HM_HOURS_MASK         (0x1F << 8)
# define HAL_MCFxxxx_RTC_ALRM_HM_HOURS_SHIFT        8
# define HAL_MCFxxxx_RTC_ALRM_HM_MINUTES_MASK       (0x3F << 0)
# define HAL_MCFxxxx_RTC_ALRM_HM_MINUTES_SHIFT      0
# define HAL_MCFxxxx_RTC_ALRM_SEC_SECONDS_MASK      (0x3F << 0)
# define HAL_MCFxxxx_RTC_ALARM_SEC_SECONDS_SHIFT    0
# define HAL_MCFxxxx_RTC_CR_EN                      (0x01 << 7)
# define HAL_MCFxxxx_RTC_CR_XTL_MASK                (0x03 << 5)
# define HAL_MCFxxxx_RTC_CR_XTL_SHIFT               5
# define HAL_MCFxxxx_RTC_CR_XTL_32768               (0x00 << 5)
# define HAL_MCFxxxx_RTC_CR_XTL_32000               (0x01 << 5)
# define HAL_MCFxxxx_RTC_CR_XTL_38400               (0x02 << 5)
# define HAL_MCFxxxx_RTC_CR_SWR                     (0x01 << 0)
# define HAL_MCFxxxx_RTC_ISR_SAM7                   (0x01 << 15)
# define HAL_MCFxxxx_RTC_ISR_SAM6                   (0x01 << 14)
# define HAL_MCFxxxx_RTC_ISR_SAM5                   (0x01 << 13)
# define HAL_MCFxxxx_RTC_ISR_SAM4                   (0x01 << 12)
# define HAL_MCFxxxx_RTC_ISR_SAM3                   (0x01 << 11)
# define HAL_MCFxxxx_RTC_ISR_SAM2                   (0x01 << 10)
# define HAL_MCFxxxx_RTC_ISR_SAM1                   (0x01 << 9)
# define HAL_MCFxxxx_RTC_ISR_SAM0                   (0x01 << 8)
# define HAL_MCFxxxx_RTC_ISR_2HZ                    (0x01 << 7)
# define HAL_MCFxxxx_RTC_ISR_HR                     (0x01 << 5)
# define HAL_MCFxxxx_RTC_ISR_1HZ                    (0x01 << 4)
# define HAL_MCFxxxx_RTC_ISR_DAY                    (0x01 << 3)
# define HAL_MCFxxxx_RTC_ISR_ALM                    (0x01 << 2)
# define HAL_MCFxxxx_RTC_ISR_MIN                    (0x01 << 1)
# define HAL_MCFxxxx_RTC_ISR_SW                     (0x01 << 0)
# define HAL_MCFxxxx_RTC_IER_SAM7                   (0x01 << 15)
# define HAL_MCFxxxx_RTC_IER_SAM6                   (0x01 << 14)
# define HAL_MCFxxxx_RTC_IER_SAM5                   (0x01 << 13)
# define HAL_MCFxxxx_RTC_IER_SAM4                   (0x01 << 12)
# define HAL_MCFxxxx_RTC_IER_SAM3                   (0x01 << 11)
# define HAL_MCFxxxx_RTC_IER_SAM2                   (0x01 << 10)
# define HAL_MCFxxxx_RTC_IER_SAM1                   (0x01 << 9)
# define HAL_MCFxxxx_RTC_IER_SAM0                   (0x01 << 8)
# define HAL_MCFxxxx_RTC_IER_2HZ                    (0x01 << 7)
# define HAL_MCFxxxx_RTC_IER_HR                     (0x01 << 5)
# define HAL_MCFxxxx_RTC_IER_1HZ                    (0x01 << 4)
# define HAL_MCFxxxx_RTC_IER_DAY                    (0x01 << 3)
# define HAL_MCFxxxx_RTC_IER_ALM                    (0x01 << 2)
# define HAL_MCFxxxx_RTC_IER_MIN                    (0x01 << 1)
# define HAL_MCFxxxx_RTC_IER_SW                     (0x01 << 0)
# define HAL_MCFxxxx_RTC_STPWCH_CNT_MASK            (0x3F << 0)
# define HAL_MCFxxxx_RTC_STPWCH_CNT_SHIFT           0
# define HAL_MCFxxxx_RTC_DAYS_DAYS_MASK             (0x00FFFF << 0)
# define HAL_MCFxxxx_RTC_DAYS_DAYS_SHIFT            0
# define HAL_MCFxxxx_RTC_ALRM_DAY_DAYS_MASK         (0x00FFFF << 0)
# define HAL_MCFxxxx_RTC_ALRM_DAY_DAYS_SHIFT        0
#endif  // HAS_MCFxxxx_RTC

// ----------------------------------------------------------------------------
// Message digest hardware
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_MDHA
# define HAL_MCFxxxx_MDHA_MR                        0x0000
# define HAL_MCFxxxx_MDHA_CR                        0x0004
# define HAL_MCFxxxx_MDHA_CMR                       0x0008
# define HAL_MCFxxxx_MDHA_SR                        0x000C
# define HAL_MCFxxxx_MDHA_ISR                       0x0010
# define HAL_MCFxxxx_MDHA_IMR                       0x0014
# define HAL_MCFxxxx_MDHA_DSR                       0x001C
# define HAL_MCFxxxx_MDHA_MIN                       0x0020
# define HAL_MCFxxxx_MDHA_A0                        0x0030
# define HAL_MCFxxxx_MDHA_B0                        0x0034
# define HAL_MCFxxxx_MDHA_C0                        0x0038
# define HAL_MCFxxxx_MDHA_D0                        0x003C
# define HAL_MCFxxxx_MDHA_E0                        0x0040
# define HAL_MCFxxxx_MDHA_MDS                       0x0044
# define HAL_MCFxxxx_MDHA_A1                        0x0070
# define HAL_MCFxxxx_MDHA_B1                        0x0074
# define HAL_MCFxxxx_MDHA_C1                        0x0078
# define HAL_MCFxxxx_MDHA_D1                        0x007C
# define HAL_MCFxxxx_MDHA_E1                        0x0080

# define HAL_MCFxxxx_MDHA_MR_SSL                    (0x01 << 10)
# define HAL_MCFxxxx_MDHA_MR_MACFULL                (0x01 << 9)
# define HAL_MCFxxxx_MDHA_MR_SWAP                   (0x01 << 8)
# define HAL_MCFxxxx_MDHA_MR_OPAD                   (0x01 << 7)
# define HAL_MCFxxxx_MDHA_MR_IPAD                   (0x01 << 6)
# define HAL_MCFxxxx_MDHA_MR_INIT                   (0x01 << 5)
# define HAL_MCFxxxx_MDHA_MR_MAC_MASK               (0x03 << 3)
# define HAL_MCFxxxx_MDHA_MR_MAC_SHIFT              3
# define HAL_MCFxxxx_MDHA_MR_MAC_NONE               (0x00 << 3)
# define HAL_MCFxxxx_MDHA_MR_MAC_HMAC               (0x01 << 3)
# define HAL_MCFxxxx_MDHA_MR_MAC_EHMAC              (0x02 << 3)
# define HAL_MCFxxxx_MDHA_MR_PDATA                  (0x01 << 2)
# define HAL_MCFxxxx_MDHA_MR_ALG                    (0x01 << 0)
# define HAL_MCFxxxx_MDHA_CR_DMAL_MASK              (0x1F << 16)
# define HAL_MCFxxxx_MDHA_CR_DMAL_SHIFT             16
# define HAL_MCFxxxx_MDHA_CR_END                    (0x01 << 2)
# define HAL_MCFxxxx_MDHA_CR_DMA                    (0x01 << 2)
# define HAL_MCFxxxx_MDHA_CR_IE                     (0x01 << 2)
# define HAL_MCFxxxx_MDHA_CMR_GO                    (0x01 << 3)
# define HAL_MCFxxxx_MDHA_CMR_CI                    (0x01 << 2)
# define HAL_MCFxxxx_MDHA_CMR_RI                    (0x01 << 1)
# define HAL_MCFxxxx_MDHA_CMR_SWR                   (0x01 << 0)
# define HAL_MCFxxxx_MDHA_SR_IFL_MASK               (0x00FF << 16)
# define HAL_MCFxxxx_MDHA_SR_IFL_SHIFT              16
# define HAL_MCFxxxx_MDHA_SR_APD_MASK               (0x07 << 13)
# define HAL_MCFxxxx_MDHA_SR_APD_SHIFT              13
# define HAL_MCFxxxx_MDHA_SR_APD_STANDARD           (0x00 << 13)
# define HAL_MCFxxxx_MDHA_SR_APD_PAD_LAST_WORD      (0x01 << 13)
# define HAL_MCFxxxx_MDHA_SR_APD_ADD_A_WORD         (0x02 << 13)
# define HAL_MCFxxxx_MDHA_SR_APD_LAST_HASH          (0x03 << 13)
# define HAL_MCFxxxx_MDHA_SR_APD_STALL_STATE        (0x04 << 13)
# define HAL_MCFxxxx_MDHA_SR_FS_MASK                (0x07 << 8)
# define HAL_MCFxxxx_MDHA_SR_FS_SHIFT               8
# define HAL_MCFxxxx_MDHA_SR_GNW                    (0x01 << 7)
# define HAL_MCFxxxx_MDHA_SR_HSH                    (0x01 << 6)
# define HAL_MCFxxxx_MDHA_SR_BUSY                   (0x01 << 4)
# define HAL_MCFxxxx_MDHA_SR_RD                     (0x01 << 3)
# define HAL_MCFxxxx_MDHA_SR_ERR                    (0x01 << 2)
# define HAL_MCFxxxx_MDHA_SR_DONE                   (0x01 << 1)
# define HAL_MCFxxxx_MDHA_SR_INT                    (0x01 << 0)
# define HAL_MCFxxxx_MDHA_ISR_DRL                   (0x01 << 10)
# define HAL_MCFxxxx_MDHA_ISR_GTDS                  (0x01 << 9)
# define HAL_MCFxxxx_MDHA_ISR_ERE                   (0x01 << 8)
# define HAL_MCFxxxx_MDHA_ISR_RMDP                  (0x01 << 7)
# define HAL_MCFxxxx_MDHA_ISR_DSE                   (0x01 << 5)
# define HAL_MCFxxxx_MDHA_ISR_IME                   (0x01 << 4)
# define HAL_MCFxxxx_MDHA_ISR_NEIF                  (0x01 << 2)
# define HAL_MCFxxxx_MDHA_ISR_IFO                   (0x01 << 0)
# define HAL_MCFxxxx_MDHA_IMR_DRL                   (0x01 << 10)
# define HAL_MCFxxxx_MDHA_IMR_GTDS                  (0x01 << 9)
# define HAL_MCFxxxx_MDHA_IMR_ERE                   (0x01 << 8)
# define HAL_MCFxxxx_MDHA_IMR_RMDP                  (0x01 << 7)
# define HAL_MCFxxxx_MDHA_IMR_DSE                   (0x01 << 5)
# define HAL_MCFxxxx_MDHA_IMR_IME                   (0x01 << 4)
# define HAL_MCFxxxx_MDHA_IMR_NEIF                  (0x01 << 2)
# define HAL_MCFxxxx_MDHA_IMR_IFO                   (0x01 << 0)
#endif  // HAS_MCFxxxx_MDHA

// ----------------------------------------------------------------------------
// Random number generation
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_RNG
# define HAL_MCFxxxx_RNG_CR                         0x00000000
# define HAL_MCFxxxx_RNG_SR                         0x00000004
# define HAL_MCFxxxx_RNG_ER                         0x00000008
# define HAL_MCFxxxx_RNG_OUT                        0x0000000C

# define HAL_MCFxxxx_RNG_CR_CI                      (0x01 << 3)
# define HAL_MCFxxxx_RNG_CR_IM                      (0x01 << 2)
# define HAL_MCFxxxx_RNG_CR_HA                      (0x01 << 1)
# define HAL_MCFxxxx_RNG_CR_GO                      (0x01 << 0)
# define HAL_MCFxxxx_RNG_SR_OFS_MASK                (0x00FF << 16)
# define HAL_MCFxxxx_RNG_SR_OFS_SHIFT               16
# define HAL_MCFxxxx_RNG_SR_OFL_MASK                (0x00FF << 8)
# define HAL_MCFxxxx_RNG_SR_OFL_SHIFT               8
# define HAL_MCFxxxx_RNG_SR_EI                      (0x01 << 3)
# define HAL_MCFxxxx_RNG_SR_FUF                     (0x01 << 2)
# define HAL_MCFxxxx_RNG_SR_LRS                     (0x01 << 1)
# define HAL_MCFxxxx_RNG_SR_SR                      (0x01 << 0)
#endif  // HAS_MCFxxxx_RNG

// ----------------------------------------------------------------------------
// Symmetric key hardware
#ifdef HAL_MCFxxxx_HAS_MCFxxxx_SKHA

# define HAL_MCFxxxx_SKHA_MR                        0x00000000
# define HAL_MCFxxxx_SKHA_CR                        0x00000004
# define HAL_MCFxxxx_SKHA_CMR                       0x00000008
# define HAL_MCFxxxx_SKHA_SR                        0x0000000C
# define HAL_MCFxxxx_SKHA_ESR                       0x00000010
# define HAL_MCFxxxx_SKHA_EMR                       0x00000014
# define HAL_MCFxxxx_SKHA_KSR                       0x00000018
# define HAL_MCFxxxx_SKHA_DSR                       0x0000001C
# define HAL_MCFxxxx_SKHA_IN                        0x00000020
# define HAL_MCFxxxx_SKHA_OUT                       0x00000024
# define HAL_MCFxxxx_SKHA_KDR1                      0x00000030
# define HAL_MCFxxxx_SKHA_KDR2                      0x00000034
# define HAL_MCFxxxx_SKHA_KDR3                      0x00000038
# define HAL_MCFxxxx_SKHA_KDR4                      0x0000003C
# define HAL_MCFxxxx_SKHA_KDR5                      0x00000040
# define HAL_MCFxxxx_SKHA_KDR6                      0x00000044
# define HAL_MCFxxxx_SKHA_C1                        0x00000070
# define HAL_MCFxxxx_SKHA_C2                        0x00000074
# define HAL_MCFxxxx_SKHA_C3                        0x00000078
# define HAL_MCFxxxx_SKHA_C4                        0x0000007C
# define HAL_MCFxxxx_SKHA_C5                        0x00000080
# define HAL_MCFxxxx_SKHA_C6                        0x00000084
# define HAL_MCFxxxx_SKHA_C7                        0x00000088
# define HAL_MCFxxxx_SKHA_C8                        0x0000008C
# define HAL_MCFxxxx_SKHA_C9                        0x00000090
# define HAL_MCFxxxx_SKHA_C10                       0x00000094
# define HAL_MCFxxxx_SKHA_C11                       0x00000098

# define HAL_MCFxxxx_SKHA_MR_CTRM_MASK              (0x0F << 9)
# define HAL_MCFxxxx_SKHA_MR_CTRM_SHIFT             9
# define HAL_MCFxxxx_SKHA_MR_DKP                    (0x01 << 8)
# define HAL_MCFxxxx_SKHA_MR_CM_MASK                (0x03 << 3)
# define HAL_MCFxxxx_SKHA_MR_CM_SHIFT               3
# define HAL_MCFxxxx_SKHA_MR_CM_ECB                 (0x00 << 3)
# define HAL_MCFxxxx_SKHA_MR_CM_CBC                 (0x01 << 3)
# define HAL_MCFxxxx_SKHA_MR_CM_CTR                 (0x03 << 3)
# define HAL_MCFxxxx_SKHA_MR_DIR                    (0x01 << 2)
# define HAL_MCFxxxx_SKHA_MR_ALG_MASK               (0x03 << 0)
# define HAL_MCFxxxx_SKHA_MR_ALG_SHIFT              0
# define HAL_MCFxxxx_SKHA_MR_ALG_AES                (0x00 << 0)
# define HAL_MCFxxxx_SKHA_MR_ALG_DES                (0x01 << 0)
# define HAL_MCFxxxx_SKHA_MR_ALG_3DES               (0x02 << 0)
# define HAL_MCFxxxx_SKHA_CR_ODMAL_MASK             (0x3F << 24)
# define HAL_MCFxxxx_SKHA_CR_ODMAL_SHIFT            24
# define HAL_MCFxxxx_SKHA_CR_IDMAL_MASK             (0x3F << 16)
# define HAL_MCFxxxx_SKHA_CR_IDMAL_SHIFT            16
# define HAL_MCFxxxx_SKHA_CR_END                    (0x01 << 3)
# define HAL_MCFxxxx_SKHA_CR_ODMA                   (0x01 << 2)
# define HAL_MCFxxxx_SKHA_CR_IDMA                   (0x01 << 1)
# define HAL_MCFxxxx_SKHA_CR_IE                     (0x01 << 0)
# define HAL_MCFxxxx_SKHA_CMR_GO                    (0x01 << 3)
# define HAL_MCFxxxx_SKHA_CMR_CI                    (0x01 << 2)
# define HAL_MCFxxxx_SKHA_CMR_RI                    (0x01 << 1)
# define HAL_MCFxxxx_SKHA_CMR_SWR                   (0x01 << 0)
# define HAL_MCFxxxx_SKHA_SR_OFL_MASK               (0x00FF << 24)
# define HAL_MCFxxxx_SKHA_SR_OFL_SHIFT              24
# define HAL_MCFxxxx_SKHA_SR_IFL_MASK               (0x00FF << 16)
# define HAL_MCFxxxx_SKHA_SR_IFL_SHIFT              16
# define HAL_MCFxxxx_SKHA_SR_BUSY                   (0x01 << 4)
# define HAL_MCFxxxx_SKHA_SR_RD                     (0x01 << 3)
# define HAL_MCFxxxx_SKHA_SR_ERR                    (0x01 << 2)
# define HAL_MCFxxxx_SKHA_SR_DONE                   (0x01 << 1)
# define HAL_MCFxxxx_SKHA_SR_INT                    (0x01 << 0)
# define HAL_MCFxxxx_SKHA_ESR_DRL                   (0x01 << 11)
# define HAL_MCFxxxx_SKHA_ESR_KRE                   (0x01 << 10)
# define HAL_MCFxxxx_SKHA_ESR_KPE                   (0x01 << 9)
# define HAL_MCFxxxx_SKHA_ESR_ERE                   (0x01 << 8)
# define HAL_MCFxxxx_SKHA_ESR_RMDP                  (0x01 << 7)
# define HAL_MCFxxxx_SKHA_ESR_KSE                   (0x01 << 6)
# define HAL_MCFxxxx_SKHA_ESR_DSE                   (0x01 << 5)
# define HAL_MCFxxxx_SKHA_ESR_IME                   (0x01 << 4)
# define HAL_MCFxxxx_SKHA_ESR_NEOF                  (0x01 << 3)
# define HAL_MCFxxxx_SKHA_ESR_NEIF                  (0x01 << 2)
# define HAL_MCFxxxx_SKHA_ESR_OFU                   (0x01 << 1)
# define HAL_MCFxxxx_SKHA_ESR_IFO                   (0x01 << 0)
# define HAL_MCFxxxx_SKHA_EMR_DRL                   (0x01 << 11)
# define HAL_MCFxxxx_SKHA_EMR_KRE                   (0x01 << 10)
# define HAL_MCFxxxx_SKHA_EMR_KPE                   (0x01 << 9)
# define HAL_MCFxxxx_SKHA_EMR_ERE                   (0x01 << 8)
# define HAL_MCFxxxx_SKHA_EMR_RMDP                  (0x01 << 7)
# define HAL_MCFxxxx_SKHA_EMR_KSE                   (0x01 << 6)
# define HAL_MCFxxxx_SKHA_EMR_DSE                   (0x01 << 5)
# define HAL_MCFxxxx_SKHA_EMR_IME                   (0x01 << 4)
# define HAL_MCFxxxx_SKHA_EMR_NEOF                  (0x01 << 3)
# define HAL_MCFxxxx_SKHA_EMR_NEIF                  (0x01 << 2)
# define HAL_MCFxxxx_SKHA_EMR_OFU                   (0x01 << 1)
# define HAL_MCFxxxx_SKHA_EMR_IFO                   (0x01 << 0)
#endif  // HAS_MCFxxxx_SKHA

// ----------------------------------------------------------------------------
// Allow the processor-specific header to override some of the above, if
// necessary, or to extend the definitions.
#include <cyg/hal/proc_io.h>

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_VAR_IO_H
