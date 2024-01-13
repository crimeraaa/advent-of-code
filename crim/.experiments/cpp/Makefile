EXE := bin/test
SRC := $(EXE:bin/%=%.cpp)
INCLUDE := $(wildcard *.hpp)
DEPENDS := $(SRC) $(INCLUDE)

all: $(EXE)

$(EXE): $(DEPENDS)
	@echo Target: $(EXE)
	@echo Dependencies: $(DEPENDS)
	$(CXX) $(DEBUGFLAGS) $(CXXFLAGS) -o $@ $(filter-out %.hpp %.h, $(DEPENDS))

clean:
	$(RM) $(EXE)

.PHONY: all clean
