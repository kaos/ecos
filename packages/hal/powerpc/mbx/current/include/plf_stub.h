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
// Purpose:     Platform HAL stub support for PowerPC/MBX board.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_powerpc_mbx.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/ppc_stub.h>           // architecture stub support

#include <cyg/hal/quicc/quicc_smc1.h>   // QUICC serial IO support

//----------------------------------------------------------------------------
// Define serial stuff. All comes from the quicc_smc1.c file.

#include <cyg/hal/quicc/quicc_smc1.h>

#define HAL_STUB_PLATFORM_INIT_SERIAL()     cyg_quicc_init_smc1()
#define HAL_STUB_PLATFORM_GET_CHAR()        cyg_quicc_smc1_uart_rcvchar()
#define HAL_STUB_PLATFORM_PUT_CHAR(c)       cyg_quicc_smc1_uart_putchar((c))

#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) CYG_UNUSED_PARAM(int, (baud))

//----------------------------------------------------------------------------
// Stub initializer.
externC void hal_mbx_set_led( int val );
#ifdef CYG_HAL_STARTUP_ROM
# define HAL_STUB_PLATFORM_INIT() hal_mbx_set_led( 4 )
// to distinguish eCos stub ROM ready state from either CygMon or app.
#endif

//----------------------------------------------------------------------------
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#define HAL_STUB_PLATFORM_INIT_BREAK_IRQ()    CYG_EMPTY_STATEMENT
#endif

//----------------------------------------------------------------------------
// Reset.
#ifdef CYGSEM_HAL_ROM_MONITOR
// Just call the ROM's entry point. Not as safe as a reset, but the MBX
// doesn't have a watchdog, so this'll have to do.
externC void _start(void);
#define HAL_STUB_PLATFORM_RESET() _start()
#else
#define HAL_STUB_PLATFORM_RESET() CYG_EMPTY_STATEMENT
#endif

#define HAL_STUB_PLATFORM_INTERRUPTIBLE  0

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
