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
// Author(s):    David A Rusling
// Contributors: Philippe Robin
// Date:         November 7, 2000
// Purpose:     ARM INTEGRATOR platform specific support routines
// Description: 
// Usage:       #include <cyg/hal/hal_platform_setup.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <cyg/hal/hal_integrator.h>

// Define macro used to diddle the LEDs during early initialization.
// Can use r0+r1. Argument in \x.
// Control the LEDs PP0-PP3. This requires the jumpers on pins 9-16 to
// be set on LK11 in order to be visible. Otherwise the parallel port
// data pins are diddled instead.

#ifdef CYGHWR_HAL_ARM_INTEGRATOR_DIAG_LEDS
#define CYGHWR_LED_MACRO				\
        mov     r1, #(15 & (\x))			;\
        ldr     r0,=INTEGRATOR_DBG_BASE			;\
        strb    r1, [r0, #INTEGRATOR_DBG_LEDS_OFFSET]                    ;
#endif

#define PLATFORM_SETUP1                                                   \
        ldr     r0,=INTEGRATOR_DBG_BASE                                  ;\
        ldr     r1,=0                                                    ;\
        strb    r1, [r0, #INTEGRATOR_DBG_LEDS_OFFSET]                    ;\
	ldr	r0, =INTEGRATOR_HDR_BASE                                 ;\
	ldr	r1, [r0, #INTEGRATOR_HDR_CTRL_OFFSET]                    ;\
	orr	r1, r1, #INTEGRATOR_HDR_CTRL_REMAP	                 ;\
        str     r1, [r0, #INTEGRATOR_HDR_CTRL_OFFSET]                    ;\
	ldr	r1, =INTEGRATOR_IRQCONT_BASE                             ;\
        ldr	r0, =0xFFFFFFFF                                          ;\
	str	r0, [r1, #INTEGRATOR_IRQENABLECLEAR]             	 ;\
	str	r0, [r1, #INTEGRATOR_FIQENABLECLEAR]             	 ;\
        ldr     r0,=INTEGRATOR_DBG_BASE                                  ;\
        ldr     r1,=0xF                                                  ;\
        strb    r1, [r0, #INTEGRATOR_DBG_LEDS_OFFSET]                    ;

/*---------------------------------------------------------------------------*/
/* end of hal_platform_setup.h                                               */
#endif /* CYGONCE_HAL_PLATFORM_SETUP_H */
