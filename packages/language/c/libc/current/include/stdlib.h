#ifndef CYGONCE_LIBC_STDLIB_H
#define CYGONCE_LIBC_STDLIB_H
//========================================================================
//
//      stdlib.h
//
//      ISO C standard utility functions (ISO C standard section 7.10)
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-17
// Purpose:     
// Description: 
// Usage:         #include <stdlib.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h> // Common type definitions and support
#include <stddef.h>             // NULL, wchar_t and size_t from compiler
#include <limits.h>             // for INT_MAX definition used below

// CONSTANTS

// as described in ISO para 7.10

// codes to pass to exit()

// Successful exit status - must be zero (POSIX 1003.1 8.1)
#define EXIT_SUCCESS  0
// Failing exit status - must be non-zero (POSIX 1003.1 8.1)
#define EXIT_FAILURE  1

// Maximum value returned by rand()

#define RAND_MAX  INT_MAX


// Maximum number of bytes in a multibyte character for the current locale

#define MB_CUR_MAX  1


// TYPE DEFINITIONS

// as described in ANSI para 7.10

// return type of the div() function

typedef struct {
    int quot;      // quotient
    int rem;       // remainder
} div_t;


// return type of the ldiv() function

typedef struct {
    long quot;     // quotient
    long rem;      // remainder
} ldiv_t;

// Unofficial type of comparison function used by both bsearch() and
// qsort()

typedef int (*Cyg_comparison_fn_t)(const void * /* object1 */,
                                   const void * /* object2 */);

// Similarly, type of function used by atexit()
typedef void (*Cyg_libc_atexit_fn_t)( void );


// FUNCTION PROTOTYPES

//========================================================================

// 7.10.1 String conversion functions

externC double
atof( const char * /* double_str */ );

externC int
atoi( const char * /* int_str */ );

externC long
atol( const char * /* long_str */ );

externC double
strtod( const char * /* double_str */, char ** /* endptr */ );

externC long
strtol( const char * /* long_str */, char ** /* endptr */,
        int /* base */ );

externC unsigned long
strtoul( const char * /* ulong_str */, char ** /* endptr */,
         int /* base */ );

//========================================================================

// 7.10.2 Pseudo-random sequence generation functions

externC int
rand( void );

externC void
srand( unsigned int /* seed */ );

// POSIX 1003.1 section 8.3.8 rand_r()
externC int
rand_r( unsigned int * /* seed */ );

//========================================================================

// 7.10.3 Memory management functions

#ifdef CYGPKG_LIBC_MALLOC

externC void *
calloc( size_t /* num_objects */, size_t /* object_size */ );

externC void
free( void * /* ptr */ );

externC void *
malloc( size_t /* size */ );

externC void *
realloc( void * /* ptr */, size_t /* size */ );

#endif // ifdef CYGPKG_LIBC_MALLOC

//========================================================================

// 7.10.4 Communication with the environment

externC void
abort( void ) CYGBLD_ATTRIB_NORET;

externC int
atexit( Cyg_libc_atexit_fn_t /* func_to_register */ );

externC void
exit( int /* status */ ) CYGBLD_ATTRIB_NORET;

/* POSIX 1003.1 section 3.2.2 "Terminate a process" */

externC void
_exit( int /* status */ ) CYGBLD_ATTRIB_NORET;

externC char *
getenv( const char * /* name */ );

externC int
system( const char * /* command */ );


//========================================================================

// 7.10.5 Searching and sorting utilities

externC void *
bsearch( const void * /* search_key */, const void * /* first_object */,
         size_t /* num_objects */, size_t /* object_size */,
         Cyg_comparison_fn_t /* comparison_fn */ );

externC void
qsort( void * /* first_object */, size_t /* num_objects */,
       size_t /* object_size */, Cyg_comparison_fn_t /* comparison_fn */ );


//========================================================================

// 7.10.6 Integer arithmetic functions

externC int
abs( int /* val */ ) CYGBLD_ATTRIB_CONST;

externC div_t
div( int /* numerator */, int /* denominator */ ) CYGBLD_ATTRIB_CONST;

externC long
labs( long /* val */ ) CYGBLD_ATTRIB_CONST;

externC ldiv_t
ldiv( long /* numerator */, long /* denominator */ ) CYGBLD_ATTRIB_CONST;

//========================================================================

// NON-STANDARD FUNCTIONS

/////////////////////
// cyg_libc_itoa() //
/////////////////////
//
// This converts num to a string and puts it into s padding with
// "0"'s if padzero is set, or spaces otherwise if necessary.
// The number of chars written to s is returned
//

externC cyg_uint8
cyg_libc_itoa( cyg_uint8 *__s, cyg_int32 __num, cyg_uint8 __width,
               cyg_bool __padzero );

//========================================================================


// INLINE FUNCTIONS

#ifdef CYGIMP_LIBC_STDLIB_INLINES
#include <stdlib.inl>
#endif

#endif // CYGONCE_LIBC_STDLIB_H multiple inclusion protection

// EOF stdlib.h
