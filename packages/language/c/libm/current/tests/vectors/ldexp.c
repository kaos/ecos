//===========================================================================
//
//      ldexp.c
//
//      Test of ldexp() math library function
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
// Description: 
// Usage:       
//
//####DESCRIPTIONEND####
//
//===========================================================================

// Declarations for test system:
//
// TESTCASE_TYPE=CYG_TEST_MODULE


// CONFIGURATION

#include <pkgconf/libm.h>   // Configuration header


// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/testcase.h>    // Test infrastructure
#include <math.h>                  // Header for this package
#include <sys/ieeefp.h>            // Cyg_libm_ieee_double_shape_type
#include "vectors/vector_support.h"// extra support for math tests

#include "vectors/ldexp.h"

// FUNCTIONS

static void
test( CYG_ADDRWORD data )
{
    cyg_ucount32 vec_size;
    cyg_bool ret;

    vec_size = sizeof(ldexp_vec) / sizeof(Cyg_libm_test_double_vec_t);
    ret = doTestVec( (CYG_ADDRWORD) &ldexp, CYG_LIBM_TEST_VEC_DOUBLE,
                     CYG_LIBM_TEST_VEC_INT, CYG_LIBM_TEST_VEC_DOUBLE,
                     &ldexp_vec[0], vec_size );

    if (ret==true)
    {
        CYG_TEST_PASS("ldexp() is stable");
    } // if
    else
    {
        CYG_TEST_FAIL("ldexp() failed tests");
    } // else

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for Math "
                    "library ldexp() function");
} // test()


int
main(int argc, char *argv[])
{
    CYG_TEST_INIT();

    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for Math library "
                  "ldexp() function");

    START_TEST( test );

    CYG_TEST_PASS_FINISH("Testing is not applicable to this configuration");

} // main()


// EOF ldexp.c
