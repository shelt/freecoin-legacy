#   $< means first prereq
#   $@ is file being generated

CC=gcc
CFLAGS=-I$(I_DIR) -std=c99

I_DIR = include
L_DIR = lib
O_DIR = build
B_DIR = bin
S_DIR = src



_HEADERS = transactions.h shared.h
_OBJECTS = transactions.o testing.o
default: all



# add paths
HEADERS = $(patsubst %,$(I_DIR)/%,$(_HEADERS))
OBJECTS = $(patsubst %,$(O_DIR)/%,$(_OBJECTS))

# generate object files
$(O_DIR)/%.o: $(S_DIR)/%.c $(HEADERS)
	$(CC) -c $< -o $@ $(CFLAGS)

all: $(OBJECTS)
	$(CC) $(OBJECTS) -o $(B_DIR)/$@ $(CFLAGS)

clean:
	-rm -f $(OBJECTS)
	-rm -r $(B_DIR)