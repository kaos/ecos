//=================================================================
//
//        diag_sprintf2.c
//
//        Testcase for infra library diag_sprintf()
//
//=================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):       ctarpy, jlarmour
// Contributors:    andrew lunn
// Date:            2005-02-09
// Description:     Contains testcode for infra library diag_sprintf() function
//
//
//####DESCRIPTIONEND####

// CONFIGURATION

// INCLUDES

#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

// FUNCTIONS

// Functions to avoid having to use libc strings

static int
my_strlen(const char *s)
{
    const char *ptr;

    ptr = s;
    for ( ptr=s ; *ptr != '\0' ; ptr++ )
        ;

    return (int)(ptr-s);
} // my_strlen()


static int
my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


static char *
my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()



static void
test( CYG_ADDRWORD data )
{
    static char x[200];
    static char y[200];
    static char z[200];
    int ret;

    // Check 1
    my_strcpy(x, "I'm afraid the shield generator");
    ret = diag_sprintf(y, "%s will be quite operational - %5d%%%c%05X", x,
                   13, '5', 0x89ab);
    my_strcpy( z, "I'm afraid the shield generator will be "
                  "quite operational -    13%5089AB" );
    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "%s%n%d%%%c%0X test");

    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "%s%n%d%%%c%0X test return code" );

    // Check 2
    ret = diag_sprintf(y, "|%5d|%10s|%03d|%c|", 2, "times", 6, '=');
    my_strcpy(z, "|    2|     times|006|=|");

    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "|%5d|%10s|%03d|%c| test");

    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "|%5d|%10s|%03d|%c|%o| test return code" );

    // Check 3
    ret = diag_snprintf(y, 19, "print up to here >< and not this bit" );
    my_strcpy(z, "print up to here >");
    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "simple diag_snprintf test #1");
    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "simple diag_snprintf test #1 return code" );
    
    // Check 4
    ret = diag_snprintf(y, 31, "print a bit of this number: %05d nyer", 1234);
    my_strcpy(z, "print a bit of this number: 01");
    CYG_TEST_PASS_FAIL(my_strcmp(y,z) == 0, "simple diag_snprintf test #2");
    CYG_TEST_PASS_FAIL(ret == my_strlen(z),
                       "simple diag_snprintf test #2 return code" );
    
    CYG_TEST_FINISH("Finished tests from testcase " __FILE__
                    " for C library diag_sprintf() function");

} // test()

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for infra "
                  "library diag_sprintf() function");
    CYG_TEST_INFO("These test combinations of diag_sprintf() features");

    test(0);

    return 0;
} // main()

// EOF diag_sprintf2.c
