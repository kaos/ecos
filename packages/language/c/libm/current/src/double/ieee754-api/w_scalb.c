//===========================================================================
//
//      w_scalb.c
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour@cygnus.co.uk
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

#include <pkgconf/libm.h>   // Configuration header

// Include the Math library?
#ifdef CYGPKG_LIBM     

// Derived from code with the following copyright


/* @(#)w_scalb.c 1.3 95/01/18 */
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
 * wrapper scalb(double x, double fn) is provide for
 * passing various standard test suite. One 
 * should use scalbn() instead.
 */

#include "mathincl/fdlibm.h"

#ifndef CYGSEM_LIBM_COMPAT_IEEE_ONLY
# include <errno.h>
#endif

#ifdef CYGFUN_LIBM_SVID3_scalb
        double scalb(double x, double fn)       /* wrapper scalb */
#else
        double scalb(double x, int fn)          /* wrapper scalb */
#endif
{
#ifdef CYGSEM_LIBM_COMPAT_IEEE_ONLY
        return __ieee754_scalb(x,fn);
#else
        double z;
        z = __ieee754_scalb(x,fn);
        if(cyg_libm_get_compat_mode() == CYGNUM_LIBM_COMPAT_IEEE) return z;
        if(!(finite(z)||isnan(z))&&finite(x)) {
            return __kernel_standard(x,(double)fn,32); /* scalb overflow */
        }
        if(z==0.0&&z!=x) {
            return __kernel_standard(x,(double)fn,33); /* scalb underflow */
        } 
#ifdef CYGFUN_LIBM_SVID3_scalb
        if(!finite(fn)) errno = ERANGE;
#endif
        return z;
#endif 
}

#endif // ifdef CYGPKG_LIBM     

// EOF w_scalb.c
