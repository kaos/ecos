/*=================================================================
//
//        slebtime.cxx
//
//        SPARClite HAL timing tests
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
// Author(s):     dsm
// Contributors:    dsm, nickg
// Date:          1998-06-18
//####DESCRIPTIONEND####
*/

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/testcase.h>

#ifdef CYGPKG_KERNEL

#include <pkgconf/kernel.h>

#ifdef CYGVAR_KERNEL_COUNTERS_CLOCK

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_clock.h>

#include <pkgconf/infra.h>

#include <cyg/infra/diag.h>

#include <cyg/kernel/thread.hxx>
#include <cyg/kernel/thread.inl>
#include <cyg/kernel/sched.hxx>
#include <cyg/kernel/sched.inl>

// -------------------------------------------------------------------------

static void entry1( CYG_ADDRWORD arg )
{
    int i;
    char *s = "tick 00";
    extern Cyg_Thread thread2;
    CYG_TEST_INFO( "Starting measured seconds..." );
    for ( i = 0; i < 20 ; i++ ) {
        Cyg_Thread::self()->delay( 100 ); // units should be centiSeconds
        if ( '9' == s[ 6 ]++ ) {
            s[ 6 ] = '0';
            s[ 5 ] ++;
        }
        CYG_TEST_INFO( s );
    }
    CYG_TEST_PASS( "Done measured seconds..." );
    thread2.resume();
}

static void entry2( CYG_ADDRWORD arg )
{
#define LOOPS 30
    int i, t1, t2, a;
    int loops[ LOOPS ] = { 0,0,0,0,0 , 0,0,0,0,0 ,
                           0,0,0,0,0 , 0,0,0,0,0 ,
                           0,0,0,0,0 , 0,0,0,0,0 };

    char s[] = "0 count 00 loops 00000";

    for ( a = 0; a < 10; a++, (*s)++ ) {
        CYG_TEST_INFO( "------------------------------------------" );
        HAL_CLOCK_READ( &t1 );
        do {
            t2 = t1;
            HAL_CLOCK_READ( &t1 );
        } while ( t1 >= t2 );

        for ( i = 0; i < LOOPS; i++ ) {
            register int z = 0;
            while ( t1 <= i ) {
                z++;
                HAL_CLOCK_READ( &t1 );
            }
            loops[ i ] = z;
        }

        s[9] = '0';
        s[8] = '0';
        for ( i = 0; i < LOOPS; i++ ) {
            s[ sizeof( s ) - 2 ] = '0' + loops[ i ] / 1       % 10;
            s[ sizeof( s ) - 3 ] = '0' + loops[ i ] / 10      % 10;
            s[ sizeof( s ) - 4 ] = '0' + loops[ i ] / 100     % 10;
            s[ sizeof( s ) - 5 ] = '0' + loops[ i ] / 1000    % 10;
            s[ sizeof( s ) - 6 ] = '0' + loops[ i ] / 10000   % 10;
            CYG_TEST_INFO( s );
            if ( '9' == s[9]++ ) {
                s[9] = '0';
                s[8]++;
            }        
        }
    }
    CYG_TEST_PASS( "Counted loops per timer tick" );
    CYG_TEST_EXIT( "All done" );
}

// -------------------------------------------------------------------------

static char stack1[ CYGNUM_HAL_STACK_SIZE_TYPICAL ];
static char stack2[ CYGNUM_HAL_STACK_SIZE_TYPICAL ];

static Cyg_Thread thread1     CYG_INIT_PRIORITY( APPLICATION )
 = Cyg_Thread( 2u, entry1, 0u, "timed minute thread",
               (CYG_ADDRWORD)stack1,
               (CYG_ADDRWORD)CYGNUM_HAL_STACK_SIZE_TYPICAL );

static Cyg_Thread thread2     CYG_INIT_PRIORITY( APPLICATION )
 = Cyg_Thread( 4u, entry2, 0u, "uS clock loops timing thread",
               (CYG_ADDRWORD)stack2,
               (CYG_ADDRWORD)CYGNUM_HAL_STACK_SIZE_TYPICAL );

// -------------------------------------------------------------------------

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
externC void
cyg_hal_invoke_constructors();
#endif

externC void
cyg_user_start( void )
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    cyg_hal_invoke_constructors();
#endif
    CYG_TEST_INIT();
    CYG_TEST_INFO( "cyg_user_start()" );
    thread1.resume();
}

// -------------------------------------------------------------------------

#else  // ! CYGVAR_KERNEL_COUNTERS_CLOCK
#define N_A_MSG "no kernel clock"
#endif // CYGVAR_KERNEL_COUNTERS_CLOCK
#else  // ! CYGPKG_KERNEL
#define N_A_MSG "no kernel"
#endif // CYGPKG_KERNEL

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG );
}
#endif // N_A_MSG defined ie. we are N/A.

/* EOF slebtime.cxx */
