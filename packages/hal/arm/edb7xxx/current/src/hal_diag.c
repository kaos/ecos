/*=============================================================================
//
//      hal_diag.c
//
//      HAL diagnostic output code
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
// Author(s):   nickg, gthomas
// Contributors:nickg, gthomas
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/system.h>
#include CYGBLD_HAL_PLATFORM_H

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // basic machine info
#include <cyg/hal/hal_intr.h>           // interrupt macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_stub.h>           // cyg_hal_gdb_interrupt
#endif
#include <cyg/hal/hal_edb7xxx.h>         // Hardware definitions

#if CYGHWR_HAL_ARM_EDB7XXX_DIAG_PORT == 0
#define CYG_DIAG_DEV_DATA UARTDR1
#define CYG_DIAG_DEV_BLCR UBLCR1
#define CYG_DIAG_DEV_STAT SYSFLG1
#define CYG_DIAG_DEV_CTRL SYSCON1
#define CYG_DIAG_DEV_INT  CYGNUM_HAL_INTERRUPT_URXINT1
#else
#define CYG_DIAG_DEV_DATA UARTDR2
#define CYG_DIAG_DEV_STAT SYSFLG2
#define CYG_DIAG_DEV_CTRL SYSCON2
#define CYG_DIAG_DEV_BLCR UBLCR2
#define CYG_DIAG_DEV_INT  CYGNUM_HAL_INTERRUPT_URXINT2
#endif

// Assumption: all diagnostic output must be GDB packetized unless this is a ROM (i.e.
// totally stand-alone) system.

#ifdef CYGSEM_HAL_ROM_MONITOR
#define CYG_HAL_STARTUP_ROM
#undef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#endif

#if defined(CYG_HAL_STARTUP_ROM) && !defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#define HAL_DIAG_USES_HARDWARE
#else
#if defined(CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL)
#define HAL_DIAG_USES_HARDWARE
#elif CYGHWR_HAL_ARM_EDB7XXX_DIAG_PORT != CYGHWR_HAL_ARM_EDB7XXX_GDB_PORT
#define HAL_DIAG_USES_HARDWARE
#endif
#endif

/*---------------------------------------------------------------------------*/
// EDB7XXX Serial Port (UARTx) for Debug

// Actually send character down the wire
static void
hal_diag_write_char_serial(char c)
{
    cyg_uint32 stat;
    // Wait for Tx FIFO not full
    do {
        stat = *(volatile cyg_uint32 *)CYG_DIAG_DEV_STAT;
    } while ((stat & SYSFLG1_UTXFF1) != 0) ;
    *(volatile cyg_uint8 *)CYG_DIAG_DEV_DATA = c;
}

static bool
hal_diag_read_serial(char *c)
{
    long timeout = 1000000000;  // A long time...
    cyg_uint32 stat, val;

    do {
        stat = *(volatile cyg_uint32 *)CYG_DIAG_DEV_STAT;
        if (--timeout == 0) return false;
    } while ((stat & SYSFLG1_URXFE1) != 0);
    // Need to read 32 bits
    val = *(volatile cyg_uint32 *)CYG_DIAG_DEV_DATA & 0xFF;
    *c = val;
    return true;
}

#ifdef HAL_DIAG_USES_HARDWARE

void hal_diag_init(void)
{
    static int init = 0;
#ifndef CYG_HAL_STARTUP_ROM
    char *msg = "\n\rRAM EDB7XXX eCos\n\r";
#endif
    if (init++) return;

    // Enable port
    *(volatile cyg_uint32 *)CYG_DIAG_DEV_CTRL |= SYSCON1_UART1EN;
    // Configure
    *(volatile cyg_uint32 *)CYG_DIAG_DEV_BLCR = 
        UART_BITRATE(CYGHWR_HAL_ARM_EDB7XXX_DIAG_BAUD) |
        UBLCR_FIFOEN | UBLCR_WRDLEN8;
#ifndef CYG_HAL_STARTUP_ROM
    while (*msg) hal_diag_write_char(*msg++);
#endif
}

#ifdef DEBUG_DIAG
#ifndef CYG_HAL_STARTUP_ROM
#define DIAG_BUFSIZE 2048
static char diag_buffer[DIAG_BUFSIZE];
static int diag_bp = 0;
#endif
#endif

void hal_diag_write_char(char c)
{
    hal_diag_init();
#ifdef DEBUG_DIAG
#ifndef CYG_HAL_STARTUP_ROM
    diag_buffer[diag_bp++] = c;
    if (diag_bp == sizeof(diag_buffer)) diag_bp = 0;
#endif
#endif
    hal_diag_write_char_serial(c);
}

void hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

#else // HAL_DIAG relies on GDB

// Initialize diag port
void hal_diag_init(void)
{
#if 0  // Assume port is already setup
    // Enable port
    *(volatile cyg_uint32 *)CYG_DIAG_DEV_CTRL |= SYSCON1_UART1EN;
    // Configure
    *(volatile cyg_uint32 *)CYG_DIAG_DEV_BLCR = 
        UART_BITRATE(CYGHWR_HAL_ARM_EDB7XXX_DIAG_BAUD) |
        UBLCR_FIFOEN | UBLCR_WRDLEN8;
#endif
}

void 
hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

void 
hal_diag_write_char(char c)
{
    static char line[100];
    static int pos = 0;

    // No need to send CRs
    if( c == '\r' ) return;

    line[pos++] = c;

    if( c == '\n' || pos == sizeof(line) )
    {
        
        CYG_INTERRUPT_STATE old;

        // Disable interrupts. This prevents GDB trying to interrupt us
        // while we are in the middle of sending a packet. The serial
        // receive interrupt will be seen when we re-enable interrupts
        // later.
        
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_ENTER_CRITICAL_IO_REGION(old);
#else
        HAL_DISABLE_INTERRUPTS(old);
#endif

        while(1)
        {
            static char hex[] = "0123456789ABCDEF";
            cyg_uint8 csum = 0;
            int i;
            char c1;
        
            hal_diag_write_char_serial('$');
            hal_diag_write_char_serial('O');
            csum += 'O';
            for( i = 0; i < pos; i++ )
            {
                char ch = line[i];
                char h = hex[(ch>>4)&0xF];
                char l = hex[ch&0xF];
                hal_diag_write_char_serial(h);
                hal_diag_write_char_serial(l);
                csum += h;
                csum += l;
            }
            hal_diag_write_char_serial('#');
            hal_diag_write_char_serial(hex[(csum>>4)&0xF]);
            hal_diag_write_char_serial(hex[csum&0xF]);

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.  This is the reason for this clause
            // being a loop.
            if (!hal_diag_read_serial(&c1))
                continue;   // No response - try sending packet again

            if( c1 == '+' )
                break;              // a good acknowledge

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
            cyg_drv_interrupt_acknowledge(CYG_DIAG_DEV_INT);
            if( c1 == 3 ) {
                // Ctrl-C: breakpoint.
                cyg_hal_gdb_interrupt (__builtin_return_address(0));
                break;
            }
#endif
            // otherwise, loop round again
        }
        
        pos = 0;

        
        // And re-enable interrupts
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
        CYG_HAL_GDB_LEAVE_CRITICAL_IO_REGION(old);
#else
        HAL_RESTORE_INTERRUPTS(old);
#endif
        
    }
}
#endif

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
