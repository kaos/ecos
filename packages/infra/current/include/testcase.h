#ifndef CYGONCE_INFRA_TESTCASE_H
#define CYGONCE_INFRA_TESTCASE_H
//==========================================================================
//
//        testcase.h
//
//        Target side interface for tests
//
//==========================================================================
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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     ctarpy
// Contributors:    ctarpy
// Date:          1998/16/3
//
//
//####DESCRIPTIONEND####

#include <cyg/infra/cyg_type.h> // Common type definitions and support


// The values of status have the following meaning:
// 0 fail
// 1 pass
// 2 exit
// 3 info
externC void
cyg_test_output(int status, char* msg, int line_number, char* file);

// This should be called at the start of each test file
externC void
cyg_test_init(void);

// This causes the test to exit
externC void
cyg_test_exit(void) __attribute__ ((__noreturn__));


// ----------- Info -----------
//
// Any macro with EXIT in it should only be used in a panic situation. It
// is synonymous with assert. If the test behaves as expected, it
// should call one of the FINISH macros.
//
// - Compound testcases
// If a testcase is capable of being part of a compound, then the following
// rules apply:
// - The testcase must only ever call one of the EXIT macros if it decides
//   the state of the system is such that further testing is meaningless;
//   such a call would prevent subsequent tests in the compound from being
//   run.
// - In order to terminate the test, the testcase should call one of the
//   FINISH macros. This must be done from within main().




// The following is the testcase API to be used by testcases.

#define CYG_TEST_INIT() cyg_test_init()

#define CYG_TEST_INFO( _msg_ ) \
 cyg_test_output(3, _msg_, __LINE__, __FILE__)

#define CYG_TEST_PASS( _msg_ ) \
 cyg_test_output(1, _msg_, __LINE__, __FILE__)

#define CYG_TEST_FAIL( _msg_ ) \
 cyg_test_output(0, _msg_, __LINE__, __FILE__)

#define CYG_TEST_EXIT( _msg_ ) \
 (cyg_test_output(2, _msg_, __LINE__, __FILE__), \
  cyg_test_exit())

#ifdef CYG_COMPOUND_TEST
#  define CYG_TEST_FINISH( _msg_ ) \
     (cyg_test_output(2, _msg_, __LINE__, __FILE__), \
     return 0;
#else
#  define CYG_TEST_FINISH( _msg_ ) \
   (cyg_test_output(2, _msg_, __LINE__, __FILE__), \
   cyg_test_exit())
#endif

#define CYG_TEST_STILL_ALIVE( _ctr_ , _msg_ ) CYG_TEST_INFO( _msg_ )




// ----- The following are convenience functions

#define CYG_TEST_PASS_FINISH( _msg_ ) \
 (cyg_test_output(1, _msg_, __LINE__, __FILE__), \
 CYG_TEST_FINISH("done"))
 
#define CYG_TEST_FAIL_FINISH( _msg_ ) \
 (cyg_test_output(0, _msg_, __LINE__, __FILE__), \
 CYG_TEST_FINISH("done"))


#define CYG_TEST_CHECK( _chk_ , _msg_) \
    (void)(( _chk_ ) || ( cyg_test_output(0, _msg_, __LINE__, __FILE__),  \
                                        cyg_test_exit(), 1) )

#define CYG_TEST_PASS_FAIL( _cdn_, _msg_ ) \
if ( _cdn_ ) CYG_TEST_PASS( _msg_ ); else CYG_TEST_FAIL( _msg_ )


// CYG_TEST_PASS_EXIT and CYG_TEST_FAIL_EXIT are now obscelete, 
// but are here for now
// to avoid breaking testcases which still use them. They will
// soon go away.
#define CYG_TEST_PASS_EXIT( _msg_ ) \
 (cyg_test_output(1, _msg_, __LINE__, __FILE__), \
 CYG_TEST_EXIT("done"))

#define CYG_TEST_FAIL_EXIT( _msg_ ) \
 (cyg_test_output(0, _msg_, __LINE__, __FILE__), \
 CYG_TEST_EXIT("done"))


#endif // CYGONCE_INFRA_TESTCASE_H
// EOF testcase.h
