//==========================================================================
//
//      hal_m68k.c
//
//      M68K HAL miscellaneous C functions
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2003, 2004, 2006, 2008 Free Software Foundation, Inc.      
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
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2003-06-04
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_m68k.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_stub.h>

// ----------------------------------------------------------------------------
// Interrupt support.
//
// Space for the interrupt data. These are updated by macros in hal_intr.h
volatile CYG_ADDRESS    cyg_hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRWORD   cyg_hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRESS    cyg_hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

// A status register value that should be used while running DSRs.
cyg_uint32  hal_m68k_dsr_ipl_level = 0x2000 | (CYGNUM_HAL_INTERRUPT_DEFAULT_IPL_LEVEL << 8);

// In virtual vector configurations we can get apparently spurious
// interrupts in application space when the interrupt will be serviced
// by RedBoot instead. Hence the virtual vector code in the common HAL
// provides hal_default_isr() which gets installed as the default
// handler for all interrupts. If it turns out that RedBoot does not
// recognize the interrupt either, i.e. it really is spurious, then
// the virtual vector code will call hal_arch_default_isr().
//
// In non-virtual vector configurations hal_default_isr() needs to
// be provided by the architecture/variant/processor/platform HAL
// on the off-chance that the interrupt is not entirely spurious
// and some useful processing can take place.

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
# ifndef _HAL_M68K_HAL_ARCH_DEFAULT_ISR_DEFINED_
cyg_uint32
hal_arch_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_FAIL("Spurious interrupt!");
    return 0;
}
# endif
#else
# ifndef _HAL_M68K_HAL_DEFAULT_ISR_DEFINED_
cyg_uint32
hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    CYG_FAIL("Spurious interrupt!");
    return 0;
}
# endif
#endif

// ----------------------------------------------------------------------------
// Exception handling. The assembler routine calls this C function as
// soon as possible. Usually exceptions are passed to gdb stubs - if
// this application does not have gdb stubs included then the relevant
// entries in the exception vector table will still belong to the stubs.


void
hal_m68k_exception_handler(HAL_SavedRegisters* regs)
{
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    
    // If we caught an exception inside the stubs, see if we were expecting it
    // and if so jump to the saved address
    extern void* volatile __mem_fault_handler;
    if (__mem_fault_handler) {
        regs->pc = (CYG_ADDRWORD)__mem_fault_handler;
        return; // Caught an exception inside stubs        
    }

    _hal_registers   = regs;
    __handle_exception();
    
#elif defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && defined(CYGPKG_HAL_EXCEPTIONS)
    
    CYG_WORD    code;
    HAL_CONTEXT_PCSR_GET_EXCEPTION(regs, code);
    cyg_hal_deliver_exception(code, (CYG_ADDRWORD) regs);
    
#else
    
    CYG_FAIL("Exception!!!");
    
#endif
}

// ----------------------------------------------------------------------------
// C++ constructor support. The constructors are run in a separate function
// so that a breakpoint can be set, at the cost of a couple of extra bytes
// of code.
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
cyg_bool    cyg_hal_stop_constructors   = 0;
#endif

typedef void    (*pfunc)(void);
extern  pfunc   __CTOR_LIST__[];
extern  pfunc   __CTOR_END__[];

void
cyg_hal_invoke_constructors(void)
{
#ifdef CYGSEM_HAL_STOP_CONSTRUCTORS_ON_FLAG
    static pfunc*   p           = &__CTOR_END__[-1];
    cyg_hal_stop_constructors   = 0;
    for ( ; p >= __CTOR_LIST__; p--) {
        (*p)();
        if (cyg_hal_stop_constructors) {
            p--;
            break;
        }
    }
#else
    pfunc*  p;
    for ( p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--) {
        (*p)();
    }
#endif        
}

// ----------------------------------------------------------------------------
// This entry point is called from vectors.S as soon as the C environment
// has been set up. We are running on the startup stack, interrupts are
// disabled, and the hardware is only minimally initialized. Most of
// the initialization will usually be done by the platform HAL.

externC void cyg_start(void);

void
hal_m68k_c_startup(void)
{
    int i;

    for (i = 0; i < CYGNUM_HAL_ISR_COUNT; i++) {
        cyg_hal_interrupt_handlers[i]   = (CYG_ADDRESS) &hal_default_isr;
    }

#ifdef HAL_M68K_VAR_INIT
    HAL_M68K_VAR_INIT();
#endif
#ifdef HAL_M68K_PROC_INIT
    HAL_M68K_PROC_INIT();
#endif
#ifdef HAL_M68K_PLATFORM_INIT    
    HAL_M68K_PLATFORM_INIT();
#endif    

#ifdef CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    hal_if_init();
#endif
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    initialize_stub();
#endif
#if defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT) || defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)
   {
       extern void hal_ctrlc_isr_init(void);
       hal_ctrlc_isr_init();
   }
#endif    
    
    cyg_hal_invoke_constructors();

#ifdef HAL_M68K_VAR_INIT2
    HAL_M68K_VAR_INIT2();
#endif
#ifdef HAL_M68K_PROC_INIT2
    HAL_M68K_PROC_INIT2();
#endif
#ifdef HAL_M68K_PLATFORM_INIT2
    HAL_M68K_PLATFORM_INIT2();
#endif
    
    // And call into application-level code
    cyg_start();
    for ( ; ; );
}
