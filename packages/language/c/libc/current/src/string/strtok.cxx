//===========================================================================
//
//      strtok.cxx
//
//      ANSI standard strtok() routine 
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour@cygnus.co.uk
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================
//
// This code is based on original code with the following copyright:
//
/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *      This product includes software developed by the University of
 *      California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */


// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library? And do we want strtok() at all?
#if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtok)

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <string.h>                // Header for this file
#include <stddef.h>         // Compiler definitions such as size_t, NULL etc.
#include "clibincl/stringsupp.hxx" // Useful string function support and
                                   // prototypes
#include "clibincl/clibdata.hxx"   // C library internal data

// EXPORTED SYMBOLS

externC char *
strtok( char *s1, const char *s2 ) CYGPRI_LIBC_WEAK_ALIAS("_strtok");

externC char *
strtok_r( char *s1, const char *s2, char **lasts ) \
    CYGPRI_LIBC_WEAK_ALIAS("_strtok_r");

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_STRTOK_TRACE_LEVEL)
static int strtok_trace = CYGNUM_LIBC_STRTOK_TRACE_LEVEL;
# define TL1 (0 < strtok_trace)
#else
# define TL1 (0)
#endif


// FUNCTIONS

char *
_strtok( char *s1, const char *s2 )
{
    char **lasts;
    char *retval;

    CYG_REPORT_FUNCNAMETYPE( "_strtok", "returning %08x" );
    CYG_REPORT_FUNCARG2( "s1=%08x, s2=%08x", s1, s2 );
    
    if (s1 != NULL)
        CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );

    CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE;

    lasts = CYGPRI_LIBC_INTERNAL_DATA.get_strtok_last_p();

    CYG_TRACE2( TL1, "Retrieved strtok_last address %08x containing %s",
                lasts, *lasts );

    retval = _strtok_r( s1, s2, lasts );
    
    CYG_REPORT_RETVAL( retval );

    return retval;
} // _strtok()

char *
_strtok_r( char *s1, const char *s2, char **lasts )
{
    char *spanp;
    int c, sc;
    char *tok;
    
    CYG_REPORT_FUNCNAMETYPE( "_strtok_r", "returning %08x" );
    CYG_REPORT_FUNCARG3( "s1=%08x, s2=%08x, lasts=%08x", s1, s2, lasts );
    
    if (s1 != NULL)
        CYG_CHECK_DATA_PTR( s1, "s1 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( s2, "s2 is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( lasts, "lasts is not a valid pointer!" );


    if (s1 == NULL && (s1 = *lasts) == NULL)
    {
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if
    
    //
    // Skip (span) leading delimiters (s += strspn(s, delim), sort of).
    //
cont:
    c = *s1++;
    for (spanp = (char *)s2; (sc = *spanp++) != 0;) {
        if (c == sc)
            goto cont;
    } // for
    
    if (c == 0) {               // no non-delimiter characters
        *lasts = NULL;

        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if
    tok = s1 - 1;
    
    //
    // Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
    // Note that delim must have one NUL; we stop if we see that, too.
    //
    for (;;) {
        c = *s1++;
        spanp = (char *)s2;
        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s1 = NULL;
                else
                    s1[-1] = 0;
                *lasts = s1;

                CYG_REPORT_RETVAL( tok );

                return (tok);
            } // if
        } while (sc != 0);
    } // for
    // NOTREACHED
} // _strtok_r()

#endif // if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtok)

// EOF strtok.cxx
