/*=================================================================
//
//        kexcept1.cxx
//
//        Kernel C API Exception test 1
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
*/

#include <cyg/hal/hal_arch.h>           // CYGNUM_HAL_STACK_SIZE_TYPICAL

#include <cyg/kernel/kapi.h>

#include <cyg/infra/testcase.h>

#ifdef CYGPKG_KERNEL_EXCEPTIONS

#ifdef CYGFUN_KERNEL_API_C

#include <cyg/hal/hal_intr.h>           // exception ranges

#include "testaux.h"

#define NTHREADS 1
#define STACKSIZE CYGNUM_HAL_STACK_SIZE_TYPICAL

static cyg_handle_t thread[NTHREADS];

static cyg_thread thread_obj[NTHREADS];
static char stack[NTHREADS][STACKSIZE];


static cyg_exception_handler_t handler0;
static int d0;

static void handler0(cyg_addrword_t data, cyg_code_t number, cyg_addrword_t info)
{
    CYG_TEST_INFO("handler 0 called");
    
    CYG_TEST_CHECK((cyg_addrword_t)123 == data, "handler given wrong data");
    
    // ignore machine specific stuff
    CYG_UNUSED_PARAM(cyg_code_t, number);
    CYG_UNUSED_PARAM(cyg_addrword_t, info);

    CYG_TEST_PASS_FINISH("Except 1 OK");
}

static void handler1(cyg_addrword_t data, cyg_code_t number, cyg_addrword_t info)
{
    CYG_TEST_INFO("handler 1 called");

    CYG_TEST_CHECK((cyg_addrword_t)&d0 == data, "handler given wrong data");

#ifdef CYGSEM_KERNEL_EXCEPTIONS_DECODE
    CYG_TEST_CHECK(number == CYGNUM_HAL_EXCEPTION_MAX, "handler given wrong number");
#else
    CYG_UNUSED_PARAM(cyg_code_t, number);
#endif

    CYG_TEST_CHECK((cyg_addrword_t)99 == info, "handler given wrong info");
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
    cyg_code_t n;
#endif
    cyg_exception_handler_t *old_handler, *old_handler1;
    cyg_addrword_t old_data, old_data1;

    CYG_UNUSED_PARAM(CYG_ADDRESS, data);

    cyg_exception_set_handler(
        CYGNUM_HAL_EXCEPTION_MAX, 
        &handler1,
        (cyg_addrword_t)&d0,
        &old_handler,
        &old_data);

    cyg_exception_set_handler(
        CYGNUM_HAL_EXCEPTION_MAX, 
        &handler1,
        (cyg_addrword_t)&d0,
        &old_handler1,
        &old_data1);
    
    CYG_TEST_CHECK(old_handler1 == &handler1,
        "register exception: old_handler not the one previously registered");
    CYG_TEST_CHECK(old_data1 == (cyg_addrword_t)&d0,
        "register exception: old_data not those previously registered");

    cyg_exception_call_handler(
        cyg_thread_self(),
        CYGNUM_HAL_EXCEPTION_MAX,
        (cyg_addrword_t)99);

    CYG_TEST_INFO("handler 1 returned");

    cyg_exception_clear_handler(CYGNUM_HAL_EXCEPTION_MAX);
    cyg_exception_clear_handler(CYGNUM_HAL_EXCEPTION_MAX);

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
        cyg_exception_set_handler(
            n,
            handler0,
            (cyg_addrword_t)123, 
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

extern void __default_exception_vsr(void);

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

    cyg_thread_create(4, entry0 , (cyg_addrword_t)0, "kexcept1",
        (void *)stack[0], STACKSIZE, &thread[0], &thread_obj[0]);
    cyg_thread_resume(thread[0]);

    cyg_scheduler_start();

    CYG_TEST_FAIL_FINISH("Not reached");
}

externC void
cyg_start( void )
{
    except0_main();
}

#else // def CYGFUN_KERNEL_API_C
#define N_A_MSG "Kernel C API layer disabled"
#endif // def CYGFUN_KERNEL_API_C
#else // def CYGPKG_KERNEL_EXCEPTIONS
#define N_A_MSG "Exceptions disabled"
#endif // def CYGPKG_KERNEL_EXCEPTIONS

#ifdef N_A_MSG
externC void
cyg_start( void )
{
    CYG_TEST_INIT();
    CYG_TEST_NA( N_A_MSG);
}
#endif // N_A_MSG

/* EOF kexcept1.c */
