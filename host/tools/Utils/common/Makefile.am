## Process this file with automake to produce Makefile.in
## =====================================================================
##
##     Makefile.am
##
##     utils
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
## Author(s):	jskov
## Contact(s):	jskov
## Date:	2000/09/15
## Version:	0.01
##
######DESCRIPTIONEND####
## =====================================================================

AUTOMAKE_OPTIONS = 1.3 cygnus
SUBDIRS          =

## Pick up an appropriate header file search path from configure

noinst_LIBRARIES = libutils.a

libutils_a_SOURCES = Collections.cpp Subprocess.cpp eCosSocket.cpp \
                     eCosThreadUtils.cpp Properties.cpp eCosSerial.cpp \
                     eCosStd.cpp eCosTrace.cpp

