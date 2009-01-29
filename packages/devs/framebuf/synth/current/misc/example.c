//==========================================================================
//
//      example.c
//
//      Demonstration of the synthetic target framebuffer capabilities
//
//==========================================================================
// ####ECOSGPLCOPYRIGHTBEGIN####                                            
// -------------------------------------------                              
// This file is part of eCos, the Embedded Configurable Operating System.   
// Copyright (C) 2008 Free Software Foundation, Inc.                        
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
//==========================================================================
//###DESCRIPTIONBEGIN####
//
// Author(s):     bartv
// Date:          2008-10-06
//
//###DESCRIPTIONEND####
//========================================================================

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_arch.h>
#include <cyg/io/framebuf.h>
#include <cyg/kernel/kapi.h>
#include <string.h>
#include <stdio.h>

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

// ----------------------------------------------------------------------------
// FB0. 320x240 32bpp true 0888. Just draw a simple image as per the fb.c
// example and return to main().

static void
fb0_thread(cyg_addrword_t arg)
{
#define FRAMEBUF cyg_synth_fb0    
    static cyg_ucount32 colours[16];
    cyg_ucount16 block_width;
    int i;
    
    for (i = 0; i < 16; i++) {
        colours[i]  = cyg_fb_make_colour(&FRAMEBUF,
                                         cyg_fb_palette_vga[i + i + i], cyg_fb_palette_vga[i + i + i + 1],cyg_fb_palette_vga[i + i + i + 2]);
    }

    cyg_fb_on(&FRAMEBUF);
    // A white background
    cyg_fb_fill_block(&FRAMEBUF, 0, 0, FRAMEBUF.fb_width, FRAMEBUF.fb_height, WHITE);
    // A black block in the middle, 25 pixels in.
    cyg_fb_fill_block(&FRAMEBUF, 25, 25, FRAMEBUF.fb_width - 50, FRAMEBUF.fb_height - 50, BLACK);

    // Four diagonal lines in the corners. Red in the top left, blue in the top right,
    // green in the bottom left, and yellow in the bottom right.
    for (i = 0; i < 25; i++) {
        cyg_fb_write_pixel(&FRAMEBUF, i,                           i,                            RED);
        cyg_fb_write_pixel(&FRAMEBUF, (FRAMEBUF.fb_width - 1) - i, i,                            BLUE);
        cyg_fb_write_pixel(&FRAMEBUF, i,                           (FRAMEBUF.fb_height - 1) - i, GREEN);
        cyg_fb_write_pixel(&FRAMEBUF, (FRAMEBUF.fb_width - 1) - i, (FRAMEBUF.fb_height - 1) - i, YELLOW);
    }

    // Horizontal and vertical lines. Cyan at the top, magenta on the bottom,
    // brown on the left, lightgrey on the right.
    cyg_fb_write_hline(&FRAMEBUF, 25, 12, FRAMEBUF.fb_width - 50, CYAN);
    cyg_fb_write_hline(&FRAMEBUF, 25, FRAMEBUF.fb_height - 12, FRAMEBUF.fb_width - 50, MAGENTA);
    cyg_fb_write_vline(&FRAMEBUF, 12, 25, FRAMEBUF.fb_height - 50, BROWN);
    cyg_fb_write_vline(&FRAMEBUF, FRAMEBUF.fb_width - 12, 25, FRAMEBUF.fb_height - 50, LIGHTGREY);

    // And 14 vertical stripes, from blue to yellow, in the centre of the box.
    block_width     = (FRAMEBUF.fb_width - 100) / 14;
    for (i = 1; i <= 14; i++) {
        cyg_fb_fill_block(&FRAMEBUF, 50 + ((i - 1) * block_width), 50, block_width, FRAMEBUF.fb_height - 100, colours[i]);
    }

    cyg_fb_synch(&FRAMEBUF, CYG_FB_UPDATE_NOW);
#undef FRAMEBUF    
}

// ----------------------------------------------------------------------------
// FB1, 320x240 15bpp, true 555, two pages. Draw a set of horizontal lines
// in one page, vertical lines in the other, and flip between them at
// one second intervals.

static void
fb1_thread(cyg_addrword_t arg)
{
#define FRAMEBUF cyg_synth_fb1
    static cyg_ucount32 colours[16];
    struct cyg_fb_ioctl_page_flip page_flip;
    size_t len;
    int result;
    int i;

    cyg_fb_on(&FRAMEBUF);
    for (i = 0; i < 16; i++) {
        colours[i]  = cyg_fb_make_colour(&FRAMEBUF,
                                         cyg_fb_palette_vga[i + i + i], cyg_fb_palette_vga[i + i + i + 1],cyg_fb_palette_vga[i + i + i + 2]);
    }

    // Draw the horizontal stripes on page 0.
    page_flip.fbpf_visible_page     = 0;
    page_flip.fbpf_drawable_page    = 1;
    page_flip.fbpf_when             = CYG_FB_UPDATE_NOW;
    len                             = sizeof(page_flip);
    result = cyg_fb_ioctl(&FRAMEBUF, CYG_FB_IOCTL_PAGE_FLIPPING_SET_PAGES, &page_flip, &len);
    if (result != 0) {
        fprintf(stderr, "fb1_thread: initial page flip failed.\n");
    }
    // 16 colours, 240 rows -> 15 rows/colour
    for (i = 0; i < 16; i++) {
        cyg_fb_fill_block(&FRAMEBUF, 0, 15 * i, 320, 15, colours[i]);
    }
    
    // Show the horizontal stripes and draw the vertical stripes on page 0.
    page_flip.fbpf_visible_page     = 1;
    page_flip.fbpf_drawable_page    = 0;
    page_flip.fbpf_when             = CYG_FB_UPDATE_NOW;
    len                             = sizeof(page_flip);
    result = cyg_fb_ioctl(&FRAMEBUF, CYG_FB_IOCTL_PAGE_FLIPPING_SET_PAGES, &page_flip, &len);
    if (result != 0) {
        fprintf(stderr, "fb1_thread: second page flip failed.\n");
    }
    // 16 colours, 320 columns -> 20 columns/colour
    for (i = 0; i < 16; i++) {
        cyg_fb_fill_block(&FRAMEBUF, 20 * i, 0, 20, 240, colours[i]);
    }
    
    for ( i = 0; ; i = 1 - i) {
        page_flip.fbpf_visible_page     = i;
        page_flip.fbpf_drawable_page    = 1 - i;
        page_flip.fbpf_when             = CYG_FB_UPDATE_NOW;
        len                             = sizeof(page_flip);
        result = cyg_fb_ioctl(&FRAMEBUF, CYG_FB_IOCTL_PAGE_FLIPPING_SET_PAGES, &page_flip, &len);
        if (result != 0) {
            fprintf(stderr, "fb1_thread: ongoing page flip failed.\n");
        }
        cyg_thread_delay(100);
    }
#undef FRAMEBUF    
}

// ----------------------------------------------------------------------------
// FB2, 320x240 15bpp, true 565, with a 160x120 viewport. Draw a simple image
// as per the fbmacro.c example. Then move around within the viewport in a
// clockwise direction at 110ms intervals.

static void
fb2_thread(cyg_addrword_t arg)
{
#define FRAMEBUF fb2
#define WIDTH    320
#define HEIGHT   240
    
    static cyg_ucount32 colours[16];
    cyg_ucount16 block_width;
    int i, j;
    int x = 0, y = 0;
    CYG_FB_PIXEL0_VAR(FRAMEBUF);
    CYG_FB_PIXEL1_VAR(FRAMEBUF);
    cyg_fb_ioctl_viewport viewport;
    size_t len;

    CYG_FB_ON(FRAMEBUF);
    for (i = 0; i < 16; i++) {
        colours[i]  = CYG_FB_MAKE_COLOUR(FRAMEBUF,
                                         cyg_fb_palette_vga[i + i + i], cyg_fb_palette_vga[i + i + i + 1],cyg_fb_palette_vga[i + i + i + 2]);
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
    
    block_width     = (WIDTH - 100) / 14;
    for (i = 1; i <= 14; i++) {
        CYG_FB_FILL_BLOCK(FRAMEBUF, 50 + ((i - 1) * block_width), 50, block_width, HEIGHT - 100, colours[i]);
    }
    
    
    for ( ; ; ) {
        for (x = 0; x <= 160; x += 5) {
            viewport.fbvp_x     = x;
            viewport.fbvp_y     = 0;
            viewport.fbvp_when  = CYG_FB_UPDATE_NOW;
            len                 = sizeof(viewport);
            CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_VIEWPORT_SET_POSITION, &viewport, &len);
            cyg_thread_delay(11);
        }
        for (y = 0; y < 120; y += 5) {
            viewport.fbvp_x     = 160;
            viewport.fbvp_y     = y;
            viewport.fbvp_when  = CYG_FB_UPDATE_NOW;
            len                 = sizeof(viewport);
            CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_VIEWPORT_SET_POSITION, &viewport, &len);
            cyg_thread_delay(11);
        }
        for (x = 160; x >= 0; x -= 5) {
            viewport.fbvp_x     = x;
            viewport.fbvp_y     = 120;
            viewport.fbvp_when  = CYG_FB_UPDATE_NOW;
            len                 = sizeof(viewport);
            CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_VIEWPORT_SET_POSITION, &viewport, &len);
            cyg_thread_delay(11);
        }
        for (y = 120; y >= 0; y -= 5) {
            viewport.fbvp_x     = 0;
            viewport.fbvp_y     = y;
            viewport.fbvp_when  = CYG_FB_UPDATE_NOW;
            len                 = sizeof(viewport);
            CYG_FB_IOCTL(FRAMEBUF, CYG_FB_IOCTL_VIEWPORT_SET_POSITION, &viewport, &len);
            cyg_thread_delay(11);
        }
    }
#undef FRAMEBUF
#undef WIDTH
#undef HEIGHT
}

// ----------------------------------------------------------------------------
// FB3, 320x240 8bpp, paletted 888. Draw a series of vertical stripes, 5 pixels
// wide, and rotate through the palette at 210ms intervals.

static void
fb3_thread(cyg_addrword_t arg)
{
#define FRAMEBUF    fb3
    static cyg_uint8 palette[(128 +16) * 3];
    int i;

    CYG_FB_ON(FRAMEBUF);

    // Read in the first 128 palette entries, and copy the first 16
    // entries to give a wrap-around. After 128 the palette gets less
    // interesting.
    CYG_FB_READ_PALETTE(FRAMEBUF, 0, 128, palette);
    memcpy(&(palette[128 * 3]), palette, 16 * 3);
    
    for (i = 0; i < 32; i++) {
        CYG_FB_FILL_BLOCK(FRAMEBUF, 10 * i, 0, 10, 240, i);
    }
    
    for ( i = 0; ; i = (i + 1) % 128 ) {
        cyg_thread_delay(21);
        CYG_FB_WRITE_PALETTE(FRAMEBUF, 0, 64, &(palette[i * 3]), CYG_FB_UPDATE_NOW);
    }
#undef FRAMEBUF    
}

// ----------------------------------------------------------------------------
// main(). Start up separate threads for FB1 and FB2, run the FB0 code since
// it just does some drawing and finishes, then run the FB3 code.
static cyg_thread       fb1_thread_data;
static cyg_handle_t     fb1_thread_handle;
static unsigned char    fb1_thread_stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];
static cyg_thread       fb2_thread_data;
static cyg_handle_t     fb2_thread_handle;
static unsigned char    fb2_thread_stack[CYGNUM_HAL_STACK_SIZE_TYPICAL];

int
main(int argc, char** argv)
{
    cyg_thread_create(10, &fb1_thread, 0, "fb1", fb1_thread_stack, CYGNUM_HAL_STACK_SIZE_TYPICAL, &fb1_thread_handle, &fb1_thread_data);
    cyg_thread_create(10, &fb2_thread, 0, "fb2", fb2_thread_stack, CYGNUM_HAL_STACK_SIZE_TYPICAL, &fb2_thread_handle, &fb2_thread_data);
    cyg_thread_resume(fb1_thread_handle);
    cyg_thread_resume(fb2_thread_handle);
    fb0_thread(0);
    fb3_thread(0);
    return 0;
}

