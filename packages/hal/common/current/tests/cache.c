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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
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

// -------------------------------------------------------------------------

#define MAXSIZE 1<<18

volatile char m[MAXSIZE];

// -------------------------------------------------------------------------

static void time0(register cyg_uint32 stride)
{
    register cyg_uint32 j,k;
//    cyg_tick_count_t count0, count1;
//    cyg_ucount32 t;
    register char c;

//    count0 = current_time();

    for(k=0; k<4000;k++) {
        for(j=0; j<(HAL_DCACHE_SIZE/HAL_DCACHE_LINE_SIZE); j++) {
            c=m[stride*j];
        }
    }

//    count1 = current_time();
//    t = count1 - count0;
//    diag_printf("stride=%d, time=%d\n", stride, t);
}

// -------------------------------------------------------------------------

void time1(void)
{
    cyg_uint32 i;

    for(i=1; i<65; i+=i) {
        time0(i);
    }
}

// -------------------------------------------------------------------------
// This test could be improved by counting number of passes possible 
// in a given number of ticks.

static void entry0( void )
{
    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    CYG_TEST_INFO("Dcache off Icache off");
    time1();

    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_ENABLE();
    CYG_TEST_INFO("Dcache on  Icache off");
    time1();

    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_DISABLE();
    CYG_TEST_INFO("Dcache off Icache on");
    time1();

    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_ENABLE();
    CYG_TEST_INFO("Dcache on Icache on");
    time1();


    HAL_ICACHE_INVALIDATE_ALL();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_DISABLE();
    CYG_TEST_INFO("Dcache off Icache off");
    time1();

    
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
