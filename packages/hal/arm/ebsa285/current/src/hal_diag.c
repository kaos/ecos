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
#include <cyg/hal/drv_api.h>            // cyg_drv_interrupt_acknowledge
#include <cyg/hal/hal_stub.h>           // cyg_hal_gdb_interrupt
#endif
#include <cyg/hal/hal_ebsa285.h>        // Hardware definitions

#ifdef CYGSEM_HAL_ROM_MONITOR
#define CYG_HAL_STARTUP_ROM
#undef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#endif

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL)
#define HAL_DIAG_USES_HARDWARE
#endif

/*---------------------------------------------------------------------------*/
// EBSA285 Serial Port (UARTx) for Debug

// Actually send character down the wire
static void
hal_diag_write_char_serial(char c)
{
    // Wait for Tx FIFO not full
    while ((*SA110_UART_FLAG_REGISTER & SA110_TX_FIFO_STATUS_MASK) == SA110_TX_FIFO_BUSY)
        ;
    *SA110_UART_DATA_REGISTER = c;
}

static bool
hal_diag_read_serial(char *c)
{
    long timeout = 1000000000;  // A long time...
    while ((*SA110_UART_FLAG_REGISTER & SA110_RX_FIFO_STATUS_MASK) == SA110_RX_FIFO_EMPTY)
        if ( --timeout == 0 )
            return false;
    *c = (char)(*SA110_UART_DATA_REGISTER & 0xFF);
    return true;
}

/*
 * Baud rate selection stuff
 */
#if 0
struct _baud {
    int baud;
    unsigned short divisor_high, divisor_low;
};

const static struct _baud bauds[] = {
#if (FCLK_MHZ == 50)
    {   300, 0xA, 0x2B},                  /* 2603  = 0x0A2B */
    {   600, 0x5, 0x15},                  /* 1301  = 0x0515 */
    {  1200, 0x2, 0x8A},                  /* 650   = 0x028A */
    {  2400, 0x1, 0x45},                  /* 325   = 0x0145 */
    {  4800, 0x0, 0xA2},                  /* 162   = 0x00A2 */
    {  9600, 0x0, 0x50},                  /* 80    = 0x0050 */
    { 19200, 0x0, 0x28},                  /* 40    = 0x0028 */
    { 38400, 0x0, 0x13},                  /* 19    = 0x0013 */
#elif (FCLK_MHZ == 60)
    {   300, 0xC, 0x34},                  /* 2603  = 0x0A2B */
    {   600, 0x6, 0x19},                  /* 1301  = 0x0515 */
    {  1200, 0x3, 0x0C},                  /* 650   = 0x028A */
    {  2400, 0x1, 0x86},                  /* 325   = 0x0145 */
    {  4800, 0x0, 0xC2},                  /* 162   = 0x00A2 */
    {  9600, 0x0, 0x61},                  /* 80    = 0x0050 */
    { 19200, 0x0, 0x30},                  /* 40    = 0x0028 */
    { 38400, 0x0, 0x17},                  /* 19    = 0x0013 */
#endif
};
#endif

void hal_diag_init(void)
{
    int dummy;
    /*
     * Make sure everything is off
     */
    *SA110_UART_CONTROL_REGISTER = SA110_UART_DISABLED | SA110_SIR_DISABLED;
    
    /*
     * Read the RXStat to drain the fifo
     */
    dummy = *SA110_UART_RXSTAT;

    /*
     * Set the baud rate - this also turns the uart on.
     *
     * Note that the ordering of these writes is critical,
     * and the writes to the H_BAUD_CONTROL and CONTROL_REGISTER
     * are necessary to force the UART to update its register
     * contents.
     */
    *SA110_UART_L_BAUD_CONTROL   = 0x13; // bp->divisor_low;
    *SA110_UART_M_BAUD_CONTROL   = 0x00; // bp->divisor_high;
    *SA110_UART_H_BAUD_CONTROL = SA110_UART_BREAK_DISABLED    |
        SA110_UART_PARITY_DISABLED   |
        SA110_UART_STOP_BITS_ONE     |
        SA110_UART_FIFO_ENABLED      |
        SA110_UART_DATA_LENGTH_8_BITS;
    *SA110_UART_CONTROL_REGISTER = SA110_UART_ENABLED | 
        SA110_SIR_DISABLED;
    // All done
}

#ifdef HAL_DIAG_USES_HARDWARE

#ifdef DEBUG_DIAG
#ifndef CYG_HAL_STARTUP_ROM
#define DIAG_BUFSIZE 2048
static char diag_buffer[DIAG_BUFSIZE];
static int diag_bp = 0;
#endif
#endif

void hal_diag_read_char(char *c)
{
    while (!hal_diag_read_serial(c)) ;
}

void hal_diag_write_char(char c)
{
#ifdef DEBUG_DIAG
#ifndef CYG_HAL_STARTUP_ROM
    diag_buffer[diag_bp++] = c;
    if (diag_bp == sizeof(diag_buffer)) diag_bp = 0;
#endif
#endif
    hal_diag_write_char_serial(c);
}

#else // not HAL_DIAG_USES_HARDWARE - it uses GDB protocol

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

#if 0
    // Do not unconditionally poke the XBUS LED location - XBUS may not be
    // available if external arbiter is in use.  This fragment may still be
    // useful for debugging in the future, so left thus:
    {
//        int i;
        *(cyg_uint32 *)0x40012000 = 7 & (cyg_uint32)c; // LED XBUS location
//        for ( i = 0x1000000; i > 0; i-- ) ;
    }
#endif

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
#ifndef CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
            char c1;
#endif        
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

#ifdef CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT

            break; // regardless

#else // not CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT Ie. usually...

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
                cyg_hal_gdb_interrupt(
                    (target_register_t)__builtin_return_address(0) );
                break;
            }
#endif // CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT

#endif // ! CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
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
#ifndef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#ifdef CYGDBG_HAL_DEBUG_GDB_CTRLC_SUPPORT
// This routine is polled from the default ISR - the assumption is that any
// other ISR installed knows what it's going and can handle a ^C correctly.
int hal_ctrlc_isr( CYG_ADDRWORD vector, CYG_ADDRWORD data)
{
    char c;
    int reg;
    if ( CYG_DIAG_DEV_INT != vector )
        return 0; // not for us

    reg = *SA110_UART_FLAG_REGISTER;
    // read it anyway just in case - no harm done and we might prevent an
    // interrup loop
    c = (char)(*SA110_UART_DATA_REGISTER & 0xFF);
    if ( (reg & SA110_RX_FIFO_STATUS_MASK) != SA110_RX_FIFO_EMPTY ) {
        if ( 3 == c ) {
            HAL_BREAKPOINT( hal_ctrlc_isr_breakinst );
        }
    }
    return 1;
}
#endif
#endif

/*---------------------------------------------------------------------------*/
/* End of hal_diag.c */
