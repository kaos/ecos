//========================================================================
//
//      signal2.c
//
//      ISO C signal handling test
//
//========================================================================
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
//========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     jlarmour
// Contributors:  jlarmour
// Date:          1999-02-18
// Purpose:       Test hardware signal functionality
// Description:   This file contains a number of tests for ISO C signal
//                handling when used with hardware exceptions
// Usage:         
//
//####DESCRIPTIONEND####
//
//========================================================================

// CONFIGURATION

#include <pkgconf/libc.h>          // C library configuration

// INCLUDES

#include <cyg/infra/cyg_type.h>    // Common type definitions and support
#include <cyg/infra/testcase.h>    // Test infrastructure
#include <sys/cstartup.h>          // C library initialisation

#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS

#include <cyg/hal/hal_intr.h>      // exception ranges &
                                   // HAL_VSR_SET_TO_ECOS_HANDLER

#include <signal.h>                // Signal functions
#include <setjmp.h>                // setjmp(), longjmp()

// STATICS

static int state;
static jmp_buf jbuf;
#endif

// FUNCTIONS

externC void
cyg_package_start( void )
{
    cyg_iso_c_start();
} // cyg_package_start()

#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS

static void
myhandler(int sig)
{
    __sighandler_t handler1;

    CYG_TEST_INFO("myhandler() called");
    ++state;

    handler1 = signal(sig, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "handler reset itself to default");

    longjmp(jbuf, 1);
} // myhandler()

static void
cause_memerror(void)
{
    volatile int x;
    volatile CYG_ADDRESS p=(CYG_ADDRESS) &state;

    do {
        // do a read which prevents us accidentally writing over something
        // important. Make it misaligned to increase the chances of an
        // exception happening
        x = *(volatile int *)(p+1);
        p += (CYG_ADDRESS)0x100000;
    } while(p != 0);
} // cause_memerror()

// num must always be 0 - do it this way in case the optimizer tries to
// get smart
static double
cause_fpe(int num)
{
    return 1.0/num;
} // cause_fpe()

#endif

int
main( int argc, char *argv[] )
{
#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS
    __sighandler_t handler1;

    // special callout to request GDB to alter its handling of signals
    CYG_TEST_GDBCMD("handle SIGBUS nostop");
    CYG_TEST_GDBCMD("handle SIGSEGV nostop");
    CYG_TEST_GDBCMD("handle SIGILL nostop");
    CYG_TEST_GDBCMD("handle SIGFPE nostop");
    CYG_TEST_GDBCMD("handle SIGSYS nostop");

// Not sure about this - Jifl. Do any of our targets generate SIGTRAP anyway?
#if 0
#ifndef CYGPKG_HAL_I386_LINUX
    CYG_TEST_GDBCMD("handle SIGTRAP nostop pass");
#endif
#endif
#endif // ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS

    CYG_TEST_INIT();

#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS
    // Avoid compiler warnings if tests are not applicable.
    if (0) cause_memerror();
    if (0) cause_fpe(0);
#endif
    
    CYG_TEST_INFO("Starting tests from testcase " __FILE__ " for C "
                  "library signal functions");

// Now reset the exception handlers various to eCos handlers so that we
// have control; this is the target side equivalent of the CYG_TEST_GDBCMD
// lines above:
#ifdef HAL_VSR_SET_TO_ECOS_HANDLER
    // Reclaim the VSR off CygMon possibly
#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DATA_ACCESS, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION, NULL );
#endif
#endif

#ifdef CYGSEM_LIBC_SIGNALS_HWEXCEPTIONS


    // Test 1

    CYG_TEST_INFO("Test 1");

    handler1 = signal(SIGSEGV, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGSEGV handler initialized to default");


    handler1 = signal(SIGBUS, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGBUS handler initialized to default");


    handler1 = signal(SIGILL, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGILL handler initialized to default");

    handler1 = signal(SIGSYS, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGSYS handler initialized to default");

    handler1 = signal(SIGTRAP, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGTRAP handler initialized to default");

    handler1 = signal(SIGFPE, &myhandler);

    CYG_TEST_PASS_FAIL(handler1 == SIG_DFL,
                       "SIGFPE handler initialized to default");

    // Test 2

    CYG_TEST_INFO("Test 2");

    state = 2;

#if 0
#elif defined(CYGPKG_HAL_POWERPC_SIM)
    // The exception generated by the SIM is not recognized by GDB.
    // PR 19945 workaround.
    CYG_TEST_PASS("Test 3 not applicable to PowerPC SIM");
#else
    if (0==setjmp(jbuf)) {
        cause_memerror();
        CYG_TEST_FAIL("Didn't cause exception");
    }
    
    CYG_TEST_PASS_FAIL(3==state, "handler returned correctly");
#endif

    // Test 3

    CYG_TEST_INFO("Test 3");
    
    state = 3;

#if 0
#elif defined(CYGPKG_HAL_MN10300)
    CYG_TEST_PASS("Test 3 not applicable to mn10300");
#elif defined(CYGPKG_HAL_MIPS_TX39)
    CYG_TEST_PASS("Test 3 not applicable to mips-tx39");
#elif defined(CYGPKG_HAL_SPARCLITE)
    CYG_TEST_PASS("Test 3 not applicable to sparclite");
#elif defined(CYGPKG_HAL_I386_LINUX)
    CYG_TEST_PASS("Test 3 not applicable to i386/Linux");
    // See linux.S for details.
#elif defined(CYGPKG_HAL_POWERPC)
    CYG_TEST_PASS("Test 3 not applicable to PowerPC");
#else

    if (0==setjmp(jbuf)) {
        (void)cause_fpe(0);
        CYG_TEST_FAIL("Didn't cause exception");
    }
    
    CYG_TEST_PASS_FAIL(4==state, "handler returned correctly");
#endif

#else
    CYG_TEST_NA("Testing not applicable to this configuration");
#endif

    CYG_TEST_FINISH("Finished tests from testcase " __FILE__ " for C "
                    "library signal functions");

} // main()

// EOF signal2.c
