//==========================================================================
//
//        timer1.cxx
//
//        POSIX signal test 1
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
// Author(s):     nickg
// Contributors:  nickg
// Date:          2000-04-10
// Description:   Tests POSIX signal functionality.
//
//####DESCRIPTIONEND####
//==========================================================================

#include <cyg/infra/testcase.h>
#include <pkgconf/posix.h>

#ifndef CYGPKG_POSIX_SIGNALS
#define NA_MSG "No POSIX signals"
#elif !defined(CYGPKG_POSIX_TIMERS)
#define NA_MSG "No POSIX timers"
#endif

#ifdef NA_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(NA_MSG);
}
#else

#include <sys/types.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>
#include <time.h>

//--------------------------------------------------------------------------
// Thread stack.

char thread1_stack[PTHREAD_STACK_MIN*2];
char thread2_stack[PTHREAD_STACK_MIN*2];

//--------------------------------------------------------------------------
// Local variables

// Sync semaphore
sem_t sem;

// Thread IDs
pthread_t thread1;
pthread_t thread2;

timer_t timer1;
timer_t timer2;

volatile int sigusr1_called = 0;
volatile int sigusr2_called = 0;

//--------------------------------------------------------------------------
// Signal handler functions

static void sigusr1( int signo, siginfo_t *info, void *context )
{
    CYG_TEST_INFO( "sigusr1() handler called" );
    CYG_TEST_CHECK( signo == SIGUSR1, "Signal not SIGUSR1");
    CYG_TEST_CHECK( signo == info->si_signo, "Bad signal number in siginfo" );
    CYG_TEST_CHECK( info->si_code == SI_TIMER, "Siginfo code not SI_TIMER" );
    CYG_TEST_CHECK( info->si_value.sival_int == 0xABCDEF01, "Siginfo value wrong");
    CYG_TEST_CHECK( pthread_equal(pthread_self(), thread1), "Not called in thread1");

    sigusr1_called++;
}

static void sigusr2( int signo, siginfo_t *info, void *context )
{
    CYG_TEST_INFO( "sigusr2() handler called" );
    CYG_TEST_CHECK( signo == SIGUSR2, "Signal not SIGUSR2");
    CYG_TEST_CHECK( signo == info->si_signo, "Bad signal number in siginfo" );
    CYG_TEST_CHECK( info->si_code == SI_TIMER, "Siginfo code not SI_TIMER" );
    CYG_TEST_CHECK( info->si_value.sival_int == 0xABCDEF02, "Siginfo value wrong");
    CYG_TEST_CHECK( pthread_equal(pthread_self(), thread2), "Not called in thread2");

    sigusr2_called++;
}

//--------------------------------------------------------------------------

void *pthread_entry1( void *arg)
{
    sigset_t mask;

    
    CYG_TEST_INFO( "Thread 1 running" );

    // Make a full set
    sigfillset( &mask );

    // remove USR1 signal
    sigdelset( &mask, SIGUSR1 );

    // Set signal mask
    pthread_sigmask( SIG_SETMASK, &mask, NULL );
    
    // Get main thread going again
    sem_post( &sem );

    while( sigusr1_called < 1 )
    {
        CYG_TEST_INFO( "Thread1: calling pause()");        
        pause();
    }

    CYG_TEST_INFO( "Thread1: calling pthread_exit()");    
    pthread_exit( arg );
}

//--------------------------------------------------------------------------

void *pthread_entry2( void *arg)
{
    sigset_t mask;
    
    CYG_TEST_INFO( "Thread 2 running" );

    // Make a full set
    sigfillset( &mask );

    // remove USR2 signal
    sigdelset( &mask, SIGUSR2 );

    // Set signal mask
    pthread_sigmask( SIG_SETMASK, &mask, NULL );
    
    // Get main thread going again
    sem_post( &sem );

    while( sigusr2_called < 6 )
    {
        CYG_TEST_INFO( "Thread2: calling pause()");        
        pause();
    }

    CYG_TEST_INFO( "Thread2: calling pthread_exit()");    
    pthread_exit( arg );
}

//--------------------------------------------------------------------------

int main(int argc, char **argv)
{
    int ret;
    sigset_t mask;
    pthread_attr_t attr;
    void *retval;
    
    CYG_TEST_INIT();

    // Make a full signal set
    sigfillset( &mask );

    
    // Install signal handlers
    {
        struct sigaction sa;

        sa.sa_sigaction = sigusr1;
        sa.sa_mask = mask;
        sa.sa_flags = SA_SIGINFO;

        ret = sigaction( SIGUSR1, &sa, NULL );

        CYG_TEST_CHECK( ret == 0 , "sigaction returned error");
    }
    {
        struct sigaction sa;

        sa.sa_sigaction = sigusr2;
        sa.sa_mask = mask;
        sa.sa_flags = SA_SIGINFO;

        ret = sigaction( SIGUSR2, &sa, NULL );

        CYG_TEST_CHECK( ret == 0 , "sigaction returned error");
    }


    // Create the timers

    {
        struct sigevent sev;
        struct itimerspec value;
        
        sev.sigev_notify                = SIGEV_SIGNAL;
        sev.sigev_signo                 = SIGUSR1;
        sev.sigev_value.sival_int       = 0xABCDEF01;

        value.it_value.tv_sec           = 1;
        value.it_value.tv_nsec          = 0;
        value.it_interval.tv_sec        = 0;
        value.it_interval.tv_nsec       = 0;
        
        ret = timer_create( CLOCK_REALTIME, &sev, &timer1 );

        CYG_TEST_CHECK( ret == 0 , "timer_create returned error");

        ret = timer_settime( timer1, 0, &value, NULL );

        CYG_TEST_CHECK( ret == 0 , "timer_settime returned error");
    }

#if 1    
    {
        struct sigevent sev;
        struct itimerspec value;
        
        sev.sigev_notify                = SIGEV_SIGNAL;
        sev.sigev_signo                 = SIGUSR2;
        sev.sigev_value.sival_int       = 0xABCDEF02;

        value.it_value.tv_sec           = 0;
        value.it_value.tv_nsec          = 500000000;
        value.it_interval.tv_sec        = 0;
        value.it_interval.tv_nsec       = 250000000;
        
        ret = timer_create( CLOCK_REALTIME, &sev, &timer2 );

        CYG_TEST_CHECK( ret == 0 , "timer_create returned error");

        ret = timer_settime( timer2, 0, &value, NULL );

        CYG_TEST_CHECK( ret == 0 , "timer_settime returned error");
    }
#endif    
    
    // Mask all signals
    pthread_sigmask( SIG_SETMASK, &mask, NULL );
    
    sem_init( &sem, 0, 0 );
    
    // Create test threads

    {
        pthread_attr_init( &attr );

        pthread_attr_setstackaddr( &attr, (void *)&thread1_stack[sizeof(thread1_stack)] );
        pthread_attr_setstacksize( &attr, sizeof(thread1_stack) );

        pthread_create( &thread1,
                        &attr,
                        pthread_entry1,
                        (void *)0x12345671);
    }

    {
        pthread_attr_init( &attr );

        pthread_attr_setstackaddr( &attr, (void *)&thread2_stack[sizeof(thread2_stack)] );
        pthread_attr_setstacksize( &attr, sizeof(thread2_stack) );

        pthread_create( &thread2,
                        &attr,
                        pthread_entry2,
                        (void *)0x12345672);
    }
    
    // Wait for other thread to get started
    CYG_TEST_INFO( "Main: calling sem_wait()");
    sem_wait( &sem );
    CYG_TEST_INFO( "Main: calling sem_wait() again");    
    sem_wait( &sem );

    // Now join with thread1
    CYG_TEST_INFO( "Main: calling pthread_join(thread1)");
    pthread_join( thread1, &retval );

    CYG_TEST_CHECK( retval == (void *)0x12345671, "Thread 1 retval wrong");
    
    // And thread 2
    CYG_TEST_INFO( "Main: calling pthread_join(thread2)");
    pthread_join( thread2, &retval );

    // now delete the timers
    CYG_TEST_INFO( "Main: calling timer_delete(timer1)");    
    ret = timer_delete( timer1 );

    CYG_TEST_CHECK( ret == 0 , "timer_delete(timer1) returned error");

    CYG_TEST_INFO( "Main: calling timer_delete(timer2)");    
    ret = timer_delete( timer2 );

    CYG_TEST_CHECK( ret == 0 , "timer_delete(timer2) returned error");

    
    CYG_TEST_CHECK( retval == (void *)0x12345672, "Thread 2 retval wrong");
    
    CYG_TEST_CHECK( sigusr1_called == 1, "SIGUSR1 signal handler not called once" );
    CYG_TEST_CHECK( sigusr2_called == 6, "SIGUSR2 signal handler not called six times" );

    CYG_TEST_PASS_FINISH( "timer1" );
}

#endif

//--------------------------------------------------------------------------
// end of timer1.c
