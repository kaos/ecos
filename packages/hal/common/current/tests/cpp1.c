/*=================================================================
//
//        cpp1.c
//
//        cpp arithmetic bug regression test
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
// Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     hmt
// Contributors:  hmt
// Date:          2001-04-30
//####DESCRIPTIONEND####
*/

#include <pkgconf/hal.h>

#include <pkgconf/infra.h>

#include <cyg/infra/testcase.h>

// -----------------------------------------------------------------
// This is smaller than 2048.
// Unless the parser binds '+' too strongly because it is after a ket, so
// it is mistaken for unary plus, when (4 * 20 + 2) * 4 * 20 is larger.
#define CYGNUM_HAL_STACK_SIZE_MINIMUM ((4 * 20) + 2 * 4 * 20)

#define CYGNUM_UITRON_STACK_SIZE (2048)

#ifdef CYGNUM_HAL_STACK_SIZE_MINIMUM
# ifdef CYGNUM_UITRON_STACK_SIZE
#  if CYGNUM_UITRON_STACK_SIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM

// then override the configured stack size
#   undef CYGNUM_UITRON_STACK_SIZE
#   define CYGNUM_UITRON_STACK_SIZE CYGNUM_HAL_STACK_SIZE_MINIMUM

#  endif // CYGNUM_UITRON_STACK_SIZE < CYGNUM_HAL_STACK_SIZE_MINIMUM
# endif // CYGNUM_UITRON_STACK_SIZE
#endif // CYGNUM_HAL_STACK_SIZE_MINIMUM


// This tests for the bug per se:
int i = CYGNUM_UITRON_STACK_SIZE;

// This tests the workaround independently of more complex context:
#define MAX(_x_,_y_) ((_x_) > (_y_) ? (_x_) : (_y_))

static char stack1[
    MAX(
        CYGNUM_HAL_STACK_SIZE_MINIMUM,
        2048)
    ];

// Better to report a fully-fledged failure and test the workaround than
// fail early.
#if 0
# if CYGNUM_UITRON_STACK_SIZE != 2048
#  error FAIL: CPP '+' binding bug detected
# endif
#endif

// -------------------------------------------------------------------------
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS("CPP '+' binding test compiled OK");
    CYG_TEST_PASS_FAIL( 2048 == i, "initialized i should be 2048" );
    CYG_TEST_PASS_FAIL( 2048 == sizeof( stack1 ),
                        "workaround: sizeof( stack1 ) should be 2048" );
    CYG_TEST_EXIT("All done");
}

// -------------------------------------------------------------------------
/* EOF cpp1.c */
