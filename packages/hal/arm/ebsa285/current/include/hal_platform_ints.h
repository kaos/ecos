#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock support
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
// Author(s):    hmt
// Contributors: hmt
// Date:         1999-04-21
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the EBSA285 are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_reserved0                   0
#define CYGNUM_HAL_INTERRUPT_SOFT_IRQ                    1
#define CYGNUM_HAL_INTERRUPT_SERIAL_RX                   2
#define CYGNUM_HAL_INTERRUPT_SERIAL_TX                   3
#define CYGNUM_HAL_INTERRUPT_TIMER_1                     4
#define CYGNUM_HAL_INTERRUPT_TIMER_2                     5
#define CYGNUM_HAL_INTERRUPT_TIMER_3                     6    
#define CYGNUM_HAL_INTERRUPT_TIMER_4                     7
#define CYGNUM_HAL_INTERRUPT_IRQ_IN_0                    8
#define CYGNUM_HAL_INTERRUPT_IRQ_IN_1                    9
#define CYGNUM_HAL_INTERRUPT_IRQ_IN_2                    10
#define CYGNUM_HAL_INTERRUPT_IRQ_IN_3                    11
#define CYGNUM_HAL_INTERRUPT_XBUS_CS_0                   12
#define CYGNUM_HAL_INTERRUPT_XBUS_CS_1                   13
#define CYGNUM_HAL_INTERRUPT_XBUS_CS_2                   14
#define CYGNUM_HAL_INTERRUPT_DOORBELL                    15
#define CYGNUM_HAL_INTERRUPT_DMA_1                       16
#define CYGNUM_HAL_INTERRUPT_DMA_2                       17
#define CYGNUM_HAL_INTERRUPT_PCI_IRQ                     18
#define CYGNUM_HAL_INTERRUPT_PMCSR                       19
#define CYGNUM_HAL_INTERRUPT_reserved20                  20
#define CYGNUM_HAL_INTERRUPT_reserved21                  21
#define CYGNUM_HAL_INTERRUPT_BIST                        22
#define CYGNUM_HAL_INTERRUPT_SERR                        23
#define CYGNUM_HAL_INTERRUPT_SDRAM_PARITY                24
#define CYGNUM_HAL_INTERRUPT_I2O_POST                    25
#define CYGNUM_HAL_INTERRUPT_reserved26                  26
#define CYGNUM_HAL_INTERRUPT_DISCARD_TIMER               27
#define CYGNUM_HAL_INTERRUPT_PCI_DATA_PARITY             28
#define CYGNUM_HAL_INTERRUPT_PCI_MASTER_ABORT            29
#define CYGNUM_HAL_INTERRUPT_PCI_TARGET_ABORT            30
#define CYGNUM_HAL_INTERRUPT_PCI_PARITY_ERROR            31

#define CYGNUM_HAL_ISR_MIN               0
#define CYGNUM_HAL_ISR_MAX              31

#define CYGNUM_HAL_ISR_COUNT            (CYGNUM_HAL_ISR_MAX+1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC        CYGNUM_HAL_INTERRUPT_TIMER_3

#endif // CYGONCE_HAL_PLATFORM_INTS_H
