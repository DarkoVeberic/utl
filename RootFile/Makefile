# GPLv2 and 2C-BSD
# Copyright (c) Darko Veberic, 2014

CPPFLAGS := -I. $(shell $(ROOTSYS)/bin/root-config --cflags)
CXXFLAGS := -Wall -Wextra -ggdb3 -O0 -fno-inline -pipe
#CXXFLAGS := -Wall -Wextra -ggdb3 -O2 -pipe
LDFLAGS := $(CXXFLAGS) $(shell $(ROOTSYS)/bin/root-config --ldflags --libs) -lMathMore -lMinuit

SUFFIXES := .o .cc .cpp

EXES := $(basename $(wildcard *.cpp))
OBJS := $(patsubst %.cc, %.o, $(wildcard *.cc)) $(patsubst %.LinkDef.h, %.Dict.o, $(wildcard *.LinkDef.h))
DEPS := $(patsubst %.o, %.P, $(OBJS)) $(addsuffix %.P, $(EXES))

define cxx_compile_with_dependency_creation
  $(COMPILE.cc) -MD -o $@ $<
  @sed -e 's|.*:|$*.o:|' <$*.d >$*.P
  @sed -e 's/.*://' -e 's/\\$$//' <$*.d | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >>$*.P
  @rm -f $*.d
endef

define cxx_link_rule
  $(CXX) $^ $(LDFLAGS) -o $@
endef

%.o: %.cc
	$(call cxx_compile_with_dependency_creation)

%.o: %.cpp
	$(call cxx_compile_with_dependency_creation)

%: %.o
	$(call cxx_link_rule)

%.Dict.cc %.Dict.h: %.h %.LinkDef.h
	$(ROOTSYS)/bin/rootcint -v4 -f $@ -c -I. $^

.PHONY: all check

all: $(EXES)

test_rootfile: test_rootfile.o $(OBJS)

check: test_rootfile
	./test_rootfile

clean:
	- $(RM) $(OBJS) *.Dict.cc *.Dict.h $(addsuffix .o, $(EXES)) $(EXES) *.P

-include $(DEPS)
