//=============================================================================
//
//      plf_stub.c
//
//      Platform specific code for GDB stub support.
//
//=============================================================================
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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, jskov (based on the old tx39 hal_stub.c)
// Contributors:nickg, jskov, gthomas
// Date:        2000-03-10
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_diag.h>           // diag output. FIXME
#include <cyg/hal/drv_api.h>

#include <cyg/hal/hal_intr.h>

#include <cyg/hal/v850_common.h>

/*---------------------------------------------------------------------------*/
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
static cyg_interrupt gdb_interrupt;
static cyg_handle_t  gdb_interrupt_handle;
#define CYG_DEVICE_SERIAL_INT CYGNUM_HAL_VECTOR_INTCSI1

// This ISR is called only for serial receive interrupts.
int 
cyg_hal_gdb_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    volatile unsigned char *RxDATA = (volatile unsigned char *)V850_REG_RXS0;
    cyg_uint8 c;    

    c = *RxDATA;  // Fetch the character
    if( 3 == c ) {  // ^C
        // Ctrl-C: set a breakpoint at PC so GDB will display the
        // correct program context when stopping rather than the
        // interrupt handler.
        cyg_hal_gdb_interrupt (regs->pc);
        cyg_drv_interrupt_acknowledge(CYG_DEVICE_SERIAL_INT);
    }
    return 0;  // No need to run DSR
}

int 
hal_plf_interruptible(int state)
{
    if (state) {
        cyg_drv_interrupt_unmask(CYG_DEVICE_SERIAL_INT);
    } else {
        cyg_drv_interrupt_mask(CYG_DEVICE_SERIAL_INT);
    }

    return 0;
}
#endif

void hal_plf_init_serial()
{
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    cyg_drv_interrupt_create(CYG_DEVICE_SERIAL_INT,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             cyg_hal_gdb_isr,
                             0,
                             &gdb_interrupt_handle,
                             &gdb_interrupt);
    cyg_drv_interrupt_attach(gdb_interrupt_handle);
    cyg_drv_interrupt_unmask(CYG_DEVICE_SERIAL_INT);
#endif
}

/*---------------------------------------------------------------------------*/

void hal_plf_put_char( int c)
{
    hal_diag_write_char_serial(c);
}

/*---------------------------------------------------------------------------*/

int hal_plf_get_char( void )
{
    volatile unsigned char *LED = (volatile unsigned char *)V850_REG_P10;
    char c;
    unsigned char led = *LED & 0x7F;
    if (led == 0) {
        led = 0x01;
    } else {
        led <<= 1;
    }
    *LED = 0x80 | led;
    hal_diag_read_char(&c);
    *LED &= 0x7F;
    return c;
}


//-----------------------------------------------------------------------------
// Stub init

void hal_plf_stub_init(void)
{
    // Set up watchdog timer since we have to use it to emulate a breakpoint
    volatile unsigned char *wdcs = (volatile unsigned char *)V850_REG_WDCS;
    volatile unsigned char *wdtm = (volatile unsigned char *)V850_REG_WDTM;
    *wdcs = 0x07;  // 246.7ms - lowest possible overhead
    *wdtm = 0x90;  // Start watchdog
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
