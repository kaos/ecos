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
//              Travis C. Furrer <furrer@mit.edu>
// Date:        2000-05-08
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
#include <cyg/hal/hal_sa11x0.h>         // Hardware definitions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
static cyg_interrupt gdb_interrupt;
static cyg_handle_t  gdb_interrupt_handle;
#endif

#if CYGHWR_HAL_ARM_SA11X0_GDB_PORT == 0
#define CYG_GDB_DEV_UTCR0       SA11X0_UART1_CONTROL0
#define CYG_GDB_DEV_UTCR1       SA11X0_UART1_CONTROL1
#define CYG_GDB_DEV_UTCR2       SA11X0_UART1_CONTROL2
#define CYG_GDB_DEV_UTCR3       SA11X0_UART1_CONTROL3
#define CYG_GDB_DEV_UTDR        SA11X0_UART1_DATA
#define CYG_GDB_DEV_UTSR0       SA11X0_UART1_STATUS0
#define CYG_GDB_DEV_UTSR1       SA11X0_UART1_STATUS1
#define CYG_GDB_DEV_INT         CYGNUM_HAL_INTERRUPT_UART1
#else
#define CYG_GDB_DEV_UTCR0       SA11X0_UART3_CONTROL0
#define CYG_GDB_DEV_UTCR1       SA11X0_UART3_CONTROL1
#define CYG_GDB_DEV_UTCR2       SA11X0_UART3_CONTROL2
#define CYG_GDB_DEV_UTCR3       SA11X0_UART3_CONTROL3
#define CYG_GDB_DEV_UTDR        SA11X0_UART3_DATA
#define CYG_GDB_DEV_UTSR0       SA11X0_UART3_STATUS0
#define CYG_GDB_DEV_UTSR1       SA11X0_UART3_STATUS1
#define CYG_GDB_DEV_INT         CYGNUM_HAL_INTERRUPT_UART3
#endif


//-----------------------------------------------------------------------------
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

// This ISR is called only for serial receive interrupts.  This is true because
// we do not enable Tx FIFO interrupts (in hal_sa11x0_init_serial, below).
static int ISR_count, ISR_c;
int 
cyg_hal_gdb_isr(cyg_vector_t vector, cyg_addrword_t data, HAL_SavedRegisters *regs)
{
    cyg_uint8 c;
    c = hal_sa11x0_get_char();  // Fetch the character
    ISR_count++;  ISR_c = c;
    if( 3 == c ) {  // ^C
        // Ctrl-C: set a breakpoint at PC so GDB will display the
        // correct program context when stopping rather than the
        // interrupt handler.
        cyg_hal_gdb_interrupt (regs->pc);
        // Clear receiver idle status bit, to allow another interrupt to
        // occur in the case where the receive fifo is almost empty.
        *CYG_GDB_DEV_UTSR0 = SA11X0_UART_RX_IDLE;
    }
    cyg_drv_interrupt_acknowledge(CYG_GDB_DEV_INT);
    return 0;  // No need to run DSR
}

int 
hal_sa11x0_interruptible(int state)
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
void hal_sa11x0_init_serial(unsigned long baudRate)
{
    cyg_uint32 brd;

    if (baudRate == 0) baudRate = CYGHWR_HAL_ARM_SA11X0_GDB_BAUD;

    // Disable Receiver and Transmitter (clears FIFOs)
    *CYG_GDB_DEV_UTCR3 = SA11X0_UART_RX_DISABLED |
                         SA11X0_UART_TX_DISABLED;

    // Clear sticky (writable) status bits.
    *CYG_GDB_DEV_UTSR0 = SA11X0_UART_RX_IDLE |
                         SA11X0_UART_RX_BEGIN_OF_BREAK |
                         SA11X0_UART_RX_END_OF_BREAK;

#if 0 // later
#if CYGHWR_HAL_ARM_SA11X0_GDB_PORT == 1

    //Set pin 14 as an output (Tx) and pin 15 as in input (Rx).
    *SA11X0_REG_GPDR = (*SA11X0_REG_GPDR | SA11X0_GPIO_14) & ~SA11X0_GPIO_15;

    // Use GPIO 14 & 15 pins for serial port 1.
    *SA11X0_REG_GAFR |= SA11X0_GPIO_14 | SA11X0_GPIO_15;

    // Pin reassignment for serial port 1.
    *SA11X0_REG_PPAR |= SA11X0_UART_PIN_REASSIGNMENT_MASK;

#endif
#endif

    // Set UART to 8N1 (8 data bits, no partity, 1 stop bit)
    *CYG_GDB_DEV_UTCR0 = SA11X0_UART_PARITY_DISABLED |
                         SA11X0_UART_STOP_BITS_1 |
                         SA11X0_UART_DATA_BITS_8;

    // Set the desired baud rate.
    brd = SA11X0_UART_BAUD_RATE_DIVISOR(baudRate);
    *CYG_GDB_DEV_UTCR1 = (brd >> 8) & SA11X0_UART_H_BAUD_RATE_DIVISOR_MASK;
    *CYG_GDB_DEV_UTCR2 = brd & SA11X0_UART_L_BAUD_RATE_DIVISOR_MASK;

    // Enable the receiver (with interrupts) and the transmitter.
    *CYG_GDB_DEV_UTCR3 = SA11X0_UART_RX_ENABLED |
                         SA11X0_UART_TX_ENABLED |
                         SA11X0_UART_RX_FIFO_INT_ENABLED;

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
void hal_sa11x0_put_char(int c)
{
    // Wait for Tx FIFO not full
    while ((*CYG_GDB_DEV_UTSR1 & SA11X0_UART_TX_FIFO_NOT_FULL) == 0)
        ;
    *CYG_GDB_DEV_UTDR = c;
}

// Read one character from the current serial port.
// FIXME: shouldn't we check for PARITY_ERROR, FRAMING_ERROR, or
// RECEIVE_FIFO_OVERRUN_ERROR in the received data?  This
// means check the appropriate bits in UTSR1.
int hal_sa11x0_get_char(void)
{
    long timeout = 1000000000;  // A long time...
    int c;
    // If receive fifo is empty, block and wait for a new char.
    while ((*CYG_GDB_DEV_UTSR1 & SA11X0_UART_RX_FIFO_NOT_EMPTY) == 0)
        if ( --timeout == 0 )
            return -1;
    c = *CYG_GDB_DEV_UTDR;
    // Clear receiver idle status bit, to allow another interrupt to
    // occur in the case where the receive fifo is almost empty.
    *CYG_GDB_DEV_UTSR0 = SA11X0_UART_RX_IDLE;
    return c;
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
