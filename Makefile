default: freecoin
I_DIR = include
L_DIR = lib
O_DIR = build
B_DIR = bin
S_DIR = src

CC=gcc
CFLAGS=-I$(I_DIR)

# Generate object files list
sfiles  = $(wildcard $(S_DIR)/*.c)
_ofiles = $(sfiles:.c=.o)                          # change %.c to %.o
ofiles  = $(patsubst $(S_DIR)/%,$(O_DIR)/%,$(_ofiles))

# Generate header files list
hfiles  = $(wildcard $(I_DIR)/*.h)

#-include $(hfiles)

# Generate object file
$(O_DIR)/%.o: $(S_DIR)/%.c
	$(CC) -c $< -o $@ $(CFLAGS)

freecoin: $(ofiles)
	$(CC) $^ -o $(B_DIR)/$@ $(CFLAGS)

.PHONY: clean

clean:
	-rm -f $(O_DIR)/*
	-rm -f $(B_DIR)/*
