#!/bin/sh
# the next line restarts using tclsh \
 exec tclsh "$0" ${1+"$@"}

#===============================================================================
#
#    fixhtml.tcl
#
#    Patch HTML files generated from DocBook sources.
#
#===============================================================================
#####COPYRIGHTBEGIN####
#                                                                 
# =============================================================== 
# Copyright (C) 2000, 2001 Red Hat, Inc.
#
# This file is part of the eCos host tools.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of
# the License, or (at your option) any later version.
#                                                                 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the  
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public
# License along with this program; if not, write to the Free
# Software Foundation, Inc.,  59 Temple Place - Suite 330,
# Boston, MA  02111-1307, USA. 
#
# =============================================================== 
#
#####COPYRIGHTEND####
#===============================================================================
######DESCRIPTIONBEGIN####
#
# Author(s):	bartv
# Contributors:	bartv
# Date:		2000-03-14
# Purpose:      HTML files generated from DocBook sources using the nwalsh
#               stylesheets have a number of problems. Most importantly,
#               Netscape 4.x does not understand all of the character entities
#               defined by HTML 4.0x
#
#####DESCRIPTIONEND####
#===============================================================================
#

# Find out the current year for the copyright message. Ideally
# this would be a range extracted from the sources, but that is
# a little bit tricky.

set year [clock format [clock seconds] -format "%Y"]

set copyright_banner \
"<!-- Copyright (C) $year Red Hat, Inc.                                -->
<!-- This material may be distributed only subject to the terms      -->
<!-- and conditions set forth in the Open Publication License, v1.0  -->
<!-- or later (the latest version is presently available at          -->
<!-- http://www.opencontent.org/openpub/).                           -->
<!-- Distribution of substantively modified versions of this         -->
<!-- document is prohibited without the explicit permission of the   -->
<!-- copyright holder.                                               -->
<!-- Distribution of the work or derivative of the work in any       -->
<!-- standard (paper) book form is prohibited unless prior           -->
<!-- permission is obtained from the copyright holder.               -->"

set files [glob *.html]
foreach file $files {
    set status [catch {
	
	set fd [open $file "r"]
	set data [read $fd]
	close $fd

	# If there is already a (C) message on the first line, skip this file.
	if {[regexp {[^\n]*Copyright (C) [0-9]* Red Hat.*} $data] == 0} {

	    # The DSSSL has the annoying habit of splitting tags over several lines.
	    # This should sort things out.
            # REMOVED by jifl: doing this can add newlines in tags like
            # <literallayout>, <screen>, and/or <programlisting>
	    # regsub -all "\n>" $data ">\n" data
	    
	    # Add a copyright banner
	    set data "[set copyright_banner]\n[set data]"

	    # Look for a smarttags meta. If absent, insert one. There should
	    # already be one meta present identifying the stylesheet, so
	    # that identifies a sensible location for inserting another meta.
	    if {[regexp {MSSmartTagsPreventParsing} $data] == 0} {
		regsub -nocase {<META} $data "<meta name=\"MSSmartTagsPreventParsing\" content=\"TRUE\">\n<META" data
	    }
	    
	    # Take care of some character entities that Netscape does not understand
	    regsub -all "&mgr;"    $data "\\&#03BC;" data
	    regsub -all "&mdash;"  $data "\\&#8212;" data
            regsub -all "&ndash;"  $data "\\&#8211;" data
	    regsub -all "&hellip;" $data "\\&#8230;" data
	    regsub -all "&ldquo;"  $data "\\&#8220;" data
	    regsub -all "&rdquo;"  $data "\\&#8221;" data
	    regsub -all "&lsqb;"   $data "\\&#0091;" data
	    regsub -all "&rsqb;"   $data "\\&#0093;" data
	    regsub -all "&lcub;"   $data "\\&#0123;" data
	    regsub -all "&rcub;"   $data "\\&#0125;" data
	    regsub -all "&lsquo;"  $data "\\&#8216;" data
	    regsub -all "&rsquo;"  $data "\\&#8217;" data
	    regsub -all "&trade;"  $data "\\&#8482;" data
	    regsub -all "&ast;"    $data "\\&#0042;" data
	    regsub -all "&lowbar;" $data "\\&#0095;" data
	    regsub -all "&sol;"    $data "\\&#0047;" data
	    regsub -all "&equals;" $data "\\&#0061;" data
            regsub -all "&num;"    $data "\\&#0035;" data
            regsub -all "&plus;"   $data "\\&#0043;" data

	    # Now write the data back to the file. Do not bother to
	    # keep an old version lying around, the html files can be
	    # regenerated easily enough.
	    set fd [open $file "w"]
	    puts -nonewline $fd $data
	    close $fd
	}
    } result]

    if {0 != $status} {
	puts "Error while processing file $file\n    $result"
	exit 1
    }
}

