#ifndef CYGONCE_ISO_STDIO_H
#define CYGONCE_ISO_STDIO_H
/*========================================================================
//
//      stdio.h
//
//      ISO standard I/O functions
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
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides the stdio functions required by 
//                ISO C and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <stdio.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get NULL and size_t from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#define __need_size_t
#include <stddef.h>

#ifdef CYGINT_ISO_STDIO_FILETYPES
# ifdef CYGBLD_ISO_STDIO_FILETYPES_HEADER
#  include CYGBLD_ISO_STDIO_FILETYPES_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_STREAMS
# ifdef CYGBLD_ISO_STDIO_STREAMS_HEADER
#  include CYGBLD_ISO_STDIO_STREAMS_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_FILEOPS
# ifdef CYGBLD_ISO_STDIO_FILEOPS_HEADER
#  include CYGBLD_ISO_STDIO_FILEOPS_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_FILEACCESS
# ifdef CYGBLD_ISO_STDIO_FILEACCESS_HEADER
#  include CYGBLD_ISO_STDIO_FILEACCESS_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_FORMATTED_IO
# ifdef CYGBLD_ISO_STDIO_FORMATTED_IO_HEADER
#  include CYGBLD_ISO_STDIO_FORMATTED_IO_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_CHAR_IO
# ifdef CYGBLD_ISO_STDIO_CHAR_IO_HEADER
#  include CYGBLD_ISO_STDIO_CHAR_IO_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_DIRECT_IO
# ifdef CYGBLD_ISO_STDIO_DIRECT_IO_HEADER
#  include CYGBLD_ISO_STDIO_DIRECT_IO_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_FILEPOS
# ifdef CYGBLD_ISO_STDIO_FILEPOS_HEADER
#  include CYGBLD_ISO_STDIO_FILEPOS_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_ERROR
# ifdef CYGBLD_ISO_STDIO_ERROR_HEADER
#  include CYGBLD_ISO_STDIO_ERROR_HEADER
# endif
#endif

#ifdef CYGINT_ISO_STDIO_POSIX_FDFUNCS
# ifdef CYGBLD_ISO_STDIO_POSIX_FDFUNCS_HEADER
#  include CYGBLD_ISO_STDIO_POSIX_FDFUNCS_HEADER
# else

#  ifdef __cplusplus
extern "C" {
#  endif

extern int
fileno( FILE *__stream );

extern FILE *
fdopen( int __fildes, const char *__type );

#  ifdef __cplusplus
} // extern "C"
#  endif
# endif
#endif

#endif /* CYGONCE_ISO_STDIO_H multiple inclusion protection */

/* EOF stdio.h */
