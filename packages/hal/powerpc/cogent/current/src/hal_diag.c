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
// Author(s):    nickg, jskov
// Contributors: nickg, jskov
// Date:         1999-03-23
// Purpose:      HAL diagnostic output
// Description:  Implementations of HAL diagnostic output support.
//
//####DESCRIPTIONEND####
//
//=============================================================================

#include <pkgconf/hal.h>
#include <pkgconf/hal_powerpc_cogent.h> // CYGHWR_HAL_POWERPC_COGENT_DIAG_PORT

#include <cyg/hal/hal_diag.h>           // our header.

#if defined(CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS)
#include <cyg/hal/hal_stub.h>           // hal_output_gdb_string
#endif

#include <cyg/infra/cyg_type.h>         // base types, externC
#include <cyg/hal/hal_io.h>             // IO macros
#include <cyg/hal/hal_intr.h>           // Interrupt macros

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

// Always use LCD when building a GDB stub ROM.
#ifdef CYG_HAL_STARTUP_STUBS
#undef CYG_KERNEL_DIAG_SERIAL
#undef CYG_KERNEL_DIAG_ROMART

#define CYG_KERNEL_DIAG_LCD
#endif

// Assumption: all diagnostic output must be GDB packetized unless
// this is a ROM (i.e.  totally stand-alone) system.
#ifdef CYG_HAL_STARTUP_ROM
#define HAL_DIAG_USES_HARDWARE
#endif


//-----------------------------------------------------------------------------
// Serial diag functions.
#ifdef CYG_KERNEL_DIAG_SERIAL

// Include the serial driver.
#define CYG_CMA_PORT CYGHWR_HAL_POWERPC_COGENT_DIAG_PORT
#include <cma_ser.inl>

#ifdef HAL_DIAG_USES_HARDWARE

void hal_diag_init(void)
{
    hal_cma_init_serial();
}

void hal_diag_write_char(char __c)
{
    hal_cma_put_char(__c);
}

void hal_diag_read_char(char *c)
{
    *c = (char) hal_cma_get_char();
}

#else  // ifdef HAL_DIAG_USES_HARDWARE

// Initialize diag port
void hal_diag_init(void)
{
#ifdef CYGDBG_HAL_DEBUG_GDB_INCLUDE_STUBS
    // assume GDB channel is already set up.
    if (0) hal_cma_init_serial();       // avoids compiler warning
#else
    hal_cma_init_serial();
#endif
      
}

void 
hal_diag_write_char_serial( char c )
{
    unsigned long __state;
    HAL_DISABLE_INTERRUPTS(__state);
    hal_cma_put_char(c);
    HAL_RESTORE_INTERRUPTS(__state);
}

void
hal_diag_read_char(char *c)
{
    *c = (char) hal_cma_get_char();
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
        
        HAL_DISABLE_INTERRUPTS(old);
        
        while(1)
        {
            char c1;
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

            // Wait for the ACK character '+' from GDB here and handle
            // receiving a ^C instead.
            hal_diag_read_char(&c1);

            if( c1 == '+' )
                break;              // a good acknowledge

#ifdef CYGDBG_HAL_DEBUG_GDB_BREAK_SUPPORT
            if( 3 == c1 ) {
                // Ctrl-C: breakpoint.
                cyg_hal_gdb_interrupt(
                    (target_register_t)__builtin_return_address(0));
                break;
            }
#endif
            // otherwise, loop round again
        }
        
        pos = 0;

        // And re-enable interrupts
        HAL_RESTORE_INTERRUPTS(old);
        
    }
}

#endif // ifdef HAL_DIAG_USES_HARDWARE

#endif // ifdef CYG_KERNEL_DIAG_SERIAL

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

externC void diag_write_string (const char*);

void hal_diag_init()
{
    cyg_uint8 stat;
    int i;
    
    // wait for not busy
    // Note: It seems that the LCD isn't quite ready to process commands
    // when it clears the BUSY flag. Reading the status address an extra
    // time seems to give it enough breathing room.
    do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);
    HAL_READ_UINT8 (LCD_STAT, stat);

    // configure the lcd for 8 bits/char, 2 lines
    // and 5x7 dot matrix
    HAL_WRITE_UINT8 (LCD_CMD, LCD_CMD_MODE);

    // wait for not busy
    do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);
    HAL_READ_UINT8 (LCD_STAT, stat);

    // turn the LCD display on
    HAL_WRITE_UINT8 (LCD_CMD, LCD_CMD_DON);

    lcd_curline = 0;
    lcd_linepos = 0;

    for( i = 0; i < LCD_LINE_LENGTH; i++ )
        lcd_line[0][i] = lcd_line[1][i] = ' ';

    lcd_line[0][LCD_LINE_LENGTH] = lcd_line[1][LCD_LINE_LENGTH] = 0;

    lcd_dis( LCD_LINE0, lcd_line[0] );
    lcd_dis( LCD_LINE1, lcd_line[1] );


#ifdef CYG_HAL_STARTUP_STUBS
    // It's handy to have the LCD initialized at reset when using it
    // for debugging output.
    {
        diag_write_string ("eCos ROM   " __TIME__ "\n");
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

    // wait for not busy (see Note in hal_diag_init above)
    do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);
    HAL_READ_UINT8 (LCD_STAT, stat);

    // write the address
    HAL_WRITE_UINT8 (LCD_CMD, (LCD_CMD_ADD + add));

    // write the string out to the display stopping when we reach 0
    for (i = 0; *string != '\0'; i++)
    {
        // wait for not busy
        do { HAL_READ_UINT8 (LCD_STAT, stat); } while (stat & LCD_STAT_BUSY);
        HAL_READ_UINT8 (LCD_STAT, stat);

        // write the data
        HAL_WRITE_UINT8 (LCD_DATA, *string++);
    }
}

void hal_diag_write_char(char c)
{
    unsigned long __state;
    int i;
    
    // ignore CR
    if( c == '\r' ) return;
    
    HAL_DISABLE_INTERRUPTS(__state)
    if( c == '\n' )
    {
        lcd_dis( LCD_LINE0, &lcd_line[lcd_curline^1][0] );
        lcd_dis( LCD_LINE1, &lcd_line[lcd_curline][0] );            

        // Do a line feed
        lcd_curline ^= 1;
        lcd_linepos = 0;
        
        for( i = 0; i < LCD_LINE_LENGTH; i++ )
            lcd_line[lcd_curline][i] = ' ';

        HAL_RESTORE_INTERRUPTS(__state);
        return;
    }

    // Only allow to be output if there is room on the LCD line
    if( lcd_linepos < LCD_LINE_LENGTH )
        lcd_line[lcd_curline][lcd_linepos++] = c;

    HAL_RESTORE_INTERRUPTS(__state);
}

void hal_diag_read_char(char* c) {}

#endif


//---------------------------------------------------------------------------*/
// PromICE AI interface


#if defined(CYG_KERNEL_DIAG_ROMART)

#ifdef CYG_HAL_POWERPC_COGENT
#define PROMICE_AILOC           0xfff00020
#endif

// Add this to the LoadICE config file: ailoc 20 19200
// Note: I couldn't get this to work. jskov

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
    unsigned long __state;
    
    HAL_DISABLE_INTERRUPTS(__state)

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

    HAL_RESTORE_INTERRUPTS(__state);

    // all done
}

void hal_diag_write_char(char c)
{
    ai_write_char((cyg_uint8)c);
}

#endif

//-----------------------------------------------------------------------------
// End of hal_diag.c
