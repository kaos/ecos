//===========================================================================
//
//      w_atan2.c
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


/* @(#)w_atan2.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice 
 * is preserved.
 * ====================================================
 *
 */

/* 
 * wrapper atan2(y,x)
 */

#include "mathincl/fdlibm.h"


        double atan2(double y, double x)        /* wrapper atan2 */
{
#ifdef CYGSEM_LIBM_COMPAT_IEEE_ONLY
        return __ieee754_atan2(y,x);
#else
        double z;
        z = __ieee754_atan2(y,x);
        if(cyg_libm_get_compat_mode() == CYGNUM_LIBM_COMPAT_IEEE||isnan(x)||isnan(y)) return z;
        if(x==0.0&&y==0.0) {
                return __kernel_standard(y,x,3); /* atan2(+-0,+-0) */
        } else
            return z;
#endif
}

#endif // ifdef CYGPKG_LIBM     

// EOF w_atan2.c
