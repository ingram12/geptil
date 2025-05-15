CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -O2
LDFLAGS = 
INCLUDES = -Iinclude

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)
BIN = build/geptil

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	@echo "Cleaning build artifacts..."
	@find src -name '*.o' -delete
	@find . -type f \( -name '*.log' -o -name '*.tmp' -o -name 'core' \) -delete
	@find . -name '*.dSYM' -exec rm -rf {} +
	@echo "Clean complete."
