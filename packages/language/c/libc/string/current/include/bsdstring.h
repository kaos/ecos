#ifndef CYGONCE_LIBC_BSDSTRING_H
#define CYGONCE_LIBC_BSDSTRING_H
/*===========================================================================
//
//      bsdstring.h
//
//      BSD standard string and memory area manipulation routines
//
//===========================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-11-27
// Purpose:       This file provides various string functions normally
//                provided in the BSD UNIX operating system.
// Description:   
// Usage:         Do not include this file directly - use #include <string.h>
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

/* CONFIGURATION */

#include <pkgconf/libc_string.h>   /* Configuration header */

#ifdef CYGFUN_LIBC_STRING_BSD_FUNCS

#define __need_size_t
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*=========================================================================*/

/* FUNCTION PROTOTYPES */


extern int
strcasecmp( const char * /* s1 */, const char * /* s2 */ );

extern int
strncasecmp( const char * /* s1 */, const char * /* s2 */, size_t /* n */ );

extern int
bcmp( const void * /* s1 */, const void * /* s2 */, size_t /* n */ );

extern void
bcopy( const void * /* src */, void * /* dest */, size_t /* n */ );

extern void
bzero( void * /* s */, size_t /* n */ );

extern char *
index( const char * /* s */, int /* c */ );

extern char *
rindex( const char * /* s */, int /* c */ );

extern void
swab( const void * /* from */, void * /* to */, size_t /* n */ );

/*=========================================================================*/

#ifdef __cplusplus
}   /* extern "C" */
#endif

#endif /* ifdef CYGFUN_LIBC_STRING_BSD_FUNCS */

#endif /* CYGONCE_LIBC_BSDSTRING_H multiple inclusion protection */

/* EOF bsdstring.h */
