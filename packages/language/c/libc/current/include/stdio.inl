#ifndef CYGONCE_LIBC_STDIO_INL
#define CYGONCE_LIBC_STDIO_INL
//===========================================================================
//
//      stdio.inl
//
//      ANSI standard I/O routines - inlined functions
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
// Usage:       #include <stdio.h> - do not include this file directly
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want the stdio stuff?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <stddef.h>                // NULL and size_t from compiler
#include <stdarg.h>                // va_list
#include <stdio.h>                 // Just be sure it has been included
#include <errno.h>                 // Definition of error codes and errno
#include <string.h>                // Definition of strerror() for perror()
#include <limits.h>                // INT_MAX

// INLINE FUNCTION DEFINITIONS

//===========================================================================

// 7.9.5 File access functions

CYGPRI_LIBC_INLINE void
setbuf( FILE *stream, char *buf )
{
    if (buf == NULL)
        setvbuf( stream, NULL, _IONBF, BUFSIZ );
    else
        // NB: Should use full buffering by default ordinarily, but in
        // the current system we're always connected to an interactive
        // terminal, so use line buffering
        setvbuf( stream, buf, _IOLBF, BUFSIZ );

} // setbuf()

//===========================================================================

// 7.9.6 Formatted input/output functions

CYGPRI_LIBC_INLINE int
vfprintf( FILE *stream, const char *format, va_list arg )
{
    return vfnprintf(stream, INT_MAX, format, arg);
} // vfprintf()


CYGPRI_LIBC_INLINE int
vprintf( const char *format, va_list arg )
{
    return vfnprintf(stdout, INT_MAX, format, arg);
} // vprintf()


CYGPRI_LIBC_INLINE int
vsprintf( char *s, const char *format, va_list arg )
{
    return vsnprintf(s, INT_MAX, format, arg);
} // vsprintf()


//===========================================================================

// 7.9.7 Character input/output functions

CYGPRI_LIBC_INLINE int
puts( const char *s )
{
    int rc;

    rc = fputs( s, stdout );

    if (rc >= 0)
        rc = fputc('\n', stdout );

    return rc;
} // puts()


//===========================================================================

// 7.9.10 Error-handling functions

CYGPRI_LIBC_INLINE void
perror( const char *s )
{
    if (s && *s)
        fprintf( stderr, "%s: %s\n", s, strerror(errno) );
    else
        fputs( strerror(errno), stderr );

} // perror()

//===========================================================================

// Other non-ANSI functions

CYGPRI_LIBC_INLINE int
vscanf( const char *format, va_list arg )
{
    return vfscanf( stdin, format, arg );
} // vscanf()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_STDIO)

#endif // CYGONCE_LIBC_STDIO_INL multiple inclusion protection

// EOF stdio.inl
