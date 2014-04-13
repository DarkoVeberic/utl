CPPFLAGS := -I.
#CXXFLAGS := -Wall -Wextra -ggdb3 -O9 -march=native -mfpmath=sse -msse3 -ftree-vectorize -pipe -DNDEBUG
#CXXFLAGS := -Wall -Wextra -ggdb3 -O0 -fno-inline -pipe
CXXFLAGS := -Wall -Wextra -ggdb3 -O2 -pipe
#CXXFLAGS := -Wall -Wextra -Wtype-limits -ggdb3 -O9 -march=native -mfpmath=sse -msse3 -ftree-vectorize -pipe
LDFLAGS := $(CXXFLAGS)

SUFFIXES := .o .cc .cpp

EXES := $(basename $(wildcard *.cpp))
OBJS := $(patsubst %.cc, %.o, $(wildcard *.cc))
DEPS := $(OBJS:.o=.P)

define cxx_compile_with_dependency_creation
  $(COMPILE.cc) -MD -o $@ $<
  @sed -e 's|.*:|$*.o:|' <$*.d >$*.P
  @sed -e 's/.*://' -e 's/\\$$//' <$*.d | fmt -1 | \
    sed -e 's/^ *//' -e 's/$$/:/' >>$*.P
  @rm -f $*.d
endef

define cxx_link_rule
  $(CXX) $(LDFLAGS) $^ -o $@
endef

%.o: %.cc
	$(call cxx_compile_with_dependency_creation)

%.o: %.cpp
	$(call cxx_compile_with_dependency_creation)

%: %.o
	$(call cxx_link_rule)

.PHONY: all check

all: $(EXES)

test_output: test_output.o $(OBJS)

check: test_output
	./test_output

clean:
	- $(RM) $(OBJS) $(addsuffix .o, $(EXES)) $(EXES) *.P

-include $(DEPS)
