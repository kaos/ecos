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
// Author(s):    gthomas
// Contributors: gthomas
// Date:         1999-04-21
// Purpose:      Define Interrupt support
// Description:  The interrupt details for the CL7xxx are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_unused     0
#define CYGNUM_HAL_INTERRUPT_EXTFIQ     1
#define CYGNUM_HAL_INTERRUPT_BLINT      2
#define CYGNUM_HAL_INTERRUPT_WEINT      3
#define CYGNUM_HAL_INTERRUPT_MCINT      4
#define CYGNUM_HAL_INTERRUPT_CSINT      5
#define CYGNUM_HAL_INTERRUPT_EINT1      6    
#define CYGNUM_HAL_INTERRUPT_EINT2      7
#define CYGNUM_HAL_INTERRUPT_EINT3      8
#define CYGNUM_HAL_INTERRUPT_TC1OI      9
#define CYGNUM_HAL_INTERRUPT_TC2OI      10
#define CYGNUM_HAL_INTERRUPT_RTCMI      11
#define CYGNUM_HAL_INTERRUPT_TINT       12
#define CYGNUM_HAL_INTERRUPT_UTXINT1    13
#define CYGNUM_HAL_INTERRUPT_URXINT1    14
#define CYGNUM_HAL_INTERRUPT_UMSINT     15
#define CYGNUM_HAL_INTERRUPT_SSEOTI     16
#define CYGNUM_HAL_INTERRUPT_KBDINT     17
#define CYGNUM_HAL_INTERRUPT_SS2RX      18
#define CYGNUM_HAL_INTERRUPT_SS2TX      19
#define CYGNUM_HAL_INTERRUPT_UTXINT2    20
#define CYGNUM_HAL_INTERRUPT_URXINT2    21
#if defined(__EDB7211)
#define CYGNUM_HAL_INTERRUPT_MCPINT     22
#endif

#define CYGNUM_HAL_ISR_MIN              0
#if defined(__CL7111)
#define CYGNUM_HAL_ISR_MAX              21
#else
#define CYGNUM_HAL_ISR_MAX              22
#endif
#define CYGNUM_HAL_ISR_COUNT            (CYGNUM_HAL_ISR_MAX+1)

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC        CYGNUM_HAL_INTERRUPT_TC2OI

#endif // CYGONCE_HAL_PLATFORM_INTS_H
