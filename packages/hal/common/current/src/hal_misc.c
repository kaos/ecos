//==========================================================================
//
//      hal_misc.c
//
//      Common HAL miscellaneous functions
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
// Contributors: nickg, jskov
// Date:         2000-06-08
// Purpose:      HAL miscellaneous functions
// Description:  This file contains miscellaneous functions provided by the
//               HAL.
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_if.h>

#include <cyg/hal/hal_intr.h>
#include <cyg/hal/hal_misc.h>           // our header

#include <cyg/infra/cyg_type.h>         // Base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Macro for finding return address. 
#ifndef CYGARC_HAL_GET_RETURN_ADDRESS

#define CYGARC_HAL_GET_RETURN_ADDRESS(_x_) \
  (_x_) = (CYG_ADDRWORD)&&__backup_return_address

#define CYGARC_HAL_GET_RETURN_ADDRESS_BACKUP() \
__backup_return_address:

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
#if defined(CYGSEM_HAL_USE_ROM_MONITOR_GDB_stubs) \
    || defined(CYGSEM_HAL_USE_ROM_MONITOR_CygMon) \
    || defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)

    CYG_ADDRWORD __ra;
    CYG_WORD32 __pc;
    HAL_SavedRegisters *sreg = (HAL_SavedRegisters *)regs;

    CYGARC_HAL_GET_RETURN_ADDRESS(__ra);

    if( regs == NULL ) __pc = __ra;
    else __pc = sreg->pc;

    CYGACC_CALL_IF_INSTALL_BPT_FN((void *)__pc);

    CYGARC_HAL_GET_RETURN_ADDRESS_BACKUP();
    
#else

    HAL_BREAKPOINT(breakinst);

#endif
}


//--------------------------------------------------------------------------
// The system default interrupt ISR. It calls the architecture default
// ISR as well if necessary.
externC cyg_uint32 hal_arch_default_isr(CYG_ADDRWORD vector, 
                                        CYG_ADDRWORD data);

cyg_uint32
hal_default_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    cyg_uint32 result;

#if (defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)           \
     || defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)) &&    \
        (defined(CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT) ||     \
          defined(CYGHWR_HAL_GDB_PORT_VECTOR) &&           \
          defined(HAL_CTRLC_ISR))

#ifndef CYGIMP_HAL_COMMON_INTERRUPTS_CHAIN    
#if CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
    int gdb_vector = -1;
    // This check only to avoid crash on older stubs in case of unhandled
    // interrupts. It is a bit messy, but required in a transition period.
    if (CYGNUM_CALL_IF_TABLE_VERSION+1 == CYGACC_CALL_IF_VERSION()) {
        hal_virtual_comm_table_t* comm = CYGACC_CALL_IF_DEBUG_PROCS();
        gdb_vector = CYGACC_COMM_IF_CONTROL(*comm, __COMMCTL_DBG_ISR_VECTOR);
    }
    if( vector == gdb_vector )
#else
    // Old code using hardwired channels. This should go away eventually.
    if( vector == CYGHWR_HAL_GDB_PORT_VECTOR )
#endif
#endif        
    {
        result = HAL_CTRLC_ISR( vector, data );
        if( 0 != result ) return result;
    }
#endif

    result = hal_arch_default_isr (vector, data);
    if( 0 != result) return result;

    CYG_TRACE2(true, "Interrupt: %d, Data: 0x%08x", vector, data);
    CYG_FAIL("Spurious Interrupt!!!");
    return 0;
}


//--------------------------------------------------------------------------
// End of hal_misc.c
