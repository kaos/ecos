//===========================================================================
//
//      setjmp.c
//
//      Tests for ANSI standard setjmp() and longjmp() functions
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   jlarmour
// Contributors:  jlarmour
// Date:        1998-02-13
// Purpose:     
// Description: Contains test code for C library setjmp() and longjmp()
//              functions
//              The ANSI standard allows setjmp calls to be used ONLY in the
//              forms:
//                while (setjmp (jumpbuf))
//                while (setjmp (jumpbuf) < 42)
//                while (!setjmp (jumpbuf))
//                setjmp (jumpbuf);
//                result = setjmp(jumpbuf);
//
//              Or "while" can also be "if" or "switch" or the implicit
//              while of "for". 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE
// COMPOUND_TESTCASE

// CONFIGURATION

#include <pkgconf/libc.h>   // Configuration header

// INCLUDES

#include <cyg/infra/testcase.h>    // Test infrastructure header
#include <setjmp.h>                // Header for what we're testing
#include <sys/cstartup.h>          // C library initialisation

// GLOBALS

#ifdef CYGPKG_LIBC

static jmp_buf jbuf, jbuf2, jbuf3;


// FUNCTIONS

static int
test_fun( void )
{
    volatile int i=0; // add something to the stack to confuse things

    longjmp( jbuf, 5 );
    i+=5;

    return i;
} // test_fun();

#endif // ifdef CYGPKG_LIBC

externC void
cyg_package_start( void )
{
#ifdef CYGPKG_LIBC
    cyg_iso_c_start();
#else
    (void)main(0, NULL);
#endif
} // cyg_package_start()


int
main( int argc, char *argv[] )
{
#ifdef CYGPKG_LIBC
    static int static_i=0;         // temporary variable
    int j=0;                       // ditto
    volatile int vol_k=0, vol_l=0; // ditto
#endif

    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C library "
                  "setjmp()/longjmp() functions");

#ifdef CYGPKG_LIBC
    // Test 1
    if ( setjmp(jbuf) == 0 )
    {
        static_i=1;
        longjmp(jbuf, 1);
        static_i=2;
    } // if

    CYG_TEST_PASS_FAIL( static_i==1, "Simple test 1" );

    // Test 2
    j=2;

    if ( !setjmp(jbuf) )
    {
        static_i=3;
        longjmp(jbuf, 1);
        static_i=4;
        j=3;
    } // if

    CYG_TEST_PASS_FAIL( (static_i==3) && (j==2), "Simple test 2" );

    // Test 3

    static_i=0;
    switch (setjmp(jbuf))
    {
    case 0:
        static_i++;
        if (static_i!=1)
            CYG_TEST_FAIL("Test of value passed to longjmp()");
        else
            longjmp(jbuf, 5);
        break;
    case 5:
        static_i++;
        CYG_TEST_PASS_FAIL( (static_i==2),"Test of value passed to longjmp()");
        break;
    default:
        CYG_TEST_FAIL( "Test of value passed to longjmp()");
        break;
    } // switch

    // Test 3

    static_i=0;
    switch (setjmp(jbuf))
    {
    case 0:
        static_i++;
        if (static_i!=1)
            CYG_TEST_FAIL("Test of value passed to longjmp() being 0");
        else
            longjmp(jbuf, 0);
        break;
    case 1:
        static_i++;
        CYG_TEST_PASS_FAIL( (static_i==2),
                            "Test of value passed to longjmp() being 0");
        break;
    default:
        CYG_TEST_FAIL( "Test of value passed to longjmp() being 0");
        break;
    } // switch


    // Test 4
    
    vol_k=0;
    static_i=0;
    setjmp( jbuf );

    static_i++;
    setjmp( jbuf2 );
    
    static_i++;
    setjmp( jbuf3 );

    if (vol_k==0)
    {
        vol_k++;
        longjmp( jbuf2, 1 );
    }
    else
    {
        CYG_TEST_PASS_FAIL( (vol_k==1) && (static_i==3), "Multiple setjmp's" );
    }

    // Test 5
    
    vol_k=3;
    if ( (j=setjmp(jbuf)) == 0 )
    {
        volatile int l; // put something extra on the stack to confuse things

        vol_k++;
        l=test_fun();
        l++;
        vol_k=l;
    } // if

    CYG_TEST_PASS_FAIL( (j==5) && (vol_k==4), "longjmp from a function" );

    // Test 6

    vol_k=0;

    vol_l=setjmp(jbuf);

    vol_k += 3;

    if (!vol_l)
        test_fun();

    vol_l=setjmp(jbuf);

    vol_k += 7;

    if (!vol_l)
        test_fun();

    CYG_TEST_PASS_FAIL ( (vol_k == 20), "Repeated longjmps from a function" );

#else // ifndef CYGPKG_LIBC
    CYG_TEST_NA("Testing is not applicable to this configuration");
#endif // ifndef CYGPKG_LIBC

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C library "
                    "setjmp()/longjmp() functions");

} // main()

// EOF setjmp.c
