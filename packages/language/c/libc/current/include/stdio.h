#ifndef CYGONCE_LIBC_STDIO_H
#define CYGONCE_LIBC_STDIO_H
//========================================================================
//
//      stdio.h
//
//      ISO C standard I/O routines - with some POSIX 1003.1 extensions
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
// Date:          1999-01-19
// Purpose:       ISO C standard I/O routines
// Description: 
// Usage:         #include <stdio.h>
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h> // common type definitions and support
#include <stddef.h>             // NULL and size_t from compiler
#include <stdarg.h>             // va_list from compiler

// CONSTANTS

// Some of these values are odd to ensure that asserts have better effect
// should spurious values be passed to functions expecting these constants.

// _IOFBF, _IOLBF, and _IONBF specify full, line or no buffering when used
// with setvbuf() - ISO C standard chap 7.9.1

#define _IOFBF      (-2)
#define _IOLBF      (-4)
#define _IONBF      (-8)

// EOF is a macro defined to any negative integer constant - ISO C standard
// chap. 7.9.1
#define EOF         (-1)

// SEEK_CUR, SEEK_END and SEEK_SET are used with fseek() as position
// anchors - ISO C standard chap. 7.9.1
#define SEEK_SET    0
#define SEEK_CUR    1
#define SEEK_END    2


// TYPE DEFINITIONS

// A type capable of specifying uniquely every file position - ISO C
// standard chap 7.9.1
typedef cyg_ucount32 fpos_t;


// FILE is just cast to an address here. It is uncast internally to the
// C library in stream.hxx  as the C++ Cyg_StdioStream class.
// Optional run-time checking can be enabled to ensure that the cast is
// valid, using the standard assertion functionality.
//
// The array size is irrelevant other than being more than 8, and is present
// to stop references to FILEs being marked as able to be put in the small
// data section. We can't just mark it as in the ".data" section as on some
// targets it may actually be ".common". Maybe this doesn't matter?
// FIXME: This doesn't work if things are larger than 16 are put in sdata,
// e.g. on mips by specifying -G 32
typedef CYG_ADDRESS FILE[16];

// EXTERNAL VARIABLES

// Default file streams for input/output. These only need to be
// expressions, not l-values - ISO C standard chap. 7.9.1
//
// cyg_libc_stdio_stdin/out/err aren't really files of course,
// but they are only named here so that we can take the address of
// them, in which case it doesn't matter what type they point to really
//
// CYGPRI_LIBC_STDIO_NO_DEFAULT_STREAMS is used when initializing
// cyg_libc_stdio_stdin/out/err privately inside the C library

#ifndef CYGPRI_LIBC_STDIO_NO_DEFAULT_STREAMS
extern FILE cyg_libc_stdio_stdin, cyg_libc_stdio_stdout,
            cyg_libc_stdio_stderr;

#define stdin  (&cyg_libc_stdio_stdin)
#define stdout (&cyg_libc_stdio_stdout)
#define stderr (&cyg_libc_stdio_stderr)
#endif

// FUNCTION PROTOTYPES

//========================================================================

// ISO C 7.9.5 File access functions

externC int
fclose( FILE * /* stream */ );

externC int
fflush( FILE * /* stream */ );

externC FILE *
fopen( const char * /* filename */, const char * /* mode */ );

externC FILE *
freopen( const char * /* filename */, const char * /* mode */,
         FILE * /* stream */ );

externC void
setbuf( FILE * /* stream */, char * /* buffer */ );

externC int
setvbuf( FILE * /* stream */, char * /* buffer */, int /* mode */,
         size_t /* size */ );

//========================================================================

// ISO C 7.9.6 Formatted input/output functions

externC int
fprintf( FILE * /* stream */, const char * /* format */, ... );

externC int
fscanf( FILE * /* stream */, const char * /* format */, ... );

externC int
printf( const char * /* format */, ... );

externC int
scanf( const char * /* format */, ... );

externC int
sprintf( char * /* str */, const char * /* format */, ... );

externC int
sscanf( const char * /* str */, const char * /* format */, ... );

externC int
vfprintf( FILE * /* stream */, const char * /* format */,
          va_list /* args */ );

externC int
vprintf( const char * /* format */, va_list /* args */ );

externC int
vsprintf( char * /* str */, const char * /* format */,
          va_list /* args */ );

//========================================================================

// ISO C 7.9.7 Character input/output functions

externC int
fgetc( FILE * /* stream */ );

externC char *
fgets( char * /* str */, int /* length */, FILE * /* stream */ );

externC int
fputc( int /* c */, FILE * /* stream */ );

externC int
putc( int /* c */, FILE * /* stream */ );

externC int
putchar( int /* c */ );

externC int
fputs( const char * /* str */, FILE * /* stream */ );

externC char *
gets( char * );

externC int
getc( FILE * /* stream */ );

externC int
getchar( void );

externC int
puts( const char * /* str */ );

externC int
ungetc( int /* c */, FILE * /* stream */ );

// no function equivalent is required for getchar() or putchar(), so we can
// just #define them

#define getchar() fgetc( stdin )

#define putchar(__c) fputc(__c, stdout)

//========================================================================

// ISO C 7.9.8 Direct input/output functions

externC size_t
fread( void * /* ptr */, size_t /* object_size */,
       size_t /* num_objects */, FILE * /* stream */ );

externC size_t
fwrite( const void * /* ptr */, size_t /* object_size */,
        size_t /* num_objects */, FILE * /* stream */ );


//========================================================================

// ISO C 7.9.10 Error-handling functions

externC void
clearerr( FILE * /* stream */ );

externC int
feof( FILE * /* stream */ );

externC int
ferror( FILE * /* stream */ );

externC void
perror( const char * /* prefix_str */ );

//========================================================================

// Other non-ISO C functions

externC int
fnprintf( FILE * /* stream */, size_t /* length */,
          const char * /* format */, ... );

externC int
snprintf( char * /* str */, size_t /* length */, const char * /* format */,
          ... );

externC int
vfnprintf( FILE * /* stream */, size_t /* length */,
           const char * /* format */, va_list /* args */ );

externC int
vsnprintf( char * /* str */, size_t /* length */,
           const char * /* format */, va_list /* args */ );

externC int
vscanf( const char * /* format */, va_list /* args */ );

externC int
vsscanf( const char * /* str */, const char * /* format */,
         va_list /* args */ );

externC int
vfscanf( FILE * /* stream */, const char * /* format */,
         va_list /* args */ );


// INLINE FUNCTIONS

#ifdef CYGIMP_LIBC_STDIO_INLINES
#include <stdio.inl>
#endif

#endif // ifdef CYGPKG_LIBC     

#endif // CYGONCE_LIBC_STDIO_H multiple inclusion protection

// EOF stdio.h
