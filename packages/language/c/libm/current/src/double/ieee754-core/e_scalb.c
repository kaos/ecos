//===========================================================================
//
//      e_scalb.c
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


/* @(#)e_scalb.c 1.3 95/01/18 */
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
 * __ieee754_scalb(x, fn) is provide for
 * passing various standard test suite. One 
 * should use scalbn() instead.
 */

#include "mathincl/fdlibm.h"

#ifdef CYGFUN_LIBM_SVID3_scalb
        double __ieee754_scalb(double x, double fn)
#else
        double __ieee754_scalb(double x, int fn)
#endif
{
#ifdef CYGFUN_LIBM_SVID3_scalb
        if (isnan(x)||isnan(fn)) return x*fn;
        if (!finite(fn)) {
            if(fn>0.0) return x*fn;
            else       return x/(-fn);
        }
        if (rint(fn)!=fn) return (fn-fn)/(fn-fn);
        if ( fn > 65000.0) return scalbn(x, 65000);
        if (-fn > 65000.0) return scalbn(x,-65000);
        return scalbn(x,(int)fn);
#else
        return scalbn(x,fn);
#endif
}

#endif // ifdef CYGPKG_LIBM     

// EOF e_scalb.c
