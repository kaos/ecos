/*=================================================================
//
//        cache.c
//
//        HAL Cache timing test
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
// Author(s):     dsm
// Contributors:    dsm, nickg
// Date:          1998-06-18
//####DESCRIPTIONEND####
*/

#include <pkgconf/hal.h>

#include <cyg/infra/testcase.h>

#include <cyg/hal/hal_cache.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/infra/diag.h>

// -------------------------------------------------------------------------
// If the HAL does not supply this, we supply our own version

#ifdef HAL_DCACHE_SYNC

# define HAL_DCACHE_PURGE_ALL() HAL_DCACHE_SYNC()

#else

static cyg_uint8 dca[HAL_DCACHE_SIZE + HAL_DCACHE_LINE_SIZE*2];

#define HAL_DCACHE_PURGE_ALL()                                          \
CYG_MACRO_START                                                         \
    volatile cyg_uint8 *addr = &dca[HAL_DCACHE_LINE_SIZE];              \
    volatile cyg_uint8 tmp = 0;                                         \
    int i;                                                              \
    for( i = 0; i < HAL_DCACHE_SIZE; i += HAL_DCACHE_LINE_SIZE )        \
    {                                                                   \
        tmp = addr[i];                                                  \
    }                                                                   \
CYG_MACRO_END

#endif

// -------------------------------------------------------------------------

#define MAXSIZE 1<<18

volatile char m[MAXSIZE];

#ifndef MAX_STRIDE
#define MAX_STRIDE 64
#endif

// -------------------------------------------------------------------------

static void time0(register cyg_uint32 stride)
{
    register cyg_uint32 j,k;
    register char c;

    diag_printf("stride=%d\n", stride);

    k = 0;
    if ( cyg_test_is_simulator )
        k = 3960;

    for(; k<4000;k++) {
        for(j=0; j<(HAL_DCACHE_SIZE/HAL_DCACHE_LINE_SIZE); j++) {
            c=m[stride*j];
        }
    }
}

// -------------------------------------------------------------------------

void time1(void)
{
    cyg_uint32 i;

    for(i=1; i<=MAX_STRIDE; i+=i) {
        time0(i);
    }
}

// -------------------------------------------------------------------------
// This test could be improved by counting number of passes possible 
// in a given number of ticks.

static void entry0( void )
{
    register CYG_INTERRUPT_STATE oldints;

#ifdef HAL_CACHE_UNIFIED

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();             // rely on above definition
    HAL_UCACHE_INVALIDATE_ALL();
    HAL_UCACHE_DISABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Cache off");
    time1();

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();             // rely on above definition
    HAL_UCACHE_INVALIDATE_ALL();
    HAL_UCACHE_ENABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Cache on");
    time1();

#else // HAL_CACHE_UNIFIED

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Dcache off Icache off");
    time1();

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_ENABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Dcache on  Icache off");
    time1();

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_DISABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Dcache off Icache on");
    time1();

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_ENABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Dcache on  Icache on");
    time1();

    HAL_DISABLE_INTERRUPTS(oldints);
    HAL_DCACHE_PURGE_ALL();
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    HAL_RESTORE_INTERRUPTS(oldints);
    CYG_TEST_INFO("Dcache off Icache off");
    time1();

#endif // HAL_CACHE_UNIFIED

    CYG_TEST_PASS_FINISH("End of test");
}

// -------------------------------------------------------------------------

void cache_main( void )
{
    CYG_TEST_INIT();

    entry0();

}

// -------------------------------------------------------------------------

externC void
cyg_start( void )
{
    cache_main();
}

// -------------------------------------------------------------------------
/* EOF cache.c */
