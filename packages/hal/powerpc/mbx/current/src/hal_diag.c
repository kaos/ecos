//=============================================================================
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
// Version 1.0 (the "License"); you may not use this file except in         
// compliance with the License.  You may obtain a copy of the License at    
// http://sourceware.cygnus.com/ecos                                        
//                                                                          
// Software distributed under the License is distributed on an       
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
// Author(s):   hmt
// Contributors:hmt
// Date:        1999-06-08
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_intr.h>           // Interrupt macros

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // hal_output_gdb_string
#endif

#include <cyg/hal/ppc_regs.h>
#include <cyg/hal/quicc/quicc_smc1.h>

//-----------------------------------------------------------------------------
// Select default diag channel to use

//#define CYG_KERNEL_DIAG_ROMART
//#define CYG_KERNEL_DIAG_SERIAL

#if !defined(CYG_KERNEL_DIAG_SERIAL)
#define CYG_KERNEL_DIAG_SERIAL
#endif

#ifdef CYGDBG_DIAG_BUF
// Keep diag messages in a buffer for later [re]display

int enable_diag_uart = 1;
int enable_diag_buf = 1;
static char diag_buf[40960*4];
static int  diag_buf_ptr = 0;

static void
diag_putc(char c)
{
    if (enable_diag_buf) {
        diag_buf[diag_buf_ptr++] = c;
        if (diag_buf_ptr == sizeof(diag_buf)) diag_buf_ptr--;
    }
}

void
dump_diag_buf(int start, int len)
{
    int i;
    enable_diag_uart = 1;
    enable_diag_buf = 0;
    if (len == 0) len = diag_buf_ptr;
    diag_printf("\nDiag buf\n");
    for (i = start;  i < len;  i++) {
        hal_diag_write_char(diag_buf[i]);
    }
}
#endif // CYGDBG_DIAG_BUF


//-----------------------------------------------------------------------------
// MBX board specific serial output; using GDB protocol by default:


#if defined(CYG_KERNEL_DIAG_SERIAL)

void hal_diag_init(void)
{
    static int init = 0;
    if (init) return;
    init++;
    // init the actual serial port
    cyg_quicc_init_smc1();
#ifndef CYG_HAL_STARTUP_ROM
    // We are talking to GDB; ack the "go" packet!
    cyg_quicc_smc1_uart_putchar('+');
#endif
}

void hal_diag_write_char_serial( char c )
{
    unsigned long __state;
    HAL_DISABLE_INTERRUPTS(__state)
    cyg_quicc_smc1_uart_putchar(c);
    HAL_RESTORE_INTERRUPTS(__state);
}

#ifdef CYG_HAL_STARTUP_ROM
void hal_diag_write_char(char c)
{
#ifdef CYGDBG_DIAG_BUF
    diag_putc(c);
    if (!enable_diag_uart) return;
#endif // CYGDBG_DIAG_BUF
    hal_diag_write_char_serial(c);
}

#else // RAM start so encode for GDB

void hal_diag_write_char(char c)
{
    static char line[100];
    static int pos = 0;

#ifdef CYGDBG_DIAG_BUF
    diag_putc(c);
    if (!enable_diag_uart) return;
#endif // CYGDBG_DIAG_BUF

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

#ifndef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
            // only gobble characters if no interrupt handler to grab ^Cs
            // is installed (which is exclusive with device driver use)

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.  This is the reason for this clause
            // being a loop.
            c = cyg_quicc_smc1_uart_rcvchar();

            if( c == '+' )
                break;              // a good acknowledge
#if 0
            if( c1 == 3 ) {
                // Ctrl-C: breakpoint.
                breakpoint();
                break;
            }
#endif
            // otherwise, loop round again
#else
            break;
#endif
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
#endif // NOT def CYG_HAL_STARTUP_ROM


void hal_diag_read_char(char *c)
{
    *c = cyg_quicc_smc1_uart_rcvchar();
}

#endif // CYG_KERNEL_DIAG_SERIAL

// EOF hal_diag.c
