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
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Description:  The interrupt details for the PID are defined here.
// Usage:
//               #include <cyg/hal/hal_platform_ints.h>
//               ...
//              
//
//####DESCRIPTIONEND####
//
//==========================================================================

#define CYGNUM_HAL_INTERRUPT_unused               0
#define CYGNUM_HAL_INTERRUPT_PROGRAMMED_INTERRUPT 1
#define CYGNUM_HAL_INTERRUPT_DEBUG_Rx             2
#define CYGNUM_HAL_INTERRUPT_DEBUG_Tx             3
#define CYGNUM_HAL_INTERRUPT_TIMER1               4
#define CYGNUM_HAL_INTERRUPT_TIMER2               5
#define CYGNUM_HAL_INTERRUPT_PC_SLOTA             6
#define CYGNUM_HAL_INTERRUPT_PC_SLOTB             7
#define CYGNUM_HAL_INTERRUPT_SERIALA              8
#define CYGNUM_HAL_INTERRUPT_SERIALB              9
#define CYGNUM_HAL_INTERRUPT_PARALLEL_PORT        10
#define CYGNUM_HAL_INTERRUPT_ASB0                 11
#define CYGNUM_HAL_INTERRUPT_ASB1                 12
#define CYGNUM_HAL_INTERRUPT_APB0                 13
#define CYGNUM_HAL_INTERRUPT_APB1                 14
#define CYGNUM_HAL_INTERRUPT_APB2                 15
#define CYGNUM_HAL_INTERRUPT_EXTERNAL_FIQ         16

#define CYGNUM_HAL_ISR_MIN                        0
#define CYGNUM_HAL_ISR_MAX                        16
#define CYGNUM_HAL_ISR_COUNT                      17

// The vector used by the Real time clock
#define CYGNUM_HAL_INTERRUPT_RTC                  CYGNUM_HAL_INTERRUPT_TIMER2

#endif // CYGONCE_HAL_PLATFORM_INTS_H
