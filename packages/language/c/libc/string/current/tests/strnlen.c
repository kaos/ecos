//=================================================================
//
//        strnlen.c
//
//        Testcase for C library strnlen()
//
//=================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2010 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     wry
// Contributors:  
// Date:          2010-05-10
// Description:   Testing for C library strnlen() function
//
//
//####DESCRIPTIONEND####

// INCLUDES

#include <pkgconf/isoinfra.h>
#include <string.h>
#include <stdarg.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/libc/string/stringsupp.hxx>


// FUNCTIONS

void test_guts(char *buf, size_t real_len, size_t arg, size_t expected, int *nfails, int *ncases)
{
#if 0
    // printf takes too much memory on some boards.
    //diag_printf("testcase: buf `%s', len %u, n %u, expect %u\n", buf, real_len, arg, expected);
#endif
    size_t rv = strnlen(buf, arg);
    if (rv != expected) {
        CYG_TEST_FAIL("incorrect answer from strnlen case");
#if 0
        diag_printf("incorrect answer %u from case %u:%u (expected %u)", rv, real_len, arg, expected);
#endif
        ++*nfails;
    }
    ++*ncases;
}

void test(size_t strsize, int * nfails, int * ncases)
{
    int j;
    char testbuf[1024];

    CYG_ASSERTC(strsize <= sizeof(testbuf));
    // Insist on checking the aligned-path code.
    CYG_ASSERTC(!(CYG_LIBC_STR_UNALIGNED(testbuf)));

    memset(testbuf, 'A', strsize);
    testbuf[strsize] = 0;

    for (j=1; j<(1+4*sizeof(CYG_WORD)) && j<=strsize; j++) {
        // condition `j<=strsize' prevents underflow.
        test_guts(testbuf, strsize, strsize-j, strsize-j, nfails, ncases);
    }
    for (j=0; j<(1+4*sizeof(CYG_WORD)); j++) {
        test_guts(testbuf, strsize, strsize+j, strsize, nfails, ncases);
    }
}


#if CYGINT_ISO_MAIN_STARTUP
int main( int argc, char *argv[] )
#else
void cyg_user_start(void)
#endif
{
#ifndef CYGFUN_LIBC_STRING_GNU_STRNLEN
    CYG_TEST_NA("strnlen / CYGFUN_LIBC_STRING_GNU_STRNLEN disabled");
#else

    CYG_TEST_INIT();
    CYG_TEST_INFO("strnlen exhaustive testing");
#if defined(CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST)
    CYG_TEST_INFO("strnlen: NOTE: CYGIMP_LIBC_STRING_PREFER_SMALL_TO_FAST is defined");
#endif

    int t, nfails=0, ncases=0;

#define DO(x) do { test(x, &nfails, &ncases); } while(0)
    for (t=0; t<40; t++) DO(t);

#if 0
    diag_printf("strnlen: %u/%u cases passed", (ncases-nfails), ncases);
#endif
    if (!nfails) CYG_TEST_PASS("strnlen");
    CYG_TEST_FINISH("strnlen");

#endif
} // main()


// EOF strlen.c
