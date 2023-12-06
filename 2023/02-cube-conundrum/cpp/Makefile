EXE = part1
SRC = $(addsuffix .cpp, $(EXE))

all: $(EXE)

$(EXE): $(SRC)
	$(CXX)  -fdiagnostics-color=always -g $(CXXFLAGS) -o $@ $<

clean:
	$(RM) $(EXE)

.PHONY: all clean
