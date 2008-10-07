//==========================================================================
//
//      synthfb.c
//
//      Provide one or more framebuffer devices for the synthetic target.
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
// Date:          2005-10-28
//
//###DESCRIPTIONEND####
//========================================================================

#include <pkgconf/devs_framebuf_synth.h>
#include <pkgconf/io_framebuf.h>
#include <cyg/io/framebuf.h>
#include <errno.h>
#include <string.h>
#include <cyg/infra/cyg_type.h>
#include <cyg/infra/cyg_ass.h>
#include <cyg/infra/diag.h>
#include <cyg/hal/hal_io.h>
#include "protocol.h"

// Set the DEBUG_LEVEL to 0 for no debugging, 2 for maximum debugging.
#define DEBUG_LEVEL 0
#define DEBUG(_level_, _str_, ...)              \
    CYG_MACRO_START                             \
    if (_level_ <= DEBUG_LEVEL) {               \
        diag_printf( _str_, ## __VA_ARGS__);    \
    }                                           \
    CYG_MACRO_END

// Achieving high performance graphics operations in the synthetic
// target is difficult. Theoretically it might be possible to access
// the Linux framebuffer directly using MIT-SHM or DGA. However there
// is no practical way to handle the relevant parts of the X prococol,
// e.g. expose events. We also don't want to go via the
// general-purpose xchgmsg() functionality and a Tcl script, those
// are not appropriate for high performance.
//
// Instead if the I/O auxiliary is running a framebuffer device is
// instantiated, and the framebuf.tcl script will start a program
// framebuf. That program will create a shared memory region which
// then gets mapped into the synthetic target's address space. The
// shared memory region holds a synth_fb_data structure followed by
// the framebuffer data. If there are multiple framebuffer devices
// then there will be multiple invocations of the framebuf program.
//
// The framebuf program needs to respond to requests from several
// different sources. The X server may send events like expose.
// The synthetic target application can send synchronization requests,
// palette updates, and so on. The X events will come via a socket,
// and the requests from framebuf.tcl can come via a pipe. select()
// will serve for this. Several mechanisms can be used for the
// communication between the synthetic target application and framebuf
// including shared memory semaphores and reliable signals. To fit in
// neatly with the select() a named fifo is used.
//
// So assuming a framebuf program is running fb_op() writes a single
// byte to the fifo, then waits for framebuf to complete the
// operation. That wait also uses a named fifo. This keeps everything
// synchronous, and avoids some portability problems between
// Linux on various architectures.
static void
fb_op(cyg_fb* fb, int command)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    DEBUG(2, "target synthfb: fb_op %d\n", command);
    if (fb_data->connected) {
        cyg_uint8   data[1];
        int         rc;
        
        data[0] = (cyg_uint8)command;
        do {
            rc = cyg_hal_sys_write(fb_data->fifo_to_framebuf, (const void*) data, 1);
        } while (-CYG_HAL_SYS_EINTR == rc);
        do {
            rc = cyg_hal_sys_read(fb_data->fifo_from_framebuf, (void*) data, 1);
        } while (-CYG_HAL_SYS_EINTR == rc);
        if (rc < 0) {
            diag_printf("Internal error: unexpected result %d when receiving response from the framebuf program.\n", rc);
            diag_printf("              : disabling framebuffer device fb%d.\n", fb->fb_driver0);
            fb_data->connected = 0;
        }
    }
    DEBUG(2, "target synthfb: fb_op %d done\n", command);
}

// Create a framebuffer device. This gets called from a C++
// static constructor, to ensure that all the framebuffer
// windows are created early on during initialization before
// the host-side ecosynth support performs any cleanups.
void
_cyg_synth_fb_instantiate(struct cyg_fb* fb)
{
    synth_fb_data*  local_fb_data = (synth_fb_data*) fb->fb_driver2;
    synth_fb_data*  shared_fb_data;
    char            device_data[512];
    char            fb_name[4];
    char*           fb_format   = 0;
    char*           ptr;
    char*           filename;
    int             fd;
    int             len;
    int             reply;
    cyg_uint8*      fb_base;
    DEBUG(1, "target synth_fb_instantiate\n");

    if (!synth_auxiliary_running) {
        diag_printf("cyg_synth_fb_instantiate(): no I/O auxiliary, sticking with in-memory framebuffer\n");
        return;
    }

    diag_sprintf(fb_name, "fb%d", fb->fb_driver0);
    switch(fb->fb_format) {
        // Only bother with the formats used by gensynth_fb.tcl
      case CYG_FB_FORMAT_1BPP_PAL888:
        {
            static const cyg_uint8  fb_1bpp_palette[2 * 3]  = {
                0x00, 0x00, 0x00,   // colour 0 == black
                0xFF, 0xFF, 0xFF    // colour 1 == white
            };
            fb_format = "1BPP_PAL888";
            memcpy(local_fb_data->palette, fb_1bpp_palette, sizeof(fb_1bpp_palette));
            break;
        }
      case CYG_FB_FORMAT_2BPP_PAL888:
        {
            static const cyg_uint8  fb_2bpp_palette[4 * 3]  = {
                0x00, 0x00, 0x00,   // colour 0 == black
                0x54, 0x54, 0x54,   // dark grey
                0xA8, 0xA8, 0xA8,   // light grey
                0xFF, 0xFF, 0xFF    // colour 3 == white
            } ;
            fb_format = "2BPP_PAL888";
            memcpy(local_fb_data->palette, fb_2bpp_palette, sizeof(fb_2bpp_palette));
            break;
        }
      case CYG_FB_FORMAT_4BPP_PAL888:
        fb_format = "4BPP_PAL888";
        memcpy(local_fb_data->palette, cyg_fb_palette_ega, 16 * 3);
        break;
      case CYG_FB_FORMAT_8BPP_PAL888:
        fb_format = "8BPP_PAL888";
        memcpy(local_fb_data->palette, cyg_fb_palette_vga, 256 * 3);
        break;
      case CYG_FB_FORMAT_8BPP_TRUE_332:
        fb_format = "8BPP_TRUE_332";
        break;
      case CYG_FB_FORMAT_16BPP_TRUE_565:
        fb_format = "16BPP_TRUE_565";
        break;
      case CYG_FB_FORMAT_16BPP_TRUE_555:
        fb_format = "16BPP_TRUE_555";
        break;
      case CYG_FB_FORMAT_32BPP_TRUE_0888:
        fb_format = "32BPP_TRUE_0888";
        break;
    }
        
    diag_sprintf(device_data, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%s",
                 SYNTH_FB_PROTOCOL_VERSION,
                 fb->fb_driver0,  /* id 0-3   */
                 fb->fb_depth,
                 fb->fb_flags0 & CYG_FB_FLAGS0_LE,
                 fb->fb_width, fb->fb_height,
#ifdef CYGHWR_IO_FRAMEBUF_FUNCTIONALITY_VIEWPORT
                 fb->fb_viewport_width, fb->fb_viewport_height,
#else
                 fb->fb_width, fb->fb_height,
#endif
                 fb->fb_stride,
                 fb->fb_driver1,    /* number of pages */
                 fb_format);

    local_fb_data->devid = synth_auxiliary_instantiate("devs/framebuf/synth", SYNTH_MAKESTRING(CYGPKG_DEVS_FRAMEBUF_SYNTH),
                                                       "framebuf", fb_name, device_data);
    if (local_fb_data->devid < 0) {
        // The I/O auxiliary should have reported a suitable error.
        // Just stick with an in-memory device.
        diag_printf("cyg_synth_fb_instantiate(): failed to instantiate device, sticking with in-memory framebuffer\n");
        return;
    } else {
        DEBUG(1, "target synth_auxiliary_instantiate(), host-side framebuf utility running\n");
        // At this point the framebuf.tcl script has run, the window has
        // been created, the framebuf program has been started, it will
        // have created the shared memory region and the two fifos, 
        // passed the names back up to framebuf.tcl, and everything should
        // be running. This xchgmsg() retrieves the file names from
        // framebuf.tcl.
        synth_auxiliary_xchgmsg(local_fb_data->devid, SYNTH_FB_INIT, 0, 0, 0, 0, &reply, (unsigned char*)device_data, &len, 512);
        // First filename is the shared memory region.
        filename    = device_data;
        for (ptr = device_data; *ptr != ';'; ptr++)
            ;
        *ptr++  = '\0';
        DEBUG(1, "target :  Opening shared memory region %s\n", filename);
        fd = cyg_hal_sys_open(filename, CYG_HAL_SYS_O_RDWR, 0);
        if (fd < 0) {
            synth_auxiliary_xchgmsg(local_fb_data->devid, SYNTH_FB_ABORT, 0, 0, 0, 0, 0, 0, 0, 0);
            return;
        }
        shared_fb_data = (synth_fb_data*)cyg_hal_sys_mmap(0,
                                                          sizeof(synth_fb_data) + (fb->fb_height * fb->fb_stride * fb->fb_driver1),
                                                          CYG_HAL_SYS_PROT_READ | CYG_HAL_SYS_PROT_WRITE,
                                                          CYG_HAL_SYS_MAP_SHARED,
                                                          fd,
                                                          0
            );
        if (shared_fb_data <= 0) {
            synth_auxiliary_xchgmsg(local_fb_data->devid, SYNTH_FB_ABORT, 0, 0, 0, 0, 0, 0, 0, 0);
            cyg_hal_sys_close(fd);
            return;
        }
        DEBUG(1, "target:  mmap()'d shared memory region -> %p\n", shared_fb_data);
        shared_fb_data->devid       = local_fb_data->devid;
        
        // Next filename is the fifo to the framebuf program.
        filename = ptr;
        for (ptr = device_data; *ptr != ';'; ptr++)
            ;
        *ptr++ = '\0';
        DEBUG(1, "target:  Opening fifo to framebuf %s\n", filename);
        shared_fb_data->fifo_to_framebuf = cyg_hal_sys_open(filename, CYG_HAL_SYS_O_WRONLY, 0);
        if (shared_fb_data->fifo_to_framebuf < 0) {
            synth_auxiliary_xchgmsg(shared_fb_data->devid, SYNTH_FB_ABORT, 0, 0, 0, 0, 0, 0, 0, 0);
            cyg_hal_sys_close(fd);
            return;
        }
        // And finally the fifo from the framebuf program.
        filename = ptr;
        for (ptr = device_data; *ptr != ';'; ptr++)
            ;
        *ptr = '\0';
        DEBUG(1, "target:  Opening fifo from framebuf %s\n", filename);
        shared_fb_data->fifo_from_framebuf = cyg_hal_sys_open(filename, CYG_HAL_SYS_O_RDONLY, 0);
        if (shared_fb_data->fifo_from_framebuf < 0) {
            synth_auxiliary_xchgmsg(shared_fb_data->devid, SYNTH_FB_ABORT, 0, 0, 0, 0, 0, 0, 0, 0);
            cyg_hal_sys_close(fd);
            cyg_hal_sys_close(shared_fb_data->fifo_to_framebuf);
            return;
        }

        // We have a shared memory region and the two files. Copy
        // all existing fb_data contents (e.g. the palettes) and
        // the framebuffer contents to the shared memory region.
        // The MUST_BE_ON flag is not set for synthetic target
        // framebuffers so there may already be contents.
        fb_base  = (cyg_uint8*)fb->fb_base;
        fb_base -= (fb->fb_height * fb->fb_stride * shared_fb_data->page_drawable);
        memcpy(&(shared_fb_data->palette[0]), &(local_fb_data->palette[0]), 3 * 256);
        memcpy(&(shared_fb_data->framebuf[0]), fb_base, fb->fb_height * fb->fb_stride * fb->fb_driver1);
        shared_fb_data->connected   = 1;
        fb->fb_driver2              = (CYG_ADDRWORD)shared_fb_data;
            
        fb_base     = (cyg_uint8*)&(shared_fb_data->framebuf[0]);
        fb_base    += (fb->fb_height * fb->fb_stride * shared_fb_data->page_drawable);
        fb->fb_base = fb_base;
        *(cyg_uint8**)fb->fb_driver3    = fb_base;

        DEBUG(1, "target:  Fully instantiated, fb_data %p, blank_on %d, display_on %d\n",
              shared_fb_data, shared_fb_data->blank_on, shared_fb_data->display_on);
        DEBUG(1, "target:    devid %d, t2h %d, h2t %d\n",
                    shared_fb_data->devid, shared_fb_data->fifo_to_framebuf, shared_fb_data->fifo_from_framebuf);
        // Finally tell the framebuf program everything is ready.
        fb_op(fb, SYNTH_FB_OK);
        DEBUG(1, "target: Sent SYNTH_FB_OK\n");
    }
}


// Switch on a framebuffer device. This may get called multiple
// times, e.g. when switching between different screen modes.
// It just involves sending a message to the auxiliary.
static int
cyg_synth_fb_on(struct cyg_fb* fb)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    if (fb_data->connected) {
        synth_auxiliary_xchgmsg(fb_data->devid, SYNTH_FB_ON, 0, 0, 0, 0, 0, 0, 0, 0);
    }

    return 0;
}

static int
cyg_synth_fb_off(struct cyg_fb* fb)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    if (fb_data->connected) {
        synth_auxiliary_xchgmsg(fb_data->devid, SYNTH_FB_OFF, 0, 0, 0, 0, 0, 0, 0, 0);
    }
    return 0;
}

static int
cyg_synth_fb_ioctl(struct cyg_fb* fb, cyg_ucount16 key, void* data, size_t* len)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    int             result  = ENOSYS;

    switch(key) {
      case CYG_FB_IOCTL_VIEWPORT_GET_POSITION:
        DEBUG(1, "cyg_synth_fb_ioctl: viewport_get_position\n");
        if (fb->fb_flags0 & CYG_FB_FLAGS0_VIEWPORT) {
            cyg_fb_ioctl_viewport*  viewport = (cyg_fb_ioctl_viewport*)data;
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_viewport), "data argument should be a cyg_fb_ioctl_viewport structure");
            viewport->fbvp_x    = fb_data->viewport_x;
            viewport->fbvp_y    = fb_data->viewport_y;
            result  = 0;
            DEBUG(1, "                  : current viewport x %d, y %d\n", fb_data->viewport_x, fb_data->viewport_y);
        } else {
            DEBUG(1, "                  : framebuffer does not support a viewport\n");
        }
        break;
      case CYG_FB_IOCTL_VIEWPORT_SET_POSITION:
        DEBUG(1, "cyg_synth_fb_ioctl: viewport_set_position\n");
        if (fb->fb_flags0 & CYG_FB_FLAGS0_VIEWPORT) {
            cyg_fb_ioctl_viewport*  viewport = (cyg_fb_ioctl_viewport*)data;
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_viewport), "data argument should be a cyg_fb_ioctl_viewport structure");
            CYG_ASSERT(((viewport->fbvp_x + fb->fb_viewport_width) <= fb->fb_width) &&
                       ((viewport->fbvp_y + fb->fb_viewport_height) <= fb->fb_height),
                       "viewport should be within framebuffer dimensions");
            DEBUG(1, "                  : setting viewport from x %d, y %d to x %d, y %d\n",
                  fb_data->viewport_x, fb_data->viewport_y, (int) viewport->fbvp_x, (int) viewport->fbvp_y);
            if ((fb_data->viewport_x != (int)viewport->fbvp_x) || (fb_data->viewport_y != (int)viewport->fbvp_y)) {
                fb_data->viewport_x = (int)viewport->fbvp_x;
                fb_data->viewport_y = (int)viewport->fbvp_y;
                fb_op(fb, SYNTH_FB_VIEWPORT);
            }
            result = 0;
        } else {
            DEBUG(1, "                  : framebuffer does not support a viewport\n");
        }
        break;
      case CYG_FB_IOCTL_PAGE_FLIPPING_GET_PAGES:
        DEBUG(1, "cyg_synth_fb_ioctl: page_flipping_get_pages\n");
        if (fb->fb_flags0 & CYG_FB_FLAGS0_PAGE_FLIPPING) {
            cyg_fb_ioctl_page_flip* page_flip = (cyg_fb_ioctl_page_flip*)data;
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_page_flip), "data argument should be a cyg_fb_ioctl_page_flip structure");
            page_flip->fbpf_number_pages    = fb->fb_driver1;
            page_flip->fbpf_visible_page    = fb_data->page_visible;
            page_flip->fbpf_drawable_page   = fb_data->page_drawable;
            result = 0;
            DEBUG(1, "                  : number_pages %d, visible page %d, drawable page %d\n",
                  fb->fb_driver1, fb_data->page_visible, fb_data->page_drawable);
        } else {
            DEBUG(1, "                  : framebuffer does not support page flipping\n");
        }
        break;
      case CYG_FB_IOCTL_PAGE_FLIPPING_SET_PAGES:
        DEBUG(1, "cyg_synth_fb_ioctl: page_flipping_set_pages\n");
        if (fb->fb_flags0 & CYG_FB_FLAGS0_PAGE_FLIPPING) {
            cyg_fb_ioctl_page_flip* page_flip = (cyg_fb_ioctl_page_flip*)data;
            cyg_uint8*  fb_base;
            
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_page_flip), "data argument should be a cyg_fb_ioctl_page_flip structure");
            CYG_ASSERT((page_flip->fbpf_visible_page  < fb->driver1) &&
                       (page_flip->fbpf_drawable_page < fb->driver1),
                       "framebuffer does not have that many pages");
            DEBUG(1, "                  : drawable page was %d, now %d, visible page was %d, now %d\n",
                  fb_data->page_drawable, (int)page_flip->fbpf_drawable_page,
                  fb_data->page_visible, (int)page_flip->fbpf_visible_page);
            fb_base  = (cyg_uint8*)fb->fb_base;
            fb_base -= (fb->fb_height * fb->fb_stride * fb_data->page_drawable);
            fb_data->page_drawable          = page_flip->fbpf_drawable_page;
            fb_base += (fb->fb_height * fb->fb_stride * fb_data->page_drawable);
            fb->fb_base = fb_base;
            *(cyg_uint8**)fb->fb_driver3 = fb_base;
            if (fb_data->page_visible != (int)page_flip->fbpf_visible_page) {
                fb_data->page_visible = (int)page_flip->fbpf_visible_page;
                fb_op(fb, SYNTH_FB_PAGE_FLIP);
            }
            result = 0;
        } else {
            DEBUG(1, "                  : framebuffer does not support page flipping\n");
        }
        break;
      case CYG_FB_IOCTL_BLANK_GET:
        {
            cyg_fb_ioctl_blank* blank = (cyg_fb_ioctl_blank*)data;
            DEBUG(1, "cyg_synth_fb_ioctl: blank_get, current on state %d\n", fb_data->blank_on);
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_blank), "data argument should be a cyg_fb_ioctl_blank structure");
            blank->fbbl_on  = fb_data->blank_on;
            result = 0;
        }
        break;
      case CYG_FB_IOCTL_BLANK_SET:
        {
            cyg_fb_ioctl_blank* blank = (cyg_fb_ioctl_blank*)data;
            CYG_ASSERT(*len == sizeof(cyg_fb_ioctl_blank), "data argument should be a cyg_fb_ioctl_blank structure");
            DEBUG(1, "cyg_synth_fb_ioctl: blank_set, on was %d, now %d\n", fb_data->blank_on, blank->fbbl_on);
            if (blank->fbbl_on != fb_data->blank_on) {
                fb_data->blank_on = blank->fbbl_on;
                fb_op(fb, SYNTH_FB_BLANK);
            }
            result = 0;
        }
        break;
      default:
        result  = ENOSYS;
        break;
    }
    return result;
}

void
cyg_synth_fb_synch(struct cyg_fb* fb, cyg_ucount16 when)
{
    // FIXME: update synch_x0/y0/x1/y1 once the generic framebuffer
    // code actually maintains a bounding box.
    fb_op(fb, SYNTH_FB_SYNC);
}

#ifdef CYGHWR_DEVS_FRAMEBUF_SYNTH_FUNCTIONALITY_PALETTED
// The palette is held in the synth_fb_data structure.
static void
cyg_synth_fb_read_palette(struct cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, void* dest)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    CYG_ASSERT(fb->fb_flags0 & CYG_FB_FLAGS0_PALETTE, "reading palette of non-paletted display");
    CYG_ASSERT((first + count) <= (0x01 << fb->fb_depth), "palette size exceeded");

    memcpy(dest, &(fb_data->palette[3 * first]), 3 * count);
}

static void
cyg_synth_fb_write_palette(struct cyg_fb* fb, cyg_ucount32 first, cyg_ucount32 count, const void* source, cyg_ucount16 when)
{
    synth_fb_data*  fb_data = (synth_fb_data*) fb->fb_driver2;
    CYG_ASSERT(fb->fb_flags0 & CYG_FB_FLAGS0_PALETTE, "reading palette of non-paletted display");
    CYG_ASSERT((first + count) <= (0x01 << fb->fb_depth), "palette size exceeded");

    DEBUG(1, "write_palette: fb %p, first %d, count %d, source %p\n", fb, first, count, source);
    memcpy(&(fb_data->palette[3 * first]), source, 3 * count);
    fb_op(fb, SYNTH_FB_WRITE_PALETTE);
    CYG_UNUSED_PARAM(cyg_ucount16, when);
}
#endif

#define LINEAR1(_fn_, _suffix_)  cyg_fb_linear_ ## _fn_ ## _ ## _suffix_
#define LINEAR( _fn_, _suffix_)  LINEAR1(_fn_, _suffix_)

#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB0
# ifdef CYGNUM_DEVS_FRAMEBUF_SYNTH_FB0_PAGE_FLIPPING
#  define FB0_PAGES CYGNUM_DEVS_FRAMEBUF_SYNTH_FB0_PAGE_FLIPPING
# else
#  define FB0_PAGES 1
#endif

// A default area of memory for the framebuffer, if the auxiliary is not
// running.
static cyg_uint8            cyg_synth_fb0_default_base[CYG_FB_fb0_HEIGHT * CYG_FB_fb0_STRIDE * FB0_PAGES];

// Pointer to framebuffer memory. This defaults to a statically
// allocated memory but will switch to a shared memory region if
// the auxiliary is running. It may also change if page flipping
// is enabled.
cyg_uint8*  cyg_synth_fb0_base  = cyg_synth_fb0_default_base;

// Driver-specific data needed for interacting with the auxiliary.
static synth_fb_data    cyg_synth_fb0_data;

CYG_FB_FRAMEBUFFER(CYG_FB_fb0_STRUCT,
                   CYG_FB_fb0_DEPTH,
                   CYG_FB_fb0_FORMAT,
                   CYG_FB_fb0_WIDTH,
                   CYG_FB_fb0_HEIGHT,
                   CYG_FB_fb0_VIEWPORT_WIDTH,
                   CYG_FB_fb0_VIEWPORT_HEIGHT,
                   cyg_synth_fb0_default_base,
                   CYG_FB_fb0_STRIDE,
                   CYG_FB_fb0_FLAGS0,
                   CYG_FB_fb0_FLAGS1,
                   CYG_FB_fb0_FLAGS2,
                   CYG_FB_fb0_FLAGS3,
                   (CYG_ADDRWORD) 0,  // id, 0 - 3
                   (CYG_ADDRWORD) FB0_PAGES,
                   (CYG_ADDRWORD) &cyg_synth_fb0_data,
                   (CYG_ADDRWORD) &cyg_synth_fb0_base,
                   &cyg_synth_fb_on,
                   &cyg_synth_fb_off,
                   &cyg_synth_fb_ioctl,
                   &cyg_synth_fb_synch,
                   &CYG_FB_fb0_READ_PALETTE_FN,
                   &CYG_FB_fb0_WRITE_PALETTE_FN,
                   &CYG_FB_fb0_MAKE_COLOUR_FN,
                   &CYG_FB_fb0_BREAK_COLOUR_FN,
                   LINEAR(write_pixel, CYG_FB_fb0_SUFFIX),
                   LINEAR(read_pixel, CYG_FB_fb0_SUFFIX),
                   LINEAR(write_hline, CYG_FB_fb0_SUFFIX),
                   LINEAR(write_vline, CYG_FB_fb0_SUFFIX),
                   LINEAR(fill_block, CYG_FB_fb0_SUFFIX),
                   LINEAR(write_block, CYG_FB_fb0_SUFFIX),
                   LINEAR(read_block, CYG_FB_fb0_SUFFIX),
                   LINEAR(move_block, CYG_FB_fb0_SUFFIX),
                   0, 0, 0, 0       // Spare0 -> spare3
    );
                   
#endif

#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB1

# ifdef CYGNUM_DEVS_FRAMEBUF_SYNTH_FB1_PAGE_FLIPPING
#  define FB1_PAGES CYGNUM_DEVS_FRAMEBUF_SYNTH_FB1_PAGE_FLIPPING
# else
#  define FB1_PAGES 1
#endif
static cyg_uint8 cyg_synth_fb1_default_base[CYG_FB_fb1_HEIGHT * CYG_FB_fb1_STRIDE * FB1_PAGES];
cyg_uint8*  cyg_synth_fb1_base  = cyg_synth_fb1_default_base;
static synth_fb_data    cyg_synth_fb1_data;

CYG_FB_FRAMEBUFFER(CYG_FB_fb1_STRUCT,
                   CYG_FB_fb1_DEPTH,
                   CYG_FB_fb1_FORMAT,
                   CYG_FB_fb1_WIDTH,
                   CYG_FB_fb1_HEIGHT,
                   CYG_FB_fb1_VIEWPORT_WIDTH,
                   CYG_FB_fb1_VIEWPORT_HEIGHT,
                   cyg_synth_fb1_default_base,
                   CYG_FB_fb1_STRIDE,
                   CYG_FB_fb1_FLAGS0,
                   CYG_FB_fb1_FLAGS1,
                   CYG_FB_fb1_FLAGS2,
                   CYG_FB_fb1_FLAGS3,
                   (CYG_ADDRWORD) 1,  // id, 0 - 3
                   (CYG_ADDRWORD) FB1_PAGES,
                   (CYG_ADDRWORD) &cyg_synth_fb1_data,
                   (CYG_ADDRWORD) &cyg_synth_fb1_base,
                   &cyg_synth_fb_on,
                   &cyg_synth_fb_off,
                   &cyg_synth_fb_ioctl,
                   &cyg_synth_fb_synch,
                   &CYG_FB_fb1_READ_PALETTE_FN,
                   &CYG_FB_fb1_WRITE_PALETTE_FN,
                   &CYG_FB_fb1_MAKE_COLOUR_FN,
                   &CYG_FB_fb1_BREAK_COLOUR_FN,
                   LINEAR(write_pixel, CYG_FB_fb1_SUFFIX),
                   LINEAR(read_pixel, CYG_FB_fb1_SUFFIX),
                   LINEAR(write_hline, CYG_FB_fb1_SUFFIX),
                   LINEAR(write_vline, CYG_FB_fb1_SUFFIX),
                   LINEAR(fill_block, CYG_FB_fb1_SUFFIX),
                   LINEAR(write_block, CYG_FB_fb1_SUFFIX),
                   LINEAR(read_block, CYG_FB_fb1_SUFFIX),
                   LINEAR(move_block, CYG_FB_fb1_SUFFIX),
                   0, 0, 0, 0       // Spare0 -> spare3
    );
                   
#endif

#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB2

# ifdef CYGNUM_DEVS_FRAMEBUF_SYNTH_FB2_PAGE_FLIPPING
#  define FB2_PAGES CYGNUM_DEVS_FRAMEBUF_SYNTH_FB2_PAGE_FLIPPING
# else
#  define FB2_PAGES 1
#endif
static cyg_uint8 cyg_synth_fb2_default_base[CYG_FB_fb2_HEIGHT * CYG_FB_fb2_STRIDE * FB2_PAGES];
cyg_uint8*  cyg_synth_fb2_base  = cyg_synth_fb2_default_base;
static synth_fb_data    cyg_synth_fb2_data;

CYG_FB_FRAMEBUFFER(CYG_FB_fb2_STRUCT,
                   CYG_FB_fb2_DEPTH,
                   CYG_FB_fb2_FORMAT,
                   CYG_FB_fb2_WIDTH,
                   CYG_FB_fb2_HEIGHT,
                   CYG_FB_fb2_VIEWPORT_WIDTH,
                   CYG_FB_fb2_VIEWPORT_HEIGHT,
                   cyg_synth_fb2_default_base,
                   CYG_FB_fb2_STRIDE,
                   CYG_FB_fb2_FLAGS0,
                   CYG_FB_fb2_FLAGS1,
                   CYG_FB_fb2_FLAGS2,
                   CYG_FB_fb2_FLAGS3,
                   (CYG_ADDRWORD) 2,  // id, 0 - 3
                   (CYG_ADDRWORD) FB2_PAGES,
                   (CYG_ADDRWORD) &cyg_synth_fb2_data,
                   (CYG_ADDRWORD) &cyg_synth_fb2_base,
                   &cyg_synth_fb_on,
                   &cyg_synth_fb_off,
                   &cyg_synth_fb_ioctl,
                   &cyg_synth_fb_synch,
                   &CYG_FB_fb2_READ_PALETTE_FN,
                   &CYG_FB_fb2_WRITE_PALETTE_FN,
                   &CYG_FB_fb2_MAKE_COLOUR_FN,
                   &CYG_FB_fb2_BREAK_COLOUR_FN,
                   LINEAR(write_pixel, CYG_FB_fb2_SUFFIX),
                   LINEAR(read_pixel, CYG_FB_fb2_SUFFIX),
                   LINEAR(write_hline, CYG_FB_fb2_SUFFIX),
                   LINEAR(write_vline, CYG_FB_fb2_SUFFIX),
                   LINEAR(fill_block, CYG_FB_fb2_SUFFIX),
                   LINEAR(write_block, CYG_FB_fb2_SUFFIX),
                   LINEAR(read_block, CYG_FB_fb2_SUFFIX),
                   LINEAR(move_block, CYG_FB_fb2_SUFFIX),
                   0, 0, 0, 0       // Spare0 -> spare3
    );
                   
#endif

#ifdef CYGPKG_DEVS_FRAMEBUF_SYNTH_FB3

# ifdef CYGNUM_DEVS_FRAMEBUF_SYNTH_FB3_PAGE_FLIPPING
#  define FB3_PAGES CYGNUM_DEVS_FRAMEBUF_SYNTH_FB3_PAGE_FLIPPING
# else
#  define FB3_PAGES 1
#endif
static cyg_uint8 cyg_synth_fb3_default_base[CYG_FB_fb3_HEIGHT * CYG_FB_fb3_STRIDE * FB3_PAGES];
cyg_uint8*  cyg_synth_fb3_base  = cyg_synth_fb3_default_base;
static synth_fb_data    cyg_synth_fb3_data;

CYG_FB_FRAMEBUFFER(CYG_FB_fb3_STRUCT,
                   CYG_FB_fb3_DEPTH,
                   CYG_FB_fb3_FORMAT,
                   CYG_FB_fb3_WIDTH,
                   CYG_FB_fb3_HEIGHT,
                   CYG_FB_fb3_VIEWPORT_WIDTH,
                   CYG_FB_fb3_VIEWPORT_HEIGHT,
                   cyg_synth_fb3_default_base,
                   CYG_FB_fb3_STRIDE,
                   CYG_FB_fb3_FLAGS0,
                   CYG_FB_fb3_FLAGS1,
                   CYG_FB_fb3_FLAGS2,
                   CYG_FB_fb3_FLAGS3,
                   (CYG_ADDRWORD) 3,  // id, 0 - 3
                   (CYG_ADDRWORD) FB3_PAGES,
                   (CYG_ADDRWORD) &cyg_synth_fb3_data,
                   (CYG_ADDRWORD) &cyg_synth_fb3_base,
                   &cyg_synth_fb_on,
                   &cyg_synth_fb_off,
                   &cyg_synth_fb_ioctl,
                   &cyg_synth_fb_synch,
                   &CYG_FB_fb3_READ_PALETTE_FN,
                   &CYG_FB_fb3_WRITE_PALETTE_FN,
                   &CYG_FB_fb3_MAKE_COLOUR_FN,
                   &CYG_FB_fb3_BREAK_COLOUR_FN,
                   LINEAR(write_pixel, CYG_FB_fb3_SUFFIX),
                   LINEAR(read_pixel, CYG_FB_fb3_SUFFIX),
                   LINEAR(write_hline, CYG_FB_fb3_SUFFIX),
                   LINEAR(write_vline, CYG_FB_fb3_SUFFIX),
                   LINEAR(fill_block, CYG_FB_fb3_SUFFIX),
                   LINEAR(write_block, CYG_FB_fb3_SUFFIX),
                   LINEAR(read_block, CYG_FB_fb3_SUFFIX),
                   LINEAR(move_block, CYG_FB_fb3_SUFFIX),
                   0, 0, 0, 0       // Spare0 -> spare3
    );
                   
#endif
