# -*- SOURCES 
SRC := $(wildcard *.cpp)
INCLUDE := $(wildcard *.hpp *.tcc)

# -*- BUILD DIRECTORIES 
BIN := ./bin
OBJ := ./obj
LIB := ./lib

# -*- BUILD TARGETS 
EXE := $(SRC:%.cpp=$(BIN)/%)

# -*- BEGIN RECIPES 
all: $(EXE)

# $@ only expands to the left-hand item that caused this recipe to be called.	
$(BIN) $(OBJ) $(LIB):
	$(MKDIR) $@

# |, the pipe operator, means "(re)build this target if needed"
$(BIN)/%: %.cpp $(INCLUDE) | $(BIN)
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(EXE)

# -*- END RECIPES 
.PHONY: all clean
