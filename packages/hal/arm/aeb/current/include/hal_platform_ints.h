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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
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
#define CYGNUM_HAL_INTERRUPT_TIMER0            6
#define CYGNUM_HAL_INTERRUPT_TIMER1            7
#define CYGNUM_HAL_INTERRUPT_TIMER2            8
#define CYGNUM_HAL_INTERRUPT_UART0             9
#define CYGNUM_HAL_INTERRUPT_UART1            10
#define CYGNUM_HAL_INTERRUPT_UART2            11
#define CYGNUM_HAL_INTERRUPT_WATCHDOG         12
#define CYGNUM_HAL_INTERRUPT_unused           13

#define CYGNUM_HAL_ISR_MIN                     0
#define CYGNUM_HAL_ISR_MAX                     13
#define CYGNUM_HAL_ISR_COUNT                   14

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC               CYGNUM_HAL_INTERRUPT_TIMER0

#endif // CYGONCE_HAL_PLATFORM_INTS_H
