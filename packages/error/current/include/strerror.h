#ifndef CYGONCE_ERROR_STRERROR_H
#define CYGONCE_ERROR_STRERROR_H
/*========================================================================
//
//      strerror.h
//
//      ISO C strerror function
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
// Purpose:       This file provides the strerror() function
//                required by ISO C and POSIX 1003.1
// Description: 
// Usage:         Do not include this file directly - use #include <string.h>
//
//####DESCRIPTIONEND####
//
//======================================================================*/

/* CONFIGURATION */

#include <pkgconf/error.h>         /* Configuration header */

/* INCLUDES */

#include <cyg/error/codes.h>       /* for Cyg_ErrNo */

#ifdef __cplusplus
extern "C" {
#endif

/* FUNCTION PROTOTYPES */

/* Standard strerror() function as described by ISO C 1990 chap. 7.11.6.2.
 * This is normally provided by <string.h>
 */

extern char *
strerror( Cyg_ErrNo );

/* prototype for the actual implementation. Equivalent to the above, but
 * used internally by this product in preference
 */

extern char *
__strerror( Cyg_ErrNo );

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* CYGONCE_ERROR_STRERROR_H multiple inclusion protection */

/* EOF strerror.h */
