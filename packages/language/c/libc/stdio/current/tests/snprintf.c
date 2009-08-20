//=================================================================
//
//        snprintf.c
//
//        Testcase for C library snprintf() implementation
//
//=================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2009 Free Software Foundation, Inc.
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
// Author(s):       Uwe Kindler
// Contributors:
// Date:            2009-08-05
// Description:     Contains testcode for C library snprintf() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

#include <pkgconf/libc_stdio.h>   // Configuration header

// INCLUDES

#include <stdio.h>
#include <cyg/infra/testcase.h>

static int my_strnlen(const char *s, size_t maxlen)
{
    const char *ptr;
    const char *endptr;

    ptr = s;
    endptr = s + maxlen;
    while ((*ptr != '\0') && (ptr != endptr))
    {
        ptr++;
    }

    return (int)(ptr-s);
} // my_strlen()

static void
test( CYG_ADDRWORD data )
{
    static char x[32];
    static char y[4];
    int xret;
    int yret;
    int xstrlen;
    int ystrlen;

    // fill buffer to ensure that there are no zeros in the buffers
    memset(x, 0xFF, sizeof(x));
    memset(y, 0xFF, sizeof(y));
    
    // print into a buffer with sufficient size 
    xret = snprintf(x, sizeof(x), "%d:%d:%d:%d", 1, 2, 3, 4);
    xstrlen = my_strnlen(x, sizeof(x));
    
    // print into a buffer that is too small
    yret = snprintf(y, sizeof(y), "%d:%d:%d:%d", 1, 2, 3, 4);
    ystrlen = my_strnlen(y, sizeof(y));
    
    CYG_TEST_PASS_FAIL(xret == xstrlen, "[buffer > strlen] return code");
    
#ifdef CYGIMP_LIBC_STDIO_C99_SNPRINTF
    // C99 compliant implementation returns the number of characters that 
    // would have been written had size been sufficiently large, 
    // not counting the terminating nul character
    CYG_TEST_PASS_FAIL(xret == yret, "[buffer < strlen] return code");
    CYG_TEST_INFO("C99 compliant implementation of snprintf()");
#else
    // default eCos implementation returns number of bytes written into
    // the buffer without terminating nul character
    CYG_TEST_PASS_FAIL(yret == ystrlen, "[buffer < strlen] return code");
    CYG_TEST_INFO("Default implementation of snprintf() (no C99 compliance)");
#endif


    CYG_TEST_FINISH("Finished tests from testcase " __FILE__
                    " for C library snprintf() function return values");

} // test()

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library snprintf() function return values");
    CYG_TEST_INFO("These test return values of snprinf() family of functions");

    test(0);

    return 0;
} // main()

// EOF snprintf.c
