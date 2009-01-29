//==========================================================================
//
//      hal_misc.c
//
//      HAL miscellaneous functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2007 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros
#include <cyg/hal/hal_arch.h>           // HAL header
#include <cyg/hal/hal_intr.h>           // VSR/ISR defines
#include <cyg/hal/hal_misc.h>

/*------------------------------------------------------------------------*/
/* If required, define a variable to store the clock period.              */

#ifdef CYGHWR_HAL_CLOCK_PERIOD_DEFINED

CYG_WORD32 cyg_hal_clock_period;

#endif


/*****************************************************************************
hal_default_exception_handler -- First level C exception handler

     The assembly default VSR  handler calls  this routine  to handle  the
exception.  When this routine returns, the  state is restored to the  state
pointed to by regs.

     We declare this  routine as  weak so  that other  handlers can  easily
become the default exception handler.

INPUT:

     vector: The exception vector number.

     regs: A pointer to the saved state.

OUTPUT:

RETURN VALUE:

     None

*****************************************************************************/

externC void
hal_default_exception_handler(CYG_WORD vector, HAL_SavedRegisters *regs)
                                                    __attribute__ ((weak));

void hal_default_exception_handler(CYG_WORD vector, HAL_SavedRegisters *regs)
{

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    externC void __handle_exception(void);
    externC HAL_SavedRegisters * _hal_registers;

    // Set the pointer to the registers of the current exception
    // context. At entry the GDB stub will expand the
    // HAL_SavedRegisters structure into a (bigger) register array.
    _hal_registers = regs;

    __handle_exception();

#elif defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && \
      defined(CYGPKG_HAL_EXCEPTIONS)

    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

    cyg_hal_deliver_exception(vector, (CYG_ADDRWORD)regs);

#else

    CYG_FAIL("Exception!!!");

#endif

    return;
}

//---------------------------------------------------------------------------
// Default arch ISR

externC cyg_uint32
hal_arch_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}

//---------------------------------------------------------------------------
// Idle thread action

void
hal_idle_thread_action( cyg_uint32 count )
{
}

//-----------------------------------------------------------------------------
// Monitor initialization. This means to init the vsr vector table.
// TODO maybe init hal_vsr_table through defining it in assembler code

#ifndef CYGPKG_HAL_FR30_MON_DEFINED
void hal_mon_init(void){
    int i;
    /* 0 - 14 exceptions */
    for(i = 0; i < CYGNUM_HAL_VECTOR_INTRFIRST; i++){
        hal_vsr_table[i] = (CYG_ADDRESS)__default_exception_vsr;
    }
    /* 15 - xx interrupts */
    for( ; i < CYGNUM_HAL_VECTOR_INTRLAST; i++){
        hal_vsr_table[i] = (CYG_ADDRESS)__default_interrupt_vsr;
    }
    /*  xx+1 - 255 exceptions (fr30 traps) */
    for( ; i < CYGNUM_HAL_VSR_MAX; i++){
        hal_vsr_table[i] = (CYG_ADDRESS)__default_exception_vsr;
    }
}
#endif

//---------------------------------------------------------------------------
// Determine the index of the ls bit of the supplied mask.

cyg_uint32
hal_lsbit_index(cyg_uint32 mask)
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

//---------------------------------------------------------------------------
// Determine the index of the ms bit of the supplied mask. FIXME: since we
// have hardware support for it, use it! (bit search module)

cyg_uint32
hal_msbit_index(cyg_uint32 mask)
{
    cyg_uint32 x = mask;
    cyg_uint32 w;

    // Phase 1: make word with all ones from that one to the right.
    x |= x >> 16;
    x |= x >> 8;
    x |= x >> 4;
    x |= x >> 2;
    x |= x >> 1;

    // Phase 2: calculate number of "1" bits in the word.
    w = (x & 0x55555555) + ((x >> 1) & 0x55555555);
    w = (w & 0x33333333) + ((w >> 2) & 0x33333333);
    w = w + (w >> 4);
    w = (w & 0x000F000F) + ((w >> 8) & 0x000F000F);
    return (cyg_uint32)((w + (w >> 16)) & 0xFF);

}

/*------------------------------------------------------------------------*/
/* C++ support - run initial constructors                                 */

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

typedef void (*pfunc) (void);
extern pfunc __CTOR_LIST__[];
extern pfunc __CTOR_END__[];

void
        cyg_hal_invoke_constructors (void)
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc *p = &__CTOR_END__[-1];

    cyg_hal_stop_constructors = 0;
    for (; p >= __CTOR_LIST__; p--) {
        (*p) ();
        if (cyg_hal_stop_constructors) {
            p--;
            break;
        }
    }
#else
    pfunc *p;
    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--) {
        (*p) ();
    }
#endif
}

//---------------------------------------------------------------------------
