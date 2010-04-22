MAIN=$(shell basename `pwd`)

ARCH         := $(shell root-config --arch)

CXX           =
DllSuf        = so

ROOTCFLAGS   := $(shell root-config --cflags)
ROOTLIBS     := $(shell root-config --libs)
ROOTGLIBS    := $(shell root-config --glibs)

###########################################################################
## ##################################################################### ##
## ## We have (more or less) taken these flags out of                 ## ##
## ## $ROOTSYS/test/Makefile.arch.  If your architechture/compiler    ## ##
## ## is not listed, try grabbing the necessary flags from that file. ## ##
## ##################################################################### ##
###########################################################################

# Cygwin
ifeq ($(ARCH),win32gcc)
# Windows with gcc
DllSuf        = dll
CXX           = g++
CXXFLAGS      = -g -O2 -Wall -Woverloaded-virtual -I/usr/X11R6/include 
LD            = g++
LDFLAGS       = -g --no_exceptions -shared
SOFLAGS       = -shared -D_DLL -Wl,--export-all-symbols
EXPLLINKLIBS  = $(ROOTLIBS) $(ROOTGLIBS)
endif

# gcc compiler (CDF Gen 6)
ifeq ($(ARCH),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
FXX           = g77
CXXFLAGS      = -g -O3 -Wall -fPIC
FXXFLAGS      = -O3 -c -x f77-cpp-input
LD            = g++
LDFLAGS       = -g -O2 -shared
SOFLAGS       = -shared
endif

# gcc compiler (CDF Gen 6 64-bit)
ifeq ($(ARCH),linuxx8664gcc)
# Linux with egcs, gcc 2.9x, gcc 3.x (>= RedHat 5.2)
CXX           = g++
FXX           = g77
CXXFLAGS      = -g -O3 -Wall -fPIC
FXXFLAGS      = -O3 -c -fPIC
LD            = g++
LDFLAGS       = -g -O3 --no_exceptions -shared
SOFLAGS       = -shared
endif

# Kai compiler (CDF Gen 5)
ifeq ($(ARCH),linuxkcc)
# Linux with the KAI compiler
CXX           = KCC --one_instantiation_per_object
CXXFLAGS      = -O -fPIC +K0
LD            = KCC
LDFLAGS       = -O $(shell root-config --cflags) --no_exceptions -shared
SOFLAGS       =
endif

# MacOS X 10.4.5 (gcc 4.0)
ifeq ($(ARCH),macosx)
ifndef MACOSX_DEPLOYMENT_TARGET
$(error You must define MACOSX_DEPLOYMENT_TARGET first, e.g. "export MACOSX_DEPLOYMENT_TARGET=10.4")
endif
MACOSX_DEPLOYMENT_TARGET = 10.4
DllSuf            = dylib
CXX               = g++
CXXFLAGS          = -g -pipe -W -Wall -Woverloaded-virtual -fsigned-char -fno-common -D__REGEXP -DG__UNIX -DG__SHAREDLIB -DG__ROOT -DG__REDIRECTIO -DG__OSFDLL
LD                = g++
LDFLAGS           = -g -dynamiclib -single_module -undefined dynamic_lookup -install_name ../shlib/lib$(MAIN).$(DllSuf)
LDFLAGSROOT       = -Xlinker -bind_at_load -flat_namespace -bundle
SHAREDROOT        = ../shlib/lib$(MAIN).so
EXPLLINKLIBS      = $(ROOTGLIBS)
endif

ifeq ($(CXX),)
$(error $(ARCH) invalid architecture)
endif


SHARED=../shlib/lib$(MAIN).$(DllSuf)

INCFLAGS =-Iinclude -I../include -I$(MAIN) -I../$(MAIN)
CXXFLAGS += $(shell root-config --cflags) $(INCFLAGS) 

#

SRCS =  $(wildcard src/*.cc) 
FSRCS = $(wildcard fsrc/*.f)

INCS = 	$(wildcard $(MAIN)/*.hh)

DEPS =  $(patsubst src/%, objects/%, \
		  $(patsubst %.cc, %.d, $(wildcard src/*.cc)) )
FDEPS = $(patsubst fsrc/%, objects/%, \
		  $(patsubst %.f, %.d, $(wildcard fsrc/*.f)) )

# We want dictionaries only for classes that have _linkdef.h files
DICTOBS =  $(patsubst %_linkdef.h, %.o, \
		      $(patsubst dict/%, objects/dict_%, \
			  $(wildcard dict/*_linkdef.h) ) )

OBJS =   $(patsubst src/%, objects/%, \
		    $(patsubst %.cc, %.o, $(SRCS)) )
FOBJS =  $(patsubst fsrc/%, objects/%, \
		    $(patsubst %.f, %.o, $(FSRCS)) )

OBJS += $(DICTOBS)

# MacOS needs different builds for the shared library to be linked (*.dylib) 
# and the shared library to be loaded into ROOT (*.so)
ifeq ($(ARCH),macosx)
shared:	$(SHARED) $(SHAREDROOT)

$(SHARED): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(SHARED)

$(SHARED): $(FOBJS)
	$(LD) $(LDFLAGS) $(FOBJS) -o $(SHARED)

$(SHAREDROOT): $(OBJS)
	$(LD) $(LDFLAGSROOT) $(OBJS) $(EXPLLINKLIBS) -o $(SHAREDROOT)

$(SHAREDROOT): $(FOBJS)
	$(LD) $(LDFLAGSROOT) $(FOBJS) $(EXPLLINKLIBS) -o $(SHAREDROOT)

else
shared:	$(SHARED)
$(SHARED): $(OBJS) $(FOBJS)
	@echo "Creating library $(SHARED)"
	@$(LD) $(LDFLAGS) $(OBJS) $(FOBJS) $(EXPLLINKLIBS) -o $(SHARED) 
	@echo "$(SHARED) successfully compiled!"

#$(SHARED): $(FOBJS)
#	$(LD) $(LDFLAGS) $(FOBJS) $(EXPLLINKLIBS) -o $(SHARED) 
endif

echo:
	@echo For Debugging:
	@echo .
	@echo SRCS: $(SRCS)
	@echo .
	@echo INCLUDES: $(INCS)
	@echo .
	@echo OBJECTS: $(OBJS)
	@echo .
	@echo DEPS: $(DEPS)
	@echo .
	@echo DICTOBS: $(DICTOBS)

objects/dict_%.o: $(MAIN)/%.hh dict/%_linkdef.h
	@echo "Generating dictionary for $<"
	@$(ROOTSYS)/bin/rootcint -f $(patsubst %.o, %.C, $@) -c -Idict $(INCFLAGS) $(notdir $^) 
	@$(CXX) -c $(CXXFLAGS) -o $@ $(patsubst %.o, %.C, $@)

objects/%.o: src/%.cc $(MAIN)/%.hh
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c -o $@ $<

objects/%.o: fsrc/%.f
	@echo "Compiling $<"
	@$(FXX) $(FXXFLAGS) -c -o $@ $<

objects/%.d: src/%.cc
	@echo "Generating dependencies for $<"
	@set -e; $(CXX) -M $(CXXFLAGS) $< \
	| sed 's%\($*\)\.o[ :]*%\1.o $@ : %g' > $@; \
	[ -s $@ ] || rm -f $@

objects/%.d: fsrc/%.f
	@echo "Generating dependencies for $<"
	@set -e; $(FXX) -M $(FXXFLAGS) $< \
	| sed 's%\($*\)\.o[ :]*%\1.o $@ : %g' > $@; \
	[ -s $@ ] || rm -f $@

clean:
	@echo "Cleaning everything..."
	@rm -f objects/*.d objects/*.o core* objects/dict_* $(SHARED)

tar:
	@echo "Making Tar file..."
	@tar czvf $(MAIN).tgz -X files/exclude.txt *

scp:
	@scp $(MAIN).tgz fcdflnx4.fnal.gov:backups/

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif
