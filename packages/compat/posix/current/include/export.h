#ifndef CYGONCE_POSIX_EXPORT_H
#define CYGONCE_POSIX_EXPORT_H
//=============================================================================
//
//      export.h
//
//      POSIX export header
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-09-18
// Purpose:       POSIX export header
// Description:   This header contains definitions that the POSIX package exports
//                to other packages. These are generally interfaces that are not
//                provided by the public API.
//                
//              
// Usage:
//              #ifdef CYGPKG_POSIX
//              #include <export.h>
//              #endif
//              ...
//              
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/kernel.h>
#include <pkgconf/posix.h>

#include <cyg/infra/cyg_type.h>

#include <stddef.h>             // NULL, size_t

#include <limits.h>

#include <sys/types.h>

#include <sched.h>              // SCHED_*

//=============================================================================
// POSIX API function management.
// These macros should be inserted near the start and all returns of
// any function that is part of the POSIX API.

__externC void cyg_posix_function_start();
__externC void cyg_posix_function_finish();

#define CYG_POSIX_FUNCTION_START() cyg_posix_function_start()

#define CYG_POSIX_FUNCTION_FINISH() cyg_posix_function_finish()

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_POSIX_EXPORT_H
// End of export.h
