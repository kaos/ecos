//==========================================================================
//
//        gdb_module.c
//
//        Cogent CMA230 board GDB stubs (module wrapper)
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
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1999-05-11
// Description:   Cirrus CMA230 module for eCos GDB stubs
//####DESCRIPTIONEND####

//
// This is the module 'wrapper' for the GDB stubs
//

#include <pkgconf/hal.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/hal/hal_stub.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

void cyg_start (void)
{
    for(;;) breakpoint();
}
#else

#include <cyg/infra/testcase.h>

void cyg_start (void)
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH("N/A: Stand-alone GDB stubs only");
}
#endif



