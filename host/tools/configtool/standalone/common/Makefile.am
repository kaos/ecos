## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     ecostest
##
## =====================================================================
#####ECOSHOSTGPLCOPYRIGHTBEGIN####
# -------------------------------------------
# Copyright (C) 1998, 1999, 2000, 2001, 2002 Red Hat, Inc.
#
# This file is part of the eCos host tools.
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the Free
# Software Foundation; either version 2 of the License, or (at your option)
# any later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License along with
# this program; if not, write to the Free Software Foundation, Inc.,
# 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#
# -------------------------------------------
#####ECOSHOSTGPLCOPYRIGHTEND####
## =====================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):	bartv
## Contact(s):	bartv
## Date:	1999/11/09
## Version:	0.01
##
######DESCRIPTIONEND####
## =====================================================================

AUTOMAKE_OPTIONS = 1.3 cygnus
SUBDIRS          =

## Pick up compiler flags etc. from configury
INCLUDES        = -I@cyg_ac_infra_incdir@ -I@cyg_ac_cdl_incdir@ -I@cyg_ac_tcl_incdir@ -I@win_srcdir@/../../common/common
AM_CFLAGS	= @cyg_CFLAGS@
AM_CXXFLAGS	= @cyg_CXXFLAGS@

if MSVC
LIBS    = @cyg_ac_cdl_libraries@ @cyg_ac_infra_libraries@ @cyg_ac_tcl_libs@ \
          -link -libpath=@cyg_ac_infra_libdir@ -libpath=@cyg_ac_cdl_libdir@ -libpath=@cyg_ac_tcl_libdir@
else
LIBS     = -L@cyg_ac_infra_libdir@ -L@cyg_ac_cdl_libdir@ -L@cyg_ac_tcl_libdir@ \
           $(foreach x,@cyg_ac_cdl_libraries@,-l$x) \
           $(foreach x,@cyg_ac_infra_libraries@,-l$x) \
           @cyg_ac_tcl_libs@
endif

BUILT_SOURCES    = build.cxx flags.cxx
bin_PROGRAMS     = ecosconfig
ecosconfig_SOURCES = ecosconfig.cxx cdl_exec.cxx build.cxx flags.cxx
if MSVC
ecosconfig_DEPENDENCIES = @cyg_ac_cdl_libdir@/cdl.lib @cyg_ac_infra_libdir@/cyginfra.lib
else
ecosconfig_DEPENDENCIES = @cyg_ac_cdl_libdir@/libcdl.a @cyg_ac_infra_libdir@/libcyginfra.a
endif


noinst_HEADERS = cdl_exec.hxx ecosconfig.hxx

build.cxx: ../../common/common/build.cxx
	$(INSTALL) $< $@

flags.cxx: ../../common/common/flags.cxx
	$(INSTALL) $< $@

