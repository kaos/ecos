#ifndef CYGONCE_LIBC_ERRNO_H
#define CYGONCE_LIBC_ERRNO_H
//========================================================================
//
//      errno.h
//
//      ISO C errno variable and constants
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-01-21
// Purpose:       This file provides the errno variable (or more strictly
//                expression) and the E* error codes required by ISO C
//                and POSIX 1003.1
// Description: 
// Usage:         #include <errno.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions etc.
#include <cyg/error/codes.h>       // Common error code definitions

// FUNCTION PROTOTYPES

#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO

externC Cyg_ErrNo * const
cyg_libc_get_errno_p( void ) CYGBLD_ATTRIB_CONST;

#endif // ifdef CYGSEM_LIBC_PER_THREAD_ERRNO

// VARIABLES

#ifdef CYGSEM_LIBC_PER_THREAD_ERRNO

# define errno (*cyg_libc_get_errno_p())  // Global error status

#else

externC Cyg_ErrNo errno;            // Global error status

#endif // ifdef CYGSEM_LIBC_PER_THREAD_ERRNO


#endif // CYGONCE_LIBC_ERRNO_H multiple inclusion protection

// EOF errno.h
