//==========================================================================
//
//      fiq.c
//
//      FIQ interrupt test
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002, 2012 Free Software Foundation, Inc.
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
//#####DESCRIPTIONBEGIN####
//
// Author(s):    nickg
// Date:         2012-04-20
// Purpose:      
// Description:  FIQ test
//
//####DESCRIPTIONEND####
//
//==========================================================================

#include <pkgconf/system.h>
#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>

#include <cyg/infra/testcase.h>

#include <cyg/kernel/kapi.h>

//==========================================================================

volatile int fiq_count = 0;

void __attribute__ ((interrupt ("FIQ"))) fiq_vsr( void )
{
    fiq_count++;

    // Clear interrupt
    HAL_INTERRUPT_ACKNOWLEDGE( CYGNUM_HAL_INTERRUPT_EINT0 );
}

//==========================================================================

void fiq_test(CYG_ADDRWORD p)
{
    volatile int fiq_count0 = 0;
    volatile int fiq_count1 = 0;
    volatile cyg_uint32 val1 = 0;
    cyg_uint32 pinsel = CYGARC_HAL_LPC24XX_REG_PIN_BASE;
    cyg_uint32 pinsel4;
    cyg_uint32 vic = CYGARC_HAL_LPC24XX_REG_VIC_BASE;
    cyg_uint32 intsel;
    cyg_uint32 gpio = CYGARC_HAL_LPC24XX_REG_FIO_BASE;

    // Configure P2.10 as EINT0
    HAL_READ_UINT32( pinsel+CYGARC_HAL_LPC24XX_REG_PINSEL4, pinsel4 );
    pinsel4 &= ~(3<<20);
    pinsel4 |= (1<<20);
    HAL_WRITE_UINT32( pinsel+CYGARC_HAL_LPC24XX_REG_PINSEL4, pinsel4 );

    // Configure EINT0 line for falling edge detection
    HAL_INTERRUPT_CONFIGURE( CYGNUM_HAL_INTERRUPT_EINT0, false, false );

#if 1 
    // Route EINT0 to FIQ input
    HAL_READ_UINT32( vic+CYGARC_HAL_LPC24XX_REG_VICINTSELECT, intsel );
    intsel |= (1<<CYGNUM_HAL_INTERRUPT_EINT0);
    HAL_WRITE_UINT32( vic+CYGARC_HAL_LPC24XX_REG_VICINTSELECT, intsel );    
#endif
    
    // Install and unmask FIQ VSR
    HAL_VSR_SET( CYGNUM_HAL_VECTOR_FIQ, fiq_vsr, NULL );
    HAL_INTERRUPT_UNMASK( CYGNUM_HAL_INTERRUPT_EINT0 );

    CYG_TEST_INFO("FIQ test starting");
        
    // Enable FIQs
    HAL_ENABLE_FIQ();

    while(fiq_count1 < 20)
    {
        cyg_uint32 val;
        cyg_uint8 pin;

        HAL_READ_UINT8( gpio+CYGARC_HAL_LPC24XX_REG_FIO2PIN+1, pin );
        val = 1&(pin>>2);
        
        if( val != val1 )
        {
            cyg_uint32 qf;
            HAL_QUERY_FIQ( qf );            
            //diag_printf("P2.10 %d fiq_count %d qf %08x\n", val, fiq_count, qf );
            val1 = val;
        }


        if( fiq_count0 != fiq_count )
        {
            int dbcount = 10000;
            int i = dbcount;

            // Crude debouncing: loop here while fiq_count is
            // changing, only continue when it has been stable for
            // dbcount loops.
            while( i-- )
            {
                if( fiq_count0 != fiq_count )
                {
                    fiq_count0 = fiq_count;
                    i = dbcount;
                }
            }
            
            fiq_count1++;
            diag_printf("FIQ count %3d debounced %2d\n", fiq_count, fiq_count1 );
        }
    }

    CYG_TEST_PASS_FINISH("fiq");
}

//==========================================================================

#define STACK_SIZE CYGNUM_HAL_STACK_SIZE_TYPICAL
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

void
cyg_start(void)
{
    CYG_TEST_INIT();

    cyg_thread_create(10,                // Priority - just a number
                      fiq_test,          // entry
                      0,                 // entry parameter
                      "FIQ test",        // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
        );
    cyg_thread_resume(thread_handle);  /* Start it */
    cyg_scheduler_start();
}

//==========================================================================
// EOF
