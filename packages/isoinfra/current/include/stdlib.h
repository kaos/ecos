#ifndef CYGONCE_ISO_STDLIB_H
#define CYGONCE_ISO_STDLIB_H
/*========================================================================
//
//      stdlib.h
//
//      ISO standard library functions
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.          
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
// Purpose:       This file provides the stdlib functions required by 
//                ISO C and POSIX 1003.1.
// Description:   The real contents of this file get set from the
//                configuration (set by the implementation)
// Usage:         #include <stdlib.h>
//
//####DESCRIPTIONEND####
//
//======================================================================
*/

/* CONFIGURATION */

#include <pkgconf/isoinfra.h>          /* Configuration header */

/* INCLUDES */

/* This is the "standard" way to get NULL, wchar_t and size_t from stddef.h,
 * which is the canonical location of the definitions.
 */
#define __need_NULL
#define __need_size_t
#define __need_wchar_t
#include <stddef.h>

/*==========================================================================*/

#if CYGINT_ISO_STDLIB_STRCONV
# ifdef CYGBLD_ISO_STDLIB_STRCONV_HEADER
#  include CYGBLD_ISO_STDLIB_STRCONV_HEADER
# else

/* ISO C 7.10.1 - String conversion functions */

#ifdef __cplusplus
extern "C" {
#endif

extern int
atoi( const char * /* int_str */ );

extern long
atol( const char * /* long_str */ );

extern long
strtol( const char * /* long_str */, char ** /* endptr */,
        int /* base */ );

extern unsigned long
strtoul( const char * /* ulong_str */, char ** /* endptr */,
         int /* base */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 


# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_STDLIB_STRCONV_FLOAT
# ifdef CYGBLD_ISO_STDLIB_STRCONV_FLOAT_HEADER
#  include CYGBLD_ISO_STDLIB_STRCONV_FLOAT_HEADER
# else

/* ISO C 7.10.1 - String conversion functions */

#ifdef __cplusplus
extern "C" {
#endif

extern double
atof( const char * /* double_str */ );

extern double
strtod( const char * /* double_str */, char ** /* endptr */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 


# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_RAND
# ifdef CYGBLD_ISO_RAND_HEADER
#  include CYGBLD_ISO_RAND_HEADER
# else

/* ISO C 7.10.2 - Pseudo-random sequence generation functions */

/* Maximum value returned by rand().  */
#define RAND_MAX  2147483647

#ifdef __cplusplus
extern "C" {
#endif

extern int
rand( void );

extern void
srand( unsigned int /* seed */ );

/* POSIX 1003.1 section 8.3.8 rand_r() */
extern int
rand_r( unsigned int * /* seed */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_MALLOC
# ifdef CYGBLD_ISO_MALLOC_HEADER
#  include CYGBLD_ISO_MALLOC_HEADER
# else

/* ISO C 7.10.3 - Memory management functions */

#ifdef __cplusplus
extern "C" {
#endif

extern void *
calloc( size_t /* num_objects */, size_t /* object_size */ );

extern void
free( void * /* ptr */ );

extern void *
malloc( size_t /* size */ );

extern void *
realloc( void * /* ptr */, size_t /* size */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_MALLINFO
# ifdef CYGBLD_ISO_MALLINFO_HEADER
#  include CYGBLD_ISO_MALLINFO_HEADER
# else

#ifdef __cplusplus
extern "C" {
#endif

/* SVID2/XPG mallinfo structure */

struct mallinfo {
    int arena;    /* total size of memory arena */
    int ordblks;  /* number of ordinary memory blocks */
    int smblks;   /* number of small memory blocks */
    int hblks;    /* number of mmapped regions */
    int hblkhd;   /* total space in mmapped regions */
    int usmblks;  /* space used by small memory blocks */
    int fsmblks;  /* space available for small memory blocks */
    int uordblks; /* space used by ordinary memory blocks */
    int fordblks; /* space free for ordinary blocks */
    int keepcost; /* top-most, releasable (via malloc_trim) space */
    int maxfree;  /* (NON-STANDARD EXTENSION) size of largest free block */
};

extern struct mallinfo
mallinfo( void );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_EXIT
# ifdef CYGBLD_ISO_EXIT_HEADER
#  include CYGBLD_ISO_EXIT_HEADER
# else

/* ISO C 7.10.4 - Communication with the environment */

/* codes to pass to exit() */

/* Successful exit status - must be zero (POSIX 1003.1 8.1) */
#define EXIT_SUCCESS  0
/* Failing exit status - must be non-zero (POSIX 1003.1 8.1) */
#define EXIT_FAILURE  1

#ifdef __cplusplus
extern "C" {
#endif

/* Type of function used by atexit() */
typedef void (*__atexit_fn_t)( void );

extern void
abort( void ) __attribute__((__noreturn__));

extern int
atexit( __atexit_fn_t /* func_to_register */ );

extern void
exit( int /* status */ ) __attribute__((__noreturn__));

/* POSIX 1003.1 section 3.2.2 "Terminate a process" */

//@@@ FIXME unistd.h
extern void
_exit( int /* status */ ) __attribute__((__noreturn__));

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_STDLIB_ENVIRON
# ifdef CYGBLD_ISO_STDLIB_ENVIRON_HEADER
#  include CYGBLD_ISO_STDLIB_ENVIRON_HEADER
# else

/* ISO C 7.10.4 - Communication with the environment */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _POSIX_SOURCE

extern char **environ;   /* standard definition of environ */

#endif
    
extern char *
getenv( const char * /* name */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_STDLIB_SYSTEM
# ifdef CYGBLD_ISO_STDLIB_SYSTEM_HEADER
#  include CYGBLD_ISO_STDLIB_SYSTEM_HEADER
# else

/* ISO C 7.10.4 - Communication with the environment */

#ifdef __cplusplus
extern "C" {
#endif

extern int
system( const char * /* command */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_BSEARCH
# ifdef CYGBLD_ISO_BSEARCH_HEADER
#  include CYGBLD_ISO_BSEARCH_HEADER
# else

/* ISO C 7.10.5 - Searching and sorting utilities */

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*__bsearch_comparison_fn_t)(const void * /* object1 */,
                                         const void * /* object2 */);

extern void *
bsearch( const void * /* search_key */, const void * /* first_object */,
         size_t /* num_objects */, size_t /* object_size */,
         __bsearch_comparison_fn_t /* comparison_fn */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#if CYGINT_ISO_QSORT
# ifdef CYGBLD_ISO_QSORT_HEADER
#  include CYGBLD_ISO_QSORT_HEADER
# else

/* ISO C 7.10.5 - Searching and sorting utilities */

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*__qsort_comparison_fn_t)(const void * /* object1 */,
                                       const void * /* object2 */);

extern void
qsort( void * /* first_object */, size_t /* num_objects */,
       size_t /* object_size */, __qsort_comparison_fn_t /* comparison_fn */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*======================================================================*/

#if CYGINT_ISO_ABS
# ifdef CYGBLD_ISO_STDLIB_ABS_HEADER
#  include CYGBLD_ISO_STDLIB_ABS_HEADER
# else

/* TYPE DEFINITIONS */

/* ISO C 7.10 and 7.10.6 - Integer Arithmetic Functions */

#ifdef __cplusplus
extern "C" {
#endif

extern int
abs( int /* val */ ) __attribute__((__const__));

extern long
labs( long /* val */ ) __attribute__((__const__));

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*======================================================================*/

#if CYGINT_ISO_DIV
# ifdef CYGBLD_ISO_STDLIB_DIV_HEADER
#  include CYGBLD_ISO_STDLIB_DIV_HEADER
# else

/* ISO C 7.10 and 7.10.6 - Integer Arithmetic Functions */

/* return type of the div() function */

typedef struct {
    int quot;      /* quotient  */
    int rem;       /* remainder */
} div_t;


/* return type of the ldiv() function */

typedef struct {
    long quot;     /* quotient  */
    long rem;      /* remainder */
} ldiv_t;

#ifdef __cplusplus
extern "C" {
#endif

extern div_t
div( int /* numerator */, int /* denominator */ ) __attribute__((__const__));

extern ldiv_t
ldiv( long /* numerator */, long /* denominator */ ) __attribute__((__const__));

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

/* Maximum number of bytes in a multibyte character for the current locale */

#ifdef CYGBLD_ISO_STDLIB_MB_CUR_MAX_HEADER
# include CYGBLD_ISO_STDLIB_MB_CUR_MAX_HEADER
#else
# define MB_CUR_MAX 1
#endif

#if CYGINT_ISO_STDLIB_MULTIBYTE
# ifdef CYGBLD_ISO_STDLIB_MULTIBYTE_HEADER
#  include CYGBLD_ISO_STDLIB_MULTIBYTE_HEADER
# else

/* ISO C 7.10.7 - Multibyte character functions */


#ifdef __cplusplus
extern "C" {
#endif

extern int
mblen( const char * /* s */, size_t /* n */ );

extern int
mbtowc( wchar_t * /* pwc */, const char * /* s */, size_t /* n */ );

extern int
wctomb( char * /* s */, wchar_t /* wchar */ );

extern size_t
mbstowcs( wchar_t * /* pwcs */, const char * /* s */, size_t /* n */ );

extern size_t
wcstombs( char * /* s */, const wchar_t * /* pwcs */, size_t /* n */ );

#ifdef __cplusplus
} /* extern "C" */
#endif 

# endif
#endif

/*==========================================================================*/

#endif /* CYGONCE_ISO_STDLIB_H multiple inclusion protection */

/* EOF stdlib.h */
