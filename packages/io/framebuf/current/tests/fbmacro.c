//==========================================================================
//
//      fbmacro.c
//
//      Framebuffer testcase
//
//==========================================================================
//###ECOSGPLCOPYRIGHTBEGIN####
//-------------------------------------------
// This file is part of eCos, the Embedded Configurable Operating System.
// Copyright (C) 2008 Free Software Foundation, Inc.
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
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2008-10-06
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/system.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/testcase.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/hal/hal_intr.h>
#include <pkgconf/io_framebuf.h>
#include <cyg/io/framebuf.h>

#if defined(CYGDAT_IO_FRAMEBUF_TEST_DEVICE)
# define FRAMEBUF   CYGDAT_IO_FRAMEBUF_TEST_DEVICE
#elif defined(CYGDAT_IO_FRAMEBUF_DEFAULT_TEST_DEVICE)
# define FRAMEBUF   CYGDAT_IO_FRAMEBUF_DEFAULT_TEST_DEVICE
#else
# define NA_MSG "No framebuffer test device selected"
#endif

#ifndef NA_MSG
# if (CYG_FB_DEPTH(FRAMEBUF) < 4)
#  define NA_MSG    "Testcase requires a display depth of at least 4 bits/pixel"
# endif
#endif

#ifdef NA_MSG
void
cyg_start(void)
{
    CYG_TEST_INIT();
    CYG_TEST_NA(NA_MSG);
}
#else

#define STRING1(_a_) # _a_
#define STRING(_a_) STRING1(_a_)

// A simple bitmap. Black on the outside, then blue,
// then green, and a red centre.
static cyg_uint8    bitmap8[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static cyg_uint16   bitmap16[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

static cyg_uint32   bitmap32[8][8] = {
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x04, 0x04, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x02, 0x02, 0x02, 0x02, 0x01, 0x00 },
    { 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 },
    { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
};

// The colours used by this test code. Default to the standard palette
// but if running on a true colour display then adjust.
static cyg_ucount32 colours[16]  = {
    CYG_FB_DEFAULT_PALETTE_BLACK,
    CYG_FB_DEFAULT_PALETTE_BLUE,
    CYG_FB_DEFAULT_PALETTE_GREEN,
    CYG_FB_DEFAULT_PALETTE_CYAN,
    CYG_FB_DEFAULT_PALETTE_RED,
    CYG_FB_DEFAULT_PALETTE_MAGENTA,
    CYG_FB_DEFAULT_PALETTE_BROWN,
    CYG_FB_DEFAULT_PALETTE_LIGHTGREY,
    CYG_FB_DEFAULT_PALETTE_DARKGREY,
    CYG_FB_DEFAULT_PALETTE_LIGHTBLUE,
    CYG_FB_DEFAULT_PALETTE_LIGHTGREEN,
    CYG_FB_DEFAULT_PALETTE_LIGHTCYAN,
    CYG_FB_DEFAULT_PALETTE_LIGHTRED,
    CYG_FB_DEFAULT_PALETTE_LIGHTMAGENTA,
    CYG_FB_DEFAULT_PALETTE_YELLOW,
    CYG_FB_DEFAULT_PALETTE_WHITE
};

#define BLACK        colours[0x00]
#define BLUE         colours[0x01]
#define GREEN        colours[0x02]
#define CYAN         colours[0x03]
#define RED          colours[0x04]
#define MAGENTA      colours[0x05]
#define BROWN        colours[0x06]
#define LIGHTGREY    colours[0x07]
#define DARKGREY     colours[0x08]
#define LIGHTBLUE    colours[0x09]
#define LIGHTGREEN   colours[0x0A]
#define LIGHTCYAN    colours[0x0B]
#define LIGHTRED     colours[0x0C]
#define LIGHTMAGENTA colours[0x0D]
#define YELLOW       colours[0x0E]
#define WHITE        colours[0x0F]

static void
reset_colours_to_true(void)
{
#if (CYG_FB_FLAGS0_TRUE_COLOUR & CYG_FB_FLAGS0(FRAMEBUF))    
    int i, j;
    
    for (i = 0; i < 16; i++) {
        colours[i] = CYG_FB_MAKE_COLOUR(FRAMEBUF,
                                        cyg_fb_palette_vga[i + i + i], cyg_fb_palette_vga[i + i + i + 1],cyg_fb_palette_vga[i + i + i + 2]);
    }
    for (i = 0; i < 8; i++) {
        for (j = 0; j < 8; j++) {
            bitmap16[i][j]  = colours[bitmap16[i][j]];
            bitmap32[i][j]  = colours[bitmap32[i][j]];
        }
    }
#endif    
}

void
cyg_start(void)
{
    int             i, j;
    int             x = 0, y = 0;
    int             result;
    cyg_ucount16    block_width;
    
    CYG_FB_PIXEL0_VAR(FRAMEBUF);
    CYG_FB_PIXEL1_VAR(FRAMEBUF);
    
#define DEPTH   CYG_FB_DEPTH(FRAMEBUF)
#define WIDTH   CYG_FB_WIDTH(FRAMEBUF)
#define HEIGHT  CYG_FB_HEIGHT(FRAMEBUF)    
    
    CYG_TEST_INIT();

    diag_printf("Frame buffer %s\n", STRING(FRAMEBUF));
    diag_printf("Depth %d, width %d, height %d\n", DEPTH, WIDTH, HEIGHT);
    
    result = CYG_FB_ON(FRAMEBUF);

    if (CYG_FB_FLAGS0(FRAMEBUF) & CYG_FB_FLAGS0_TRUE_COLOUR) {
        reset_colours_to_true();
    }
    
    // A white background
    CYG_FB_FILL_BLOCK(FRAMEBUF, 0, 0, WIDTH, HEIGHT, WHITE);
    // A black block in the middle, 25 pixels in.
    CYG_FB_FILL_BLOCK(FRAMEBUF, 32, 32, WIDTH - 64, HEIGHT - 64, BLACK);

    // Four diagonal lines in the corners. Red in the top left, blue in the top right,
    // green in the bottom left, and yellow in the bottom right.
    for (i = 0; i < 32; i++) {
        CYG_FB_WRITE_PIXEL(FRAMEBUF, i,               i,                RED);
        CYG_FB_WRITE_PIXEL(FRAMEBUF, (WIDTH - 1) - i, i,                BLUE);
        CYG_FB_WRITE_PIXEL(FRAMEBUF, i,               (HEIGHT - 1) - i, GREEN);
        CYG_FB_WRITE_PIXEL(FRAMEBUF, (WIDTH - 1) - i, (HEIGHT - 1) - i, YELLOW);
    }

    // Horizontal and vertical lines. Cyan at the top, magenta on the bottom,
    // brown on the left, lightgrey on the right.
    CYG_FB_WRITE_HLINE(FRAMEBUF, 32,         16,            WIDTH - 64, CYAN);
    CYG_FB_WRITE_HLINE(FRAMEBUF, 32,         HEIGHT - 16,   WIDTH - 64, MAGENTA);
    CYG_FB_WRITE_VLINE(FRAMEBUF, 16,         32,            HEIGHT - 64, BROWN);
    CYG_FB_WRITE_VLINE(FRAMEBUF, WIDTH - 16, 32,            HEIGHT - 64, LIGHTGREY);

    // Top left, diagonal lines away from 0,0 with increasing spacing horizontally
    for (i = 0; i < 16; i++) {
        CYG_FB_PIXEL0_SET(FRAMEBUF, i + 16, i);
        for (j = 0; j < 16; j++) {
            CYG_FB_PIXEL0_WRITE(FRAMEBUF, colours[i]);
            CYG_FB_PIXEL0_ADDX(FRAMEBUF, j);
        }
    }

    // Top right, diagonal lines away from the corner, with increasing spacing horizontally
    for (i = 0; i < 16; i++) {
        CYG_FB_PIXEL0_SET(FRAMEBUF, WIDTH - (i + 16), i);
        for (j = 0; j < 16; j++) {
            CYG_FB_PIXEL0_WRITE(FRAMEBUF, colours[i]);
            CYG_FB_PIXEL0_ADDX(FRAMEBUF, -1 * j);
        }
    }

    // Top left, diagonal lines away from the corner, with increasing spacing vertically
    for (i = 0; i < 16; i++) {
        CYG_FB_PIXEL0_SET(FRAMEBUF, i, i + 16);
        for (j = 0; j < 16; j++) {
            CYG_FB_PIXEL0_WRITE(FRAMEBUF, colours[i]);
            CYG_FB_PIXEL0_ADDY(FRAMEBUF, j);
        }
    }
    // Bottom left, diagonal lines away from the corner, with increasing spacing vertically
    for (i = 0; i < 16; i++) {
        CYG_FB_PIXEL0_SET(FRAMEBUF, i, HEIGHT - (i + 16));
        for (j = 0; j < 16; j++) {
            CYG_FB_PIXEL0_WRITE(FRAMEBUF, colours[i]);
            CYG_FB_PIXEL0_ADDY(FRAMEBUF, -1 * j);
        }
    }

    // Thin vertical bars in the top-middle of the screen, between the hline and the box.
    // Starting in the center and moving out with increasing spacing.
    for (j = 0; j < 8; j++) {
        CYG_FB_PIXEL0_SET(FRAMEBUF, (WIDTH / 2) - 2, 20 + j);
        CYG_FB_PIXEL0_GET(FRAMEBUF, x, y);
        CYG_FB_PIXEL1_SET(FRAMEBUF, x + 3, y);
        for (i = 0; i < 16; i++) {
            CYG_FB_PIXEL0_ADDX(FRAMEBUF, -1 * i);
            CYG_FB_PIXEL1_ADDX(FRAMEBUF, i);
            CYG_FB_PIXEL0_WRITE(FRAMEBUF, colours[i]);
            CYG_FB_PIXEL1_WRITE(FRAMEBUF, colours[i]);
        }
    }
    
    
    if (8 == DEPTH) {
        CYG_FB_WRITE_BLOCK(FRAMEBUF, 0, HEIGHT / 2, 8, 8, bitmap8, 0, 8);
    } else if (16 == DEPTH) {
        CYG_FB_WRITE_BLOCK(FRAMEBUF, 0, HEIGHT / 2, 8, 8, bitmap16, 0, 8);
    } else if (32 == DEPTH) {
        CYG_FB_WRITE_BLOCK(FRAMEBUF, 0, HEIGHT / 2, 8, 8, bitmap32, 0, 8);
    }

    block_width     = (WIDTH - 100) / 14;
    for (i = 1; i <= 14; i++) {
        CYG_FB_FILL_BLOCK(FRAMEBUF, 50 + ((i - 1) * block_width), 50, block_width, HEIGHT - 100, colours[i]);
    }

    CYG_TEST_EXIT("Done");
}
#endif  // NA_MSG
