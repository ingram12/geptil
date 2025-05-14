CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
LDFLAGS = 
INCLUDES = -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)
BIN = build/ui-lib

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@rm -rf build
	@find src -name '*.o' -delete
