//========================================================================
//
//      w_pow.c
//
//      Part of the standard mathematical function library
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// Derived from code with the following copyright



/* @(#)w_pow.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 */

/* 
 * wrapper pow(x,y) return x**y
 */

#include "mathincl/fdlibm.h"


        double pow(double x, double y)  /* wrapper pow */
{
#ifdef CYGSEM_LIBM_COMPAT_IEEE_ONLY
        return  __ieee754_pow(x,y);
#else
        double z;
        z=__ieee754_pow(x,y);
        if(cyg_libm_get_compat_mode() == CYGNUM_LIBM_COMPAT_IEEE||
           isnan(y))
            return z;
        if(isnan(x)) {
            if(y==0.0) 
                return __kernel_standard(x,y,42); /* pow(NaN,0.0) */
            else 
                return z;
        }
        if(x==0.0){ 
            if(y==0.0)
                return __kernel_standard(x,y,20); /* pow(0.0,0.0) */
            if(finite(y)&&y<0.0)
                return __kernel_standard(x,y,23); /* pow(0.0,negative) */
            return z;
        }
        if(!finite(z)) {
            if(finite(x)&&finite(y)) {
                if(isnan(z))
                    return __kernel_standard(x,y,24);/* pow neg**non-int */
                else 
                    return __kernel_standard(x,y,21); /* pow overflow */
            }
        } 
        if(z==0.0&&finite(x)&&finite(y))
            return __kernel_standard(x,y,22); /* pow underflow */
        return z;
#endif
}

#endif // ifdef CYGPKG_LIBM     

// EOF w_pow.c
