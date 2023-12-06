EXE = substring
SRC = $(addsuffix .c, $(EXE))

CFLAGS := -fdiagnostics-color=always -g $(CFLAGS)

all: $(EXE)

$(EXE): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

clean:
	$(RM) $(EXE)

.PHONY: all clean
