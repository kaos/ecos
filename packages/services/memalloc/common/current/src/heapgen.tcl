#!/bin/sh
# these lines restart using the tcl shell \
  exec sh -c "if ( echo | tclsh ) 2>/dev/null ; then \
      exec tclsh \"${0}\" ${1+${*}} ; \
    elif ( echo | cygtclsh80 ) 2>/dev/null ; then \
      exec cygtclsh80 \"${0}\" `echo "\`cygpath -w ${1} 2>/dev/null\`" |sed 's#\\\\#/#g'` `echo "\`cygpath -w ${2} 2>/dev/null\`" |sed 's#\\\\#/#g'` ; \
    else \
      echo Could not find TCL interpreter ; \
      exit 1 ; \
    fi"

#===============================================================================
#
#    heapgen.tcl
#
#    Script to generate memory pool instantiations based on the memory map
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
# Author(s):	jlarmour
# Contributors:	
# Date:		2000-06-13
# Purpose:      Generate memory pool instantiations based on the memory map
#               along with information in a header file to allow access from
#               C source
# Description:
# Usage:
#
#####DESCRIPTIONEND####
#===============================================================================

set debug 0

if { $argc != 2 } {
    error "Usage: heapgen.tcl installdir builddir"
}

proc dputs { args } {
    global debug
    if { $debug > 0 } {
        puts -nonewline "DEBUG: "
        foreach i $args {
          puts -nonewline $i
        }
        puts ""
    }
}

set installdir [ lindex $argv 0 ]
set builddir   [ lindex $argv 1 ]

# Fetch relevant config data placed in the generated file heapgeninc.tcl
source [ file join $builddir heapgeninc.tcl ]

dputs "builddir=" $builddir
dputs "installdir=" $installdir
dputs "pwd=" [pwd]
dputs "memlayout_h=" $memlayout_h

# ----------------------------------------------------------------------------
# Get heap information

# trim brackets
set ldi_name [ string trim $memlayout_ldi "<>" ]
dputs $ldi_name 
# prefix full leading path including installdir
set ldifile [open [ file join $installdir include $ldi_name ] r]

# now read the .ldi file and find the user-defined sections with the
# prefix "heap"
set heaps ""
while { [gets $ldifile line] >= 0} {
    # Search for user-defined name beginning heap (possibly with leading
    # underscores
    if [ regexp {^[ \t]+(CYG_LABEL_DEFN\(|)[ \t]*_*heap} $line ] {
        set heapnamestart [ string first heap $line ]
        set heapnameend1 [ string first ")" $line ]
        incr heapnameend1 -1
        set heapnameend2 [ string wordend $line $heapnamestart ]
        if { $heapnameend1 < 0 } {
            set $heapnameend1 $heapnameend2
        }
        set heapnameend [ expr $heapnameend1 < $heapnameend2 ? $heapnameend1 : $heapnameend2 ]
        set heapname [ string range $line $heapnamestart $heapnameend ]
        set heaps [ concat $heaps $heapname ]
        dputs [ format "Found heap \"%s\"" $heapname ]
    }
}
close $ldifile

set heapcount [ llength $heaps ]
set heapcount1 [ expr 1 + $heapcount ]

# ----------------------------------------------------------------------------
# Generate header file

# Could have made it generate the header file straight into include/pkgconf,
# but that knowledge of the build system is best left in the make rules in CDL

set hfile [ open [ file join $builddir heaps.hxx ] w]
puts $hfile "#ifndef CYGONCE_PKGCONF_HEAPS_HXX"
puts $hfile "#define CYGONCE_PKGCONF_HEAPS_HXX"
puts $hfile "/* <pkgconf/heaps.hxx> */\n"
puts $hfile "/* This is a generated file - do not edit! */\n"
# Allow CYGMEM_HEAP_COUNT to be available to the implementation header file
puts $hfile [ format "#define CYGMEM_HEAP_COUNT %d" $heapcount ]
puts $hfile [ concat "#include " $malloc_impl_h ]
puts $hfile ""
puts $hfile [ format "extern %s *cygmem_memalloc_heaps\[ %d \];" \
        $malloc_impl_class $heapcount1 ]
puts $hfile "\n#endif"
puts $hfile "/* EOF <pkgconf/heaps.hxx> */"
close $hfile

# ----------------------------------------------------------------------------
# Generate C file in the current directory (ie. the build directory)
# that instantiates the pools

set cfile [ open [ file join $builddir heaps.cxx ] w ]
puts $cfile "/* heaps.cxx */\n"
puts $cfile "/* This is a generated file - do not edit! */\n"
puts $cfile "#include <pkgconf/heaps.hxx>"
puts $cfile [ concat "#include " $memlayout_h ]
puts $cfile "#include <cyg/infra/cyg_type.h>"
puts $cfile "#include <cyg/hal/hal_intr.h>"
puts $cfile [ concat "#include " $malloc_impl_h ]
puts $cfile ""

foreach heap $heaps {
    puts $cfile "#ifdef HAL_MEM_REAL_REGION_TOP\n"

    puts $cfile [ format "%s cygmem_pool_%s ( (cyg_uint8 *)CYGMEM_SECTION_%s ," \
            $malloc_impl_class $heap $heap ]
    puts $cfile [ format "    HAL_MEM_REAL_REGION_TOP( (cyg_uint8 *)CYGMEM_SECTION_%s + CYGMEM_SECTION_%s_SIZE ) - (cyg_uint8 *)CYGMEM_SECTION_%s ) " \
            $heap $heap $heap ]
    puts $cfile "        CYGBLD_ATTRIB_INIT_BEFORE(CYG_INIT_LIBC);\n"

    puts $cfile "#else\n"

    puts $cfile [ format "%s cygmem_pool_%s ( (cyg_uint8 *)CYGMEM_SECTION_%s , CYGMEM_SECTION_%s_SIZE ) CYGBLD_ATTRIB_INIT_BEFORE(CYG_INIT_LIBC);\n" \
            $malloc_impl_class $heap $heap $heap ]

    puts $cfile "#endif"
}

puts $cfile ""
puts $cfile [ format "%s *cygmem_memalloc_heaps\[ %d \] = { " \
        $malloc_impl_class $heapcount1 ]
foreach heap $heaps {
    puts $cfile [ format "    &cygmem_pool_%s," $heap ]
}
puts $cfile "    NULL\n};"

puts $cfile "\n/* EOF heaps.cxx */"
close $cfile

# ----------------------------------------------------------------------------
# EOF heapgen.tcl
