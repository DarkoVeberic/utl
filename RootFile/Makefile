# GPLv2 and 2C-BSD
# Copyright (c) Darko Veberic, 2014

ifndef ROOTSYS
  # try system default
  ROOTSYS := /usr
endif

CPPFLAGS := -I. $(shell $(ROOTSYS)/bin/root-config --cflags)
#CXXFLAGS := -Wall -Wextra -ggdb3 -O0 -fno-inline -pipe
CXXFLAGS := -Wall -Wextra -ggdb3 -O2 -fPIC -pipe
LDFLAGS := $(CXXFLAGS) $(shell $(ROOTSYS)/bin/root-config --ldflags --libs)

SHELL := bash
SUFFIXES := .o .cc .cxx

VPATH := . io

EXES := $(basename $(wildcard *.cxx))
OBJS := $(foreach d, $(VPATH), $(patsubst %.cc, %.o, $(wildcard $(d)/*.cc)))

LINKDEFS := $(patsubst %.LinkDef.h, %, $(wildcard *.LinkDef.h))
OBJS += $(addsuffix .Dict.o, $(LINKDEFS))
.PRECIOUS: $(addsuffix .Dict.h, $(LINKDEFS)) $(addsuffix .Dict.cc, $(LINKDEFS))

DEPS := $(patsubst %.o, %.P, $(OBJS)) $(addsuffix .P, $(EXES))

define cxx_compile_with_dependency_creation
  $(COMPILE.cc) -MD -o $@ $<
  @sed -e 's|.*:|$*.o:|' <$*.d >$*.P
  @sed -e 's/.*://' -e 's/\\$$//' <$*.d | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >>$*.P
  @rm -f $*.d
endef

define cxx_link_rule
  $(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@
endef

%.o: %.cc
	$(call cxx_compile_with_dependency_creation)

%.o: %.cxx
	$(call cxx_compile_with_dependency_creation)

%: %.o
	$(call cxx_link_rule)

%.Dict.cc %.Dict.h: %.h %.LinkDef.h
	$(ROOTSYS)/bin/rootcint -v4 -f $@ -c -I. $^

.PHONY: all
all: $(EXES)

test_rootfile: test_rootfile.o $(OBJS)

.PHONY: check
check: $(basename $(wildcard test_*.cxx))
	for t in $^ ; do echo check: run $$t ; ./$$t || exit $$? ; done

.PHONY: clean
clean:
	- $(RM) $(OBJS) *.Dict.{h,cc} $(addsuffix .o, $(EXES)) $(EXES) $(DEPS) TestFile-*.root

-include $(DEPS)
