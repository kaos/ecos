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
// by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
// -------------------------------------------
//
//####COPYRIGHTEND####
//=============================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):   nickg, jskov
// Contributors:        nickg
// Date:        1998-03-02
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

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // __output_gdb_string
#endif

#include <cyg/hal/ppc_regs.h>


//-----------------------------------------------------------------------------
// Select default diag channel to use

//#define CYG_KERNEL_DIAG_ROMART
//#define CYG_KERNEL_DIAG_LCD
//#define CYG_KERNEL_DIAG_SERIAL

#if !defined(CYG_KERNEL_DIAG_SERIAL) && \
    !defined(CYG_KERNEL_DIAG_LCD)    && \
    !defined(CYG_KERNEL_DIAG_ROMART)

#define CYG_KERNEL_DIAG_SERIAL

#endif

//-----------------------------------------------------------------------------
// Cogent board specific 16550 code.

#if defined(CYG_KERNEL_DIAG_SERIAL)

//-----------------------------------------------------------------------------
// From serial_16550.h
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

void hal_diag_init(void)
{
#if !defined(CYGDBG_KERNEL_DEBUG_GDB_INCLUDE_STUBS)
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
#endif
}

void hal_diag_write_char(char c)
{
#if !defined(CYGDBG_KERNEL_DEBUG_GDB_INCLUDE_STUBS)
    cyg_uint8 lsr;

    do {
        HAL_READ_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_LSR, lsr);
    } while ((lsr & SIO_LSR_THRE) == 0);

    HAL_WRITE_UINT8 (CYG_DEVICE_SERIAL_RS232_16550_THR, c);
#else
    __output_gdb_string (&c, 1);
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
#endif

//-----------------------------------------------------------------------------
// Cogent board specific LCD code

#if defined(CYG_KERNEL_DIAG_LCD)

// FEMA 162B 16 character x 2 line LCD
// base addresses and register offsets *

#define MBD_BASE 0

#define LCD_BASE                (MBD_BASE + 0xEB00007)

#define LCD_DATA                (LCD_BASE + 0x00) // read/write lcd data
#define LCD_STAT                (LCD_BASE + 0x08) // read lcd busy status
#define LCD_CMD                 (LCD_BASE + 0x08) // write lcd command

// status register bit definitions
#define LCD_STAT_BUSY   0x80    // 1 = display busy
#define LCD_STAT_ADD    0x7F    // bits 0-6 return current display address

// command register definitions
#define LCD_CMD_RST             0x01    // clear entire display and reset display address
#define LCD_CMD_HOME            0x02    // reset display address and reset any shifting
#define LCD_CMD_ECL             0x04    // move cursor left one position on next data write
#define LCD_CMD_ESL             0x05    // shift display left one position on next data write
#define LCD_CMD_ECR             0x06    // move cursor right one position on next data write
#define LCD_CMD_ESR             0x07    // shift display right one position on next data write
#define LCD_CMD_DOFF            0x08    // display off, cursor off, blinking off
#define LCD_CMD_BL              0x09    // blink character at current cursor position
#define LCD_CMD_CUR             0x0A    // enable cursor on
#define LCD_CMD_DON             0x0C    // turn display on
#define LCD_CMD_CL              0x10    // move cursor left one position
#define LCD_CMD_SL              0x14    // shift display left one position
#define LCD_CMD_CR              0x18    // move cursor right one position
#define LCD_CMD_SR              0x1C    // shift display right one position
#define LCD_CMD_MODE            0x38    // sets 8 bits, 2 lines, 5x7 characters
#define LCD_CMD_ACG             0x40    // bits 0-5 sets the character generator address
#define LCD_CMD_ADD             0x80    // bits 0-6 sets the display data address to line 1 +

// LCD status values
#define LCD_OK                  0x00
#define LCD_ERR                 0x01

#define LCD_LINE0       0x00
#define LCD_LINE1       0x40

#define LCD_LINE_LENGTH 16

static char lcd_line0[LCD_LINE_LENGTH+1];
static char lcd_line1[LCD_LINE_LENGTH+1];
static char *lcd_line[2] = { lcd_line0, lcd_line1 };
static int lcd_curline = 0;
static int lcd_linepos = 0;


static void lcd_dis(int add, char *string);

void hal_diag_init()
{
    cyg_uint8 stat;
    int i;
    
    // wait for not busy
    do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);

    // configure the lcd for 8 bits/char, 2 lines
    // and 5x7 dot matrix
    HAL_WRITE_UINT8 (LCD_CMD, LCD_CMD_MODE);

    // wait for not busy
    do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);

    // turn the LCD display on
    HAL_WRITE_UINT8 (LCD_CMD, LCD_CMD_DON);

    lcd_curline = 0;
    lcd_linepos = 0;

    for( i = 0; i < LCD_LINE_LENGTH; i++ )
        lcd_line[0][i] = lcd_line[1][i] = ' ';

    lcd_line[0][LCD_LINE_LENGTH] = lcd_line[1][LCD_LINE_LENGTH] = 0;

    lcd_dis( LCD_LINE0, lcd_line[0] );
    lcd_dis( LCD_LINE1, lcd_line[1] );


#if defined(CYG_HAL_ROM_MONITOR)
    // It's handy to have the LCD initialized at reset when using it
       for debugging output.
    {
        extern void diag_write_string (const char*);
        diag_write_string ("eCos ROM " __TIME__ "\n");
        diag_write_string (__DATE__ "\n");
    }
#endif
}

// this routine writes the string to the LCD
// display after setting the address to add
static void lcd_dis(int add, char *string)
{
    cyg_uint8 stat;
    int i;

    // write the string out to the display stopping when we reach 0
    for (i = 0; *string != '\0'; i++)
    {
        // wait for not busy
        do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);

        // write the address
        HAL_WRITE_UINT8 (LCD_CMD, (LCD_CMD_ADD + add));
        add++;

        // wait for not busy
        do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);

        // write the data
        HAL_WRITE_UINT8 (LCD_DATA, *string++);
    }
}

void hal_diag_write_char(char c)
{
    int i;
    
    // ignore CR
    if( c == '\r' ) return;
    
    if( c == '\n' )
    {
        lcd_dis( LCD_LINE0, &lcd_line[lcd_curline^1][0] );
        lcd_dis( LCD_LINE1, &lcd_line[lcd_curline][0] );            

        // Do a line feed
        lcd_curline ^= 1;
        lcd_linepos = 0;
        
        for( i = 0; i < LCD_LINE_LENGTH; i++ )
            lcd_line[lcd_curline][i] = ' ';

        return;
    }

    // Truncate long lines
    if( lcd_linepos >= LCD_LINE_LENGTH ) return;

    lcd_line[lcd_curline][lcd_linepos++] = c;
}

char hal_diag_read_char(void)
{
    return 0;
}

#endif


//---------------------------------------------------------------------------*/
// PromICE AI interface


#if defined(CYG_KERNEL_DIAG_ROMART)

#ifdef CYG_HAL_POWERPC_COGENT
#define PROMICE_AILOC           0xfff00020
#endif

// Add this to the LoadICE config file: ailoc 20 19200
// FIXME: I couldn't get this to work. jskov

#define PROMICE_BUS_SIZE        16
#define PROMICE_BURST_SIZE      1

#if PROMICE_BUS_SIZE == 16

typedef volatile struct
{
    volatile cyg_uint16 zero;
//    cyg_uint16          pad1[PROMICE_BURST_SIZE];
    volatile cyg_uint16 one;
//    cyg_uint16          pad2[PROMICE_BURST_SIZE];
    volatile cyg_uint16 data;
//    cyg_uint16          pad3[PROMICE_BURST_SIZE];
    volatile cyg_uint16 status;

} AISTRUCT;

#endif

AISTRUCT *AI = (AISTRUCT *)PROMICE_AILOC;

#define PROMICE_STATUS_TDA      0x01
#define PROMICE_STATUS_HDA      0x02
#define PROMICE_STATUS_OVR      0x04

void hal_diag_init()
{
    volatile cyg_uint8 junk;
 
    while( AI->status == 0xCC )
        continue;

    junk = AI->data;
}

static void ai_write_char(cyg_uint8 data)
{
    volatile cyg_uint8 junk;
    int i;
    
    // Wait for tda == 0
    while( (AI->status & PROMICE_STATUS_TDA) == PROMICE_STATUS_TDA )
        continue;

    // Send start bit
    junk = AI->one;

    for( i = 0; i < 8; i++ )
    {
        // send ls bit of data
        if( (data & 1) == 1 )
            junk = AI->one;
        else
            junk = AI->zero;

        // shift down for next bit
        data >>= 1;
    }

    // Send stop bit
    junk = AI->one;

    // all done
}

void hal_diag_write_char(char c)
{
    ai_write_char((cyg_uint8)c);
}

#endif

//-----------------------------------------------------------------------------
// End of hal_diag.c
