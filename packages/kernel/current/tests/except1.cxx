//=================================================================
//
//        except1.cxx
//
//        Exception test 1
//
//=================================================================
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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:    dsm
// Date:          1998-06-15
// Description:   Test basic exception functionality
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>

#include <cyg/infra/testcase.h>

#ifdef CYGPKG_KERNEL_EXCEPTIONS

#include <cyg/kernel/sched.hxx>         // Cyg_Scheduler::start()
#include <cyg/kernel/thread.hxx>        // Cyg_Thread
#include <cyg/kernel/intr.hxx>          // cyg_VSR

#include <cyg/hal/hal_intr.h>           // exception ranges

#include <cyg/kernel/sched.inl>
#include <cyg/kernel/thread.inl>

#define NTHREADS 1
#include "testaux.hxx"

static cyg_exception_handler handler0;
static int d0;

static void handler0(CYG_ADDRWORD data, cyg_code number, CYG_ADDRWORD info)
{
    CYG_TEST_INFO("handler 0 called");
    
    CYG_TEST_CHECK((CYG_ADDRWORD)123 == data, "handler given wrong data");
    
    // ignore machine specific stuff
    CYG_UNUSED_PARAM(cyg_code, number);
    CYG_UNUSED_PARAM(CYG_ADDRWORD, info);

    CYG_TEST_PASS_FINISH("Except 1 OK");
}

static void handler1(CYG_ADDRWORD data, cyg_code number, CYG_ADDRWORD info)
{
    CYG_TEST_INFO("handler 1 called");

    CYG_TEST_CHECK((CYG_ADDRWORD)&d0 == data, "handler given wrong data");

#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE
    CYG_TEST_CHECK(number == CYG_EXCEPTION_MAX, "handler given wrong number");
#else
    CYG_UNUSED_PARAM(cyg_code, number);
#endif

    CYG_TEST_CHECK((CYG_ADDRWORD)99 == info, "handler given wrong info");
}


// The following function attempts to cause an exception in various
// hacky ways.  It is machine dependent what exception is generated.
// It does reads rather than writes hoping not to corrupt anything
// important.
void cause_exception(void)
{
    int x;
    unsigned int p=0;

    do {
        x=*(volatile int *)(p-1)/p;
        p+=0x100000;
    } while(p != 0);
}

static void entry0( CYG_ADDRWORD data )
{
    cyg_code n;
    cyg_exception_handler *old_handler, *old_handler1;
    CYG_ADDRWORD old_data, old_data1;
    Cyg_Thread *p=Cyg_Thread::self();

    CYG_UNUSED_PARAM(CYG_ADDRESS, data);

    p->register_exception(
        CYG_EXCEPTION_MAX, 
        &handler1,
        (CYG_ADDRWORD)&d0,
        &old_handler,
        &old_data);

    p->register_exception(
        CYG_EXCEPTION_MAX, 
        &handler1,
        (CYG_ADDRWORD)&d0,
        &old_handler1,
        &old_data1);
    
    CYG_TEST_CHECK(old_handler1 == &handler1,
        "register exception: old_handler not the one previously registered");
    CYG_TEST_CHECK(old_data1 == (CYG_ADDRWORD)&d0,
        "register exception: old_data not those previously registered");

    p->deliver_exception(CYG_EXCEPTION_MAX, (CYG_ADDRWORD)99);

    CYG_TEST_INFO("handler 1 returned");

    p->deregister_exception(CYG_EXCEPTION_MAX);
    p->deregister_exception(CYG_EXCEPTION_MAX);

    for(n = CYG_EXCEPTION_MIN; n <= CYG_EXCEPTION_MAX; n++) {
        p->register_exception(
            n,
            handler0,
            (CYG_ADDRWORD)123, 
            &old_handler1,
            &old_data1);
    }

    CYG_TEST_INFO("Attempting to provoke exception");

    cause_exception();

    CYG_TEST_FAIL_FINISH("Couldn't cause exception");
}

#ifdef CYG_HAL_TX39_JMR3904

externC cyg_VSR __default_exception_vsr;
cyg_VSR *old_vsr;

#endif

void except0_main( void )
{
    CYG_TEST_INIT();

#ifdef CYG_HAL_TX39_JMR3904
    
    HAL_VSR_SET( CYG_VECTOR_LOAD_ADDRESS, __default_exception_vsr, &old_vsr );

#endif

    new_thread(entry0, 0);

    Cyg_Scheduler::start();

    CYG_TEST_FAIL_FINISH("Not reached");
}
externC void
cyg_start( void )
{
    except0_main();
}
#else // def CYGPKG_KERNEL_EXCEPTIONS
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_PASS_FINISH("NA: Exceptions disabled");
}
#endif // def CYGPKG_KERNEL_EXCEPTIONS

// EOF except1.cxx
