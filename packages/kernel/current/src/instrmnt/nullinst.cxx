//==========================================================================
//
//      instrmnt/null.cxx
//
//      Null instrumentation functions
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg
// Contributors:        nickg
// Date:        1997-10-24
// Purpose:     Instrumentation functions
// Description: The functions in this file are null implementations of the
//              standard instrumentation functions. These can be used to
//              eliminate all instrumentation without recompiling.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/kernel.h>

#include <cyg/kernel/ktypes.h>             // base kernel types
#include <cyg/infra/cyg_trac.h>            // tracing macros
#include <cyg/infra/cyg_ass.h>             // assertion macros
#include <cyg/kernel/instrmnt.h>           // instrumentation

#ifdef CYGPKG_KERNEL_INSTRUMENT

// -------------------------------------------------------------------------

void cyg_instrument( cyg_uint32 type, CYG_ADDRWORD arg1, CYG_ADDRWORD arg2 )
{
    return;
}

// -------------------------------------------------------------------------

#endif //CYGPKG_KERNEL_INSTRUMENT

// EOF instrmnt/null.cxx
