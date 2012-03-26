// ==========================================================================
//
//      serial.c
//
//      Public Domain Curses for eCos
//
// ===========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc.
//
// eCos is free software; you can redistribute it and/or modify it under    
// the terms of the GNU General Public License as published by the Free     
// Software Foundation; either version 2 or (at your option) any later      
// version.                                                                 
//
// eCos is distributed in the hope that it will be useful, but WITHOUT      
// ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or    
// FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License    
// for more details.                                                        
//
// You should have received a copy of the GNU General Public License        
// along with eCos; if not, write to the Free Software Foundation, Inc.,    
// 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.            
//
// As a special exception, if other files instantiate templates or use      
// macros or inline functions from this file, or you compile this file      
// and link it with other works to produce a work based on this file,       
// this file does not by itself cause the resulting work to be covered by   
// the GNU General Public License. However the source code for this file    
// must still be made available in accordance with section (3) of the GNU   
// General Public License v2.                                               
//
// This exception does not invalidate any other reasons why a work based    
// on this file might be covered by the GNU General Public License.         
// -------------------------------------------                              
// ####ECOSGPLCOPYRIGHTEND####                                              
// ===========================================================================
// ===========================================================================
// #####DESCRIPTIONBEGIN####
//
// Author(s):    Sergei Gavrikov
// Contributors: Sergei Gavrikov
// Date:         2009-04-24
// Purpose:      PDCurses driver for a serial device
// Description:
//
// ####DESCRIPTIONEND####
//
// ========================================================================*/

#include <cyg/kernel/kapi.h>           // cyg_thread_delay
#include <cyg/infra/diag.h>            // diagnostic routings
#include <stdlib.h>                    // free, calloc

// eCos serial devices
#include <cyg/io/io.h>
#include <pkgconf/io_serial.h>
#include <cyg/io/serialio.h>
#include <cyg/io/config_keys.h>

// PDCurses
#include <pkgconf/pdcurses.h>
#include <curspriv.h>

#define NDEBUG

#if CYGDAT_PDCURSES_DRIVER_USE_VT100_ATTR
# define ENABLE_VT100_ATTR
#endif
#if CYGDAT_PDCURSES_DRIVER_USE_ANSI_COLORS
# define ENABLE_COLOR
#endif
#if CYGDAT_PDCURSES_DRIVER_USE_KEYPAD_KEYS
# define ENABLE_KEYPAD
#endif

static char     device[] = CYGDAT_PDCURSES_DRIVER_SERIAL_DEV;
static cyg_io_handle_t handle = NULL;

// ---------------------------------------------------------------------------
// acs_map --
//
// A port of PDCurses must provide acs_map[], a 128-element array of chtypes,
// with values laid out based on the Alternate Character Set of the VT100
// (see curses.h). PDC_transform_line() must use this table; when it
// encounters a chtype with the A_ALTCHARSET flag set. For far details reffer
// to a pdcurses/IMPLEMNT document.

chtype          acs_map[128];
// ---------------------------------------------------------------------------

unsigned long   pdc_key_modifiers = 0L;

// ---------------------------------------------------------------------------
// atrtab --
//
// An attibutes table (it is used internally for the port's PDC_init_pair(),
// PDC_pair_content().

static struct {
    short           f,
                    b;
} atrtab[PDC_COLOR_PAIRS];
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// __gotoyx --
//
static void
__gotoyx(int y, int x)
{
    char            buf[8] = { '\033', '[', '0', '0', ';', '0', '0', 'H' };
    unsigned int    len = sizeof(buf);

    buf[2] = ((y + 1) / 10) + '0';
    buf[3] = ((y + 1) % 10) + '0';
    buf[5] = ((x + 1) / 10) + '0';
    buf[6] = ((x + 1) % 10) + '0';

    cyg_io_write(handle, buf, &len);
}

// ---------------------------------------------------------------------------
// PDC_gotoyx --
//
// Move the physical cursor (as opposed to the logical cursor affected by
// wmove()) to the given location. This is called mainly from doupdate(). In
// general, this function need not compare the old location with the new one,
// and should just move the cursor unconditionally.
// ---------------------------------------------------------------------------

void
PDC_gotoyx(int y, int x)
{

#ifdef DEBUG
    diag_printf("PDC_gotoyx called: x %d y %d\n", x, y);
#endif

    __gotoyx(y, x);

    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN, NULL,
                      NULL);
}

// ---------------------------------------------------------------------------
// PDC_transform_line --
//
// The core output routine. It takes len chtype entities from srcp (a pointer
// into curscr) and renders them to the physical screen at line lineno,
// column x. It must also translate characters 0-127 via acs_map[], if
// they're flagged with A_ALTCHARSET in the attribute portion of the chtype.
// ---------------------------------------------------------------------------

static short    curstoansi[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
static chtype   prev_attr = A_PROTECT;

void
PDC_transform_line(int lineno, int x, int len, const chtype *srcp)
{
    int             i;
    chtype          attr;

#ifdef DEBUG
    diag_printf("PDC_transform_line called: lineno %d curpos %d len %d\n",
                lineno, x, len);
#endif

    __gotoyx(lineno, x);

    for (i = 0; i < len; i++) {
        unsigned char   byte;
        chtype          ch = srcp[i];
        unsigned int    single = 1;

#if defined(ENABLE_COLOR) || defined(ENABLE_VT100_ATTR)
        attr = ch & A_ATTRIBUTES;
        // <ESC>[#; ... m
        if (prev_attr != attr) {
            short           fg,
                            bg,
                            pair;
            char            escape[32];
            unsigned int    count;

            count = 0;
            escape[count++] = '\x1b';
            escape[count++] = '[';
            escape[count++] = '0';
# ifdef ENABLE_COLOR
            pair = PAIR_NUMBER(attr);
            fg = curstoansi[atrtab[pair].f];
            bg = curstoansi[atrtab[pair].b];
            escape[count++] = ';';
            escape[count++] = '3';
            escape[count++] = '0' + fg;
            escape[count++] = ';';
            escape[count++] = '4';
            escape[count++] = '0' + bg;
# endif                         // ENABLE_COLOR
# ifdef ENABLE_VT100_ATTR
            if (attr & A_BOLD) {
                escape[count++] = ';';
                escape[count++] = '1';
            }
            if (attr & A_UNDERLINE) {
                escape[count++] = ';';
                escape[count++] = '4';
            }
            if (attr & A_BLINK) {
                escape[count++] = ';';
                escape[count++] = '5';
            }
            if (attr & A_REVERSE) {
                escape[count++] = ';';
                escape[count++] = '7';
            }
            if (attr & A_INVIS) {
                escape[count++] = ';';
                escape[count++] = '8';
            }
# endif                         // ENABLE_VT100_ATTR
            escape[count++] = 'm';
            cyg_io_write(handle, escape, &count);

            prev_attr = attr;
        }
#endif                          // ENABLE_COLOR || ENABLE_VT100_ATTR
#ifdef CHTYPE_LONG
        if (ch & A_ALTCHARSET && !(ch & 0xff80))
            ch = acs_map[ch & 0x7f];
#endif
        ch &= 0xff;
        byte = (unsigned char) ch;
        cyg_io_write(handle, &byte, &single);
    }

    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN, NULL,
                      NULL);
}

// ---------------------------------------------------------------------------
// PDC_get_columns --
//
// Returns the size of the screen in columns. It's used in resize_term() to
// set the new value of COLS. (Some existing implementations also call it
// internally from PDC_scr_open(), but this is not required.)
// ---------------------------------------------------------------------------

int
PDC_get_columns(void)
{
    return CYGDAT_PDCURSES_DRIVER_SCREEN_COLUMNS;
}

// ---------------------------------------------------------------------------
// PDC_get_cursor_mode --
//
// Returns the size/shape of the cursor. The format of the result is
// unspecified, except that it must be returned as an int. This function is
// called from initscr(), and the result is stored in SP->orig_cursor, which
// is used by PDC_curs_set() to determine the size/shape of the cursor in
// normal visibility mode (curs_set(1)).
// ---------------------------------------------------------------------------

int
PDC_get_cursor_mode(void)
{
    return 0;
}

// ---------------------------------------------------------------------------
// PDC_get_rows --
//
// Returns the size of the screen in rows. It's used in resize_term() to set
// the new value of LINES. (Some existing implementations also call it
// internally from PDC_scr_open(), but this is not required.)
// ---------------------------------------------------------------------------

int
PDC_get_rows(void)
{
    return CYGDAT_PDCURSES_DRIVER_SCREEN_ROWS;
}

// ---------------------------------------------------------------------------
// PDC_check_key --
//
// Keyboard/mouse event check, called from wgetch(). Returns TRUE if there's
// an event ready to process. This function must be non-blocking.
// ---------------------------------------------------------------------------

static int      key_code = ERR;
static union {
    cyg_uint32      key;
    cyg_uint8       seq[4];
} kk = {0};

#define SYM kk.seq
#define FLUSH() kk.key=0

#if (CYG_BYTEORDER == CYG_LSBFIRST)
# define POP(x) kk.key>>=8*(x)
#else
# define POP(x) kk.key<<=8*(x)
#endif

bool
PDC_check_key(void)
{
    unsigned int    len,
                    count;
    cyg_serial_buf_info_t dev_buf_conf;

    len = sizeof(dev_buf_conf);
    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_BUFFER_INFO,
                      &dev_buf_conf, &len);

#ifndef ENABLE_KEYPAD
    return (dev_buf_conf.rx_count > 0);
#else
    if ((count = dev_buf_conf.rx_count) > 0) {
        if (SYM[0] == '\0') {
            count = min(count, 4);
            cyg_io_read(handle, &SYM[0], &count);
        } else if (SYM[1] == '\0') {
            count = min(count, 3);
            cyg_io_read(handle, &SYM[1], &count);
        } else if (SYM[2] == '\0') {
            count = min(count, 2);
            cyg_io_read(handle, &SYM[2], &count);
        } else if (SYM[3] == '\0') {
            count = min(count, 1);
            cyg_io_read(handle, &SYM[3], &count);
        }
    }
    if (SYM[0] == '\0')
        return FALSE;
    switch (SYM[0]) {
    case '\x1b':
        switch (SYM[1]) {
        case '[':
            switch (SYM[2]) {
                // ^[[A -- KEY_UP
            case 'A':
                key_code = KEY_UP;
                POP(3);
                return TRUE;
                // ^[[B -- KEY_DOWN
            case 'B':
                key_code = KEY_DOWN;
                POP(3);
                return TRUE;
                // ^[[C -- KEY_RIGHT
            case 'C':
                key_code = KEY_RIGHT;
                POP(3);
                return TRUE;
                // ^[[D -- KEY_LEFT
            case 'D':
                key_code = KEY_LEFT;
                POP(3);
                return TRUE;
            case '1':
            case '4':
            case '5':
            case '6':
                switch (SYM[3]) {
                case '~':
                    switch (SYM[2]) {
                        // ^[[1~ -- KEY_HOME
                    case '1':
                        key_code = KEY_HOME;
                        FLUSH();
                        return TRUE;
                        // ^[[4~ -- KEY_END
                    case '4':
                        key_code = KEY_END;
                        FLUSH();
                        return TRUE;
                        // ^[[5~ -- KEY_PPAGE
                    case '5':
                        key_code = KEY_PPAGE;
                        FLUSH();
                        return TRUE;
                        // ^[[6~ -- KEY_NPAGE
                    case '6':
                        key_code = KEY_NPAGE;
                        FLUSH();
                        return TRUE;
                    }
                default:
                    break;
                }
            default:
                break;
            }
        case 'O':
            switch (SYM[2]) {
                // ^[OF -- KEY_END
            case 'F':
                key_code = KEY_END;
                POP(3);
                return TRUE;
                // ^[OH -- KEY_HOME
            case 'H':
                key_code = KEY_HOME;
                POP(3);
                return TRUE;
            default:
                break;
            }
        default:
            break;
        }
    default:
        break;
    }
    key_code = SYM[0];
    POP(1);
    return TRUE;
#endif                          // ENABLE_KEYPAD
}

// ---------------------------------------------------------------------------
// PDC_flushinp --
//
// This is the core of flushinp(). It discards any pending key or mouse
// events, removing them from any internal queue and from the OS queue, if
// applicable.
// ---------------------------------------------------------------------------

void
PDC_flushinp(void)
{
    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_INPUT_FLUSH, NULL,
                      NULL);
#ifdef ENABLE_KEYPAD
    FLUSH();
#endif
}

// ---------------------------------------------------------------------------
// PDC_get_key --
//
// Get the next available key, or mouse event (indicated by a return of
// KEY_MOUSE), and remove it from the OS' input queue, if applicable. This
// function is called from wgetch(). This function may be blocking, and
// traditionally is; but it need not be. If a valid key or mouse event cannot
// be returned, for any reason, this function returns -1. For more details
// read `pdcurses/IMPLEMNT' document.
// ---------------------------------------------------------------------------

int
PDC_get_key(void)
{
#ifndef ENABLE_KEYPAD
    int             input = 0;
    unsigned int    count = 1;

    if (ENOERR == cyg_io_read(handle, &input, &count))
        return input;

    return ERR;
#else
    return key_code;
#endif                          // ENABLE_KEYPAD
}

// ---------------------------------------------------------------------------
// PDC_modifiers_set --
//
// Called from PDC_return_key_modifiers(). If your platform needs to do
// anything in response to a change in SP->return_key_modifiers, do it here.
// Returns OK or ERR, which is passed on by the caller.
// ---------------------------------------------------------------------------

int
PDC_modifiers_set(void)
{
    return ERR;
}

// ---------------------------------------------------------------------------
// PDC_mouse_set --
//
// Called by mouse_set(), mouse_on(), and mouse_off() -- all the functions
// that modify SP->_trap_mbe. If your platform needs to do anything in
// response to a change in SP->_trap_mbe (for example, turning the mouse
// cursor on or off), do it here. Returns OK or ERR, which is passed on by
// the caller.
// ---------------------------------------------------------------------------

int
PDC_mouse_set(void)
{
    return ERR;
}

// ---------------------------------------------------------------------------
// PDC_set_keyboard_binary --
//
// Set keyboard input to "binary" mode. If you need to do something to keep
// the OS from processing ^C, etc. on your platform, do it here. TRUE turns
// the mode on; FALSE reverts it. This function is called from raw() and
// noraw().
// ---------------------------------------------------------------------------

void
PDC_set_keyboard_binary(bool on)
{
#ifdef DEBUF
    diag_printf("PDC_set_keyboard_binary() called: on %d\n", on);
#endif
}

// ---------------------------------------------------------------------------
// PDC_can_change_color --
//
// Returns TRUE if init_color() and color_content() give meaningful results,
// FALSE otherwise. Called from can_change_color().
// ---------------------------------------------------------------------------

bool
PDC_can_change_color(void)
{
    return FALSE;
}

// ---------------------------------------------------------------------------
// PDC_color_content --
//
// The core of color_content(). This does all the work of that function,
// except checking for values out of range and null pointers.
// ---------------------------------------------------------------------------

int
PDC_color_content(short color, short *red, short *green, short *blue)
{
    return ERR;
}

// ---------------------------------------------------------------------------
// PDC_init_color --
//
// The core of init_color(). This does all the work of that function, except
// checking for values out of range.
// ---------------------------------------------------------------------------

int
PDC_init_color(short color, short red, short green, short blue)
{
    return ERR;
}

// ---------------------------------------------------------------------------
// PDC_init_pair --
//
// The core of init_pair(). This does all the work of that function, except
// checking for values out of range. The values passed to this function
// should be returned by a call to PDC_pair_content() with the same pair
// number. PDC_transform_line() should use the specified colors when
// rendering a chtype with the given pair number.
// ---------------------------------------------------------------------------

void
PDC_init_pair(short pair, short fg, short bg)
{
#ifdef DEBUG
    diag_printf("PDC_init_pair called: pair %d fg %d bg %d\n", pair, fg,
                bg);
#endif

    atrtab[pair].f = fg;
    atrtab[pair].b = bg;
}

// ---------------------------------------------------------------------------
// PDC_pair_content --
//
// The core of pair_content(). This does all the work of that function,
// except checking for values out of range and null pointers.
// ---------------------------------------------------------------------------

int
PDC_pair_content(short pair, short *fg, short *bg)
{
    *fg = atrtab[pair].f;
    *bg = atrtab[pair].b;

    return OK;
}

// ---------------------------------------------------------------------------
// PDC_reset_prog_mode --
//
// The non-portable functionality of reset_prog_mode() is handled here --
// whatever's not done in _restore_mode(). In current ports: In OS/2, this
// sets the keyboard to binary mode; in Win32, it enables or disables the
// mouse pointer to match the saved mode; in others it does nothing.
// ---------------------------------------------------------------------------

void
PDC_reset_prog_mode(void)
{
}

// ---------------------------------------------------------------------------
// PDC_reset_shell_mode --
//
// The same thing, for reset_shell_mode(). In OS/2 and Win32, it restores the
// default console mode; in others it does nothing.
// ---------------------------------------------------------------------------

void
PDC_reset_shell_mode(void)
{
}

// ---------------------------------------------------------------------------
// PDC_resize_screen --
//
// This does the main work of resize_term(). It may respond to non-zero
// parameters, by setting the screen to the specified size; to zero
// parameters, by setting the screen to a size chosen by the user at runtime,
// in an unspecified way (e.g., by dragging the edges of the window); or
// both. It may also do nothing, if there's no appropriate action for the
// platform.
// ---------------------------------------------------------------------------

int
PDC_resize_screen(int nlines, int ncols)
{
    if (nlines == 0 && ncols == 0)
        return OK;

    SP->resized = FALSE;
    SP->cursrow = SP->curscol = 0;

    return OK;

}

// ---------------------------------------------------------------------------
// PDC_restore_screen_mode --
//
// Called from _restore_mode() in kernel.c, this function does the actual
// mode changing, if applicable. Currently used only in DOS and OS/2.
// ---------------------------------------------------------------------------

void
PDC_restore_screen_mode(int i)
{
}

// ---------------------------------------------------------------------------
// PDC_save_screen_mode --
//
// Called from _save_mode() in kernel.c, this function saves the actual
// screen mode, if applicable. Currently used only in DOS and OS/2.
// ---------------------------------------------------------------------------

void
PDC_save_screen_mode(int i)
{
}

// ---------------------------------------------------------------------------
// PDC_scr_close --
//
// The platform-specific part of endwin(). It may restore the image of the
// original screen saved by PDC_scr_open(), if the PDC_RESTORE_SCREEN
// environment variable is set; either way, if using an existing terminal,
// this function should restore it to the mode it had at startup, and move
// the cursor to the lower left corner. (The X11 port does nothing.)
// ---------------------------------------------------------------------------

void
PDC_scr_close(void)
{
    handle = NULL;
}

// ---------------------------------------------------------------------------
// PDC_scr_free --
//
// Frees the memory for SP allocated by PDC_scr_open(). Called by
// delscreen().
// ---------------------------------------------------------------------------

void
PDC_scr_free(void)
{
#ifdef DEBUG
    diag_printf("PDC_scr_free called\n");
#endif

    if (SP)
        free(SP);
}

// ---------------------------------------------------------------------------
// PDC_scr_open --
//
// The platform-specific part of initscr(). It's actually called from
// Xinitscr(); the arguments, if present, correspond to those used with
// main(), and may be used to set the title of the terminal window, or for
// other, platform-specific purposes.
// ---------------------------------------------------------------------------

int
PDC_scr_open(int argc, char **argv)
{
    static bool     acs_map_init = false;

    int             i = 0;

#ifdef DEBUG
    diag_printf("PDC_scr_open called\n");
#endif

    if (NULL == handle) {
        unsigned int    wait = 0;
        unsigned int    len = sizeof(wait);

        if (ENOERR != cyg_io_lookup(device, &handle))
            return ERR;

        cyg_io_set_config(handle,
                          CYG_IO_SET_CONFIG_READ_BLOCKING, &wait, &len);
    }

    SP = calloc(1, sizeof(SCREEN));

    if (!SP)
        return ERR;

    SP->cols = PDC_get_columns();
    SP->lines = PDC_get_rows();
    SP->cursrow = SP->curscol = 0;
    SP->raw_inp = SP->raw_out = TRUE;

    if (!acs_map_init)
        for (i = 0; i < 128; i++) {
            // Square shapes, no more
            switch (i) {
            case 'j':
            case 'k':
            case 'l':
            case 'm':
                acs_map[i] = '+';
                break;
            case 'q':
                acs_map[i] = '-';
                break;
            case 'x':
                acs_map[i] = '|';
                break;
            default:
                acs_map[i] = (chtype) i | A_ALTCHARSET;
            }
        }

    return OK;
}

// ---------------------------------------------------------------------------
// PDC_curs_set --
//
// Called from curs_set(). Changes the appearance of the cursor -- 0 turns it
// off, 1 is normal (the terminal's default, if applicable, as determined by
// SP->orig_cursor), and 2 is high visibility. The exact appearance of these
// modes is not specified.
// ---------------------------------------------------------------------------

int
PDC_curs_set(int visibility)
{
    int             vis;
    char            buf[6] = { '\033', '[', '?', '2', '5', 'h' };
    unsigned int    len = sizeof(buf);

#ifdef DEBUG
    diag_printf("PDC_curs_set called: visibility %d\n", visibility);
#endif

    vis = SP->visibility;

    if (visibility)
        // <ESC>[?25h -- show cursor
        buf[5] = 'h';
    else
        // <ESC>[?25l -- hide cursor
        buf[5] = 'l';

    cyg_io_write(handle, buf, &len);
    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN, NULL,
                      NULL);

    SP->visibility = vis;

    return vis;
}

// ---------------------------------------------------------------------------
// PDC_beep --
//
// Emits a short audible beep. If this is not possible on your platform, you
// must set SP->audible to FALSE during initialization (i.e., from
// PDC_scr_open() -- not here); otherwise, set it to TRUE. This function is
// called from beep().
// ---------------------------------------------------------------------------

void
PDC_beep(void)
{
    char            buf[1] = { '\007' };
    unsigned int    len = 1;

    cyg_io_write(handle, buf, &len);
    cyg_io_get_config(handle, CYG_IO_GET_CONFIG_SERIAL_OUTPUT_DRAIN, NULL,
                      NULL);
}

// ---------------------------------------------------------------------------
// PDC_napms --
//
// This is the core delay routine, called by napms(). It pauses for about
// (the X/Open spec says "at least") ms milliseconds, then returns. High
// degrees of accuracy and precision are not expected (though desirable, if
// you can achieve them). More important is that this function gives back the
// process' time slice to the OS, so that PDCurses idles at low CPU usage.
// ---------------------------------------------------------------------------

void
PDC_napms(int ms)
{

    // There is alone call of napms() in current PDCurses implementation, its
    // aragument is 50 (50 of 1/20th second ticks).

    cyg_thread_delay(ms / 10);
}

// ---------------------------------------------------------------------------
// PDC_sysname --
//
// Returns a short string describing the platform, such as "DOS" or "X11".
// This is used by longname(). It must be no more than 100 characters; it
// should be much, much shorter (existing platforms use no more than 5).
// ---------------------------------------------------------------------------

const char     *
PDC_sysname(void)
{
    return "ECOS";
}

// ---------------------------------------------------------------------------
// PDC_getclipboard, PDC_clearclipboard, PDC_freeclipboard PDC_setclipboard,
// PDC_get_input_fd, PDC_set_blink, PDC_set_title --
//
// The following functions are implemented in the platform directories, but
// are accessed directly by apps. Refer to the user documentation for their
// descriptions:
// ---------------------------------------------------------------------------

// ---------------------------------------------------------------------------
// PDC_getclipboard --
//
int
PDC_getclipboard(char **contents, long *length)
{
    return PDC_CLIP_ACCESS_ERROR;
}

// ---------------------------------------------------------------------------
// PDC_clearclipboard --
//
int
PDC_clearclipboard(void)
{
    return PDC_CLIP_ACCESS_ERROR;
}

// ---------------------------------------------------------------------------
// PDC_freeclipboard --
//
int
PDC_freeclipboard(char *contents)
{
    return PDC_CLIP_ACCESS_ERROR;
}

// ---------------------------------------------------------------------------
// PDC_setclipboard --
//
int
PDC_setclipboard(const char *contents, long length)
{
    return PDC_CLIP_ACCESS_ERROR;
}

// ---------------------------------------------------------------------------
// PDC_get_input_fd --
//
unsigned long
PDC_get_input_fd(void)
{
    return -1;
}

// ---------------------------------------------------------------------------
// PDC_set_blink --
//
int
PDC_set_blink(bool blinkon)
{
#ifdef DEBUG
    diag_printf("PDC_set_blink called: blinkon %d\n", blinkon);
#endif

    // start_color() guides that PDC_set_blink() should also set COLORS, to 8
    // or 16.
#ifdef ENABLE_COLOR
    COLORS = 8;
#else
    COLORS = 0;
#endif

    return blinkon ? ERR : OK;
}

// ---------------------------------------------------------------------------
// PDC_set_title --
//
void
PDC_set_title(const char *title)
{
}

// ---------------------------------------------------------------------------
// EOF serial.c
