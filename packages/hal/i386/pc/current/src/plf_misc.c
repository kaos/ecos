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
// Contributors: nickg, jlarmour, pjo
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

#include <cyg/hal/plf_misc.h>

#include <cyg/hal/hal_io.h>

#ifdef  CYGSEM_HAL_VIRTUAL_VECTOR_SUPPORT
#include <cyg/hal/hal_if.h>
#endif

#ifdef CYGPKG_KERNEL
#include <pkgconf/kernel.h>
#include <cyg/kernel/ktypes.h>
#include <cyg/kernel/kapi.h>
#endif /* CYGPKG_KERNEL */

/*------------------------------------------------------------------------*/

extern void patch_dbg_syscalls(void * vector);

extern void hal_pcmb_init(void);


//----------------------------------------------------------------------------
// ISR tables

volatile CYG_ADDRESS    hal_interrupt_handlers[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRWORD   hal_interrupt_data[CYGNUM_HAL_ISR_COUNT];
volatile CYG_ADDRESS    hal_interrupt_objects[CYGNUM_HAL_ISR_COUNT];

//-----------------------------------------------------------------------------
// IDT interrupt gate initialization

externC void cyg_hal_pc_set_idt_entry(CYG_ADDRESS routine,short *idtEntry)
{  
   
   idtEntry[0]=routine & 0xFFFF;
   idtEntry[1]=8;
   idtEntry[2]=0x8E00;
   idtEntry[3]=routine >> 16;
}

/*------------------------------------------------------------------------*/

void hal_platform_init(void)
{
    int vector;

    HAL_ICACHE_INVALIDATE_ALL();    
    HAL_ICACHE_ENABLE();
    HAL_DCACHE_INVALIDATE_ALL();
    HAL_DCACHE_ENABLE();

    // Call motherboard init function
    hal_pcmb_init();
    
    // ISR table setup: plant the default ISR in all interrupt handlers
    // and the default interrupt VSR in the equivalent VSR table slots.
    for (vector = CYGNUM_HAL_ISR_MIN; vector <= CYGNUM_HAL_ISR_MAX; vector++)        
    {
        cyg_uint32 index;
        HAL_TRANSLATE_VECTOR( vector, index );
        hal_interrupt_handlers[index] = (CYG_ADDRESS) HAL_DEFAULT_ISR;
        HAL_VSR_SET( vector, &__default_interrupt_vsr, NULL );
    }
    
#if !defined(CYG_HAL_STARTUP_RAM)
    for (vector = CYGNUM_HAL_EXCEPTION_MIN;
         vector <= CYGNUM_HAL_EXCEPTION_MAX;
         vector++)        
    {
        HAL_VSR_SET( vector, &__default_exception_vsr, NULL );
    }
#endif
    
#if defined(CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT)    
    {
        void hal_ctrlc_isr_init(void);
        hal_ctrlc_isr_init();
    }

#endif
        
#ifdef CYGPKG_REDBOOT

    // Start the timer device running if we are in a RedBoot
    // configuration. 
    
    HAL_CLOCK_INITIALIZE( CYGNUM_HAL_RTC_PERIOD );
    
#endif    
    
    hal_if_init();

    
}

/*------------------------------------------------------------------------*/

void hal_pc_reset(void)
{
    /* Use Intel's IDT triple-fault trick. */
    asm("movl $badIdt, %eax\n"
        "lidt (%eax)\n"
        "int $3\n"
        "hlt\n"

        ".align 4\n"

        "badIdt:\n"
        ".word		0\n"
        ".long		0\n"
        ) ;
}

/*------------------------------------------------------------------------*/

void 
hal_delay_us(int us)
{
    while( us > 0 )
    {
        cyg_uint32 us1 = us;
        cyg_int32 ticks;
        cyg_uint32 cval1, cval2;

        // Wait in bursts of 1s to avoid overflow problems with the
        // multiply by 1000 below.
        
        if( us1 > 1000000 )
            us1 = 1000000;

        us -= us1;
        
        // The PC clock ticks at 838ns per tick. So we convert the us
        // value we were given to clock ticks and wait for that many
        // to pass.

        ticks = (us1 * 1000UL) / 838UL;

        HAL_CLOCK_READ( &cval1 );

        // We just loop, waiting for clock ticks to happen,
        // and subtracting them from ticks when they do.
        
        while( ticks > 0 )
        {
            cyg_int32 diff;
            HAL_CLOCK_READ( &cval2 );

            diff = cval2 - cval1;

            // Cope with counter wrap-around.
            if( diff < 0 )
                diff += CYGNUM_HAL_RTC_PERIOD;

            ticks -= diff;
            cval1 = cval2;

        }
    }
}


/*------------------------------------------------------------------------*/
/* End of plf_misc.c                                                      */
