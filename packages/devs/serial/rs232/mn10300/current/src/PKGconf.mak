#===============================================================================
#
#    makefile
#
#    devs/serial/rs232/mn10300/src
#
#===============================================================================
#####COPYRIGHTBEGIN####
#
# -------------------------------------------
# The contents of this file are subject to the Cygnus eCos Public License
# Version 1.0 (the "License"); you may not use this file except in
# compliance with the License.  You may obtain a copy of the License at
# http://sourceware.cygnus.com/ecos
# 
# Software distributed under the License is distributed on an "AS IS"
# basis, WITHOUT WARRANTY OF ANY KIND, either express or implied.  See the
# License for the specific language governing rights and limitations under
# the License.
# 
# The Original Code is eCos - Embedded Cygnus Operating System, released
# September 30, 1998.
# 
# The Initial Developer of the Original Code is Cygnus.  Portions created
# by Cygnus are Copyright (C) 1998 Cygnus Solutions.  All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
#===============================================================================

PACKAGE       := devices_serial_rs232_mn10300
include ../../../../../../pkgconf/pkgconf.mak

LIBRARY       := libtarget.a
COMPILE       := serial_mn10300_2.cxx serial_mn10300_1.cxx
OTHER_OBJS    :=
OTHER_TARGETS :=
OTHER_CLEAN   :=

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.src

