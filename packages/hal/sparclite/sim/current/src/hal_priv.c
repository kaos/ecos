//===========================================================================
//
//      hal_priv.c
//
//      SPARClite Architecture sim-specific private variables
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    hmt
// Contributors: hmt
// Date:         1999-02-20
// Purpose:      private vars for SPARClite sim.
//              
//####DESCRIPTIONEND####
//
//===========================================================================


#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_arch.h>

// ------------------------------------------------------------------------
// Clock static to keep period recorded.
cyg_int32 cyg_hal_sparclite_clock_period = 0;

// ------------------------------------------------------------------------
// Board specific startups.

extern void hal_board_prestart( void );
extern void hal_board_poststart( void );

#define WATCHDOG()     *((int *)0x01f80064) = (int)0x00000000 // watchdog off

void hal_board_prestart( void )
{
    WATCHDOG();                         // Turn off the ERC32 watchdog.
}

void hal_board_poststart( void )
{
    HAL_ENABLE_INTERRUPTS();
    // OK to do this post constructors, and good for testing.
}

// EOF hal_priv.c
