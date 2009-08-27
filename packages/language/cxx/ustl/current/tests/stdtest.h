// This file is part of the ustl library, an STL implementation.
//
// Copyright (C) 2005 by Mike Sharov <msharov@users.sourceforge.net>
// This file is free software, distributed under the MIT License.
//
// \file stdtest.h
// \brief Standard BVT harness.
//
#ifndef CYGONCE_STDTEST_H
#define CYGONCE_STDTEST_H

#include <ustl.h>
#include <signal.h>
#include <cyg/infra/testcase.h>
using namespace ustl;

typedef void (*stdtestfunc_t)(void);

#ifdef CYGVAR_USTL_CIN_COUT_CERR
/// Called when a signal is received.
static void OnSignal (int sig)
{
    cout.flush();
#ifdef HAVE_STRSIGNAL
    cerr.format ("Fatal error: %s received.\n", strsignal(sig));
#else
    cerr.format ("Fatal error: system signal %d received.\n", sig);
#endif
    cerr.flush();
    CYG_TEST_FAIL("Test failed");
    exit (sig);
}

/// Installs OnSignal as handler for signals.
static void InstallCleanupHandlers (void)
{
    static const uint8_t c_Signals[] = {
    SIGINT, SIGQUIT, SIGILL,  SIGTRAP, SIGABRT,
    SIGIOT, SIGBUS,  SIGFPE,  SIGSEGV, SIGTERM};
    for (uoff_t i = 0; i < VectorSize(c_Signals); ++i)
    {
        signal (c_Signals[i], OnSignal);
    }
}

/// executes the test function
#ifndef __EXCEPTIONS
// if eCos is compiled without exceptions support then we implement and install
// our own low level exception handler
void app_exception_handler(const ustl::exception& ex)
{
    ustl::diag_print_exception(ex);
    CYG_TEST_FAIL("Test failed because of an uSTL exception");
}

int StdTestHarness (stdtestfunc_t testFunction)
{
    InstallCleanupHandlers();
    ustl::set_app_exception_handler(app_exception_handler);
    (*testFunction)();
    cout << endl;
    cout.flush(); // if there is anything left to print then print it now
    return (EXIT_SUCCESS);
}
#else
// if eCos is compiled with exceptions support the we use the default 
// StdTestHarness implementation from uSTL library
int StdTestHarness (stdtestfunc_t testFunction)
{
    InstallCleanupHandlers();
    int rv = EXIT_FAILURE;
    try {
    (*testFunction)();
    rv = EXIT_SUCCESS;
    } catch (ustl::exception& e) {
    cout.flush();
    cerr << "Error: " << e << endl;
    } catch (...) {
    cout.flush();
    cerr << "Unexpected error." << endl;
    }
    cout << endl;
    cout.flush(); // if there is anything left to print then print it now
    return (rv);
}
#endif


#define StdBvtMain(function) int main(int argc, char *argv[])                    \
{                                                                                \
    CYG_TEST_INIT();                                                             \
    CYG_TEST_INFO("Starting tests from uSTL testcase " #function);               \
    StdTestHarness(&function);                                                   \
    CYG_TEST_PASS_FINISH(#function " finished successfully");                    \
    return EXIT_SUCCESS;                                                         \
}
#else
#define StdBvtMain(function) int main(int argc, char *argv[])                    \
{                                                                                \
    CYG_TEST_INIT();                                                             \
    CYG_TEST_NA("cin, cout & cerr support disabled");                            \
}
#endif

//---------------------------------------------------------------------------
#endif // CYGONCE_STDTEST_H
