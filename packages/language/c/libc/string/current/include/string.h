#ifndef CYGONCE_LIBC_STRING_H
#define CYGONCE_LIBC_STRING_H
/*===========================================================================
//
//      string.h
//
//      ANSI standard string and memory area manipulation routines
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-14
// Purpose:       This file provides various string functions required by
//                ISO C 9899:1990
// Description:   The real contents of this file get filled in from the
//                configuration set by the implementation
// Usage:         Do not include this file directly - use #include <string.h>
//
//####DESCRIPTIONEND####
//
//=========================================================================*/

/* CONFIGURATION */

#include <pkgconf/libc_string.h>   /* Configuration header */

#ifdef __cplusplus
extern "C" {
#endif

/* FUNCTION PROTOTYPES */

/*=========================================================================*/

/* 7.11.2 Copying functions */

extern void *
memmove( void *, const void *, size_t );

extern char *
strcpy( char *, const char * );

extern char *
strncpy( char *, const char *, size_t );

/*=========================================================================*/

/* 7.11.3 Concatenation functions */

extern char *
strcat( char *, const char * );

extern char *
strncat( char *, const char *, size_t );


/*=========================================================================*/

/* 7.11.4 Comparison functions */

extern int
memcmp( const void *, const void *, size_t );

extern int
strcmp( const char *, const char * );

extern int
strcoll( const char *, const char * );

extern int
strncmp( const char *, const char *, size_t );

extern size_t
strxfrm( char *, const char *, size_t );


/*=========================================================================*/

/* 7.11.5 Search functions */

extern void *
memchr( const void *, int,  size_t );

extern char *
strchr( const char *, int );

extern size_t
strcspn( const char *, const char * );

extern char *
strpbrk( const char *, const char * );

extern char *
strrchr( const char *, int );

extern size_t
strspn( const char *, const char * );

extern char *
strstr( const char *, const char * );

extern char *
strtok( char *, const char * );

/* POSIX 1003.1 section 8.3.3 strtok_r() */

extern char *
strtok_r( char *, const char *, char ** );


/*=========================================================================*/

/* 7.11.6 Miscellaneous functions */

extern size_t
strlen( const char * );

#ifdef __cplusplus
}   /* extern "C" */
#endif

/* INLINE FUNCTIONS */

#ifdef CYGIMP_LIBC_STRING_INLINES
#include <cyg/libc/string/string.inl>
#endif

#endif /* CYGONCE_LIBC_STRING_H multiple inclusion protection */

/* EOF string.h */
