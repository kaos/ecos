#ifndef CYGONCE_LIBM_MATHINCL_FDLIBM_H
#define CYGONCE_LIBM_MATHINCL_FDLIBM_H
//===========================================================================
//
//      fdlibm.h
//
//      Internal definitions for math library implementation based on fdlibm
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-09
// Purpose:     
// Description:   Internal implementation-specific header for math library
//                based on fdlibm
// Usage:         From within this package, #include "mathincl/fdlibm.h"
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <math.h>                  // Main header for math library
#include <float.h>                 // Properties of FP representation on this
                                   // platform

// SANITY CHECKS

// Just check that we support IEEE-style 64-bit doubles. If not, this
// math library will not work
// This check will go away when support for single-precision alternatives are
// provided

#if DBL_MAX_EXP != 1024
# error IEEE-style 64-bit doubles are required to use the math library
#endif // if DBL_MAX_EXP == 1024


// MACRO DEFINITIONS

#define CYG_LIBM_HI(__x)  (*cyg_libm_hi(&__x))
#define CYG_LIBM_LO(__x)  (*cyg_libm_lo(&__x))
#define CYG_LIBM_HIp(__x) (*cyg_libm_hi(__x))
#define CYG_LIBM_LOp(__x) (*cyg_libm_lo(__x))

// REPLACEMENTS FOR STUFF FROM MATH.H DUE TO CONFIG OPTION

#ifdef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION

#define HUGE            FLT_MAX    // from float.h
#define DOMAIN          1
#define SING            2
#define OVERFLOW        3
#define UNDERFLOW       4
#define TLOSS           5
#define PLOSS           6

struct exception {
    int type;       // One of DOMAIN, SING, OVERFLOW, UNDERFLOW, TLOSS, PLOSS
    char *name;     // Name of the function generating the exception
    double arg1;    // First argument to the function
    double arg2;    // Second argument to the function
    double retval;  // Value to be returned - can be altered by matherr()
};

externC int
matherr( struct exception * );    // User-overridable error handling - see
                                  // <pkgconf/libm.h> for a discussion
#endif // ifdef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION


// INLINE FUNCTIONS

static __inline__ cyg_uint32 *
cyg_libm_hi(double *__x)
{
    Cyg_libm_ieee_double_shape_type *__y;
    __y = (Cyg_libm_ieee_double_shape_type *)__x;
    return &__y->parts.msw;
}

static __inline__ cyg_uint32 *
cyg_libm_lo(double *__x)
{
    Cyg_libm_ieee_double_shape_type *__y;
    __y = (Cyg_libm_ieee_double_shape_type *)__x;
    return &__y->parts.lsw;
}


// FUNCTION PROTOTYPES

// IEEE-754 style elementary functions */

externC double
__ieee754_sqrt( double );

externC double
__ieee754_acos( double );


externC double
__ieee754_log( double );


externC double
__ieee754_asin( double );

externC double
__ieee754_atan2( double, double );

externC double
__ieee754_exp( double );

externC double
__ieee754_cosh( double );

externC double
__ieee754_fmod( double, double );

externC double
__ieee754_pow( double, double );


externC double
__ieee754_log10( double );

externC double
__ieee754_sinh( double );


externC int
__ieee754_rem_pio2( double, double * );

#ifdef CYGFUN_LIBM_SVID3_scalb
externC double
__ieee754_scalb( double, double );
#else
externC double
__ieee754_scalb( double, int );
#endif

// FDLIBM kernel functions

externC double
__kernel_standard( double, double, int );

externC double
__kernel_sin( double, double, int );

externC double
__kernel_cos( double, double );

externC double
__kernel_tan( double, double, int );

externC int
__kernel_rem_pio2( double *, double *, int, int, int, const int * );

#endif // ifdef CYGPKG_LIBM     

#endif // CYGONCE_LIBM_MATHINCL_FDLIBM_H multiple inclusion protection

// EOF fdlibm.h
