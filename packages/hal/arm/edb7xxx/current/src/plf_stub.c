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
#include <cyg/hal/hal_edb7xxx.h>         // Hardware definitions
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif

#if CYGHWR_HAL_ARM_EDB7XXX_GDB_PORT == 0
#define CYG_GDB_DEV_DATA UARTDR1
#define CYG_GDB_DEV_BLCR UBLCR1
#define CYG_GDB_DEV_STAT SYSFLG1
#define CYG_GDB_DEV_CTRL SYSCON1
#define CYG_GDB_DEV_INT  CYGNUM_HAL_INTERRUPT_URXINT1
#else
#define CYG_GDB_DEV_DATA UARTDR2
#define CYG_GDB_DEV_STAT SYSFLG2
#define CYG_GDB_DEV_CTRL SYSCON2
#define CYG_GDB_DEV_BLCR UBLCR2
#define CYG_GDB_DEV_INT  CYGNUM_HAL_INTERRUPT_URXINT2
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

    c = hal_edb7xxx_get_char();  // Fetch the character
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
hal_edb7xxx_interruptible(int state)
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
void hal_edb7xxx_init_serial(void)
{
    // Enable port
    *(volatile cyg_uint32 *)CYG_GDB_DEV_CTRL |= SYSCON1_UART1EN;
    // Configure
    *(volatile cyg_uint32 *)CYG_GDB_DEV_BLCR = 
        UART_BITRATE(CYGHWR_HAL_ARM_EDB7XXX_GDB_BAUD) |
        UBLCR_FIFOEN | UBLCR_WRDLEN8;

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
void hal_edb7xxx_put_char(int c)
{
    cyg_uint32 stat;
    // Wait for Tx FIFO not full
    do {
        stat = *(volatile cyg_uint32 *)CYG_GDB_DEV_STAT;
    } while ((stat & SYSFLG1_UTXFF1) != 0) ;
    *(volatile cyg_uint32 *)CYG_GDB_DEV_DATA = c;
}

// Read one character from the current serial port.
int hal_edb7xxx_get_char(void)
{
    cyg_uint32 stat, val;
    do {
        stat = *(volatile cyg_uint32 *)CYG_GDB_DEV_STAT;
    } while ((stat & SYSFLG1_URXFE1) != 0);
    // Need to read 32 bits
    val = *(volatile cyg_uint32 *)CYG_GDB_DEV_DATA & 0xFF;
    return val;
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
