#   $< means first prereq
#   $@ is file being generated

CC=gcc
CFLAGS=-I$(I_DIR) -std=c99

I_DIR = include
L_DIR = lib
O_DIR = build
B_DIR = bin
S_DIR = src



_HEADERS = shared.h crypto.h transactions.h 
_OBJECTS = crypto.o transactions.o testing.o 
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
	-rm -f $(B_DIR)/*.exe