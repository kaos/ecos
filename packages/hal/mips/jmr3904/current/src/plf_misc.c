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

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

#if !defined(CYGHWR_HAL_MIPS_TX39_JMR3904_ENABLE_TOE)
    
    HAL_TX39_DEBUG_TOE_DISABLE();

#else

    HAL_TX39_DEBUG_TOE_ENABLE();

#endif

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)    
        {
            void hal_ctrlc_isr_init(void);
            hal_ctrlc_isr_init();
        }

#endif
        
#if defined(CYGFUN_HAL_COMMON_KERNEL_SUPPORT)   && \
    defined(CYGSEM_HAL_USE_ROM_MONITOR_CygMon)
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
#if defined(CYGSEM_HAL_USE_ROM_MONITOR_CygMon)
    // The following code should be at the very start of this function so
    // that it can access the RA register before it is saved and reused.
    register CYG_WORD32 ra;
    asm volatile ( "move %0,$31;" : "=r" (ra) );

        {
            typedef void install_bpt_fn(void *epc);
            CYG_WORD32 pc;
            HAL_SavedRegisters *sreg = (HAL_SavedRegisters *)regs;
            install_bpt_fn *ibp = (install_bpt_fn *)hal_vsr_table[35];

            if( regs == NULL ) pc = ra;
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

#if defined(CYGDBG_HAL_MIPS_DEBUG_GDB_CTRLC_SUPPORT)

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

struct Hal_SavedRegisters *hal_saved_interrupt_state;

void hal_ctrlc_isr_init(void)
{
    CYG_WORD16 dicr;
    
    HAL_READ_UINT16( DIAG_SLDICR , dicr );
    dicr = 0x0001;
    HAL_WRITE_UINT16( DIAG_SLDICR , dicr );

    HAL_INTERRUPT_SET_LEVEL( CYGHWR_HAL_GDB_PORT_VECTOR, 1 );
    HAL_INTERRUPT_UNMASK( CYGHWR_HAL_GDB_PORT_VECTOR );
}

cyg_uint32 hal_ctrlc_isr(CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    char c;
    CYG_WORD16 disr;

    HAL_INTERRUPT_ACKNOWLEDGE( CYGHWR_HAL_GDB_PORT_VECTOR );
    
    HAL_READ_UINT16( DIAG_SLDISR , disr );

    if( disr & 0x0001 )
    {

        disr = disr & ~0x0001;
    
        HAL_READ_UINT8( DIAG_RFIFO, c );
    
        HAL_WRITE_UINT16( DIAG_SLDISR , disr );
        
        if( cyg_hal_is_break( &c , 1 ) )
            cyg_hal_user_break( (CYG_ADDRWORD *)hal_saved_interrupt_state );
    }

    return 2;
}

#endif

/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
