#ifndef CYGONCE_HAL_PLATFORM_SETUP_H
#define CYGONCE_HAL_PLATFORM_SETUP_H

/*=============================================================================
//
//      hal_platform_setup.h
//
//      Platform specific support for HAL (assembly code)
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    gthomas
// Contributors: gthomas
// Date:        1999-04-13
// Purpose:     ARM/PID platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/


// Define macro used to diddle the LEDs during early initialization.
// Can use r0+r1. Argument in \x.
// Control the LEDs PP0-PP3. This requires the jumpers on pins 9-16 to
// be set on LK11 in order to be visible. Otherwise the parallel port
// data pins are diddled instead.
#ifdef CYGHWR_HAL_ARM_PID_DIAG_LEDS
#define CYGHWR_LED_MACRO                                                  \
        ldr     r0,=0x0d800040                                           ;\
        mov     r1, #((15 & (\x)) << 4)                                  ;\
        strb    r1, [r0]                                                 ;
#endif

#define MEM_RESET         0x0B000020  // Write to this location to enable RAM

#define PLATFORM_SETUP1                                 \
	ldr	r0,=MEM_RESET	/* enable RAM */       ;\
	str	r0,[r0]

/*---------------------------------------------------------------------------*/
/* end of hal_platform_setup.h                                               */
#endif /* CYGONCE_HAL_PLATFORM_SETUP_H */
