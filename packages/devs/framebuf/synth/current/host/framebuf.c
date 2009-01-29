//============================================================================
//
//     framebuf.c
//
//     A utility program to perform low-level ethernet operations
//
//============================================================================
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
//============================================================================
//#####DESCRIPTIONBEGIN####
//
//  Author(s):   bartv
//  Contact(s):  bartv
//  Date:        2005/10/28
//  Version:     0.01
//  Description:
//      Implementation of a framebuffer device. This script should only ever
//      be run from inside the ecosynth auxiliary.
//
// This program is fork'ed by the framebuf.tcl script running inside
// the synthetic target auxiliary. It is responsible for performing the
// low-level framebuffer accesses.
//
//####DESCRIPTIONEND####
//============================================================================

// We want to instantiate multiple conversion routines for the different
// graphics formats, with extensive use of macros for efficiences. Multiple
// C #include's are used for this.
#ifndef RENDERFN
# include <stdio.h>
# include <stdarg.h>
# include <stdlib.h>
# include <string.h>
# include <time.h>
# include <signal.h>
# include <limits.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
# include <sys/param.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <sys/socket.h>
# include <sys/ioctl.h>
# include <sys/mman.h>
# include <X11/Xlib.h>

// The protocol between host and target is defined by a private
// target-side header.
# include "../src/protocol.h"

// ----------------------------------------------------------------------------
// Globals
// First the variables needed for X operations. The window is created by
// Tcl/Tk code and its id is supplied as an argument.
static Display*         host_display;
static Window           host_win    = 0;
static GC               host_gc;
static XImage*          host_image;

// R, G & B shifts for the host-side. Used for palette conversion.
static int              host_r_shift;
static int              host_g_shift;
static int              host_b_shift;

// The image data. Only 32bpp is supported.
// NOTE: this is not 64-bit clean.
static unsigned int*    img_fb;

// This is used for display depths up to and including 16bpp -
// 256K of static data is irrelevant for a Linux host-side app.
// 32bpp will have to be handled differently.
static unsigned int     host_palette[65536];

// The current function for rendering target-side data
static void             (*render_fn)(int, int, int, int);

// And palette init and update functions.
static void             (*palette_init_fn)(void);
static void             (*palette_update_fn)(void);

// Positions. With target-side viewports, magnification, and the
// possibility of windows being resized, this can
// get very messy.
//
// The target-side framebuffer consists of n pages, each of
// width*height pixels, plus possibly padding at the end of each
// scanline.
//
// img_fb consists of viewport_width*viewport_height pixels.
// At magnification 1 each pixel is a 32-bit unsigned int, but
// at higher magnification each pixel takes up mag^2 unsigned
// ints.
//
// win_width and win_height determine the current window dimensions in
// target pixel units, i.e. ignoring magnification. x_win_width
// and x_win_height are the same dimensions in X pixels, so
// mag*win_width. These dimensions are provided by X configure events.
// They may be smaller, the same size, or larger than the viewport.
//
// A coordinate (target_x,target_y) within the current page
// corresponds to (target_x - viewport_x, target_y - viewport_y).
// within host_image and within the window.
//
// For an X expose event we get X windows coordinates.

// The actually visible window dimensions. If the Tk window gets
// resized then only part of the image data may be visible. These
// variables change in response to X configure events.
static int              win_width;
static int              win_height;
static int              x_win_width;
static int              x_win_height;

// Target-side framebuffer.
//
// The synth_fb_data structure at the start of the shared memory region.
static synth_fb_data*   shared_fb_data;

// The target-side framebuffer starts at shared_fb_data->framebuf[0].
// However that is not necessarily where the visible data starts
// because of page flipping and viewport support. This always points
// at the top-left corner of visible data.
static void*            target_fb;

// Parameters supplied by the target.
static int              target_id;
static int              target_depth;
static int              target_le;
static int              target_width;
static int              target_height;
static int              target_viewport_width;
static int              target_viewport_height;
static int              target_stride;
static int              target_number_pages;
static char*            target_format;

// Parameters supplied by a host-side configuration file
static int              config_magnification;

// Communication between host and target happens partially through the
// shared memory region and partially through a fifo, the latter
// allowing for select(). shared_fb_data already points at the shared
// memory region.
static char             shm_name[L_tmpnam];
static int              shm_created;
static int              shm_fd;
static char             fifo_t2h_name[L_tmpnam];
static int              fifo_t2h_created;
static int              fifo_t2h_fd;
static char             fifo_h2t_name[L_tmpnam];
static int              fifo_h2t_created;
static int              fifo_h2t_fd;

// An atexit() handler for cleaning up the fifos and shared memory.
static void
atexit_handler(void)
{
    if (shm_created) {
        unlink(shm_name);
    }
    if (fifo_t2h_created) {
        unlink(fifo_t2h_name);
    }
    if (fifo_h2t_created) {
        unlink(fifo_h2t_name);
    }
}

// ----------------------------------------------------------------------------
// Diagnostics. Warnings and errors are sent up to the I/O auxiliary for
// display there. Debug output goes straight to stderr

// Set the DEBUG_LEVEL to 0 for no debugging, 3 for maximum debugging.
#define DEBUG_LEVEL 0
#define DEBUG(_level_, _str_, ...)                                  \
    if (_level_ <= DEBUG_LEVEL) {                                   \
        fprintf(stderr, "%d: " _str_, target_id, ## __VA_ARGS__);   \
    }

#if 0
static void
warn(char* fmt, ...)
{
    char        buf[512];
    va_list     args;
    va_start(args, fmt);

    sprintf(buf, "Warning (fb%d) : ", target_id);
    vsnprintf(buf + strlen(buf), 512 - strlen(buf), fmt, args);
    buf[511] = '\0';

    (void) write(1, buf, strlen(buf));
}
#endif

static void
error(char* fmt, ...)
{
    char        buf[512];
    va_list     args;
    va_start(args, fmt);

    sprintf(buf, "Error (fb%d) : ", target_id);
    vsnprintf(buf + strlen(buf), 512 - strlen(buf), fmt, args);
    buf[511] = '\0';

    (void) write(1, buf, strlen(buf));
    exit(1);
}

// ----------------------------------------------------------------------------
// Host-side.
//
// The main redraw routines. schedule_redraw() can get called as a result of:
//
// 1) an X expose event.
// 2) a message from the Tk code affecting the on/off setting of
//    the framebuffer.
// 3) a message from the target-side code indicating some of the
//    target-side data has changed, or some other event such as
//    viewport repositioning or page flipping.
//
// It just maintains a bounding box. do_redraw() does the real work and
// is called from inside the main loop.

static int  redraw_pending = 0;
static int  x_redraw_x, x_redraw_y, x_redraw_width, x_redraw_height;

static void
do_redraw(void)
{
    while (redraw_pending) {
        redraw_pending = 0;
        DEBUG(3, "do_redraw: win_x %d, win_y %d, width %d, height %d\n", x_redraw_x, x_redraw_y, x_redraw_width, x_redraw_height);
        XPutImage(host_display, host_win, host_gc, host_image, x_redraw_x, x_redraw_y, x_redraw_x, x_redraw_y, x_redraw_width, x_redraw_height);
        XFlush(host_display);
    }
}

static void
schedule_redraw(int x_new_x, int x_new_y, int x_new_width, int x_new_height)
{
    if (! redraw_pending) {
        redraw_pending  = 1;
        x_redraw_x        = x_new_x;
        x_redraw_y        = x_new_y;
        x_redraw_width    = x_new_width;
        x_redraw_height   = x_new_height;
    } else {
        if (x_redraw_x > x_new_x) {
            x_redraw_x = x_new_x;
        } else {
            x_new_width += (x_new_x - x_redraw_x);
        }
        if (x_redraw_y > x_new_y) {
            x_redraw_y = x_new_y;
        } else {
            x_new_height += (x_new_y - x_redraw_y);
        }
        if (x_new_width > x_redraw_width) {
            x_redraw_width = x_new_width;
        }
        if (x_new_height > x_redraw_height) {
            x_redraw_height = x_new_height;
        }
    }
}

// ----------------------------------------------------------------------------
// Utility for blanking the screen
static void
blackout(void)
{
    memset(img_fb, 0, target_viewport_width * target_viewport_height * sizeof(int) * config_magnification * config_magnification);
    if (0 != host_win) {
        schedule_redraw(0, 0, x_win_width, x_win_height);
    }
}

// ----------------------------------------------------------------------------
// This function gets called when the main select() indicates the
// X server is trying to send some data.
static void
handle_X_event(void)
{
    XEvent      event;

    XNextEvent(host_display, &event);
    switch(event.type) {
      case Expose:
        {
            // In theory this code should be able to do partial redraws for every
            // expose event, but that does not seem to work reliably. Instead do a
            // full redraw, but only for the final event in a sequence.
            if (0 == event.xexpose.count) {
                DEBUG(3, "X expose event, x %d, y %d, width %d, height %d\n",
                      event.xexpose.x, event.xexpose.y, event.xexpose.width, event.xexpose.height);
                schedule_redraw(0, 0, x_win_width, x_win_height);
            }
            break;
        }

      case ConfigureNotify:
        {
            DEBUG(2, "X configure notify event, width %d, height %d\n", event.xconfigure.width, event.xconfigure.height);
            x_win_width     = event.xconfigure.width;
            x_win_height    = event.xconfigure.height;
            win_width       = (x_win_width  + config_magnification - 1) / config_magnification;
            win_height      = (x_win_height + config_magnification - 1) / config_magnification;

            if (shared_fb_data->display_on && shared_fb_data->blank_on) {
                (*render_fn)(0 + shared_fb_data->viewport_x, 0 + shared_fb_data->viewport_y, win_width, win_height);
            } else {
                blackout();
            }
            break;
        }
    }
}

// ----------------------------------------------------------------------------
// Conversion routines

// This dummy function is installed to handle draw requests before the
// window is mapped - until that time the exact render function is unknown.
static void
dummy_render_fn(int target_x, int target_y, int width, int height)
{
    DEBUG(2, "rendering: target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    DEBUG(2, "         : window has not been mapped on to the display yet\n");
}
#else  // RENDERFN

// These functions get instantiated N times for the N different
// graphics formats. We need to take the target-side region defined by
// x/y/width/height, render it into the image data, and then redraw
// the relevant part of the image data. There is no point rendering more
// data than is visible.

static void
RENDERFN(1, TARGET_FORMAT)(int target_x, int target_y, int width, int height)
{
    int             x, y;
    int             win_x, win_y;
    unsigned int*   img_data;
    unsigned int    colour;
    TARGET_DATA;

    DEBUG(2, "rendering: target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((0 == host_win) || !shared_fb_data->display_on || !shared_fb_data->blank_on) {
        DEBUG(2, "         : no-op, host_win %d, display_on %d, blank_on %d\n", (int)host_win, shared_fb_data->display_on, shared_fb_data->blank_on);
        return;
    }
    DEBUG(2, "         : current viewport x %d, y %d\n", shared_fb_data->viewport_x, shared_fb_data->viewport_y);

    win_x   = target_x - shared_fb_data->viewport_x;
    win_y   = target_y - shared_fb_data->viewport_y;
    if (win_x < 0) {
        width       += win_x;
        win_x        = 0;
    }
    if (win_y < 0) {
        height      += win_y;
        win_y        = 0;
    }
    if ((win_x + width) > win_width) {
        width = win_width - win_x;
    }
    if ((win_y + height) > win_height) {
        height = win_height - win_y;
    }
    DEBUG(2, "         : after clipping, win_x %d, win_y %d, width %d, height %d\n", win_x, win_y, width, height);
    if ((width < 0) || (height < 0)) {
        return;
    }

    img_data    = img_fb    + (win_y * target_viewport_width) + win_x;
    for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
            colour  = TARGET_NEXT_COLOUR();
            *img_data++  = colour;
        }
        img_data += (target_viewport_width - width);
        TARGET_NEXT_LINE();
    }
    schedule_redraw(win_x, win_y, width, height);
}

static void
RENDERFN(2, TARGET_FORMAT)(int target_x, int target_y, int width, int height)
{
    int             x, y;
    int             win_x, win_y;
    unsigned int*   img_data0;
    unsigned int*   img_data1;
    unsigned int    colour;
    TARGET_DATA;

    DEBUG(2, "rendering: target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((0 == host_win) || !shared_fb_data->display_on || !shared_fb_data->blank_on) {
        DEBUG(2, "         : no-op, host_win %d, display_on %d, blank_on %d\n", (int)host_win, shared_fb_data->display_on, shared_fb_data->blank_on);
        return;
    }
    DEBUG(2, "         : current viewport x %d, y %d\n", shared_fb_data->viewport_x, shared_fb_data->viewport_y);
    win_x   = target_x - shared_fb_data->viewport_x;
    win_y   = target_y - shared_fb_data->viewport_y;
    if (win_x < 0) {
        width       += win_x;
        win_x        = 0;
    }
    if (win_y < 0) {
        height      += win_y;
        win_y        = 0;
    }
    if ((win_x + width) > win_width) {
        width = win_width - win_x;
    }
    if ((win_y + height) > win_height) {
        height = win_height - win_y;
    }
    DEBUG(2, "         : after clipping, target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((width < 0) || (height < 0)) {
        return;
    }
    

    for (y = 0; y < height; y++) {
        img_data0       = img_fb + (2 * (win_y + y) * (2 * target_viewport_width)) + (2 * win_x);
        img_data1       = img_data0 + (2 * target_viewport_width);
        for (x = 0; x < width; x++) {
            colour  = TARGET_NEXT_COLOUR();
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
        }
        TARGET_NEXT_LINE();
    }
    schedule_redraw(2 * win_x, 2 * win_y, 2 * width, 2 * height);
}

static void
RENDERFN(3, TARGET_FORMAT)(int target_x, int target_y, int width, int height)
{
    int             x, y;
    int             win_x, win_y;
    unsigned int*   img_data0;
    unsigned int*   img_data1;
    unsigned int*   img_data2;
    unsigned int    colour;
    TARGET_DATA;

    DEBUG(2, "rendering: target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((0 == host_win) || !shared_fb_data->display_on || !shared_fb_data->blank_on) {
        DEBUG(2, "         : no-op, host_win %d, display_on %d, blank_on %d\n", (int)host_win, shared_fb_data->display_on, shared_fb_data->blank_on);
        return;
    }
    DEBUG(2, "         : current viewport x %d, y %d\n", shared_fb_data->viewport_x, shared_fb_data->viewport_y);
    win_x   = target_x - shared_fb_data->viewport_x;
    win_y   = target_y - shared_fb_data->viewport_y;
    if (win_x < 0) {
        width       += win_x;
        win_x        = 0;
    }
    if (win_y < 0) {
        height      += win_y;
        win_y        = 0;
    }
    if ((win_x + width) > win_width) {
        width = win_width - win_x;
    }
    if ((win_y + height) > win_height) {
        height = win_height - win_y;
    }
    DEBUG(2, "         : after clipping, target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((width < 0) || (height < 0)) {
        return;
    }

    for (y = 0; y < height; y++) {
        img_data0       = img_fb + (3 * (win_y + y) * (3 * target_viewport_width)) + (3 * win_x);
        img_data1       = img_data0 + (3 * target_viewport_width);
        img_data2       = img_data1 + (3 * target_viewport_width);

        for (x = 0; x < width; x++) {
            colour  = TARGET_NEXT_COLOUR();
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
            *img_data2++  = colour;
            *img_data2++  = colour;
            *img_data2++  = colour;
        }
        TARGET_NEXT_LINE();
    }
    schedule_redraw(3 * win_x, 3 * win_y, 3 * width, 3 * height);
}

static void
RENDERFN(4, TARGET_FORMAT)(int target_x, int target_y, int width, int height)
{
    int             x, y;
    int             win_x, win_y;
    unsigned int*   img_data0;
    unsigned int*   img_data1;
    unsigned int*   img_data2;
    unsigned int*   img_data3;
    unsigned int    colour;
    TARGET_DATA;

    DEBUG(2, "rendering: target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((0 == host_win) || !shared_fb_data->display_on || !shared_fb_data->blank_on) {
        DEBUG(2, "         : no-op, host_win %d, display_on %d, blank_on %d\n", (int)host_win, shared_fb_data->display_on, shared_fb_data->blank_on);
        return;
    }
    DEBUG(2, "         : current viewport x %d, y %d\n", shared_fb_data->viewport_x, shared_fb_data->viewport_y);
    win_x   = target_x - shared_fb_data->viewport_x;
    win_y   = target_y - shared_fb_data->viewport_y;
    if (win_x < 0) {
        width       += win_x;
        win_x        = 0;
    }
    if (win_y < 0) {
        height      += win_y;
        win_y        = 0;
    }
    if ((win_x + width) > win_width) {
        width = win_width - win_x;
    }
    if ((win_y + height) > win_height) {
        height = win_height - win_y;
    }
    DEBUG(2, "         : after clipping, target_x %d, target_y %d, width %d, height %d\n", target_x, target_y, width, height);
    if ((width < 0) || (height < 0)) {
        return;
    }

    for (y = 0; y < height; y++) {
        img_data0       = img_fb + (4 * (win_y + y) * (4 * target_viewport_width)) + (4 * win_x);
        img_data1       = img_data0 + (4 * target_viewport_width);
        img_data2       = img_data1 + (4 * target_viewport_width);
        img_data3       = img_data2 + (4 * target_viewport_width);

        for (x = 0; x < width; x++) {
            colour  = TARGET_NEXT_COLOUR();
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data0++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
            *img_data1++  = colour;
            *img_data2++  = colour;
            *img_data2++  = colour;
            *img_data2++  = colour;
            *img_data2++  = colour;
            *img_data3++  = colour;
            *img_data3++  = colour;
            *img_data3++  = colour;
            *img_data3++  = colour;
        }
        TARGET_NEXT_LINE();
    }
    schedule_redraw(4 * win_x, 4 * win_y, 4 * width, 4 * height);
}

#endif
#ifndef RENDERFN

# define RENDERFN_AUX(_magnification_, _format_) render_ ## _magnification_ ## _ ## _format_
# define RENDERFN(_magnification_, _format_)     RENDERFN_AUX(_magnification_, _format_)

// ----------------------------------------------------------------------------
// 1bpp, BE. It is convenient to always render in byte-multiples
# define TARGET_FORMAT   1BPP_BE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x07;                                 \
    target_x    &= ~0x07;                                           \
    width        = (width + 7) & ~0x07;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 3);               \
    mask         = 0x0080

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask >>= 1;                                                 \
        if (0 == mask) {                                            \
            target_fb_current += 1;                                 \
            mask               = 0x0080;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 3)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 1bpp, LE
# define TARGET_FORMAT   1BPP_LE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x07;                                 \
    target_x    &= ~0x07;                                           \
    width        = (width + 7) & ~0x07;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 3);               \
    mask         = 0x0001

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask <<= 1;                                                 \
        if (0x0100 == mask) {                                       \
            target_fb_current += 1;                                 \
            mask               = 0x0001;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 3)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 2bpp, BE
# define TARGET_FORMAT   2BPP_BE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x03;                                 \
    target_x    &= ~0x03;                                           \
    width        = (width + 3) & ~0x03;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 2);               \
    mask         = 0x00C0

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask >>= 2;                                                 \
        if (0x00 == mask) {                                         \
            target_fb_current += 1;                                 \
            mask               = 0x00C0;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 2)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 2bpp, LE
# define TARGET_FORMAT   2BPP_LE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x03;                                 \
    target_x    &= ~0x03;                                           \
    width        = (width + 3) & ~0x03;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 2);               \
    mask         = 0x0003

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask <<= 2;                                                 \
        if (0x0300 == mask) {                                       \
            target_fb_current += 1;                                 \
            mask               = 0x0003;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 2)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 4bpp, BE
# define TARGET_FORMAT   4BPP_BE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x01;                                 \
    target_x    &= ~0x01;                                           \
    width        = (width + 1) & ~0x01;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 1);               \
    mask         = 0x00F0

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask >>= 4;                                                 \
        if (0 == mask) {                                            \
            target_fb_current += 1;                                 \
            mask               = 0x00F0;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 1)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 4bpp, LE
# define TARGET_FORMAT   4BPP_LE

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    unsigned int    mask;                                           \
    width       += target_x & 0x01;                                 \
    target_x    &= ~0x01;                                           \
    width        = (width + 1) & ~0x01;                             \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + (target_x >> 1);               \
    mask         = 0x000F

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int next_colour;                                   \
        next_colour = host_palette[*target_fb_current & mask];      \
        mask <<= 4;                                                 \
        if (0x0F00 == mask) {                                       \
            target_fb_current += 1;                                 \
            mask               = 0x000F;                            \
        }                                                           \
        next_colour;                                                \
    })

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += target_stride - (width >> 2)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 8bpp, paletted or true colour. In the case of a true colour display the
// host_palette array will have been filled in appropriately for 332 so
// conversion to 32-bit true colour again just involves indirecting through
// the palette, rather than the more expensive bit masking and shifting.
# define TARGET_FORMAT   8BPP

# define TARGET_DATA                                                \
    unsigned char*  target_fb_current;                              \
    target_fb_current   = ((unsigned char*)target_fb) +             \
        (target_y * target_stride) + target_x

# define TARGET_NEXT_COLOUR() host_palette[*target_fb_current++]

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += (target_stride - width)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 16bpp. Again this is handled via the host_palette array, suitably
// initialized for either 555 or 565
# define TARGET_FORMAT   16BPP

# define TARGET_DATA                                                \
    unsigned short*  target_fb_current;                             \
    target_fb_current   = ((unsigned short*)target_fb) +            \
        (target_y * (target_stride >> 1)) + target_x

# define TARGET_NEXT_COLOUR()   host_palette[*target_fb_current++]

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += ((target_stride >> 1) - width)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 32bpp, 0888, no swapping

# define TARGET_FORMAT   32BPP

# define TARGET_DATA                                                \
    unsigned int*  target_fb_current;                               \
    target_fb_current   = ((unsigned int*)target_fb) +              \
        (target_y * (target_stride >> 2)) + target_x

# define TARGET_NEXT_COLOUR() *target_fb_current++ & 0x00FFFFFF

# define TARGET_NEXT_LINE()                                         \
    target_fb_current += ((target_stride >> 2) - width)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// 32bpp, 0888, but the host uses 8880.
# define TARGET_FORMAT   32BPP_SWAPPED

# define TARGET_DATA                                                \
    unsigned int*  target_fb_current;                               \
    target_fb_current   = ((unsigned int*)target_fb) +              \
        (target_y * (target_stride >> 2)) + target_x

# define TARGET_NEXT_COLOUR()                                       \
    ({                                                              \
        unsigned int _colour_ = *target_fb_current++;               \
        _colour_ =                                                  \
            ((_colour_ & 0x000000FF) << 24) |                       \
            ((_colour_ & 0x0000FF00) << 16) |                       \
            ((_colour_ & 0x00FF0000) <<  8);                        \
        _colour_;                                                   \
    })
        
# define TARGET_NEXT_LINE()                                         \
    target_fb_current += ((target_stride >> 2) - width)

# include "framebuf.c"
# undef TARGET_FORMAT
# undef TARGET_DATA
# undef TARGET_NEXT_COLOUR
# undef TARGET_NEXT_LINE

// ----------------------------------------------------------------------------
// Palette management. 8bpp and 16bpp true colour can be initialized
// statically. 32bpp does not involve the host_palette array at all.
// Other formats involve dynamic palette updating.

static void
palette_initialize_8bpp_332(void)
{
    static const unsigned char expand2[4]  = { 0x00, 0x55, 0xAA, 0xFF };
    static const unsigned char expand3[8]  = { 0x00, 0x20 + 4, 0x40 + 9, 0x60 + 13, 0x80 + 18, 0xA0 + 22, 0xC0 + 27, 0xE0 + 31 };
    int i;
    for (i = 0; i < 256; i++) {
        int r   = (i & 0x00E0) >> 5;
        int g   = (i & 0x001C) >> 2;
        int b   = (i & 0x0003) >> 0;
        host_palette[i] = (expand3[r] << host_r_shift) | (expand3[g] << host_g_shift) | (expand2[b] << host_b_shift);
    }
}

static void
palette_initialize_16bpp_555(void)
{
    static const unsigned char expand5[32] = {
        0x00 + 0, 0x08 + 0, 0x10 + 0, 0x18 + 0, 0x20 + 1, 0x28 + 1, 0x30 + 1, 0x38 + 1,
        0x40 + 2, 0x48 + 2, 0x50 + 2, 0x58 + 2, 0x60 + 3, 0x68 + 3, 0x70 + 3, 0x78 + 3,
        0x80 + 4, 0x88 + 4, 0x90 + 4, 0x98 + 4, 0xA0 + 5, 0xA8 + 5, 0xB0 + 5, 0xB8 + 5,
        0xC0 + 6, 0xC8 + 6, 0xD0 + 6, 0xD8 + 6, 0xE0 + 7, 0xE8 + 7, 0xF0 + 7, 0xF8 + 7,
    };
    int i;
    for (i = 0; i < 65536; i++) {
        int r = (i & 0x00007C00) >> 10;
        int g = (i & 0x000003E0) >> 5;
        int b = (i & 0x0000001F) >> 0;
        host_palette[i] = (expand5[r] << host_r_shift) | (expand5[g] << host_g_shift) | (expand5[b] << host_b_shift);
    }
}

static void
palette_initialize_16bpp_565(void)
{
    static const unsigned char expand5[32] = {
        0x00 + 0, 0x08 + 0, 0x10 + 0, 0x18 + 0, 0x20 + 1, 0x28 + 1, 0x30 + 1, 0x38 + 1,
        0x40 + 2, 0x48 + 2, 0x50 + 2, 0x58 + 2, 0x60 + 3, 0x68 + 3, 0x70 + 3, 0x78 + 3,
        0x80 + 4, 0x88 + 4, 0x90 + 4, 0x98 + 4, 0xA0 + 5, 0xA8 + 5, 0xB0 + 5, 0xB8 + 5,
        0xC0 + 6, 0xC8 + 6, 0xD0 + 6, 0xD8 + 6, 0xE0 + 7, 0xE8 + 7, 0xF0 + 7, 0xF8 + 7,
    };
    static const unsigned char expand6[64] = {
        0x00 + 0, 0x04 + 0, 0x08 + 0, 0x0C + 0, 0x10 + 0, 0x14 + 0, 0x18 + 0, 0x1C + 0,
        0x20 + 0, 0x24 + 0, 0x28 + 0, 0x2C + 0, 0x30 + 0, 0x34 + 0, 0x38 + 0, 0x3C + 0,
        0x40 + 1, 0x44 + 1, 0x48 + 1, 0x4C + 1, 0x50 + 1, 0x54 + 1, 0x58 + 1, 0x5C + 1,
        0x60 + 1, 0x64 + 1, 0x68 + 1, 0x6C + 1, 0x70 + 1, 0x74 + 1, 0x78 + 1, 0x7C + 1,
        0x80 + 2, 0x84 + 2, 0x88 + 2, 0x8C + 2, 0x90 + 2, 0x94 + 2, 0x98 + 2, 0x9C + 2,
        0xA0 + 2, 0xA4 + 2, 0xA8 + 2, 0xAC + 2, 0xB0 + 2, 0xB4 + 2, 0xB8 + 2, 0xBC + 2,
        0xC0 + 3, 0xC4 + 3, 0xC8 + 3, 0xCC + 3, 0xD0 + 3, 0xD4 + 3, 0xD8 + 3, 0xDC + 3,
        0xE0 + 3, 0xE4 + 3, 0xE8 + 3, 0xEC + 3, 0xF0 + 3, 0xF4 + 3, 0xF8 + 3, 0xFC + 3,
    };
    int i;
    for (i = 0; i < 65536; i++) {
        int r = (i & 0x0000F800) >> 11;
        int g = (i & 0x000007E0) >> 5;
        int b = (i & 0x0000001F) >> 0;
        host_palette[i] = (expand5[r] << host_r_shift) | (expand6[g] << host_g_shift) | (expand5[b] << host_b_shift);
    }
}

static void
palette_update(void)
{
    int             i, j;
    unsigned char*  target_palette;
    int             r, g, b;

    target_palette  = shared_fb_data->palette;
    for (i = 0; i < (0x01 << target_depth); i++) {
        r = *target_palette++;
        g = *target_palette++;
        b = *target_palette++;
        host_palette[i] = (r << host_r_shift) | (g << host_g_shift) | (b << host_b_shift);
    }

    // Make sure the palette is replicated throughout the first 256
    // entries. That way when rendering <8bpp data we can just mask
    // the target-side bytes without having to shift.
    for (j = 0; i < 256; i++, j++) {
        host_palette[i] = host_palette[j];
    }
}

// ----------------------------------------------------------------------------
// This array is used to map the various display formats etc. on to
// render functions.
static struct _render_details {
    char*           rd_format;
    unsigned int    rd_endianness_matters;
    unsigned int    rd_le;
    void            (*rd_palette_init)(void);
    void            (*rd_palette_update)(void);
    void            (*rd_render_fns[4])(int, int, int, int);
} render_array[] = {
    // 32BPP must come first
    { "32BPP_TRUE_0888", 0, 0, NULL, NULL,
      { &render_1_32BPP, &render_2_32BPP, &render_3_32BPP, &render_4_32BPP }
    },
    { "16BPP_TRUE_555",  0, 0, &palette_initialize_16bpp_555, NULL,
      { &render_1_16BPP, &render_2_16BPP, &render_3_16BPP, &render_4_16BPP }
    },
    { "16BPP_TRUE_565",  0, 0, &palette_initialize_16bpp_565, NULL,
      { &render_1_16BPP, &render_2_16BPP, &render_3_16BPP, &render_4_16BPP }
    },
    { "8BPP_TRUE_332",   0, 0, &palette_initialize_8bpp_332, NULL,
      { &render_1_8BPP, &render_2_8BPP, &render_3_8BPP, &render_4_8BPP }
    },
    { "8BPP_PAL888",     0, 0, NULL, &palette_update,
      { &render_1_8BPP, &render_2_8BPP, &render_3_8BPP, &render_4_8BPP }
    },
    { "4BPP_PAL888",     1, 1, NULL, &palette_update,
      { &render_1_4BPP_LE, &render_2_4BPP_LE, &render_3_4BPP_LE, &render_4_4BPP_LE }
    },
    { "4BPP_PAL888",     1, 0, NULL, &palette_update,
      { &render_1_4BPP_BE, &render_2_4BPP_BE, &render_3_4BPP_BE, &render_4_4BPP_BE }
    },
    { "2BPP_PAL888",     1, 1, NULL, &palette_update,
      { &render_1_2BPP_LE, &render_2_2BPP_LE, &render_3_2BPP_LE, &render_4_2BPP_LE }
    },
    { "2BPP_PAL888",     1, 0, NULL, &palette_update,
      { &render_1_2BPP_BE, &render_2_2BPP_BE, &render_3_2BPP_BE, &render_4_2BPP_BE }
    },
    { "1BPP_PAL888",     1, 1, NULL, &palette_update,
      { &render_1_1BPP_LE, &render_2_1BPP_LE, &render_3_1BPP_LE, &render_4_1BPP_LE }
    },
    { "1BPP_PAL888",     1, 0, NULL, &palette_update,
      { &render_1_1BPP_BE, &render_2_1BPP_BE, &render_3_1BPP_BE, &render_4_1BPP_BE }
    },
    { NULL }
};


// ----------------------------------------------------------------------------
// Communication between host and target
//
// When the target wants to wake up the host it sends a single byte down
// a fifo, typically after having filled in appropriate fields in the
// shared memory region.
static void
handle_target_request(void)
{
    unsigned char   buf[1];
    int             result;

    result  = read(fifo_t2h_fd, buf, 1);
    if (-1 == result) {
        if (EINTR == errno) {
            return;
        }
        error("unexpected error %d (%s) reading fifo command from target-side code", errno, strerror(errno));
    }
    if (0 == result) {
        // The target-side must have exited. Do not follow suit yet. Instead
        // exit only when the I/O auxiliary exits.
        DEBUG(1, "eCos application has exited\n");
        close(fifo_t2h_fd);
        fifo_t2h_fd = -1;
        return;
    }

    switch(buf[0]) {
      case SYNTH_FB_OK:
        {
            // The target has finished initializing. This may have
            // involved filling in the palette
            DEBUG(1, "target request SYNTH_FB_OK, eCos application has connected.\n");
            if (palette_update_fn) {
                (*palette_update_fn)();
            }
            break;
        }
        
      case SYNTH_FB_SYNC:
        {
            // The target has updated part of the display.
            DEBUG(2, "target request SYNC, x0 %d, y0 %d, x1 %d, y1 %d\n",
                  shared_fb_data->sync_x0, shared_fb_data->sync_y0, shared_fb_data->sync_x1, shared_fb_data->sync_y1);
            (*render_fn)(shared_fb_data->sync_x0, shared_fb_data->sync_y0,
                         (shared_fb_data->sync_x1 - shared_fb_data->sync_x0),
                         (shared_fb_data->sync_y1 - shared_fb_data->sync_y0));
            break;
        }
        
      case SYNTH_FB_WRITE_PALETTE:
        {
            // The target-side palette has been updated. Adjust the
            // host-side palette and then render the whole window with
            // the curent colours (unless blanked).
            DEBUG(1, "target request WRITE_PALETTE\n");
            if (palette_update_fn) {
                (*palette_update_fn)();
            }
            (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
        }

      case SYNTH_FB_BLANK:
        {
            DEBUG(1, "target request blank, display should be %s\n", shared_fb_data->blank_on ? "on" : "off");
            if (shared_fb_data->blank_on) {
                (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
            } else {
                blackout();
            }
            break;
        }
        
      case SYNTH_FB_VIEWPORT:
        {
            // Just rerender the whole display as per the new viewport position.
            DEBUG(1, "target request move viewport to x %d, y %d\n", shared_fb_data->viewport_x, shared_fb_data->viewport_y);
            (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
            break;
        }

      case SYNTH_FB_PAGE_FLIP:
        {
            int page_size   = target_height * target_stride;
            DEBUG(1, "target request page flip to page %d\n", shared_fb_data->page_visible);
            target_fb       = (void*)(((char*)&(shared_fb_data->framebuf[0])) + (page_size * shared_fb_data->page_visible));
            (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
            break;
        }
    }

    // Send a single-byte response back to the target
    DEBUG(2, "target request handled\n");
    buf[0] = SYNTH_FB_OK;
    do {
        result = write(fifo_h2t_fd, buf, 1);
    } while ((result < 0) && (errno == EINTR));
    if (result < 0) {
        error("unexpected error %d (%s) writing fifo status to target-side code", errno, strerror(errno));
    }
}

// ----------------------------------------------------------------------------
// Communication from the Tcl/Tk script
static void
handle_mapped(int winid)
{
    Status              result;
    XWindowAttributes   attr;
    XGCValues           gc_values;
    Visual*             visual;
    int                 i;

    DEBUG(1, "X window %d has been mapped\n", winid);
    if (0 != (int)host_win) {
        error("  window has already been mapped.\n");
    }
    
    host_win    = (Window)winid;
    result      = XGetWindowAttributes(host_display, host_win, &attr);
    if (0 == result) {
        error("failed to get window attributes.");
    }
    gc_values.graphics_exposures    = False;
    host_gc = XCreateGC(host_display, host_win, GCGraphicsExposures, &gc_values);
    result = XSelectInput(host_display, host_win, ExposureMask | StructureNotifyMask);

    // The Tcl script has already checked that we are on a 24/32 bit display.
    // We need to know the colour shifts and possibly adjust the render array.
    visual  = attr.visual;
    DEBUG(1, "  red_mask 0x%08lx, green_mask 0x%08lx, blue_mask 0x%08lx\n", visual->red_mask, visual->green_mask, visual->blue_mask);
    if ((0x00FF0000 == visual->red_mask) && (0x0000FF00 == visual->green_mask) && (0x000000FF == visual->blue_mask)) {
        // 0888, nothing special needed
        host_r_shift    = 16;
        host_g_shift    =  8;
        host_b_shift    =  0;
    } else if ((0x0000FF00 == visual->red_mask) && (0x00FF0000 == visual->green_mask) && (0xFF000000 == visual->blue_mask)) {
        // 8880
        host_r_shift    =  8;
        host_g_shift    = 16;
        host_b_shift    = 24;
        render_array[0].rd_render_fns[0]    = &render_1_32BPP_SWAPPED;
        render_array[0].rd_render_fns[1]    = &render_2_32BPP_SWAPPED;
        render_array[0].rd_render_fns[2]    = &render_3_32BPP_SWAPPED;
        render_array[0].rd_render_fns[3]    = &render_4_32BPP_SWAPPED;
    }
    
    // Time to figure out which render function etc. to use.
    render_fn = (void (*)(int, int, int, int)) NULL;
    for (i = 0; render_array[i].rd_format; i++) {
        if ((0 == strcmp(render_array[i].rd_format, target_format)) &&
            (!render_array[i].rd_endianness_matters || (render_array[i].rd_le == target_le))) {

            render_fn           = render_array[i].rd_render_fns[config_magnification - 1];
            palette_init_fn     = render_array[i].rd_palette_init;
            palette_update_fn   = render_array[i].rd_palette_update;
            break;
        }
    }
    if (NULL == render_fn) {
        error("Target format not supported.");
    }

    // Now it is possible to create the XImage structure, the fifos,
    // and the shared memory region for interaction with the target.
    // The XImage needs to be large enough for the viewport. That may
    // be larger than the actually visible window, but the window may
    // get resized.
    host_image  = XCreateImage(host_display, visual,
                               24,          // image depth. 24 bits of colour info.
                               ZPixmap,
                               0,           // offset
                               NULL,        // data, filled in later
                               target_viewport_width  * config_magnification,
                               target_viewport_height * config_magnification,
                               32,          // bitmap_pad
                               0            // bytes_per_line, calculated by X
        );
    if (NULL == host_image) {
        error("Failed to allocate XImage structure.");
    }
    host_image->data    = (void*)img_fb;

    x_win_width     = attr.width;
    x_win_height    = attr.height;
    win_width       = (x_win_width  + config_magnification - 1) / config_magnification;
    win_height      = (x_win_height + config_magnification - 1) / config_magnification;
    DEBUG(1, "  mapped window, X width %d, X height %d, win width %d, win height %d, blank on %d, display on %d\n",
          x_win_width, x_win_height, win_width, win_height, shared_fb_data->blank_on, shared_fb_data->display_on);

    // The palette may get initialized locally or by the target, depending
    // on the mode.
    if (palette_init_fn) {
        (*palette_init_fn)();
    }
    if (palette_update_fn) {
        (*palette_update_fn)();
    }
    if (shared_fb_data->display_on && shared_fb_data->blank_on) {
        (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
    } else {
        // Just draw the initial blank screen.
        blackout();
    }
}

static void
handle_auxiliary_request(void)
{
    synth_fb_auxiliary_request  req;
    int                         result;
    
    result = read(0, &req, sizeof(synth_fb_auxiliary_request));
    if (result <= 0) {
        // The I/O auxiliary has terminated, so assume the window has gone as well.
        exit(0);
    }
    DEBUG(2, "handle_auxiliary request %d\n", req.command);
    switch (req.command) {
      case SYNTH_FB_AUX_MAPPED:
        {
            handle_mapped(req.arg1);
            break;
        }
      case SYNTH_FB_AUX_ON:
        {
            // This is used when the target-side switches the display
            // off and back on, rather than just blanking it. Treat it
            // the same as blanking.
            DEBUG(1, "Target has switched the display on\n");
            shared_fb_data->display_on  = 1;
            (*render_fn)(shared_fb_data->viewport_x, shared_fb_data->viewport_y, win_width, win_height);
            break;
        }
      case SYNTH_FB_AUX_OFF:
        {
            DEBUG(1, "Target has switched the display off\n");
            shared_fb_data->display_on  = 0;
            blackout();
            break;
        }
      case SYNTH_FB_AUX_REDRAW:
        {
            DEBUG(1, "Auxiliary has requested a redraw\n");
            schedule_redraw(0, 0, x_win_width, x_win_height);
        }
    }
    DEBUG(2, "handle_auxiliary request done\n");
}

// Report an error to ecosynth during initialization. This means a
// single byte 0, followed by a string.
static void
report_init_error(char* msg)
{
    write(1, "0", 1);
    write(1, msg, strlen(msg));
    close(1);
    exit(0);
}

// ----------------------------------------------------------------------------
int
main(int argc, char** argv)
{
    int                 size;
    fd_set              read_fds;
    int                 max_fd;

    atexit(&atexit_handler);
    signal(SIGPIPE, SIG_IGN);

    if (12 != argc) {
        report_init_error("Incorrect number of arguments.");
    }
    target_id               = (int) strtoul(argv[1], NULL, 0);
    target_depth            = (int) strtoul(argv[2], NULL, 0);
    target_le               = (int) strtoul(argv[3], NULL, 0);
    target_width            = (int) strtoul(argv[4], NULL, 0);
    target_height           = (int) strtoul(argv[5], NULL, 0);
    target_viewport_width   = (int) strtoul(argv[6], NULL, 0);
    target_viewport_height  = (int) strtoul(argv[7], NULL, 0);
    target_stride           = (int) strtoul(argv[8], NULL, 0);
    target_number_pages     = (int) strtoul(argv[9], NULL, 0);
    target_format           = argv[10];
    config_magnification    = (int) strtoul(argv[11], NULL, 0);

    if ((target_depth != 1) && (target_depth != 2) && (target_depth != 4) && (target_depth != 8) && (target_depth != 16) && (target_depth != 32)) {
        report_init_error("Invalid target depth.");
    }
    if ((target_width < 16) || (target_width > 4096)) {
        report_init_error("Invalid target width.");
    }
    if ((target_height < 16) || (target_height > 4096)) {
        report_init_error("Invalid target height.");
    }
    if ((target_viewport_width < 16) || (target_viewport_width > target_width)) {
        report_init_error("Invalid target viewport width.");
    }
    if ((target_viewport_height < 16) || (target_viewport_height > target_height)) {
        report_init_error("Invalid target viewport height.");
    }
    if ((target_number_pages < 1) || (target_number_pages > 4)) {
        report_init_error("Invalid target number of pages.");
    }
    if ((config_magnification < 1) || (config_magnification > 4)) {
        report_init_error("Invalid config magnification.");
    }

    host_display    = XOpenDisplay(NULL);
    if (NULL == host_display) {
        report_init_error("Failed to open X display.");
    }
    img_fb = (unsigned int*) malloc(target_viewport_width * target_viewport_height * sizeof(int) * config_magnification * config_magnification);
    if (NULL == img_fb) {
        report_init_error("Failed to allocate XImage data.");
    }
    blackout();

    // Use of tmpnam() is generally discouraged and generates a linker
    // warning, but only three temporary file names are needed and
    // there are no root privileges involved so security is not a big
    // issue. mkstemp() cannot easily be used with fifos.
    if (NULL == tmpnam(fifo_t2h_name)) {
        report_init_error("Failed to create unique file name for target->host fifo.");
    }
    if (0 != mkfifo(fifo_t2h_name, S_IRUSR | S_IWUSR)) {
        report_init_error("Failed to create target->host fifo.");
    }
    // Opening O_RDONLY or O_WRONLY will result in blocking until the
    // other end is open as well, which is not what is wanted here.
    // Instead we use the Linux feature of opening with O_RDWR which
    // gives non-blocking behaviour.
    fifo_t2h_created    = 1;
    fifo_t2h_fd         = open(fifo_t2h_name, O_RDWR);
    if (-1 == fifo_t2h_fd) {
        report_init_error("Failed to open target->host fifo.");
    }
    if (NULL == tmpnam(fifo_h2t_name)) {
        report_init_error("Failed to create unique file name for host->target fifo.");
    }
    if (0 != mkfifo(fifo_h2t_name, S_IRUSR | S_IWUSR)) {
        report_init_error("Failed to create host->target fifo.");
    }
    fifo_h2t_created    = 1;
    fifo_h2t_fd         = open(fifo_h2t_name, O_RDWR);
    if (-1 == fifo_h2t_fd) {
        report_init_error("Failed to open target->host fifo.");
    }

    size    = sizeof(synth_fb_data) + (target_height * target_stride * target_number_pages);
    if (NULL == tmpnam(shm_name)) {
        report_init_error("Failed to create unique file name for shared memory.");
    }
    shm_fd  = open(shm_name, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
    if (shm_fd < 0) {
        report_init_error("Failed to open file for shared memory region.");
    }
    shm_created = 1;
    if (ftruncate(shm_fd, size) < 0) {
        report_init_error("Failed to set shared memory file size.");
    }
    
    shared_fb_data = (synth_fb_data*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_fb_data == (synth_fb_data*)MAP_FAILED) {
        report_init_error("Failed to mmap shared memory file.");
    }
    target_fb   = &(shared_fb_data->framebuf[0]);

    shared_fb_data->connected           = 0;
    shared_fb_data->fifo_to_framebuf    = -1;
    shared_fb_data->fifo_from_framebuf  = -1;
    shared_fb_data->devid               = target_id;
    shared_fb_data->sync_x0             = 0;
    shared_fb_data->sync_y0             = 0;
    shared_fb_data->sync_x1             = target_viewport_width;
    shared_fb_data->sync_y1             = target_viewport_height;
    shared_fb_data->display_on          = 0;
    shared_fb_data->blank_on            = 1;
    shared_fb_data->viewport_x          = 0;
    shared_fb_data->viewport_y          = 0;
    shared_fb_data->page_visible        = 0;

    render_fn   = &dummy_render_fn;
    msync(shared_fb_data, size, MS_SYNC);

    // Everything seems to be in order. Report back to the auxiliary.
    {
        char    buf[513];
        if ((strlen(shm_name) + strlen(fifo_t2h_name) + strlen(fifo_h2t_name) + 3) > 512) {
            report_init_error("Temporary path names too long.");
        }
        buf[0]  = '1';
        strcpy(&(buf[1]), shm_name);
        strcat(&(buf[1]), ";");
        strcat(&(buf[1]), fifo_t2h_name);
        strcat(&(buf[1]), ";");
        strcat(&(buf[1]), fifo_h2t_name);
        strcat(&(buf[1]), ";");
        write(1, buf, 2 + strlen(&(buf[1])));
    }
    
    // Now we just loop, processing events. We want to select on file descriptor
    // 0 from the auxiliary, the X file descriptor, and the fifo t2h descriptor.
    // The latter may go away. X should never go away, and if the auxiliary goes
    // away we exit immediately.
    max_fd  = MAX(ConnectionNumber(host_display), fifo_t2h_fd);
    while ( 1 ) {
        while (XPending(host_display) > 0) {
            handle_X_event();
        }
        FD_ZERO(&read_fds);
        FD_SET(0, &read_fds);
        FD_SET(ConnectionNumber(host_display), &read_fds);
        if (-1 != fifo_t2h_fd) {
            FD_SET(fifo_t2h_fd, &read_fds);
        }
        DEBUG(3, "framebuf main loop: selecting on 0x%08x\n", *(int*)&read_fds);
        do_redraw();
        if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) >= 0) {
            if (FD_ISSET(0, &read_fds)) {
                DEBUG(2, "framebuf main loop: auxiliary request\n");
                handle_auxiliary_request();
            }
            if ((-1 != fifo_t2h_fd) && FD_ISSET(fifo_t2h_fd, &read_fds)) {
                DEBUG(2, "framebuf main loop: target request\n");
                handle_target_request();
            }
            if (FD_ISSET(ConnectionNumber(host_display), &read_fds)) {
                DEBUG(3, "framebuf main loop: X request\n");
                (void)XEventsQueued(host_display, QueuedAfterReading);
            }
        }
    }
    
    exit(EXIT_SUCCESS);
}
#endif // INSTANTIATE_CONVERTER
