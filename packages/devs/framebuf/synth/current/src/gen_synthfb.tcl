#!/usr/bin/env tclsh

#===============================================================================
#
#    gen_synthfb.tcl
#
#    Generate header files for all enabled synthetic target framebuffer devices.
#
#===============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
# -------------------------------------------
# This file is part of eCos, the Embedded Configurable Operating System.
# Copyright (C) 2008 Free Software Foundation, Inc.
#
# eCos is free software; you can redistribute it and/or modify it under
# the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 or (at your option) any later version.
#
# eCos is distributed in the hope that it will be useful, but WITHOUT ANY
# WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
# for more details.
#
# You should have received a copy of the GNU General Public License along
# with eCos; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
#
# As a special exception, if other files instantiate templates or use macros
# or inline functions from this file, or you compile this file and link it
# with other works to produce a work based on this file, this file does not
# by itself cause the resulting work to be covered by the GNU General Public
# License. However the source code for this file must still be made available
# in accordance with section (3) of the GNU General Public License.
#
# This exception does not invalidate any other reasons why a work based on
# this file might be covered by the GNU General Public License.
# -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	bartv
# Date:		2005-10-28
#
# This script is invoked via a custom make rule to generate files
# <cyg/io/framebufs/synth_fb.h> and synth_fbs.c
#
#####DESCRIPTIONEND####
#===============================================================================

proc do_it { prefix } {
    set pkgconf_file	[file join $prefix "include/pkgconf/devs_framebuf_synth.h"]
    if { ! [file exists $pkgconf_file] } {
        puts stderr "gen_synthfb.tcl: missing configuration header $pkgconf_file"
        exit 1
    }
    set fd [open $pkgconf_file "r"]
    set data [read $fd]
    close $fd
    
    array set defines	[list]
    foreach line [split $data "\n"] {
        if { [regexp -- {^\#define[[:space:]]*([a-zA-Z_][a-zA-Z0-9_]*)[[:space:]]+([^[:space:]]+)$} $line junk symbol value] } {
            set defines($symbol) $value
        }
    }

    set data \
        "/*
 * File <cyg/io/framebufs/synth_fb.h>
 *
 * This is a generated file - do not edit!
 * It should not be \#include'd directly. Instead use <cyg/io/framebuf.h>
 */

#include <cyg/io/framebuf.inl>

"
    
    for { set fb 0 } { $fb < 4 } { incr fb } {
        if { ! [info exists defines(CYGPKG_DEVS_FRAMEBUF_SYNTH_FB[set fb])] } {
            continue
        }
        
        set width	$defines(CYGNUM_DEVS_FRAMEBUF_SYNTH_FB[set fb]_WIDTH)
        set height	$defines(CYGNUM_DEVS_FRAMEBUF_SYNTH_FB[set fb]_HEIGHT)
        set flags0  "(CYG_FB_FLAGS0_LINEAR_FRAMEBUFFER | \\\n    CYG_FB_FLAGS0_DOUBLE_BUFFER | \\\n"
        if { [info exists defines(CYGIMP_DEVS_FRAMEBUF_SYNTH_FB[set fb]_VIEWPORT) ] } {
            set viewport_width	$defines(CYGNUM_DEVS_FRAMEBUF_SYNTH_FB[set fb]_VIEWPORT_WIDTH)
            set viewport_height	$defines(CYGNUM_DEVS_FRAMEBUF_SYNTH_FB[set fb]_VIEWPORT_HEIGHT)
            append flags0 "    CYG_FB_FLAGS0_VIEWPORT | \\\n"
        } else {
            set viewport_width  $width
            set viewport_height $height
        }
        if { [info exists defines(CYGNUM_DEVS_FRAMEBUF_SYNTH_FB[set fb]_PAGE_FLIPPING)] } {
            append flags0 "    CYG_FB_FLAGS0_PAGE_FLIPPING | \\\n"
        }
        set paletted		1
        switch -- $defines(CYGDAT_DEVS_FRAMEBUF_SYNTH_FB[set fb]_FORMAT) {
            1BPP_BE_PAL888 {
                set format	    "CYG_FB_FORMAT_1BPP_PAL888"
                set suffix	    "1BE"
                set depth	    1
                set stride	    [expr ($width + 7) >> 3]
            }
            1BPP_LE_PAL888 {
                append flags0	"    CYG_FB_FLAGS0_LE | \\\n"
                set format	    "CYG_FB_FORMAT_1BPP_PAL888"
                set suffix	    "1LE"
                set depth	    1
                set stride	    [expr ($width + 7) >> 3]
            }
            2BPP_BE_PAL888 {
                set format	    "CYG_FB_FORMAT_2BPP_PAL888"
                set suffix	    "2BE"
                set depth   	2
                set stride	    [expr ($width + 3) >> 2]
            }
            2BPP_LE_PAL888 {
                append flags0	"    CYG_FB_FLAGS0_LE | \\\n"
                set format	    "CYG_FB_FORMAT_2BPP_PAL888"
                set suffix	    "2LE"
                set depth	    2
                set stride	    [expr ($width + 3) >> 2]
            }
            4BPP_BE_PAL888 {
                set format	    "CYG_FB_FORMAT_4BPP_PAL888"
                set suffix	    "4BE"
                set depth	    4
                set stride	    [expr ($width + 1) >> 1]
            }
            4BPP_LE_PAL888 {
                append flags0	"    CYG_FB_FLAGS0_LE | \\\n"
                set format	    "CYG_FB_FORMAT_4BPP_PAL888"
                set suffix	    "4LE"
                set depth	    4
                set stride	    [expr ($width + 1) >> 1]
            }
            8BPP_PAL888 {
                set format	    "CYG_FB_FORMAT_8BPP_PAL888"
                set suffix	    "8"
                set depth	    8
                set stride	    $width
            }
            8BPP_TRUE_332 {
                set format		        "CYG_FB_FORMAT_8BPP_TRUE_332"
                set suffix		        "8"
                set paletted		    0
                set make_colour_fn	    "cyg_fb_dev_make_colour_8bpp_true_332"
                set break_colour_fn	    "cyg_fb_dev_break_colour_8bpp_true_332"
                set make_colour_macro	"CYG_FB_MAKE_COLOUR_8BPP_TRUE_332"
                set break_colour_macro	"CYG_FB_BREAK_COLOUR_8BPP_TRUE_332"
                set depth		        8
                set stride		        $width
            }
            16BPP_TRUE_565 {
                set format		        "CYG_FB_FORMAT_16BPP_TRUE_565"
                set suffix		        "16"
                set paletted		    0
                set make_colour_fn	    "cyg_fb_dev_make_colour_16bpp_true_565"
                set break_colour_fn	    "cyg_fb_dev_break_colour_16bpp_true_565"
                set make_colour_macro   "CYG_FB_MAKE_COLOUR_16BPP_TRUE_565"
                set break_colour_macro  "CYG_FB_BREAK_COLOUR_16BPP_TRUE_565"
                set depth		        16
                set stride		        [expr $width * 2]
            }
            16BPP_TRUE_555 {
                set format		        "CYG_FB_FORMAT_16BPP_TRUE_555"
                set suffix		        "16"
                set paletted		    0
                set make_colour_fn	    "cyg_fb_dev_make_colour_16bpp_true_555"
                set break_colour_fn	    "cyg_fb_dev_break_colour_16bpp_true_555"
                set make_colour_macro   "CYG_FB_MAKE_COLOUR_16BPP_TRUE_555"
                set break_colour_macro	"CYG_FB_BREAK_COLOUR_16BPP_TRUE_555"
                set depth		        16
                set stride		        [expr $width * 2]
            }
            32BPP_TRUE_0888 {
                set format		        "CYG_FB_FORMAT_32BPP_TRUE_0888"
                set suffix		        "32"
                set paletted		    0
                set make_colour_fn	    "cyg_fb_dev_make_colour_32bpp_true_0888"
                set break_colour_fn	    "cyg_fb_dev_break_colour_32bpp_true_0888"
                set make_colour_macro	"CYG_FB_MAKE_COLOUR_32BPP_TRUE_0888"
                set break_colour_macro  "CYG_FB_BREAK_COLOUR_32BPP_TRUE_0888"
                set depth		        32
                set stride		        [expr $width * 4]
            }
        }
        if { $paletted } {
            append flags0		"    CYG_FB_FLAGS0_PALETTE | \\\n    CYG_FB_FLAGS0_WRITEABLE_PALETTE | \\\n"
            set write_palette_fn	"cyg_synth_fb_write_palette"
            set read_palette_fn		"cyg_synth_fb_read_palette"
            set make_colour_fn		"cyg_fb_nop_make_colour"
            set break_colour_fn		"cyg_fb_nop_break_colour"
        } else {
            append flags0 		"    CYG_FB_FLAGS0_TRUE_COLOUR | \\\n"
            set write_palette_fn	"cyg_fb_nop_write_palette"
            set read_palette_fn		"cyg_fb_nop_read_palette"
        }
        append flags0		"    CYG_FB_FLAGS0_BLANK)"

        # The framebuffer parameters
        append data \
            "
extern cyg_fb cyg_synth_fb[set fb];
extern cyg_uint8* cyg_synth_fb[set fb]_base;
#define CYG_FB_fb[set fb]_STRUCT            cyg_synth_fb[set fb]
#define CYG_FB_fb[set fb]_DEPTH             $depth
#define CYG_FB_fb[set fb]_FORMAT            $format
#define CYG_FB_fb[set fb]_WIDTH             $width
#define CYG_FB_fb[set fb]_HEIGHT            $height
#define CYG_FB_fb[set fb]_VIEWPORT_WIDTH    $viewport_width
#define CYG_FB_fb[set fb]_VIEWPORT_HEIGHT   $viewport_height
#define CYG_FB_fb[set fb]_BASE              cyg_synth_fb[set fb]_base
#define CYG_FB_fb[set fb]_STRIDE            $stride
#define CYG_FB_fb[set fb]_FLAGS0            $flags0
#define CYG_FB_fb[set fb]_FLAGS1            0
#define CYG_FB_fb[set fb]_FLAGS2            0
#define CYG_FB_fb[set fb]_FLAGS3            0
"

        # These macros are not part of the usual framebuffer set, but are
        # useful because of the configurable nature of the synthetic target
        append data \
            "
#define CYG_FB_fb[set fb]_SUFFIX            $suffix
#define CYG_FB_fb[set fb]_READ_PALETTE_FN   $read_palette_fn
#define CYG_FB_fb[set fb]_WRITE_PALETTE_FN  $write_palette_fn
#define CYG_FB_fb[set fb]_MAKE_COLOUR_FN    $make_colour_fn
#define CYG_FB_fb[set fb]_BREAK_COLOUR_FN   $break_colour_fn
"
        # And the macro API, all done in terms of the linear framebuffer functions.
        append data \
            "
#define CYG_FB_fb[set fb]_PIXELx_VAR(  _fb_, _id_)    CYG_FB_PIXELx_VAR_[set suffix](  _fb_, _id_)
#define CYG_FB_fb[set fb]_PIXELx_SET(  _fb_, _id_, _x_, _y_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_SET_[set suffix](  _fb_, _id_, CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, _x_, _y_); \\
    CYG_MACRO_END
#define CYG_FB_fb[set fb]_PIXELx_GET(  _fb_, _id_, _x_, _y_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_GET_[set suffix](  _fb_, _id_, CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, _x_, _y_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_PIXELx_ADDX( _fb_, _id_, _incr_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_ADDX_[set suffix]( _fb_, _id_, CYG_FB_fb[set fb]_STRIDE, _incr_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_PIXELx_ADDY( _fb_, _id_, _incr_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_ADDY_[set suffix]( _fb_, _id_, CYG_FB_fb[set fb]_STRIDE, _incr_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_PIXELx_WRITE(_fb_, _id_, _colour_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_WRITE_[set suffix](_fb_, _id_, _colour_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_PIXELx_READ( _fb_, _id_) \\
    ({ CYG_FB_PIXELx_READ_[set suffix]( _fb_, _id_); }) 
#define CYG_FB_fb[set fb]_PIXELx_FLUSHABS( _fb_, _id_, _x0_, _y0_, _width_, _height_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_FLUSHABS_[set suffix](_fb_, _id_, _x0_, _y0_, _width_, _height_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_PIXELx_FLUSHREL( _fb_, _id_, _x0_, _y0_, _dx_, _dy_) \\
    CYG_MACRO_START \\
    CYG_FB_PIXELx_FLUSHREL_[set suffix](_fb_, _id_, _x0_, _y0_, _dx_, _dy_); \\
    CYG_MACRO_END 

#define CYG_FB_fb[set fb]_WRITE_PIXEL(_x_, _y_, _colour_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_write_pixel_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                    _x_, _y_, _colour_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_READ_PIXEL(_x_, _y_) \\
    ({ cyg_fb_linear_read_pixel_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, _x_, _y_); }) 
#define CYG_FB_fb[set fb]_WRITE_HLINE(_x_, _y_, _len_, _colour_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_write_hline_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                    _x_, _y_, _len_, _colour_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_WRITE_VLINE(_x_, _y_, _len_, _colour_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_write_vline_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                    _x_, _y_, _len_, _colour_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_FILL_BLOCK(_x_, _y_, _width_, _height_, _colour_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_fill_block_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                   _x_, _y_, _width_, _height_, _colour_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_WRITE_BLOCK(_x_, _y_, _width_, _height_, _data_, _offset_, _data_stride_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_write_block_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                    _x_, _y_, _width_, _height_, _data_, _offset_, _data_stride_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_READ_BLOCK(_x_, _y_, _width_, _height_, _data_, _offset_, _data_stride_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_read_block_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                   _x_, _y_, _width_, _height_, _data_, _offset_, _data_stride_); \\
    CYG_MACRO_END 
#define CYG_FB_fb[set fb]_MOVE_BLOCK(_x_, _y_, _width_, _height_, _new_x_, _new_y_) \\
    CYG_MACRO_START \\
    cyg_fb_linear_move_block_[set suffix]_inl(CYG_FB_fb[set fb]_BASE, CYG_FB_fb[set fb]_STRIDE, \\
                                   _x_, _y_, _width_, _height_, _new_x_, _new_y_); \\
    CYG_MACRO_END
"
        if { !$paletted } {
            append data \
                "
#define CYG_FB_fb[set fb]_MAKE_COLOUR(_r_, _g_, _b_)                \\
    ({ [set make_colour_macro](_r_, _g_, _b_); })
#define CYG_FB_fb[set fb]_BREAK_COLOUR(_colour_, _r_, _g_, _b_)     \\
    CYG_MACRO_START                                                 \\
    [set break_colour_macro](_colour_, _r_, _g_, _b_);              \\
    CYG_MACRO_END
"
        }
    }

    set old_data ""
    set framebuf_file [file join $prefix "include/cyg/io/framebufs/synth_fb.h"]
    if { [file exists $framebuf_file] && [file readable $framebuf_file] } {
        set fd [open $framebuf_file "r"]
        set old_data [read $fd]
        close $fd
    }
    if { ! [string equal $old_data $data] } {
        if { ! [info exists [file dirname $framebuf_file]] } {
            file mkdir [file dirname $framebuf_file]
        }
        set fd [open $framebuf_file "w"]
        puts -nonewline $fd $data
        close $fd
    }
}

if { 0 == $::argc } {
    puts stderr "gen_synthfb.tcl: missing argument for install directory"
    exit 1
}
if { [catch { do_it [lindex $::argv 0] } msg] } {
    puts stderr "gen_synthfb.tcl: internal error"
    puts stderr "    $msg"
    exit 1
}
exit 0

