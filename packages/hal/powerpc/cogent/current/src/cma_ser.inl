#ifndef CYGONCE_HAL_CMA_SER_H
#define CYGONCE_HAL_CMA_SER_H
//=============================================================================
//
//      cma_ser.inl
//
//      Simple driver for the Cogent CMA mother board serial
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
// Author(s):   jskov
// Contributors:jskov
// Date:        1999-01-27
// Description: Simple driver for the Cogent CMA mother board serial.
//              Clients of this file can configure the behavior with:
//              CYG_CMA_PORT:   serial port to use (0 or 1)
//              CYG_CMA_PUBLIC: define to export init, get, and put functions.
//              CYG_CMA_STUB:   define to get extra stub functions
//              CYG_CMA_NAME_PREFIX: Function prefix. Default is hal_cma_
//                                   Using different prefixes can be required
//                                   if both serial ports are used from the
//                                   same source file.
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>

#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE/MASK/UNMASK_INTERRUPTS

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
#include <cyg/hal/ppc_stub.h>           // target_register_t
#endif

// Unless explicitly asked to export functions, keep them local.
#ifndef CYG_CMA_PUBLIC
#define CYG_CMA_PUBLIC static
#endif

// Function names are prefix with hal_cma_ unless something else is specified.
#ifndef CYG_CMA_NAME_PREFIX
#define CYG_CMA_NAME_PREFIX hal_cma_
#endif

//-----------------------------------------------------------------------------
// Name Conversion magic.
#define Str(x, y) x ## y
#define Xstr(x, y) Str(x, y)
#define NC(_name)  Xstr(CYG_CMA_NAME_PREFIX, _name)

//-----------------------------------------------------------------------------
// The exported functions.
// Base:
CYG_CMA_PUBLIC void NC(init_serial)( void );
CYG_CMA_PUBLIC int  NC(get_char)( void );
CYG_CMA_PUBLIC void NC(put_char)( int c );
// GDB BREAK support:
#if defined(CYG_CMA_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int  NC(interruptible)( int state );
void NC(init_break_irq)( void );
int  cyg_hal_gdb_isr( target_register_t pc ); // called from vectors.S
#endif

//-----------------------------------------------------------------------------
// Port to use.
#if (1 == CYG_CMA_PORT)
#define CYG_DEVICE_SERIAL_RS232_16550_BASE CYG_DEVICE_SERIAL_RS232_16550_BASE_B
#else
#define CYG_DEVICE_SERIAL_RS232_16550_BASE CYG_DEVICE_SERIAL_RS232_16550_BASE_A
#endif

//-----------------------------------------------------------------------------
// Controller definitions.
#ifndef CYGONCE_HAL_CMA_SER_INL
#define CYGONCE_HAL_CMA_SER_INL

//-----------------------------------------------------------------------------
// There are two serial ports.
#define CYG_DEVICE_SERIAL_RS232_16550_BASE_A    0xe900047 // port A
#define CYG_DEVICE_SERIAL_RS232_16550_BASE_B    0xe900007 // port B

//-----------------------------------------------------------------------------
// Default baud rate is 38400
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400         0x00
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400         0x06

//-----------------------------------------------------------------------------
// Define the serial registers. The Cogent board is equipped with a 16552
// serial chip.
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
#define CYG_DEVICE_SERIAL_RS232_16550_AFR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
    // alternate function register, read/write, dlab = 1
#define CYG_DEVICE_SERIAL_RS232_16550_LCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x18)
    // line control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_MCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x20)
    // modem control register, read/write
#define CYG_DEVICE_SERIAL_RS232_16550_MCR_A \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE_A + 0x20)
#define CYG_DEVICE_SERIAL_RS232_16550_MCR_B \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE_B + 0x20)
#define CYG_DEVICE_SERIAL_RS232_16550_LSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x28)
    // line status register, read
#define CYG_DEVICE_SERIAL_RS232_16550_MSR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x30)
    // modem status register, read
#define CYG_DEVICE_SERIAL_RS232_16550_SCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x38)
    // scratch pad register

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

// The FIFO control register
#define SIO_FCR_FCR0   0x01             // enable xmit and rcvr fifos
#define SIO_FCR_FCR1   0x02             // clear RCVR FIFO
#define SIO_FCR_FCR2   0x04             // clear XMIT FIFO

#endif // ifndef CYGONCE_HAL_CMA_SER_INL

//-----------------------------------------------------------------------------
// The minimal init, get and put functions. All by polling.

CYG_CMA_PUBLIC void NC(init_serial)( void )
{
    cyg_uint8 lcr;

    // Disable interrupts.
#ifdef CYGPKG_HAL_POWERPC_MPC8xx
    HAL_INTERRUPT_MASK(CYGNUM_HAL_INTERRUPT_SIU_IRQ1);
#else
#error "Not supported on CPU"
#endif
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IER, 0);

    // Disable and clear FIFOs (need to enable to clear).
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR,
                     (SIO_FCR_FCR0 | SIO_FCR_FCR1 | SIO_FCR_FCR2));
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR, 0);

    // 8-1-no parity.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR,
                     SIO_LCR_WLS0 | SIO_LCR_WLS1);

    // Set speed to 38400.
    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);
    lcr |= SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);


    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLL,
                     CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_DLM,
                     CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400);
    lcr &= ~SIO_LCR_DLAB;
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LCR, lcr);

    {
        // Special initialization for ST16C552 on CMA102
        cyg_uint8 mcr;
        
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR_A, mcr);
        mcr |= 8;
        HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR_A, mcr);

        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR_B, mcr);
        mcr |= 8;
        HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR_B, mcr);
    }

    // Enable FIFOs (and clear them).
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR,
                     (SIO_FCR_FCR0 | SIO_FCR_FCR1 | SIO_FCR_FCR2));

}

CYG_CMA_PUBLIC int NC(get_char)( void )
{
    cyg_uint8 c, lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_DR) == 0);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, c);

    return c;
}

CYG_CMA_PUBLIC void NC(put_char)( int c )
{
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);

    // Hang around until the character has been safely sent.
    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);
}


//-----------------------------------------------------------------------------
// Additional functions required to provide GDB BREAK support.

#if defined(CYG_CMA_STUB) && defined(CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT)
int NC(interruptible)( int state )
{
    int vector;
#ifdef CYGPKG_HAL_POWERPC_MPC8xx
    // Serial interrupts are feed to IRQ1 on the 8xx CPUs.
    vector = CYGNUM_HAL_INTERRUPT_SIU_IRQ1;
#else
#error "Not supported on CPU"
#endif

    if (state) {
        HAL_INTERRUPT_UNMASK(vector);
    } else {
        HAL_INTERRUPT_MASK(vector);
    }

    return 0;
}

void NC(init_break_irq)( void )
{
    NC(interruptible)(0);

    // Enable serial receive interrupts.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IER, SIO_IER_ERDAI);
    HAL_ENABLE_INTERRUPTS();

    NC(interruptible)(1);
}

// This ISR is called from the interrupt handler. This should only
// happen when there is no real serial driver, so the code shouldn't
// mess anything up.
int cyg_hal_gdb_isr( target_register_t pc )
{
    cyg_uint8 vec;
    int break_irq = 0;

#ifdef CYGPKG_HAL_POWERPC_MPC8xx
    // Serial interrupts are feed to IRQ1 (vector 0x08) on the Cogent
    // board.
    HAL_READ_UINT8(CYGARC_REG_IMM_SIVEC, vec);
    if (0x08 == vec)
        break_irq = 1;
#else
#error "Not supported on CPU"
#endif

    if ( break_irq ){
        cyg_uint8 c;

        HAL_READ_UINT8( CYG_DEVICE_SERIAL_RS232_16550_RBR, c );
        if ( 3 == c )
        {
            // Ctrl-C: set a breakpoint at PC so GDB will display the
            // correct program context when stopping rather than the
            // interrupt handler.
            cyg_hal_gdb_interrupt( pc );

            // Interrupt handled. Don't call ISR proper. At return
            // from the VSR, execution will stop at the breakpoint
            // just set.
            return 0;
        }
    }

    // Not caused by GDB. Call ISR proper.
    return 1;
}

#endif

//-----------------------------------------------------------------------------
// Undefine all macros used in this file.
#undef CYG_CMA_PORT
#undef CYG_CMA_PUBLIC
#undef CYG_CMA_STUB
#undef CYG_CMA_NAME_PREFIX
#undef NC
#undef Str
#undef Xstr
#undef CYG_DEVICE_SERIAL_RS232_16550_BASE

//-----------------------------------------------------------------------------
// end of cma_ser.inl
#endif // CYGONCE_HAL_CMA_SER_INL
