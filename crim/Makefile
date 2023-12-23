EXE = hello
SRC = $(addsuffix .cpp, $(EXE))
HPP = $(wildcard *.hpp)
CXXFLAGS += $(C_CXX_DEBUGFLAGS)

all: $(EXE)

$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SRC): $(HPP)

clean:
	$(RM) $(EXE)
