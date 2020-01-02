# Define a default compiler. This can be overridden via (say) 'make CXX=clang++'
CXX = g++
# Default flags are for users, not developers. Your code is not for you!
# -O3 turns on optimizations.
# -g places symbols in the binary.
#    It helps the users debug their code, and binary sizes aren't an issue these days.
# -march=native: Tells the compiler to generate the most sophisticated instructions for the CPU.
#    It is a good default for open-source software,
#    but maybe not for closed source, where binaries are delivered to clients.
# -Wfatal-errors: Stop compilation on first error.
#    Since C++ is well-nigh impossible to lex and parse even in the absence of syntax errors,
#    defaulting to compiling past errors is, in my opinion, insane.
#    But a default it is: If you forget this flag, prepare to be horrified by compiler messages.
# -ffast-math: Sadly, often necessary to get SIMD vectorization, but dangerous.
# -fno-finite-math-only: Disable bizarre optimizations that assume nans and infs never occur, which are enabled by -ffast-math
# -MMD: Emit header dependencies, but don't include system headers.
CXXFLAGS = -g -Wall --std=gnu++17 -ffast-math -fno-finite-math-only -march=native -Wfatal-errors -MMD -fno-omit-frame-pointer
LINKFLAGS = -pthread -lcpprest -lcrypto -lssl -L/usr/local/lib
INCFLAGS = -I../boost 

# Developers should compile with 'make DEBUG=1'
ifdef DEBUG
	# Compiling with sanitizers during development is simply essential:
	CXXFLAGS += -O2 -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer
else
	CXXFLAGS += -O3
endif

# Find all .cpp files:
SRCS := $(wildcard *.cpp)
# For each .cpp, create a foo.x for foo.cpp:
EXECS := $(patsubst %.cpp,%.x,$(SRCS))

# A simple example showing how to detect a dependency:
HAVE_CXX := $(shell which $(CXX))
ifndef HAVE_CXX
# How to give an appropriate error message based on OS:
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
$(error $(CXX) is missing, do you have a compiler installed? Try 'sudo apt install -y g++' if not.)
endif
ifeq ($(UNAME_S),Darwin)
$(error $(CXX) is missing, do you have a compiler installed? Try 'brew install gcc' if not.)
endif
endif

all: $(EXECS)

#This builds all .cpp files into separate executables:
%.x: %.cpp
	$(CXX) $(CXXFLAGS) $(INCFLAGS) -o $@ $< $(LINKFLAGS)

-include $(SRCS:.cpp=.d)

.PHONY: clean
clean:
	rm -f *.x *.d perf.data perf.data.old test/*.x test/*.d example/*.x example/*.d

