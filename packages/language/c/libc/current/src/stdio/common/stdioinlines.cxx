//===========================================================================
//
//      stdioinlines.cxx
//
//      ANSI standard I/O routines - real alternatives for inlined functions
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

// We don't want the inline versions of stdio functions defined here

#ifdef CYGIMP_LIBC_STDIO_INLINES
#undef CYGIMP_LIBC_STDIO_INLINES
#endif

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <stddef.h>                // NULL and size_t from compiler
#include <stdarg.h>                // va_list
#include <stdio.h>                 // Header for this file
#include <errno.h>                 // Definition of error codes and errno
#include <string.h>                // Definition of strerror() for perror()
#include <limits.h>                // INT_MAX
#include "clibincl/stdiosupp.hxx"  // Support for stdio


// EXPORTED SYMBOLS

externC void
setbuf( FILE *, char * ) CYGPRI_LIBC_WEAK_ALIAS("_setbuf");

externC int
vfprintf( FILE *, const char *, va_list ) CYGPRI_LIBC_WEAK_ALIAS("_vfprintf");

externC int
vprintf( const char *, va_list ) CYGPRI_LIBC_WEAK_ALIAS("_vprintf");

externC int
vsprintf( char *, const char *, va_list ) CYGPRI_LIBC_WEAK_ALIAS("_vsprintf");

externC int
puts( const char *s ) CYGPRI_LIBC_WEAK_ALIAS("_puts");

externC void
perror( const char * ) CYGPRI_LIBC_WEAK_ALIAS("_perror");

#ifdef CYGFUN_LIBC_STDIO_ungetc
externC int
vscanf( const char *, va_list ) CYGPRI_LIBC_WEAK_ALIAS("_vscanf");
#endif

// FUNCTIONS

//===========================================================================

// 7.9.5 File access functions

externC void
_setbuf( FILE *stream, char *buf )
{
    if (buf == NULL)
        _setvbuf( stream, NULL, _IONBF, BUFSIZ );
    else
        // NB: Should use full buffering by default ordinarily, but in
        // the current system we're always connected to an interactive
        // terminal, so use line buffering
        _setvbuf( stream, buf, _IOLBF, BUFSIZ ); 

} // _setbuf()

//===========================================================================

// 7.9.6 Formatted input/output functions


externC int
_vfprintf( FILE *stream, const char *format, va_list arg )
{
    return _vfnprintf(stream, INT_MAX, format, arg);
} // _vfprintf()


externC int
_vprintf( const char *format, va_list arg)
{
    return _vfnprintf( stdout, INT_MAX, format, arg );
} // _vprintf()


externC int
_vsprintf( char *s, const char *format, va_list arg )
{
    return _vsnprintf(s, INT_MAX, format, arg);
} // _vsprintf()


//===========================================================================

// 7.9.7 Character input/output functions

externC int
_puts( const char *s )
{
    int rc;

    rc = _fputs( s, stdout );

    if (rc >= 0)
        rc = fputc('\n', stdout );

    return rc;
} // _puts()


//===========================================================================

// 7.9.10 Error-handling functions

externC void
_perror( const char *s )
{
    if (s && *s)
        _fprintf( stderr, "%s: %s\n", s, strerror(errno) );
    else
        _fputs( strerror(errno), stderr );

} // _perror()

//===========================================================================

// Other non-ANSI functions

#ifdef CYGFUN_LIBC_STDIO_ungetc
externC int
_vscanf( const char *format, va_list arg )
{
    return _vfscanf( stdin, format, arg );
} // _vscanf()
#endif

#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// EOF stdioinlines.cxx
