#ifndef CYGONCE_LIBC_STRING_H
#define CYGONCE_LIBC_STRING_H
//===========================================================================
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
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include <string.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h> // Standard definitions including externC
#include <stddef.h>             // Get size_t and NULL definitions from
                                // compiler - for ANSI 7.11.1
#include <cyg/error/codes.h>    // for strerror()

// FUNCTION PROTOTYPES

//===========================================================================

// 7.11.2 Copying functions

// memcpy is no longer actually implemented in the C library.
// It is now in the HAL

externC void *
memcpy( void *, const void *, size_t );

externC void *
memmove( void *, const void *, size_t );

externC char *
strcpy( char *, const char * );

externC char *
strncpy( char *, const char *, size_t );

//===========================================================================

// 7.11.3 Concatenation functions

externC char *
strcat( char *, const char * );

externC char *
strncat( char *, const char *, size_t );


//===========================================================================

// 7.11.4 Comparison functions

externC int
memcmp( const void *, const void *, size_t );

externC int
strcmp( const char *, const char * );

externC int
strcoll( const char *, const char * );

externC int
strncmp( const char *, const char *, size_t );

externC size_t
strxfrm( char *, const char *, size_t );


//===========================================================================

// 7.11.5 Search functions

externC void *
memchr( const void *, int,  size_t );

externC char *
strchr( const char *, int );

externC size_t
strcspn( const char *, const char * );

externC char *
strpbrk( const char *, const char * );

externC char *
strrchr( const char *, int );

externC size_t
strspn( const char *, const char * );

externC char *
strstr( const char *, const char * );

externC char *
strtok( char *, const char * );

// POSIX 1003.1 section 8.3.3 strtok_r()

externC char *
strtok_r( char *, const char *, char ** );


//===========================================================================

// 7.11.6 Miscellaneous functions

// memset is no longer actually implemented in the C library.
// It is now in the HAL

externC void *
memset( void *, int, size_t );

// strerror() is provided in <cyg/error/codes.h>

externC size_t
strlen( const char * );


// INLINE FUNCTIONS

#ifdef CYGIMP_LIBC_STRING_INLINES
#include <string.inl>
#endif

#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_STRING_H multiple inclusion protection

// EOF string.h
