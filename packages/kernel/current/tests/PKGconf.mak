#===============================================================================
#
#    makefile
#
#    kernel/tests
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
# by Cygnus are Copyright (C) 1998,1999,2000 Cygnus Solutions.
# All Rights Reserved.
# -------------------------------------------
#
#####COPYRIGHTEND####
#===============================================================================

PACKAGE       := kernel
include ../../../pkgconf/pkgconf.mak
include ../../../pkgconf/system.mak

TESTS	      := bin_sem0 bin_sem1 bin_sem2 clock0 clock1 cnt_sem0 cnt_sem1 \
		 except1 flag0 flag1 intr0 \
	         kclock0 kclock1 kexcept1 kintr0 kmbox1 kmemfix1 kmemvar1 \
		 kmutex0 kmutex1 ksched1 ksem0 ksem1 kflag0 kflag1 \
		 kthread0 kthread1 mbox1 memfix1 memfix2 memvar1 memvar2\
		 mutex0 mutex1 mutex2 mutex3 sched1 sync2 sync3 \
		 thread0 thread1 thread2 release kill thread_gdb \
		 tm_basic dhrystone

ifndef CYG_HAL_ARM
TESTS	+= kcache1 kcache2
endif


ifdef CYGPKG_HAL_ARM_EDB7XXX
TESTS	+= kcache1 kcache2
endif

ifndef CYGPKG_HAL_ARM_AEB
TESTS   += stress_threads
endif

include $(COMPONENT_REPOSITORY)/pkgconf/makrules.tst
