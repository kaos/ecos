# {{{  Banner                                                   

# ============================================================================
# 
#      framebuf.tcl
# 
#      Framebuffer support for the eCos synthetic target I/O auxiliary
# 
# ============================================================================
# ####ECOSGPLCOPYRIGHTBEGIN####                                             
# -------------------------------------------                               
# This file is part of eCos, the Embedded Configurable Operating System.    
# Copyright (C) 2008 Free Software Foundation, Inc.                         
#
# eCos is free software; you can redistribute it and/or modify it under     
# the terms of the GNU General Public License as published by the Free      
# Software Foundation; either version 2 or (at your option) any later       
# version.                                                                  
#
# eCos is distributed in the hope that it will be useful, but WITHOUT       
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or     
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License     
# for more details.                                                         
#
# You should have received a copy of the GNU General Public License         
# along with eCos; if not, write to the Free Software Foundation, Inc.,     
# 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.             
#
# As a special exception, if other files instantiate templates or use       
# macros or inline functions from this file, or you compile this file       
# and link it with other works to produce a work based on this file,        
# this file does not by itself cause the resulting work to be covered by    
# the GNU General Public License. However the source code for this file     
# must still be made available in accordance with section (3) of the GNU    
# General Public License v2.                                                
#
# This exception does not invalidate any other reasons why a work based     
# on this file might be covered by the GNU General Public License.          
# -------------------------------------------                               
# ####ECOSGPLCOPYRIGHTEND####                                               
# ============================================================================
# #####DESCRIPTIONBEGIN####
# 
#  Author(s):   bartv
#  Contact(s):  bartv
#  Date:        2005/10/28
#  Version:     0.01
#  Description:
#      Implementation of a framebuffer device. This script should only ever
#      be run from inside the ecosynth auxiliary.
# 
# ####DESCRIPTIONEND####
# ============================================================================

# }}}

# Overview
#
# The synthetic framebuffer package supports up to four framebuffer
# devices. The eCos framebuffers are displayed by a separate C program
# framebuf which gets spawned by this script. framebuf operates using
# low-level X library calls. Doing the drawing inside Tcl/Tk would be
# far too slow, and trying to draw directly to an X display from inside
# an eCos application would be complicated.
#
# When an eCos framebuffer is switched on for the first time it will
# result in a device instantiation in this script. A suitable frame
# is created, and once it is mapped the framebuf program is started.
# The device data string from the target includes a protocol version
# number, target display depth, width and height, display format, etc.

namespace eval framebuf {

    # Protocol between the target-side and the auxiliary, as per protocol.h
    variable    SYNTH_FB_INIT       0x01
    variable    SYNTH_FB_ABORT      0x02
    variable    SYNTH_FB_ON         0x03
    variable    SYNTH_FB_OFF        0x04

    # Protocol between the auxiliary and the framebuf program
    variable    SYNTH_FB_AUX_MAPPED 0x01
    variable    SYNTH_FB_AUX_ON     0x02
    variable    SYNTH_FB_AUX_OFF    0x03
    variable    SYNTH_FB_AUX_REDRAW 0x04
    
    variable 	init_ok		1
    array set	settings	[]

    # Set DEBUG_LEVEL to between 0 for no debug output, 2 for maximum
    variable    DEBUG_LEVEL 0
    proc DEBUG { level devid msg } {
        if { $level < $framebuf::DEBUG_LEVEL } {
            if { [info exists framebuf::settings($devid,fbid)] } {
                set fbid $framebuf::settings($devid,fbid)
            } else {
                set fbid "<unknown>"
            }
            puts -nonewline stderr "framebuf.tcl (fb$fbid): $msg"; flush stderr
        }
    }
    
    # One-off initialization.
    variable install_dir	$synth::device_install_dir
    variable framebuf_executable [file join $framebuf::install_dir "framebuf"]
    if { ! [file exists $framebuf_executable] } {
        synth::report_error "Framebuffer device, framebuf executable has not been installed in $framebuf::install_dir.\n"
        set init_ok 0
    } elseif { ! [file executable $framebuf_executable] } {
        synth::report_error "Framebuffer device, installed program $framebuf_executable is not executable.\n"
        set init_ok 0
    }

    if { ! [file exists [file join $framebuf::install_dir "framebuf_icon.xbm"]] ||
         ! [file exists [file join $framebuf::install_dir "framebuf_iconmask.xbm"]] } {
        synth::report_error "Framebuffer device, bitmap support files have not been installed.\n"
        set init_ok =
    }

    # A default function for creating the Tk frame for an eCos
    # framebuffer device. Framebuffer 0 is usually part of the
    # main window, unless it would take up too much space.
    # Unfortunately at this stage of initialization there is no
    # easy way to work out how big . is going to be so some
    # hard-wired numbers are used instead.
    proc default_create_frame { fbid magnification depth little_endian width height viewport_width viewport_height stride number_pages format } {

        puts "default_create_frame $fbid mag $magnification width $viewport_width height $viewport_height"
        set viewport_width [expr $viewport_width * $magnification]
        set viewport_height [expr $viewport_height * $magnification]
        puts "default_create_frame $fbid mag $magnification width $viewport_width height $viewport_height"
        
        if { (0 == $fbid) && ($viewport_height <= 240) && ($viewport_width <= 640) } {
            frame           .fb$fbid -container 1 -height $viewport_height -width $viewport_width
            pack            .fb$fbid -in .main.n -expand 0 -anchor nw
            return          .fb$fbid
        } else {
            toplevel        .fb$fbid
            frame           .fb$fbid.frame -container 1 -height $viewport_height -width $viewport_width
            pack            .fb$fbid.frame -side top -expand 0 -anchor nw
            wm title        .fb$fbid	"Synth FB[set fbid] [set width]*[set height]*[set depth]bpp"
            wm iconbitmap   .fb$fbid "@[file join $framebuf::install_dir framebuf_icon.xbm]"
            wm iconmask     .fb$fbid "@[file join $framebuf::install_dir framebuf_iconmask.xbm]"
            wm iconname     .fb$fbid "Synth FB[set fbid]"
            wm protocol     .fb$fbid WM_DELETE_WINDOW { }
            return .fb$fbid.frame
        }
    }

    variable create_frame_proc	default_create_frame
    
    if { [synth::tdf_has_option "framebuf" "create_frame_proc"] } {
        set framebuf::create_frame_proc [synth::tdf_get_option "framebuf" "create_frame_proc"]
    }

    # Optional magnification
    for { set i 0 } { $i < 4 } { incr i } {
        set framebuf::settings(fb$i,magnification) 1
        if { [synth::tdf_has_option "framebuf" "fb[set i]_magnification"] } {
            set magnification [synth::tdf_get_option "framebuf" "fb[set i]_magnification"]
            if { ! [string is integer -strict $magnification] } {
                synth::report_error [concat
                                     "Framebuf device, invalid value in target definition file $synth::target_definition\n"
                                     "    fb[set i]_magnification should be a simple integer, not $magnification\n"]
                set init_ok 0
            } elseif { ($magnification < 1) || ($magnification > 4) } {
                synth::report_error [concat
                                     "Framebuf device, invalid value in target definition file $synth::target_definition\n"
                                     "    fb[set i]_magnification should be 1, 2, 3 or 4.\n"]
                set init_ok 0
            } else {
                set framebuf::settings(fb$i,magnification) $magnification
            }
        }
    }

    proc handle_framebuf_reply { devid } {
        DEBUG 1 $devid "reading reply from framebuf program\n"
        if { ! $framebuf::settings($devid,got_reply) } {
            # The first message from the framebuf program indicates success+pathnames or failure+message
            # This will wake up a vwait in the instantiate proc
            set framebuf::settings($devid,reply) [read $framebuf::settings($devid,fd)]
            set framebuf::settings($devid,got_reply) 1
        } else {
            # The framebuf program sends back no other data, but may issue warnings or failures
            synth::report [read $framebuf::settings($devid,fd)]
        }
    }
    
    proc send_framebuf_request { devid code arg1 arg2 } {
        # FIXME handle endianness
        set auxiliary_request [binary format iii $code $arg1 $arg2]
        puts -nonewline $framebuf::settings($devid,fd) $auxiliary_request
        flush $framebuf::settings($devid,fd)
    }
    
    proc handle_ecos_request { devid reqcode arg1 arg2 reqdata reqlen reply_len } {
        DEBUG 1 $devid "got request $reqcode from eCos application\n"
        if { $framebuf::SYNTH_FB_INIT == $reqcode } {
            # This request is used after instantiation to get hold of the connectivity information
            synth::send_reply 1 [string length $framebuf::settings($devid,connectivity)] $framebuf::settings($devid,connectivity)
        } elseif { $framebuf::SYNTH_FB_ABORT == $reqcode } {
            # FIXME: kill off the framebuf program? Destroy the window/frame?
            synth::report_error "framebuf: eCos application has failed to connect to host-side framebuf program"
            synth::report_error "        : graphical display is not operational"
        } elseif { $framebuf::SYNTH_FB_ON == $reqcode } {
            send_framebuf_request $devid $framebuf::SYNTH_FB_AUX_ON 0 0
        } elseif { $framebuf::SYNTH_FB_OFF == $reqcode } {
            send_framebuf_request $devid $framebuf::SYNTH_FB_AUX_OFF 0 0
        } else {
            synth::report_error "framebuf: got invalid request $reqcode from eCos application"
        }
    }
    
    proc mapped { devid } {
        DEBUG 1 $devid "frame has been mapped\n"
        bind $framebuf::settings($devid,frame) <Map> ""
        send_framebuf_request $devid $framebuf::SYNTH_FB_AUX_MAPPED [winfo id $framebuf::settings($devid,frame)] 0
        # This should not be necessary, but sometimes there are problems
        # with an initial XPutImage() not actually drawing anything.
        after 250 framebuf::send_framebuf_request $devid $framebuf::SYNTH_FB_AUX_REDRAW 0 0
    }
    
    proc instantiate { devid name data } {

        DEBUG 1 $devid "instantiate devid $devid, name $name, data $data\n"
        
        if { ! $synth::flag_gui } {
            # If we are not running under a windowing system we cannot
            # show the framebuffer data. Just stuck to the default
            # memory framebuffer.
            return ""
        }

        if { [info exists framebuf::settings($name,id)] } {
            synth::report_error "Framebuf device: attempt to create several $name instances\n"
            return ""
        }

        # Check that the framebuf program can actually handle this display.
        set depth [winfo depth .]
        if { (24 != $depth) && (32 != $depth) } {
            synth::report_error "Framebuf device: requires 24bpp or 32bpp X display\n"
            return ""
        }
        set visual [winfo visual .]
        if { ![string equal "truecolor" $visual] && ![string equal "directcolor" $visual] } {
            synth::report_error "Framebuf device: requires a truecolor or directcolor X display\n"
            return ""
        }
        
        # Data is a set of comma-separated parameters, mostly numeric,
        # describing the desired framebuffer device. The first number
        # is a protocol number as per ../src/protocol.h
        set junk 	""
        set protocol 	0
        if { ! [regexp -- {^(\d*),.*} $data junk protocol] } {
            synth::report_error "Framebuf device: missing protocol number for device $name\n"
            return ""
        }
        if { $protocol != 1 } {
            synth::report_error "Framebuf device: protocol mismatch\n    \
                     Target uses protocol version $protocol\n    \
                     Host is only up to version 1\n"
            return ""
        }
        set framebuf::settings($devid,name) $name
        set result [regexp -- {^(\d*),(\d*),(\d*),(\d*),(\d*),(\d*),(\d*),(\d*),(\d*),(\d*),(.*)$} $data junk protocol \
                        framebuf::settings($devid,fbid) \
                        framebuf::settings($devid,depth) \
                        framebuf::settings($devid,little_endian) \
                        framebuf::settings($devid,width) \
                        framebuf::settings($devid,height) \
                        framebuf::settings($devid,viewport_width) \
                        framebuf::settings($devid,viewport_height) \
                        framebuf::settings($devid,stride) \
                        framebuf::settings($devid,number_pages) \
                        framebuf::settings($devid,format)]
        if { ! $result } {
            synth::report_error "Framebuf device: invalid parameter string $data\n"
            return ""
        }
        set framebuf::settings($devid,magnification) $framebuf::settings(fb$framebuf::settings($devid,fbid),magnification)

        # Spawn the framebuf process. Its stdin and stdout are pipes
        # connected to ecosynth. Its stderr is redirected to the current
        # tty for debugging/diagnostics.
        set cmd "|$framebuf::framebuf_executable "
        append cmd "$framebuf::settings($devid,fbid) "
        append cmd "$framebuf::settings($devid,depth) "
        append cmd "$framebuf::settings($devid,little_endian) "
        append cmd "$framebuf::settings($devid,width) "
        append cmd "$framebuf::settings($devid,height) "
        append cmd "$framebuf::settings($devid,viewport_width) "
        append cmd "$framebuf::settings($devid,viewport_height) "
        append cmd "$framebuf::settings($devid,stride) "
        append cmd "$framebuf::settings($devid,number_pages) "
        append cmd "$framebuf::settings($devid,format) "
        append cmd "$framebuf::settings($devid,magnification) "

        DEBUG 1 $devid "spawning framebuf program : $cmd\n"
        if { [catch { set framebuf::settings($devid,fd) [open "$cmd 2>/dev/tty" "w+"] } message] } {
            synth::report_error "Failed to spawn framebuf process for device $name\n    $message"
            return ""
        }
        set framebuf::settings($devid,reply) 0
        set framebuf::settings($devid,got_reply) 0
        fconfigure $framebuf::settings($devid,fd) -translation binary -blocking 0
        fileevent $framebuf::settings($devid,fd) readable "framebuf::handle_framebuf_reply $devid"
        
        # Now wait for the framebuf device to initialize. It should send back a single byte,
        # 0 for failure followed by an error string, 1 for success followed by a string with
        # the connectivity information.
        DEBUG 1 $devid "waiting for initial reply from framebuf program\n"
        vwait framebuf::settings($devid,reply)
        if { "" == $framebuf::settings($devid,reply) } {
            synth::report_error "framebuf process for device $name exited unexpectedly.\n"
            catch { close $framebuf::settings($devid,fd) }
            return ""
        }
        set code [string index $framebuf::settings($devid,reply) 0]
        if { "0" == $code } {
            synth::report_error "framebuf process was unable to initialize eCos device $name\n    $message"
            catch { close $framebuf::settings($devid,fd) }
            return ""
        }
        if { "1" !=  $code } {
            synth::report_error "Unexpected response $code from framebuf process for eCos device $name\n"
            catch { close $framebuf::settings($devid,fd) }
            return ""
        }
        set framebuf::settings($devid,connectivity) [string range $framebuf::settings($devid,reply) 1 end]

        # Now the the framebuf program is up and running, create the frame.
        # Doing this earlier could result in spurious frames appearing if
        # the framebuf program failed, and also causes timing problems
        # between the <Map> binding and the framebuf program being ready.
        DEBUG 1 $devid "invoking create_frame procedure $framebuf::create_frame_proc\n"
        set framebuf::settings($devid,frame)                  	\
            [$framebuf::create_frame_proc                   	\
                 $framebuf::settings($devid,fbid)           	\
                 $framebuf::settings($devid,magnification)   	\
                 $framebuf::settings($devid,depth)           	\
                 $framebuf::settings($devid,little_endian)   	\
                 $framebuf::settings($devid,width)           	\
                 $framebuf::settings($devid,height)          	\
                 $framebuf::settings($devid,viewport_width)  	\
                 $framebuf::settings($devid,viewport_height) 	\
                 $framebuf::settings($devid,stride)          	\
                 $framebuf::settings($devid,number_pages)    	\
                 $framebuf::settings($devid,format)          	\
                ]
        DEBUG 1 $devid "frame is $framebuf::settings($devid,frame)\n"
        if { "" == $framebuf::settings($devid,frame) } {
            return ""
        }

        # Everything appears to be up and running. When the frame becomes visible,
        # inform the framebuf program.
        if { [winfo ismapped $framebuf::settings($devid,frame)] } {
            framebuf::mapped $devid
        } else {
            bind $framebuf::settings($devid,frame) <Map>        "framebuf::mapped $devid"
        }
        DEBUG 1 $devid "instantiate succeeded, connectivity $framebuf::settings($devid,connectivity)\n"

        return framebuf::handle_ecos_request
    }
}

if { $framebuf::init_ok } {
    return framebuf::instantiate
} else {
    synth::report "Framebuffer cannot be instantiated, initialization failed.\n"
    return ""
}
