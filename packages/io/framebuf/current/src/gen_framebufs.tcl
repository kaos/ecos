#!/bin/bash
# restart using a Tcl shell \
    exec sh -c 'for tclshell in tclsh tclsh83 cygtclsh80 ; do \
    ( echo | $tclshell ) 2> /dev/null && exec $tclshell "`( cygpath -w \"$0\" ) 2> /dev/null || echo $0`" "$@" ; \
    done ; \
    echo "gen_framebufs.tcl: cannot find Tcl shell" ; exit 1' "$0" "$@"

#===============================================================================
#
#    gen_framebufs.tcl
#
#    Amalgamate the various framebuffers available on a given platform
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
# Date:		2005-03-29
#
# This script is invoked via a custom make rule to generate a file
# <cyg/io/framebufs/framebufs.h>
#
#####DESCRIPTIONEND####
#===============================================================================

proc do_it { dir } {

    # dir should be set to <prefix>/install/include/cyg/io/framebufs
    if { ! [file exists $dir] } {
        file mkdir $dir
    }
    if { ! [file isdirectory $dir] } {
        puts stderr "gen_framebufs.tcl: strange build tree, $dir should be a directory"
        exit 1
    }
    
    set	framebufs_file 	[file join $dir "framebufs.h"]
    set	current_data	""
    if { [file exists $framebufs_file] } {
        set fd [open $framebufs_file "r"]
        set current_data [read $fd]
        close $fd
    }

    set headers	[lsort [glob -nocomplain  [file join $dir "*.h*"]]]

    set new_data \
        "\#ifndef CYGONCE_IO_FRAMEBUFS_FRAMEBUFS_H
# define CYGONCE_IO_FRAMEBUFS_FRAMEBUFS_H

/* This is a generated file - do not edit! 			*/
/* <cyg/io/framebufs/framebufs.h> should not be #include'd 	*/
/* directly, instead use <cyg/io/framebuf.h>			*/
"

    foreach header $headers {
        set header [file tail $header]
        if { [string equal "framebufs.h" $header] } {
            continue
        }
        append new_data "#include <cyg/io/framebufs/[set header]>\n"
    }

    # Next, work out the default test device. We need to read pkgconf/io_framebuf.h,
    # look for CYGDAT_IO_FRAMEBUF_DEVICES, and extract the first entry.
    set pkgconf_file [file join $dir "../../../pkgconf/io_framebuf.h"]
    if { ! [file exists $pkgconf_file] || ! [file readable $pkgconf_file] } {
        puts stderr "gen_framebufs.tcl: strange build tree, no access to $pkgconf_file"
        exit 1
    }
    set fd [open $pkgconf_file "r"]
    while { ! [eof $fd] } {
        set line [gets $fd]
        if { [regexp {^#define\s*CYGDAT_IO_FRAMEBUF_DEVICES\s*(\S*)\s+.*$} $line junk fb] } {
            append new_data "#define CYGDAT_IO_FRAMEBUF_DEFAULT_TEST_DEVICE $fb\n"
            break
        }
    }

    # Close the #ifndef CYGONCE_
    append new_data "#endif\n"

    if { ! [string equal $current_data $new_data] } {
        set fd [open $framebufs_file "w"]
        puts -nonewline $fd $new_data
        close $fd
    }
}

if { 0 == $::argc } {
    puts stderr "gen_framebufs.tcl: missing argument for install directory"
    exit 1
}

if { [catch { do_it [lindex $::argv 0] } msg] } {
    puts stderr "gen_framebufs.tcl: internal error"
    puts stderr "    $msg"
    exit 1
}
exit 0
