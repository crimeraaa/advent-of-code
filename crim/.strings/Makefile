SRC := allocator.cpp
EXE := $(SRC:%.cpp=%)
BIN := $(SRC:%.cpp=bin/%)
INCLUDE := $(wildcard *.hpp *.tcc)
DEBUGEXTRA := -static-libgcc -static-libstdc++ -ggdb 

all: $(BIN)

# Order-only prerequisites aren't included in the argument list,
# But are checked if we need to rebuild this target.
bin/%: %.cpp $(INCLUDE)
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(BIN)

.PHONY: all clean
