//=================================================================
//
//        realloc.c
//
//        Testcase for C library realloc()
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  
// Date:          2000-04-30
// Description:   Contains testcode for C library realloc() function
//
//
//####DESCRIPTIONEND####


// INCLUDES

#include <pkgconf/system.h>             // Overall system configuration
#include <pkgconf/memalloc.h>           // config header
#ifdef CYGPKG_ISOINFRA
# include <pkgconf/isoinfra.h>
# include <stdlib.h>
#endif
#include <cyg/infra/testcase.h>

#if !defined(CYGPKG_ISOINFRA)
# define NA_MSG "Requires isoinfra package"
#elif !CYGINT_ISO_MAIN_STARTUP
# define NA_MSG "Requires main() to be called"
#elif !CYGINT_ISO_MALLOC
# define NA_MSG "Requires malloc"
#elif !CYGINT_ISO_MALLINFO
# define NA_MSG "Requires mallinfo"
#endif

#ifdef NA_MSG
void
cyg_start()
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
    CYG_TEST_FINISH("Done");
}
#else


// FUNCTIONS

static const char alphabet[]="abcdefghijklmnopqrstuvwxyz{-}[]#';:@~!$^&*()"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

extern int
cyg_memalloc_maxalloc( void );

static int
compare_with_alphabet( char *buf, int size, int offset )
{
    int i, buf_offset;

    for (i=offset, buf_offset=0;
         buf_offset < size;
         buf_offset++,i++ ) {

        if ( i==sizeof(alphabet)-1 )
            i=0;

        if ( buf[buf_offset] != alphabet[i] ) {
            CYG_TEST_FAIL( "buffer has not retained correct data!");
            return 0; // fail
        } // if
    } // for

    return 1; // success
} // compare_with_alphabet()

static int
fill_with_alphabet( char *buf, int size, int offset )
{
    int i, buf_offset;

    for (i=offset, buf_offset=0;
         buf_offset < size;
         buf_offset++,i++ ) {

        if ( i==sizeof(alphabet)-1 )
            i=0;

        buf[buf_offset] = alphabet[i];

    } // for

    return compare_with_alphabet( buf, size, offset); // be sure
} // fill_with_alphabet()


int
main( int argc, char *argv[] )
{
    char *str;
    int size;
    int poolmax;

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "realloc() function");

    poolmax = mallinfo().maxfree;
    
    if ( poolmax <= 0 ) {
        CYG_TEST_FAIL_FINISH( "Can't determine allocation size to use" );
    }

    size = poolmax/2;

    str = (char *)realloc( NULL, size );
    CYG_TEST_PASS_FAIL( str != NULL, "realloc doing only allocation");
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size, 0 ),
                        "allocation usability");

    str = (char *)realloc( str, 0 );
    CYG_TEST_PASS_FAIL( str == NULL, "realloc doing implicit free" );

    str = (char *)realloc( NULL, size/2 );
    CYG_TEST_PASS_FAIL( str != NULL, "realloc doing allocation to half size");
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size/2, 5 ),
                        "half allocation usability");

    str = (char *)realloc( str, size );
    CYG_TEST_PASS_FAIL( str != NULL,
                        "reallocing allocation back to normal size");
    CYG_TEST_PASS_FAIL( compare_with_alphabet(str, size/2, 5),
                        "after realloc to normal size, old contents kept" );
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size, 3 ),
                        "reallocation normal size usability");

    str = (char *)realloc( str, size/4 );
    CYG_TEST_PASS_FAIL( str != NULL, "reallocing allocation to quarter size");
    CYG_TEST_PASS_FAIL( compare_with_alphabet(str, size/4, 3),
                        "after realloc to quarter size, old contents kept" );
    CYG_TEST_PASS_FAIL( fill_with_alphabet( str, size/4, 1 ),
                        "reallocation quarter size usability");

    CYG_TEST_PASS_FAIL( realloc( str, size*4 ) == NULL,
                        "reallocing allocation that is too large" );
    CYG_TEST_PASS_FAIL( compare_with_alphabet( str, size/4, 1 ),
                        "Checking old contents maintained despite failure" );

    str = (char *)realloc( str, 0 );
    CYG_TEST_PASS_FAIL( str == NULL, "realloc doing implicit free again" );

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "realloc() function");

    return 0;
} // main()

#endif // ifndef NA_MSG

// EOF realloc.c
