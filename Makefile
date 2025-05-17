CC = clang
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -g -O0
LDFLAGS = -framework Cocoa -framework QuartzCore -L${VULKAN_SDK}/lib -lvulkan -Wl,-rpath,${VULKAN_SDK}/lib -L/opt/homebrew/Cellar/glfw/3.4/lib -lglfw -Wl,-rpath,/opt/homebrew/Cellar/glfw/3.4/lib
INCLUDES = -Iinclude -I$(VULKAN_SDK)/include -I/opt/homebrew/Cellar/glfw/3.4/include

SRC = $(shell find src -name '*.c')
OBJ = $(SRC:.c=.o)
BIN = build/geptil

# 🔧 PCH
PCH = build/vulkan_context.h.pch
PCH_SRC = src/include/vulkan/vulkan_context.h

# 📜 Shaders
SHADER_DIR = src/include/vulkan/shader
SHADER_SRC = $(wildcard $(SHADER_DIR)/*.vert) $(wildcard $(SHADER_DIR)/*.frag)
SHADER_SPV = $(SHADER_SRC:%=%.spv)

.PHONY: all clean shaders

all: shaders $(BIN)

shaders: $(SHADER_SPV)

%.spv: %
	glslc $< -o $@

# 🔨 Линкуем бинарник
$(BIN): $(OBJ)
	@mkdir -p build
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

# 🔄 Компиляция с PCH
%.o: %.c $(PCH)
	$(CC) $(CFLAGS) $(INCLUDES) -include-pch $(PCH) -c $< -o $@

# 🧠 Сборка PCH
$(PCH): $(PCH_SRC)
	@mkdir -p build
	$(CC) $(CFLAGS) $(INCLUDES) -x c-header $< -o $@

# 🧹 Очистка
clean:
	@echo "Cleaning build artifacts..."
	@find src -name '*.o' -delete
	@rm -f $(BIN)
	@rm -f $(PCH)
	@find . -type f \( -name '*.log' -o -name '*.tmp' -o -name 'core' \) -delete
	@find . -name '*.dSYM' -exec rm -rf {} +
	@echo "Clean complete."
