//==========================================================================
//
//      olpce2294_misc.c
//
//      HAL misc board support code for Olimex LPC-E2294 development board
//
//==========================================================================
//####ECOSGPLCOPYRIGHTBEGIN####
// -------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
// Copyright (C) 2008 eCosCentric Limited
//
// eCos is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free
// Software Foundation; either version 2 or (at your option) any later version.
//
// eCos is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
// for more details.
//
// You should have received a copy of the GNU General Public License along
// with eCos; if not, write to the Free Software Foundation, Inc.,
// 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
//
// As a special exception, if other files instantiate templates or use macros
// or inline functions from this file, or you compile this file and link it
// with other works to produce a work based on this file, this file does not
// by itself cause the resulting work to be covered by the GNU General Public
// License. However the source code for this file must still be made available
// in accordance with section (3) of the GNU General Public License.
//
// This exception does not invalidate any other reasons why a work based on
// this file might be covered by the GNU General Public License.
// -------------------------------------------
//####ECOSGPLCOPYRIGHTEND####
//==========================================================================
//#####DESCRIPTIONBEGIN####
//
// Author(s):     Sergei Gavrikov
// Contributors:  Sergei Gavrikov
// Date:          2008-08-31
// Purpose:       HAL board support
// Description:   Implementations of HAL board interfaces
//
//####DESCRIPTIONEND####
//
//========================================================================*/

#include <pkgconf/hal.h>
#include <cyg/hal/hal_io.h>     // IO macros
#include <cyg/infra/cyg_type.h> // base types

#include <cyg/hal/var_io.h>
#include <cyg/hal/plf_io.h>

#include <cyg/hal/hal_intr.h>   // Interrupt macros
#include <cyg/hal/hal_arch.h>   // SAVE/RESTORE GP
#include <cyg/hal/hal_if.h>
#include <cyg/hal/hal_diag.h>   // HAL_DELAY_US

extern void cyg_hal_plf_serial_init (void);

static void cyg_hal_plf_lcd_init (void);

// There are no diagnostic leds on the board, but there is a LCD there with a
// BACKLIGHT feature. So, we can drive by BACKLIGHT put on a cathode a static
// signal.

inline static void
_fake_led (bool state)
{
    HAL_WRITE_UINT32 (CYGARC_HAL_LPC2XXX_REG_IO_BASE +
                      CYGARC_HAL_LPC2XXX_REG_IO0CLR, (1 << 10));
    if (state) {
        HAL_WRITE_UINT32 (CYGARC_HAL_LPC2XXX_REG_IO_BASE +
                          CYGARC_HAL_LPC2XXX_REG_IO0SET, (1 << 10));
    }
}

//--------------------------------------------------------------------------
// hal_lpc2xxx_set_leds --
//
void
hal_lpc2xxx_set_leds (int mask)
{
    _fake_led (mask & 1);
}

//--------------------------------------------------------------------------
// cyg_hal_plf_comms_init  --
//
void
cyg_hal_plf_comms_init (void)
{
    static int initialized = 0;

    if (initialized)
        return;
    initialized = 1;

    cyg_hal_plf_serial_init ();
    cyg_hal_plf_lcd_init ();
}

//--------------------------------------------------------------------------
// LCD driver (It is based on powerpc/cogent code)
//
// Olimex LPC-E2294 development board has a LCD 16x2 display (GDM1602K) with
// a built-in controller (KS0066U). The controller is the Hitachi HD44780
// compliant chip. Note: the LCD is driven in a 4-bit mode.
//--------------------------------------------------------------------------

#define LCD_BASE        (void *)NULL

#define LCD_DATA        0x00 // read/write lcd data
#define LCD_STAT        0x08 // read lcd busy status
#define LCD_CMD         0x08 // write lcd command

// status register bit definitions
#define LCD_STAT_BUSY   0x80 // 1 = display busy
#define LCD_STAT_ADD    0x7F // bits 0-6 return current display address

// command register definitions
#define LCD_CMD_RST     0x01 // clear entire display and reset display address
#define LCD_CMD_HOME    0x02 // reset display address and reset any shifting
#define LCD_CMD_ECL     0x04 // move cursor left one position on next write
#define LCD_CMD_ESL     0x05 // shift display left one position on next write
#define LCD_CMD_ECR     0x06 // move cursor right one position on next write
#define LCD_CMD_ESR     0x07 // shift display right one position on next write
#define LCD_CMD_DOFF    0x08 // display off, cursor off, blinking off
#define LCD_CMD_BL      0x09 // blink character at current cursor position
#define LCD_CMD_CUR     0x0A // enable cursor on
#define LCD_CMD_DON     0x0C // turn display on
#define LCD_CMD_CL      0x10 // move cursor left one position
#define LCD_CMD_SL      0x14 // shift display left one position
#define LCD_CMD_CR      0x18 // move cursor right one position
#define LCD_CMD_SR      0x1C // shift display right one position
#define LCD_CMD_MODE    0x28 // sets 4 bits, 2 lines, 5x8 characters
#define LCD_CMD_ACG     0x40 // bits 0-5 sets the character generator address
#define LCD_CMD_ADD     0x80 // bits 0-6 sets the display data addr to line 1 +

// LCD status values
#define LCD_OK          0x00
#define LCD_ERR         0x01

#define LCD_LINE0       0x00 // DRAM address from 0x00 to 0x0f
#define LCD_LINE1       0x40 // DRAM address from 0x40 to 0x4f
#define LCD_LINE_LENGTH 16

static char lcd_line0[LCD_LINE_LENGTH + 1];
static char lcd_line1[LCD_LINE_LENGTH + 1];
static char *lcd_line[2] = { lcd_line0, lcd_line1 };

static int lcd_curline = 0;
static int lcd_linepos = 0;

// the LCD controller <--> MPU interface
#define MPU_DB          0x000000f0      // DB7...DB4 wired to P0.7...P0.4
#define MPU_RS          0x10000000      // RS wired to P0.28
#define MPU_EN          0x20000000      // EN wired to P0.29
#define MPU_RW          0x40000000      // RW wired to P0.30
#define MPU_XX          0x700000f0      // all MPU lines

// LCD DARKLIGHT cathode
#define DARKLIGHT       0x00000020      // P0.10

// Bus timing characteristics for Hitachi HD44780 compliant chips (when Vcc =
// 4.5 to 5.5 V):
// tcycE - enable cycle time, min 500 ns
// tPWEH - enable pulse width (high level), min 230 ns
// tAS - address setup time (RS, R/W to E), min 40 ns
// tAH - address hold time, min 10 ns
// tDDR - data delay time (read operations), max 160 ns
// tDSW - data setup time (write operations), min 80 ns

#define LCD_DELAY_US(_us_)  HAL_DELAY_US (_us_)

// It should overrite the data delay time, i.e. be grater than 160 ns.
// WARNING: be careful with the delay value, more shorter delay would
// occur a dead loop when the BF (busy flag) is checked.
#define LCD_NANO_DELAY()                                        \
    CYG_MACRO_START                                             \
        int i;                                                  \
        for (i = 0; i < 1; i++);                                \
    CYG_MACRO_END

// It should overwrite the tPWEH (enable pulse width)
#define LCD_MICRO_DELAY()                                       \
    CYG_MACRO_START                                             \
        int i;                                                  \
        for (i = 0; i < 3; i++);                                \
    CYG_MACRO_END

// Set RS, R/W to read data
#define LCD_RS_READ_DATA()                                      \
    CYG_MACRO_START                                             \
        IO0SET |= MPU_RW;                                       \
        IO0SET |= MPU_RS;                                       \
    CYG_MACRO_END

// Set RS, R/W to read a busy flag and address counter
#define LCD_RS_READ_STAT()                                      \
    CYG_MACRO_START                                             \
        IO0SET |= MPU_RW;                                       \
        IO0CLR |= MPU_RS;                                       \
    CYG_MACRO_END

// Set RS, R/W to write data
#define LCD_RS_WRITE_DATA()                                     \
    CYG_MACRO_START                                             \
        IO0CLR |= MPU_RW;                                       \
        IO0SET |= MPU_RS;                                       \
    CYG_MACRO_END

// Set RS, R/W to write an instruction
#define LCD_RS_WRITE_CMD()                                      \
    CYG_MACRO_START                                             \
        IO0CLR |= MPU_RW;                                       \
        IO0CLR |= MPU_RS;                                       \
    CYG_MACRO_END

#define LCD_ENABLE_HIGH()                                       \
        IO0SET |= MPU_EN;

#define LCD_ENABLE_LOW()                                        \
        IO0CLR |= MPU_EN;

// High-level enable pulse
#define LCD_ENABLE_PULSE()                                      \
    CYG_MACRO_START                                             \
        LCD_ENABLE_HIGH ();                                     \
        LCD_MICRO_DELAY ();                                     \
        LCD_ENABLE_LOW ();                                      \
        LCD_MICRO_DELAY ();                                     \
    CYG_MACRO_END

// Read a nibble of data from LCD controller
#define LCD_READ_NIBBLE( _n_)                                   \
    CYG_MACRO_START                                             \
        IO0DIR &= ~MPU_DB;                                      \
        _n_ = (IO0PIN & MPU_DB) >> 4;                           \
        _n_ &= 15;                                              \
    CYG_MACRO_END

// Write a nibble of data to LCD controller
#define LCD_WRITE_NIBBLE( _n_)                                  \
    CYG_MACRO_START                                             \
        IO0DIR |= MPU_DB;                                       \
        IO0CLR |= MPU_DB;                                       \
        IO0SET |= ((_n_) & 15) << 4;                            \
    CYG_MACRO_END

//--------------------------------------------------------------------------
// _lcd_read --
//
static void
_lcd_read (int sel, cyg_uint8 * dat)
{
    cyg_uint8 n;

    if (sel == LCD_DATA)
        LCD_RS_READ_DATA ();
    else
        LCD_RS_READ_STAT ();

    // read D7-D4 nibble
    LCD_ENABLE_HIGH ();
    LCD_NANO_DELAY ();
    LCD_READ_NIBBLE (n);
    *dat = n << 4;
    LCD_MICRO_DELAY ();
    LCD_ENABLE_LOW ();
    LCD_MICRO_DELAY ();

    // read D3-D0 nibble
    LCD_ENABLE_HIGH ();
    LCD_NANO_DELAY ();
    LCD_READ_NIBBLE (n);
    *dat |= n;
    LCD_MICRO_DELAY ();
    LCD_ENABLE_LOW ();
    LCD_MICRO_DELAY ();
}

//--------------------------------------------------------------------------
// _lcd_write --
//
static void
_lcd_write (int sel, cyg_uint8 dat)
{
    if (sel == LCD_DATA)
        LCD_RS_WRITE_DATA ();
    else
        LCD_RS_WRITE_CMD ();

    // write D7-D4 nibble
    LCD_WRITE_NIBBLE ((dat >> 4) & 15);
    LCD_ENABLE_HIGH ();
    LCD_MICRO_DELAY ();
    LCD_ENABLE_LOW ();
    LCD_MICRO_DELAY ();

    // write D3-D0 nibble
    LCD_WRITE_NIBBLE (dat & 15);
    LCD_ENABLE_HIGH ();
    LCD_MICRO_DELAY ();
    LCD_ENABLE_LOW ();
    LCD_MICRO_DELAY ();
}

#define LCD_READ( _register_, _data_) \
    _lcd_read(_register_, &(_data_))

#define LCD_WRITE( _register_, _data_) \
    _lcd_write(_register_, _data_)

#ifdef CYG_HAL_STARTUP_ROM
//--------------------------------------------------------------------------
// _lcd_pots_init --
//
// This routine is an early LCD intitializing on power-on event (from KS0066U
// flow diagram).
//
static void
_lcd_pots_init (void)
{

    // around power on
    IO0DIR |= MPU_XX;
    IO0CLR |= MPU_XX;

    // wait for more than 30 ms after Vdd rises to 4.5 V
    LCD_DELAY_US (32000);

    // at first, point on a using of 4-bit mode
    LCD_WRITE_NIBBLE (2);
    LCD_ENABLE_PULSE ();

    LCD_WRITE_NIBBLE (2);
    LCD_ENABLE_PULSE ();
    LCD_WRITE_NIBBLE (8);
    LCD_ENABLE_PULSE ();

    // wait for more than 39 us
    LCD_DELAY_US (40);

    // BF (busy flag) can be checked
}
#endif

// The portion of a code below is a bit adopted the LCD driver code for a
// PowerPC Cogent board.

static void lcd_dis (int add, char *s, cyg_uint8 * base);

//--------------------------------------------------------------------------
// init_lcd_channel --
//
static void
init_lcd_channel (cyg_uint8 * base)
{
    cyg_uint8 stat;
    int i;

#ifdef CYG_HAL_STARTUP_ROM
    _lcd_pots_init ();
#endif

    // Wait for not busy
    do {
        LCD_READ (LCD_STAT, stat);
    } while (stat & LCD_STAT_BUSY);

    // Configure the LCD for 4 bits/char, 2 lines and 5x8 dot matrix
    LCD_WRITE (LCD_CMD, LCD_CMD_MODE);

    // Wait for not busy
    do {
        LCD_READ (LCD_STAT, stat);
    } while (stat & LCD_STAT_BUSY);

    // Turn the LCD display on
    LCD_WRITE (LCD_CMD, LCD_CMD_DON);

    lcd_curline = 0;
    lcd_linepos = 0;

    for (i = 0; i < LCD_LINE_LENGTH; i++)
        lcd_line[0][i] = lcd_line[1][i] = ' ';

    lcd_line[0][LCD_LINE_LENGTH] = lcd_line[1][LCD_LINE_LENGTH] = 0;

    lcd_dis (LCD_LINE0, lcd_line[0], base);
    lcd_dis (LCD_LINE1, lcd_line[1], base);
}

//--------------------------------------------------------------------------
// lcd_dis --
//
// This routine writes the string to the LCD display after setting the address
// to add.
//
static void
lcd_dis (int add, char *s, cyg_uint8 * base)
{
    cyg_uint8 stat;
    int i;

    // Wait for not busy
    do {
        LCD_READ (LCD_STAT, stat);
    } while (stat & LCD_STAT_BUSY);

    // Write the address
    LCD_WRITE (LCD_CMD, (LCD_CMD_ADD + add));

    // Write the string out to the display stopping when we reach 0
    for (i = 0; *s != '\0'; i++) {
        // Wait for not busy
        do {
            LCD_READ (LCD_STAT, stat);
        } while (stat & LCD_STAT_BUSY);

        // Write the data
        LCD_WRITE (LCD_DATA, *s++);
    }
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_putc --
//
void
cyg_hal_plf_lcd_putc (void *__ch_data, cyg_uint8 c)
{
    cyg_uint8 *base = (cyg_uint8 *) __ch_data;
    unsigned long __state;
    int i;

    // Ignore CR
    if (c == '\r')
        return;

    CYGARC_HAL_SAVE_GP ();
    HAL_DISABLE_INTERRUPTS (__state);

    if (c == '\n') {
        lcd_dis (LCD_LINE0, &lcd_line[lcd_curline ^ 1][0], base);
        lcd_dis (LCD_LINE1, &lcd_line[lcd_curline][0], base);

        // Do a line feed
        lcd_curline ^= 1;
        lcd_linepos = 0;

        for (i = 0; i < LCD_LINE_LENGTH; i++)
            lcd_line[lcd_curline][i] = ' ';

        goto _exit_putc;
    }

    // Only allow to be output if there is room on the LCD line
    if (lcd_linepos < LCD_LINE_LENGTH)
        lcd_line[lcd_curline][lcd_linepos++] = c;

_exit_putc:
    HAL_RESTORE_INTERRUPTS (__state);
    CYGARC_HAL_RESTORE_GP ();
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_getc --
//
cyg_uint8
cyg_hal_plf_lcd_getc (void *__ch_data)
{
    return 0;
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_write --
//
static void
cyg_hal_plf_lcd_write (void *__ch_data, const cyg_uint8 * __buf,
                       cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP ();

    while (__len-- > 0)
        cyg_hal_plf_lcd_putc (__ch_data, *__buf++);

    CYGARC_HAL_RESTORE_GP ();
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_read --
//
static void
cyg_hal_plf_lcd_read (void *__ch_data, cyg_uint8 * __buf, cyg_uint32 __len)
{
    CYGARC_HAL_SAVE_GP ();

    while (__len-- > 0)
        *__buf++ = cyg_hal_plf_lcd_getc (__ch_data);

    CYGARC_HAL_RESTORE_GP ();
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_control --
//
static int
cyg_hal_plf_lcd_control (void *__ch_data, __comm_control_cmd_t __func, ...)
{
    return 0;
}

//--------------------------------------------------------------------------
// cyg_hal_plf_lcd_init --
//
static void
cyg_hal_plf_lcd_init (void)
{
    hal_virtual_comm_table_t *comm;
    int cur =
        CYGACC_CALL_IF_SET_CONSOLE_COMM
        (CYGNUM_CALL_IF_SET_COMM_ID_QUERY_CURRENT);

    // Init channel
    init_lcd_channel ((cyg_uint8 *) NULL);

    // Setup procs in the vector table

    // Set channel 2
    CYGACC_CALL_IF_SET_CONSOLE_COMM (2);
    comm = CYGACC_CALL_IF_CONSOLE_PROCS ();
    CYGACC_COMM_IF_CH_DATA_SET (*comm, LCD_BASE);
    CYGACC_COMM_IF_WRITE_SET (*comm, cyg_hal_plf_lcd_write);
    CYGACC_COMM_IF_READ_SET (*comm, cyg_hal_plf_lcd_read);
    CYGACC_COMM_IF_PUTC_SET (*comm, cyg_hal_plf_lcd_putc);
    CYGACC_COMM_IF_GETC_SET (*comm, cyg_hal_plf_lcd_getc);
    CYGACC_COMM_IF_CONTROL_SET (*comm, cyg_hal_plf_lcd_control);

    // Restore original console
    CYGACC_CALL_IF_SET_CONSOLE_COMM (cur);
}

#ifdef HAL_PLF_HARDWARE_INIT
//--------------------------------------------------------------------------
// hal_plf_hardware_init --
//
void
hal_plf_hardware_init (void)
{
    // Cyrrently, it does nothing
}
#endif // HAL_PLF_HARDWARE_INIT

// indent: --indent-level4 -br -nut; vim: expandtab tabstop=4 shiftwidth=4
//--------------------------------------------------------------------------
// EOF olpce2294_misc.c
