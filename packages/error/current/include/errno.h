#ifndef CYGONCE_ERROR_ERRNO_H
#define CYGONCE_ERROR_ERRNO_H
/*========================================================================
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
// Date:          2000-04-14
// Purpose:       This file provides the errno variable (or more strictly
//                expression) required by ISO C and POSIX 1003.1
// Description: 
// Usage:         Do not include this file directly - use #include <errno.h>
//
//####DESCRIPTIONEND####
//
//======================================================================*/

/* CONFIGURATION */

#include <pkgconf/error.h>         /* Configuration header */

#ifdef CYGPKG_ERROR_ERRNO

/* INCLUDES */

#include <cyg/error/codes.h>       /* for Cyg_ErrNo */

#ifdef __cplusplus
extern "C" {
#endif

/* FUNCTION PROTOTYPES */


#ifdef CYGSEM_ERROR_PER_THREAD_ERRNO

extern Cyg_ErrNo * const
cyg_error_get_errno_p( void ) __attribute__((const));

#endif /* ifdef CYGSEM_ERROR_PER_THREAD_ERRNO */


/* VARIABLES */

#ifdef CYGSEM_ERROR_PER_THREAD_ERRNO
# define errno (*cyg_error_get_errno_p())  /* Per-thread error status */
#else
extern Cyg_ErrNo errno;                /* Global error status */
#endif /* ifdef CYGSEM_ERROR_PER_THREAD_ERRNO */

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* ifdef CYGPKG_ERROR_ERRNO */

#endif /* CYGONCE_ERROR_ERRNO_H multiple inclusion protection */

/* EOF errno.h */
