//==========================================================================
//
//        mutex1.cxx
//
//        Mutex test 1
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
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-02-24
// Description:   Tests basic mutex functionality.
// Omissions:     Timed wait.
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/kernel/sched.hxx>        // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>       // Cyg_Thread

#include <cyg/kernel/mutex.hxx>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>


#define NTHREADS 3
#include "testaux.hxx"

static Cyg_Mutex m0, m1;
static Cyg_Condition_Variable cvar0( m0 ), cvar1( m0 ), cvar2( m1 );

static cyg_ucount8 m0d=0, m1d=0;

static void finish( cyg_ucount8 t )
{
    m1.lock(); {
        m1d |= 1<<t;
        if( 0x7 == m1d )
            CYG_TEST_PASS_FINISH("Mutex 1 OK");
        cvar2.wait();
    }
    CYG_TEST_FAIL_FINISH("Not reached");    
}

static void entry0( CYG_ADDRWORD data )
{
    m0.lock(); {
        CHECK( ! m0.trylock() );
        m1.lock(); {
            CHECK( ! m0.trylock() );            
        } m1.unlock();
    } m0.unlock();

    m0.lock(); {
        while ( 0 == m0d )
            cvar0.wait();
        CHECK( 1 == m0d++ );
        cvar0.signal();
        while ( 4 != m0d )
            cvar1.wait();
        CHECK( 4 == m0d );
    } m0.unlock();

    finish( data );
}

static void entry1( CYG_ADDRWORD data )
{
    m0.lock(); {
        CHECK( m1.trylock() ); {
        } m1.unlock();
    } m0.unlock();

    m0.lock(); {
        CHECK( 0 == m0d++ );
        cvar0.broadcast();
    } m0.unlock();
    
    m0.lock(); {
        while( 1 == m0d )
            cvar0.wait();
        CHECK( 2 == m0d++ );
        cvar0.signal();
        while (3 == m0d )
            cvar1.wait();
    } m0.unlock();

    finish( data );                 // At most 1 finish inside m0 lock
}

static void entry2( CYG_ADDRWORD data )
{
    m0.lock(); {
        while( 3 != m0d ) {
            cvar0.wait();
        }
        CHECK( 3 == m0d++ );
        cvar1.broadcast();
    } m0.unlock();

    finish( data );    
}

void mutex1_main( void )
{
    CYG_TEST_INIT();

    new_thread(entry0, 0);
    new_thread(entry1, 1);
    new_thread(entry2, 2);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{ 
    mutex1_main();
}
// EOF mutex1.cxx
