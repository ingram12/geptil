CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -g -O0
LDFLAGS = -framework Cocoa -framework QuartzCore -L${VULKAN_SDK}/lib -lvulkan -Wl,-rpath,${VULKAN_SDK}/lib -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -Wl,-rpath,/opt/homebrew/Cellar/glfw/3.4/lib
INCLUDES = -Iinclude -I$(VULKAN_SDK)/include -I/opt/homebrew/Cellar/glfw/3.4/include

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
