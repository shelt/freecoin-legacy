ROOT_DIR:=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))

I_DIR = include
L_DIR = lib
O_DIR = build
B_DIR = bin
S_DIR = src

BIN_PREFIX=freecoin-
CC=gcc
CFLAGS=-Wall -std=c99 -lpthread -I$(I_DIR) -Wno-unused-variable

BDB_FLAGS=--includedir=$(ROOT_DIR)/$(I_DIR) --libdir=$(ROOT_DIR)/$(O_DIR)

$(info $(BDB_FLAGS))

$(exit)


default: all
all: init mine testing

# See "Secondary Expansion"
.SECONDEXPANSION:

# Build targets
TARGETS = mine testing

# Prerequisites for build targets
#mine_PREREQS = targets/mine.o crypto.o transactions.o blocks.o printing.o util.o blockchain.o network.o
#testing_PREREQS = targets/testing$(mine_PREREQS)

# Generate object file
$(O_DIR)/%.o: $(S_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Main target
$(TARGETS): $(addprefix $(O_DIR)/, $(addsuffix .o, $(basename $(shell cd $(S_DIR) && find $@ -name '*.c'))))
	$(CC) $^ -o $(B_DIR)/$(BIN_PREFIX)$@ $(CFLAGS)

bdb:
	-cd $(L_DIR)/bdb/build_unix && ../dist/configure $(BDB_FLAGS) && make && sudo make install

.PHONY: clean
init:
	-mkdir -p $(O_DIR) $(B_DIR)
	-cd $(S_DIR) && find -type d -exec mkdir -p "../$(O_DIR)/{}" \;
clean:
	-rm -rf ./$(O_DIR)/*
	-rm -rf ./$(B_DIR)/*
