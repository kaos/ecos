#ifndef CYGONCE_LIBC_CLIBINCL_STDLIBSUPP_HXX
#define CYGONCE_LIBC_CLIBINCL_STDLIBSUPP_HXX
//========================================================================
//
//      stdlibsupp.hxx
//
//      Support for <stdlib.h> ANSI standard utility functions
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       #include "clibincl/stdlibsupp.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>// Common project-wide type definitions
#include <stdlib.h>            // Header for the standard utility functions
                               // including definition of
                               // Cyg_comparison_fn_t and Cyg_atexit_fn_t

// FUNCTION PROTOTYPES

// Allow the atexit handlers to be invoked
externC void
cyg_libc_invoke_atexit_handlers( void );

// These are function prototypes for the aliased functions that actually
// implement the stdlib functions

//========================================================================

// 7.10.1 String conversion functions

externC double
_atof( const char * /* double_str */ );

externC int
_atoi( const char * /* int_str */ );

externC long
_atol( const char * /* long_str */ );

externC double
_strtod( const char * /* double_str */, char ** /* endptr */ );

externC long
_strtol( const char * /* long_str */, char ** /* endptr */,
         int /* base */ );

externC unsigned long
_strtoul( const char * /* ulong_str */, char ** /* endptr */,
          int /* base */ );

//========================================================================

// 7.10.2 Pseudo-random sequence generation functions

externC int
_rand( void );

externC void
_srand( unsigned int /* seed */ );

// POSIX 1003.1 section 8.3.8 rand_r()
externC int
_rand_r( unsigned int * /* seed */ );

//========================================================================

// 7.10.3 Memory management functions

#ifdef CYGPKG_LIBC_MALLOC

externC void *
_calloc( size_t /* num_objects */, size_t /* object_size */ );

externC void
_free( void * /* ptr */ );

externC void *
_malloc( size_t /* size */ );

externC void *
_realloc( void * /* ptr */, size_t /* size */ );

#endif // ifdef CYGPKG_LIBC_MALLOC

//========================================================================

// 7.10.4 Communication with the environment

externC void
_abort( void ) CYGBLD_ATTRIB_NORET;

externC int
_atexit( Cyg_libc_atexit_fn_t /* func_to_register */ );

// The "real" names for exit() and _exit() are different for obvious name
// clash issues

externC void
__libc_exit( int /* status */ ) CYGBLD_ATTRIB_NORET;

// POSIX 1003.1 section 3.2.2 "Terminate a process"

externC void
__libc__exit( int /* status */ ) CYGBLD_ATTRIB_NORET;

externC char *
_getenv( const char * /* name */ );

//========================================================================

// 7.10.5 Searching and sorting utilities

externC void *
_bsearch( const void * /* search_key */, const void * /* first_object */,
          size_t /* num_objects */, size_t /* object_size */,
          Cyg_comparison_fn_t /* comparison_fn */ );

externC void
_qsort( void * /* first_object */, size_t /* num_objects */,
        size_t /* object_size */,
        Cyg_comparison_fn_t /* comparison_fn */ );


//========================================================================

// 7.10.6 Integer arithmetic functions

externC int
_abs( int /* val */ );

externC long
_labs( long /* val */ );

//========================================================================


#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_CLIBINCL_STDLIBSUPP_HXX multiple inclusion
       // protection

// EOF stdlibsupp.hxx
