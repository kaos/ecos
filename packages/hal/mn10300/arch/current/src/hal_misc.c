/*=============================================================================
//
//	hal_misc.c
//
//	HAL miscellaneous functions
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s): 	nickg
// Contributors:	nickg
// Date:	1997-11-12
// Purpose:	HAL miscellaneous functions
// Description: This file contains miscellaneous functions provided by the
//              HAL.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>

#include <cyg/hal/hal_arch.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/hal_cache.h>

/*---------------------------------------------------------------------------*/

void
cyg_hal_invoke_constructors (void)
{
    typedef void (*pfunc) (void);
    extern pfunc _CTOR_LIST__[];
    extern pfunc _CTOR_END__[];
    pfunc *p;
    
#ifdef CYG_KERNEL_USE_INIT_PRIORITY
    for (p = &_CTOR_END__[-1]; p >= _CTOR_LIST__; p--)
        (*p) ();
#else
    for (p = _CTOR_LIST__; p != _CTOR_END__; p++)
        (*p) ();
#endif        

#ifdef CYG_HAL_MN10300_STDEVAL1    

    // On the real hardware we also enable the cache.
    // doing this here is a temporary measure until we
    // have a proper platform specific place to do it.
    
    // Note that the hardware seems to come up with the
    // caches containing random data. Hence they must be
    // invalidated before being enabled.
    
    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

#endif

#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYG_HAL_USE_ROM_MONITOR)            && \
    defined(CYG_HAL_USE_ROM_MONITOR_CYGMON)
    {
        extern CYG_ADDRESS hal_virtual_vector_table[32];
        extern void patch_dbg_syscalls(void * vector);
#define DBG_SYSCALL_VEC_NUM 15
        patch_dbg_syscalls( (void *)(&hal_virtual_vector_table[DBG_SYSCALL_VEC_NUM]) );
    }
#endif

}

/*---------------------------------------------------------------------------*/
/* Determine the index of the ls bit of the supplied mask.                   */

cyg_uint32 hal_lsbit_index(cyg_uint32 mask)
{
    cyg_uint32 n = mask;

    static const signed char tab[64] =
    { -1, 0, 1, 12, 2, 6, 0, 13, 3, 0, 7, 0, 0, 0, 0, 14, 10,
      4, 0, 0, 8, 0, 0, 25, 0, 0, 0, 0, 0, 21, 27 , 15, 31, 11,
      5, 0, 0, 0, 0, 0, 9, 0, 0, 24, 0, 0 , 20, 26, 30, 0, 0, 0,
      0, 23, 0, 19, 29, 0, 22, 18, 28, 17, 16, 0
    };

    n &= ~(n-1UL);
    n = (n<<16)-n;
    n = (n<<6)+n;
    n = (n<<4)+n;

    return tab[n>>26];
}

/*---------------------------------------------------------------------------*/
/* Determine the index of the ms bit of the supplied mask.                   */

cyg_uint32 hal_msbit_index(cyg_uint32 mask)
{
    cyg_uint32 x = mask;    
    cyg_uint32 w;

    /* Phase 1: make word with all ones from that one to the right */
    x |= x >> 16;
    x |= x >> 8;
    x |= x >> 4;
    x |= x >> 2;
    x |= x >> 1;

    /* Phase 2: calculate number of "1" bits in the word        */
    w = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    w = w + (w >> 4);
    w = (w & 0x000F000F) + ((w >> 8) & 0x000F000F);
    return (cyg_uint32)((w + (w >> 16)) & 0xFF);

}

/*---------------------------------------------------------------------------*/
/* First level C exception handler.                                          */

void exception_handler(HAL_SavedRegisters *regs)
{
    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)
    
    CYG_WORD vector = regs->vector;

    if( vector == 0 )
    {
        // An NMI vector, decode it from the NMIGR
    
        cyg_uint32 nmigr = mn10300_interrupt_control[0];

        // Write back to clear interrupt bits
        mn10300_interrupt_control[0] = nmigr;
    
        if( nmigr & 0x0002 )
        {
            // Special case the watchdog timer exception, look for an
            // ISR for it and call it if present. Otherwise pass on to
            // the exception system.
        
            cyg_uint32 (*isr)(CYG_ADDRWORD,CYG_ADDRWORD);
            cyg_uint32 index;

            HAL_TRANSLATE_VECTOR( CYG_VECTOR_WATCHDOG, index );
        
            isr = (cyg_uint32 (*)(CYG_ADDRWORD,CYG_ADDRWORD))
                (hal_interrupt_handlers[index]);

            if( isr != 0 )
            {
                isr(CYG_VECTOR_WATCHDOG,
                    hal_interrupt_data[CYG_VECTOR_WATCHDOG]
                    );
                return;
            }
            vector = CYG_VECTOR_WATCHDOG;
        }
        // Check for system error
        else if( nmigr & 0x0004 ) vector = CYG_VECTOR_SYSTEM_ERROR; 

        // otherwise it is an NMI and vector is correct.
    }
    
#if defined(CYGPKG_HAL_EXCEPTIONS)

    deliver_exception( vector, (CYG_ADDRWORD)regs );

#endif

#endif
    
    return;
}

/*---------------------------------------------------------------------------*/
/* End of hal_misc.c                                                         */
