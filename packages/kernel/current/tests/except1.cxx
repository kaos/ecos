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
//=================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     dsm
// Contributors:  dsm, jlarmour
// Date:          1999-02-16
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

static int d0;
#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
static cyg_exception_handler handler0;

static void handler0(CYG_ADDRWORD data, cyg_code number, CYG_ADDRWORD info)
{
    CYG_TEST_INFO("handler 0 called");
    
    CYG_TEST_CHECK((CYG_ADDRWORD)123 == data, "handler given wrong data");
    
    // ignore machine specific stuff
    CYG_UNUSED_PARAM(cyg_code, number);
    CYG_UNUSED_PARAM(CYG_ADDRWORD, info);

    CYG_TEST_PASS_FINISH("Except 1 OK");
}
#endif

static void handler1(CYG_ADDRWORD data, cyg_code number, CYG_ADDRWORD info)
{
    CYG_TEST_INFO("handler 1 called");

    CYG_TEST_CHECK((CYG_ADDRWORD)&d0 == data, "handler given wrong data");

#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE
    CYG_TEST_CHECK(number == CYGNUM_HAL_EXCEPTION_MAX, "handler given wrong number");
#else
    CYG_UNUSED_PARAM(cyg_code, number);
#endif

    CYG_TEST_CHECK((CYG_ADDRWORD)99 == info, "handler given wrong info");
}

#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
// The following function attempts to cause an exception in various
// hacky ways.  It is machine dependent what exception is generated.
// It does reads rather than writes hoping not to corrupt anything
// important.
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

void cause_exception(void)
{
    int x;
    unsigned int p=0;

    // First try for an address exception (unaligned access exception
    // or SEGV/BUS exceptions)
    do {
        x=*(volatile int *)(p-1);
        p+=0x100000;
    } while(p != 0);

    // Next try an integer or floating point divide-by-zero exception.
    cause_fpe(0);
}
#endif

static void entry0( CYG_ADDRWORD data )
{
#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS
    cyg_code n;
#endif
    cyg_exception_handler *old_handler, *old_handler1;
    CYG_ADDRWORD old_data, old_data1;
    Cyg_Thread *p=Cyg_Thread::self();

    CYG_UNUSED_PARAM(CYG_ADDRESS, data);

    p->register_exception(
        CYGNUM_HAL_EXCEPTION_MAX, 
        &handler1,
        (CYG_ADDRWORD)&d0,
        &old_handler,
        &old_data);

    p->register_exception(
        CYGNUM_HAL_EXCEPTION_MAX, 
        &handler1,
        (CYG_ADDRWORD)&d0,
        &old_handler1,
        &old_data1);
    
    CYG_TEST_CHECK(old_handler1 == &handler1,
        "register exception: old_handler not the one previously registered");
    CYG_TEST_CHECK(old_data1 == (CYG_ADDRWORD)&d0,
        "register exception: old_data not those previously registered");

    p->deliver_exception(CYGNUM_HAL_EXCEPTION_MAX, (CYG_ADDRWORD)99);

    CYG_TEST_INFO("handler 1 returned");

    p->deregister_exception(CYGNUM_HAL_EXCEPTION_MAX);
    p->deregister_exception(CYGNUM_HAL_EXCEPTION_MAX);

#ifdef CYGNUM_HAL_EXCEPTION_DATA_ACCESS

#if 0
#elif defined(CYGPKG_HAL_POWERPC_SIM)
    // The exception generated by the SIM is not recognized by GDB.
    // PR 19945 workaround.
    CYG_TEST_NA("Not applicable to PowerPC SIM");
#elif defined(CYGPKG_HAL_I386_LINUX)
    // We can't catch segmentation violation exceptions on Linux.
    CYG_TEST_NA("Not applicable to synthetic target");
#endif

    for(n = CYGNUM_HAL_EXCEPTION_MIN; n <= CYGNUM_HAL_EXCEPTION_MAX; n++) {
        p->register_exception(
            n,
            handler0,
            (CYG_ADDRWORD)123, 
            &old_handler1,
            &old_data1);
    }

    CYG_TEST_PASS("Attempting to provoke exception");

    cause_exception();

    CYG_TEST_FAIL_FINISH("Couldn't cause exception");
#else // CYGNUM_HAL_EXCEPTION_DATA_ACCESS
    CYG_TEST_NA("Platform does not support data exceptions");
#endif

}

#ifdef CYG_HAL_MIPS_TX39_JMR3904

externC cyg_VSR __default_exception_vsr;
cyg_VSR *old_vsr;

#endif

void except0_main( void )
{
    // Use CYG_TEST_GDBCMD _before_ CYG_TEST_INIT()
    CYG_TEST_GDBCMD("handle SIGBUS nostop");
    CYG_TEST_GDBCMD("handle SIGSEGV nostop");
    CYG_TEST_GDBCMD("handle SIGFPE nostop");

    CYG_TEST_INIT();

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
#ifdef CYGNUM_HAL_EXCEPTION_DIV_BY_ZERO
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_DIV_BY_ZERO, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_FPU
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_FPU, NULL );
#endif
#ifdef CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO
    HAL_VSR_SET_TO_ECOS_HANDLER( CYGNUM_HAL_EXCEPTION_FPU_DIV_BY_ZERO, NULL );
#endif
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
    CYG_TEST_NA("Exceptions disabled");
}
#endif // def CYGPKG_KERNEL_EXCEPTIONS

// EOF except1.cxx
