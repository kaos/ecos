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
// The contents of this file are subject to the Red Hat eCos Public License 
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):       ctarpy
// Contributors:    ctarpy, jlarmour
// Date:            1999-02-16
//
//
//####DESCRIPTIONEND####

#include <cyg/infra/cyg_type.h> // Common type definitions and support


// CONSTANTS

// Status codes

typedef enum {
    CYGNUM_TEST_FAIL,
    CYGNUM_TEST_PASS,
    CYGNUM_TEST_EXIT,
    CYGNUM_TEST_INFO,
    CYGNUM_TEST_GDBCMD,
    CYGNUM_TEST_NA
} Cyg_test_code;

// FUNCTION PROTOTYPES

externC void
cyg_test_output(Cyg_test_code _status_, const char* _msg_, int _line_number_,
                const char* _file_);

// This should be called at the start of each test file
externC void
cyg_test_init(void);

// This causes the test to exit
externC void
cyg_test_exit(void) CYGBLD_ATTRIB_NORET;

// GLOBALS

externC int cyg_test_is_simulator;    // infrastructure changes as necessary

// MACROS

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
 cyg_test_output(CYGNUM_TEST_INFO, _msg_, __LINE__, __FILE__)

#define CYG_TEST_PASS( _msg_ ) \
 cyg_test_output(CYGNUM_TEST_PASS, _msg_, __LINE__, __FILE__)

#define CYG_TEST_FAIL( _msg_ ) \
 cyg_test_output(CYGNUM_TEST_FAIL, _msg_, __LINE__, __FILE__)

#define CYG_TEST_EXIT( _msg_ ) \
 (cyg_test_output(CYGNUM_TEST_EXIT, _msg_, __LINE__, __FILE__), \
  cyg_test_exit())

// Use the following macro to instruct GDB to run a command when using
// the automatic testing infrastructure. This must be used *before*
// CYG_TEST_INIT() is called

#define CYG_TEST_GDBCMD( _command_ )                                     \
     CYG_MACRO_START                                                     \
     cyg_test_output(CYGNUM_TEST_GDBCMD, _command_, __LINE__, __FILE__); \
     CYG_MACRO_END

// Use the following macro to declare that a test is not applicable for
// some reason - perhaps not appropriate due to chosen hardware,
// configuration options governing the presence of a tested feature, or
// even configuration options governing the presence of a feature the
// test relies on _in_order_ to test the feature (despite being
// unrelated!)

#define CYG_TEST_NA( _msg_ )                                         \
     CYG_MACRO_START                                                 \
     cyg_test_output(CYGNUM_TEST_NA, _msg_, __LINE__, __FILE__);     \
     cyg_test_exit();                                                \
     CYG_MACRO_END

#ifdef CYG_COMPOUND_TEST
#  define CYG_TEST_FINISH( _msg_ )                                  \
     CYG_MACRO_START                                                \
     cyg_test_output(CYGNUM_TEST_EXIT, _msg_, __LINE__, __FILE__);  \
     return 0;                                                      \
     CYG_MACRO_END
#else
#  define CYG_TEST_FINISH( _msg_ ) CYG_TEST_EXIT( _msg_ )
#endif

#define CYG_TEST_STILL_ALIVE( _ctr_ , _msg_ ) CYG_TEST_INFO( _msg_ )


// ----- The following are convenience functions

#define CYG_TEST_PASS_FINISH( _msg_ ) \
    CYG_MACRO_START                   \
    CYG_TEST_PASS( _msg_ );           \
    CYG_TEST_FINISH("done");          \
    CYG_MACRO_END
 
#define CYG_TEST_FAIL_FINISH( _msg_ ) \
    CYG_MACRO_START                   \
    CYG_TEST_FAIL( _msg_ );           \
    CYG_TEST_FINISH("done");          \
    CYG_MACRO_END


#define CYG_TEST_CHECK( _chk_ , _msg_)                                   \
    CYG_MACRO_START                                                      \
    (void)(( _chk_ ) || ( CYG_TEST_FAIL( _msg_ ) , cyg_test_exit(), 1)); \
    CYG_MACRO_END

#define CYG_TEST_PASS_FAIL( _cdn_, _msg_ )                            \
    CYG_MACRO_START                                                   \
    if ( _cdn_ ) CYG_TEST_PASS( _msg_ ); else CYG_TEST_FAIL( _msg_ ); \
    CYG_MACRO_END


// CYG_TEST_PASS_EXIT and CYG_TEST_FAIL_EXIT are now obscelete, 
// but are here for now
// to avoid breaking testcases which still use them. They will
// soon go away.
#define CYG_TEST_PASS_EXIT( _msg_ )                             \
 (cyg_test_output(CYGNUM_TEST_PASS, _msg_, __LINE__, __FILE__), \
 CYG_TEST_EXIT("done"))

#define CYG_TEST_FAIL_EXIT( _msg_ )                             \
 (cyg_test_output(CYGNUM_TEST_FAIL, _msg_, __LINE__, __FILE__), \
 CYG_TEST_EXIT("done"))


#endif // CYGONCE_INFRA_TESTCASE_H
// EOF testcase.h
