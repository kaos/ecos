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
// Author(s):   jskov
// Contributors:jskov, gthomas, pjo, nickg
// Date:        1999-02-15
// Purpose:     Platform HAL stub support for ARM/AEB boards.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_i386_pc.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/i386_stub.h>           // architecture stub support

//----------------------------------------------------------------------------
// Define serial stuff.

externC void hal_pc_init_serial(void);
externC int  hal_pc_get_char(void);
externC void hal_pc_put_char(int c);
externC int  hal_pc_interruptable(int);
externC void hal_pc_stubs_init(void) ;
externC void hal_pc_reset(void) ;

#define HAL_STUB_PLATFORM_INIT_SERIAL()       hal_pc_init_serial()
#define HAL_STUB_PLATFORM_GET_CHAR()          hal_pc_get_char()
#define HAL_STUB_PLATFORM_PUT_CHAR(c)         hal_pc_put_char((c))
#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))
#define HAL_STUB_PLATFORM_INTERRUPTABLE       (&hal_pc_interruptable)
#define HAL_STUB_PLATFORM_INTERRUPTIBLE       HAL_STUB_PLATFORM_INTERRUPTABLE
#define HAL_STUB_PLATFORM_INIT_BREAK_IRQ()    CYG_EMPTY_STATEMENT

//----------------------------------------------------------------------------
// Stub initializer.

#define HAL_STUB_PLATFORM_INIT()        hal_pc_stubs_init()

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
