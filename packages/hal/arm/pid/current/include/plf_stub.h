#ifndef CYGONCE_HAL_PLF_STUB_H
#define CYGONCE_HAL_PLF_STUB_H

//=============================================================================
//
//      plf_stub.h
//
//      Platform header for GDB stub support.
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-02-15
// Purpose:     Platform HAL stub support for ARM/PID boards.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_pid.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/arm_stub.h>           // architecture stub support

//----------------------------------------------------------------------------
// Define serial stuff.

extern void hal_pid_init_serial(void);
extern int  hal_pid_get_char(void);
extern void hal_pid_put_char(int c);
extern int  hal_pid_interruptible(int);

#define HAL_STUB_PLATFORM_INIT_SERIAL()       hal_pid_init_serial()
#define HAL_STUB_PLATFORM_GET_CHAR()          hal_pid_get_char()
#define HAL_STUB_PLATFORM_PUT_CHAR(c)         hal_pid_put_char((c))
#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))
#define HAL_STUB_PLATFORM_INTERRUPTIBLE       (&hal_pid_interruptible)
#define HAL_STUB_PLATFORM_INIT_BREAK_IRQ()    CYG_EMPTY_STATEMENT

//----------------------------------------------------------------------------
// Stub initializer.
#define HAL_STUB_PLATFORM_STUBS_INIT()        CYG_EMPTY_STATEMENT

//----------------------------------------------------------------------------
// Reset.
#ifdef CYG_HAL_STARTUP_STUBS
// Just call the ROM's entry point. Not as safe as a reset, but the PID
// doesn't have a watchdog, so this'll have to do.
externC void reset_vector(void);
#define HAL_STUB_PLATFORM_RESET() reset_vector()
#else
#define HAL_STUB_PLATFORM_RESET() CYG_EMPTY_STATEMENT
#endif

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
