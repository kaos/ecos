common_sources = \
	../../Utils/common/Properties.cpp       \
	../../Utils/common/Collections.cpp      \
	../../Utils/common/eCosStd.cpp          \
	../../Utils/common/eCosThreadUtils.cpp  \
	../../Utils/common/eCosTrace.cpp        \
	../common/eCosTestUtils.cpp							\
	../common/eCosTestSocket.cpp						\
	../common/TestResource.cpp							\
	../common/ResetAttributes.cpp						\
	../common/eCosTest.cpp									\
	../common/eCosTestSerial.cpp						\
	../common/eCosTestDownloadFilter.cpp		\
	../common/eCosTestMonitorFilter.cpp		\
	../common/eCosTestSerialFilter.cpp

common_objects = $(notdir $(common_sources:.cpp=.o))

all_sources = $(common_sources) ../common/ser_filter.cpp

programs = ser_filter


CXX=g++

ifeq ($(shell uname), SunOS)
# SunOS
LIBS :=  -lpthread -lsocket -lxnet -lstdc++
else
# Linux
LIBS :=  -lpthread -efence -lstdc++
endif

# Look in these directories for source/make files
VPATH = .:../common:../../Utils/common

# Flags
CPPFLAGS=-I../../Utils/common -I../common
CXXFLAGS=-g -O2 -Wall -D_REENTRANT -D_DEBUG $(CPPFLAGS)

%.d: %.cpp
	$(SHELL) -ec '$(CC) -MM $(CPPFLAGS) $< \
               | sed '\''s#\($*\)\.o[ :]*#\1.o $@ : #g'\'' > $@; \
               [ -s $@ ] || rm -f $@'

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) -o $@ $<

.PHONY: all
all: $(programs)

ser_filter: $(common_objects) ser_filter.o
	$(CXX) -o $@  $(CXXFLAGS) $(LIBPATH) $(LIBS) $^


.PHONY: clean
clean:
	rm -f *.o *.d
	rm -f $(programs)

# include the dependency files
include $(notdir $(all_sources:.cpp=.d))
