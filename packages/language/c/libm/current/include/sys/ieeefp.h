#ifndef CYGONCE_LIBM_SYS_IEEEFP_H
#define CYGONCE_LIBM_SYS_IEEEFP_H
//===========================================================================
//
//      ieeefp.h
//
//      Definitions specific to IEEE-754 floating-point format
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: Definitions specific to IEEE-754 floating-point format
// Usage:       #include <sys/ieeefp.h>
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// INCLUDES

#include <cyg/infra/cyg_type.h>     // Common type definitions and support
                                    // including endian-ness

#if (CYG_BYTEORDER == CYG_MSBFIRST) // Big endian

// Note: there do not seem to be any current machines which are Big Endian but
// have a mixed up double layout. 

typedef union 
{
    cyg_int32 asi32[2];

    cyg_int64 asi64;
    
    double value;
    
    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 11;
        unsigned int fraction0:4;
        unsigned int fraction1:16;
        unsigned int fraction2:16;
        unsigned int fraction3:16;
        
    } number;
    
    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 11;
        unsigned int quiet:1;
        unsigned int function0:3;
        unsigned int function1:16;
        unsigned int function2:16;
        unsigned int function3:16;
    } nan;
    
    struct 
    {
        cyg_uint32 msw;
        cyg_uint32 lsw;
    } parts;

    
} Cyg_libm_ieee_double_shape_type;


typedef union
{
    cyg_int32 asi32;
    
    float value;

    struct 
    {
        unsigned int sign : 1;
        unsigned int exponent: 8;
        unsigned int fraction0: 7;
        unsigned int fraction1: 16;
    } number;

    struct 
    {
        unsigned int sign:1;
        unsigned int exponent:8;
        unsigned int quiet:1;
        unsigned int function0:6;
        unsigned int function1:16;
    } nan;
    
} Cyg_libm_ieee_float_shape_type;


#else // Little endian

typedef union 
{
    cyg_int32 asi32[2];

    cyg_int64 asi64;
    
    double value;

    struct 
    {
#if (CYG_DOUBLE_BYTEORDER == CYG_MSBFIRST) // Big endian
        unsigned int fraction1:16;
        unsigned int fraction0: 4;
        unsigned int exponent :11;
        unsigned int sign     : 1;
        unsigned int fraction3:16;
        unsigned int fraction2:16;
#else
        unsigned int fraction3:16;
        unsigned int fraction2:16;
        unsigned int fraction1:16;
        unsigned int fraction0: 4;
        unsigned int exponent :11;
        unsigned int sign     : 1;
#endif
    } number;

    struct 
    {
#if (CYG_DOUBLE_BYTEORDER == CYG_MSBFIRST) // Big endian
        unsigned int function1:16;
        unsigned int function0:3;
        unsigned int quiet:1;
        unsigned int exponent: 11;
        unsigned int sign : 1;
        unsigned int function3:16;
        unsigned int function2:16;
#else
        unsigned int function3:16;
        unsigned int function2:16;
        unsigned int function1:16;
        unsigned int function0:3;
        unsigned int quiet:1;
        unsigned int exponent: 11;
        unsigned int sign : 1;
#endif
    } nan;

    struct 
    {
#if (CYG_DOUBLE_BYTEORDER == CYG_MSBFIRST) // Big endian
        cyg_uint32 msw;
        cyg_uint32 lsw;
#else
        cyg_uint32 lsw;
        cyg_uint32 msw;
#endif
    } parts;
    
} Cyg_libm_ieee_double_shape_type;


typedef union
{
    cyg_int32 asi32;
  
    float value;

    struct 
    {
        unsigned int fraction0: 7;
        unsigned int fraction1: 16;
        unsigned int exponent: 8;
        unsigned int sign : 1;
    } number;

    struct 
    {
        unsigned int function1:16;
        unsigned int function0:6;
        unsigned int quiet:1;
        unsigned int exponent:8;
        unsigned int sign:1;
    } nan;

} Cyg_libm_ieee_float_shape_type;

#endif // little-endian


#endif // ifdef CYGPKG_LIBM     

#endif // CYGONCE_LIBM_SYS_IEEEFP_H multiple inclusion protection

// EOF ieeefp.h
