#ifndef CYGONCE_LIBC_CLIBINCL_STDIOSUPP_HXX
#define CYGONCE_LIBC_CLIBINCL_STDIOSUPP_HXX
//========================================================================
//
//      stdiosupp.hxx
//
//      Support for C library standard I/O routines
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
// Date:          1998-02-13
// Purpose:     
// Description: 
// Usage:         #include "clibincl/stdiosupp.hxx"
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // common type definitions and support
#include <stddef.h>                 // NULL and size_t from compiler
#include <stdarg.h>                 // va_list from compiler
#include <stdio.h>                  // Main standard I/O header
#include <cyg/io/devtab.h>          // Device table
#include "clibincl/stream.hxx"      // Cyg_StdioStream
#include <errno.h>                  // Cyg_ErrNo

// FUNCTION PROTOTYPES

//========================================================================

// Miscellaneous support functions

externC cyg_io_handle_t
Cyg_libc_stdio_find_filename( const char *filename );

externC Cyg_ErrNo
cyg_libc_stdio_flush_all_but( Cyg_StdioStream *not_this_stream );

//========================================================================

// ISO C 7.9.5 File access functions

externC int
_fclose( FILE * /* stream */ );

externC int
_fflush( FILE * /* stream */ );

externC FILE *
_fopen( const char * /* filename */, const char * /* mode */ );

externC FILE *
_freopen( const char * /* filename */, const char * /* mode */,
          FILE * /* stream */ );

externC void
_setbuf( FILE * /* stream */, char * /* buffer */ );

externC int
_setvbuf( FILE * /* stream */, char * /* buffer */, int /* mode */,
          size_t /* size */ );

//========================================================================

// ISO C 7.9.6 Formatted input/output functions

externC int
_fprintf( FILE * /* stream */, const char * /* format */, ... );

externC int
_fscanf( FILE * /* stream */, const char * /* format */, ... );

externC int
_printf( const char * /* format */, ... );

externC int
_scanf( const char * /* format */, ... );

externC int
_sprintf( char * /* str */, const char * /* format */, ... );

externC int
_sscanf( const char * /* str */, const char * /* format */, ... );

externC int
_vfprintf( FILE * /* stream */, const char * /* format */,
           va_list /* args */ );

externC int
_vprintf( const char * /* format */, va_list /* args */ );

externC int
_vsprintf( char * /* str */, const char * /* format */,
           va_list /* args */ );

//========================================================================

// ISO C 7.9.7 Character input/output functions

externC char *
_fgets( char * /* str */, int /* length */, FILE * /* stream */ );

externC int
_fputs( const char * /* str */, FILE * /* stream */ );

externC char *
_gets( char * );

externC int
_puts( const char * /* str */ );

externC int
_ungetc( int /* c */, FILE * /* stream */ );

//========================================================================

// ISO C 7.9.8 Direct input/output functions

externC size_t
_fread( void * /* ptr */, size_t /* object_size */,
        size_t /* num_objects */, FILE * /* stream */ );

externC size_t
_fwrite( const void * /* ptr */, size_t /* object_size */,
         size_t /* num_objects */, FILE * /* stream */ );


//========================================================================

// ISO C 7.9.10 Error-handling functions

externC void
_clearerr( FILE * /* stream */ );

externC int
_feof( FILE * /* stream */ );

externC int
_ferror( FILE * /* stream */ );

externC void
_perror( const char * /* prefix_str */ );

//========================================================================

// Other non-ISO C functions

externC int
_fnprintf( FILE * /* stream */, size_t /* length */,
           const char * /* format */, ... );

externC int
_snprintf( char * /* str */, size_t /* length */,
           const char * /* format */, ... );

externC int
_vfnprintf( FILE * /* stream */, size_t /* length */,
            const char * /* format */, va_list /* args */ );

externC int
_vsnprintf( char * /* str */, size_t /* length */,
            const char * /* format */, va_list /* args */ );

externC int
_vscanf( const char * /* format */, va_list /* args */ );

externC int
_vsscanf( const char * /* str */, const char * /* format */,
          va_list /* args */ );

externC int
_vfscanf( FILE * /* stream */, const char * /* format */,
          va_list /* args */ );


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

#endif // CYGONCE_LIBC_CLIBINCL_STDIOSUPP_HXX multiple inclusion protection

// EOF stdiosupp.hxx
