//==========================================================================
//
//        cf_test.c
//
//        SA1110/Assabet - Compact Flash test
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
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          2000-06-05
// Description:   Tool used to test CF stuff
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros

#include <cyg/hal/hal_sa11x0.h>  // Board definitions
#include <cyg/hal/assabet.h>

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;

static cyg_interrupt cf_interrupt;
static cyg_handle_t  cf_interrupt_handle;

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

// This ISR is called when a CompactFlash board is inserted
static int
cf_detect_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_interrupt_mask(SA1110_CF_DETECT);
    return (CYG_ISR_HANDLED|CYG_ISR_CALL_DSR);  // Run the DSR
}

// This DSR handles the board insertion
static void
cf_detect_dsr(cyg_vector_t vector, cyg_ucount32 count, cyg_addrword_t data)
{
    unsigned long new_state = *SA11X0_GPIO_PIN_LEVEL;
    diag_printf("CF card inserted, state: %x\n", new_state);
    cyg_interrupt_acknowledge(SA1110_CF_DETECT);
    cyg_interrupt_unmask(SA1110_CF_DETECT);
}

static void
cf_test(cyg_addrword_t p)
{
    unsigned long cf_state;
    diag_printf("CF test here\n");
    // Disable CF bus & power (idle/off)
    assabet_BCR(SA1110_BCR_CF_POWER |
                SA1110_BCR_CF_RESET |
                SA1110_BCR_CF_BUS,
                SA1110_BCR_CF_POWER_OFF |
                SA1110_BCR_CF_RESET_DISABLE |
                SA1110_BCR_CF_BUS_OFF);
    while (1) {
        cf_state = *SA11X0_GPIO_PIN_LEVEL;
        if ((cf_state & SA1110_GPIO_CF_DETECT) == SA1110_GPIO_CF_PRESENT) {
            diag_printf("Compact Flash card inserted!\n");
            diag_dump_buf(0x30000000, 0x100);
            break;
        }
    }
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Set up interrupts
    cyg_interrupt_create(SA1110_CF_DETECT,
                         99,                     // Priority - what goes here?
                         0,                      //  Data item passed to interrupt handler
                         (cyg_ISR_t *)cf_detect_isr,
                         (cyg_DSR_t *)cf_detect_dsr,
                         &cf_interrupt_handle,
                         &cf_interrupt);
    cyg_interrupt_attach(cf_interrupt_handle);
    cyg_interrupt_configure(SA1110_CF_DETECT, true, true);  // Detect either edge
    cyg_interrupt_acknowledge(SA1110_CF_DETECT);
    cyg_interrupt_unmask(SA1110_CF_DETECT);
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      cf_test,           // entry
                      0,                 // entry parameter
                      "CompactFlash test",  // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()
