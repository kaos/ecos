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
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:          1998-02-13
// Purpose:     
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
#include <sys/file_if.h>        // FILE definition

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
#define SEEK_CUR    (-16)
#define SEEK_END    (-32)
#define SEEK_SET    (-64)


// TYPE DEFINITIONS

// A type capable of specifying uniquely every file position - ISO C
// standard chap 7.9.1
typedef cyg_ucount32 fpos_t;


// EXTERNAL VARIABLES

// Default file streams for input/output. These only need to be
// expressions, not l-values - ISO C standard chap. 7.9.1

externC FILE *stdin, *stdout, *stderr;

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
fputs( const char * /* str */, FILE * /* stream */ );

// no function equivalent is required for these, so we can just #define
// them

#define getc( __stream ) fgetc( __stream )

#define getchar() fgetc( stdin )

externC char *
gets( char * );

#define putc(__c, __stream) fputc(__c, __stream)

#define putchar(__c) fputc(__c, stdout)

externC int
puts( const char * /* str */ );

externC int
ungetc( int /* c */, FILE * /* stream */ );

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
