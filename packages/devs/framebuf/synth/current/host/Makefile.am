## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     Build support for the host-side synthetic target support,
##     the ethernetpackage.
##
## =====================================================================
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
## =====================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):	bartv
## Contact(s):	bartv
## Date:	2005/10/28
## Version:	0.01
##
######DESCRIPTIONEND####
## =====================================================================

AUTOMAKE_OPTIONS = 1.10 foreign

## The synthetic target support consists of a single program framebuf,
## a Tcl script, and some additional data files. These are
## all installed in a single directory $(libexec)/ecos/<package>_<version>/
## Neither the framebuf executable nor any of the scripts are directly
## executable, instead framebuf gets fork()'d/execve()'d by the Tcl
## script so $(libexec) is appropriate. Strictly speaking the
## Tcl scripts and data files are architecture-independent so should
## probably be installed in an analogous directory below $(datadir),
## but that would add more directories for little real gain. The scripts
## are treated as data files since they should not be executed directly,
## i.e. they should not be installed with the execute bit set.

AM_CFLAGS       = @ecos_CFLAGS@ -DECOSYNTH_VERSION=\"@VERSION@\" \
 -DECOS_REPOSITORY=\"@ECOS_REPOSITORY@\" \
 -DLIBEXECDIR=\"$(libexecdir)\" \
 -DPKG_DIR=\"@PACKAGE_DIR@\" \
 -DPKG_VERSION=\"@PACKAGE_VERSION@\" \
 -DPKG_INSTALL=\"@PACKAGE_INSTALL@\"
AM_CXXFLAGS     = @ecos_CXXFLAGS@
INCLUDES        = @ecos_INCLUDES@
LIBS            = @ecos_LIBS@ @ecos_LDADD@

if SUPPORTED
framebufdir 	= $(libexecdir)/ecos/@PACKAGE_INSTALL@
framebuf_PROGRAMS	= framebuf
framebuf_DATA		= framebuf.tcl framebuf.tdf framebuf_icon.xbm framebuf_iconmask.xbm
framebuf_SOURCES	= framebuf.c
framebuf_LDADD		= -L/usr/X11R6/lib -lX11

## Manual dependencies
framebuf.$(OBJEXT)	: Makefile ../src/protocol.h

endif
