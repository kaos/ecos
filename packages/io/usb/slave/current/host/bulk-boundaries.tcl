#===============================================================================
#
#    bulk-boundaries.tcl
#
#    Support for USB testing
#
#===============================================================================
#####ECOSGPLCOPYRIGHTBEGIN####
## -------------------------------------------
## This file is part of eCos, the Embedded Configurable Operating System.
## Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
##
## eCos is free software; you can redistribute it and/or modify it under
## the terms of the GNU General Public License as published by the Free
## Software Foundation; either version 2 or (at your option) any later version.
##
## eCos is distributed in the hope that it will be useful, but WITHOUT ANY
## WARRANTY; without even the implied warranty of MERCHANTABILITY or
## FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
## for more details.
##
## You should have received a copy of the GNU General Public License along
## with eCos; if not, write to the Free Software Foundation, Inc.,
## 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
##
## As a special exception, if other files instantiate templates or use macros
## or inline functions from this file, or you compile this file and link it
## with other works to produce a work based on this file, this file does not
## by itself cause the resulting work to be covered by the GNU General Public
## License. However the source code for this file must still be made available
## in accordance with section (3) of the GNU General Public License.
##
## This exception does not invalidate any other reasons why a work based on
## this file might be covered by the GNU General Public License.
## -------------------------------------------
#####ECOSGPLCOPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	asl
# Date:		2006-04-21
# Purpose:      Test the bulk endpoints with different sizes of packets
#               at the important boundaries.
#
#####DESCRIPTIONEND####
#===============================================================================

set pktsizes { 1 2 3 4 5 7 8 9 15 16 17 31 32 33 63 64 65 127 128 129 \
               255 256 257 511 512 513 1023 1024 1025 2047 2048 2049 \
               4095 4096 4097 }

 if { 0 != [llength $usbtest::bulk_in_endpoints] } {
     puts "Bulk IN endpoints: $usbtest::bulk_in_endpoints"
     foreach ep $usbtest::bulk_in_endpoints {
 	puts [format "  %2d: packet sizes %d to %d, padding %d" $ep \
                   $usbtest::bulk_in($ep,min_size) \
                   $usbtest::bulk_in($ep,max_size) \
                   $usbtest::bulk_in($ep,max_in_padding)]
         foreach pktsize $pktsizes {
             if { $pktsize <=  $usbtest::bulk_in($ep,max_size) } {
                 puts [format "Testing IN endpoint %d with packet size %4d" \
                           $ep $pktsize]
                 usbtest::reset
                 usbtest::bulktest $ep in 5 txsize1=$pktsize format=byteseq \
                     data1=42 data* $usbtest::MULTIPLIER \
                     data+ $usbtest::INCREMENT
                 if { [usbtest::start 10] } {
                     puts "Passed"
                 } else {
                     puts "Failed"
                     foreach result $usbtest::results {
                         puts $result
                     }
                 }
             }
         }
     }
 }

if { 0 != [llength $usbtest::bulk_out_endpoints] } {
    puts "Bulk OUT endpoints: $usbtest::bulk_out_endpoints"
    foreach ep $usbtest::bulk_out_endpoints {
	puts [format "  %2d: packet sizes %d to %d" $ep \
                  $usbtest::bulk_out($ep,min_size) \
                  $usbtest::bulk_out($ep,max_size)]
        foreach pktsize $pktsizes {
            if { $pktsize <=  $usbtest::bulk_out($ep,max_size) } {
                puts [format "Testing OUT endpoint %d with packet size %4d" \
                          $ep $pktsize]
                usbtest::reset
                usbtest::bulktest $ep out 5 txsize1=$pktsize format=byteseq \
                    data1=42 data* $usbtest::MULTIPLIER \
                    data+ $usbtest::INCREMENT rxsize1=4096 
                if { [usbtest::start 10] } {
                    puts "Passed"
                } else {
                    puts "Failed"
                    foreach result $usbtest::results {
                        puts $result
                    }
                }
            }
        }
    }
}
