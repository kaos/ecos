//===========================================================================
//
//      ldiv.cxx
//
//      ISO C implementation for ldiv() utility function
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
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-03-02
// Purpose:       Provide implementation of ldiv() from ISO C section 7.10.6.4
// Description:   
// Usage:         
//
//####DESCRIPTIONEND####
//
//===========================================================================
//

// CONFIGURATION

#include <pkgconf/libc.h>           // Configuration header

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
#include <cyg/infra/cyg_ass.h>      // Assertion support
#include <cyg/infra/cyg_trac.h>     // Tracing support

// We don't want the inline versions of stdlib functions defined here

#ifdef CYGIMP_LIBC_STDLIB_INLINES
# undef CYGIMP_LIBC_STDLIB_INLINES
#endif

#include <stddef.h>                 // NULL, wchar_t and size_t from compiler
#include <stdlib.h>                 // Main header for stdlib functions


// FUNCTIONS

externC ldiv_t
__ldiv( long __numer, long __denom )
{
    ldiv_t __ret;

    CYG_REPORT_FUNCNAMETYPE( "__ldiv", "quotient: %d");
    CYG_REPORT_FUNCARG2DV( __numer, __denom );
    // FIXME: what if they want it handled with SIGFPE? Should have option
    CYG_PRECONDITION(__denom != 0, "division by zero attempted!");
    
    __ret.quot = __numer / __denom;
    __ret.rem  = __numer % __denom;

    // But the modulo is implementation-defined for -ve numbers (ISO C 6.3.5)
    // and we are required to "round" to zero (ISO C 7.10.6.2)
    //
    // The cases we have to deal with are inexact division of:
    // a) + div +
    // b) + div -
    // c) - div +
    // d) - div -
    //
    // a) can never go wrong and the quotient and remainder are always positive
    // b) only goes wrong if the negative quotient has been "rounded" to
    //    -infinity - if so then the remainder will be negative when it
    //    should be positive or zero
    // c) only goes wrong if the negative quotient has been "rounded" to
    //    -infinity - if so then the remainder will be positive when it
    //    should be negative or zero
    // d) only goes wrong if the positive quotient has been rounded to
    //    +infinity - if so then the remainder will be positive when it
    //    should be negative or zero
    //
    // So the correct sign of the remainder corresponds to the sign of the
    // numerator. Which means we can say that the result needs adjusting
    // iff the sign of the numerator is different from the sign of the
    // remainder.
    //
    // You may be interested to know that the Berkeley version of ldiv()
    // would get this wrong for (c) and (d) on some targets.
    // e.g. for (-5)/4 it could leave the result as -2R3

    if ((__ret.rem < 0) && (__numer > 0)) {
        ++__ret.quot;
        __ret.rem -= __denom;
    } else if ((__ret.rem > 0) && (__numer < 0)) {
        --__ret.quot;
        __ret.rem += __denom;
    } // else

    CYG_REPORT_RETVAL( __ret.quot );

    return __ret;
} // __ldiv()


// EXPORTED SYMBOLS

externC ldiv_t
ldiv( long __numer, long __denom ) CYGBLD_ATTRIB_WEAK_ALIAS(__ldiv);


// EOF ldiv.cxx
