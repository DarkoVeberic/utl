CPPFLAGS := -I. $(shell $(ROOTSYS)/bin/root-config --cflags)
#CXXFLAGS := -Wall -Wextra -ggdb3 -O0 -fno-inline -pipe
CXXFLAGS := -Wall -Wextra -ggdb3 -O2 -pipe
#CXXFLAGS := -Wall -Wextra -ggdb3 -march=native -Ofast -pipe
LDFLAGS := $(CXXFLAGS) $(shell $(ROOTSYS)/bin/root-config --ldflags --libs) -lMinuit2

SUFFIXES := .o .cc .cxx

EXES := $(basename $(wildcard *.cxx))
LINKDEFS := $(patsubst %.LinkDef.h, %, $(wildcard *.LinkDef.h))
OBJS := $(patsubst %.cc, %.o, $(wildcard *.cc)) $(addsuffix .Dict.o, $(LINKDEFS))
DEPS := $(patsubst %.o, %.P, $(OBJS)) $(addsuffix .P, $(EXES))

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

%.o: %.cxx
	$(call cxx_compile_with_dependency_creation)

%: %.o
	$(call cxx_link_rule)

%.Dict.cc %.Dict.h: %.h %.LinkDef.h
	$(ROOTSYS)/bin/rootcint -v4 -f $@ -c -I. $^

.PHONY: all
all: $(EXES)

test_random_gamma: test_random_gamma.o Util.o

.PHONY: check
check: $(basename $(wildcard test_*.cxx))
	for t in $^ ; do echo $$t ; ./$$t || exit $$? ; done

.PHONY: clean
clean:
	- $(RM) $(OBJS) *.Dict.cc *.Dict.h $(addsuffix .o, $(EXES)) $(EXES) *.P

-include $(DEPS)
