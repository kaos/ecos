//===========================================================================
//
//      bsearch.cxx
//
//      ANSI standard binary search function defined in section 7.10.5.1
//      of the standard
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

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#ifdef CYGPKG_LIBC     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions

// TRACING

# if defined(CYGDBG_USE_TRACING) && \
     defined(CYGNUM_LIBC_BSEARCH_TRACE_LEVEL)
static int bsearch_trace = CYGNUM_LIBC_BSEARCH_TRACE_LEVEL;
#  define TL1 (0 < bsearch_trace)
# else
#  define TL1 (0)
# endif


// EXPORTED SYMBOLS

externC void *
bsearch( const void *key, const void *base, size_t nmemb, size_t size,
         Cyg_comparison_fn_t compar ) CYGPRI_LIBC_WEAK_ALIAS("_bsearch");

// FUNCTIONS

externC void *
_bsearch( const void *key, const void *base, size_t nmemb, size_t size,
          Cyg_comparison_fn_t compar )
{
    CYG_REPORT_FUNCNAMETYPE( "_bsearch", "returning %08x" );

    CYG_REPORT_FUNCARG5( "key=%08x, base=%08x, nmemb=%d, size=%d, "
                         "compar=%08x", key, base, nmemb, size, compar );

    CYG_CHECK_DATA_PTR( key, "key is not a valid pointer!" );
    CYG_CHECK_DATA_PTR( base, "base is not a valid pointer!" );
    CYG_CHECK_FUNC_PTR( compar, "compar is not a valid function pointer!" );

    CYG_ADDRESS current;
    size_t lower = 0;
    size_t upper = nmemb;
    size_t index;
    int result;
    
    if (nmemb == 0 || size == 0)
    {
        CYG_TRACE2( TL1, "Warning! either nmemb (%d) or size (%d) is 0",
                    nmemb, size );
        CYG_REPORT_RETVAL( NULL );
        return NULL;
    } // if
    
    while (lower < upper)
    {
        index = (lower + upper) / 2;
        current = (CYG_ADDRESS) (((char *) base) + (index * size));
        
        CYG_TRACE2( TL1, "About to call comparison function with "
                    "key=%08x, current=%08x", key, current );
        result = compar (key, (void *) current);
        CYG_TRACE1( TL1, "Comparison function returned %d", result );
        
        if (result < 0)
            upper = index;
        else if (result > 0)
            lower = index + 1;
        else
        {
            CYG_REPORT_RETVAL( current );
            return (void *)current;
        } // else
    } // while
    
    CYG_REPORT_RETVAL( NULL );
    return NULL;
} // _bsearch()

#endif // ifdef CYGPKG_LIBC     

// EOF bsearch.cxx
