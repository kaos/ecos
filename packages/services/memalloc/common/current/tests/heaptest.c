//=================================================================
//
//        heaptest.cxx
//
//        Test all the memory used by heaps to check it's all valid
//
//=================================================================
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2001-07-17
// Description:   Tests all memory allocated for use by heaps.
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/system.h>
#include <pkgconf/hal.h>
#include <pkgconf/memalloc.h> // config header
#ifdef CYGPKG_ISOINFRA
# include <pkgconf/isoinfra.h>
# include <stdlib.h>
#endif
#include <cyg/infra/testcase.h>

#if !defined(CYGPKG_ISOINFRA)
# define NA_MSG "Requires isoinfra package"
#elif !CYGINT_ISO_MALLOC
# define NA_MSG "Requires malloc"
#elif !CYGINT_ISO_MALLINFO
# define NA_MSG "Requires mallinfo"
#endif

#ifdef NA_MSG

externC void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
    CYG_TEST_FINISH("Done");
}
#else

#include <cyg/infra/diag.h>

#define ERRORTHRESHOLD 10
#define ITERS (cyg_test_is_simulator ? 1 : 10)
#define INTALIGNED(_x_) (!((unsigned long)(_x_) & (sizeof(int)-1)))

int
test_pat(unsigned char *buf, int size,
         unsigned int pat, cyg_bool addrpat,
         const char *testname)
{
    unsigned char *bufptr=buf;
    register unsigned int *ibufptr;
    unsigned char *endptr=buf+size;
    register unsigned int *endptra; // int aligned
    int errors=0;
    unsigned char bpat = pat & 0xFF;

    endptra = (int *)((unsigned long)endptr & ~(sizeof(int)-1));
    
    // Set to the pattern
    while (!INTALIGNED(bufptr)) {
        if (addrpat)
            bpat = ((int)bufptr)&0xFF;
        *bufptr++ = bpat;
    }

    ibufptr = (unsigned int *)bufptr;
    
    while ( ibufptr < endptra ) {
        if (addrpat)
            pat = (unsigned int)ibufptr;
        *ibufptr++ = pat;
    }

    bufptr = (unsigned char *)ibufptr;
    while ( bufptr < endptr ) {
        if (addrpat)
            bpat = ((int)bufptr)&0xFF;
        *bufptr++ = bpat;
    }

    // Now compare to the pattern
    bufptr = buf;
    while ( !INTALIGNED(bufptr) ) {
        if (addrpat)
            bpat = ((int)bufptr)&0xFF;
        if ( *bufptr != bpat ) {
            diag_printf( "Failed to compare at 0x%08x\n", bufptr );
            if ( errors++ == ERRORTHRESHOLD )
                CYG_TEST_FAIL_FINISH( testname );
        }
        bufptr++;
    }

    ibufptr = (unsigned int *)bufptr;
    
    while ( ibufptr < endptra ) {
        if (addrpat)
            pat = (unsigned int)ibufptr;
        if ( *ibufptr != pat ) {
            diag_printf( "Failed to compare at 0x%08x\n", bufptr );
            if ( errors++ == ERRORTHRESHOLD )
                CYG_TEST_FAIL_FINISH( testname );
        }
        ibufptr++;
    }

    bufptr = (unsigned char *)ibufptr;
    while ( bufptr < endptr ) {
        if (addrpat)
            bpat = ((int)bufptr)&0xFF;
        if ( *bufptr != bpat ) {
            diag_printf( "Failed to compare at 0x%08x\n", bufptr );
            if ( errors++ == ERRORTHRESHOLD )
                CYG_TEST_FAIL_FINISH( testname );
        }
        bufptr++;
    }
    if (errors)
        CYG_TEST_FAIL( testname );
    else
        CYG_TEST_PASS( testname );
    return errors;
} // test_pat()

externC void
cyg_start(void)
{
    unsigned int allonesint=0, checkerboardint1=0, checkerboardint2=0;
    int i;
    int errors=0;
    
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    CYG_TEST_INIT();
    CYG_TEST_INFO("Starting heaptest");

    for (i=0; i<sizeof(int); i++) {
        allonesint = allonesint << 8;
        allonesint |= 0xFF;
        checkerboardint1 = checkerboardint1 << 8;
        checkerboardint1 |= 0xAA;
        checkerboardint2 = checkerboardint2 << 8;
        checkerboardint2 |= 0x55;
    }

    for (;;) {
        struct mallinfo info;
        char *buf;
        
        info = mallinfo();

        if ( info.maxfree <= 0 )
            break;

        buf = malloc(info.maxfree);
        if (!buf) {
            diag_printf("Couldn't malloc %d bytes claimed as available",
                        info.maxfree);
            CYG_TEST_FAIL_FINISH("heaptest");
        }

        diag_printf( "INFO:<Testing memory at 0x%08x of size %d>\n",
                     buf, info.maxfree );
        for (i=0; i<ITERS; i++) {
            errors += test_pat( buf, info.maxfree, 0, 0, "all zeroes" );
            errors += test_pat( buf, info.maxfree, allonesint, 0,
                                "all ones" );
            errors += test_pat( buf, info.maxfree, checkerboardint1, 0,
                                "checkerboard 1" );
            errors += test_pat( buf, info.maxfree, checkerboardint2, 0,
                                "checkerboard 2" );
            errors += test_pat( buf, info.maxfree, 0, 1,
                                "memory addr" );
        }

        // deliberately don't free so we get the next space
    }

    if (errors)
        CYG_TEST_FAIL_FINISH( "heaptest errors found" );
    else
        CYG_TEST_PASS_FINISH( "heaptest OK" );
} // cyg_start()

#endif // !NA_MSG

// EOF heaptest.cxx
