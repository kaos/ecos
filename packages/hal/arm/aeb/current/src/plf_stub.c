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
// Author(s):   gthomas (based on the old ARM/AEB hal_stub.c)
// Contributors:gthomas, jskov
// Date:        1999-02-15
// Purpose:     Platform specific code for GDB stub support.
//              
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>

#include <cyg/hal/hal_io.h>             // HAL IO macros
#include <cyg/hal/hal_intr.h>           // HAL interrupt macros
#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/drv_api.h>            // HAL ISR support
#endif

//-----------------------------------------------------------------------------
// Serial definitions.

// 38400, UART1
#define CYG_DEVICE_SERIAL_RS232_BAUD_MSB    0
#define CYG_DEVICE_SERIAL_RS232_BAUD_LSB    (13*3)

// This is the base address of UART1 on the AEB-1 board.
#define CYG_DEVICE_SERIAL_RS232_16550_BASE      0xFFFF0400

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
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x04)
    // interrupt enable register, read/write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_DLM \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x04)
    // divisor latch (MS), read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_16550_IIR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x08)
    // interrupt identification register, read, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_FCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x08)
    // fifo control register, write, dlab = 0
#define CYG_DEVICE_SERIAL_RS232_16550_LCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x0C)
    // line control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_MCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
    // modem control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_LSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x14)
    // line status register, read
#define CYG_DEVICE_SERIAL_RS232_16550_MSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x18)
    // modem status register, read

// The interrupt enable register bits.
#define SIO_IER_ERDAI   0x01            // enable received data available irq
#define SIO_IER_ETHREI  0x02            // enable THR empty interrupt
#define SIO_IER_ELSI    0x04            // enable receiver line status irq
#define SIO_IER_EMSI    0x08            // enable modem status interrupt

// The interrupt identification register bits.
#define SIO_IIR_IP      0x01            // 0 if interrupt pending
#define SIO_IIR_ID_MASK 0x0e            // mask for interrupt ID bits

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

// Interrupt Enable Register
#define SIO_IER_RCV 0x01
#define SIO_IER_XMT 0x02
#define SIO_IER_LS  0x04
#define SIO_IER_MS  0x08

// Modem Control Register
#define SIO_MCR_DTR 0x01
#define SIO_MCR_RTS 0x02
#define SIO_MCR_INT 0x08   // Enable interrupts

#define LED (volatile char *)0xFFFF1C0C

#define LED1      (7<<1)
#define LED2      (6<<1)
#define LED3      (5<<1)
#define LED4      (4<<1)

#define LED_ON    0x01
#define LED_OFF   0x00

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

    c = hal_aeb_get_char();  // Fetch the character
    if( 3 == c ) {  // ^C
        // Ctrl-C: set a breakpoint at PC so GDB will display the
        // correct program context when stopping rather than the
        // interrupt handler.
        cyg_hal_gdb_interrupt (regs->pc);
        cyg_drv_interrupt_acknowledge(CYGNUM_HAL_INTERRUPT_UART1);
    }
    return 0;  // No need to run DSR
}

int 
hal_aeb_interruptible(int state)
{
    if (state) {
        cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_UART1);
    } else {
        cyg_drv_interrupt_mask(CYGNUM_HAL_INTERRUPT_UART1);
    }

    return 0;
}
#endif

static void dummy(void) {}

// Initialize the current serial port.
void hal_aeb_init_serial(void)
{
    cyg_uint8 lcr;

    // 8-1-no parity.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR,
                     SIO_LCR_WLS0 | SIO_LCR_WLS1);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    lcr |= SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLL,
                     CYG_DEVICE_SERIAL_RS232_BAUD_LSB);
    dummy();  // FIXME! - GCC bug
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLM,
                     CYG_DEVICE_SERIAL_RS232_BAUD_MSB);
    lcr &= ~SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR, 0x07);  // Enable & clear FIFO

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    cyg_drv_interrupt_create(CYGNUM_HAL_INTERRUPT_UART1,
                             99,                     // Priority - what goes here?
                             0,                      //  Data item passed to interrupt handler
                             cyg_hal_gdb_isr,
                             0,
                             &gdb_interrupt_handle,
                             &gdb_interrupt);
    cyg_drv_interrupt_attach(gdb_interrupt_handle);
    cyg_drv_interrupt_unmask(CYGNUM_HAL_INTERRUPT_UART1);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IER, SIO_IER_RCV);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR, SIO_MCR_INT|SIO_MCR_DTR|SIO_MCR_RTS);
#endif
}

// Write C to the current serial port.
void hal_aeb_put_char(int c)
{
    cyg_uint8 lsr;
    static int led_state = LED_ON;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);
    *LED = led_state | LED1;
    if (led_state == LED_ON) {
        led_state = LED_OFF;
    } else {
        led_state = LED_ON;
    }
}

// Read one character from the current serial port.
int hal_aeb_get_char(void)
{
    char c;
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_DR) == 0);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, c);
    return c;
}


//-----------------------------------------------------------------------------
// Reset board (definitions from watchdog file aeb1.cxx)

// Register definitions
#define CYGARC_REG_WATCHDOG_BASE        0xFFFFAC00
#define CYGARC_REG_WATCHDOG_WDCTLR      (CYGARC_REG_WATCHDOG_BASE+0x30)
#define CYGARC_REG_WATCHDOG_WDCNTR      (CYGARC_REG_WATCHDOG_BASE+0x34)

// Control register bits
#define CYGARC_REG_WATCHDOG_WDCTLR_EN        0x01 // enable
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_NMF   0x00 // non-maskable fiq
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_ER    0x04 // external reset
#define CYGARC_REG_WATCHDOG_WDCTLR_RSP_SR    0x06 // system reset
#define CYGARC_REG_WATCHDOG_WDCTLR_FRZ       0x08 // lock enable bit
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_MASK  0x70 // time out period

#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_17    0x00 // 2^17
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_17_P  5242880 // = 5.2ms

#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_25    0x40 // 2^25
#define CYGARC_REG_WATCHDOG_WDCTLR_TOP_25_P  1342177300 // = 1.3421773s

void
hal_aeb_reset(void)
{
    // Clear the watchdog counter.
    HAL_WRITE_UINT32(CYGARC_REG_WATCHDOG_WDCNTR, 0);

    // Enable the watchdog with the smallest timeout.
    HAL_WRITE_UINT8(CYGARC_REG_WATCHDOG_WDCTLR, 
                    (CYGARC_REG_WATCHDOG_WDCTLR_TOP_17
                     | CYGARC_REG_WATCHDOG_WDCTLR_FRZ
                     | CYGARC_REG_WATCHDOG_WDCTLR_RSP_SR
                     | CYGARC_REG_WATCHDOG_WDCTLR_EN));

    // Wait for it...
    for(;;);
}

#endif // ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
//-----------------------------------------------------------------------------
// End of plf_stub.c
