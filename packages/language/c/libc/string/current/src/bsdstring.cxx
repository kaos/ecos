//===========================================================================
//
//      bsdstring.cxx
//
//      BSD string routines
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.  
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-11-27
// Purpose:       Provide string functions derived from BSD
// Description: 
// Usage:         #include <string.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc_string.h>   // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>                // size_t, NULL etc.
#include <ctype.h>                 // toupper/tolower

// FUNCTIONS

/*---------------------------------------------------------------------*/
/* strcasecmp */

__externC int
__strcasecmp( const char *s1, const char *s2 )
{
    CYG_REPORT_FUNCNAMETYPE( "strcasecmp", "returning %d" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (*s1 != '\0' && tolower(*s1) == tolower(*s2))
    {
        s1++;
        s2++;
    }

    int ret = tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

__externC int
strcasecmp( const char *s1, const char *s2 ) \
  CYGBLD_ATTRIB_WEAK_ALIAS(__strcasecmp);


/*---------------------------------------------------------------------*/
/* strncasecmp */

__externC int
__strncasecmp( const char *s1, const char *s2, size_t n )
{
    CYG_REPORT_FUNCNAMETYPE( "strncasecmp", "returning %d" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, n=%d", s1, s2, n );

    if (n == 0)
    {
        CYG_REPORT_RETVAL(0);
        return 0;
    }

    CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    while (n-- != 0 && tolower(*s1) == tolower(*s2))
    {
        if (n == 0 || *s1 == '\0' || *s2 == '\0')
            break;
        s1++;
        s2++;
    }

    int ret = tolower(*(unsigned char *) s1) - tolower(*(unsigned char *) s2);
    CYG_REPORT_RETVAL( ret );
    return ret;
}

__externC int
strncasecmp( const char *s1, const char *s2, size_t n ) \
  CYGBLD_ATTRIB_WEAK_ALIAS(__strncasecmp);

/*---------------------------------------------------------------------*/
/* bcmp */

__externC int
__bcmp( const void *s1, const void *s2, size_t n )
{
    // Don't bother tracing - memcmp can do that
    return memcmp (s1, s2, n);
}

__externC int
bcmp( const void *s1, const void *s2, size_t n ) \
  CYGBLD_ATTRIB_WEAK_ALIAS(__bcmp);

/*---------------------------------------------------------------------*/
/* bcopy */

__externC void
__bcopy( const void *src, void *dest, size_t n )
{
    // Don't bother tracing - memmove can do that
    memmove (dest, src, n);
}

__externC void
bcopy( const void *src, void *dest, size_t n ) \
  CYGBLD_ATTRIB_WEAK_ALIAS(__bcopy);

/*---------------------------------------------------------------------*/
/* bzero */

__externC void
__bzero( void *s, size_t n )
{
    // Don't bother tracing - memset can do that
    memset( s, 0, n );
}

__externC void
bzero( void *s, size_t n ) CYGBLD_ATTRIB_WEAK_ALIAS(__bzero);

/*---------------------------------------------------------------------*/
/* index */

__externC char *
__index( const char *s, int c )
{
    // Don't bother tracing - strchr can do that
    return strchr(s, c);
}

__externC char *
index( const char *s, int c ) CYGBLD_ATTRIB_WEAK_ALIAS(__index);

/*---------------------------------------------------------------------*/
/* rindex */

__externC char *
__rindex( const char *s, int c )
{
    // Don't bother tracing - strrchr can do that
    return strrchr(s, c);
}

__externC char *
rindex( const char *s, int c ) CYGBLD_ATTRIB_WEAK_ALIAS(__rindex);

/*---------------------------------------------------------------------*/
/* swab */

__externC void
__swab( const void *from, void *to, size_t n )
{
    const char *f = (const char *)from;
    char *t = (char *)to;
    size_t ptr;

    CYG_REPORT_FUNCNAME( "swab" );
    CYG_REPORT_FUNCARG3( "from=%08x, to=%08x, n=%d", from, to, n );

    if (n) {
        CYG_CHECK_DATA_PTR( from, "from is not a valid pointer!" );
        CYG_CHECK_DATA_PTR( to, "to is not a valid pointer!" );
    }

    for (ptr = 1; ptr < n; ptr += 2)
    {
        char p = f[ptr];
        char q = f[ptr-1];
        t[ptr-1] = p;
        t[ptr  ] = q;
    }
    if (ptr == n)          /* I.e., if n is odd, */
        t[ptr-1] = '\0';   /* then pad with a NUL. */

    CYG_REPORT_RETURN();
}

__externC void
swab( const void *from, void *to, size_t n ) \
  CYGBLD_ATTRIB_WEAK_ALIAS(__swab);

/*---------------------------------------------------------------------*/
// EOF bsdstring.cxx
