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
// Date:         1999-04-21
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the CMA230 are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_unused     0
#define CYGNUM_HAL_INTERRUPT_ABORT      1
#define CYGNUM_HAL_INTERRUPT_SERIAL_B   2
#define CYGNUM_HAL_INTERRUPT_SERIAL_A   3
#define CYGNUM_HAL_INTERRUPT_TIMER      4
#define CYGNUM_HAL_INTERRUPT_ETHER_PP   5  // Ethernet or Parallel port
#define CYGNUM_HAL_INTERRUPT_SLOT1      6
#define CYGNUM_HAL_INTERRUPT_SLOT2      7
#define CYGNUM_HAL_INTERRUPT_SLOT3      8

#define CYGNUM_HAL_ISR_MIN              0
#define CYGNUM_HAL_ISR_MAX              8
#define CYGNUM_HAL_ISR_COUNT            (CYGNUM_HAL_ISR_MAX+1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC        CYGNUM_HAL_INTERRUPT_TIMER

#endif // CYGONCE_HAL_PLATFORM_INTS_H
