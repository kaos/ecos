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
// Date:        1999-02-12
// Purpose:     Platform HAL stub support for PowerPC/FADS board.
// Usage:       #include <cyg/hal/plf_stub.h>
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_powerpc_fads.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/infra/cyg_type.h>         // CYG_UNUSED_PARAM

#include <cyg/hal/ppc_stub.h>           // architecture stub support

//----------------------------------------------------------------------------
// Define serial stuff. All comes from the quicc_smc.c file.

#include <cyg/hal/quicc_smc2.h>

#define HAL_STUB_PLATFORM_INIT_SERIAL()       cyg_smc2_init(38400)
#define HAL_STUB_PLATFORM_GET_CHAR()          cyg_smc2_getchar()
#define HAL_STUB_PLATFORM_PUT_CHAR(c)         cyg_smc2_putchar((c))
#define HAL_STUB_PLATFORM_SET_BAUD_RATE(baud) cyg_smc2_init((baud))

//----------------------------------------------------------------------------
// Stub initializer.
#define HAL_STUB_PLATFORM_STUBS_INIT()        CYG_EMPTY_STATEMENT

//----------------------------------------------------------------------------
// Reset.
#define HAL_STUB_PLATFORM_RESET()             CYG_EMPTY_STATEMENT

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

//-----------------------------------------------------------------------------
#endif // CYGONCE_HAL_PLF_STUB_H
// End of plf_stub.h
