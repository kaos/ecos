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

extern void patch_dbg_syscalls(void * vector);

/*------------------------------------------------------------------------*/

void hal_platform_init(void)
{
    // On the real hardware we also enable the cache.
    // doing this here is a temporary measure until we
    // have a proper platform specific place to do it.
    
#if !defined(CYGHWR_HAL_TX39_JMR3904_ENABLE_TOE)

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

    HAL_TX39_DEBUG_TOE_DISABLE();

#else

    // If TOE is enabled, caches are disabled by default
    // until spurious Bus Timeout problem is fixed.

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_DISABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_DISABLE();

    HAL_TX39_DEBUG_TOE_ENABLE();

#endif

#if 0
#if defined(CYG_HAL_USE_ROM_MONITOR)            && \
    !defined(CYGDBG_INFRA_DIAG_USE_DEVICE)      && \
        defined(CYG_HAL_TX39_JMR3904)

        {
            static void hal_init_ctrlc_intr(void);
            hal_init_ctrlc_intr();
        }

#endif
#endif
        
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYG_HAL_USE_ROM_MONITOR)            && \
    defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
    {
        patch_dbg_syscalls( (void *)(&hal_vsr_table[0]) );
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
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

    // Ctrl-C: breakpoint.
    extern void breakpoint(void);
    breakpoint();

#elif defined(CYG_HAL_USE_ROM_MONITOR) && defined(CYG_HAL_USE_ROM_MONITOR_GDB_STUBS)

    // Ctrl-C: breakpoint.
    typedef void bpt_fn(void);
    bpt_fn *bfn = ((bpt_fn **)0x80000100)[61];
    bfn();

#elif defined(CYG_HAL_USE_ROM_MONITOR) && defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)

    // Ctrl-C: breakpoint.
    typedef void install_bpt_fn(void *epc);
    HAL_SavedRegisters *sreg = (HAL_SavedRegisters *)regs;
    install_bpt_fn *ibp = (install_bpt_fn *)hal_vsr_table[35];
    if( ibp != NULL ) ibp((void *)sreg->pc);

#endif

}

/*------------------------------------------------------------------------*/
/* This code installs an interrupt handler to capture Ctrl-C.             */

#if 0
#if defined(CYG_HAL_USE_ROM_MONITOR) && \
    !defined(CYGDBG_INFRA_DIAG_USE_DEVICE)    && \
    defined(CYG_HAL_TX39_JMR3904)

#define DIAG_BASE       0xfffff300
#define DIAG_SLCR       (DIAG_BASE+0x00)
#define DIAG_SLSR       (DIAG_BASE+0x04)
#define DIAG_SLDICR     (DIAG_BASE+0x08)
#define DIAG_SLDISR     (DIAG_BASE+0x0C)
#define DIAG_SFCR       (DIAG_BASE+0x10)
#define DIAG_SBRG       (DIAG_BASE+0x14)
#define DIAG_TFIFO      (DIAG_BASE+0x20)
#define DIAG_RFIFO      (DIAG_BASE+0x30)

#define BRG_T0          0x0000
#define BRG_T2          0x0100
#define BRG_T4          0x0200
#define BRG_T5          0x0300
    
static cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data, CYG_ADDRWORD regs)
{
    char c;
    CYG_WORD16 disr;
    
    HAL_READ_UINT16( DIAG_SLDISR , disr );

    if( disr & 0x0001 )
    {

        disr = disr & ~0x0001;
    
        HAL_READ_UINT8( DIAG_RFIFO, c );
    
        HAL_WRITE_UINT16( DIAG_SLDISR , disr );
        
        if( cyg_hal_is_break( &c , 1 ) )
            cyg_hal_user_break( regs );
    }

    return 0;
}

    
static void hal_init_ctrlc_intr(void)
{
    CYG_WORD16 dicr;
    
    HAL_INTERRUPT_ATTACH( CYGNUM_HAL_INTERRUPT_SIO_0, hal_ctrlc_isr, NULL,
                          NULL );

    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_SIO_0 );

    HAL_READ_UINT16( DIAG_SLDICR , dicr );
    dicr = 0x0001;
    HAL_WRITE_UINT16( DIAG_SLDICR , dicr );    
}

#endif
#endif

/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
