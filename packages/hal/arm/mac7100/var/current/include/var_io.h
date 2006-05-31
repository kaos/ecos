#ifndef CYGONCE_HAL_VAR_IO_H
#define CYGONCE_HAL_VAR_IO_H
//=============================================================================
//
//      var_io.h
//
//      Variant specific registers
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   Ilija Koco <ilijak@siva.com.mk> 
// Contributors:
// Date:        2006-02-03
// Purpose:     MAC7100 variant specific registers
// Description: based on freescale's mac7100.h
// Usage:       #include <cyg/hal/var_io.h>
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <cyg/hal/plf_io.h>

#if !defined HAL_IO_MACROS_NO_ADDRESS_MUNGING
#define HAL_IO_MACROS_NO_ADDRESS_MUNGING 1
#endif // HAL_IO_MACROS_NO_ADDRESS_MUNGING

// *********************************************************************
//
// INTC Module
//
// *********************************************************************

// Interrupt Controller Definitions 
#define MAC7100_INTC_BASE      (0xFC048000)

#define MAC7100_IPRH_OFFSET    (0x0000)
#define MAC7100_IPRL_OFFSET    (0x0004)
#define MAC7100_IMRH_OFFSET    (0x0008)
#define MAC7100_IMRL_OFFSET    (0x000C)
#define MAC7100_INTFRCH_OFFSET (0x0010)
#define MAC7100_INTFRCL_OFFSET (0x0014)
#define MAC7100_ICONFIG_OFFSET (0x001B)
#define MAC7100_SIMR_OFFSET    (0x001C)
#define MAC7100_CIMR_OFFSET    (0x001D)
#define MAC7100_CLMASK_OFFSET  (0x001E) // CLMASK - Current Level Mask Register
#define MAC7100_SLMASK_OFFSET  (0x001F) // SLMASK - Saved Level Mask Register 
#define MAC7100_ICR_OFFSET     (0x0040)
#define MAC7100_IRQIACK_OFFSET (0x00EC)
#define MAC7100_FIQIACK_OFFSET (0x00F0)

#define MAC7100_INTC_IPRH(intc_base)    (intc_base + MAC7100_IPRH_OFFSET)
#define MAC7100_INTC_IPRL(intc_base)    (intc_base + MAC7100_IPRL_OFFSET)
#define MAC7100_INTC_IMRH(intc_base)    (intc_base + MAC7100_IMRH_OFFSET)
#define MAC7100_INTC_IMRL(intc_base)    (intc_base + MAC7100_IMRL_OFFSET)
#define MAC7100_INTC_INTFRC(intc_base)  (intc_base + MAC7100_INTFRCH_OFFSET)
#define MAC7100_INTC_INTFRCH(intc_base) (intc_base + MAC7100_INTFRCH_OFFSET)
#define MAC7100_INTC_INTFRCL(intc_base) (intc_base + MAC7100_INTFRCL_OFFSET)
#define MAC7100_INTC_ICONFIG(intc_base) (intc_base + MAC7100_ICONFIG_OFFSET)
#define MAC7100_INTC_IRQIACK(intc_base) (intc_base + MAC7100_IRQIACK_OFFSET)
#define MAC7100_INTC_FIQIACK(intc_base) (intc_base + MAC7100_FIQIACK_OFFSET)
#define MAC7100_INTC_ICR(intc_base,src) (intc_base + MAC7100_ICR_OFFSET + src)
#define MAC7100_INTC_SIMR(intc_base)    (intc_base + MAC7100_SIMR_OFFSET)
#define MAC7100_INTC_CIMR(intc_base)    (intc_base + MAC7100_CIMR_OFFSET)
#define MAC7100_INTC_CLMASK(intc_base)  (intc_base + MAC7100_CLMASK_OFFSET)
#define MAC7100_INTC_SLMASK(intc_base)  (intc_base + MAC7100_SLMASK_OFFSET)

#define MAC7100_INTC_INT_LEVEL(lev) (lev)

// hardware interrupt source vector numbers 
#define MAC7100_EDMA0_IV        (0)
#define MAC7100_EDMA1_IV        (1)
#define MAC7100_EDMA2_IV        (2)
#define MAC7100_EDMA3_IV        (3)
#define MAC7100_EDMA4_IV        (4)
#define MAC7100_EDMA5_IV        (5)
#define MAC7100_EDMA6_IV        (6)
#define MAC7100_EDMA7_IV        (7)
#define MAC7100_EDMA8_IV        (8)
#define MAC7100_EDMA9_IV        (9)
#define MAC7100_EDMA10_IV       (10)
#define MAC7100_EDMA11_IV       (11)
#define MAC7100_EDMA12_IV       (12)
#define MAC7100_EDMA13_IV       (13)
#define MAC7100_EDMA14_IV       (14)
#define MAC7100_EDMA15_IV       (15)
#define MAC7100_EDMA_Error_IV   (16)
#define MAC7100_MCM_SWT_IV      (17)
#define MAC7100_CRG_IV          (18)
#define MAC7100_PIT1_IV         (19)
#define MAC7100_PIT2_IV         (20)
#define MAC7100_PIT3_IV         (21)
#define MAC7100_PIT4_RTI_IV     (22)
#define MAC7100_VREG_IV         (23)
#define MAC7100_CAN_A_MB_IV     (24)
#define MAC7100_CAN_A_MB14_IV   (25)
#define MAC7100_CAN_A_Error_IV  (26)
#define MAC7100_CAN_B_MB_IV     (27)
#define MAC7100_CAN_B_MB14_IV   (28)
#define MAC7100_CAN_B_Error_IV  (29)
#define MAC7100_CAN_C_MB_IV     (30)
#define MAC7100_CAN_C_MB14_IV   (31)
#define MAC7100_CAN_C_Error_IV  (32)
#define MAC7100_CAN_D_MB_IV     (33)
#define MAC7100_CAN_D_MB14_IV   (34)
#define MAC7100_CAN_D_Error_IV  (35)
#define MAC7100_I2C_IV          (36)
#define MAC7100_DSPI_A_IV       (37)
#define MAC7100_DSPI_B_IV       (38)
#define MAC7100_ESCI_A_IV       (39)
#define MAC7100_ESCI_B_IV       (40)
#define MAC7100_ESCI_C_IV       (41)
#define MAC7100_ESCI_D_IV       (42)
#define MAC7100_EMIOS0_IV       (43)
#define MAC7100_EMIOS1_IV       (44)
#define MAC7100_EMIOS2_IV       (45)
#define MAC7100_EMIOS3_IV       (46)
#define MAC7100_EMIOS4_IV       (47)
#define MAC7100_EMIOS5_IV       (48)
#define MAC7100_EMIOS6_IV       (49)
#define MAC7100_EMIOS7_IV       (50)
#define MAC7100_EMIOS8_IV       (51)
#define MAC7100_EMIOS9_IV       (52)
#define MAC7100_EMIOS10_IV      (53)
#define MAC7100_EMIOS11_IV      (54)
#define MAC7100_EMIOS12_IV      (55)
#define MAC7100_EMIOS13_IV      (56)
#define MAC7100_EMIOS14_IV      (57)
#define MAC7100_EMIOS15_IV      (58)
#define MAC7100_ATD_IV          (59)
#define MAC7100_CFM_IV          (60)
#define MAC7100_PIM_IV          (61)
#define MAC7100_IRQ_IV          (62)
#define MAC7100_XIRQ_IV         (63)

#define MAC7100_IRQ_SPURIOUS     (-1)

// *******************************************************************
//
// eSCI Module 
//      Note: eSCI definitions are in cyg/devs/ser_esci.h
// *******************************************************************
#define CYGADDR_IO_SERIAL_FREESCALE_ESCI_A_BASE 0xFC0C4000
#define CYGNUM_IO_SERIAL_FREESCALE_ESCI_A_INT_VECTOR MAC7100_ESCI_A_IV

#define CYGADDR_IO_SERIAL_FREESCALE_ESCI_B_BASE 0xFC0C8000
#define CYGNUM_IO_SERIAL_FREESCALE_ESCI_B_INT_VECTOR MAC7100_ESCI_B_IV

#define CYGADDR_IO_SERIAL_FREESCALE_ESCI_C_BASE 0xFC0CC000
#define CYGNUM_IO_SERIAL_FREESCALE_ESCI_C_INT_VECTOR MAC7100_ESCI_C_IV

#define CYGADDR_IO_SERIAL_FREESCALE_ESCI_D_BASE 0xFC0D0000
#define CYGNUM_IO_SERIAL_FREESCALE_ESCI_D_INT_VECTOR MAC7100_ESCI_D_IV

// *********************************************************************
//
// PIT Module
//
// *********************************************************************

// Periodic Interrupt Timer Module Definitions 

#define MAC7100_PIT_BASE         (0xFC08C000)
#define MAC7100_TLVAL0_OFFSET    (0x0000)
#define MAC7100_TVAL0_OFFSET     (0x0080)

#define MAC7100_PIT_TLVAL(pit_base,chan)        \
  (pit_base + MAC7100_TLVAL0_OFFSET + (4 * chan))
#define MAC7100_PIT_TVAL(pit_base,chan)   \
  (pit_base + MAC7100_TVAL0_OFFSET + (4 * chan))

#define MAC7100_PITFLG_OFFSET         (0x0100)
#define MAC7100_PITINTEN_OFFSET       (0x0104)
#define MAC7100_PITINTSEL_OFFSET      (0x0108)
#define MAC7100_PITEN_OFFSET          (0x010C)
#define MAC7100_PITCTRL_OFFSET        (0x0110)

#define MAC7100_PIT_FLAG_RTIF         (0x00000001)
#define MAC7100_PIT_FLAG_TIF(chan)    (0x00000001 << chan)
#define MAC7100_PIT_INTSEL_ISEL(chan) (0x00000001 << chan)
#define MAC7100_PIT_INTEN_RTIE        (0x00000001)
#define MAC7100_PIT_INTEN_TIE(chan)   (0x00000001 << chan)

#define MAC7100_PIT_EN_RTIEN          (0x00000001)
#define MAC7100_PIT_EN_PEN(chan)      (0x00000001 << chan)

#define MAC7100_PIT_FLG(pit_base)    (pit_base + MAC7100_PITFLG_OFFSET)
#define MAC7100_PIT_INTEN(pit_base)  (pit_base + MAC7100_PITINTEN_OFFSET)
#define MAC7100_PIT_INTSEL(pit_base) (pit_base + MAC7100_PITINTSEL_OFFSET)
#define MAC7100_PIT_EN(pit_base)     (pit_base + MAC7100_PITEN_OFFSET)

#define MAC7100_PIT_CTRL(pit_base)   (pit_base + MAC7100_PITCTRL_OFFSET)
#define MAC7100_PIT_MDIS    (0x01000000)


// *********************************************************************
//
// PIM Module
//
// *********************************************************************/

#define MAC7100_PIM_BASE  (0xFC0E8000)

#define MAC7100_PORT_A_OFFSET (0x000)
#define MAC7100_PORT_B_OFFSET (0x040)
#define MAC7100_PORT_C_OFFSET (0x080)
#define MAC7100_PORT_D_OFFSET (0x0C0)
#define MAC7100_PORT_E_OFFSET (0x100)
#define MAC7100_PORT_F_OFFSET (0x140)
#define MAC7100_PORT_G_OFFSET (0x180)
#define MAC7100_PORT_H_OFFSET (0x1C0)
#define MAC7100_PORT_I_OFFSET (0x200)

// PORT Pin Configuration Registers 
#define MAC7100_PIM_CONFIG(port,pin)  (MAC7100_PIM_BASE+port+((pin)*2)) 
// Port Wide Interrupt Flag Register 
#define MAC7100_PIM_PORTIFR(port)     (MAC7100_PIM_BASE+port+0x20) 
// Port Wide Data Read/Write Register 
#define MAC7100_PIM_PORTDATA(port)    (MAC7100_PIM_BASE+port+0x24) 
// Port Wide Input Register 
#define MAC7100_PIM_PORTIR(port)      (MAC7100_PIM_BASE+port+0x26) 
// Port Pin Data Registers 
#define MAC7100_PIM_DATA(port,pin)    (MAC7100_PIM_BASE+port+0x28+pin) 

// Global Interrupt Status Register 
#define MAC7100_PIM_GLBLINT    (MAC7100_PIM_BASE+0x03C0) 
// PIM Configuration Register 
#define MAC7100_PIM_PIMCONFIG  (MAC7100_PIM_BASE+0x03C2) 
// TDI Pin Configuration Register 
#define MAC7100_PIM_CONFIG_TDI (MAC7100_PIM_BASE+0x03C4) 
// TDO Pin Configuration Register 
#define MAC7100_PIM_CONFIG_TDO (MAC7100_PIM_BASE+0x03C6) 
// TMS Pin Configuration Register 
#define MAC7100_PIM_CONFIG_TMS (MAC7100_PIM_BASE+0x03C8) 
// TCK Pin Configuration Register 
#define MAC7100_PIM_CONFIG_TCK (MAC7100_PIM_BASE+0x03CA) 
// TA Pin Configuration Register 
#define MAC7100_PIM_CONFIG_TA  (MAC7100_PIM_BASE+0x03CC) 

// Bit definitions and macros for PIM_PA_CONFIGn 
//  Pin Interrupt Flag Register 
#define MAC7100_PIM_PIFR            (0x0001)       
//  Pin Interrupt Enable Register 
#define MAC7100_PIM_PIER            (0x0002)       
//  Pull-up/down Enable Register 
#define MAC7100_PIM_PULL(x)         (((x)&0x0003)<<2)    
//  Reduced Drive Strength Register 
#define MAC7100_PIM_RDR             (0x0010)       
//  Open Drain Enable Register 
#define MAC7100_PIM_ODER            (0x0020)       
//  Data Direction Register 
#define MAC7100_PIM_DDR             (0x0040)       
#define MAC7100_PIM_MODE            (0x0080)       
#define MAC7100_PIM_MODE_PERIPHERAL MAC7100_PIM_MODE

// Bit definitions and macros for PIM_GLBLINT 
//  Interrupt Pending 
#define MAC7100_PIM_INT_PENDING(x)  (((x)&0x01FF)<<0)    

// Bit definitions and macros for PIM_PIMCONFIG 
//  Clock Enable for the EIM module 
#define MAC7100_PIM_PORTHSEL        (0x0001)       
//  Port H Select 
#define MAC7100_PIM_EIMCLKEN        (0x0002)       

#define MAC7100_PIM_PORT32IR(port32ir)  (MAC7100_PIM_BASE+0x03E0+port32ir)

#define MAC7100_PIM_PORT32IR_AB (0x00)
#define MAC7100_PIM_PORT32IR_CD (0x04)
#define MAC7100_PIM_PORT32IR_EF (0x08)
#define MAC7100_PIM_PORT32IR_GH (0x0C)
#define MAC7100_PIM_PORT32IR_BC (0x10)
#define MAC7100_PIM_PORT32IR_DE (0x14)
#define MAC7100_PIM_PORT32IR_FG (0x18)
#define MAC7100_PIM_PORT32IR_HI (0x1C)


// ********************************************************************
//
// CRG Module
//
// ********************************************************************

// Register read/write macros 
#define MAC7100_CRG_BASE   0xFC088000 // SYNR - Synthesizer Register 
#define MAC7100_CRG_SYNR   0xFC088000 // SYNR - Synthesizer Register 
#define MAC7100_CRG_REFDV  0xFC088001 // REFDV - Reference Divider Register 
#define MAC7100_CRG_CTFLG  0xFC088002 // CTFLG - Test Flags Register (reserved)
#define MAC7100_CRG_CRGFLG 0xFC088003 // CRGFLG - Flags Register 
#define MAC7100_CRG_CRGINT 0xFC088004 // CRGINT - Interrupt Enable Register 
#define MAC7100_CRG_CLKSEL 0xFC088005 // CLKSEL - Clock Select Register 
#define MAC7100_CRG_PLLCTL 0xFC088006 // PLLCTL - PLL Control Register 
#define MAC7100_CRG_SDMCTL 0xFC088007 // SDMCTL - STOP/DOZE Control Register 
#define MAC7100_CRG_BDMCTL 0xFC088008 // BDMCTL - BDM Control Register 
#define MAC7100_CRG_FORBYP 0xFC088009 // FORBYP - Force and Bypass Test 
#define MAC7100_CRG_CTCTL  0xFC08800A // CTCTL - Test Control Register (resvd) 

// Bit definitions and macros for CRG_SYNR 
#define MAC7100_CRG_SYN(x) (((x)&0x3F)<<0)  //  Synthesizer Count value 

// Bit definitions and macros for CRG_REFDV 
#define MAC7100_CRG_REFD(x) (((x)&0x0F)<<0)  //Reference divider 

// Bit definitions and macros for CRG_CRGFLG 
#define MAC7100_CRG_SCM     (0x01)         //Self Clock Mode Status 
#define MAC7100_CRG_SCMIF   (0x02)         //Self Clock Mode Interrupt Flag 
#define MAC7100_CRG_TRACK   (0x04)         //Track Status 
#define MAC7100_CRG_LOCK    (0x08)         //Lock Status 
#define MAC7100_CRG_LOCKIF  (0x10)         //PLL Lock Interrupt Flag 
#define MAC7100_CRG_LVRF    (0x20)         //Low Voltage Reset Flag 
#define MAC7100_CRG_PORF    (0x40)         //Power on Reset Flag 
#define MAC7100_CRG_STPEF   (0x80)         //Stop Entry Flag 

// Bit definitions and macros for CRG_CRGINT 
#define MAC7100_CRG_SCMIE   (0x02)         //Self Clock Mode Interrupt Enable 
#define MAC7100_CRG_LOCKIE  (0x10)         //Lock Interrupt Enable 

// Bit definitions and macros for CRG_CLKSEL 
#define MAC7100_CRG_SWTDOZE  (0x01)         //SWT stops in Doze Mode 
#define MAC7100_CRG_RTIDOZE  (0x02)         /*  RTI stops in Doze Mode */
#define MAC7100_CRG_PLLDOZE  (0x08)         /*  PLL stops in Doze Mode */
#define MAC7100_CRG_DOZE_ROA (0x10)         /*  Reduced Osc Amp in Doze Mode */
#define MAC7100_CRG_PSTP     (0x40)         /*  Pseudo Stop */
#define MAC7100_CRG_PLLSEL   (0x80)         /*  PLL Select */

// Bit definitions and macros for CRG_PLLCTL 
#define MAC7100_CRG_SCME     (0x01)         //Self Clock Mode Enable 
#define MAC7100_CRG_PWE      (0x02)         //SWT Enable during Pseudo Stop 
#define MAC7100_CRG_PRE      (0x04)         //RTI Enable during Pseudo Stop 
#define MAC7100_CRG_FSTWKP   (0x08)         //Fast Wake-up from Full Stop Bit 
#define MAC7100_CRG_ACQ      (0x10)         //Acquisition 
#define MAC7100_CRG_AUTO     (0x20)         //Automatic Bandwidth Control 
#define MAC7100_CRG_PLLON    (0x40)         //Phase Lock Loop On 
#define MAC7100_CRG_CME      (0x80)         //Clock Monitor Enable 

// Bit definitions and macros for CRG_SDMCTL 
#define MAC7100_CRG_STOP     (0x01)         //STOP mode 
#define MAC7100_CRG_DOZE     (0x02)         //DOZE mode 

// Bit definitions and macros for CRG_BDMCTL 
#define MAC7100_CRG_RSBCK    (0x40)         //SWT & RTI stop in Active BDM mode


// *********************************************************************
//
// MCM Module
//
// *********************************************************************

// Register read/write macros
#define MAC7100_MCM_PCT    0xFC040000 // 
#define MAC7100_MCM_REV    0xFC040002 // 
#define MAC7100_MCM_AMC    0xFC040004 // 
#define MAC7100_MCM_ASC    0xFC040006 // 
#define MAC7100_MCM_IMC    0xFC040008 // 
// MRSR - Miscellaneous Reset Status Register 
#define MAC7100_MCM_MRSR   0xFC04000F 
// MWCR - Miscellaneous Wakeup Control Register 
#define MAC7100_MCM_MWCR   0xFC040013 
// MSWTCR - Miscellaneous Software Watchdog Timer Control Register 
#define MAC7100_MCM_MSWTCR 0xFC040016 
// MSWTSR - Miscellaneous Software Watchdog Timer Service Register 
#define MAC7100_MCM_MSWTSR 0xFC04001B 
// MSWTIR - Miscellaneous Software Watchdog Timer Interrupt Register 
#define MAC7100_MCM_MSWTIR 0xFC04001F 
// AAMR - AXBS Address Map Register 
#define MAC7100_MCM_AAMR   0xFC040020 
// CFADR - Core Fault Address Register 
#define MAC7100_MCM_CFADR  0xFC040070 
// CFLOC - Core Fault Location Register 
#define MAC7100_MCM_CFLOC  0xFC040076 
// CFATR - Core Fault Attributes Register 
#define MAC7100_MCM_CFATR  0xFC040077 
// CFDTR - Core Fault Data Register 
#define MAC7100_MCM_CFDTR  0xFC04007C 

// Bit definitions and macros for MCM_AMC 

//  AXBS Master Configuration 
#define MAC7100_MCM_AXMC(x)    (((x)&0x00FF)<<0)    

// Bit definitions and macros for MCM_ASC 
//  AXBS Slave Configuration 
#define MAC7100_MCM_AXSC(x)    (((x)&0x00FF)<<0)    
#define MAC7100_MCM_DP64       (0x8000)             //  64-bit Datapath 

// Bit definitions and macros for MCM_MRSR 
#define MAC7100_MCM_SWTR       (0x20)         //  Watchdog Timer Reset 
#define MAC7100_MCM_DIR        (0x40)         //  Device Input Reset 
#define MAC7100_MCM_POR        (0x80)         //  Power-On Reset 

// Bit definitions and macros for MCM_MWCR 
#define MAC7100_MCM_PRILVL(x)  (((x)&0x0F)<<0)  //  Interrupt Priority Level 
#define MAC7100_MCM_ENBWCR     (0x80)           //  Enable WCR

// Bit definitions and macros for MCM_MSWTCR 
#define MAC7100_MCM_SWT(x)     (((x)&0x001F)<<0)  //  Watchdog Time-Out Period 
#define MAC7100_MCM_SWRI(x)    (((x)&0x0003)<<5)  //  Watchdog Reset/Interrupt 
#define MAC7100_MCM_SWE        (0x0080)       //  Watchdog Enable 
#define MAC7100_MCM_SWRWH      (0x0100)       //  Watchdog Run While Halted 
#define MAC7100_MCM_SWCIN16    (0x0200)       //  Force SWT CarryIn16 
#define MAC7100_MCM_RO         (0x8000)       //  Read-Only 

// Bit definitions and macros for MCM_MSWTIR 
#define MAC7100_MCM_SWTIC      (0x01)         //  Watchdog Interrupt Flag 

// Bit definitions and macros for MCM_AAMR


//  Address 0 Slave Number 
#define MAC7100_MCM_ASLAVE(adr_reg,sl_n) (((sl_n)&0x00000007)<<(adr_reg*4) 
//  Enable Address Region 0 
#define MAC7100_MCM_EA(adr_reg)        (0x00000008<<(adr_reg*4))  

// Bit definitions and macros for MCM_CFLOC 
#define MAC7100_MCM_LOCALERR               (0x80)  //  Bus Error Indicator 

// Bit definitions and macros for MCM_CFATR 
//  Protection fault type 
#define MAC7100_MCM_PROTECTION(x)          (((x)&0x0F)<<0)  
//  8-16-32-64-bit core access 
#define MAC7100_MCM_SIZE(x)                (((x)&0x07)<<4)  
//  Core read/write access 
#define MAC7100_MCM_WRITE                  (0x80)         

//=============================================================================
// FIQ interrupt vector which is shared by all HAL variants.

#define CYGNUM_HAL_INTERRUPT_FIQ 0
#endif // CYGONCE_HAL_VAR_IO_H
//-----------------------------------------------------------------------------
// end of var_io.h
