//==========================================================================
//
//        panel_test.c
//
//        Cirrus CL72xx eval board LCD touch panel test code
//
//==========================================================================
//####COPYRIGHTBEGIN####
//
// -------------------------------------------
// The contents of this file are subject to the Cygnus eCos Public License
// Version 1.0 (the "License"); you may not use this file except in
// compliance with the License.  You may obtain a copy of the License at
// http://sourceware.cygnus.com/ecos
// 
// Software distributed under the License is distributed on an "AS IS"
// basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
// License for the specific language governing rights and limitations under
// the License.
// 
// The Original Code is eCos - Embedded Cygnus Operating System, released
// September 30, 1998.
// 
// The Initial Developer of the Original Code is Cygnus.  Portions created
// by Cygnus are Copyright (C) 1998,1999 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     gthomas
// Contributors:  gthomas
// Date:          1999-05-15
// Description:   Tool used to test LCD touch panel
//####DESCRIPTIONEND####

#include <pkgconf/kernel.h>   // Configuration header
#include <cyg/kernel/kapi.h>
#include <cyg/infra/diag.h>

#include <cyg/hal/hal_cl7211.h>  // Board definitions

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_arch.h>           // Register state info
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support

#ifndef FALSE
#define FALSE 0
#define TRUE  1
#endif

#define STACK_SIZE 4096
static char stack[STACK_SIZE];
static cyg_thread thread_data;
static cyg_handle_t thread_handle;
static cyg_interrupt panel_interrupt;
static cyg_handle_t  panel_interrupt_handle;

#include "lcd_support.c"

#define SYNCIO_TXFRMEN     (1<<14)
#define SYNCIO_FRAMELEN(n) (n<<8)
#define ADC_START          (1<<7)
#define ADC_CHAN(n)        (n<<4)
#define ADC_UNIPOLAR       (1<<3)
#define ADC_SINGLE         (1<<2)
#define ADC_EXT_CLOCK      (3<<0)
#define TOUCH_CTL          KBD_PORT

// FUNCTIONS

static void
cyg_test_exit(void)
{
    while (TRUE) ;
}

static cyg_uint32
adc_sample(int chan)
{
    cyg_uint32 val;
    *(volatile cyg_uint32 *)SYNCIO = SYNCIO_TXFRMEN | SYNCIO_FRAMELEN(24) |
        ADC_START | ADC_CHAN(chan) | ADC_UNIPOLAR | ADC_SINGLE | ADC_EXT_CLOCK;
    while (*(volatile cyg_uint32 *)SYSFLG1 & SYSFLG1_SSIBUSY) ;
    val = *(volatile cyg_uint32 *)SYNCIO;
    return val & 0xFFFF;
}

static void
panel_delay(void)
{
    volatile int i;
    for (i = 0;  i < 50;  i++) ;
}

// This ISR is called when the touch panel interrupt occurs
int 
cyg_hal_panel_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    int iX, iY;
    cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_EINT2);
    diag_printf("Touched!\n");
    *(volatile cyg_uint8 *)TOUCH_CTL = 0x50;  // Drive TSPY, ground TSMY, and disconnect TSPX and TSMX
    panel_delay();
    iY = adc_sample(2);
    *(volatile cyg_uint8 *)TOUCH_CTL = 0xA0;  // Drive TSPX, ground TSMX, and disconnect TSPY and TSMY
    panel_delay();
    iX = adc_sample(7);
    diag_printf("X = %d, Y = %d, ISR1: %x\n", iX, iY, *(volatile cyg_uint32 *)INTSR1);
    *(volatile cyg_uint8 *)TOUCH_CTL = 0x00;  // Disable drives
    while (*(volatile cyg_uint32 *)INTSR1 & INTSR1_EINT2) ;
    *(volatile cyg_uint8 *)TOUCH_CTL = 0x70;  // Idle state (so interrupt works)
//    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT2);
    return 0;  // No need to run DSR
}

static void
panel_exercise(cyg_addrword_t p)
{
    int i, j;
    diag_printf("LCD touch panel test here!\n");

    *(volatile cyg_uint8 *)PEDDR   = 0x0F;  // Register E data direction
    *(volatile cyg_uint8 *)PEDR   |= 0x04;  // Enable touch panel

    diag_printf("PEDR = %x\n", *(volatile cyg_uint8 *)PEDR);

    *(volatile cyg_uint8 *)TOUCH_CTL = 0x70;  // Idle state (so interrupt works)

    lcd_on(4);
    lcd_clear();

    *(volatile cyg_uint32 *)SYSCON1 |= SYSCON1_ADC_CLOCK_128kHZ;

    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_EINT2,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             cyg_hal_panel_isr,
                             0,
                             &panel_interrupt_handle,
                             &panel_interrupt);
    cyg_drv_interrupt_attach(panel_interrupt_handle);
    cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_EINT2);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT2);

    for (i = 0;  i < 50;  i++) {
        diag_printf("Please touch screen now:\n");
        cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_EINT2);
        cyg_thread_delay(5*100);
    }

    diag_printf("All done!\n");
    cyg_test_exit();
}

externC void
cyg_start( void )
{
    // Create a main thread, so we can run the scheduler and have time 'pass'
    cyg_thread_create(10,                // Priority - just a number
                      panel_exercise,    // entry
                      0,                 // initial parameter
                      "LCD_panel_thread", // Name
                      &stack[0],         // Stack
                      STACK_SIZE,        // Size
                      &thread_handle,    // Handle
                      &thread_data       // Thread data structure
            );
    cyg_thread_resume(thread_handle);  // Start it
    cyg_scheduler_start();
} // cyg_package_start()

