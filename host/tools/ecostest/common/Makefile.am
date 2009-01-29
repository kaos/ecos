## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     ecostest
##
## =====================================================================
# ####ECOSHOSTGPLCOPYRIGHTBEGIN####                                         
# -------------------------------------------                               
# This file is part of the eCos host tools.                                 
# Copyright (C) 1998, 1999, 2000, 2001, 2002 Free Software Foundation, Inc. 
#
# This program is free software; you can redistribute it and/or modify      
# it under the terms of the GNU General Public License as published by      
# the Free Software Foundation; either version 2 or (at your option) any    
# later version.                                                            
#
# This program is distributed in the hope that it will be useful, but       
# WITHOUT ANY WARRANTY; without even the implied warranty of                
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU         
# General Public License for more details.                                  
#
# You should have received a copy of the GNU General Public License         
# along with this program; if not, write to the                             
# Free Software Foundation, Inc., 51 Franklin Street,                       
# Fifth Floor, Boston, MA  02110-1301, USA.                                 
# -------------------------------------------                               
# ####ECOSHOSTGPLCOPYRIGHTEND####                                           
## =====================================================================
#######DESCRIPTIONBEGIN####
##
## Author(s):	bartv,jskov
## Contact(s):	jskov
## Date:	2000/09/15
## Version:	0.01
##
######DESCRIPTIONEND####
## =====================================================================

AUTOMAKE_OPTIONS = 1.3 cygnus
SUBDIRS          =

## Pick up an appropriate header file search path from configure
INCLUDES = -I@cyg_ac_infra_incdir@ -I@cyg_ac_cdl_incdir@ -I@cyg_ac_utils_incdir@ -I@cyg_ac_tcl_incdir@

LIBS     = -L@cyg_ac_infra_libdir@ -L@cyg_ac_cdl_libdir@ \
           -L@cyg_ac_utils_libdir@ -L@cyg_ac_tcl_libdir@ \
           $(foreach x,@cyg_ac_cdl_libraries@,-l$x) \
           $(foreach x,@cyg_ac_infra_libraries@,-l$x) \
           $(foreach x,@cyg_ac_utils_libraries@,-l$x) \
           @cyg_ac_tcl_libs@

bin_PROGRAMS = ser_filter permtest

ser_filter_SOURCES = ser_filter.cpp eCosTestMonitorFilter.cpp \
                     eCosTestSerialFilter.cpp eCosTestUtils.cpp \
                     eCosTestDownloadFilter.cpp TestResource.cpp \
                     eCosTestPlatform.cpp eCosTest.cpp \
                     ResetAttributes.cpp

ser_filter_DEPENDENCIES = @cyg_ac_utils_libdir@/libutils.a
ser_filter_LDADD = -lpthread

permtest_SOURCES = permtest.cxx
permtest_DEPENDENCIES = @cyg_ac_cdl_libdir@/libcdl.a @cyg_ac_infra_libdir@/libcyginfra.a
