#pragma once

#include "../vulkan_context.h"

VkResult load_shader_code(const char* filename, uint32_t** code, size_t* codeSize);
