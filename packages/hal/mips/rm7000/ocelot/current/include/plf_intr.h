#ifndef CYGONCE_HAL_PLF_INTR_H
#define CYGONCE_HAL_PLF_INTR_H

//==========================================================================
//
//      plf_intr.h
//
//      Ocelot Interrupt and clock support
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
// Author(s):    jskov
// Contributors: jskov, nickg
// Date:         2000-05-09
// Purpose:      Define Interrupt support
// Description:  The macros defined here provide the HAL APIs for handling
//               interrupts and the clock for the REF4955 board.
//              
// Usage:
//              #include <cyg/hal/plf_intr.h>
//              ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

//--------------------------------------------------------------------------
// Interrupt vectors.

#ifndef CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

// The first 10 correspond to the interrupt lines in the status/cause regs
#define CYGNUM_HAL_INTERRUPT_ETH0               0
#define CYGNUM_HAL_INTERRUPT_ETH1               1
#define CYGNUM_HAL_INTERRUPT_UART1              2
#define CYGNUM_HAL_INTERRUPT_21555              3
#define CYGNUM_HAL_INTERRUPT_GALILEO            4
#define CYGNUM_HAL_INTERRUPT_COMPARE            5
#define CYGNUM_HAL_INTERRUPT_PMC1               6
#define CYGNUM_HAL_INTERRUPT_PMC2               7
#define CYGNUM_HAL_INTERRUPT_CPCI               8
#define CYGNUM_HAL_INTERRUPT_UART2              9

// PCI interrupts are hardwired for the devices connected to the bus
#define CYGNUM_HAL_INTERRUPT_PCI_INTA           CYGNUM_HAL_INTERRUPT_GALILEO
#define CYGNUM_HAL_INTERRUPT_PCI_INTB           CYGNUM_HAL_INTERRUPT_ETH0
#define CYGNUM_HAL_INTERRUPT_PCI_INTC           CYGNUM_HAL_INTERRUPT_GALILEO
#define CYGNUM_HAL_INTERRUPT_PCI_INTD           CYGNUM_HAL_INTERRUPT_GALILEO

// Min/Max ISR numbers and how many there are
#define CYGNUM_HAL_ISR_MIN                     CYGNUM_HAL_INTERRUPT_ETH0
#define CYGNUM_HAL_ISR_MAX                     CYGNUM_HAL_INTERRUPT_UART2
#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC            CYGNUM_HAL_INTERRUPT_COMPARE

#define CYGHWR_HAL_INTERRUPT_VECTORS_DEFINED

#endif

//--------------------------------------------------------------------------
// Interrupt controller information

// V320USC 
#define CYGARC_REG_INT_STAT   0xb80000ec

#define CYGARC_REG_INT_CFG0   0xb80000e0
#define CYGARC_REG_INT_CFG1   0xb80000e4
#define CYGARC_REG_INT_CFG2   0xb80000e8
#define CYGARC_REG_INT_CFG3   0xb8000158

#define CYGARC_REG_INT_CFG_INT0 0x00000100
#define CYGARC_REG_INT_CFG_INT1 0x00000200
#define CYGARC_REG_INT_CFG_INT2 0x00000400
#define CYGARC_REG_INT_CFG_INT3 0x00000800


// FPGA
#define CYGARC_REG_PCI_STAT   0xb5300000
#define CYGARC_REG_PCI_MASK   0xb5300030

#define CYGARC_REG_IO_STAT    0xb5300010
#define CYGARC_REG_IO_MASK    0xb5300040


//----------------------------------------------------------------------------
// Reset.
// Uses Secondary Reset Bit in 21555. Don't know where it is mapped though.
#define CYGARC_REG_BOARD_RESET 0x????????

#define HAL_PLATFORM_RESET() /* HAL_WRITE_UINT8(CYGARC_REG_BOARD_RESET,1) */

#define HAL_PLATFORM_RESET_ENTRY 0xbfc00000

//--------------------------------------------------------------------------
#endif // ifndef CYGONCE_HAL_PLF_INTR_H
// End of plf_intr.h
