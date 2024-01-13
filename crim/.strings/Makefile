EXE := optimize
BIN := bin/$(EXE)
SRC := $(addsuffix .cpp, $(EXE))
INCLUDE := $(addsuffix .hpp, $(EXE))
DEBUGEXTRA := -static-libgcc -static-libstdc++ -ggdb 

all: $(BIN)

bin/%: %.cpp %.hpp
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(BIN)

.PHONY: all clean
