/*==========================================================================
//
//      hal_misc.c
//
//      HAL miscellaneous functions
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2005 eCosCentric Ltd.
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
//
// Alternative licenses for eCos may be arranged by contacting Red Hat, Inc.
// at http://sources.redhat.com/ecos/ecos-license/
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: nickg, jlarmour
// Date:         1999-02-18
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_trac.h>

#include <cyg/hal/hal_arch.h>

#include <cyg/hal/hal_intr.h>

#if 0
void trace( CYG_ADDRWORD tag, CYG_ADDRWORD a1, CYG_ADDRWORD a2)
{
    CYG_ADDRWORD **pp = (CYG_ADDRWORD **)0x48100000;
    CYG_ADDRWORD *ix = (CYG_ADDRWORD *)0x4810000C;
    CYG_ADDRWORD *p = *pp;
    *p++ = tag;
    *ix = *ix + 1;
    *p++ = *ix;
    *p++ = a1;
    *p++ = a2;
    *pp = p;
}
#endif

/*------------------------------------------------------------------------*/

#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool cyg_hal_stop_constructors;
#endif

void
cyg_hal_invoke_constructors(void)
{
    typedef void (*pfunc) (void);
    extern pfunc __CTOR_LIST__[];
    extern pfunc __CTOR_END__[];

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

    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
        (*p) ();
#endif

} // cyg_hal_invoke_constructors()

/*------------------------------------------------------------------------*/
// Default ISR
externC cyg_uint32
hal_arch_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    return 0;
}

//--------------------------------------------------------------------------
/* Determine the index of the ls bit of the supplied mask.                */

cyg_uint32
hal_lsbit_index(cyg_uint32 mask)
{
    int bit = -1;
    
    if (mask == 0)
	return -1;

    if ((mask & 0xffff) == 0) {
	mask >>= 16;
	bit += 16;
    }
    if ((mask & 0xff) == 0) {
	mask >>= 8;
	bit += 8;
    }

    __asm__("1:\n\t"
	    "adds #1,%0\n\t"
	    "shlr.b %w2\n\t"
	    "bcc 1b\n\t"
	    :"=r"(bit):"0"(bit),"r"(mask));

    return bit;
}

/*------------------------------------------------------------------------*/
/* Determine the index of the ms bit of the supplied mask.                */

cyg_uint32
hal_msbit_index(cyg_uint32 mask)
{
    unsigned int bit = 8;
    
    if (mask == 0)
	return -1;

    if ((mask & ~0xffff) != 0)
	mask >>= 16;
    else
	bit += 16;

    if ((mask & 0xff00) != 0)
	mask >>= 8;
    else
	bit += 8;

    __asm__("1:\n\t"
	    "dec.b %w0\n\t"
	    "shll.b %w2\n\t"
	    "bcc 1b\n\t"
	    :"=r"(bit):"0"(bit),"r"(mask));

    return bit;
}

/*------------------------------------------------------------------------*/
/* First level C exception handler.                                       */

externC void __handle_exception (void);

externC HAL_SavedRegisters *_hal_registers;

void
cyg_hal_exception_handler(HAL_SavedRegisters *regs,CYG_WORD vector)
{
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

    // Set the pointer to the registers of the current exception
    // context. At entry the GDB stub will expand the
    // HAL_SavedRegisters structure into a (bigger) register array.
    _hal_registers = regs;

    __handle_exception();

#endif
#if defined(CYGPKG_HAL_EXCEPTIONS)

    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

    cyg_hal_deliver_exception( vector, (CYG_ADDRWORD)regs );

#endif

    return;
}

/*------------------------------------------------------------------------*/
/* default ISR                                                            */

#ifndef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
externC cyg_uint32 hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT) && \
    defined(CYGHWR_HAL_GDB_PORT_VECTOR) &&         \
    defined(HAL_CTRLC_ISR)

#ifndef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN    
    if( vector == CYGHWR_HAL_GDB_PORT_VECTOR )
#endif        
    {
        cyg_uint32 result = HAL_CTRLC_ISR( vector, data );
        if( result != 0 ) return result;
    }
    
#endif
    
    CYG_TRACE1(true, "Interrupt: %d", vector);
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}
#endif

/*------------------------------------------------------------------------*/
/* Idle thread activity.                                                  */
   
externC void hal_idle_thread_action(cyg_uint32 loop_count)
{
}

/*------------------------------------------------------------------------*/
/* End of hal_misc.c                                                      */
