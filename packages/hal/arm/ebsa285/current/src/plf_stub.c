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
// Author(s):   gthomas
// Contributors:gthomas, jskov
// Date:        1999-04-21
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_ebsa285.h>         // Hardware definitions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
static cyg_interrupt gdb_interrupt;
static cyg_handle_t  gdb_interrupt_handle;
#endif

//-----------------------------------------------------------------------------
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

// This ISR is called only for serial receive interrupts.
int 
cyg_hal_gdb_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_uint8 c;

    c = hal_ebsa285_get_char();  // Fetch the character
    if( 3 == c ) {  // ^C
        // Ctrl-C: set a breakpoint at PC so GDB will display the
        // correct program context when stopping rather than the
        // interrupt handler.
        cyg_hal_gdb_interrupt (regs->pc);
        cyg_drv_interrupt_acknowledge(CYG_GDB_DEV_INT);
    }
    return 0;  // No need to run DSR
}

int 
hal_ebsa285_interruptible(int state)
{
    if (state) {
        cyg_drv_interrupt_unmask(CYG_GDB_DEV_INT);
    } else {
        cyg_drv_interrupt_mask(CYG_GDB_DEV_INT);
    }

    return 0;
}
#endif

// Initialize the current serial port.
void hal_ebsa285_init_serial(void)
{
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    cyg_drv_interrupt_create(CYG_GDB_DEV_INT,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             cyg_hal_gdb_isr,
                             0,
                             &gdb_interrupt_handle,
                             &gdb_interrupt);
    cyg_drv_interrupt_attach(gdb_interrupt_handle);
    cyg_drv_interrupt_unmask(CYG_GDB_DEV_INT);
#endif
}

// Write C to the current serial port.
void hal_ebsa285_put_char(int c)
{
    // Wait for Tx FIFO not full
    while ((*SA110_UART_FLAG_REGISTER & SA110_TX_FIFO_STATUS_MASK) == SA110_TX_FIFO_BUSY)
        ;
    *SA110_UART_DATA_REGISTER = c;
}

// Read one character from the current serial port.
int hal_ebsa285_get_char(void)
{
    long timeout = 1000000000;  // A long time...
    int c;
    while ((*SA110_UART_FLAG_REGISTER & SA110_RX_FIFO_STATUS_MASK) == SA110_RX_FIFO_EMPTY)
        if ( --timeout == 0 )
            return -1;
    c = *SA110_UART_DATA_REGISTER & 0xFF;
    return c;
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
