#=============================================================================
#
#    makrules.tst
#
#    This file contains generic rules that can be applied to all tests
#    directories.
#
#=============================================================================
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
#=============================================================================

.PHONY: default build clean

# Source files can be picked up from the build tree or the component
# repository. This allows copies of the sources to be made in the
# build tree and edited there.
VPATH        := . $(COMPONENT_REPOSITORY)/$($(PACKAGE)_DIR)/tests

# The object files and the dependency files can be deduced mainly
# from the list of tests.
STAMPS       := $(foreach target,$(TESTS),$(target).stamp)
OBJECTS      := $(STAMPS:.stamp=.o)
OBJECTS	     := $(foreach obj,$(OBJECTS),$(dir $(obj))$(PACKAGE)_$(notdir $(obj)))
DEPS	     := $(OBJECTS:.o=.d) $(OTHER_DEPS)	     

# The header file search path is as follows:
#
#   $(PREFIX)/include
#     These headers are what will be used by application code.
#
#   .
#     Header files private to a package and copied into the build tree.
#
#   $(COMPONENT_REPOSITORY)/<package>/src
#     Header files private to a package and still in the repository.
#
# It is possible for additional paths to be specified by the user,
# typically by editing the makevars file. Any such search paths take
# precedence over the default path.

INCLUDE_PATH := $(INCLUDE_PATH) -I$(PREFIX)/include $(foreach dir,$(VPATH),-I$(dir))

# These rules cause mostof the work to be done.
build: $(STAMPS) $(OTHER_TESTS)
ifneq ($(strip $(DEPS)),)
	@$(CAT) $(DEPS) > makefile.deps
endif

clean: $(foreach test,$(TESTS),$(test).clean) $(OTHER_CLEAN)
	@$(RM) makefile.deps
ifneq ($(strip $(DEPS)),)
	@$(RM) $(DEPS)
endif

# These rules generate a test executable from a single .o file,
# and perform a matching clean-up operation. It is also necessary
# to have dependencies on libtarget.a et al.

ifneq ($(STAMPS),)

LIBDEPS := $(wildcard $(PREFIX)/lib/*)

%.stamp: $(PACKAGE)_%.o $(LIBDEPS)
ifneq ($(IGNORE_LINK_ERRORS),)
	-$(CC) -o $(PREFIX)/tests/$(PACKAGE)/$*$(EXEEXT) $< $(LDFLAGS) -L$(PREFIX)/lib -Ttarget.ld -nostdlib
else
	$(CC) -o $(PREFIX)/tests/$(PACKAGE)/$*$(EXEEXT) $< $(LDFLAGS) -L$(PREFIX)/lib -Ttarget.ld -nostdlib
endif
	@$(TOUCH) $@

%.clean:
	$(RM) $(PREFIX)/tests/$(PACKAGE)/$*$(EXEEXT) $(dir $*.o)$(PACKAGE)_$(notdir $*.o)
	@$(RM) $*.stamp
endif

# These file dependencies ensure that all files are rebuilt if
# there is a change to the build environment, e.g. if a different
# compiler is being used.
#
ifneq ($(strip $(OBJECTS)),)
$(OBJECTS) : $(BUILD_TREE)/pkgconf/pkgconf.mak
$(OBJECTS) : $(BUILD_TREE)/pkgconf/makevars $(PACKAGE_RULES_FILE)
endif

#
# This rule reads in any previously generated dependency information.
#
ifneq ($(wildcard makefile.deps),)
include makefile.deps
endif
