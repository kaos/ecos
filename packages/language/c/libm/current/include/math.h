#ifndef CYGONCE_LIBM_MATH_H
#define CYGONCE_LIBM_MATH_H
//===========================================================================
//
//      math.h
//
//      Standard mathematical functions conforming to ANSI and other standards
//
//===========================================================================
//####COPYRIGHTBEGIN####
//                                                                          
// -------------------------------------------                              
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Description: Standard mathematical functions. These can be
//              configured to conform to ANSI section 7.5. There are also
//              a number of extensions conforming to IEEE-754 and behaviours
//              compatible with other standards
// Usage:       #include <math.h>
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
#include <float.h>                 // Properties of FP representation on this
                                   // platform
#include <sys/ieeefp.h>            // Cyg_libm_ieee_double_shape_type

// CONSTANT DEFINITIONS


// HUGE_VAL is a positive double (not necessarily representable as a float)
// representing infinity as specified in ANSI 7.5. cyg_libm_infinity is
// defined further down
#define HUGE_VAL        (cyg_libm_infinity.value)


#ifndef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION
// HUGE is defined in System V Interface Definition 3 (SVID3) as the largest
// finite single precision number
#define HUGE            FLT_MAX    // from float.h


// Values used in the type field of struct exception below

#define DOMAIN          1
#define SING            2
#define OVERFLOW        3
#define UNDERFLOW       4
#define TLOSS           5
#define PLOSS           6


// TYPE DEFINITIONS

// Things required to support matherr() ( see comments in <pkgconf/libm.h>)

struct exception {
    int type;       // One of DOMAIN, SING, OVERFLOW, UNDERFLOW, TLOSS, PLOSS
    char *name;     // Name of the function generating the exception
    double arg1;    // First argument to the function
    double arg2;    // Second argument to the function
    double retval;  // Value to be returned - can be altered by matherr()
};

#endif // ifndef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION


// GLOBALS

externC const Cyg_libm_ieee_double_shape_type cyg_libm_infinity;

//===========================================================================
// FUNCTION PROTOTYPES

// Functions not part of a standard

// This retrieves a pointer to the current compatibility mode of the Math
// library. See <pkgconf/libm.h> for the definition of Cyg_libm_compat_t

#ifdef CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE

externC Cyg_libm_compat_t
cyg_libm_get_compat_mode( void );

externC Cyg_libm_compat_t
cyg_libm_set_compat_mode( Cyg_libm_compat_t );

#else

externC Cyg_libm_compat_t cygvar_libm_compat_mode;

// Defined as static inline as it is unlikely that anyone wants to take the
// address of these functions.
//
// This returns the current compatibility mode

static inline Cyg_libm_compat_t
cyg_libm_get_compat_mode( void )
{
    return cygvar_libm_compat_mode;
}

// This sets the compatibility mode, and returns the previous mode
static inline Cyg_libm_compat_t
cyg_libm_set_compat_mode( Cyg_libm_compat_t math_compat_mode)
{
    Cyg_libm_compat_t oldmode;

    oldmode = cygvar_libm_compat_mode;
    cygvar_libm_compat_mode = math_compat_mode;
    return oldmode;
}

#endif // ifdef CYGSEM_LIBM_THREAD_SAFE_COMPAT_MODE


//===========================================================================
// Standard ANSI functions. Angles are always in radians

// Trigonometric functions - ANSI 7.5.2

externC double
acos( double );            // arc cosine i.e. inverse cos

externC double
asin( double );            // arc sine i.e. inverse sin

externC double
atan( double );            // arc tan i.e. inverse tan

externC double
atan2( double, double );   // arc tan of (first arg/second arg) using signs
                           // of args to determine quadrant

externC double
cos( double );             // cosine

externC double
sin( double );             // sine

externC double
tan( double );             // tangent

// Hyperbolic functions - ANSI 7.5.3

externC double
cosh( double );            // hyperbolic cosine

externC double
sinh( double );            // hyperbolic sine

externC double
tanh( double );            // hyperbolic tangent

// Exponential and Logarithmic Functions - ANSI 7.5.4

externC double
exp( double );             // exponent

externC double
frexp( double, int * );    // break number into normalized fraction (returned)
                           // and integral power of 2 (second arg)

externC double
ldexp( double, int );      // multiples number by integral power of 2 

externC double
log( double );             // natural logarithm

externC double
log10( double );           // base ten logarithm

externC double
modf( double, double * );  // break number into integral and fractional
                           // parts, each of which has same sign as arg.
                           // It returns signed fractional part, and
                           // puts integral part in second arg

// Power Functions - ANSI 7.5.5

externC double
pow( double, double );     // (1st arg) to the power of (2nd arg)

externC double
sqrt( double );            // square root

// Nearest integer, absolute value and remainder functions - ANSI 7.5.6

externC double
ceil( double );            // smallest integer >= arg

externC double
fabs( double );            // absolute value

externC double
floor( double );           // largest integer <= arg

externC double
fmod( double, double );    // remainder of (1st arg)/(2nd arg)

//===========================================================================
// Other standard functions

#ifndef CYGSYM_LIBM_NO_XOPEN_SVID_NAMESPACE_POLLUTION
externC int
matherr( struct exception * );    // User-overridable error handling - see
#endif                            // <pkgconf/libm.h> for a discussion

// scalb*()

externC double                    // scalbn(x,n) returns x*(2**n)
scalbn( double, int );

#ifdef CYGFUN_LIBM_SVID3_scalb

externC double
scalb( double, double );          // as above except n is a floating point arg

#else
externC double
scalb( double, int );             // as scalbn()

#endif // ifdef CYGFUN_LIBM_SVID3_scalb

// And the rest


externC int                       // whether the argument is NaN
isnan( double );

externC int
finite( double );                 // whether the argument is finite

                                 
//===========================================================================
// Non-standard functions

externC double                    // copysign(x,y) returns a number with
copysign ( double, double );      // the absolute value of x and the sign of y

externC double                    // rounds to an integer according to the
rint( double );                   // current rounding mode


// BSD functions

externC double                    // expm1(x) returns the equivalent of
expm1( double );                  // (exp(x) - 1) but more accurately when
                                  // x tends to zero


#endif // ifdef CYGPKG_LIBM     

#endif // CYGONCE_LIBM_MATH_H multiple inclusion protection

// EOF math.h
