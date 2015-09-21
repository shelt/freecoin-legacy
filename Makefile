I_DIR = include
L_DIR = lib
O_DIR = build
B_DIR = bin
S_DIR = src

BIN_PREFIX=freecoin-
CC=gcc
CFLAGS=-I$(I_DIR)


default: all
all: clean init mine testing

# See "Secondary Expansion"
.SECONDEXPANSION:

# Build targets
TARGETS = mine testing

# Prerequisites for build targets
mine_PREREQS = mine.o crypto.o transactions.o
testing_PREREQS = testing.o crypto.o transactions.o



# Generate object file
$(O_DIR)/%.o: $(S_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Main target
$(TARGETS): $$(addprefix $(O_DIR)/, $$($$@_PREREQS)) 
	$(CC) $^ -o $(B_DIR)/$(BIN_PREIFX)$@ $(CFLAGS)


.PHONY: clean
init:
	-mkdir $(O_DIR) $(B_DIR)
clean:
	-rm -f $(O_DIR)/*
	-rm -f $(B_DIR)/*
