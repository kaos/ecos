//===========================================================================
//
//      ldiv.cxx
//
//      Real alternative for inline implementation of the ANSI standard
//      ldiv() utility function defined in section 7.10.6.4 of the standard
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
// This code is based on code with the following copyright:
//
/*
 * Copyright (c) 1990 Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Chris Torek.
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

#include <pkgconf/libc.h>           // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_trac.h>     // Tracing support

// We don't want the inline versions of stdlib functions defined here

#ifdef CYGIMP_LIBC_STDLIB_INLINES
#undef CYGIMP_LIBC_STDLIB_INLINES
#endif

#include <stddef.h>                 // NULL, wchar_t and size_t from compiler
#include <stdlib.h>                 // Main header for stdlib functions
#include "clibincl/stdlibsupp.hxx"  // Support for stdlib functions


// EXPORTED SYMBOLS

externC ldiv_t
ldiv( long numer, long denom ) CYGPRI_LIBC_WEAK_ALIAS("_ldiv");


// FUNCTIONS

ldiv_t
_ldiv( long numer, long denom )
{
    ldiv_t result;
    
    CYG_REPORT_FUNCNAMETYPE( "_ldiv", "quotient: %d");
    CYG_REPORT_FUNCARG2( "numer=%d, denom=%d", numer, denom );
    
    // The ANSI standard says that |r.quot| <= |n/d|, where
    // n/d is to be computed in infinite precision.  In other
    // words, we should always truncate the quotient towards
    // 0, never -infinity.
    //
    // Machine ldivision and remainer may work either way when
    // one or both of n or d is negative.  If only one is
    // negative and r.quot has been truncated towards -inf,
    // r.rem will have the same sign as denom and the opposite
    // sign of num; if both are negative and r.quot has been
    // truncated towards -inf, r.rem will be positive (will
    // have the opposite sign of num).  These are considered
    // `wrong'.
    //
    // If both are num and denom are positive, r will always
    // be positive.
    //
    // This all boils down to:
    //      if num >= 0, but r.rem < 0, we got the wrong answer.
    // In that case, to get the right answer, add 1 to r.quot and
    // subtract denom from r.rem.

    result.quot = numer / denom;
    result.rem = numer % denom;

    if ( (numer >= 0) && (result.rem < 0) )
    {
        result.quot++;
        result.rem -= denom;
    } // if

    CYG_REPORT_RETVAL( result.quot );

    return result;
} // _ldiv()

#endif // ifdef CYGPKG_LIBC     

// EOF ldiv.cxx
