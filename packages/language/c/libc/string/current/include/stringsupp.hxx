#ifndef CYGONCE_LIBC_STRING_STRINGSUPP_HXX
#define CYGONCE_LIBC_STRING_STRINGSUPP_HXX
//===========================================================================
//
//      stringsupp.hxx
//
//      Support for the ANSI standard string functions
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
// Contributors:  jlarmour
// Date:          2000-04-14
// Purpose:       Internal support for the libc string function implementations
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_string.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h> // Common type definitions
#include <stddef.h>             // Compiler definitions such as
                                // size_t, NULL etc.

// CONSTANTS

#ifndef CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST

// Masks for CYG_LIBC_STR_DETECTNULL below
externC const cyg_uint64 Cyg_libc_str_null_mask_1;
externC const cyg_uint64 Cyg_libc_str_null_mask_2;

#endif

// MACROS

// Nonzero if X is not aligned on a word boundary.
#define CYG_LIBC_STR_UNALIGNED(X) ((CYG_WORD)(X) & (sizeof (CYG_WORD) - 1))

// Nonzero if either X or Y is not aligned on a word boundary.
#define CYG_LIBC_STR_UNALIGNED2(X , Y) \
    (((CYG_WORD)(X) & (sizeof (CYG_WORD) - 1)) | \
     ((CYG_WORD)(Y) & (sizeof (CYG_WORD) - 1)))

// Nonzero if any byte of X contains a NULL.
#define CYG_LIBC_STR_DETECTNULL(X) \
    (((X) - (CYG_WORD)Cyg_libc_str_null_mask_1) & \
     ~(X) & (CYG_WORD)Cyg_libc_str_null_mask_2)

// How many bytes are copied each iteration of the 4X unrolled loop in the
// optimised string implementations
#define CYG_LIBC_STR_OPT_BIGBLOCKSIZE     (sizeof(CYG_WORD) << 2)

// How many bytes are copied each iteration of the word copy loop in the
// optimised string implementations
#define CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE  (sizeof (CYG_WORD))

// Threshold for punting to the byte copier in the optimised string
// implementations
#define CYG_LIBC_STR_OPT_TOO_SMALL(LEN) \
    ((LEN) < CYG_LIBC_STR_OPT_LITTLEBLOCKSIZE)


// FUNCTION PROTOTYPES

// These are function prototypes for the aliased functions that actually
// implement the string functions

//===========================================================================

// 7.11.2 Copying functions

externC void *
__memmove( void *, const void *, size_t );


externC char *
__strcpy( char *, const char * );


externC char *
__strncpy( char *, const char *, size_t );


//===========================================================================

// 7.11.3 Concatenation functions


externC char *
__strcat( char *, const char * );


externC char *
__strncat( char *, const char *, size_t );


//===========================================================================

// 7.11.4 Comparison functions

externC int
__memcmp( const void *, const void *, size_t );


externC int
__strcmp( const char *, const char * );


externC int
__strcoll( const char *, const char * );


externC int
__strncmp( const char *, const char *, size_t );


externC size_t
__strxfrm( char *, const char *, size_t );


//===========================================================================

// 7.11.5 Search functions


externC void *
__memchr( const void *, int , size_t );


externC char *
__strchr( const char *, int );


externC size_t
__strcspn( const char *, const char * );


externC char *
__strpbrk( const char *, const char * );


externC char *
__strrchr( const char *, int );


externC size_t
__strspn( const char *, const char * );


externC char *
__strstr( const char *, const char * );


externC char *
__strtok( char *, const char * );

// For POSIX 1003.1 section 8.3.3 strtok_r()

externC char *
__strtok_r( char *, const char *, char ** );


//===========================================================================

// 7.11.6 Miscellaneous functions

externC size_t
__strlen( const char * );

#endif // CYGONCE_LIBC_STRING_STRINGSUPP_HXX multiple inclusion protection

// EOF stringsupp.hxx
