# Yank all source files in the current directory
# SRC = $(wildcard *.cpp)
SRC = std_readfile.cpp crim_readfile.cpp

# Create a list of binaries, each from its respective source file
EXE = $(SRC:%.cpp=bin/%)

# Yank all the crim namespace headers which are in the previous directory
HPP = $(wildcard ../*.hpp ../*.tcc)

# See ~/devconfig/config.gcc.mk
CXXFLAGS += $(DEBUGFLAGS)

all: $(EXE)

bin/%: %.cpp $(HPP)
	$(CXX) $(CXXFLAGS) -o $@ $< 

clean:
	$(RM) $(EXE)
