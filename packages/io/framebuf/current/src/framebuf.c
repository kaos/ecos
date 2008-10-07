//==========================================================================
//
//      framebuf.c
//
//      Generic API for accessing framebuffers
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
// Date:          2005-03-29
//
//###DESCRIPTIONEND####
//========================================================================

#define __CYG_FB_IN_FRAMEBUF_C  1
#include <cyg/io/framebuf.h>

#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <errno.h>

// Implementations of the framebuffer functions. Production code
// normally uses extern inline versions of these, defined in
// framebuf.h. However real functions are supplied here in case
// higher-level code chooses to take the address of a function for
// some reason. Also when building for debugging (CYGPKG_INFRA_DEBUG)
// the inline functions are suppressed and the real functions here
// contain lots of useful assertions.

int
cyg_fb_on(cyg_fb* fb)
{
    int result;
    
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_on_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_off_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_pixel_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_read_pixel_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_hline_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_vline_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_fill_block_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_block_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_read_block_fn);
    CYG_CHECK_FUNC_PTRC(fb->fb_move_block_fn);

    result = (*(fb->fb_on_fn))(fb);
    return result;
}

int
cyg_fb_off(cyg_fb* fb)
{
    int result;
    
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_off_fn);
    
    result = (*(fb->fb_off_fn))(fb);
    return result;
}

void
cyg_fb_write_pixel(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_fb_colour colour)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_pixel_fn);

    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((colour <=     1)  || (fb->fb_depth >  1));
    CYG_PRECONDITIONC((colour <=     3)  || (fb->fb_depth >  2));
    CYG_PRECONDITIONC((colour <=    15)  || (fb->fb_depth >  4));
    CYG_PRECONDITIONC((colour <=   255)  || (fb->fb_depth >  8));
    CYG_PRECONDITIONC((colour <= 65535)  || (fb->fb_depth >  16));
    
    (*(fb->fb_write_pixel_fn))(fb, x, y, colour);
}

cyg_fb_colour
cyg_fb_read_pixel(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_read_pixel_fn);
    
    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    
    return (*(fb->fb_read_pixel_fn))(fb, x, y);
}

void
cyg_fb_write_hline(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_hline_fn);

    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((x + len) <= fb->fb_width);
    CYG_PRECONDITIONC((colour <=     1)  || (fb->fb_depth >  1));
    CYG_PRECONDITIONC((colour <=     3)  || (fb->fb_depth >  2));
    CYG_PRECONDITIONC((colour <=    15)  || (fb->fb_depth >  4));
    CYG_PRECONDITIONC((colour <=   255)  || (fb->fb_depth >  8));
    CYG_PRECONDITIONC((colour <= 65535)  || (fb->fb_depth >  16));
    
    (*(fb->fb_write_hline_fn))(fb, x, y, len, colour);
}

void
cyg_fb_write_vline(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 len, cyg_fb_colour colour)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_vline_fn);

    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((y + len) <= fb->fb_height);
    CYG_PRECONDITIONC((colour <=     1)  || (fb->fb_depth >  1));
    CYG_PRECONDITIONC((colour <=     3)  || (fb->fb_depth >  2));
    CYG_PRECONDITIONC((colour <=    15)  || (fb->fb_depth >  4));
    CYG_PRECONDITIONC((colour <=   255)  || (fb->fb_depth >  8));
    CYG_PRECONDITIONC((colour <= 65535)  || (fb->fb_depth >  16));

    (*(fb->fb_write_vline_fn))(fb, x, y, len, colour);
}

void
cyg_fb_fill_block(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_fb_colour colour)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_fill_block_fn);

    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((x + width) <= fb->fb_width);
    CYG_PRECONDITIONC((y + height) <= fb->fb_height);
    CYG_PRECONDITIONC((colour <=     1)  || (fb->fb_depth >  1));
    CYG_PRECONDITIONC((colour <=     3)  || (fb->fb_depth >  2));
    CYG_PRECONDITIONC((colour <=    15)  || (fb->fb_depth >  4));
    CYG_PRECONDITIONC((colour <=   255)  || (fb->fb_depth >  8));
    CYG_PRECONDITIONC((colour <= 65535)  || (fb->fb_depth >  16));

    (*(fb->fb_fill_block_fn))(fb, x, y, width, height, colour);
}

void
cyg_fb_write_block(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                   const void* source, cyg_ucount16 offset, cyg_ucount16 stride)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_block_fn);
    
    CYG_CHECK_DATA_PTRC(source);
    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((x + width) <= fb->fb_width);
    CYG_PRECONDITIONC((y + height) <= fb->fb_height);

    (*(fb->fb_write_block_fn))(fb, x, y, width, height, source, offset, stride);
}

void
cyg_fb_read_block(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height,
                  void* dest, cyg_ucount16 offset, cyg_ucount16 stride)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_read_block_fn);

    CYG_CHECK_DATA_PTRC(dest);
    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((x + width) <= fb->fb_width);
    CYG_PRECONDITIONC((y + height) <= fb->fb_height);
    
    (*(fb->fb_read_block_fn))(fb, x, y, width, height, dest, offset, stride);
}

void
cyg_fb_move_block(cyg_fb* fb, cyg_ucount16 x, cyg_ucount16 y, cyg_ucount16 width, cyg_ucount16 height, cyg_ucount16 new_x, cyg_ucount16 new_y)
{
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_move_block_fn);

    CYG_PRECONDITIONC(x != new_x);
    CYG_PRECONDITIONC(y != new_y);
    CYG_PRECONDITIONC(x < fb->fb_width);
    CYG_PRECONDITIONC(y < fb->fb_height);
    CYG_PRECONDITIONC((x + width) <= fb->fb_width);
    CYG_PRECONDITIONC((y + height) <= fb->fb_height);
    CYG_PRECONDITIONC(new_x < fb->fb_width);
    CYG_PRECONDITIONC(new_y < fb->fb_height);
    CYG_PRECONDITIONC((new_x + width) <= fb->fb_width);
    CYG_PRECONDITIONC((new_y + height) <= fb->fb_height);

    (*(fb->fb_move_block_fn))(fb, x, y, width, height, new_x, new_y);
}

int
cyg_fb_ioctl(cyg_fb* fb, cyg_uint16 key, void* data, size_t* len)
{
    int result;
                 
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_ioctl_fn);

    result = (*(fb->fb_ioctl_fn))(fb, key, data, len);
    return result;
}

void
cyg_fb_synch(cyg_fb* fb, cyg_ucount16 when)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_DOUBLE_BUFFER    
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_move_block_fn);
    (*(fb->fb_synch_fn))(fb, when);
#else
    // Synch is a no-op
#endif    
}

void
cyg_fb_read_palette(cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, void* dest)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_PALETTE
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_read_palette_fn);
    CYG_CHECK_DATA_PTRC(dest);

    CYG_PRECONDITIONC( (first < 16)             || (fb->fb_depth > 4));
    CYG_PRECONDITIONC(((first + count) < 16)    || (fb->fb_depth > 4));
    CYG_PRECONDITIONC( (first < 256)            || (fb->fb_depth > 8));
    CYG_PRECONDITIONC(((first + count) <= 256)  || (fb->fb_depth > 8));
    
    (*(fb->fb_read_palette_fn))(fb, first, count, dest);
#else
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount32, first);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(void*, dest);
#endif
}

void
cyg_fb_write_palette(cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, const void* source, cyg_ucount16 when)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_WRITEABLE_PALETTE
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_write_palette_fn);
    CYG_CHECK_DATA_PTRC(source);
    
    CYG_PRECONDITIONC( (first < 16)             || (fb->fb_depth > 4));
    CYG_PRECONDITIONC(((first + count) < 16)    || (fb->fb_depth > 4));
    CYG_PRECONDITIONC( (first < 256)            || (fb->fb_depth > 8));
    CYG_PRECONDITIONC(((first + count) <= 256)  || (fb->fb_depth > 8));

    (*(fb->fb_write_palette_fn))(fb, first, count, source, when);
#else
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount32, first);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(const void*, source);
    CYG_UNUSED_PARAM(cyg_ucount16, when);
#endif
}

cyg_fb_colour
cyg_fb_make_colour(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_make_colour_fn);
    CYG_PRECONDITIONC((r <= 0x00FF) && (g <= 0x00FF) && (b <= 0x00FF));

    return (*(fb->fb_make_colour_fn))(fb, r, g, b);
#else
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount8, r);
    CYG_UNUSED_PARAM(cyg_ucount8, g);
    CYG_UNUSED_PARAM(cyg_ucount8, b);
    return 0;
#endif
}

void
cyg_fb_break_colour(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_TRUE_COLOUR
    CYG_CHECK_DATA_PTRC(fb);
    CYG_PRECONDITIONC(CYG_FB_MAGIC == fb->fb_magic);
    CYG_CHECK_FUNC_PTRC(fb->fb_break_colour_fn);
    CYG_CHECK_DATA_PTRC(r);
    CYG_CHECK_DATA_PTRC(g);
    CYG_CHECK_DATA_PTRC(b);

    (*(fb->fb_break_colour_fn))(fb, colour, r, g, b);
#else
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_fb_colour, colour);
    CYG_UNUSED_PARAM(cyg_ucount8*, r);
    CYG_UNUSED_PARAM(cyg_ucount8*, g);
    CYG_UNUSED_PARAM(cyg_ucount8*, b);
#endif
}

// ----------------------------------------------------------------------------
// Dummy functions for use by device drivers when instantiating a cyg_fb
// structure

int
cyg_fb_nop_on(cyg_fb* fb)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return 0;
}

int
cyg_fb_nop_off(cyg_fb* fb)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return 0;
}

int
cyg_fb_nop_ioctl(cyg_fb* fb, cyg_uint16 key, void* data, size_t* len)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_uint16, key);
    CYG_UNUSED_PARAM(void*, data);
    CYG_UNUSED_PARAM(size_t*, len);
    return ENOSYS;
}

void
cyg_fb_nop_synch(cyg_fb* fb, cyg_ucount16 when)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount16, when);
}

void
cyg_fb_nop_write_palette(cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, const void* source, cyg_ucount16 when)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount32, first);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(const void*, source);
    CYG_UNUSED_PARAM(cyg_ucount16, when);
}

void
cyg_fb_nop_read_palette(cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, void* dest)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount32, first);
    CYG_UNUSED_PARAM(cyg_ucount32, count);
    CYG_UNUSED_PARAM(void*, dest);
}

cyg_fb_colour
cyg_fb_nop_make_colour(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_ucount8, r);
    CYG_UNUSED_PARAM(cyg_ucount8, g);
    CYG_UNUSED_PARAM(cyg_ucount8, b);
    return 0;
}

cyg_fb_colour
cyg_fb_nop_make_color(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
    __attribute__((alias("cyg_fb_nop_make_colour")));

void
cyg_fb_nop_break_colour(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_UNUSED_PARAM(cyg_fb_colour, colour);
    CYG_UNUSED_PARAM(cyg_ucount8*, r);
    CYG_UNUSED_PARAM(cyg_ucount8*, g);
    CYG_UNUSED_PARAM(cyg_ucount8*, b);
}

void
cyg_fb_nop_break_color(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
    __attribute__((alias("cyg_fb_nop_break_colour")));
    
// ----------------------------------------------------------------------------
// Utility functions for common true colour modes

cyg_fb_colour
cyg_fb_dev_make_colour_8bpp_true_332(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return CYG_FB_MAKE_COLOUR_8BPP_TRUE_332(r, g, b);
}

void
cyg_fb_dev_break_colour_8bpp_true_332(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_FB_BREAK_COLOUR_8BPP_TRUE_332(colour, r, g, b);
}

cyg_fb_colour
cyg_fb_dev_make_colour_16bpp_true_565(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return CYG_FB_MAKE_COLOUR_16BPP_TRUE_565(r, g, b);
}

void
cyg_fb_dev_break_colour_16bpp_true_565(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_FB_BREAK_COLOUR_16BPP_TRUE_565(colour, r, g, b);
}

cyg_fb_colour
cyg_fb_dev_make_colour_16bpp_true_555(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return CYG_FB_MAKE_COLOUR_16BPP_TRUE_555(r, g, b);
}

void
cyg_fb_dev_break_colour_16bpp_true_555(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_FB_BREAK_COLOUR_16BPP_TRUE_555(colour, r, g, b);
}

cyg_fb_colour
cyg_fb_dev_make_colour_32bpp_true_0888(cyg_fb* fb, cyg_ucount8 r, cyg_ucount8 g, cyg_ucount8 b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    return CYG_FB_MAKE_COLOUR_32BPP_TRUE_0888(r, g, b);
}

void
cyg_fb_dev_break_colour_32bpp_true_0888(cyg_fb* fb, cyg_fb_colour colour, cyg_ucount8* r, cyg_ucount8* g, cyg_ucount8* b)
{
    CYG_UNUSED_PARAM(cyg_fb*, fb);
    CYG_FB_BREAK_COLOUR_32BPP_TRUE_0888(colour, r, g, b);
}

