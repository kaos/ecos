//==========================================================================
//
//      plf_misc.c
//
//      HAL platform miscellaneous functions
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
// Author(s):    nickg
// Contributors: nickg, jlarmour, pjo
// Date:         1999-01-21
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

#include <cyg/hal/plf_misc.h>

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/kapi.h>
#endif /* CYGPKG_KERNEL */

/*------------------------------------------------------------------------*/

extern void patch_dbg_syscalls(void * vector);

/*------------------------------------------------------------------------*/
/* Floating point context switch support.                                 */
/* NOTE:  This code makes use of kernel API calls, which it should not do.*/
/* We need to rewrite this to make use of only HAL supplied facilites,    */
/* this may require the addition of some extra HAL level support.         */
   
#if !defined(CYGSEM_HAL_ROM_MONITOR)

volatile int hal_pc_fpe_interrupts = 0 ;
volatile int hal_pc_fpe_switches = 0 ;
volatile cyg_uint32 hal_pc_fpe_owner = 0 ;

cyg_uint32 hal_pc_fpe_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    return 2 ;
}

void hal_pc_fpe_dsr(CYG_ADDRWORD vector, cyg_ucount32 count, CYG_ADDRWORD data)
{
    cyg_handle_t me ;
    CYG_ADDRWORD stack ;
    cyg_uint32 * p ;

    hal_pc_fpe_interrupts++ ;

/* Gotta turn that darn interrupt off!  Otherwise we're in a loop! */
    asm("movl %%cr0, %%eax\n"
        "andl $0xFFFFFFF7, %%eax\n"
        "movl %%eax, %%cr0\n"
	:	/* No outputs. */
	:	/* No inputs. */
	:	"eax"
	);

    me = cyg_thread_self() ;

    if (hal_pc_fpe_owner != me)
    {
        if (hal_pc_fpe_owner)
        {	/* Then save his state at the bottom of his stack. */
            stack = cyg_thread_get_stack_base(hal_pc_fpe_owner) ;
            p = (cyg_uint32*) stack ;
            stack = (cyg_addrword_t) &(p[1]);
            asm("movl %0, %%eax
fsave (%%eax)"
                :	/* No outputs. */
                :	"g" (stack)
                :	"eax") ;
            p[0] = 0xCAFEBABE ;
        }
        hal_pc_fpe_owner = me ;
        stack = cyg_thread_get_stack_base(hal_pc_fpe_owner) ;
        p = (cyg_uint32*) stack ;
        if (p[0] == 0xCAFEBABE)
        {
            stack = (cyg_addrword_t) &(p[1]) ;
            asm("movl %0, %%eax\n"
                "frstor (%%eax)\n"
                :	/* No outputs. */
                :	"g" (stack)
                :	"eax");
        }

        hal_pc_fpe_switches++ ;
    }
}


cyg_interrupt hal_pc_fpe_interrupt_object ;
cyg_handle_t hal_pc_fpe_interrupt ;

#endif

/*------------------------------------------------------------------------*/

void hal_platform_init(void)
{

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)    
    {
        void hal_ctrlc_isr_init(void);
        hal_ctrlc_isr_init();
    }

#endif
        
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYG_HAL_USE_ROM_MONITOR)            && \
    defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
    {
        patch_dbg_syscalls( (void *)(&hal_vsr_table[0]) );
    }
#endif

#if !defined(CYGSEM_HAL_ROM_MONITOR)
    /* Connect to the floating point interrupt. */
    cyg_interrupt_create(CYGNUM_HAL_VECTOR_NO_DEVICE, 1, 0,
                         hal_pc_fpe_isr, hal_pc_fpe_dsr,
                         &hal_pc_fpe_interrupt, &hal_pc_fpe_interrupt_object);
    cyg_interrupt_attach(hal_pc_fpe_interrupt);
#endif        

}

/*------------------------------------------------------------------------*/

void hal_pc_reset(void)
{
    /* Use Intel's IDT triple-fault trick. */
    asm("movl $badIdt, %eax\n"
        "lidt (%eax)\n"
        "int $3\n"
        "hlt\n"

        ".align 4\n"

        "badIdt:\n"
        ".word		0\n"
        ".long		0\n"
        ) ;
}


/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
