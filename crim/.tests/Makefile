EXE = initlist
SRC = $(addsuffix .cpp, $(EXE))
HPP = $(wildcard *.hpp)
CXXFLAGS += $(C_CXX_DEBUGFLAGS)

all: bin/$(EXE)

bin/$(EXE): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $< 

$(SRC): $(HPP)

clean:
	$(RM) bin/$(EXE)
