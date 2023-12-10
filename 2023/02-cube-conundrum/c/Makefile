EXE = part1 part2
# Found in ~/.config/crimeraaa-dotfiles/config.gcc.mk
CFLAGS += $(C_CXX_DEBUGFLAGS)

# Surround in double quotes to allow tabs/whitespaces to reflect in output
# Use \$$(var) to output the one $ literally and not treat as command
# See: https://superuser.com/a/1154203
all: 
	@echo Please enter one of:
	@echo "$(EXE)"
	@echo ""
	@echo This Makefile uses implicit rules, such that typing:
	@echo "make part1"
	@echo ""
	@echo Will run:
	@echo "\$$(CC) \$$(CFLAGS) -o \$$@ \$$<"
	@echo ""
	@echo Which expands to:
	@echo "$(CC)"
	@echo "$(CFLAGS)"
	@echo "-o part1 part1.c"

clean:
	$(RM) $(EXE)
	
.PHONY: all clean
