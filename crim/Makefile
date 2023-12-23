EXE = hello
SRC = $(addsuffix .cpp, $(EXE))
HPP = $(wildcard *.hpp)
CXXFLAGS += $(C_CXX_DEBUGFLAGS)

all: $(EXE)

$(EXE): $(SRC) $(HPP)
	$(CXX) $(CXXFLAGS) -o $@ $< 

clean:
	$(RM) $(EXE)
