//===========================================================================
//
//      rand.cxx
//
//      ANSI and POSIX 1003.1 standard random number generation functions
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
// Description: This implements rand() and srand() of section 7.10.2.1 of
//              the ANSI standard. Also rand_r() defined in section 8.3.8
//              of the POSIX 1003.1 standard
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// Include the C library?
#if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_RAND)

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/cyg_trac.h>    // Tracing support
#include <cyg/infra/cyg_ass.h>     // Assertion support
#include <stdlib.h>                // Header for all stdlib functions
                                   // (like this one)
#include "clibincl/stdlibsupp.hxx" // Support for stdlib functions
#include "clibincl/clibdata.hxx"   // Header for thread-specific data such
                                   // as random seed

// TRACE

#if defined(CYGDBG_USE_TRACING) && defined(CYGNUM_LIBC_RAND_TRACE_LEVEL)
static int rand_trace = CYGNUM_LIBC_RAND_TRACE_LEVEL;
# define TL1 (0 < rand_trace)
#else
# define TL1 (0)
#endif


// EXPORTED SYMBOLS

externC int
rand( void ) CYGPRI_LIBC_WEAK_ALIAS("_rand");

externC int
rand_r( unsigned int *seed ) CYGPRI_LIBC_WEAK_ALIAS("_rand_r");

externC void
srand( unsigned int seed ) CYGPRI_LIBC_WEAK_ALIAS("_srand");


// FUNCTIONS

int
_rand( void )
{
    Cyg_CLibInternalData::Cyg_rand_seed_t *seed_p;
    int retval;

    CYG_REPORT_FUNCNAMETYPE( "_rand", "returning %d" );

    CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE;

    // get seed for this thread (if relevant )
    seed_p = CYGPRI_LIBC_INTERNAL_DATA.get_rand_seed_p();

    CYG_TRACE2( TL1, "Retrieved seed address %08x containing %d",
                seed_p, *seed_p );

    retval = _rand_r( (unsigned int *) seed_p );

    CYG_REPORT_RETVAL( retval );

    return retval;

} // _rand()


int 
_rand_r( unsigned int *seed )
{
    int retval;
    
    CYG_REPORT_FUNCNAMETYPE( "_rand_r", "returning %d" );

    CYG_CHECK_DATA_PTR( seed, "pointer to seed invalid!" );

    *seed = (*seed * 1103515245) + 12345; // permutate seed
    
    retval = (int)( *seed & RAND_MAX );

    CYG_REPORT_RETVAL( retval );

    return retval;
   
} // _rand_r()


void
_srand( unsigned int seed )
{
    Cyg_CLibInternalData::Cyg_rand_seed_t *seed_p;

    CYG_REPORT_FUNCNAMETYPE( "_srand", "returning %d" );

    CYG_REPORT_FUNCARG1DV( (int)seed );

    CYGPRI_LIBC_INTERNAL_DATA_ALLOC_CHECK_PREAMBLE;

    // get seed for this thread ( if relevant )
    seed_p = CYGPRI_LIBC_INTERNAL_DATA.get_rand_seed_p();

    CYG_CHECK_DATA_PTR( seed_p, "Help! Returned address of seed is invalid!" );
    
    CYG_TRACE1( TL1, "Retrieved seed address %08x", seed_p );

    *seed_p = (Cyg_CLibInternalData::Cyg_rand_seed_t) seed;

    CYG_REPORT_RETURN();

} // _srand()


#endif // if defined(CYGPKG_LIBC) && defined(CYGPKG_LIBC_RAND)

// EOF rand.cxx
