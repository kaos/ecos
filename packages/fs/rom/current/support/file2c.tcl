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
#    file2c.tcl
#
#    Convert a file into a header that can be #included from C.
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
# Copyright (C) 1998, 1999, 2000, 2001 Red Hat, Inc.                  
# All Rights Reserved.                                                     
# -------------------------------------------                              
#                                                                          
#####COPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	jlarmour,bartv
# Contact(s):	
# Date:		2001-07-20
# Purpose:      
# Description:
# Usage:        file2c.tcl <file to encode> <output C header file>
#
#####DESCRIPTIONEND####
#===============================================================================



if { $argc != 2 } {
    puts "Usage: file2c.tcl <file to encode> <output C header file>"
    exit 1
}
set infile [lindex $argv 0]
set outfile [lindex $argv 1]

set status [ catch {
    set infilefd [open $infile "r"]
    fconfigure $infilefd -translation binary
    set data [read $infilefd]
    close $infilefd
} message]

if { $status != 0 } {
    error "Unable to read file $infile: $message"
}

set result ""

set status [ catch {
    set outfilefd [ open $outfile "w" ]
} message ]

if { $status != 0 } {
    error "Unable to create file $outfile: $message"
}

append result "/* This is a generated file. Do not edit. */\n\n"
append result "static const unsigned char filedata\[\] __attribute__((aligned(32))) = {\n"

set datalength [ string length $data ]

set aligned_datalength [expr $datalength - ($datalength % 8)]

for { set i 0 } {$i < $aligned_datalength} {incr i 8} {
    binary scan $data "@[set i]H16" var0
    append result [format "    0x%2s, 0x%2s, 0x%2s, 0x%2s, 0x%2s, 0x%2s, 0x%2s, 0x%2s,\n" \
            [string range $var0  0  1] \
            [string range $var0  2  3] \
            [string range $var0  4  5] \
            [string range $var0  6  7] \
            [string range $var0  8  9] \
            [string range $var0 10 11] \
            [string range $var0 12 13] \
            [string range $var0 14 15]]
}

if { $aligned_datalength != $datalength } {
    append result "    "
    for { set i $aligned_datalength } {$i < $datalength} {incr i} {
        binary scan $data "@[set i]H2" var0
        append result [format "0x%2s, " $var0]
    }
}

# Remove either comma+newline or comma+space from the end
set result [string range $result 0 [expr [string length $result] - 3]]

append result "\n};"

puts $outfilefd $result
close $outfilefd
