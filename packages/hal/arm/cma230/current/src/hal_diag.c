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
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, gthomas
// Contributors:        nickg, gthomas
// Date:        1998-03-02
// Purpose:     HAL diagnostic output
// Description: Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//===========================================================================*/

#include <pkgconf/hal.h>
#include <pkgconf/hal_arm_cma230.h>        // board specifics

#include <cyg/infra/cyg_type.h>         // base types
#include <cyg/infra/cyg_trac.h>         // tracing macros
#include <cyg/infra/cyg_ass.h>          // assertion macros

#include <cyg/hal/hal_arch.h>           // basic machine info
#include <cyg/hal/hal_intr.h>           // interrupt macros
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_cma230.h>         // Hardware definitions
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
#include <cyg/hal/drv_api.h>
#include <cyg/hal/hal_stub.h>           // cyg_hal_gdb_interrupt
#endif

// Assumption: all diagnostic output must be GDB packetized unless this is a ROM (i.e.
// totally stand-alone) system.

#if defined(CYG_HAL_STARTUP_ROM) || defined(CYGDBG_HAL_DIAG_DISABLE_GDB_PROTOCOL)
#define HAL_DIAG_USES_HARDWARE
#endif

/*---------------------------------------------------------------------------*/
#if CYGHWR_HAL_ARM_CMA230_DIAG_PORT==0
// This is the base address of the A-channel
#define CYG_DEVICE_SERIAL_RS232_16550_BASE      CMA101_DUARTA
#define CYG_DEVICE_SERIAL_INT                   CYGNUM_HAL_INTERRUPT_SERIAL_A
#else
// This is the base address of the B-channel
#define CYG_DEVICE_SERIAL_RS232_16550_BASE      CMA101_DUARTB
#define CYG_DEVICE_SERIAL_INT                   CYGNUM_HAL_INTERRUPT_SERIAL_B
#endif

// Based on 3.6864 MHz xtal
#if CYGHWR_HAL_ARM_CMA230_DIAG_BAUD==9600
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB        0x00
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB        0x18
#endif
#if CYGHWR_HAL_ARM_CMA230_DIAG_BAUD==19200
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB        0x00
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB        0x0C
#endif
#if CYGHWR_HAL_ARM_CMA230_DIAG_BAUD==38400
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB        0x00
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB        0x06
#endif
#if CYGHWR_HAL_ARM_CMA230_DIAG_BAUD==115200
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB        0x00
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB        0x02
#endif

#ifndef CYG_DEVICE_SERIAL_RS232_BAUD_MSB
#error Missing/incorrect serial baud rate defined - CDL error?
#endif

// Define the serial registers.
#define CYG_DEVICE_SERIAL_RS232_16550_RBR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x00)
    // receiver buffer register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_THR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x00)
    // transmitter holding register, write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_DLL \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x00)
    // divisor latch (LS), read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_16550_IER \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x08)
    // interrupt enable register, read/write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_DLM \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x08)
    // divisor latch (MS), read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_16550_IIR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
    // interrupt identification register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_FCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
    // fifo control register, write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_LCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x18)
    // line control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_MCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x20)
    // modem control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_LSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x28)
    // line status register, read
#define CYG_DEVICE_SERIAL_RS232_16550_MSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x30)
    // modem status register, read

// The line status register bits.
#define SIO_LSR_DR      0x01            // data ready
#define SIO_LSR_OE      0x02            // overrun error
#define SIO_LSR_PE      0x04            // parity error
#define SIO_LSR_FE      0x08            // framing error
#define SIO_LSR_BI      0x10            // break interrupt
#define SIO_LSR_THRE    0x20            // transmitter holding register empty
#define SIO_LSR_TEMT    0x40            // transmitter register empty
#define SIO_LSR_ERR     0x80            // any error condition

// The modem status register bits.
#define SIO_MSR_DCTS  0x01              // delta clear to send
#define SIO_MSR_DDSR  0x02              // delta data set ready
#define SIO_MSR_TERI  0x04              // trailing edge ring indicator
#define SIO_MSR_DDCD  0x08              // delta data carrier detect
#define SIO_MSR_CTS   0x10              // clear to send
#define SIO_MSR_DSR   0x20              // data set ready
#define SIO_MSR_RI    0x40              // ring indicator
#define SIO_MSR_DCD   0x80              // data carrier detect

// The line control register bits.
#define SIO_LCR_WLS0   0x01             // word length select bit 0
#define SIO_LCR_WLS1   0x02             // word length select bit 1
#define SIO_LCR_STB    0x04             // number of stop bits
#define SIO_LCR_PEN    0x08             // parity enable
#define SIO_LCR_EPS    0x10             // even parity select
#define SIO_LCR_SP     0x20             // stick parity
#define SIO_LCR_SB     0x40             // set break
#define SIO_LCR_DLAB   0x80             // divisor latch access bit


#ifdef HAL_DIAG_USES_HARDWARE

void hal_diag_init(void)
{
    static int init = 0;
    char *msg = "\n\rARM eCos\n\r";
    cyg_uint8 lcr;

    if (init++) return;
    // 8-1-no parity.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR,
                     SIO_LCR_WLS0 | SIO_LCR_WLS1);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    lcr |= SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLL,
                     CYG_DEVICE_SERIAL_RS232_BAUD_LSB);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLM,
                     CYG_DEVICE_SERIAL_RS232_BAUD_MSB);
    lcr &= ~SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR, 0x07);  // Enable & clear FIFO
    while (*msg) hal_diag_write_char(*msg++);
}

#ifdef DEBUG_DIAG
#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#define DIAG_BUFSIZE 32
#else
#define DIAG_BUFSIZE 2048
#endif
static char diag_buffer[DIAG_BUFSIZE];
static int diag_bp = 0;
#endif

void hal_diag_write_char(char c)
{
    cyg_uint8 lsr;

    hal_diag_init();
    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);
#ifdef DEBUG_DIAG
    diag_buffer[diag_bp++] = c;
    if (diag_bp == DIAG_BUFSIZE) {
        while (1) ;
        diag_bp = 0;
    }
#endif
}

void hal_diag_read_char(char *c)
{
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_DR) == 0);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, *c);
}

#else // HAL_DIAG relies on GDB

// Initialize diag port - assume GDB channel is already set up
void hal_diag_init(void)
{
}

// Actually send character down the wire
static void
hal_diag_write_char_serial(char c)
{
    cyg_uint8 lsr;

    hal_diag_init();
    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);
    HAL_IO_BARRIER ();
}

static bool
hal_diag_read_serial(char *c)
{
    long timeout = 1000000000;  // A long time...
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
        if (--timeout == 0) return false;
    } while ((lsr & SIO_LSR_DR) == 0);
    HAL_IO_BARRIER ();                  // Prevent RBR preload.

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, *c);
    HAL_IO_BARRIER ();
    return true;
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
            cyg_drv_interrupt_acknowledge(CYG_DEVICE_SERIAL_INT);
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
