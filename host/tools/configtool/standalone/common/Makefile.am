## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     ecostest
##
## =====================================================================
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
######COPYRIGHTEND####
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

## Pick up an appropriate header file search path from configure
INCLUDES = -I@cyg_ac_infra_incdir@ -I@cyg_ac_cdl_incdir@ -I@cyg_ac_tcl_incdir@ -I@win_srcdir@/../../common/common

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

