#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock assignments for AEB-1
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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-02-20
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for the AEB-1 board/platform are
//               defined here.
//              
// Usage:        #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_EXT0              0
#define CYGNUM_HAL_INTERRUPT_EXT1              1
#define CYGNUM_HAL_INTERRUPT_EXT2              2
#define CYGNUM_HAL_INTERRUPT_EXT3              3
#define CYGNUM_HAL_INTERRUPT_UART0_TX          4
#define CYGNUM_HAL_INTERRUPT_UART0_RX          5
#define CYGNUM_HAL_INTERRUPT_UART1_TX          6
#define CYGNUM_HAL_INTERRUPT_UART1_RX          7
#define CYGNUM_HAL_INTERRUPT_GDMA0             8
#define CYGNUM_HAL_INTERRUPT_GDMA1             9
#define CYGNUM_HAL_INTERRUPT_TIMER0           10
#define CYGNUM_HAL_INTERRUPT_TIMER1           11
#define CYGNUM_HAL_INTERRUPT_HDLCA_TX         12
#define CYGNUM_HAL_INTERRUPT_HDLCA_RX         13
#define CYGNUM_HAL_INTERRUPT_HDLCB_TX         14
#define CYGNUM_HAL_INTERRUPT_HDLCB_RX         15
#define CYGNUM_HAL_INTERRUPT_ETH_BDMA_TX      16
#define CYGNUM_HAL_INTERRUPT_ETH_BDMA_RX      17
#define CYGNUM_HAL_INTERRUPT_ETH_MAC_TX       18
#define CYGNUM_HAL_INTERRUPT_ETH_MAC_RX       19
#define CYGNUM_HAL_INTERRUPT_I2C              20

#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     CYGNUM_HAL_INTERRUPT_I2C
#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX - CYGNUM_HAL_ISR_MIN + 1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_INTERRUPT_TIMER0


//----------------------------------------------------------------------------
// Reset.
#define HAL_PLATFORM_RESET()

#define HAL_PLATFORM_RESET_ENTRY 0x01820000

#endif // CYGONCE_HAL_PLATFORM_INTS_H
