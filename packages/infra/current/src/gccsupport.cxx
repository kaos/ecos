/*===========================================================================
//
//      gccsupport.cxx
//
//      Miscellaneous generic support functions required by GCC
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2004, 2005, 2008 Free Software Foundation, Inc.            
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    jlarmour
// Contributors: 
// Date:         2005-03-06
// Purpose:      This file provides miscellaneous support functions that have
//               been assumed to be present by GCC.
// Description:  These functions and definitions are usually defaults
//               for when the "real" implementation does not exist.
//               These are intentionally very basic implementations. Users
//               should pull in the real implementations from other packages
//               rather than these. These are purely here for GCC's
//               requirements and nothing more.
//
//####DESCRIPTIONEND####
//
//==========================================================================*/


/* INCLUDES */

#include <pkgconf/infra.h>      /* Configuration of infra package */
#include <pkgconf/isoinfra.h>   /* Configuration of isoinfra package */

#include <cyg/infra/cyg_type.h> /* Common type definitions */
    //#include <cyg/infra/cyg_trac.h> /* Tracing support */
    //#include <cyg/infra/cyg_ass.h>  /* Assertion support */
#include <stddef.h>             /* Compiler defns such as size_t, NULL etc. */
#include <stdio.h>
#include <cyg/infra/diag.h>     /* Diagnostic output */

/* GLOBALS */


#ifndef CYGINT_ISO_STDIO_FILETYPES
typedef long FILE;
#endif
#ifndef CYGINT_ISO_STDIO_STREAMS
FILE *stdout, *stderr; /* In practice, ignored */
#endif


/* FUNCTIONS */
#ifndef CYGINT_ISO_STDIO_CHAR_IO
__externC int
fputs( const char *string, FILE * /* ignored */ ) __THROW
{
    diag_write_string( string );
    return 0;
}

__externC int
fputc( int c, FILE * /* ignored */ ) __THROW
{
    diag_write_char( (char)c );
    return c;
}

#endif

#ifndef CYGINT_ISO_STDIO_DIRECT_IO
/* Recent GCC can "optimise" fputs(string, stream)
 * to fwrite(string, 1, len, stream).
 * No idea why they think that's a good idea though! */
__externC size_t
fwrite( const void *ptr, size_t object_size, size_t num_objects,
        FILE * /* ignored */ ) __THROW
{
    const char *str = static_cast<const char *>(ptr);

    /* double check GCC use before assuming. Hmm, is it possible
     * GCC could optimise away the terminating NULL?
     */
    if (object_size == 1 && str[num_objects] == '\0')
        diag_write_string( str );
    else
        return 0;
    return num_objects;
}
#endif

#ifndef CYGINT_ISO_STRING_MEMFUNCS
__externC int
memcmp( const void *s1, const void *s2, size_t n )
{
    const unsigned char *m1 = (const unsigned char *) s1;
    const unsigned char *m2 = (const unsigned char *) s2;

    while (n--)
    {
        if (*m1 != *m2)
        {
            return *m1 - *m2;
        }
        m1++;
        m2++;
    }
    return 0;
} // memcmp()
#endif

#ifndef CYGINT_ISO_STRING_STRFUNCS
__externC int
strncmp( const char *s1, const char *s2, size_t n )
{
    if (n == 0)
    {
        return 0;
    }
    while (n-- != 0 && *s1 == *s2)
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    return (*(unsigned char *) s1) - (*(unsigned char *) s2);
} // strncmp()

__externC int
strcmp( const char *s1, const char *s2 )
{
    // Could do simple and direct implementation, but smaller is better
    return strncmp( s1, s2, (size_t)-1 );
} // strcmp()

__externC char *
strcat( char *s1, const char *s2 )
{
    char *s = s1;

    while (*s1)
        s1++;

    while ((*s1++ = *s2++))
        ;
    return s;
} // strcat()

__externC char *
strcpy( char *s1, const char *s2 )
{
    char *s = s1;

    while ((*s1++ = *s2++) != '\0')
        ;

    return s;
} // strcpy()
#endif


/* EOF gccsupport.cxx */
