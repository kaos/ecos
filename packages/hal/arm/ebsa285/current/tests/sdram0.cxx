//==========================================================================
//
//        sdram0.cxx
//
//        SDRAM function test 0
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
// Author(s):     hmt
// Contributors:  hmt
// Date:          1999-11-11
// Description:   Basic memory test, knowledgeable of the EBSA285's
//                memory size.
//####DESCRIPTIONEND####

#include <cyg/infra/testcase.h>

#include <cyg/infra/diag.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#define ONE_MEG (0x100000)
#define ONE_MEG_IN_WORDS (ONE_MEG/4)

#define MEGS 9
#define WORDS ONE_MEG_IN_WORDS
#define START 0x400000

#define INNERLOOPS 10

#define NUMTESTS 1

#if WORDS > ONE_MEG_IN_WORDS
# error "Too many WORDS in a block - they'll overlap!"
#endif

#include <cyg/hal/hal_ebsa285.h>


void
check_addrsize_setup( void )
{
    cyg_uint32 sizes[4] = { 0, };
    cyg_uint32 bases[4] = { 0, };
    cyg_uint32 codes[4] = { 0, };
    cyg_uint32 muxes[4] = { 0, };
    cyg_uint32 i;

#define MBytes <<20

    static cyg_uint32 lookup[] =
    { 0, 1 MBytes, 2 MBytes, 4 MBytes, 8 MBytes, 16 MBytes, 32 MBytes, 64 MBytes };

    static cyg_uint32 maxsizes[] =
    { 2 MBytes, 16 MBytes, 64 MBytes, 8 MBytes, 64 MBytes,  0, 0, 0 };

    codes[0] = *SA110_SDRAM_ADDRESS_SIZE_ARRAY_0;
    codes[1] = *SA110_SDRAM_ADDRESS_SIZE_ARRAY_1;
    codes[2] = *SA110_SDRAM_ADDRESS_SIZE_ARRAY_2;
    codes[3] = *SA110_SDRAM_ADDRESS_SIZE_ARRAY_3;

    // Print all the info for the benefit of humans:
    for ( i = 0; i < 4; i++ ) {
        bases[i] = 0x0ff00000 & codes[i];
        sizes[i] = lookup[ 7 & codes[i] ];
        muxes[i] = 7 & (codes[i] >> 4);
        diag_printf( "Bank %d: [%08x]: base %08x, size %08x; mux mode %d\n",
                     i, codes[i], bases[i] , sizes[i] , muxes[i] );
    }
    // THEN check individual entries for sanity
    for ( i = 0; i < 4; i++ ) {
        if ( 0 == sizes[i] ) {
            // then the bank is not in use
            CYG_TEST_CHECK( 0 == bases[i], "Unused bank nonzero address" );
            CYG_TEST_CHECK( 0 == muxes[i], "Unused bank nonzero mux mode" );
        } else {
            CYG_TEST_CHECK( muxes[i] <= 4, "Mux mode overflow" );
            if ( (muxes[i] == 3) && (8 != sizes[i]) )
                CYG_TEST_FAIL( "Mux mode 3 and size not 8Mb" );
            CYG_TEST_CHECK( maxsizes[ muxes[i] ] >= sizes[i],
                            "Size too larget for mux mode" );
        }
    }

    // NEXT check that addresses are singly mapped IYSWIM:
    // Easiest way is, foreach megabyte, check it is mapped exactly once;
    // shouldn't take too long.
    for ( i = 0; i < hal_dram_size; i += ONE_MEG ) {
        int j = 0;
        int k;
        for ( k = 0; k < 4; k++ )
            // this test works OK for an unused slot because i is +ve:
            if ( (bases[k] <= i) && (i < (bases[k] + sizes[k])) )
                j++;
        CYG_TEST_CHECK( 2 > j, "Good memory is multiply mapped" );
        CYG_TEST_CHECK( 0 < j, "Good memory is not mapped" );
    }
    for ( /* i */ ; i < 256 MBytes; i += ONE_MEG ) {
        int j = 0;
        int k;
        for ( k = 0; k < 4; k++ )
            // this test works OK for an unused slot because i is +ve:
            if ( (bases[k] <= i) && (i < (bases[k] + sizes[k])) )
                j++;
        CYG_TEST_CHECK( 2 > j,  "Non-existent memory is multiply mapped" );
        CYG_TEST_CHECK( 0 == j, "Non-existent memory is mapped" );
    }
    CYG_TEST_PASS( "Memory controller setup self-consistent" );
}


void mymain( void )
{
    cyg_uint32 h, i, j, k;
    cyg_uint32 *pbase[ MEGS ] = { 0, };
    int totaltests = 0;
    int ptotalerrors[ MEGS ] = { 0, };

    CYG_TEST_INIT();


    check_addrsize_setup();


    h = hal_dram_size - ONE_MEG;
    i = MEGS - 1;
    j = START;
    k = 0;
    while ( (i > k) && (h > j) ) {
        pbase[i] = (cyg_uint32 *)h; 
        pbase[k] = (cyg_uint32 *)j;
        i--;
        k++;
        h -= ONE_MEG;
        j += ONE_MEG;
    }
    if ( (i == k) && (h > j) )
        pbase[ i ] = (cyg_uint32 *)((h+j)/2);

    for ( h = 0; h < NUMTESTS; h++ ) { 
        int perrors[ MEGS ] = { 0, };
        cyg_uint32 pbadbits[ MEGS ] = { 0, };
        for ( i = 0 ; i < INNERLOOPS; i++ ) {
            cyg_uint32 d = 0xdeadbeef ^ ((cyg_uint32)i * 0x10001);
            for ( k = 0; k < MEGS; k++ ) {
                cyg_uint32 *p = pbase[k];
                cyg_uint32 dp = d ^ (cyg_uint32)p;
                if ( ! p ) continue;
                for ( j = 0; j < WORDS; j++ )
                    p[j] = dp ^ j ^ (j << 19) ;
            }
            for ( k = 0; k < MEGS; k++ ) {
                cyg_uint32 *p = pbase[k];
                cyg_uint32 dp = d ^ (cyg_uint32)p;
                if ( ! p ) continue;
                for ( j = 0; j < WORDS; j++ )
                    if ( p[j] != (dp ^ j ^ (j << 19)) ) {
                        perrors[k]++;
                        pbadbits[k] |= (p[j] ^ dp ^ j ^ (j << 19));
                    }
            }
        }
        totaltests += i * j;
        for ( k = 0; k < MEGS; k++ ) {
            if ( ! pbase[k] ) continue;
            ptotalerrors[k] += perrors[k];
            diag_printf(
"p %x: %d tests of %d words: %d errors, badbits %x ...totals %d tests %d errors\n",
 pbase[k], i,      j,      perrors[k], pbadbits[k],  totaltests, ptotalerrors[k] );
            if ( 0 != perrors[k] )
                CYG_TEST_FAIL( "Errors in memory test" );
        }
    }

    h = j = 0;
    for ( k = 0; k < MEGS; k++ ) {
        if ( ! pbase[k] ) continue;
        h += ptotalerrors[k] ;
        j += totaltests;
    }
    diag_printf( "Total tests %d, total errors %d\n", j, h );
    if ( 0 == h )
        CYG_TEST_PASS( "Memory test all OK" );

    CYG_TEST_EXIT("End of mem test");
    
}

externC void
cyg_start( void )
{ 
    HAL_ENABLE_INTERRUPTS();

#ifdef CYGPKG_HAL_ARM_EBSA285
    cyg_uint32 i;
    i = *(cyg_uint32 *)(0x42000000 + 0x10c);
    diag_printf( "SDRAM timing %08x\n", i );
    for ( i = 0; i < 4; i++ ) {
        diag_printf( "Bank %d: addrsize %08x\n", i, *(cyg_uint32 *)(0x42000000 + 0x110 + i * 4 ));
    }
    diag_printf( "Mem size: %08x == %d\n", hal_dram_size, hal_dram_size );
#endif

    mymain();
}
// EOF sdram0.cxx
