//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
//
//=============================================================================

// - pjo, 28 sep 1999
// - Copied ARM version for use with i386/pc.

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
// Author(s):   gthomas (based on the old ARM/AEB hal_stub.c)
// Contributors:gthomas, jskov, pjo, nickg
// Date:        1999-02-15
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros

#include <cyg/hal/plf_misc.h>

//-----------------------------------------------------------------------------
// Connect our VSR to the exception vectors.

externC void __default_exception_vsr(void);

void hal_pc_stubs_init(void)
{
    int i ;
    for(i=0; i<=31; i++)
        HAL_VSR_SET(i,&__default_exception_vsr,NULL);	

}


//-----------------------------------------------------------------------------

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
// End of plf_stub.c
