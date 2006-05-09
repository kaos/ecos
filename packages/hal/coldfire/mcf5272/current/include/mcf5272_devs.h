#ifndef CYGONCE_MCF5272_DEVS_H
#define CYGONCE_MCF5272_DEVS_H

//=============================================================================
//
//      mcf5272_devs.h
//
//      Definitions for the MCF5272 on-chip peripherals
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2006 eCosCentric Ltd.
//
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Enrico Piria, Wade Jensen
// Contributors:
// Date:          2005-25-06
// Purpose:       Definitions for the MCF5272 on-chip peripherals.
// Usage:         #include <cyg/hal/mcf5272_devs.h>
//
//####DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>

// General configuration registers
typedef struct
{

    // Module base  address  register
    cyg_uint32 mbar;

    // System configuration register
    cyg_uint16 scr;

    // System protection register
    cyg_uint16 spr;
    
    // Power management register
    cyg_uint32 pmr;

    // Gap
    cyg_uint16 _res1;

    // Active low power register
    cyg_uint16 alpr;

    // Device identification register
    cyg_uint32 dir;

    // Gap
    cyg_uint32 _res2[3];

} __attribute__ ((aligned (4), packed)) mcf5272_sim_cfg_t;

// Configuration registers macros

#define MCF5272_SIM_SCR_HWWD_1024   0x0003

#define MCF5272_SIM_SPR_ADC         0x8000
#define MCF5272_SIM_SPR_ADCEN       0x0080
#define MCF5272_SIM_SPR_WPV         0x4000
#define MCF5272_SIM_SPR_WPVEN       0x0040
#define MCF5272_SIM_SPR_SMV         0x2000
#define MCF5272_SIM_SPR_SMVEN       0x0020
#define MCF5272_SIM_SPR_SBE         0x1000
#define MCF5272_SIM_SPR_SBEEN       0x0010
#define MCF5272_SIM_SPR_HWT         0x0800
#define MCF5272_SIM_SPR_HWTEN       0x0008
#define MCF5272_SIM_SPR_RPV         0x0400
#define MCF5272_SIM_SPR_RPVEN       0x0004
#define MCF5272_SIM_SPR_EXT         0x0200
#define MCF5272_SIM_SPR_EXTEN       0x0002
#define MCF5272_SIM_SPR_SUV         0x0100
#define MCF5272_SIM_SPR_SUVEN       0x0001

// ---------------------------------------------------------------------------

// Interrupt controller registers
typedef struct
{

    // Interrupt control register 1-4
    cyg_uint32 icr[4];

    // Interrupt source register
    cyg_uint32 isr;

    // Programmable interrupt transition register
    cyg_uint32 pitr;

    // Programmable interrupt wakeup register
    cyg_uint32 piwr;

    // Gap
    cyg_uint8  _res1[3];

    // Programmable interrupt vector register
    cyg_uint8  ipvr;

} __attribute__ ((aligned (4), packed)) mcf5272_sim_int_t;

// Interrupt controller related macros

#define MCF5272_SIM_PITR_INT1_POS_EDGE         (0x80000000)
#define MCF5272_SIM_PITR_INT2_POS_EDGE         (0x40000000)
#define MCF5272_SIM_PITR_INT3_POS_EDGE         (0x20000000)
#define MCF5272_SIM_PITR_INT4_POS_EDGE         (0x10000000)
#define MCF5272_SIM_PITR_INT5_POS_EDGE         (0x00000040)
#define MCF5272_SIM_PITR_INT6_POS_EDGE         (0x00000020)

#define MCF5272_SIM_PIWR_INT1_WAKE             (0x80000000)
#define MCF5272_SIM_PIWR_INT2_WAKE             (0x40000000)
#define MCF5272_SIM_PIWR_INT3_WAKE             (0x20000000)
#define MCF5272_SIM_PIWR_INT4_WAKE             (0x10000000)
#define MCF5272_SIM_PIWR_TMR0_WAKE             (0x08000000)
#define MCF5272_SIM_PIWR_TMR1_WAKE             (0x04000000)
#define MCF5272_SIM_PIWR_TMR2_WAKE             (0x02000000)
#define MCF5272_SIM_PIWR_TMR3_WAKE             (0x01000000)
#define MCF5272_SIM_PIWR_UART1_WAKE            (0x00800000)
#define MCF5272_SIM_PIWR_UART2_WAKE            (0x00400000)
#define MCF5272_SIM_PIWR_PLIP_WAKE             (0x00200000)
#define MCF5272_SIM_PIWR_PLIA_WAKE             (0x00100000)
#define MCF5272_SIM_PIWR_USB0_WAKE             (0x00080000)
#define MCF5272_SIM_PIWR_USB1_WAKE             (0x00040000)
#define MCF5272_SIM_PIWR_USB2_WAKE             (0x00020000)
#define MCF5272_SIM_PIWR_USB3_WAKE             (0x00010000)
#define MCF5272_SIM_PIWR_USB4_WAKE             (0x00008000)
#define MCF5272_SIM_PIWR_USB5_WAKE             (0x00004000)
#define MCF5272_SIM_PIWR_USB6_WAKE             (0x00002000)
#define MCF5272_SIM_PIWR_USB7_WAKE             (0x00001000)
#define MCF5272_SIM_PIWR_DMA_WAKE              (0x00000800)
#define MCF5272_SIM_PIWR_ERX_WAKE              (0x00000400)
#define MCF5272_SIM_PIWR_ETX_WAKE              (0x00000200)
#define MCF5272_SIM_PIWR_ENTC_WAKE             (0x00000100)
#define MCF5272_SIM_PIWR_QSPI_WAKE             (0x00000080)
#define MCF5272_SIM_PIWR_INT5_WAKE             (0x00000040)
#define MCF5272_SIM_PIWR_INT6_WAKE             (0x00000020)
#define MCF5272_SIM_PIWR_SWTO_WAKE             (0x00000010)

// ---------------------------------------------------------------------------

// Chip-select module
typedef struct
{

    // CS base register
    cyg_uint32 csbr;

    // CS option register
    cyg_uint32 csor;

} __attribute__ ((aligned (4), packed)) mcf5272_sim_cs_t;

// Chip-select modules related macros

#define MCF5272_CS_BR_BASE(a)           ((a) & 0xFFFFF000)

#define MCF5272_CS_OR_MASK_128M         (0xF8000000)
#define MCF5272_CS_OR_MASK_64M          (0xFC000000)
#define MCF5272_CS_OR_MASK_32M          (0xFE000000)
#define MCF5272_CS_OR_MASK_16M          (0xFF000000)
#define MCF5272_CS_OR_MASK_8M           (0xFF800000)
#define MCF5272_CS_OR_MASK_4M           (0xFFC00000)
#define MCF5272_CS_OR_MASK_2M           (0xFFE00000)
#define MCF5272_CS_OR_MASK_1M           (0xFFF00000)
#define MCF5272_CS_OR_MASK_512K         (0xFFF80000)
#define MCF5272_CS_OR_MASK_256K         (0xFFFC0000)
#define MCF5272_CS_OR_MASK_128K         (0xFFFE0000)
#define MCF5272_CS_OR_MASK_64K          (0xFFFF0000)
#define MCF5272_CS_OR_MASK_32K          (0xFFFF8000)
#define MCF5272_CS_OR_MASK_16K          (0xFFFFC000)
#define MCF5272_CS_OR_MASK_8K           (0xFFFFE000)
#define MCF5272_CS_OR_MASK_4K           (0xFFFFF000)
#define MCF5272_CS_OR_WS_MASK           (0x007C)
#define MCF5272_CS_OR_WS(a)             (((a) & 0x1F) << 2)
#define MCF5272_CS_OR_BRST              (0x0100)
#define MCF5272_CS_OR_WR_ONLY           (0x0003)
#define MCF5272_CS_OR_RD_ONLY           (0x0001)

#define MCF5272_CS_BR_PS_8              (0x0100)
#define MCF5272_CS_BR_PS_16             (0x0200)
#define MCF5272_CS_BR_PS_32             (0x0000)
#define MCF5272_CS_BR_PS_LINE           (0x0300)
#define MCF5272_CS_BR_ROM               (0x0000)
#define MCF5272_CS_BR_SRAM              (0x0000)
#define MCF5272_CS_BR_SRAM_8            (0x0C00)
#define MCF5272_CS_BR_SDRAM             (0x0400)
#define MCF5272_CS_BR_ISA               (0x0800)
#define MCF5272_CS_BR_SV                (0x0080)
#define MCF5272_CS_BR_EN                (0x0001)

// ---------------------------------------------------------------------------

// General purpose I/O module
typedef struct
{

    // Port A control register
    cyg_uint32 pacnt;

    // Port A data direction register
    cyg_uint16 paddr;

    // Port A data register
    cyg_uint16 padat;

    // Port B control register
    cyg_uint32 pbcnt;

    // Port B data direction register
    cyg_uint16 pbddr;

    // Port B data register
    cyg_uint16 pbdat;

    // Gap
    cyg_uint32 _res1;

    // Port C data direction register
    cyg_uint16 pcddr;

    // Port C data register
    cyg_uint16 pcdat;

    // Port D control register
    cyg_uint32 pdcnt;

    // Gap
    cyg_uint16 _res2;
    cyg_uint16 _res3;

} __attribute__ ((aligned (4), packed)) mcf5272_sim_gpio_t;

// GPIO ports related macros

#define MCF5272_GPIO_DDR_IN         (0)
#define MCF5272_GPIO_DDR_OUT        (1)

#define MCF5272_GPIO_PBCNT_ETH_EN   (0x55550000)
#define MCF5272_GPIO_PBCNT_ETH_DE   (0x00000000)
#define MCF5272_GPIO_PBCNT_ETH_MSK  (0xFFFF0000)

#define MCF5272_GPIO_PBCNT_TA_EN    (0x00000400)
#define MCF5272_GPIO_PBCNT_TA_DE    (0x00000000)
#define MCF5272_GPIO_PBCNT_TA_MSK   (0x00000C00)

#define MCF5272_GPIO_PBCNT_URT0_EN  (0x00000155)
#define MCF5272_GPIO_PBCNT_URT0_DE  (0x00000000)
#define MCF5272_GPIO_PBCNT_URT0_MSK (0x000003FF)

#define MCF5272_GPIO_PDCNT_INT4_EN  (0x00000C00)
#define MCF5272_GPIO_PDCNT_INT4_DE  (0x00000000)
#define MCF5272_GPIO_PDCNT_INT4_MSK (0x00000C00)

#define MCF5272_GPIO_PDCNT_URT1_EN  (0x000002AA)
#define MCF5272_GPIO_PDCNT_URT1_DE  (0x00000000)
#define MCF5272_GPIO_PDCNT_URT1_MSK (0x000003FF)

// ---------------------------------------------------------------------------

// UART module
typedef struct
{

    // UART mode register
    cyg_uint8 umr;

    // Gap
    cyg_uint8 _res1[3];

    // UART status register (R) and clock-select register (W)
    cyg_uint8 usr_ucsr;

    // Gap
    cyg_uint8 _res2[3];

    // UART command register
    cyg_uint8 ucr;

    // Gap
    cyg_uint8 _res3[3];

    // UART receiver buffers (R) and transmitter buffers (W)
    cyg_uint8 urb_utb;

    // Gap
    cyg_uint8 _res4[3];

    // UART input port change register (R) and auxiliary control register (W)
    cyg_uint8 uipcr_uacr;

    // Gap
    cyg_uint8 _res5[3];

    // UART interrupt status register (R) and interrupt mask register (W)
    cyg_uint8 uisr_uimr;

    // Gap
    cyg_uint8 _res6[3];

    // UART divider upper register
    cyg_uint8 udu;

    // Gap
    cyg_uint8 _res7[3];

    // UART divider lower register
    cyg_uint8 udl;

    // Gap
    cyg_uint8 _res8[3];

    // UART autobaud register MSB
    cyg_uint8 uabu;

    // Gap
    cyg_uint8 _res9[3];

    // UART autobaud register LSB
    cyg_uint8 uabl;

    // Gap
    cyg_uint8 _res10[3];

    // UART transmitter FIFO register
    cyg_uint8 utf;

    // Gap
    cyg_uint8 _res11[3];

    // UART receiver FIFO register
    cyg_uint8 urf;

    // Gap
    cyg_uint8 _res12[3];

    // UART fractional precision divider register
    cyg_uint8 ufpd;

    // Gap
    cyg_uint8 _res13[3];

    // UART input port register
    cyg_uint8 uip;

    // Gap
    cyg_uint8 _res14[3];

    // UART output port register 1
    cyg_uint8 uop1;

    // Gap
    cyg_uint8 _res15[3];

    // UART output port register 0
    cyg_uint8 uop0;

    // Gap
    cyg_uint8 _res16[3];

} __attribute__ ((aligned (4), packed)) mcf5272_uart_t;

// ---------------------------------------------------------------------------

// SDRAM controller
typedef struct
{

    // Gap
    cyg_uint8   _res1[2];

    // SDRAM configuration register
    cyg_uint16  sdcr;

    // Gap
    cyg_uint8   _res2[2];

    // SDRAM timing register
    cyg_uint16  sdtr;

    // Gap
    cyg_uint8   _res3[120];

} __attribute__ ((aligned (4), packed)) mcf5272_sim_sdramctrl_t;

// SDRAM controller related macros

#define MCF5272_SDRAMC_SDCCR_MCAS_A7    (0x0 << 13)
#define MCF5272_SDRAMC_SDCCR_MCAS_A8    (0x1 << 13)
#define MCF5272_SDRAMC_SDCCR_MCAS_A9    (0x2 << 13)
#define MCF5272_SDRAMC_SDCCR_MCAS_A10   (0x3 << 13)
#define MCF5272_SDRAMC_SDCCR_BALOC_A19  (0x0 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A20  (0x1 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A21  (0x2 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A22  (0x3 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A23  (0x4 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A24  (0x5 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A25  (0x6 << 8)
#define MCF5272_SDRAMC_SDCCR_BALOC_A26  (0x7 << 8)
#define MCF5272_SDRAMC_SDCCR_GSL        (0x00000080)
#define MCF5272_SDRAMC_SDCCR_REG        (0x00000010)
#define MCF5272_SDRAMC_SDCCR_INV        (0x00000008)
#define MCF5272_SDRAMC_SDCCR_SLEEP      (0x00000004)
#define MCF5272_SDRAMC_SDCCR_ACT        (0x00000002)
#define MCF5272_SDRAMC_SDCCR_INIT       (0x00000001)

#define MCF5272_SDRAMC_SDCTR_RTP_66MHz  (0x3D << 10)
#define MCF5272_SDRAMC_SDCTR_RTP_48MHz  (0x2B << 10)
#define MCF5272_SDRAMC_SDCTR_RTP_33MHz  (0x1D << 10)
#define MCF5272_SDRAMC_SDCTR_RTP_25MHz  (0x16 << 10)
#define MCF5272_SDRAMC_SDCTR_RC(x)      ((x & 0x3) << 8)
#define MCF5272_SDRAMC_SDCTR_RP(x)      ((x & 0x3) << 4)
#define MCF5272_SDRAMC_SDCTR_RCD(x)     ((x & 0x3) << 2)
#define MCF5272_SDRAMC_SDCTR_CLT_2      (0x00000001)
#define MCF5272_SDRAMC_SDCTR_CLT_3      (0x00000002)
#define MCF5272_SDRAMC_SDCTR_CLT_4      (0x00000003)

// ---------------------------------------------------------------------------

// Timer module
typedef struct
{

    // Timer mode register
    cyg_uint16 tmr;

    // Gap
    cyg_uint16 _res1;

    // Timer reference register
    cyg_uint16 trr;

    // Gap
    cyg_uint16 _res2;

    // Timer capture register
    cyg_uint16 tcap;

    // Gap
    cyg_uint16 _res3;

    // Timer counter register
    cyg_uint16 tcn;

    // Gap
    cyg_uint16 _res4;

    // Timer event register
    cyg_uint16 ter;

    // Gap
    cyg_uint16 _res5;

    // Gap
    cyg_uint32 _res6[3];

} __attribute__ ((aligned (4), packed)) mcf5272_timer_t;

// Related macros

#define MCF5272_TIMER_TMR_PS            (0xFF00)
#define MCF5272_TIMER_TMR_PS_BIT        (8)
#define MCF5272_TIMER_TMR_CE            (0x00C0)
#define MCF5272_TIMER_TMR_CE_BIT        (6)
#define MCF5272_TIMER_TMR_OM            (0x0020)
#define MCF5272_TIMER_TMR_OM_BIT        (5)
#define MCF5272_TIMER_TMR_ORI           (0x0010)
#define MCF5272_TIMER_TMR_ORI_BIT       (4)
#define MCF5272_TIMER_TMR_FRR           (0x0008)
#define MCF5272_TIMER_TMR_FRR_BIT       (3)
#define MCF5272_TIMER_TMR_CLK           (0x0006)
#define MCF5272_TIMER_TMR_CLK_BIT       (1)
#define MCF5272_TIMER_TMR_RST           (0x0001)
#define MCF5272_TIMER_TMR_RST_BIT       (0)
#define MCF5272_TIMER_TER_REF           (0x0002)
#define MCF5272_TIMER_TER_REF_BIT       (1)
#define MCF5272_TIMER_TER_CAP           (0x0001)
#define MCF5272_TIMER_TER_CAP_BIT       (0)

// ---------------------------------------------------------------------------

// Watchdog timer
typedef struct
{

    // Watchdog reset reference register
    cyg_uint16 wrrr;

    // Gap
    cyg_uint16 _res1;

    // Watchdog interrupt reference register
    cyg_uint16 wirr;

    // Gap
    cyg_uint16 _res2;

    // Watchdog counter register
    cyg_uint16 wcr;

    // Gap
    cyg_uint16 _res3;

    // Watchdog event register
    cyg_uint16 wer;

    // Gap
    cyg_uint16 _res4;

    // Gap
    cyg_uint32 _res5[28];

} __attribute__ ((aligned (4), packed)) mcf5272_sim_wdtmr_t;

// ---------------------------------------------------------------------------

// Fast Ethernet Controller module
typedef struct
{

    // Ethernet control register
    cyg_uint32 ecr;

    // Ethernet interrupt event register
    cyg_uint32 eir;

    // Ethernet interrupt mask register
    cyg_uint32 eimr;

    // Interrupt vector status register
    cyg_uint32 ivsr;
                                        
    // Receive descriptor active register
    cyg_uint32 rdar;
                                        
    // Transmit descriptor active register
    cyg_uint32 tdar;
                                        
    // Gap
    cyg_uint8 _res2[0x0880 - 0x0858];

    // MII management frame register
    cyg_uint32 mmfr;

    // MII speed control register
    cyg_uint32 mscr;

    // Gap
    cyg_uint8 _res3[0x08cc - 0x0888];

    // FIFO receive bound register
    cyg_uint32 frbr;

    // FIFO receive start register
    cyg_uint32 frsr;

    // Gap
    cyg_uint8 _res4[0x08e4 - 0x08d4];

    // Transmit FIFO watermark
    cyg_uint32 tfwr;

    // Gap
    cyg_uint8 _res5[0x08ec - 0x08e8];

    // Transmit FIFO start register
    cyg_uint32 tfsr;

    // Gap
    cyg_uint8 _res6[0x0944 - 0x08f0];

    // Receive control register
    cyg_uint32 rcr;

    // Maximum frame length register
    cyg_uint32 mflr;

    // Gap
    cyg_uint8 _res7[0x0984 - 0x094c];

    // Transmit control register
    cyg_uint32 tcr;

    // Gap
    cyg_uint8 _res8[0x0c00 - 0x0988];

    // RAM perfect match address low register
    cyg_uint32 malr;

    // RAM perfect match address high register
    cyg_uint32 maur;

    // Hash table high register
    cyg_uint32 htur;

    // Hash table low register
    cyg_uint32 htlr;

    // Pointer to receive descriptor ring
    cyg_uint32 erdsr;

    // Pointer to transmit descriptor ring
    cyg_uint32 etdsr;
                                        
    // Maximum receive buffer size
    cyg_uint32 emrbr;

    // Gap
    cyg_uint8 _res9[0x0c40 - 0x0c1c];

    // FIFO RAM space
    cyg_uint8 efifo[0x0e00 - 0x0c40];

    // Gap
    cyg_uint8 _res10[0x1000 - 0x0e00];

} __attribute__ ((aligned (4), packed)) mcf5272_fec_t;

// ---------------------------------------------------------------------------

// On-chip peripherals: this structure defines each register's offset from the
// current value of the MBAR register.
typedef struct
{

    // 0x0000: System Integration Module (SIM) general configuration registers
    mcf5272_sim_cfg_t cfg;

    // 0x0020: SIM interrupt controller registers
    mcf5272_sim_int_t intc;

    // 0x0040: SIM chip-select modules
    mcf5272_sim_cs_t cs[8];             

    // 0x0080: SIM general purpose I/O control registers
    mcf5272_sim_gpio_t gpio;

    // 0x00a0: QSPI module
    // TODO: a specific data structure is needed
    cyg_uint32 qspi[8];

    // 0x00c0: PWM module
    // TODO: a specific data structure is needed
    cyg_uint32 pwm[8];

    // 0x00e0: DMA controller
    // TODO: a specific data structure is needed
    cyg_uint32 dmac[8];

    // 0x0100: UART modules
    mcf5272_uart_t uart[2];

    // 0x0180: SIM SDRAM controller
    mcf5272_sim_sdramctrl_t sdramc;

    // 0x0200: timer module
    mcf5272_timer_t timer[4];

    // 0x0280: SIM watchdog timer module
    mcf5272_sim_wdtmr_t wdtimer;

    // 0x0300: physical layer interface controller
    // TODO: a specific data structure is needed
    cyg_uint32 plic[336];
                                        
    // 0x0840: ethernet module
    mcf5272_fec_t fec;

    // 0x1000: USB module
    // TODO: a specific data structure is needed
    cyg_uint32 usb[512];

} __attribute__ ((aligned (4), packed)) mcf5272_devs_t;

// ---------------------------------------------------------------------------
// End of mcf5272_devs.h
#endif // CYGONCE_MCF5272_DEVS_H
