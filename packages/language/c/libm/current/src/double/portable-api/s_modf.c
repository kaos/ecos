//===========================================================================
//
//      s_modf.c
//
//      Part of the standard mathematical function library
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
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// Derived from code with the following copyright


/* @(#)s_modf.c 1.3 95/01/18 */
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
 * modf(double x, double *iptr) 
 * return fraction part of x, and return x's integral part in *iptr.
 * Method:
 *      Bit twiddling.
 *
 * Exception:
 *      No exception.
 */

#include "mathincl/fdlibm.h"

static const double one = 1.0;

        double modf(double x, double *iptr)
{
        int i0,i1,j0;
        unsigned i;
        i0 =  CYG_LIBM_HI(x);           /* high x */
        i1 =  CYG_LIBM_LO(x);           /* low  x */
        j0 = ((i0>>20)&0x7ff)-0x3ff;    /* exponent of x */
        if(j0<20) {                     /* integer part in high x */
            if(j0<0) {                  /* |x|<1 */
                CYG_LIBM_HIp(iptr) = i0&0x80000000;
                CYG_LIBM_LOp(iptr) = 0;         /* *iptr = +-0 */
                return x;
            } else {
                i = (0x000fffff)>>j0;
                if(((i0&i)|i1)==0) {            /* x is integral */
                    *iptr = x;
                    CYG_LIBM_HI(x) &= 0x80000000;
                    CYG_LIBM_LO(x)  = 0;        /* return +-0 */
                    return x;
                } else {
                    CYG_LIBM_HIp(iptr) = i0&(~i);
                    CYG_LIBM_LOp(iptr) = 0;
                    return x - *iptr;
                }
            }
        } else if (j0>51) {             /* no fraction part */
            *iptr = x*one;
            CYG_LIBM_HI(x) &= 0x80000000;
            CYG_LIBM_LO(x)  = 0;        /* return +-0 */
            return x;
        } else {                        /* fraction part in low x */
            i = ((unsigned)(0xffffffff))>>(j0-20);
            if((i1&i)==0) {             /* x is integral */
                *iptr = x;
                CYG_LIBM_HI(x) &= 0x80000000;
                CYG_LIBM_LO(x)  = 0;    /* return +-0 */
                return x;
            } else {
                CYG_LIBM_HIp(iptr) = i0;
                CYG_LIBM_LOp(iptr) = i1&(~i);
                return x - *iptr;
            }
        }
}

#endif // ifdef CYGPKG_LIBM     

// EOF s_modf.c
