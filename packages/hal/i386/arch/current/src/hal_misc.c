//===========================================================================
//
//      hal_misc.c
//
//      HAL miscellaneous functions
//
//===========================================================================
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
//===========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Contributors: proven, pjo
// Date:        1999-02-20
// Purpose:     HAL miscellaneous functions
// Description: This file contains miscellaneous functions provided by the
//              HAL.
//
//####DESCRIPTIONEND####
//
//===========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>      // diag_printf

#include <cyg/hal/hal_arch.h>

#include <cyg/hal/hal_if.h>

//---------------------------------------------------------------------------

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

    for (p = &__CTOR_END__[-1]; p >= __CTOR_LIST__; p--)
        (*p) ();
#endif
}

//---------------------------------------------------------------------------
// First level C exception handler.

externC void __handle_exception (void);
externC HAL_SavedRegisters *_hal_registers;

void
cyg_hal_exception_handler(HAL_SavedRegisters *regs)
{
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS) && !defined(CYGPKG_CYGMON)

    _hal_registers = regs;
    __handle_exception();
    
#elif defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT) && defined(CYGPKG_HAL_EXCEPTIONS)
    // We should decode the vector and pass a more appropriate
    // value as the second argument. For now we simply pass a
    // pointer to the saved registers. We should also divert
    // breakpoint and other debug vectors into the debug stubs.

    cyg_hal_deliver_exception( regs->vector>>8, (CYG_ADDRWORD)regs );

#else
    CYG_FAIL("Exception!!!");
#endif    
    return;
}

/*------------------------------------------------------------------------*/
/* default architecture ISR                                               */
/* The real default ISR is in hal/common/.../src/hal_misc.c               */

externC cyg_uint32 hal_arch_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    // For some reason I seem to be geting spurious interrupts on
    // interrupt 0x27(39). This is the parallel port. This is masked
    // in the PIC so the exact reason for these is a mystery. They do
    // appear to be real interrupts since they always appear just
    // after a clear of the interrupt flag, and the stack shows a
    // proper interrupt context being pushed by the hardware. This may
    // be some feature of the Celeron CPU I am using. 
    
    if( vector == 0x27 )
        return 2;

    diag_printf("hal_arch_default_isr: %d (data: %d)\n", vector,data);
    CYG_FAIL("Spurious Interrupt!!!");    
    
    return 0;
}

//---------------------------------------------------------------------------
// End of hal_misc.c
