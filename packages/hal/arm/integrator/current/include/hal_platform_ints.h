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
// Author(s):    David A Rusling
// Contributors: Philippe Robin
// Date:         November 7, 2000
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the INTEGRATOR are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_SOFTINT                     0
#define CYGNUM_HAL_INTERRUPT_UARTINT0                    1
#define CYGNUM_HAL_INTERRUPT_UARTINT1                    2
#define CYGNUM_HAL_INTERRUPT_KMIINT0                     3
#define CYGNUM_HAL_INTERRUPT_KMIINT1                     4
#define CYGNUM_HAL_INTERRUPT_TIMERINT0                   5
#define CYGNUM_HAL_INTERRUPT_TIMERINT1                   6
#define CYGNUM_HAL_INTERRUPT_TIMERINT2                   7
#define CYGNUM_HAL_INTERRUPT_RTCINT                      8
#define CYGNUM_HAL_INTERRUPT_EXPINT0                     9
#define CYGNUM_HAL_INTERRUPT_EXPINT1                     10
#define CYGNUM_HAL_INTERRUPT_EXPINT2                     11
#define CYGNUM_HAL_INTERRUPT_EXPINT3                     12
#define CYGNUM_HAL_INTERRUPT_PCIINT0                     13
#define CYGNUM_HAL_INTERRUPT_PCIINT1                     14
#define CYGNUM_HAL_INTERRUPT_PCIINT2                     15
#define CYGNUM_HAL_INTERRUPT_PCIINT3                     16
#define CYGNUM_HAL_INTERRUPT_V3INT                       17
#define CYGNUM_HAL_INTERRUPT_CPINT0                      18
#define CYGNUM_HAL_INTERRUPT_CPINT1                      19
#define CYGNUM_HAL_INTERRUPT_LBUSTIMEOUT                 20
#define CYGNUM_HAL_INTERRUPT_APCINT                      21
#define CYGNUM_HAL_INTERRUPT_CM_SOFTINT                  24
#define CYGNUM_HAL_INTERRUPT_CM_COMMRX                   25
#define CYGNUM_HAL_INTERRUPT_CM_COMMTX                   26

#define CYGNUM_HAL_ISR_MIN                        0
#define CYGNUM_HAL_ISR_MAX                        26
#define CYGNUM_HAL_ISR_COUNT                      27

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC                  CYGNUM_HAL_INTERRUPT_TIMERINT2

//----------------------------------------------------------------------------
// Reset.

#define HAL_PLATFORM_RESET() CYG_EMPTY_STATEMENT

#define HAL_PLATFORM_RESET_ENTRY 0x4000000

#endif // CYGONCE_HAL_PLATFORM_INTS_H
