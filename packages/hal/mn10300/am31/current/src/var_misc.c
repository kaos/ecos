//==========================================================================
//
//      var_misc.c
//
//      HAL CPU variant miscellaneous functions
//
//==========================================================================
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jlarmour
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_cache.h>

/*------------------------------------------------------------------------*/
/* Variant specific initialization routine.                               */

void hal_variant_init(void)
{
}

/*------------------------------------------------------------------------*/
/* Cache functions.                                                       */

#if !defined(CYG_HAL_MN10300_SIM)
void cyg_hal_dcache_store(CYG_ADDRWORD base, int size)
{
    volatile register CYG_BYTE *way0 = HAL_DCACHE_PURGE_WAY0;
    volatile register CYG_BYTE *way1 = HAL_DCACHE_PURGE_WAY1;
    register int i;
    register CYG_ADDRWORD state;

    HAL_DCACHE_IS_ENABLED(state);
    if (state)
        HAL_DCACHE_DISABLE();

    way0 += base & 0x000007f0;
    way1 += base & 0x000007f0;
    for( i = 0; i < size; i += HAL_DCACHE_LINE_SIZE )
    {
        *(CYG_ADDRWORD *)way0 = 0;
        *(CYG_ADDRWORD *)way1 = 0;
        way0 += HAL_DCACHE_LINE_SIZE;
        way1 += HAL_DCACHE_LINE_SIZE;
    }
    if (state)
        HAL_DCACHE_ENABLE();
}
#endif


/*------------------------------------------------------------------------*/
/* End of var_misc.c                                                      */
