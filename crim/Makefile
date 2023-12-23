EXE = hello
SRC = $(addsuffix .cpp, $(EXE))
HPP = $(wildcard *.hpp)
CXXFLAGS += $(C_CXX_DEBUGFLAGS)

all: bin/$(EXE)

bin/$(EXE): $(SRC) $(HPP)
	$(CXX) $(CXXFLAGS) -o $@ $< 

clean:
	$(RM) bin/$(EXE)
