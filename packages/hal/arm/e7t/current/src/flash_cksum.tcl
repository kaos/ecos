#!/bin/sh
# these lines restart using the tcl shell \
  exec sh -c "if ( echo | tclsh ) 2>/dev/null ; then \
      exec tclsh \"${0}\" ${1+${*}} ; \
    elif ( echo | cygtclsh80 ) 2>/dev/null ; then \
      exec cygtclsh80 \"${0}\" ${1+${*}} ; \
    else \
      echo Could not find TCL interpreter ; \
      exit 1 ; \
    fi"

#===============================================================================
#
#    flash_cksum.tcl
#
#    Compute the checksum for a AEB-1 flash module
#
#===============================================================================
#####COPYRIGHTBEGIN####
#                                                                          
# -------------------------------------------                              
# The contents of this file are subject to the Red Hat eCos Public License 
# Version 1.1 (the "License"); you may not use this file except in         
# compliance with the License.  You may obtain a copy of the License at    
# http://www.redhat.com/                                                   
#                                                                          
# Software distributed under the License is distributed on an "AS IS"      
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the 
# License for the specific language governing rights and limitations under 
# the License.                                                             
#                                                                          
# The Original Code is eCos - Embedded Configurable Operating System,      
# released September 30, 1998.                                             
#                                                                          
# The Initial Developer of the Original Code is Red Hat.                   
# Portions created by Red Hat are                                          
# Copyright (C) 1998, 1999, 2000 Red Hat, Inc.                             
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	gthomas
# Contact(s):	gthomas
# Date:		1999/01/06
# Version:	0.01 $RcsVersion$
# Purpose:      Compute the checksum for a FLASH ROM module.
# Description:
# Requires:     A working Tcl interpreter, V8.0 or later.
# Provides:     Command line operation only.
# Inputs:       
# Side FX:      
# Outputs:
# See also:	'Notes' which describes the process of creating the GDB stubs
#               module.
# Known bugs:   
# Usage:
#
#####DESCRIPTIONEND####
#===============================================================================

#
# This ckecksum is the 32-bit XOR of all 32 bit words in the file.
# The output of the program is a single number, suitable for use in
# a Makefile/script used to create an AEB-1 module for FLASH ROM.
#

proc checksum { name } {

    set status [ catch {
        set fd [open $name "r"]
    fconfigure $fd -translation binary
        set data [read $fd]
        close $fd
    } message]

    if { $status != 0 } {
        puts "Unable to read file $name: $message"
        exit 1
    }

    set sum 0
    set length [string length $data]
    for { set i 0 } { $i < $length } { incr i 4 } {
        # Fetch the next 32 bit word.  The funky tests are to handle the case
        # of zero bytes.
        set char0 [string index $data [expr $i + 0]]
        if { "$char0" != "" } {
            scan $char0 "%c" ascii0
        } else {
            set ascii0 0
        }
        set char1 [string index $data [expr $i + 1]]
        if { "$char1" != "" } {
            scan $char1 "%c" ascii1
        } else {
            set ascii1 0
        }
        set char2 [string index $data [expr $i + 2]]
        if { "$char2" != "" } {
            scan $char2 "%c" ascii2
        } else {
            set ascii2 0
        }
        set char3 [string index $data [expr $i + 3]]
        if { "$char3" != "" } {
            scan $char3 "%c" ascii3
        } else {
            set ascii3 0
        }
        set word [expr $ascii0 << 24]
        set word [expr $word | [expr $ascii1 << 16]]
        set word [expr $word | [expr $ascii2 << 8]]
        set word [expr $word | $ascii3]
        set sum [expr $sum ^ $word]
        # puts "sum: [format %8.8X $sum], word: [format %8.8X $word]"
    }

    return $sum
}

proc bswap { native } {
    set byte0 [expr [expr $native >> 24] & 255]
    set byte1 [expr [expr $native >> 16] & 255]
    set byte2 [expr [expr $native >> 8] & 255]
    set byte3 [expr [expr $native >> 0] & 255]
    set swapped [expr $byte3 << 24]
    set swapped [expr $swapped | [expr $byte2 << 16]]
    set swapped [expr $swapped | [expr $byte1 << 8]]
    set swapped [expr $swapped | [expr $byte0 << 0]]
    # puts "native: [format %8.8X $native], swapped: [format %8.8X $swapped]"
    return $swapped
}

global tcl_platform
set byteOrder $tcl_platform(byteOrder)
# puts "platform byte order = $byteOrder"

# Parse command line arguments
set argc 0
array set args { }
foreach arg $argv {
    set args([incr argc]) $arg
}

if { "$argc" != "1" } {
	puts "usage: flash_cksum <file>"
	exit 1
}

set cksum [checksum $args(1)]

# if { "$byteOrder" == "littleEndian" } {
    # puts "Swap bytes!"
    set cksum [bswap $cksum]
# }

# Tcl up to at least version 8.3 has problems on alphas,
# Tcl_FormatObjCmd() invokes Tcl_GetIntFromObj() which
# ends up doing dodgy conversions between int and long.
# This results in an error if bit 32 is set. Rewriting
# the format string to output one byte at a time is
# safer.

# puts "[format 0x%8.8X $cksum]"

puts [format "0x%02X%02X%02X%02X"       \
	[expr (($cksum >> 24) & 0x0FF)] \
	[expr (($cksum >> 16) & 0x0FF)] \
	[expr (($cksum >>  8) & 0x0FF)] \
	[expr (($cksum      ) & 0x0FF)]]

