//==========================================================================
//
//      protocol.h
//
//      Data common to host and target.
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

// These requests are sent by eCos to the I/O auxiliary and the
// framebuf.tcl script.
//
// Protocol version number
#define SYNTH_FB_PROTOCOL_VERSION   0x01

// Initialize, getting back various file names.
#define SYNTH_FB_INIT               0x01
// Abort, failed to open the file names
#define SYNTH_FB_ABORT              0x02
// Switch a device back on.
#define SYNTH_FB_ON                 0x03
// Or off
#define SYNTH_FB_OFF                0x04

// These requests are sent by eCos to the host-side framebuf program
// to indicate what has changed.
//
// All connected
#define SYNTH_FB_OK                 0x01
// A double buffer synch.
#define SYNTH_FB_SYNC               0x02
// Palette change, redraw the lot
#define SYNTH_FB_WRITE_PALETTE      0x03
// Blank or unblank the screen
#define SYNTH_FB_BLANK              0x04
// Move the viewport
#define SYNTH_FB_VIEWPORT           0x05
// Page flipping
#define SYNTH_FB_PAGE_FLIP          0x06

// These requests go from the framebuf.tcl script to the framebuf
// program.
typedef struct synth_fb_auxiliary_request {
    unsigned int command;
    unsigned int arg1;
    unsigned int arg2;
} synth_fb_auxiliary_request;

// This is sent when the frame becomes visible. It is followed
// by a 32-bit windows id.
#define SYNTH_FB_AUX_MAPPED         0x01
// ON, processed by auxiliary and then passed on to framebuf
#define SYNTH_FB_AUX_ON             0x02
// OFF, processed by auxiliary and then passed on to framebuf
#define SYNTH_FB_AUX_OFF            0x03
// Refresh, sent by the auxiliary a second after mapping.
#define SYNTH_FB_AUX_REDRAW         0x04

// This data structure is in the shared memory region.
typedef struct synth_fb_data {
    // Do we have a connection to a host-side framebuf program to do
    // the drawing?
    int             connected;
    // Named fifo between framebuf and the synthetic target.
    int             fifo_to_framebuf;
    int             fifo_from_framebuf;
    // The device id for the auxiliary
    int             devid;
    // The bounding box for syncs.
    int             sync_x0;
    int             sync_y0;
    int             sync_x1;
    int             sync_y1;
    // Is the display on?
    int             display_on;
    // Current blank state
    int             blank_on;
    // Current viewport position, top left
    int             viewport_x;
    int             viewport_y;
    // Current page for page flipping
    int             page_visible;
    int             page_drawable;
    // The palette, if used.
    unsigned char   palette[3 * 256];
    // The framebuffer data follows.
    unsigned int    framebuf[1];
} synth_fb_data;
