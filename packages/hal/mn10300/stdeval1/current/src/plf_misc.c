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
// Author(s):    nickg
// Contributors: nickg, jlarmour
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

/*------------------------------------------------------------------------*/

void hal_platform_init(void)
{
    // Note that the hardware seems to come up with the
    // caches containing random data. Hence they must be
    // invalidated before being enabled.
    
    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

#if defined(CYGPKG_KERNEL)                      && \
    defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYG_HAL_USE_ROM_MONITOR)            && \
    defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
    {
        extern CYG_ADDRESS hal_virtual_vector_table[32];
        extern void patch_dbg_syscalls(void * vector);
        patch_dbg_syscalls( (void *)(&hal_virtual_vector_table[0]) );
    }
#endif    
}

/*------------------------------------------------------------------------*/
/* Functions to support the detection and execution of a user provoked    */
/* program break. These are usually called from interrupt routines.       */

cyg_bool cyg_hal_is_break(char *buf, int size)
{
    while( size )
        if( buf[--size] == 0x03 ) return true;

    return false;
}

void cyg_hal_user_break( CYG_ADDRWORD *regs )
{

#if defined(CYG_HAL_USE_ROM_MONITOR) && defined(CYG_HAL_USE_ROM_MONITOR_GDB_STUBS)

        {
            extern CYG_ADDRESS hal_virtual_vector_table[64];        
            typedef void install_bpt_fn(void *pc);
            CYG_WORD32 retpc = ((CYG_WORD32 *)(&regs))[-1];
            CYG_WORD32 pc;
            HAL_SavedRegisters *sreg = (HAL_SavedRegisters *)regs;
            install_bpt_fn *ibp = (install_bpt_fn *)hal_virtual_vector_table[35];

            if( regs == NULL ) pc = retpc;
            else pc = sreg->pc;

            if( ibp != NULL ) ibp((void *)pc);
        }
    
#elif defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

    {
        extern void breakpoint(void);
        breakpoint();
    }
    
#else

    HAL_BREAKPOINT(breakinst);

#endif
}

/*------------------------------------------------------------------------*/
/* Control C ISR support                                                  */

#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)

struct Hal_SavedRegisters *hal_saved_interrupt_state;

static void hal_ctrlc_isr_init(void)
{
}

cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{

//    char c;

    HAL_INTERRUPT_ACKNOWLEDGE( CYGHWR_HAL_GDB_PORT_VECTOR ); 

    return 2;
}

#endif

/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
