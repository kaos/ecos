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

#if !defined(CYG_HAL_MN10300_AM31_SIM)
void cyg_hal_dcache_store(CYG_ADDRWORD base, int size)
{
#if 0    
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
#else

    // Despite several people trying to understand it, the above code
    // still does not work correctly. Whether this is a result of the
    // AM31 hardware being faulty, or simply a problem with register
    // pressure causing the values of some variables to be lost as a
    // result of the purge we are not sure. Similar code on the AM33
    // works perfectly, but that processor has more registers to play
    // with.  The following code, taken from CygMon, works correctly,
    // and does not appear to exhibit the problems of the code above.
    // It's only drawback is that it purges the entire cache. However,
    // this is permitted under the definition of the cache macros, so
    // this is not a major concern at present.
    
    __asm__ (
              ".equ CHCTR,        0x20000070;"
              ".equ CHCTR_ICEN,   0x0001;"
              ".equ CHCTR_DCEN,   0x0002;"
              ".equ CHCTR_ICBUSY, 0x0004;"
              ".equ CHCTR_DCBUSY, 0x0008;"
              ".equ CHCTR_ICINV,  0x0010;"
              ".equ CHCTR_DCINV,  0x0020;"
              ".equ DCACHE_PURGE_WAY0_START, 0x28400000;"
              ".equ DCACHE_PURGE_WAY0_END,   0x28400800;"
              ".equ DCACHE_PURGE_WAY1_START, 0x28401000;"
              ".equ DCACHE_PURGE_WAY1_END,   0x28401800;"

              "mov CHCTR,a0;"

              /* DCACHE */

              /* first check if dcache enabled */
              "mov (a0),d0;"
              "btst CHCTR_DCEN,d0;"
              /* if not, jump to end */
              "beq 2f;"

              /* if so, we have to disable the cache */
              "and 0xfffffffd,d0;"
              "mov d0,(a0);"

              /* wait for it to stop being busy */
              "setlb;"
              "nop;"
              "mov (a0),d0;"
              "btst CHCTR_DCBUSY,d0;"
              "lne;"

              /* now purge the dcache */
              "mov DCACHE_PURGE_WAY0_START,a1;"
              "1:"
              "mov (0,a1),d0;"
              "add 0x10,a1;"
              "cmp DCACHE_PURGE_WAY0_END,a1;"
              "bne 1b;"
              "mov DCACHE_PURGE_WAY1_START,a1;"
              "1:"
              "mov (0,a1),d0;"
              "add 0x10,a1;"
              "cmp DCACHE_PURGE_WAY1_END,a1;"
              "bne 1b;"

              /* wait for it to stop being busy */
              "setlb;"
              "nop;"
              "mov (a0),d0;"
              "btst CHCTR_DCBUSY,d0;"
              "lne;"

              /* and re-enable */
              "or CHCTR_DCEN,d0;"
              "mov d0,(a0);"

              "2:"

              :
              :
              : "a0", "a1", "d0"
        );
    
#endif    
}
#endif


/*------------------------------------------------------------------------*/
/* End of var_misc.c                                                      */
