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
// Author(s):    jskov
// Contributors: jskov,nickg, jlarmour
// Date:         1999-10-13
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // Base types

#include <cyg/hal/hal_arch.h>           // architectural definitions

#include <cyg/hal/hal_intr.h>           // Interrupt handling

#include <cyg/hal/hal_cache.h>          // Cache handling

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/hal_stub.h>           // stub functionality
#endif

//--------------------------------------------------------------------------
// Functions to support the detection and execution of a user provoked
// program break. These are usually called from interrupt routines.

cyg_bool
cyg_hal_is_break(char *buf, int size)
{
    while( size )
        if( buf[--size] == 0x03 ) return true;

    return false;
}

void
cyg_hal_user_break( CYG_ADDRWORD *regs )
{
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    HAL_SavedRegisters *__sreg = (HAL_SavedRegisters *)regs;
    target_register_t __pc;

    if (__sreg)
        __pc = (target_register_t) __sreg->pc;
    else
        __pc = (target_register_t) &&safe_breakpoint;
    
    // Note: This would be better for the else case but doesn't work
    // at the moment.  CR: 101357
    // __pc = (target_register_t) __builtin_return_address(0);

    cyg_hal_gdb_interrupt (__pc);

 safe_breakpoint:
#elif defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs)

    // Note: Need to communicate to the ROM stub instead
    HAL_BREAKPOINT(breakinst);

#else

    HAL_BREAKPOINT(breakinst);

#endif

}
