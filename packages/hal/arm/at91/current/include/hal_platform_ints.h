#ifndef CYGONCE_HAL_PLATFORM_INTS_H
#define CYGONCE_HAL_PLATFORM_INTS_H
//==========================================================================
//
//      hal_platform_ints.h
//
//      HAL Interrupt and clock assignments for AT91/EB40
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:         2001-07-12
// Purpose:      Define Interrupt support
// Description:  The interrupt specifics for the AT91/EB40 board/platform are
//               defined here.
//              
// Usage:        #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_USART0            2
#define CYGNUM_HAL_INTERRUPT_USART1            3
#define CYGNUM_HAL_INTERRUPT_TIMER0            4
#define CYGNUM_HAL_INTERRUPT_TIMER1            5
#define CYGNUM_HAL_INTERRUPT_TIMER2            6
#define CYGNUM_HAL_INTERRUPT_WATCHDOG          7
#define CYGNUM_HAL_INTERRUPT_PIO               8
#define CYGNUM_HAL_INTERRUPT_EXT0              16
#define CYGNUM_HAL_INTERRUPT_EXT1              17
#define CYGNUM_HAL_INTERRUPT_EXT2              18

#define CYGNUM_HAL_ISR_MIN                     2
#define CYGNUM_HAL_ISR_MAX                     18
// Note: extra slots (0,1) to avoid messing with vector translation
#define CYGNUM_HAL_ISR_COUNT                   (CYGNUM_HAL_ISR_MAX + 1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_INTERRUPT_TIMER0


//----------------------------------------------------------------------------
// Reset.
#define HAL_PLATFORM_RESET()

#define HAL_PLATFORM_RESET_ENTRY 0x01010000

#endif // CYGONCE_HAL_PLATFORM_INTS_H
