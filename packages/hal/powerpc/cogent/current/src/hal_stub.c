/* hal_stub.c - helper functions for stub, specific to ECC HAL
 * 
 * Copyright (c) 1998 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 */

#include <stddef.h>

#include <pkgconf/hal.h>

#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS

#include <cyg/hal/hal_stub.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // HAL_ENABLE_INTERRUPTS
#include <cyg/hal/ppc_regs.h>           // Register values
#include <cyg/hal/hal_diag.h>
#include <cyg/hal/hal_cache.h>

/*---------------------------------------------------------------------------*/
/* From serial_16550.h */
#define CYG_DEVICE_SERIAL_RS232_T1_VALUE_B38400         0x00
#define CYG_DEVICE_SERIAL_RS232_T2_VALUE_B38400         0x06

// FIXME: This is the base address of the B-channel on the PowerPC
// Cogent board.
#define CYG_DEVICE_SERIAL_RS232_16550_BASE      0xe900007


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
#define CYG_DEVICE_SERIAL_RS232_16550_AFR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x10)
    // alternate function register, read/write, dlab = 1
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
#define CYG_DEVICE_SERIAL_RS232_16550_SCR \
    ((volatile cyg_uint8 *) CYG_DEVICE_SERIAL_RS232_16550_BASE + 0x38)
    // scratch pad register

// FIXME: Naming convention?
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

/*---------------------------------------------------------------------------*/

/* Local data */


/* If ENABLED is non-zero, supply a memory fault handler that traps
   subsequent memory errors and sets _mem_err to 1. Otherwise, memory
   faults are processed as exceptions. */

void __set_mem_fault_trap (int enable) {}

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
// This ISR is called from the interrupt handler on external
// interrupts.  This should only happen when there is no serial
// driver, so the code shouldn't mess anything up.
int cyg_hal_gdb_isr (void)
{
    cyg_uint8 vec;


    // Serial interrupts are feed to IRQ1 (vector 0x08) on the Cogent
    // board.
    HAL_READ_UINT8 (IMM_SIVEC, vec);
    if (0x08 == vec) {
        cyg_uint8 c;

        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, c);
        if( 3 == c )
        {
            // Ctrl-C: breakpoint.
            extern void breakpoint(void);
            breakpoint();

            // Interrupt handled. Don't call ISR proper.
            return 0;
        }
    }

    // Not caused by GDB. Call ISR proper.
    return 1;
}

static int interruptible (int state)
{
    unsigned long mask;
    HAL_READ_UINT32(IMM_SIMASK, mask);

    // Serial interrupts are feed to IRQ1.
    if (state) {
        HAL_WRITE_UINT32(IMM_SIMASK, mask | 0x20000000);
    } else {
        HAL_WRITE_UINT32(IMM_SIMASK, mask & (~0x20000000));
    }

    return 0;
}
#endif

static void serial_init(void)
{
    cyg_uint8 lcr;

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
        
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR, mcr);
        mcr |= 8;
        HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR, mcr);

        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR+0x40, mcr);
        mcr |= 8;
        HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_MCR+0x40, mcr);
    }

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // Enable IRQ1.
    HAL_WRITE_UINT32(IMM_SIMASK, 0x20000000);
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_FCR, 0);
    // Enable serial receive interrupts.
    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_IER, SIO_IER_ERDAI);
    HAL_ENABLE_INTERRUPTS();
#endif
}

/* Write C to the current serial port. */

void putDebugChar (int c)
{
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);
}

/* Read one character from the current serial port. */

int getDebugChar (void)
{
    char c;
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_DR) == 0);

    HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_RBR, c);

    return c;
}


/* Install the standard set of trap handlers for the stub. */

void __install_traps (void)
{
    // Nothing to do, the HAL will call handle_exception when one occurs.
}

/* Flush the instruction cache. */
void flush_i_cache (void) { HAL_ICACHE_INVALIDATE_ALL(); }


/* Reset the board. */
void __reset (void) {}


/* Set the baud rate for the current serial port. */
void __set_baud_rate (int baud) {}


/* Initialize the hardware. */
void initHardware (void) 
{
    static int initialized = 0;

    if (!initialized) {
        initialized = 1;

        serial_init ();

#if defined(CYG_HAL_ROM_MONITOR)
        /* Initialize the LCD */
        diag_init ();
#endif

    }
}

#if 0
/* Alternate read function for use when stub is running.  It
   reads the data for standard input from the remote GDB.  */

int dve_read (int fd, char *buf, int nbytes)
{
  __get_gdb_input (buf, nbytes, 1);
}

/* Alternate write function for use when stub is running.  It
   writes the data for standard output to the remote GDB.  */

int dve_write (int fd, char *buf, int nbytes)
{
    __output_gdb_string (buf, nbytes);
}

#endif


int ppc_process_signal (int signal)
{
    /* We don't care about the signal (atm). */
    return 0;
}

/* Install traps and otherwise initialize stub. */

void set_debug_traps (void)
{
    /* Set signal handling vector so we can treat 'C<signum>' as 'c'. */
    __process_signal_vec = &ppc_process_signal;

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
    // Control of GDB interrupts.
    __interruptible_control = &interruptible;
#endif

    initHardware ();
}


/* This function will generate a breakpoint exception.  It is used at the
   beginning of a program to sync up with a debugger and can be used
   otherwise as a quick means to stop program execution and "break" into
   the debugger. */

void
breakpoint()
{
    HAL_BREAKPOINT(_breakinst);
}


/* This function returns the opcode for a 'trap' instruction.  */

unsigned long __break_opcode ()
{
  return HAL_BREAKINST;
}

#endif // CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
