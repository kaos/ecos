//==========================================================================
//
//        signal2.cxx
//
//        POSIX signal test 2
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
// Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
// All Rights Reserved.                                                     
// -------------------------------------------                              
//                                                                          
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     nickg
// Contributors:  jlarmour
// Date:          2000-04-10
// Description:   Tests POSIX signal functionality.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <pkgconf/isoinfra.h>
#include <cyg/hal/hal_intr.h>   // For exception codes

#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#include <setjmp.h>

#include <cyg/infra/testcase.h>

#if CYGINT_ISO_SETJMP == 0
# define NA_MSG "Requires setjmp/longjmp implementation"
#endif

#ifdef NA_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA( NA_MSG );
}
#else

//--------------------------------------------------------------------------
// Local variables

static jmp_buf jbuf;

//--------------------------------------------------------------------------

#ifdef CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS

static void
cause_unaligned_access(void)
{
    volatile int x;
    volatile CYG_ADDRESS p=(CYG_ADDRESS) &jbuf;

    x = *(volatile int *)(p+1);

} // cause_unaligned_access()

#endif

//--------------------------------------------------------------------------

#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS

static void
cause_illegal_access(void)
{
    volatile int x;
    volatile CYG_ADDRESS p=(CYG_ADDRESS) &jbuf;

    do
    {
        x = *(volatile int *)(p);
        p += (CYG_ADDRESS)0x100000;
    } while( p != 0 );

} // cause_unaligned_access()

#endif

//--------------------------------------------------------------------------

#ifdef CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO

// num must always be 0 - do it this way in case the optimizer tries to
// get smart

static int
cause_fpe(int num)
{
    double a;

    a = 1.0/num;                        // Depending on FPU emulation and/or
                                        // the FPU architecture, this may
                                        // cause an exception.
                                        // (float division by zero)

    return ((int)a)/num;                // This may cause an exception if
                                        // the architecture supports it.
                                        // (integer division by zero).
} // cause_fpe()

#endif

//--------------------------------------------------------------------------
// Signal handler functions

static void sigsegv( int signo )
{
    CYG_TEST_INFO( "sigsegv() handler called" );
    CYG_TEST_CHECK( signo == SIGSEGV, "Signal not SIGSEGV");

    longjmp( jbuf, 1 );
}

static void sigbus( int signo )
{
    CYG_TEST_INFO( "sigbus() handler called" );
    CYG_TEST_CHECK( signo == SIGBUS, "Signal not SIGBUS");

    longjmp( jbuf, 1 );
}

static void sigfpe( int signo )
{
    CYG_TEST_INFO( "sigfpe() handler called" );
    CYG_TEST_CHECK( signo == SIGFPE, "Signal not SIGFPE");

    longjmp( jbuf, 1 );
}


//--------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int ret;
    sigset_t mask;
    struct sigaction sa;
    
    CYG_TEST_INIT();

    // Make a full signal set
    sigfillset( &mask );

   
    // Install signal handlers

    sa.sa_mask = mask;
    sa.sa_flags = 0;

    sa.sa_handler = sigsegv;
    ret = sigaction( SIGSEGV, &sa, NULL );
    CYG_TEST_CHECK( ret == 0 , "sigaction returned error");

    sa.sa_handler = sigbus;
    ret = sigaction( SIGBUS, &sa, NULL );
    CYG_TEST_CHECK( ret == 0 , "sigaction returned error");

    sa.sa_handler = sigfpe;
    ret = sigaction( SIGFPE, &sa, NULL );
    CYG_TEST_CHECK( ret == 0 , "sigaction returned error");

    // now make an empty signal set
    sigemptyset( &mask );
    
// Now reset the various exception handlers to eCos handlers so that we
// have control; this is the target side equivalent of the CYG_TEST_GDBCMD
// lines above:
#ifdef HAL_VSR_SET_TO_ECOS_HANDLER
    // Reclaim the VSR off CygMon possibly
#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DATA_ACCESS, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_ACCESS
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DATA_TLBMISS_ACCESS, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_ILLEGAL_INSTRUCTION, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO, NULL );
#endif
#endif

#ifdef CYGNUM_HAL_EXCEPTION_DATA_UNALIGNED_ACCESS
    
    CYG_TEST_INFO("Test 1 - provoke unaligned access");
    
    if( setjmp( jbuf ) == 0 )
    {
        pthread_sigmask( SIG_SETMASK, &mask, NULL );
        cause_unaligned_access();
        CYG_TEST_FAIL("Didn't cause exception");        
    }

#else

    CYG_TEST_INFO("Test 1 - provoke unaligned access - not supported");

#endif    

#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
    
    CYG_TEST_INFO("Test 2 - provoke illegal access");
    
    if( setjmp( jbuf ) == 0 )
    {
        pthread_sigmask( SIG_SETMASK, &mask, NULL );
        cause_illegal_access();
        CYG_TEST_FAIL("Didn't cause exception");        
    }

#else

    CYG_TEST_INFO("Test 1 - provoke illegal access - not supported");

#endif    
    
#ifdef CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO
    
    CYG_TEST_INFO("Test 3 - provoke FP error");    

    if( setjmp( jbuf ) == 0 )
    {
        pthread_sigmask( SIG_SETMASK, &mask, NULL );
        cause_fpe(0);
        CYG_TEST_FAIL("Didn't cause exception");        
    }

#else

    CYG_TEST_INFO("Test 3 - provoke FP error - not supported");    
    
#endif    

    CYG_TEST_PASS_FINISH( "signal2" );
}

#endif // ifndef NA_MSG

//--------------------------------------------------------------------------
// end of signal1.c
