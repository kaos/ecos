#ifndef CYGONCE_UTSNAME_H
#define CYGONCE_UTSNAME_H
//=============================================================================
//
//      utsname.h
//
//      POSIX utsname header
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
// Date:          2000-03-17
// Purpose:       POSIX utsname header
// Description:   This header contains all the definitions needed to support
//                utsnames under eCos. The reader is referred to the POSIX
//                standard or equivalent documentation for details of the
//                functionality contained herein.
//              
// Usage:
//              #include <utsname.h>
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

#include <cyg/hal/hal_arch.h>   // CYGNUM_HAL_STACK_SIZE_MINIMUM

#include <stddef.h>             // NULL, size_t

#include <limits.h>

#include <sys/types.h>

#include <sched.h>              // SCHED_*

//=============================================================================
// UTSName structure

struct utsname
{
    char        sysname[CYG_POSIX_UTSNAME_LENGTH];
    char        nodename[CYG_POSIX_UTSNAME_NODENAME_LENGTH];
    char        release[CYG_POSIX_UTSNAME_LENGTH];
    char        version[CYG_POSIX_UTSNAME_LENGTH];
    char        machine[CYG_POSIX_UTSNAME_LENGTH];
};

//=============================================================================
// uname() function

__externC int uname( struct utsname *name );

//-----------------------------------------------------------------------------
#endif // ifndef CYGONCE_UTSNAME_H
// End of utsname.h
