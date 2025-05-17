#include "../../logger/logger.h"
#include "../vulkan_context.h"
#include <stdio.h>
#include <stdlib.h>

VkResult load_shader_code(const char* filename, uint32_t** code, size_t* codeSize) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        LOG_ERROR("Failed to open shader file: %s", filename);
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    fseek(file, 0, SEEK_END);
    *codeSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    *code = malloc(*codeSize);
    if (!*code) {
        LOG_ERROR("Failed to allocate memory for shader code");
        fclose(file);
        return VK_ERROR_OUT_OF_HOST_MEMORY;
    }

    size_t readSize = fread(*code, 1, *codeSize, file);
    fclose(file);

    if (readSize != *codeSize) {
        LOG_ERROR("Failed to read shader file");
        free(*code);
        *code = NULL;
        return VK_ERROR_INITIALIZATION_FAILED;
    }

    return VK_SUCCESS;
}
