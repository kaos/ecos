//=================================================================
//
//        diag_sprintf1.c
//
//        Testcase for infra diag_sprintf()
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
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy, jlarmour
// Contributors:  andrew lunn
// Date:          2005-02-09
// Description:   Contains testcode for infra diag_sprintf() function
//
//
//####DESCRIPTIONEND####


// INCLUDES

#include <cyg/infra/diag.h>
#include <cyg/infra/testcase.h>

// FUNCTIONS

// Functions to avoid having to use libc strings

static int my_strlen(const char *s)
{
    const char *ptr;

    ptr = s;
    for ( ptr=s ; *ptr != '\0' ; ptr++ )
        ;

    return (int)(ptr-s);
} // my_strlen()


static int my_strcmp(const char *s1, const char *s2)
{
    for ( ; *s1 == *s2 ; s1++,s2++ )
    {
        if ( *s1 == '\0' )
            break;
    } // for

    return (*s1 - *s2);
} // my_strcmp()


static char *my_strcpy(char *s1, const char *s2)
{
    while (*s2 != '\0') {
        *(s1++) = *(s2++);
    }
    *s1 = '\0';

    return s1; 
} // my_strcpy()



static void test(CYG_ADDRWORD data)
{
    static char x[500];
    static char y[500];
    int ret;
    int tmp;
    int *ptr;


    // Check 1
    ret = diag_sprintf(x, "%d", 20);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "20")==0, "%d test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%d test return code");

    // Check 2
    my_strcpy(y, "Pigs noses. Get 'em while there 'ot");
    ret = diag_sprintf(x, "%s", y);
    CYG_TEST_PASS_FAIL(my_strcmp(x, y)==0, "%s test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%s test return code");

    // Check 3
    ret = diag_sprintf(x, "||%7d||", 2378);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "||   2378||")==0, "padding test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "padding test return code");

    // Check 4
    ret = diag_sprintf(x, "%x", 3573);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "df5")==0, "hex conversion (lowercase)");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "hex conv (lowercase) return code");

    // Check 5
    ret = diag_sprintf(x, "%X", 3573);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "DF5")==0, "hex conversion (uppercase)");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "hex conv (upperbase ) return code");

    // Check 6
    ret = diag_sprintf(x, "%c", 65);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "A")==0, "%c test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%c test return code");

    // Check 7
    // Check 7 was for octal, but diag_sprintf does not support this.

    // Check 8
    ret = diag_sprintf(x, "%u", (unsigned int) 4738);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "4738")==0, "%u test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%u test return code");

    // Check 9
    // Check 9 was for the %n conversion specifier which diag_sprintf
    // does not support.

    // Check 10
    ret = diag_sprintf(x, "%%");
    CYG_TEST_PASS_FAIL(my_strcmp(x, "%")==0, "%% test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%% test return code");

    // Check 11
    ret = diag_sprintf(x, "%ld", (long)1<<30);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "1073741824")==0, "%ld test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%ld test return code");

    // Check 12
    ret = diag_sprintf(x, "%lu", (unsigned long)(1<<31) + 100);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "2147483748")==0, "%lu test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%lu test return code");

    // Check 13
    ret = diag_sprintf(x, "%x", 0x789a);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "789a")==0, "%x test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%x test return code");

    // Check 14
    ret = diag_sprintf(x, "%X", 0x789ab2);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "789AB2")==0, "%X test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%x test return code");

    // Check 15
    ret = diag_sprintf(x, "%08x", 0xdea2f2);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "00dea2f2")==0, "%0x test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%0x test return code");

    // Check 16
    ret = diag_sprintf(x, "%09X", 0x12fa1c);
    CYG_TEST_PASS_FAIL(my_strcmp(x, "00012FA1C")==0, "%0X test");
    CYG_TEST_PASS_FAIL(ret==my_strlen(x), "%0X test return code");

    // Check 17
    ptr=&tmp;
    ret = diag_sprintf(x, "%p", (void *)ptr);
    // just check _something_ was returned
    CYG_TEST_PASS_FAIL((ret==my_strlen(x)) && (ret > 0),
                       "%p test return code");

    // Long string tests
    ret = diag_sprintf(x, "This is a very long string so I hope this works as "
                  "otherwise I would be very, very, sad. The cat sat on the "
                  "hat, and the mat sat on the rat. Quick brown fax, etc.etc. "
                  "blah, blah and all that jazz. Isn't he finished yet? My "
                  "old man's a dustman, why do I have to think up this "
                  "drivel, isn't that what summer students are for, if "
                  "anything that seems thinking up mindless drivel seems to "
                  "be their occupation in life. Yoof of today, eh? What, "
                  "what? %s So there.",
                  "And this is a middly bit.");
    my_strcpy(y, "This is a very long string so I hope this works as "
                  "otherwise I would be very, very, sad. The cat sat on the "
                  "hat, and the mat sat on the rat. Quick brown fax, etc.etc. "
                  "blah, blah and all that jazz. Isn't he finished yet? My "
                  "old man's a dustman, why do I have to think up this "
                  "drivel, isn't that what summer students are for, if "
                  "anything that seems thinking up mindless drivel seems to "
                  "be their occupation in life. Yoof of today, eh? What, "
                  "what? And this is a middly bit. So there.");
    CYG_TEST_PASS_FAIL(my_strcmp(x, y)==0, "long (480 char) string output #1");
    CYG_TEST_PASS_FAIL(ret == my_strlen(y),
                       "long (480 char) string output #1 return code");

    ret = diag_sprintf(x, "Boo! This %s So there.",
                  "is a very long string so I hope this works as "
                  "otherwise I would be very, very, sad. The cat sat on the "
                  "hat, and the mat sat on the rat. Quick brown fax, etc.etc. "
                  "blah, blah and all that jazz. Isn't he finished yet? My "
                  "old man's a dustman, why do I have to think up this "
                  "drivel, isn't that what summer students are for, if "
                  "anything that seems thinking up mindless drivel seems to "
                  "be their occupation in life. Yoof of today, eh? What, "
                  "what? And this is a middly bit.");
    my_strcpy(y, "Boo! This is a very long string so I hope this works as "
                  "otherwise I would be very, very, sad. The cat sat on the "
                  "hat, and the mat sat on the rat. Quick brown fax, etc.etc. "
                  "blah, blah and all that jazz. Isn't he finished yet? My "
                  "old man's a dustman, why do I have to think up this "
                  "drivel, isn't that what summer students are for, if "
                  "anything that seems thinking up mindless drivel seems to "
                  "be their occupation in life. Yoof of today, eh? What, "
                  "what? And this is a middly bit. So there.");
    CYG_TEST_PASS_FAIL(my_strcmp(x, y)==0, "long (485 char) string output #2");
    CYG_TEST_PASS_FAIL(ret == my_strlen(y),
                       "long (485 char) string output #2 return code");




    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "diag_sprintf() function");

} // test()

int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for infra library "
                  "diag_sprintf() function");
    CYG_TEST_INFO("These test individual features separately");

    test(0);

    return 0;
} // main()

// EOF diag_sprintf1.c
