//===========================================================================
//
//      strtod.cxx
//
//      ANSI String to double-precision floating point conversion
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

#include <pkgconf/libc.h>           // Configuration header

// Include the C library? And strtod()?
#if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtod)

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_trac.h>     // Tracing support
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <stddef.h>                 // NULL, wchar_t and size_t from compiler
#include <stdlib.h>                 // Main header for stdlib functions
#include <ctype.h>                  // isspace() and isdigit()
#include "clibincl/stdlibsupp.hxx"  // Support for stdlib functions
#include <float.h>                  // DBL_MIN_10_EXP and DBL_MAX_10_EXP
#include <math.h>                   // HUGE_VAL
#include <errno.h>                  // errno

// EXPORTED SYMBOLS

externC double
strtod( const char *, char ** ) CYGPRI_LIBC_WEAK_ALIAS("_strtod");

// CONSTANTS

#define MAXE (DBL_MAX_10_EXP)
#define MINE (DBL_MIN_10_EXP)

// flags
#define SIGN    0x01
#define ESIGN   0x02
#define DECP    0x04


// MACROS

#define Ise(c)          ((c == 'e') || (c == 'E') || (c == 'd') || (c == 'D'))
#define Issign(c)       ((c == '-') || (c == '+'))
#define Val(c)          ((c - '0'))


// FUNCTIONS

/*
 * [atw] multiply 64 bit accumulator by 10 and add digit.
 * The KA/CA way to do this should be to use
 * a 64-bit integer internally and use "adjust" to
 * convert it to float at the end of processing.
 */
static int
ten_mul(double *acc, int digit)
{
    /* [atw] Crude, but effective (at least on a KB)...
     */
    *acc *= 10;
    *acc += digit;
    
    return 0;     /* no overflow */
} // ten_mul()


/*
 * compute 10**x by successive squaring.
 */

static const double
exp10(unsigned x)
{
    static double powtab[] = {1.0,
                              10.0,
                              100.0,
                              1000.0,
                              10000.0};
    
    if (x < (sizeof(powtab)/sizeof(double)))
        return powtab[x];
    else if (x & 1)
        return 10.0 * exp10(x-1);
    else
        return exp10(x/2) * exp10(x/2);
} // exp10()


/*
 * return (*acc) scaled by 10**dexp.
 */

static double
adjust(double *acc, int dexp, int sign)
     /* *acc    the 64 bit accumulator */
     /* dexp    decimal exponent       */
     /* sign    sign flag              */
{
    double r;
    
    if (dexp > MAXE)
    {
        errno = ERANGE;
        return (sign) ? -HUGE_VAL : HUGE_VAL;
    }
    else if (dexp < MINE)
    {
        errno = ERANGE;
        return 0.0;
    }
    
    r = *acc;
    if (sign)
        r = -r;
    if (dexp==0)
        return r;
    
    if (dexp < 0)
        return r / exp10(abs(dexp));
    else
        return r * exp10(dexp);
} // adjust()


externC double
_strtod( const char *nptr, char **endptr )
{
    const char *start=nptr;
    double accum = 0.0;
    int flags = 0;
    int texp  = 0;
    int e     = 0;
    int conv_done = 0;
  
    double retval;

    CYG_REPORT_FUNCNAMETYPE( "_strtod", "returning %f" );

    CYG_CHECK_DATA_PTR( nptr, "nptr is an invalid pointer!" );

    // endptr is allowed to be NULL, but if it isn't, we check it
    if (endptr != NULL)
        CYG_CHECK_DATA_PTR( endptr, "endptr is an invalid pointer!" );
    
    while(isspace(*nptr)) nptr++;
    if(*nptr == '\0')
    {   /* just leading spaces */
        if(endptr != NULL) *endptr = (char *)start;
        return 0.0;
    }
    
    
    if(Issign(*nptr))
    {
        if(*nptr == '-') flags = SIGN;
        if(*++nptr == '\0')
        {   /* "+|-" : should be an error ? */
            if(endptr != NULL) *endptr = (char *)start;
            return 0.0;
        }
    }
    
    for(; (isdigit(*nptr) || (*nptr == '.')); nptr++)
    {
        conv_done = 1;
        if(*nptr == '.')
            flags |= DECP;
        else
        {
            if( ten_mul(&accum, Val(*nptr)) ) texp++;
            if(flags & DECP) texp--;
        }
    }
    
    if(Ise(*nptr))
    {
        conv_done = 1;
        if(*++nptr != '\0') /* skip e|E|d|D */
        {  /* ! ([nptr]xxx[.[yyy]]e)  */
            
            while(isspace(*nptr)) nptr++; /* Ansi allows spaces after e */
            if(*nptr != '\0')
            { /*  ! ([nptr]xxx[.[yyy]]e[space])  */
                
                if(Issign(*nptr))
                    if(*nptr++ == '-') flags |= ESIGN;
                
                if(*nptr != '\0')
                { /*  ! ([nptr]xxx[.[yyy]]e[nptr])  -- error?? */
                    
                    for(; isdigit(*nptr); nptr++)
                        if (e < MAXE) /* prevent from grossly overflowing */
                            e = e*10 + Val(*nptr);
                    
                    /* dont care what comes after this */
                    if(flags & ESIGN)
                        texp -= e;
                    else
                        texp += e;
                }
            }
        }
    }
    
    if(endptr != NULL) 
        *endptr = (char *)((conv_done) ? nptr : start);
    
    retval = adjust(&accum, (int)texp, (int)(flags & SIGN));
  

    CYG_REPORT_RETVAL( retval );

    return retval;
} // _strtod()


#endif // if defined(CYGPKG_LIBC) && defined(CYGFUN_LIBC_strtod)

// EOF strtod.cxx
