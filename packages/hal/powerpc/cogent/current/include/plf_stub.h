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
// Contributors:jskov
// Date:        1999-02-12
// Purpose:     Platform HAL stub support for PowerPC/Cogent boards.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/hal_powerpc_cogent.h>

#ifdef CYGPKG_IO_SERIAL
#include <pkgconf/io_serial.h>
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM, externC

#include <cyg/hal/ppc_stub.h>           // architecture stub support

//----------------------------------------------------------------------------
// Define serial stuff.
externC void hal_cma_stub_init_serial( void );
externC int  hal_cma_stub_get_char( void );
externC void hal_cma_stub_put_char( int c );
externC int  hal_cma_stub_interruptible( int state );
externC void hal_cma_stub_init_break_irq( void );

// Only define init function if no serial driver is used on the same port.
#if ((CYGHWR_HAL_POWERPC_COGENT_GDB_PORT == 0                   \
      && defined(CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_A)) ||  \
     (CYGHWR_HAL_POWERPC_COGENT_GDB_PORT == 1                   \
      && defined(CYGPKG_IO_SERIAL_POWERPC_COGENT_SERIAL_B)))
#define HAL_STUB_PLATFORM_INIT_SERIAL()       CYG_EMPTY_STATEMENT
#else
#define HAL_STUB_PLATFORM_INIT_SERIAL()       hal_cma_stub_init_serial()
#endif

#define HAL_STUB_PLATFORM_GET_CHAR()          hal_cma_stub_get_char()
#define HAL_STUB_PLATFORM_PUT_CHAR(c)         hal_cma_stub_put_char((c))
#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))
#define HAL_STUB_PLATFORM_INTERRUPTIBLE       (&hal_cma_stub_interruptible)
#define HAL_STUB_PLATFORM_INIT_BREAK_IRQ()    hal_cma_stub_init_break_irq()

//----------------------------------------------------------------------------
// Stub initializer.
#ifdef CYGSEM_HAL_ROM_MONITOR
extern void diag_init (void);
# define HAL_STUB_PLATFORM_INIT()              diag_init()
#else
# define HAL_STUB_PLATFORM_INIT()              CYG_EMPTY_STATEMENT
#endif

//----------------------------------------------------------------------------
// Reset.
#define HAL_STUB_PLATFORM_RESET()             CYG_EMPTY_STATEMENT

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
